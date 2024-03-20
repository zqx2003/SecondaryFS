#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <future>
#include "../inc/BlockDevice.h"
#include "../inc/BufferManager.h"
#include "../inc/DiskDriver.h"
#include "../inc/FileManager.h"
#include "../inc/FileSystem.h"
#include "../inc/INode.h"
#include "../inc/Kernel.h"

std::fstream* DiskDriver::disk = nullptr;

void DiskDriver::DiskFormat(const char* disk_file_path, int isize, int fsize)
{
	/* 构造SuperBlock */
	SuperBlock spb;
	memset(&spb, 0, sizeof(spb));
	spb.s_isize = (isize + FileSystem::INODE_NUMBER_PER_SECTOR - 1) / FileSystem::INODE_NUMBER_PER_SECTOR * FileSystem::INODE_NUMBER_PER_SECTOR;
	spb.s_fsize = fsize;
	
	int block_base = spb.s_isize / FileSystem::INODE_NUMBER_PER_SECTOR + 2;	// 文件区起始块号
	spb.s_nfree = (spb.s_fsize - block_base - 14) % 100;	// 5个初始目录各1个块，1个shell文件占9个块
	for (int i = 0; i < spb.s_nfree; i++) {
		spb.s_free[i] = block_base + (spb.s_fsize - block_base - 14) / 100 * 100 + i;
	}
	spb.s_flock = 0;

	spb.s_ninode = std::min(isize - 7, SuperBlock::NFREE);
	for (int i = 0; i < spb.s_ninode; i++) {
		spb.s_inode[i] = i + 8;	// 5个初始目录，1个shell文件，1个字符设备文件，以及0#Inode
	}
	spb.ilock = 0;

	spb.s_fmod = 0;
	spb.s_ronly = 0;
	spb.s_time = 0;

	/* 构建Inode区 */
	DiskInode initInode[8];	// Inode::BLOCK_SIZE字节
	memset(initInode, 0, sizeof(initInode));

	/* 根目录 */
	initInode[1].d_mode = Inode::IFDIR;
	initInode[1].d_uid = 0;
	initInode[1].d_size = 7 * sizeof(DirectoryEntry);
	initInode[1].d_addr[0] = fsize - 14;

	/* bin目录 */
	initInode[2].d_mode = Inode::IFDIR;
	initInode[2].d_uid = 0;
	initInode[2].d_size = 2 * sizeof(DirectoryEntry);
	initInode[2].d_addr[0] = fsize - 13;

	/* etc目录 */
	initInode[3].d_mode = Inode::IFDIR;
	initInode[3].d_uid = 0;
	initInode[3].d_size = 2 * sizeof(DirectoryEntry);
	initInode[3].d_addr[0] = fsize - 12;

	/* dev目录 */
	initInode[4].d_mode = Inode::IFDIR;
	initInode[4].d_uid = 0;
	initInode[4].d_size = 3 * sizeof(DirectoryEntry);
	initInode[4].d_addr[0] = fsize - 11;

	/* home目录 */
	initInode[5].d_mode = Inode::IFDIR;
	initInode[5].d_uid = 0;
	initInode[5].d_size = 2 * sizeof(DirectoryEntry);
	initInode[5].d_addr[0] = fsize - 10;

	/* shell文件 */
	initInode[6].d_mode = 0;
	initInode[6].d_uid = 0;
	initInode[6].d_size = 8 * Inode::BLOCK_SIZE;
	initInode[6].d_addr[0] = fsize - 9;
	initInode[6].d_addr[1] = fsize - 8;
	initInode[6].d_addr[2] = fsize - 7;
	initInode[6].d_addr[3] = fsize - 6;
	initInode[6].d_addr[4] = fsize - 5;
	initInode[6].d_addr[5] = fsize - 4;
	initInode[6].d_addr[6] = fsize - 3;

	/* tty1文件 */
	initInode[7].d_mode = Inode::IFCHR;
	initInode[7].d_uid = 0;
	initInode[7].d_size = 0;

	/* 构建文件区 */
	struct DirEntry {
		int m_ino;
		char m_name[DirectoryEntry::DIRSIZ];
	};

	/* 根目录 */
	DirEntry rootDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{1, "."},
		{1, ".."},
		{2, "bin"},
		{3, "etc"},
		{4, "dev"},
		{5, "home"},
		{6, "shell"}
	};

	/* bin目录 */
	DirEntry binDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{2, "."},
		{1, ".."}
	};

	/* etc目录 */
	DirEntry etcDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{3, "."},
		{1, ".."}
	};

	/* dev目录 */
	DirEntry devDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{4, "."},
		{1, ".."},
		{7, "tty1"}
	};

	/* home目录 */
	DirEntry homeDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{5, "."},
		{1, ".."}
	};

	/* shell文件 */
	char shellFile[9][Inode::BLOCK_SIZE] = { '\0' };
	*reinterpret_cast<int*>(shellFile[6]) = fsize - 2;
	*(reinterpret_cast<int*>(shellFile[6]) + 1) = fsize - 1;

	/* 格式化磁盘 */
	std::ofstream fout(disk_file_path, std::ios::out | std::ios::binary);

	/* 格式化SuperBlock */
	fout.write(reinterpret_cast<const char*>(&spb), sizeof(spb));

	/* 格式化Inode区 */
	fout.write(reinterpret_cast<const char*>(initInode), Inode::BLOCK_SIZE);
	for (int i = 1; i < isize / FileSystem::INODE_NUMBER_PER_SECTOR; i++) {	// 之后均为空Inode
		char empty_inode[Inode::BLOCK_SIZE] = { '\0' };
		fout.write(empty_inode, Inode::BLOCK_SIZE);
	}

	/* 格式化文件区 */
	for (int i = block_base; i < spb.s_fsize - 14; i++) {
		char empty_block[Inode::BLOCK_SIZE] = { '\0' };
		if (i != block_base && (i - block_base) % 100 == 0) {
			int& nfree = *reinterpret_cast<int*>(empty_block);
			int(&free)[100] = *reinterpret_cast<int(*)[100]>(empty_block + sizeof(nfree));

			nfree = 100;
			for (int j = 0; j < 100; j++) {
				free[j] = i - 100 + j;
			}
		}

		fout.write(empty_block, Inode::BLOCK_SIZE);
	}
	fout.write(reinterpret_cast<const char*>(rootDir), Inode::BLOCK_SIZE);
	fout.write(reinterpret_cast<const char*>(binDir), Inode::BLOCK_SIZE);
	fout.write(reinterpret_cast<const char*>(etcDir), Inode::BLOCK_SIZE);
	fout.write(reinterpret_cast<const char*>(devDir), Inode::BLOCK_SIZE);
	fout.write(reinterpret_cast<const char*>(homeDir), Inode::BLOCK_SIZE);
	fout.write(reinterpret_cast<const char*>(shellFile), 9 * Inode::BLOCK_SIZE);
}

void DiskDriver::Open()
{
	if (nullptr != disk)
		return;

	disk = new std::fstream("../dev/SecondaryFS.img", std::ios::in | std::ios::out | std::ios::binary);
	if (!disk->is_open()) {
		std::cout << "Disk Open Error!" << std::endl;
		return;
	}
}

void DiskDriver::Close()
{
	if (nullptr != disk) {
		disk->close();
		delete disk;
		disk = nullptr;
	}
}

void DiskDriver::DiskHandler()
{
	Buf* bp;
	Devtab* dtab;
	short major = 0;

	DiskBlockDevice& bdev = 
		dynamic_cast<DiskBlockDevice&>(Kernel::Instance().GetDeviceManager().GetBlockDevice(major));


}

void DiskDriver::DevStart(Buf* bp)
{
	if (nullptr == bp) {
		std::cout << "Invalid Buf in DevStart()" << std::endl;
		return;
	}

	/* 计算磁盘物理块的起始地址 */
	const int BUFSIZE = BufferManager::BUFFER_SIZE;
	int disk_addr = bp->b_blkno * BUFSIZE;

	if ((bp->b_flags & Buf::B_READ) == Buf::B_READ) {	/* 读操作 */
		disk->seekg(disk_addr, std::ios::beg);

		/* 异步方式读磁盘 */
		auto future = std::async(
			std::launch::async,
			[bp, BUFSIZE](std::function<void(void)> callback) {			/* 读磁盘 */
			disk->read(reinterpret_cast<char*>(bp->b_addr), BUFSIZE);
			callback();
		},
			[]() {														/* 使用回调函数模拟磁盘中断 */
			DiskHandler();
		});
	}
	else {	/* 写操作 */
		disk->seekp(disk_addr, std::ios::beg);

		/* 异步方式写磁盘 */
		auto future = std::async(
			std::launch::async,
			[bp, BUFSIZE](std::function<void(void)> callback) {			/* 写磁盘 */
			disk->write(reinterpret_cast<const char*>(bp->b_addr), BUFSIZE);
			callback();
		},
			[]() {														/* 使用回调函数模拟磁盘中断 */
			DiskHandler();
		});		
	}
}

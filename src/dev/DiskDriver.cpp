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
#include "../inc/Utility.h"

std::fstream* DiskDriver::disk = nullptr;
std::mutex DiskDriver::mtx_rw;

std::queue<std::future<void>> DiskDriver::tasks;
std::mutex DiskDriver::mtx_queue;

void DiskDriver::DiskFormat(const char* disk_file_path, int isize, int fsize)
{
	/* ����SuperBlock */
	SuperBlock spb;
	memset(&spb, 0, sizeof(spb));
	spb.s_isize = (isize + FileSystem::INODE_NUMBER_PER_SECTOR - 1) / FileSystem::INODE_NUMBER_PER_SECTOR * FileSystem::INODE_NUMBER_PER_SECTOR;
	spb.s_fsize = fsize;
	
	int block_base = spb.s_isize / FileSystem::INODE_NUMBER_PER_SECTOR + 2;	// �ļ�����ʼ���
	spb.s_nfree = (spb.s_fsize - block_base - 14) % 100;	// 5����ʼĿ¼��1���飬1��shell�ļ�ռ9����
	for (int i = 0; i < spb.s_nfree; i++) {
		spb.s_free[i] = block_base + (spb.s_fsize - block_base - 14) / 100 * 100 + i;
	}
	spb.s_flock = 0;

	spb.s_ninode = std::min(isize - 7, SuperBlock::NFREE);
	for (int i = 0; i < spb.s_ninode; i++) {
		spb.s_inode[i] = i + 8;	// 5����ʼĿ¼��1��shell�ļ���1���ַ��豸�ļ����Լ�0#Inode
	}
	spb.s_ilock = 0;

	spb.s_fmod = 0;
	spb.s_ronly = 0;
	spb.s_time = 0;

	/* ����Inode�� */
	DiskInode initInode[8];	// Inode::BLOCK_SIZE�ֽ�
	memset(initInode, 0, sizeof(initInode));

	/* ��Ŀ¼ */
	initInode[1].d_mode = Inode::IFDIR;
	initInode[1].d_uid = 0;
	initInode[1].d_size = 7 * sizeof(DirectoryEntry);
	initInode[1].d_addr[0] = fsize - 14;

	/* binĿ¼ */
	initInode[2].d_mode = Inode::IFDIR;
	initInode[2].d_uid = 0;
	initInode[2].d_size = 2 * sizeof(DirectoryEntry);
	initInode[2].d_addr[0] = fsize - 13;

	/* etcĿ¼ */
	initInode[3].d_mode = Inode::IFDIR;
	initInode[3].d_uid = 0;
	initInode[3].d_size = 2 * sizeof(DirectoryEntry);
	initInode[3].d_addr[0] = fsize - 12;

	/* devĿ¼ */
	initInode[4].d_mode = Inode::IFDIR;
	initInode[4].d_uid = 0;
	initInode[4].d_size = 3 * sizeof(DirectoryEntry);
	initInode[4].d_addr[0] = fsize - 11;

	/* homeĿ¼ */
	initInode[5].d_mode = Inode::IFDIR;
	initInode[5].d_uid = 0;
	initInode[5].d_size = 2 * sizeof(DirectoryEntry);
	initInode[5].d_addr[0] = fsize - 10;

	/* shell�ļ� */
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

	/* tty1�ļ� */
	initInode[7].d_mode = Inode::IFCHR;
	initInode[7].d_uid = 0;
	initInode[7].d_size = 0;

	/* �����ļ��� */
	struct DirEntry {
		int m_ino;
		char m_name[DirectoryEntry::DIRSIZ];
	};

	/* ��Ŀ¼ */
	DirEntry rootDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{1, "."},
		{1, ".."},
		{2, "bin"},
		{3, "etc"},
		{4, "dev"},
		{5, "home"},
		{6, "shell"}
	};

	/* binĿ¼ */
	DirEntry binDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{2, "."},
		{1, ".."}
	};

	/* etcĿ¼ */
	DirEntry etcDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{3, "."},
		{1, ".."}
	};

	/* devĿ¼ */
	DirEntry devDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{4, "."},
		{1, ".."},
		{7, "tty1"}
	};

	/* homeĿ¼ */
	DirEntry homeDir[Inode::BLOCK_SIZE / sizeof(DirEntry)] = {
		{5, "."},
		{1, ".."}
	};

	/* shell�ļ� */
	char shellFile[9][Inode::BLOCK_SIZE] = { '\0' };
	*reinterpret_cast<int*>(shellFile[6]) = fsize - 2;
	*(reinterpret_cast<int*>(shellFile[6]) + 1) = fsize - 1;

	/* ��ʽ������ */
	std::ofstream fout(disk_file_path, std::ios::out | std::ios::binary);

	/* ��ʽ��SuperBlock */
	fout.write(reinterpret_cast<const char*>(&spb), sizeof(spb));

	/* ��ʽ��Inode�� */
	fout.write(reinterpret_cast<const char*>(initInode), Inode::BLOCK_SIZE);
	for (int i = 1; i < isize / FileSystem::INODE_NUMBER_PER_SECTOR; i++) {	// ֮���Ϊ��Inode
		char empty_inode[Inode::BLOCK_SIZE] = { '\0' };
		fout.write(empty_inode, Inode::BLOCK_SIZE);
	}

	/* ��ʽ���ļ��� */
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
	short major = Utility::GetMajor(DeviceManager::ROOTDEV);

	DiskBlockDevice& bdev = 
		dynamic_cast<DiskBlockDevice&>(Kernel::Instance().GetDeviceManager().GetBlockDevice(major));
	dtab = bdev.d_tab;
	//std::cout << "�����ж�" << std::endl;
	{
		std::lock_guard<std::recursive_mutex> lock(BufferManager::mtx_av);
		//std::cout << "��������ж�" << std::endl;
		if (dtab->d_active == 0) {		/* û�������� */
			return;
		}
		bp = dtab->d_actf;				/* ��ȡ�����ж϶�Ӧ��I/O����Buf */
		dtab->d_active = 0;				/* ��ʾ�豸�ѿ��� */

		dtab->d_actf = bp->av_forw;		/* ��I/O������ȡ������ɵ�I/O����Buf */
		Kernel::Instance().GetBufferManager().IODone(bp);	/* I/O�����ƺ��� */
		bdev.Start();					/* ����I/O�����������һ��I/O���� */
	}
}

void DiskDriver::DevStart(Buf* bp)
{
	if (nullptr == bp) {
		std::cout << "Invalid Buf in DevStart()" << std::endl;
		return;
	}
	//std::cout << "��ȡ����" << std::endl;

	/* �������ɵĴ����̣߳������̹߳���ռ���ڴ� */
	TaskClear();

	/* ���������������ʼ��ַ */
	if ((bp->b_flags & Buf::B_READ) == Buf::B_READ) {	/* ������ */
		/* �첽��ʽ������ */
		std::lock_guard<std::mutex> lock_queue(mtx_queue);

		tasks.emplace(												/* ���첽��д������ӵ���������У���Ϊstd::future�����뿪������ǰǿ�ƽ��첽����ִ���� */
			std::async(
				std::launch::async,
				[bp](std::function<void(void)> callback) {			/* ������ */
			int disk_addr = bp->b_blkno * BufferManager::BUFFER_SIZE;

			{
				std::lock_guard<std::mutex> lock(mtx_rw);				/* ������ʹ���ļ������ļ����뻥�� */
				disk->seekg(disk_addr, std::ios::beg);
				disk->read(reinterpret_cast<char*>(bp->b_addr), BufferManager::BUFFER_SIZE);
			}

			callback();
		},
				[]() {												/* ʹ�ûص�����ģ������ж� */
			DiskHandler();
		}));
	}
	else {	/* д���� */
		/* �첽��ʽд���� */
		std::lock_guard<std::mutex> lock_queue(mtx_queue);

		tasks.emplace(
			std::async(
				std::launch::async,
				[bp](std::function<void(void)> callback) {			/* д���� */
			int disk_addr = bp->b_blkno * BufferManager::BUFFER_SIZE;

			{
				std::lock_guard<std::mutex> lock(mtx_rw);				/* ������ʹ���ļ���д�ļ� */
				disk->seekp(disk_addr, std::ios::beg);
				disk->write(reinterpret_cast<const char*>(bp->b_addr), BufferManager::BUFFER_SIZE);
			}

			callback();
		},
				[]() {														/* ʹ�ûص�����ģ������ж� */
			DiskHandler();
		})
		);	
	}
}

void DiskDriver::TaskClear()
{
	{
		std::lock_guard<std::mutex> lock_queue(mtx_queue);

		for (;;) {
			/* ����Ϊ�ջ��߶��׵�����δ��ɣ�ֱ�ӷ��� */
			if (tasks.empty() || tasks.front().wait_for(std::chrono::seconds(0)) == std::future_status::timeout)
				return;

			tasks.pop();
		}
	}
}

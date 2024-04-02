#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <future>
#include <vector>
#include <stack>
#include <queue>
#include <random>
#include "test.h"
#include "../src/inc/BufferManager.h"
#include "../src/inc/DiskDriver.h"
#include "../src/inc/Kernel.h"

void showBlock(const char * buffer)
{
	BufferManager& bm = Kernel::Instance().GetBufferManager();

	const int B_PER_LINE = 16;
	std::cout << "读取测试：" << std::endl;
	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	for (int i = 0; i < bm.BUFFER_SIZE; i++) {
		int val = static_cast<unsigned char>(buffer[i]);
		std::cout << std::setfill('0') << std::setw(2) << std::hex << val << " ";
		if (i % B_PER_LINE == B_PER_LINE - 1) {
			std::cout << std::endl;
		}
	}
	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
}

void testDiskDriver()
{
	const int BUFSIZE = BufferManager::BUFFER_SIZE;

	Buf buf;
	char buffer1[BUFSIZE] = { 0 }, buffer2[BUFSIZE] = { 0 };
	std::memset(&buf, 0, sizeof(buf));
	
	/* 构造数据 */
	for (int i = 0; i < BUFSIZE; i++) {
		buffer2[i] = 'a';
	}

	/* 读测试 */
	buf.b_flags |= Buf::B_READ;
	buf.b_blkno = 0;
	buf.b_addr = buffer1;
	auto future = std::async(std::launch::async, [&buf]() {
		DiskDriver::DevStart(&buf);
	});
	future.wait();

	showBlock(buffer1);

	/* 写测试 */
	buf.b_flags |= Buf::B_READ;
	buf.b_blkno = 127;
	buf.b_addr = buffer1;
	std::async(std::launch::async, [&buf]() {	/* 备份 */
		DiskDriver::DevStart(&buf);
	}).wait();

	buf.b_flags &= ~Buf::B_READ;
	buf.b_blkno = 127;
	buf.b_addr = buffer2;
	std::async(std::launch::async, [&buf]() {	/* 写新数据 */
		DiskDriver::DevStart(&buf);
	}).wait();

	std::cout << "写入测试" << std::endl;
	std::cout << "请打开img文件查看内容，按回车继续" << std::endl;
	char ch = 0;
	while ((ch = getchar()) != '\n')
		;

	buf.b_flags &= ~Buf::B_READ;
	buf.b_blkno = 127;
	buf.b_addr = buffer1;
	std::async(std::launch::async, [&buf]() {	/* 恢复 */
		DiskDriver::DevStart(&buf);
	}).wait();
}

void testBuffer()
{
	short dev = DeviceManager::ROOTDEV;
	BufferManager& bm = Kernel::Instance().GetBufferManager();

	/* 测试缓存的申请和释放 */
	const int threadNum = 20;
	std::future<void> futures[threadNum];

	std::cout << "测试GetBlk()与Brelse()" << std::endl;
	
	/* 自由队列为空时 */
	if (1) {
		std::cout << "测试自由队列为空的情况" << std::endl;
		for (int blkno = 0; blkno < 20; blkno++) {
			futures[blkno] = std::async(std::launch::async, [&bm](int dev, int blkno) {
				std::ostringstream sout;
				sout << "<" << dev << ", " << blkno << "> -- 申请缓存块中。。。" << std::endl;
				std::cout << sout.str();

				Buf* bp = bm.GetBlk(dev, blkno);
				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- 申请缓存块成功" << std::endl;
				std::cout << sout.str();
				//std::cout << "缓存标志位: (B_BUSY: " << ((bp->b_flags & Buf::B_BUSY) ? "T" : "F") << ")" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(10));

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- 释放缓存块中。。。，缓存块地址: " << bm.GetBufId(bp) << std::endl;
				std::cout << sout.str();

				bm.Brelse(bp);

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- 释放缓存块成功" << std::endl;
				std::cout << sout.str();
			},
				dev,
				blkno);
		}

		/* 阻塞至测试完成，才能进行下一项测试 */
		for (int blkno = 0; blkno < threadNum; blkno++) {
			futures[blkno].wait();
		}
		std::cout << "测试自由队列为空的情况完成" << std::endl << std::endl;
	}
	
	if (1) {
		std::cout << "测试多线程锁定同一块缓存的情况" << std::endl;
		for (int blkno = 0; blkno < 20; blkno++) {
			futures[blkno] = std::async(std::launch::async, [&bm](int dev, int blkno) {
				std::ostringstream sout;
				sout << "<" << dev << ", " << blkno << "> -- 申请缓存块中。。。" << std::endl;
				std::cout << sout.str();

				Buf* bp = bm.GetBlk(dev, blkno);
				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- 申请缓存块成功" << std::endl;
				std::cout << sout.str();
				//std::cout << "缓存标志位: (B_BUSY: " << ((bp->b_flags & Buf::B_BUSY) ? "T" : "F") << ")" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(10));

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- 释放缓存块中。。。，缓存块地址: " << bm.GetBufId(bp) << std::endl;
				std::cout << sout.str();

				bm.Brelse(bp);

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- 释放缓存块成功" << std::endl;
				std::cout << sout.str();
			},
				dev,
				blkno % 10);
		}

		/* 阻塞至测试完成，才能进行下一项测试 */
		for (int blkno = 0; blkno < threadNum; blkno++) {
			futures[blkno].wait();
		}
		std::cout << "测试多线程锁定同一块缓存的情况完成" << std::endl << std::endl;
	}
	
	/* 测试同步读 */
	if (0) {
		std::cout << "测试同步读" << std::endl;

		short dev = Kernel::Instance().GetDeviceManager().ROOTDEV;
		std::cout << "同步读开始" << std::endl;
		Buf* bp = bm.Bread(dev, 0);
		std::cout << "同步读完成" << std::endl;

		showBlock(bp->b_addr);
		std::cout << "测试同步读完成" << std::endl << std::endl;
	}

	/* 测试预读 */
	if (0) {
		std::cout << "测试预读" << std::endl;
		
		short adev = Kernel::Instance().GetDeviceManager().ROOTDEV;
		std::cout << "预读开始" << std::endl;
		Buf* bp = bm.Breada(adev, 0, 1);
		std::cout << "预读完成" << std::endl;

		std::cout << "同步读块" << std::endl;
		showBlock(bp->b_addr);

		Buf* abp = bm.Bread(adev, 1);

		std::cout << "预读块" << std::endl;
		showBlock(abp->b_addr);
		std::cout << "测试预读完成" << std::endl << std::endl;
	}

	/* 测试异步写 */
	if (0) {
		std::cout << "测试异步写" << std::endl;
		short dev = Kernel::Instance().GetDeviceManager().ROOTDEV;

		Buf* bp = bm.GetBlk(dev, 127);
		/* 构造数据 */
		for (int i = 0; i < bm.BUFFER_SIZE; i++) {
			bp->b_addr[i] = 'b';
		}

		std::cout << "异步写开始" << std::endl;
		std::async(
			std::launch::async,
			[&bm, bp] {
			bm.Bawrite(bp);
		}).wait();
		std::cout << "异步写完成" << std::endl;
		
		showBlock(bp->b_addr);
		std::cout << "测试异步写完成" << std::endl << std::endl;
	}

	/* 测试延迟写 */
	if (0) {
		std::cout << "测试延迟写" << std::endl;
		std::cout << "测试延迟写完成" << std::endl << std::endl;
	}

	/* 测试清空缓冲区 */
	if (0) {
		std::cout << "测试清空缓冲区" << std::endl;
		std::cout << "测试清空缓冲区完成" << std::endl << std::endl;
	}

	/* 测试刷新缓冲区 */
	if (0) {
		std::cout << "测试刷新缓冲区" << std::endl;
		std::cout << "测试刷新缓冲区完成" << std::endl << std::endl;
	}
}

void testFileSystem()
{
	short dev = DeviceManager::ROOTDEV;
	FileSystem& fs = Kernel::Instance().GetFileSystem();
	BufferManager& bm = Kernel::Instance().GetBufferManager();

	/* 测试数据块的申请与释放 */
	if (1) {
		std::cout << "测试申请与释放数据块" << std::endl;

		//std::stack<int> blknos;
		//std::queue<int> blknos;
		std::vector<int> blknos;

		std::cout << "申请数据块开始" << std::endl;
		for (;;) {
			Buf* buf = fs.Alloc(dev);
			if (nullptr == buf) {
				break;
			}

			//blknos.push(buf->b_blkno);
			blknos.push_back(buf->b_blkno);
			if (blknos.size() != 0 && blknos.size() % 10000 == 0) {
				std::cout << "已申请数据块数量：" << blknos.size() << std::endl;
			}
			//std::cout << "申请数据块号：" << buf->b_blkno << std::endl;
			bm.Brelse(buf);

			if (blknos.size() > 65536) {
				std::cout << "Error" << std::endl;
				break;
			}
		}
		std::cout << "申请数据块完成" << std::endl;
		std::cout << "总申请数据块数量：" << blknos.size() << std::endl << std::endl;
		
		std::default_random_engine rng(std::random_device{}());
		std::shuffle(blknos.begin(), blknos.end(), rng);

		std::cout << "释放数据块开始" << std::endl;
		while (!blknos.empty()) {
			//fs.Free(dev, blknos.top());
			//fs.Free(dev, blknos.front());
			fs.Free(dev, blknos.back());
			//blknos.pop();
			blknos.pop_back();

			if (blknos.size() % 10000 == 0) {
				std::cout << "剩余数据块数量：" << blknos.size() << std::endl;
			}
		}
		std::cout << "释放数据块完成" << std::endl;
		std::cout << "测试申请与释放数据块完成" << std::endl << std::endl;
		fs.Update();
	}
}

void test()
{
	//testDiskDriver();
	//testBuffer();
	testFileSystem();
}
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
	std::cout << "��ȡ���ԣ�" << std::endl;
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
	
	/* �������� */
	for (int i = 0; i < BUFSIZE; i++) {
		buffer2[i] = 'a';
	}

	/* ������ */
	buf.b_flags |= Buf::B_READ;
	buf.b_blkno = 0;
	buf.b_addr = buffer1;
	auto future = std::async(std::launch::async, [&buf]() {
		DiskDriver::DevStart(&buf);
	});
	future.wait();

	showBlock(buffer1);

	/* д���� */
	buf.b_flags |= Buf::B_READ;
	buf.b_blkno = 127;
	buf.b_addr = buffer1;
	std::async(std::launch::async, [&buf]() {	/* ���� */
		DiskDriver::DevStart(&buf);
	}).wait();

	buf.b_flags &= ~Buf::B_READ;
	buf.b_blkno = 127;
	buf.b_addr = buffer2;
	std::async(std::launch::async, [&buf]() {	/* д������ */
		DiskDriver::DevStart(&buf);
	}).wait();

	std::cout << "д�����" << std::endl;
	std::cout << "���img�ļ��鿴���ݣ����س�����" << std::endl;
	char ch = 0;
	while ((ch = getchar()) != '\n')
		;

	buf.b_flags &= ~Buf::B_READ;
	buf.b_blkno = 127;
	buf.b_addr = buffer1;
	std::async(std::launch::async, [&buf]() {	/* �ָ� */
		DiskDriver::DevStart(&buf);
	}).wait();
}

void testBuffer()
{
	short dev = DeviceManager::ROOTDEV;
	BufferManager& bm = Kernel::Instance().GetBufferManager();

	/* ���Ի����������ͷ� */
	const int threadNum = 20;
	std::future<void> futures[threadNum];

	std::cout << "����GetBlk()��Brelse()" << std::endl;
	
	/* ���ɶ���Ϊ��ʱ */
	if (1) {
		std::cout << "�������ɶ���Ϊ�յ����" << std::endl;
		for (int blkno = 0; blkno < 20; blkno++) {
			futures[blkno] = std::async(std::launch::async, [&bm](int dev, int blkno) {
				std::ostringstream sout;
				sout << "<" << dev << ", " << blkno << "> -- ���뻺����С�����" << std::endl;
				std::cout << sout.str();

				Buf* bp = bm.GetBlk(dev, blkno);
				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- ���뻺���ɹ�" << std::endl;
				std::cout << sout.str();
				//std::cout << "�����־λ: (B_BUSY: " << ((bp->b_flags & Buf::B_BUSY) ? "T" : "F") << ")" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(10));

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- �ͷŻ�����С�������������ַ: " << bm.GetBufId(bp) << std::endl;
				std::cout << sout.str();

				bm.Brelse(bp);

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- �ͷŻ����ɹ�" << std::endl;
				std::cout << sout.str();
			},
				dev,
				blkno);
		}

		/* ������������ɣ����ܽ�����һ����� */
		for (int blkno = 0; blkno < threadNum; blkno++) {
			futures[blkno].wait();
		}
		std::cout << "�������ɶ���Ϊ�յ�������" << std::endl << std::endl;
	}
	
	if (1) {
		std::cout << "���Զ��߳�����ͬһ�黺������" << std::endl;
		for (int blkno = 0; blkno < 20; blkno++) {
			futures[blkno] = std::async(std::launch::async, [&bm](int dev, int blkno) {
				std::ostringstream sout;
				sout << "<" << dev << ", " << blkno << "> -- ���뻺����С�����" << std::endl;
				std::cout << sout.str();

				Buf* bp = bm.GetBlk(dev, blkno);
				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- ���뻺���ɹ�" << std::endl;
				std::cout << sout.str();
				//std::cout << "�����־λ: (B_BUSY: " << ((bp->b_flags & Buf::B_BUSY) ? "T" : "F") << ")" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(10));

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- �ͷŻ�����С�������������ַ: " << bm.GetBufId(bp) << std::endl;
				std::cout << sout.str();

				bm.Brelse(bp);

				sout.str("");
				sout << "<" << dev << ", " << blkno << "> -- �ͷŻ����ɹ�" << std::endl;
				std::cout << sout.str();
			},
				dev,
				blkno % 10);
		}

		/* ������������ɣ����ܽ�����һ����� */
		for (int blkno = 0; blkno < threadNum; blkno++) {
			futures[blkno].wait();
		}
		std::cout << "���Զ��߳�����ͬһ�黺���������" << std::endl << std::endl;
	}
	
	/* ����ͬ���� */
	if (0) {
		std::cout << "����ͬ����" << std::endl;

		short dev = Kernel::Instance().GetDeviceManager().ROOTDEV;
		std::cout << "ͬ������ʼ" << std::endl;
		Buf* bp = bm.Bread(dev, 0);
		std::cout << "ͬ�������" << std::endl;

		showBlock(bp->b_addr);
		std::cout << "����ͬ�������" << std::endl << std::endl;
	}

	/* ����Ԥ�� */
	if (0) {
		std::cout << "����Ԥ��" << std::endl;
		
		short adev = Kernel::Instance().GetDeviceManager().ROOTDEV;
		std::cout << "Ԥ����ʼ" << std::endl;
		Buf* bp = bm.Breada(adev, 0, 1);
		std::cout << "Ԥ�����" << std::endl;

		std::cout << "ͬ������" << std::endl;
		showBlock(bp->b_addr);

		Buf* abp = bm.Bread(adev, 1);

		std::cout << "Ԥ����" << std::endl;
		showBlock(abp->b_addr);
		std::cout << "����Ԥ�����" << std::endl << std::endl;
	}

	/* �����첽д */
	if (0) {
		std::cout << "�����첽д" << std::endl;
		short dev = Kernel::Instance().GetDeviceManager().ROOTDEV;

		Buf* bp = bm.GetBlk(dev, 127);
		/* �������� */
		for (int i = 0; i < bm.BUFFER_SIZE; i++) {
			bp->b_addr[i] = 'b';
		}

		std::cout << "�첽д��ʼ" << std::endl;
		std::async(
			std::launch::async,
			[&bm, bp] {
			bm.Bawrite(bp);
		}).wait();
		std::cout << "�첽д���" << std::endl;
		
		showBlock(bp->b_addr);
		std::cout << "�����첽д���" << std::endl << std::endl;
	}

	/* �����ӳ�д */
	if (0) {
		std::cout << "�����ӳ�д" << std::endl;
		std::cout << "�����ӳ�д���" << std::endl << std::endl;
	}

	/* ������ջ����� */
	if (0) {
		std::cout << "������ջ�����" << std::endl;
		std::cout << "������ջ��������" << std::endl << std::endl;
	}

	/* ����ˢ�»����� */
	if (0) {
		std::cout << "����ˢ�»�����" << std::endl;
		std::cout << "����ˢ�»��������" << std::endl << std::endl;
	}
}

void testFileSystem()
{
	short dev = DeviceManager::ROOTDEV;
	FileSystem& fs = Kernel::Instance().GetFileSystem();
	BufferManager& bm = Kernel::Instance().GetBufferManager();

	/* �������ݿ���������ͷ� */
	if (1) {
		std::cout << "�����������ͷ����ݿ�" << std::endl;

		//std::stack<int> blknos;
		//std::queue<int> blknos;
		std::vector<int> blknos;

		std::cout << "�������ݿ鿪ʼ" << std::endl;
		for (;;) {
			Buf* buf = fs.Alloc(dev);
			if (nullptr == buf) {
				break;
			}

			//blknos.push(buf->b_blkno);
			blknos.push_back(buf->b_blkno);
			if (blknos.size() != 0 && blknos.size() % 10000 == 0) {
				std::cout << "���������ݿ�������" << blknos.size() << std::endl;
			}
			//std::cout << "�������ݿ�ţ�" << buf->b_blkno << std::endl;
			bm.Brelse(buf);

			if (blknos.size() > 65536) {
				std::cout << "Error" << std::endl;
				break;
			}
		}
		std::cout << "�������ݿ����" << std::endl;
		std::cout << "���������ݿ�������" << blknos.size() << std::endl << std::endl;
		
		std::default_random_engine rng(std::random_device{}());
		std::shuffle(blknos.begin(), blknos.end(), rng);

		std::cout << "�ͷ����ݿ鿪ʼ" << std::endl;
		while (!blknos.empty()) {
			//fs.Free(dev, blknos.top());
			//fs.Free(dev, blknos.front());
			fs.Free(dev, blknos.back());
			//blknos.pop();
			blknos.pop_back();

			if (blknos.size() % 10000 == 0) {
				std::cout << "ʣ�����ݿ�������" << blknos.size() << std::endl;
			}
		}
		std::cout << "�ͷ����ݿ����" << std::endl;
		std::cout << "�����������ͷ����ݿ����" << std::endl << std::endl;
		fs.Update();
	}
}

void test()
{
	//testDiskDriver();
	//testBuffer();
	testFileSystem();
}
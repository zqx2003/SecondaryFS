#include <iostream>
#include <iomanip>
#include <cstring>
#include <future>
#include "test.h"
#include "../src/inc/BufferManager.h"
#include "../src/inc/DiskDriver.h"

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

	DiskDriver::Open();

	/* ������ */
	buf.b_flags |= Buf::B_READ;
	buf.b_blkno = 0;
	buf.b_addr = buffer1;
	auto future = std::async(std::launch::async, [&buf]() {
		DiskDriver::DevStart(&buf);
	});
	future.wait();

	const int B_PER_LINE = 16;
	std::cout << "��ȡ���ԣ�" << std::endl;
	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	for (int i = 0; i < BUFSIZE; i++) {
		int val = static_cast<unsigned char>(buffer1[i]);
		std::cout << std::setfill('0') << std::setw(2) << std::hex << val << " ";
		if (i % B_PER_LINE == B_PER_LINE - 1) {
			std::cout << std::endl;
		}
	}
	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;

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
	DiskDriver::Close();

	std::cout << "д�����" << std::endl;
	std::cout << "���img�ļ��鿴���ݣ����س�����" << std::endl;
	char ch = 0;
	while ((ch = getchar()) != '\n')
		;

	DiskDriver::Open();
	buf.b_flags &= ~Buf::B_READ;
	buf.b_blkno = 127;
	buf.b_addr = buffer1;
	std::async(std::launch::async, [&buf]() {	/* �ָ� */
		DiskDriver::DevStart(&buf);
	}).wait();

	DiskDriver::Close();
}

void test()
{
	testDiskDriver();
}
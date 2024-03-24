#pragma once
#include <queue>
#include <fstream>
#include <future>
#include <mutex>
#include "../inc/Buf.h"

class DiskDriver {
public:
	static const int NDISK = 16;

public:
	static void DiskFormat(const char* disk_file_path, int isize, int fsize);
	
	static void Open();
	static void Close();

	static void DiskHandler();
	static void DevStart(Buf* bp);

	static void TaskClear();

private:
	static std::fstream* disk;
	static std::mutex mtx_rw;		/* 文件读写锁，与disk指针数目对应
									* 在极端情况下，可能会对同一个块有读写操作，所以只设置一个锁，控制读写互斥
									*/

	static std::mutex mtx_queue;
	static std::queue<std::future<void>> tasks;
};
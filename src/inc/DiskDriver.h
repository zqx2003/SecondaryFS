#pragma once
#include <fstream>
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

private:
	static std::fstream* disk;
	static std::mutex mtx_r;	/* 文件读锁，与disk指针数目对应 */
	static std::mutex mtx_w;	/* 文件写锁，与disk指针数目对应 */
};
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
	static std::mutex mtx_rw;		/* �ļ���д������diskָ����Ŀ��Ӧ
									* �ڼ�������£����ܻ��ͬһ�����ж�д����������ֻ����һ���������ƶ�д����
									*/

	static std::mutex mtx_queue;
	static std::queue<std::future<void>> tasks;
};
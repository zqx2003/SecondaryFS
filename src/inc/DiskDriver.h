#pragma once
#include <fstream>
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
};
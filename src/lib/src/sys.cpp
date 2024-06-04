#include "../inc/sys.h"
#include "../../inc/DiskDriver.h"
#include "../../inc/Kernel.h"

void Format(const char* disk_file_path, int isize, int fsize)
{
	DiskDriver::DiskFormat(disk_file_path, isize, fsize);
}

void _ls(const char* dir_path)
{
}

int _fopen(const char* file_path, int mode)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& fileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入文件路径，打开模式参数
	u.u_dirp = (char *)file_path;
	u.u_arg[1] = mode;

	// 打开文件获取fd
	fileMgr.Open();
	return u.u_ar0;
}


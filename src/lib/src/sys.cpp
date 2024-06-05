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

void _fclose(int fd)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入文件描述符
	u.u_arg[0] = fd;

	// 调用系统调用关闭文件
	FileMgr.Close();
}

int _fread(int fd, char* buffer, int length)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_arg[0] = fd;
	u.u_arg[1] = (int)buffer;
	u.u_arg[2] = length;

	// 调用读文件系统调用，返回读取字符数
	FileMgr.Read();
	return u.u_ar0;
}




#include "../inc/sys.h"
#include "../../inc/DiskDriver.h"
#include "../../inc/Kernel.h"

void Format(const char* disk_file_path, int isize, int fsize)
{
	DiskDriver::DiskFormat(disk_file_path, isize, fsize);
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

int _fwrite(int fd, const char* buffer, int length)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_arg[0] = fd;
	u.u_arg[1] = (int)buffer;
	u.u_arg[2] = length;

	// 调用读文件系统调用，返回写入字符数
	FileMgr.Write();
	return u.u_ar0;
}


void _flseek(int fd, int position, int whence)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_arg[0] = fd;
	u.u_arg[1] = position;
	u.u_arg[2] = whence;

	// 调用移动文件指针系统调用
	FileMgr.Seek();
}

int _fcreat(const char* file_path, int mode)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_arg[0] = (int)file_path;
	u.u_arg[1] = mode;
	u.u_dirp = (char*)file_path;

	// 调用移动文件指针系统调用
	FileMgr.Creat();
	return u.u_ar0;
}

void _mkdir(const char* dir_path)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_dirp = (char*)dir_path;
	u.u_arg[1] = 040755;

	// 调用创建目录系统调用
	FileMgr.MkNod();
}

void _fdelete(const char* path)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_dirp = (char*)path;

	// 调用删除文件系统调用
	FileMgr.UnLink();
}

void _cd(const char* dir_path)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_arg[0] = (int)dir_path;
	u.u_dirp = (char*)dir_path;

	// 调用跳转目录系统调用
	FileMgr.ChDir();
}

void _fsync(void)
{
	// 获取内核对象
	FileSystem& FileSys = Kernel::Instance().GetFileSystem();

	// 调用跳转目录系统调用
	FileSys.Update();
}

void _link(const char* new_path, const char* path)
{
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// 向user对象传入系统调用参数
	u.u_arg[1] = (int)new_path;
	u.u_dirp = (char*)path;

	// 调用跳转目录系统调用
	FileMgr.Link();
}

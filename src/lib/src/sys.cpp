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
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& fileMgr = Kernel::Instance().GetFileManager();

	// ��user�������ļ�·������ģʽ����
	u.u_dirp = (char *)file_path;
	u.u_arg[1] = mode;

	// ���ļ���ȡfd
	fileMgr.Open();
	return u.u_ar0;
}


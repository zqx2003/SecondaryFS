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

void _fclose(int fd)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user�������ļ�������
	u.u_arg[0] = fd;

	// ����ϵͳ���ùر��ļ�
	FileMgr.Close();
}

int _fread(int fd, char* buffer, int length)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_arg[0] = fd;
	u.u_arg[1] = (int)buffer;
	u.u_arg[2] = length;

	// ���ö��ļ�ϵͳ���ã����ض�ȡ�ַ���
	FileMgr.Read();
	return u.u_ar0;
}




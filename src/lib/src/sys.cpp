#include "../inc/sys.h"
#include "../../inc/DiskDriver.h"
#include "../../inc/Kernel.h"

void Format(const char* disk_file_path, int isize, int fsize)
{
	DiskDriver::DiskFormat(disk_file_path, isize, fsize);
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

int _fwrite(int fd, const char* buffer, int length)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_arg[0] = fd;
	u.u_arg[1] = (int)buffer;
	u.u_arg[2] = length;

	// ���ö��ļ�ϵͳ���ã�����д���ַ���
	FileMgr.Write();
	return u.u_ar0;
}


void _flseek(int fd, int position, int whence)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_arg[0] = fd;
	u.u_arg[1] = position;
	u.u_arg[2] = whence;

	// �����ƶ��ļ�ָ��ϵͳ����
	FileMgr.Seek();
}

int _fcreat(const char* file_path, int mode)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_arg[0] = (int)file_path;
	u.u_arg[1] = mode;
	u.u_dirp = (char*)file_path;

	// �����ƶ��ļ�ָ��ϵͳ����
	FileMgr.Creat();
	return u.u_ar0;
}

void _mkdir(const char* dir_path)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_dirp = (char*)dir_path;
	u.u_arg[1] = 040755;

	// ���ô���Ŀ¼ϵͳ����
	FileMgr.MkNod();
}

void _fdelete(const char* path)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_dirp = (char*)path;

	// ����ɾ���ļ�ϵͳ����
	FileMgr.UnLink();
}

void _cd(const char* dir_path)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_arg[0] = (int)dir_path;
	u.u_dirp = (char*)dir_path;

	// ������תĿ¼ϵͳ����
	FileMgr.ChDir();
}

void _fsync(void)
{
	// ��ȡ�ں˶���
	FileSystem& FileSys = Kernel::Instance().GetFileSystem();

	// ������תĿ¼ϵͳ����
	FileSys.Update();
}

void _link(const char* new_path, const char* path)
{
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	FileManager& FileMgr = Kernel::Instance().GetFileManager();

	// ��user������ϵͳ���ò���
	u.u_arg[1] = (int)new_path;
	u.u_dirp = (char*)path;

	// ������תĿ¼ϵͳ����
	FileMgr.Link();
}

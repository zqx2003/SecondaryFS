#pragma once
#include "File.h"
#include "INode.h"
#include "FileManager.h"

/*
 * @comment ������Unixv6�� struct user�ṹ��Ӧ�����ֻ�ı�
 * ���������޸ĳ�Ա�ṹ���֣������������͵Ķ�Ӧ��ϵ����:
 */
class User
{
public:
	/* u_error's Error Code */
	/* 1~32 ����linux ���ں˴����е�/usr/include/asm/errno.h, ����for V6++ */
	enum ErrorCode
	{
		_NOERROR = 0,	/* No error */
		_EPERM = 1,		/* Operation not permitted */
		_ENOENT = 2,		/* No such file or directory */
		_ESRCH = 3,		/* No such process */
		_EINTR = 4,		/* Interrupted system call */
		_EIO = 5,		/* I/O error */
		_ENXIO = 6,		/* No such device or address */
		_E2BIG = 7,		/* Arg list too long */
		_ENOEXEC = 8,	/* Exec format error */
		_EBADF = 9,		/* Bad file number */
		_ECHILD = 10,	/* No child processes */
		_EAGAIN = 11,	/* Try again */
		_ENOMEM = 12,	/* Out of memory */
		_EACCES = 13,	/* Permission denied */
		_EFAULT = 14,	/* Bad address */
		_ENOTBLK = 15,	/* Block device required */
		_EBUSY = 16,		/* Device or resource busy */
		_EEXIST = 17,	/* File exists */
		_EXDEV = 18,		/* Cross-device link */
		_ENODEV = 19,	/* No such device */
		_ENOTDIR = 20,	/* Not a directory */
		_EISDIR = 21,	/* Is a directory */
		_EINVAL = 22,	/* Invalid argument */
		_ENFILE = 23,	/* File table overflow */
		_EMFILE = 24,	/* Too many open files */
		_ENOTTY = 25,	/* Not a typewriter(terminal) */
		_ETXTBSY = 26,	/* Text file busy */
		_EFBIG = 27,		/* File too large */
		_ENOSPC = 28,	/* No space left on device */
		_ESPIPE = 29,	/* Illegal seek */
		_EROFS = 30,		/* Read-only file system */
		_EMLINK = 31,	/* Too many links */
		_EPIPE = 32,		/* Broken pipe */
		_ENOSYS = 100
		//EFAULT	= 106
	};

	/* ϵͳ������س�Ա */
	unsigned int u_ar0;			/* ָ�����ջ�ֳ���������EAX�Ĵ���
								��ŵ�ջ��Ԫ�����ֶδ�Ÿ�ջ��Ԫ�ĵ�ַ��
								��V6��r0���ϵͳ���õķ���ֵ���û�����
								x86ƽ̨��ʹ��EAX��ŷ���ֵ�����u.u_ar0[R0] */

	int u_arg[5];				/* ��ŵ�ǰϵͳ���ò��� */
	char* u_dirp;				/* ϵͳ���ò���(һ������Pathname)��ָ�� */

	/* �ļ�ϵͳ��س�Ա */
	Inode* u_cdir;		/* ָ��ǰĿ¼��Inodeָ�� */
	Inode* u_pdir;		/* ָ��Ŀ¼��Inodeָ�� */

	DirectoryEntry u_dent;					/* ��ǰĿ¼��Ŀ¼�� */
	char u_dbuf[DirectoryEntry::DIRSIZ];	/* ��ǰ·������ */
	char u_curdir[128];						/* ��ǰ����Ŀ¼����·�� */

	ErrorCode u_error;			/* ��Ŵ����� */

	/* ���̵��û���ʶ */
	short u_uid;		/* ��Ч�û�ID */
	short u_gid;		/* ��Ч��ID */
	short u_ruid;		/* ��ʵ�û�ID */
	short u_rgid;		/* ��ʵ��ID */

	/* �ļ�ϵͳ��س�Ա */
	OpenFiles u_ofiles;		/* ���̴��ļ������������ */

	/* �ļ�I/O���� */
	IOParameter u_IOParam;	/* ��¼��ǰ����д�ļ���ƫ�������û�Ŀ�������ʣ���ֽ������� */

	/* Member Functions */
public:
	/* ����ϵͳ���ò���uid������Ч�û�ID����ʵ�û�ID�������û�ID(p_uid) */
	void Setuid();

	/* ��ȡ�û�ID����16����Ϊ��ʵ�û�ID(u_ruid)����16����Ϊ��Ч�û�ID(u_uid) */
	void Getuid();

	/* ����ϵͳ���ò���gid������Ч��ID����ʵ��ID */
	void Setgid();

	/* ��ȡ��ID, ��16����Ϊ��ʵ��ID(u_rgid)����16����Ϊ��Ч��ID(u_gid) */
	void Getgid();

	/* ��ȡ��ǰ�û�����Ŀ¼ */
	void Pwd();

	/* ��鵱ǰ�û��Ƿ��ǳ����û� */
	bool SUser();
};

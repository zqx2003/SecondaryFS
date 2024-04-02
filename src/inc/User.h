#pragma once
#include "File.h"
#include "INode.h"
#include "FileManager.h"

/*
 * @comment 该类与Unixv6中 struct user结构对应，因此只改变
 * 类名，不修改成员结构名字，关于数据类型的对应关系如下:
 */
class User
{
public:
	/* u_error's Error Code */
	/* 1~32 来自linux 的内核代码中的/usr/include/asm/errno.h, 其余for V6++ */
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

	/* 系统调用相关成员 */
	unsigned int u_ar0;			/* 指向核心栈现场保护区中EAX寄存器
								存放的栈单元，本字段存放该栈单元的地址。
								在V6中r0存放系统调用的返回值给用户程序，
								x86平台上使用EAX存放返回值，替代u.u_ar0[R0] */

	int u_arg[5];				/* 存放当前系统调用参数 */
	char* u_dirp;				/* 系统调用参数(一般用于Pathname)的指针 */

	/* 文件系统相关成员 */
	Inode* u_cdir;		/* 指向当前目录的Inode指针 */
	Inode* u_pdir;		/* 指向父目录的Inode指针 */

	DirectoryEntry u_dent;					/* 当前目录的目录项 */
	char u_dbuf[DirectoryEntry::DIRSIZ];	/* 当前路径分量 */
	char u_curdir[128];						/* 当前工作目录完整路径 */

	ErrorCode u_error;			/* 存放错误码 */

	/* 进程的用户标识 */
	short u_uid;		/* 有效用户ID */
	short u_gid;		/* 有效组ID */
	short u_ruid;		/* 真实用户ID */
	short u_rgid;		/* 真实组ID */

	/* 文件系统相关成员 */
	OpenFiles u_ofiles;		/* 进程打开文件描述符表对象 */

	/* 文件I/O操作 */
	IOParameter u_IOParam;	/* 记录当前读、写文件的偏移量，用户目标区域和剩余字节数参数 */

	/* Member Functions */
public:
	/* 根据系统调用参数uid设置有效用户ID，真实用户ID，进程用户ID(p_uid) */
	void Setuid();

	/* 获取用户ID，低16比特为真实用户ID(u_ruid)，高16比特为有效用户ID(u_uid) */
	void Getuid();

	/* 根据系统调用参数gid设置有效组ID，真实组ID */
	void Setgid();

	/* 获取组ID, 低16比特为真实组ID(u_rgid)，高16比特为有效组ID(u_gid) */
	void Getgid();

	/* 获取当前用户工作目录 */
	void Pwd();

	/* 检查当前用户是否是超级用户 */
	bool SUser();
};

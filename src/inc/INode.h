#pragma once

class Inode {
public:
	/* static const member */
	static const unsigned int IALLOC = 0x8000;		/* 文件被使用 */
	static const unsigned int IFMT = 0x6000;		/* 文件类型掩码 */
	static const unsigned int IFDIR = 0x4000;		/* 文件类型：目录文件 */
	static const unsigned int IFCHR = 0x2000;		/* 字符设备特殊类型文件 */
	static const unsigned int IFBLK = 0x6000;		/* 块设备特殊类型文件，为0表示常规数据文件 */
	static const unsigned int ILARG = 0x1000;		/* 文件长度类型：大型或巨型文件 */
	static const unsigned int ISUID = 0x800;		/* 执行时文件时将用户的有效用户ID修改为文件所有者的User ID */
	static const unsigned int ISGID = 0x400;		/* 执行时文件时将用户的有效组ID修改为文件所有者的Group ID */
	static const unsigned int ISVTX = 0x200;		/* 使用后仍然位于交换区上的正文段 */
	static const unsigned int IREAD = 0x100;		/* 对文件的读权限 */
	static const unsigned int IWRITE = 0x80;		/* 对文件的写权限 */
	static const unsigned int IEXEC = 0x40;			/* 对文件的执行权限 */
	static const unsigned int IRWXU = (IREAD | IWRITE | IEXEC);		/* 文件主对文件的读、写、执行权限 */
	static const unsigned int IRWXG = ((IRWXU) >> 3);			/* 文件主同组用户对文件的读、写、执行权限 */
	static const unsigned int IRWXO = ((IRWXU) >> 6);			/* 其他用户对文件的读、写、执行权限 */

	static const int BLOCK_SIZE = 512;		/* 文件逻辑块大小: 512字节 */
	static const int ADDRESS_PER_INDEX_BLOCK = BLOCK_SIZE / sizeof(int);	/* 每个间接索引表(或索引块)包含的物理盘块号 */

	static const int SMALL_FILE_BLOCK = 6;	/* 小型文件：直接索引表最多可寻址的逻辑块号 */
	static const int LARGE_FILE_BLOCK = 128 * 2 + 6;	/* 大型文件：经一次间接索引表最多可寻址的逻辑块号 */
	static const int HUGE_FILE_BLOCK = 128 * 128 * 2 + 128 * 2 + 6;	/* 巨型文件：经二次间接索引最大可寻址文件逻辑块号 */

	static const int PIPSIZ = SMALL_FILE_BLOCK * BLOCK_SIZE;

	static const int BNUM = 10;

	unsigned	i_flag;			/* 状态标志位，暂未使用 */
	unsigned	i_mode;			/* 文件工作方式信息 */

	int			i_count;		/* 引用计数 */
	int			i_nlink;		/* 文件链接计数，即该文件在目录树中不同路径名的数量 */

	short		i_dev;			/* 外存inode所在存储设备的设备号，暂未使用 */
	int			i_number;		/* 外存inode区中的编号，暂未使用 */

	short		i_uid;			/* 文件所有者用户数 */
	short		i_gid;			/* 文件所有者组标识数，暂未使用 */

	int			i_size;			/* 文件大小，字节为单位 */
	int			i_addr[BNUM];	/* 用于文件逻辑块号和物理块号转换的基本索引表 */

	int			i_lastr;		/* 存放最近一次读取文件的逻辑块号，用于判断是否需要预读，暂未使用 */
};

class DiskInode {
public:
	static const int BNUM = 10;

	unsigned	d_mode;			/* 文件类型和访问控制位 */
	int			d_nlink;		/* 硬链接计数，即该文件在目录树中不同路径名的数量 */
	short		d_uid;			/* 文件所有者uid */
	short		d_gid;			/* 文件所有者gid，暂未使用 */
	
	int			d_size;			/* 文件长度，字节为单位 */
	int			d_addr[BNUM];	/* 地址映射表，登记逻辑块和物理块之间的映射关系 */

	int			d_atime;		/* 最后访问时间，暂未使用 */
	int			d_mtime;		/* 最后修改时间，暂未使用 */
};
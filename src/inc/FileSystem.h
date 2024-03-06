#pragma once

class SuperBlock {
public:
	static const int NFREE = 100;
	static const int NINODE = 100;

	int s_isize;		/* Inode区尺寸，数据块数量 */
	int s_fsize;		/* 磁盘数据块总数 */

	int s_nfree;		/* 直接管理的空闲盘块数量 */
	int s_free[NFREE];	/* 直接管理的空闲盘块索引表 */
	int s_flock;		/* 空闲盘块索引表的锁，暂未使用 */

	int s_ninode;		/* 直接管理的空闲外存Inode数量 */
	int s_inode[NINODE];	/* 直接管理的空闲外存Inode索引表 */
	int ilock;			/* 空闲Inode表的锁，暂未使用 */

	int s_fmod;			/* 内存SuperBlock副本的修改标志，卸载时需写回磁盘 */
	int s_ronly;		/* 本文件系统只能读出，暂未使用 */
	int s_time;			/* 最近一次更新时间，暂未使用 */
	int padding[47];	/* 填充SuperBlock块大小等于1024字节，占据2个扇区 */
};

class FileSystem {
public:
	static const int INODE_NUMBER_PER_SECTOR = 8;

	void Initialize();
};
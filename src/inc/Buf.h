#pragma once
#include <mutex>
#include <condition_variable>

class Buf {
public:
	enum BufFlag {	/* b_flags中标志位 */
		B_WRITE = 0x1,		/* 写操作。将缓存中的信息写到硬盘上去 */
		B_READ = 0x2,		/* 读操作。从盘读取信息到缓存中 */
		B_DONE = 0x4,		/* I/O操作结束 */
		B_ERROR = 0x8,		/* I/O因出错而终止 */
		B_BUSY = 0x10,		/* 相应缓存正在使用中 */
		B_WANTED = 0x20,	/* 有进程正在等待使用该buf管理的资源，清B_BUSY标志时，要唤醒这种进程 */
		B_ASYNC = 0x40,		/* 异步I/O，不需要等待其结束 */
		B_DELWRI = 0x80		/* 延迟写，在相应缓存要移做他用时，再将其内容写到相应块设备上 */
	};

public:
	unsigned b_flags;		/* 缓存控制块标志位 */
	
	int		padding;		/* 4字节填充，使得b_forw和b_back在Buf类和Devtab类中的字段顺序一致，否则强转回出错 */

	/* 缓存控制块勾连指针 */
	Buf*	b_forw;
	Buf*	b_back;
	Buf*	av_forw;
	Buf*	av_back;

	short	b_dev;			/* 主次设备号，高8位是主设备号，低8位是次设备号 */
	int		b_wcount;		/* 需传送的字节数，暂未使用 */
	char*	b_addr;			/* 指向该缓存控制块所管理的缓冲区首地址 */
	int		b_blkno;		/* 磁盘逻辑块号 */
	int		b_error;		/* I/O出错时信息，暂未使用 */
	int		b_resid;		/* I/O出错时尚未传送的字节数，暂未使用 */

	std::mutex	b_mtx;				/* 缓存的互斥锁 */
	std::condition_variable b_cv;	/* 缓存的条件变量 */
};
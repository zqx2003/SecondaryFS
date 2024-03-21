#pragma once
#include "../inc/Buf.h"
#include "../inc/DeviceManager.h"

class BufferManager {
public:
	static const int NBUF = 15;			/* 缓存控制块、缓冲区的数量 */
	static const int BUFFER_SIZE = 512;	/* 缓冲区大小，以字节为单位 */

public:
	BufferManager();
	~BufferManager();

	void Initialize();					/* 缓存控制队列的初始化 */

	Buf* GetBlk(short dev, int blkno);	/* 申请一块缓存，用于读写设备dev上的字符块blkno */
	void Brelse(Buf* bp);				/* 释放缓存控制块buf */
	void IOWait(Buf* bp);				/* 同步方式I/O，等待I/O结束 */
	void IODone(Buf* bp);				/* I/O操作的善后处理 */

	Buf* Bread(short dev, int blkno);	/* 读一个磁盘块 */
	Buf* Breada(short adev, int blkno, int rablkno);	/* 读一个磁盘块，带有预读方式。
														 * adev为主、次设备号。blkno为目标磁盘块逻辑块号，同步方式读blkno。
														 * rablkno为预读磁盘块逻辑块号，异步方式读rablkno。 */

	void Bwrite(Buf* bp);				/* 写一个磁盘块 */
	void Bdwrite(Buf* bp);				/* 延迟写磁盘块 */
	void Bawrite(Buf* bp);				/* 异步写磁盘块 */

	void ClrBuf(Buf* bp);				/* 清空缓冲区内容 */
	void Bflush(short dev);				/* 将dev制定设备队列中延迟写的缓存全部输出到磁盘 */

	Buf& GetBFreeList();				/* 获取自由缓存队列控制块Buf对象引用 */

private:
	void NotAvail(Buf* bp);				/* 从自由队列中摘下指定的缓存控制块buf */
	Buf* InCore(short adev, int blkno);	/* 检查指定字符块是否已在缓存中 */

private:
	Buf bFreeList;						/* 自由缓存队列控制块 */
	Buf m_Buf[NBUF];					/* 缓存控制块数组 */
	char Buffer[NBUF][BUFFER_SIZE];		/* 缓冲区数组 */

	DeviceManager* m_DeviceManager;		/* 指向设别管理模块的全局对象 */
};
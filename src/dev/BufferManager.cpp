#include "../inc/BufferManager.h"

/* ========================================BufferManager======================================== */
BufferManager::BufferManager()
{
}

BufferManager::~BufferManager()
{
}

void BufferManager::Initialize()
{
}

Buf* BufferManager::GetBlk(short dev, int blkno)
{
	return nullptr;
}

void BufferManager::Brelse(Buf* bp)
{
}

void BufferManager::IOWait(Buf* bp)
{
}

void BufferManager::IODone(Buf* bp)
{
}

Buf* BufferManager::Bread(short dev, int blkno)
{
	return nullptr;
}

Buf* BufferManager::Breada(short adev, int blkno, int rablkno)
{
	return nullptr;
}

void BufferManager::Bwrite(Buf* bp)
{
}

void BufferManager::Bdwrite(Buf* bp)
{
}

void BufferManager::Bawrite(Buf* bp)
{
}

void BufferManager::ClrBuf(Buf* bp)
{
}

void BufferManager::Bflush(short dev)
{
}

Buf& BufferManager::GetBFreeList()
{
	return this->bFreeList;
}

void BufferManager::NotAvail(Buf* bp)
{
	/* 从自由队列中取出 */
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	/* 置B_BUSY标志 */
	bp->b_flags |= Buf::B_BUSY;
}

Buf* BufferManager::InCore(short adev, int blkno)
{
	Buf* bp;
	Devtab* dp;
	short major = 0;	/* 后续更改为根据设备号获取主设备号 */

	dp = this->m_DeviceManager->GetBlockDevice(major).d_tab;
	for (bp = dp->b_forw; bp != (Buf*)dp; bp = bp->b_forw) {
		if (blkno == bp->b_blkno && adev == bp->b_dev) {
			return bp;
		}
	}
	return nullptr;
}

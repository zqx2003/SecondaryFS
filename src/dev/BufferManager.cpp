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
	/* �����ɶ�����ȡ�� */
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	/* ��B_BUSY��־ */
	bp->b_flags |= Buf::B_BUSY;
}

Buf* BufferManager::InCore(short adev, int blkno)
{
	Buf* bp;
	Devtab* dp;
	short major = 0;	/* ��������Ϊ�����豸�Ż�ȡ���豸�� */

	dp = this->m_DeviceManager->GetBlockDevice(major).d_tab;
	for (bp = dp->b_forw; bp != (Buf*)dp; bp = bp->b_forw) {
		if (blkno == bp->b_blkno && adev == bp->b_dev) {
			return bp;
		}
	}
	return nullptr;
}

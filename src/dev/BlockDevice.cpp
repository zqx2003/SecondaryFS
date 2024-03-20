#include "../inc/BlockDevice.h"
#include "../inc/BufferManager.h"
#include "../inc/DiskDriver.h"
#include "../inc/Kernel.h"

/* ========================================Devtab======================================== */
Devtab g_Dtab;

Devtab::Devtab()
{
	this->d_active = 0;
	this->d_errcnt = 0;
	this->b_forw = nullptr;
	this->b_back = nullptr;
	this->d_actf = nullptr;
	this->d_actl = nullptr;
}

Devtab::~Devtab()
{
}

/* ========================================BlockDevice======================================== */

BlockDevice::BlockDevice()
{
	this->d_tab = nullptr;
}

BlockDevice::BlockDevice(Devtab* tab)
{
	this->d_tab = tab;
	if (nullptr != this->d_tab) {
		this->d_tab->b_forw = reinterpret_cast<Buf*> (this->d_tab);
		this->d_tab->b_back = reinterpret_cast<Buf*> (this->d_tab);
	}
}

BlockDevice::~BlockDevice()
{
}

/* ========================================DiskBlockDevice======================================== */
DiskBlockDevice g_DiskDevice(&g_Dtab);

int DiskBlockDevice::NSECTOR = 0x00010000;

DiskBlockDevice::DiskBlockDevice(Devtab* tab)
	: BlockDevice(tab)
{
}

DiskBlockDevice::~DiskBlockDevice()
{
}

int DiskBlockDevice::Open(short dev, int mode)
{
	return 0;
}

int DiskBlockDevice::Close(short dev, int mode)
{
	return 0;
}

int DiskBlockDevice::Strategy(Buf* bp)
{
	/* 检查I/O操作块是否超出硬盘扇区数上限 */
	if (bp->b_blkno >= DiskBlockDevice::NSECTOR) {
		/* 设置出错标志 */
		bp->b_flags |= Buf::B_ERROR;

		BufferManager bm = Kernel::Instance().GetBufferManager();

		/* 出错不执行I/O操作，调用IODone结束I/O */
		bm.IODone(bp);
		return 0;
	}

	/* 将bp加入I/O请求队列的队尾 */
	bp->av_forw = nullptr;					/* 当前缓存的尾部没有缓存 */

	this->mtx_tab.lock();					/* 加锁 */
	if (nullptr == this->d_tab->d_actf) {	/* 队列为空 */
		this->d_tab->d_actf = bp;			/* 将队首指针直接指向该缓存 */
	}
	else {									/* 队列不为空 */
		this->d_tab->d_actl->av_forw = bp;	/* 将当前队尾缓存的av_forw指向该缓存 */
	}
	this->d_tab->d_actl = bp;				/* 将队尾指针更新为当前缓存 */

	if (!this->d_tab->d_active) {
		this->Start();
	}
	this->mtx_tab.unlock();					/* 解锁 */

	return 0;
}

void DiskBlockDevice::Start()
{
	Buf* bp = this->d_tab->d_actf;

	if (nullptr == bp) {	/* I/O请求队列为空，立即返回 */
		return;
	}

	this->d_tab->d_active++;	/* 置I/O忙标志 */

	/* 启动IO操作 */
	DiskDriver::DevStart(bp);
}

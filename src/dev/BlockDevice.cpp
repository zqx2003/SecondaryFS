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
	/* ���I/O�������Ƿ񳬳�Ӳ������������ */
	if (bp->b_blkno >= DiskBlockDevice::NSECTOR) {
		/* ���ó����־ */
		bp->b_flags |= Buf::B_ERROR;

		BufferManager bm = Kernel::Instance().GetBufferManager();

		/* ����ִ��I/O����������IODone����I/O */
		bm.IODone(bp);
		return 0;
	}

	/* ��bp����I/O������еĶ�β */
	bp->av_forw = nullptr;					/* ��ǰ�����β��û�л��� */

	this->mtx_tab.lock();					/* ���� */
	if (nullptr == this->d_tab->d_actf) {	/* ����Ϊ�� */
		this->d_tab->d_actf = bp;			/* ������ָ��ֱ��ָ��û��� */
	}
	else {									/* ���в�Ϊ�� */
		this->d_tab->d_actl->av_forw = bp;	/* ����ǰ��β�����av_forwָ��û��� */
	}
	this->d_tab->d_actl = bp;				/* ����βָ�����Ϊ��ǰ���� */

	if (!this->d_tab->d_active) {
		this->Start();
	}
	this->mtx_tab.unlock();					/* ���� */

	return 0;
}

void DiskBlockDevice::Start()
{
	Buf* bp = this->d_tab->d_actf;

	if (nullptr == bp) {	/* I/O�������Ϊ�գ��������� */
		return;
	}

	this->d_tab->d_active++;	/* ��I/Oæ��־ */

	/* ����IO���� */
	DiskDriver::DevStart(bp);
}

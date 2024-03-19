#include "../inc/BlockDevice.h"

/* ========================================Devtab======================================== */
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
DiskBlockDevice::DiskBlockDevice(Devtab* tab)
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
	return 0;
}

void DiskBlockDevice::Start()
{
}

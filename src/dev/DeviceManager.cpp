#include <iostream>
#include "../inc/BlockDevice.h"
#include "../inc/DeviceManager.h"

extern DiskBlockDevice g_DiskDevice;

DeviceManager::DeviceManager()
{
	this->nblkdev = 0;
	memset(this->bdevsw, 0, sizeof(this->bdevsw));
}

DeviceManager::~DeviceManager()
{
}

void DeviceManager::Initilize()
{
	this->bdevsw[0] = &g_DiskDevice;
	this->nblkdev = 1;
}

int DeviceManager::GetNBlkDev()
{
	return this->nblkdev;
}

BlockDevice& DeviceManager::GetBlockDevice(short major)
{
	if (major >= this->nblkdev || major < 0) {
		std::cout << "Block Device Doesn't Exist!" << std::endl;
		exit(-1);
	}
	return *(this->bdevsw[major]);
}

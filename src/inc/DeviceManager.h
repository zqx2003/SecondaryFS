#pragma once
#include "BlockDevice.h"

class DeviceManager {
public:
	static const int MAX_DEVICE_NUM = 10;			/* 系统允许最大设备数量 */

public:
	DeviceManager();
	~DeviceManager();

	/* 初始化设备基类指针数组，相当于对设备开关表bdevsw的初始化 */
	void Initilize();

	int GetNBlkDev();								/* 获取系统中实际块设备数量 */
	BlockDevice& GetBlockDevice(short major);		/* 根据主设备号major获取响应的块设备对象实例 */

private:
	int nblkdev;									/* 系统中块设备的数量 */
	BlockDevice* bdevsw[MAX_DEVICE_NUM];			/* 指向块设备基类的指针数组，相当于块设备开关表 */
};
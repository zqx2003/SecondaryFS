#pragma once
#include "BlockDevice.h"

class DeviceManager {
public:
	static const int MAX_DEVICE_NUM = 10;			/* ϵͳ��������豸���� */

public:
	DeviceManager();
	~DeviceManager();

	/* ��ʼ���豸����ָ�����飬�൱�ڶ��豸���ر�bdevsw�ĳ�ʼ�� */
	void Initilize();

	int GetNBlkDev();								/* ��ȡϵͳ��ʵ�ʿ��豸���� */
	BlockDevice& GetBlockDevice(short major);		/* �������豸��major��ȡ��Ӧ�Ŀ��豸����ʵ�� */

private:
	int nblkdev;									/* ϵͳ�п��豸������ */
	BlockDevice* bdevsw[MAX_DEVICE_NUM];			/* ָ����豸�����ָ�����飬�൱�ڿ��豸���ر� */
};
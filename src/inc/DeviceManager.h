#pragma once
#include "BlockDevice.h"

class DeviceManager {
public:
	static const int MAX_DEVICE_NUM = 10;			/* ϵͳ��������豸���� */
	static const int NODEV = -1;					/* NODEV�豸�� */

	static const short ROOTDEV = (0 << 8) | 0;		/* ���������豸�� */
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
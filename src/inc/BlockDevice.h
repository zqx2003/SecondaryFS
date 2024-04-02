#pragma once
#include <mutex>
#include "Buf.h"

class Devtab {
public:
	Devtab();
	~Devtab();

public:
	int d_active;
	int d_errcnt;
	Buf* b_forw;
	Buf* b_back;
	Buf* d_actf;
	Buf* d_actl;
};

class BlockDevice {
public:
	BlockDevice();
	BlockDevice(Devtab* tab);
	virtual ~BlockDevice();

	virtual int Open(short dev, int mode) = 0;
	virtual int Close(short dev, int mode) = 0;
	virtual int Strategy(Buf* bp) = 0;
	virtual void Start() = 0;

public:
	int d_super_block_sector_number = 0;/* ����SuperBlockλ�ڴ����ϵ������ţ�ռ��0, 1���������� */
	int d_inode_zone_start_sector = 0;
	int d_inode_zone_size = 0;			/* ���������Inode��ռ�ݵ������� */
	int d_data_zone_start_sector = 0;	/* ����������ʼ������ */
	int d_data_zone_end_sector = 0;		/* �������Ľ��������� */
	int d_data_zone_size = 0;			/* ������ռ�ݵ��������� */

	Devtab* d_tab;
	std::mutex mtx_dev;
};

class DiskBlockDevice : public BlockDevice {
public:
	static int NSECTOR;		/* �������������������ĳ�ϵͳ����ʱ�Զ���ȡ */

public:
	DiskBlockDevice(Devtab* tab);
	virtual ~DiskBlockDevice();

	int Open(short dev, int mode) override;
	int Close(short dev, int mode) override;
	int Strategy(Buf* bp) override;

	void Start() override;
};

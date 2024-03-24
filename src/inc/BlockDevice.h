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
	Devtab* d_tab;
	std::mutex mtx_dev;
};

class DiskBlockDevice : public BlockDevice {
public:
	static int NSECTOR;		/* 磁盘扇区数，后续将改成系统启动时自动获取 */

public:
	DiskBlockDevice(Devtab* tab);
	virtual ~DiskBlockDevice();

	int Open(short dev, int mode) override;
	int Close(short dev, int mode) override;
	int Strategy(Buf* bp) override;

	void Start() override;
};

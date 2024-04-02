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
	int d_super_block_sector_number = 0;/* 定义SuperBlock位于磁盘上的扇区号，占据0, 1两个扇区。 */
	int d_inode_zone_start_sector = 0;
	int d_inode_zone_size = 0;			/* 磁盘上外存Inode区占据的扇区数 */
	int d_data_zone_start_sector = 0;	/* 数据区的起始扇区号 */
	int d_data_zone_end_sector = 0;		/* 数据区的结束扇区号 */
	int d_data_zone_size = 0;			/* 数据区占据的扇区数量 */

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

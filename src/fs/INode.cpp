#include "../inc/BufferManager.h"
#include "../inc/DeviceManager.h"
#include "../inc/INode.h"
#include "../inc/Kernel.h"
#include "../inc/Utility.h"

/* ========================================Inode======================================== */
int Inode::rablock = 0;

Inode::Inode()
{
	/* 清空Inode对象中的数据 */
// this->Clean(); 
/* 去除this->Clean();的理由：
 * Inode::Clean()特定用于IAlloc()中清空新分配DiskInode的原有数据，
 * 即旧文件信息。Clean()函数中不应当清除i_dev, i_number, i_flag, i_count,
 * 这是属于内存Inode而非DiskInode包含的旧文件信息，而Inode类构造函数需要
 * 将其初始化为无效值。
 */

 /* 将Inode对象的成员变量初始化为无效值 */
	this->i_flag = 0;
	this->i_mode = 0;
	this->i_count = 0;
	this->i_nlink = 0;
	this->i_dev = -1;
	this->i_number = -1;
	this->i_uid = -1;
	this->i_gid = -1;
	this->i_size = 0;
	this->i_lastr = -1;
	for (int i = 0; i < 10; i++)
	{
		this->i_addr[i] = 0;
	}
}

Inode::~Inode()
{
}

void Inode::ReadI()
{
	int lbn;	/* 文件逻辑块号 */
	int bn;		/* lbn对应的物理盘块号 */
	int offset;	/* 当前字符块内起始传送位置 */
	int nbytes;	/* 传送至用户目标区字节数量 */
	short dev;
	Buf* pBuf;
	User& u = Kernel::Instance().GetUser();
	BufferManager& bufMgr = Kernel::Instance().GetBufferManager();
	DeviceManager& devMgr = Kernel::Instance().GetDeviceManager();

	if (0 == u.u_IOParam.m_Count)
	{
		/* 需要读字节数为零，则返回 */
		return;
	}

	this->i_flag |= Inode::IACC;

	///* 如果是字符设备文件 ，调用外设读函数*/
	//if ((this->i_mode & Inode::IFMT) == Inode::IFCHR)
	//{
	//	short major = Utility::GetMajor(this->i_addr[0]);

	//	devMgr.GetCharDevice(major).Read(this->i_addr[0]);
	//	return;
	//}

	/* 一次一个字符块地读入所需全部数据，直至遇到文件尾 */
	while (u.u_IOParam.m_Count != 0)
	{
		lbn = bn = u.u_IOParam.m_Offset / Inode::BLOCK_SIZE;
		offset = u.u_IOParam.m_Offset % Inode::BLOCK_SIZE;
		/* 传送到用户区的字节数量，取读请求的剩余字节数与当前字符块内有效字节数较小值 */
		nbytes = Utility::Min(Inode::BLOCK_SIZE - offset /* 块内有效字节数 */, u.u_IOParam.m_Count);

		if ((this->i_mode & Inode::IFMT) != Inode::IFBLK)
		{	/* 如果不是特殊块设备文件 */

			int remain = this->i_size - u.u_IOParam.m_Offset;
			/* 如果已读到超过文件结尾 */
			if (remain <= 0)
			{
				return;
			}
			/* 传送的字节数量还取决于剩余文件的长度 */
			nbytes = Utility::Min(nbytes, remain);

			/* 将逻辑块号lbn转换成物理盘块号bn ，Bmap有设置Inode::rablock。当UNIX认为获取预读块的开销太大时，
			 * 会放弃预读，此时 Inode::rablock 值为 0。
			 * */
			if ((bn = this->Bmap(lbn)) == 0)
			{
				return;
			}
			dev = this->i_dev;
		}
		else	/* 如果是特殊块设备文件 */
		{
			dev = this->i_addr[0];	/* 特殊块设备文件i_addr[0]中存放的是设备号 */
			Inode::rablock = bn + 1;
		}

		if (this->i_lastr + 1 == lbn)	/* 如果是顺序读，则进行预读 */
		{
			/* 读当前块，并预读下一块 */
			pBuf = bufMgr.Breada(dev, bn, Inode::rablock);
		}
		else
		{
			pBuf = bufMgr.Bread(dev, bn);
		}
		/* 记录最近读取字符块的逻辑块号 */
		this->i_lastr = lbn;

		/* 缓存中数据起始读位置 */
		char* start = pBuf->b_addr + offset;

		/* 读操作: 从缓冲区拷贝到用户目标区
		 * i386芯片用同一张页表映射用户空间和内核空间，这一点硬件上的差异 使得i386上实现 iomove操作
		 * 比PDP-11要容易许多*/
		Utility::IOMove(start, u.u_IOParam.m_Base, nbytes);

		/* 用传送字节数nbytes更新读写位置 */
		u.u_IOParam.m_Base += nbytes;
		u.u_IOParam.m_Offset += nbytes;
		u.u_IOParam.m_Count -= nbytes;

		bufMgr.Brelse(pBuf);	/* 使用完缓存，释放该资源 */
	}
}

void Inode::WriteI()
{
}

int Inode::Bmap(int lbn)
{
	return 0;
}

void Inode::OpenI(int mode)
{
}

void Inode::CloseI(int mode)
{
}

void Inode::IUpdate(int time)
{
}

void Inode::ITrunc()
{
}

void Inode::Prele()
{
}

void Inode::Plock()
{
}

void Inode::NFrele()
{
}

void Inode::NFlock()
{
}

void Inode::Clean()
{
}

void Inode::ICopy(Buf* bp, int inumber)
{
}

/* ========================================DiskInode======================================== */
DiskInode::DiskInode()
{
	/*
 * 如果DiskInode没有构造函数，会发生如下较难察觉的错误：
 * DiskInode作为局部变量占据函数Stack Frame中的内存空间，但是
 * 这段空间没有被正确初始化，仍旧保留着先前栈内容，由于并不是
 * DiskInode所有字段都会被更新，将DiskInode写回到磁盘上时，可能
 * 将先前栈内容一同写回，导致写回结果出现莫名其妙的数据。
 */
	this->d_mode = 0;
	this->d_nlink = 0;
	this->d_uid = -1;
	this->d_gid = -1;
	this->d_size = 0;
	for (int i = 0; i < 10; i++)
	{
		this->d_addr[i] = 0;
	}
	this->d_atime = 0;
	this->d_mtime = 0;
}

DiskInode::~DiskInode()
{
}

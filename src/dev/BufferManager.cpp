#include <iostream>
#include "../inc/BufferManager.h"
#include "../inc/Kernel.h"
#include "../inc/Utility.h"

/* ========================================BufferManager======================================== */
BufferManager::BufferManager()
{
}

BufferManager::~BufferManager()
{
}

void BufferManager::Initialize()
{
	int i;
	Buf* bp;

	this->bFreeList.b_forw = this->bFreeList.b_back = &(this->bFreeList);
	this->bFreeList.av_forw = this->bFreeList.av_back = &(this->bFreeList);

	for (i = 0; i < NBUF; i++) {
		bp = &(this->m_Buf[i]);
		bp->b_dev = -1;
		bp->b_addr = this->Buffer[i];
		/* 初始化NODEV队列 */
		bp->b_back = &(this->bFreeList);
		bp->b_forw = this->bFreeList.b_forw;
		this->bFreeList.b_forw->b_back = bp;
		this->bFreeList.b_forw = bp;
		/* 初始化自由队列 */
		bp->b_flags = Buf::B_BUSY;
		Brelse(bp);
	}

	this->m_DeviceManager = &Kernel::Instance().GetDeviceManager();
}

Buf* BufferManager::GetBlk(short dev, int blkno)
{
	Buf* bp;
	Devtab* dp;

	/* 如果主设备号超出了系统中块设备数量 */
	if (Utility::GetMajor(dev) >= this->m_DeviceManager->GetNBlkDev()) {
		std::cout << "nblkdev: There doesn't exist the device" << std::endl;
		exit(-1);
	}

	/*
	 * 如果设备队列中已经存在相应缓存，则返回该缓存；
	 * 否则从自由队列中分配新的缓存用于字符块读写。
	 */
loop:
	/* 表示请求NODEV设备中字符块 */
	if (dev < 0)
	{
		dp = (Devtab*)(&this->bFreeList);
	}
	else
	{
		short major = Utility::GetMajor(dev);
		/* 根据主设备号获得块设备表 */
		dp = this->m_DeviceManager->GetBlockDevice(major).d_tab;

		if (nullptr == dp)
		{
			std::cout << "Null devtab!" << std::endl;
			exit(-1);
		}
		/* 首先在该设备队列中搜索是否有相应的缓存 */
		for (bp = dp->b_forw; bp != (Buf*)dp; bp = bp->b_forw)
		{
			/* 不是要找的缓存，则继续 */
			if (bp->b_blkno != blkno || bp->b_dev != dev)
				continue;

			/*
			 * 临界区之所以要从这里开始，而不是从上面的for循环开始。
			 * 主要是因为，中断服务程序并不会去修改块设备表中的
			 * 设备buf队列(b_forw)，所以不会引起冲突。
			 */

			/* 对于访问共享的bp的字段，使用互斥锁保护 */
			/* 等待bp指向的缓存I/O完成，使用信号量实现 */

			//X86Assembly::CLI();
			if (bp->b_flags & Buf::B_BUSY)
			{
				bp->b_flags |= Buf::B_WANTED;
				//u.u_procp->Sleep((unsigned long)bp, ProcessManager::PRIBIO);
				//X86Assembly::STI();
				goto loop;
			}
			//X86Assembly::STI();
			/* 从自由队列中抽取出来 */
			this->NotAvail(bp);
			return bp;
		}
	}//end of else

	/* 访问自由队列，使用互斥锁保护 */
	/* 等待自由队列空闲，使用信号量实现 */
	//X86Assembly::CLI();
	/* 如果自由队列为空 */
	if (this->bFreeList.av_forw == &this->bFreeList)
	{
		this->bFreeList.b_flags |= Buf::B_WANTED;
		//u.u_procp->Sleep((unsigned long)&this->bFreeList, ProcessManager::PRIBIO);
		//X86Assembly::STI();
		goto loop;
	}
	//X86Assembly::STI();

		/* 取自由队列第一个空闲块 */
	bp = this->bFreeList.av_forw;
	this->NotAvail(bp);

	/* 如果该字符块是延迟写，将其异步写到磁盘上 */
	if (bp->b_flags & Buf::B_DELWRI)
	{
		bp->b_flags |= Buf::B_ASYNC;
		this->Bwrite(bp);
		goto loop;
	}
	/* 注意: 这里清除了所有其他位，只设了B_BUSY */
	bp->b_flags = Buf::B_BUSY;

	/* 从原设备队列中抽出 */
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	/* 加入新的设备队列 */
	bp->b_forw = dp->b_forw;
	bp->b_back = (Buf*)dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;

	bp->b_dev = dev;
	bp->b_blkno = blkno;
	return bp;
}

void BufferManager::Brelse(Buf* bp)
{
	/* 如果有等待使用该缓存的进程，将信号量全部释放 */

	/* 如果有正在等待自由缓存的进程，将信号量全部释放 */

	/* 关中断修改自由缓存队列 */
	bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);
	(this->bFreeList.av_back)->av_forw = bp;
	bp->av_back = this->bFreeList.av_back;
	bp->av_forw = &(this->bFreeList);
	this->bFreeList.av_back = bp;

	return;
}

void BufferManager::IOWait(Buf* bp)
{
	/* 关中断，睡眠等待bp->b_flags变为B_DONE */
}

void BufferManager::IODone(Buf* bp)
{
	bp->b_flags |= Buf::B_DONE;
	if (bp->b_flags & Buf::B_ASYNC) {
		/* 异步操作。立即释放缓存块 */
		this->Brelse(bp);
	}
	else {
		/* 清除B_WANTED标志位 */
		bp->b_flags &= ~Buf::B_WANTED;

		/* 唤醒等待进程 */
	}
	return;
}

Buf* BufferManager::Bread(short dev, int blkno)
{
	Buf* bp;
	/* 根据设备号，块号申请缓存 */
	bp = this->GetBlk(dev, blkno);
	/* 如果在设备队列中找到所需缓存，不需要I/O直接返回该缓存 */
	if (bp->b_flags & Buf::B_DONE) {
		return bp;
	}
	/* 没有找到相应缓存，构成I/O读请求块 */
	bp->b_flags |= Buf::B_READ;
	bp->b_wcount = BufferManager::BUFFER_SIZE;

	this->m_DeviceManager->GetBlockDevice(Utility::GetMajor(dev)).Strategy(bp);
	/* 同步读，等待I/O操作结束 */
	this->IOWait(bp);
	return bp;
}

Buf* BufferManager::Breada(short adev, int blkno, int rablkno)
{
	Buf* bp = nullptr;	/* 非预读字符块的缓存Buf */
	Buf* abp;			/* 预读字符块的缓存Buf */
	short major = Utility::GetMajor(adev);	/* 主设备号 */

	/* 当前字符块是否已在设备Buf队列中 */
	if (!this->InCore(adev, blkno))
	{
		bp = this->GetBlk(adev, blkno);		/* 若没找到，GetBlk()分配缓存 */

		/* 如果分配到缓存的B_DONE标志已设置，意味着在InCore()检查之后，
		 * 其它进程碰巧读取同一字符块，因而在GetBlk()中再次搜索的时候
		 * 发现该字符块已在设备Buf队列缓冲区中，本进程重用该缓存。*/
		if ((bp->b_flags & Buf::B_DONE) == 0)
		{
			/* 构成读请求块 */
			bp->b_flags |= Buf::B_READ;
			bp->b_wcount = BufferManager::BUFFER_SIZE;
			/* 驱动块设备进行I/O操作 */
			this->m_DeviceManager->GetBlockDevice(major).Strategy(bp);
		}
	}
	else
		/*UNIX V6没这条语句。加入的原因：如果当前块在缓存池中，放弃预读
		 * 这是因为，预读的价值在于利用当前块和预读块磁盘位置大概率邻近的事实，
		 * 用预读操作减少磁臂移动次数提高有效磁盘读带宽。若当前块在缓存池，磁头不一定在当前块所在的位置，
		 * 此时预读，收益有限*/
		rablkno = 0;

	/* 预读操作有2点值得注意：
	 * 1、rablkno为0，说明UNIX打算放弃预读。
	 *      这是开销与收益的权衡
	 * 2、若预读字符块在设备Buf队列中，针对预读块的操作已经成功
	 * 		这是因为：
	 * 		作为预读块，并非是进程此次读盘的目的。
	 * 		所以如果不及时释放，将使得预读块一直得不到释放。
	 * 		而将其释放它依然存在在设备队列中，如果在短时间内
	 * 		使用这一块，那么依然可以找到。
	 * */
	if (rablkno && !this->InCore(adev, rablkno))
	{
		abp = this->GetBlk(adev, rablkno);	/* 若没找到，GetBlk()分配缓存 */

		/* 检查B_DONE标志位，理由同上。 */
		if (abp->b_flags & Buf::B_DONE)
		{
			/* 预读字符块已在缓存中，释放占用的缓存。
			 * 因为进程未必后面一定会使用预读的字符块，
			 * 也就不会去释放该缓存，有可能导致缓存资源
			 * 的长时间占用。
			 */
			this->Brelse(abp);
		}
		else
		{
			/* 异步读预读字符块 */
			abp->b_flags |= (Buf::B_READ | Buf::B_ASYNC);
			abp->b_wcount = BufferManager::BUFFER_SIZE;
			/* 驱动块设备进行I/O操作 */
			this->m_DeviceManager->GetBlockDevice(major).Strategy(abp);
		}
	}

	/* bp == NULL意味着InCore()函数检查时刻，非预读块在设备队列中，
	 * 但是InCore()只是“检查”，并不“摘取”。经过一段时间执行到此处，
	 * 有可能该字符块已经重新分配它用。
	 * 因而重新调用Bread()重读字符块，Bread()中调用GetBlk()将字符块“摘取”
	 * 过来。短时间内该字符块仍在设备队列中，所以此处Bread()一般也就是将
	 * 缓存重用，而不必重新执行一次I/O读取操作。
	 */
	if (nullptr == bp)
	{
		return (this->Bread(adev, blkno));
	}

	/* InCore()函数检查时刻未找到非预读字符块，等待I/O操作完成 */
	this->IOWait(bp);
	return bp;
}

void BufferManager::Bwrite(Buf* bp)
{
	unsigned int flags;

	flags = bp->b_flags;
	bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);
	bp->b_wcount = BufferManager::BUFFER_SIZE;		/* 512字节 */

	this->m_DeviceManager->GetBlockDevice(Utility::GetMajor(bp->b_dev)).Strategy(bp);

	if ((flags & Buf::B_ASYNC) == 0)
	{
		/* 同步写，需要等待I/O操作结束 */
		this->IOWait(bp);
		this->Brelse(bp);
	}

	return;
}

void BufferManager::Bdwrite(Buf* bp)
{
	/* 置上B_DONE允许其它进程使用该磁盘块内容 */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}

void BufferManager::Bawrite(Buf* bp)
{
	/* 置上B_DONE允许其它进程使用该磁盘块内容 */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}

void BufferManager::ClrBuf(Buf* bp)
{
	int* pInt = (int*)bp->b_addr;

	/* 将缓冲区中数据清零 */
	for (unsigned int i = 0; i < BufferManager::BUFFER_SIZE / sizeof(int); i++)
	{
		pInt[i] = 0;
	}
	return;
}

void BufferManager::Bflush(short dev)
{
	Buf* bp;
	/* 注意：这里之所以要在搜索到一个块之后重新开始搜索，
	 * 因为在bwite()进入到驱动程序中时有开中断的操作，所以
	 * 等到bwrite执行完成后，CPU已处于开中断状态，所以很
	 * 有可能在这期间产生磁盘中断，使得bfreelist队列出现变化，
	 * 如果这里继续往下搜索，而不是重新开始搜索那么很可能在
	 * 操作bfreelist队列的时候出现错误。
	 */
loop:
	/* 关中断，将所有延迟写的磁盘块异步写回磁盘 */
	//X86Assembly::CLI();
	for (bp = this->bFreeList.av_forw; bp != &(this->bFreeList); bp = bp->av_forw)
	{
		/* 找出自由队列中所有延迟写的块 */
		if ((bp->b_flags & Buf::B_DELWRI) && (dev == DeviceManager::NODEV || dev == bp->b_dev))
		{
			bp->b_flags |= Buf::B_ASYNC;
			this->NotAvail(bp);
			this->Bwrite(bp);
			goto loop;
		}
	}
	//X86Assembly::STI();
	return;
}

Buf& BufferManager::GetBFreeList()
{
	return this->bFreeList;
}

void BufferManager::NotAvail(Buf* bp)
{
	/* 从自由队列中取出 */
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	/* 置B_BUSY标志 */
	bp->b_flags |= Buf::B_BUSY;
}

Buf* BufferManager::InCore(short adev, int blkno)
{
	Buf* bp;
	Devtab* dp;
	short major = 0;	/* 后续更改为根据设备号获取主设备号 */

	dp = this->m_DeviceManager->GetBlockDevice(major).d_tab;
	for (bp = dp->b_forw; bp != (Buf*)dp; bp = bp->b_forw) {
		if (blkno == bp->b_blkno && adev == bp->b_dev) {
			return bp;
		}
	}
	return nullptr;
}

#include <iostream>
#include "../inc/Kernel.h"
#include "../inc/OpenFileManager.h"

/* ========================================OpenFileTable======================================== */
/* 系统全局打开文件表对象实例的定义 */
OpenFileTable g_OpenFileTable;

OpenFileTable::OpenFileTable()
{
}

OpenFileTable::~OpenFileTable()
{
}

File *OpenFileTable::FAlloc()
{
	int fd;
	User &u = Kernel::Instance().GetUser();

	/* 在进程打开文件描述符表中获取一个空闲项 */
	fd = u.u_ofiles.AllocFreeSlot();

	if (fd < 0) /* 如果寻找空闲项失败 */
	{
		return nullptr;
	}

	for (int i = 0; i < OpenFileTable::NFILE; i++)
	{
		/* f_count==0表示该项空闲 */
		if (this->m_File[i].f_count == 0)
		{
			/* 建立描述符和File结构的勾连关系 */
			u.u_ofiles.SetF(fd, &this->m_File[i]);
			/* 增加对file结构的引用计数 */
			this->m_File[i].f_count++;
			/* 清空文件读、写位置 */
			this->m_File[i].f_offset = 0;
			return (&this->m_File[i]);
		}
	}

	std::cout << "No Free File Struct" << std::endl;
	// Diagnose::Write("No Free File Struct\n");
	u.u_error = User::_ENFILE;
	return nullptr;
}

void OpenFileTable::CloseF(File *pFile)
{
	Inode *pNode;
	// ProcessManager& procMgr = Kernel::Instance().GetProcessManager();

	/* 管道类型 */
	if (pFile->f_flag & File::FPIPE)
	{
		pNode = pFile->f_inode;
		pNode->i_mode &= ~(Inode::IREAD | Inode::IWRITE);
		(pNode + 1)->i_cv.notify_all();
		(pNode + 2)->i_cv.notify_all();
		// procMgr.WakeUpAll((unsigned long)(pNode + 1));
		// procMgr.WakeUpAll((unsigned long)(pNode + 2));
	}

	if (pFile->f_count <= 1)
	{
		/*
		 * 如果当前进程是最后一个引用该文件的进程，
		 * 对特殊块设备、字符设备文件调用相应的关闭函数
		 */
		pFile->f_inode->CloseI(pFile->f_flag & File::FWRITE);
		g_InodeTable.IPut(pFile->f_inode);
	}

	/* 引用当前File的进程数减1 */
	pFile->f_count--;
}

/* ========================================InodeTable======================================== */
/*  定义内存Inode表的实例 */
InodeTable g_InodeTable;

InodeTable::InodeTable()
{
}

InodeTable::~InodeTable()
{
}

void InodeTable::Initialize()
{
	/* 获取对g_FileSystem的引用 */
	this->m_FileSystem = &Kernel::Instance().GetFileSystem();
}

Inode *InodeTable::IGet(short dev, int inumber)
{
	Inode *pInode;
	User &u = Kernel::Instance().GetUser();

	while (true)
	{
		/* 检查指定设备dev中编号为inumber的外存Inode是否有内存拷贝 */
		int index = this->IsLoaded(dev, inumber);
		if (index >= 0) /* 找到内存拷贝 */
		{
			pInode = &(this->m_Inode[index]);
			/* 如果该内存Inode被上锁 */
			if (pInode->i_flag & Inode::ILOCK)
			{
				/* 增设IWANT标志，然后睡眠 */
				pInode->i_flag |= Inode::IWANT;

				std::unique_lock<std::mutex> i_lock(pInode->i_mtx);
				pInode->i_cv.wait(i_lock, [&pInode]
													{ return !(pInode->i_flag & Inode::ILOCK); });

				/* 回到while循环，需要重新搜索，因为该内存Inode可能已经失效 */
				continue;
			}

			/* 如果该内存Inode用于连接子文件系统，查找该Inode对应的Mount装配块 */
			if (pInode->i_flag & Inode::IMOUNT)
			{
				Mount *pMount = this->m_FileSystem->GetMount(pInode);
				if (nullptr == pMount)
				{
					/* 没有找到 */
					// Utility::Panic("No Mount Tab...");
					std::cout << "No Mount Tab..." << std::endl;
					exit(-1);
				}
				else
				{
					/* 将参数设为子文件系统设备号、根目录Inode编号 */
					dev = pMount->m_dev;
					inumber = FileSystem::ROOTINO;
					/* 回到while循环，以新dev，inumber值重新搜索 */
					continue;
				}
			}

			/*
			 * 程序执行到这里表示：内存Inode高速缓存中找到相应内存Inode，
			 * 增加其引用计数，增设ILOCK标志并返回之
			 */
			pInode->i_count++;
			pInode->i_flag |= Inode::ILOCK;
			return pInode;
		}
		else /* 没有Inode的内存拷贝，则分配一个空闲内存Inode */
		{
			pInode = this->GetFreeInode();
			/* 若内存Inode表已满，分配空闲Inode失败 */
			if (nullptr == pInode)
			{
				std::cout << "Inode Table Overflow !" << std::endl;
				return nullptr;
			}
			else /* 分配空闲Inode成功，将外存Inode读入新分配的内存Inode */
			{
				/* 设置新的设备号、外存Inode编号，增加引用计数，对索引节点上锁 */
				pInode->i_dev = dev;
				pInode->i_number = inumber;
				pInode->i_flag = Inode::ILOCK;
				pInode->i_count++;
				pInode->i_lastr = -1;

				BufferManager &bm = Kernel::Instance().GetBufferManager();
				/* 将该外存Inode读入缓冲区 */
				Buf *pBuf = bm.Bread(dev, FileSystem::INODE_ZONE_START_SECTOR + inumber / FileSystem::INODE_NUMBER_PER_SECTOR);

				/* 如果发生I/O错误 */
				if (pBuf->b_flags & Buf::B_ERROR)
				{
					/* 释放缓存 */
					bm.Brelse(pBuf);
					/* 释放占据的内存Inode */
					this->IPut(pInode);
					return nullptr;
				}

				/* 将缓冲区中的外存Inode信息拷贝到新分配的内存Inode中 */
				pInode->ICopy(pBuf, inumber);
				/* 释放缓存 */
				bm.Brelse(pBuf);
				return pInode;
			}
		}
	}
	return nullptr; /* GCC likes it! */
}

void InodeTable::IPut(Inode *pNode)
{
	/* 当前进程为引用该内存Inode的唯一进程，且准备释放该内存Inode */
	if (pNode->i_count == 1)
	{
		/*
		 * 上锁，因为在整个释放过程中可能因为磁盘操作而使得该进程睡眠，
		 * 此时有可能另一个进程会对该内存Inode进行操作，这将有可能导致错误。
		 */
		pNode->i_flag |= Inode::ILOCK;

		/* 该文件已经没有目录路径指向它 */
		if (pNode->i_nlink <= 0)
		{
			/* 释放该文件占据的数据盘块 */
			pNode->ITrunc();
			pNode->i_mode = 0;
			/* 释放对应的外存Inode */
			this->m_FileSystem->IFree(pNode->i_dev, pNode->i_number);
		}

		/* 更新外存Inode信息 */
		pNode->IUpdate(static_cast<int>(std::time(nullptr)));

		///* 解锁内存Inode，并且唤醒等待进程 */
		pNode->Prele();
		/* 清除内存Inode的所有标志位 */
		pNode->i_flag = 0;
		/* 这是内存inode空闲的标志之一，另一个是i_count == 0 */
		pNode->i_number = -1;
	}

	/* 减少内存Inode的引用计数，唤醒等待进程 */
	pNode->i_count--;
	pNode->Prele();
}

void InodeTable::UpdateInodeTable()
{
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		/*
		 * 如果Inode对象没有被上锁，即当前未被其它进程使用，可以同步到外存Inode；
		 * 并且count不等于0，count == 0意味着该内存Inode未被任何打开文件引用，无需同步。
		 */
		if ((this->m_Inode[i].i_flag & Inode::ILOCK) == 0 && this->m_Inode[i].i_count != 0)
		{
			/* 将内存Inode上锁后同步到外存Inode */
			this->m_Inode[i].i_flag |= Inode::ILOCK;
			this->m_Inode[i].IUpdate(static_cast<int>(std::time(nullptr)));

			///* 对内存Inode解锁 */
			this->m_Inode[i].Prele();
		}
	}
}

int InodeTable::IsLoaded(short dev, int inumber)
{
	/* 寻找指定外存Inode的内存拷贝 */
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		if (this->m_Inode[i].i_dev == dev && this->m_Inode[i].i_number == inumber && this->m_Inode[i].i_count != 0)
		{
			return i;
		}
	}
	return -1;
}

Inode *InodeTable::GetFreeInode()
{
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		/* 如果该内存Inode引用计数为零，则该Inode表示空闲 */
		if (this->m_Inode[i].i_count == 0)
		{
			return &(this->m_Inode[i]);
		}
	}
	return nullptr; /* 寻找失败 */
}

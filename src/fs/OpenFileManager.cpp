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

File* OpenFileTable::FAlloc()
{
	return nullptr;
}

void OpenFileTable::CloseF(File* pFile)
{
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
}

Inode* InodeTable::IGet(short dev, int inumber)
{
	return nullptr;
}

void InodeTable::IPut(Inode* pNode)
{
}

void InodeTable::UpdateInodeTable()
{
}

int InodeTable::IsLoaded(short dev, int inumber)
{
	return 0;
}

Inode* InodeTable::GetFreeInode()
{
	return nullptr;
}

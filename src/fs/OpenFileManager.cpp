#include "../inc/OpenFileManager.h"

/* ========================================OpenFileTable======================================== */
/* ϵͳȫ�ִ��ļ������ʵ���Ķ��� */
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
/*  �����ڴ�Inode���ʵ�� */
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

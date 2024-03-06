#include "../inc/Kernel.h"

Kernel Kernel::instance;

/*
* �豸�������ȫ��manager
*/
BufferManager g_BufferManager;

/*
* �ļ�ϵͳ���ȫ��manager
*/
FileManager g_FileManager;
FileSystem g_FileSystem;

/*
* ȫ��User
*/
User g_user;

Kernel::Kernel()
{
}

Kernel::~Kernel()
{
}

Kernel& Kernel::Instance()
{
	return Kernel::instance;
}

void Kernel::Initialize()
{
	InitFileSystem();
}

BufferManager& Kernel::GetBufferManager()
{
	return *(this->m_BufferManager);
}

FileManager& Kernel::GetFileManager()
{
	return *(this->m_FileManager);
}

FileSystem& Kernel::GetFileSystem()
{
	return *(this->m_FileSystem);
}

User& Kernel::GetUser()
{
	return *(this->m_user);
}

void Kernel::InitBuffer()
{
	this->m_BufferManager = &g_BufferManager;

	this->GetBufferManager().Initialize();
}

void Kernel::InitFileSystem()
{
	this->m_FileManager = &g_FileManager;
	this->m_FileSystem = &g_FileSystem;

	
}

void Kernel::InitUser()
{
}

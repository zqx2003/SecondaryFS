#include <iostream>
#include "../inc/Kernel.h"

Kernel Kernel::instance;

/*
* 设备管理相关全局manager
*/
BufferManager g_BufferManager;
DeviceManager g_DeviceManager;

/*
* 文件系统相关全局manager
*/
FileManager g_FileManager;
FileSystem g_FileSystem;

/*
* 全局User
*/
User g_User;

Kernel::Kernel()
{
	this->m_BufferManager = nullptr;
	this->m_DeviceManager = nullptr;
	this->m_FileManager = nullptr;
	this->m_FileSystem = nullptr;

	this->m_User = nullptr;	/* 模拟一个全局User */
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
	InitUser();
	InitBuffer();
	InitFileSystem();
}

BufferManager& Kernel::GetBufferManager()
{
	return *(this->m_BufferManager);
}

DeviceManager& Kernel::GetDeviceManager()
{
	return *(this->m_DeviceManager);
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
	return *(this->m_User);
}

void Kernel::InitBuffer()
{
	this->m_BufferManager = &g_BufferManager;
	this->m_DeviceManager = &g_DeviceManager;

	std::cout << "Initialize Buffer Manager..." << std::endl;
	this->GetBufferManager().Initialize();
	std::cout << "OK." << std::endl;

	std::cout << "Initialize Device Manager..." << std::endl;
	this->GetDeviceManager().Initilize();
	std::cout << "OK." << std::endl;
}

void Kernel::InitFileSystem()
{
	this->m_FileManager = &g_FileManager;
	this->m_FileSystem = &g_FileSystem;

	std::cout << "Initialize File Manager..." << std::endl;
	this->GetFileManager().Initialize();
	std::cout << "OK." << std::endl;

	std::cout << "Initialize File System..." << std::endl;
	this->GetFileSystem().Initialize();
	std::cout << "OK." << std::endl;
}

void Kernel::InitUser()
{
	this->m_User = &g_User;
}

#pragma once
#include "BufferManager.h"
#include "DeviceManager.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "User.h"

class Kernel {
public:
	Kernel();
	~Kernel();
	static Kernel& Instance();
	void Initialize();

	BufferManager& GetBufferManager();
	DeviceManager& GetDeviceManager();
	FileManager& GetFileManager();
	FileSystem& GetFileSystem();
	User& GetUser();

private:
	void InitBuffer();
	void InitFileSystem();

	void InitUser();	/* ��ʼ��һ��ģ��ȫ��User */

private:
	static Kernel instance;

	BufferManager* m_BufferManager;
	DeviceManager* m_DeviceManager;
	FileManager* m_FileManager;
	FileSystem* m_FileSystem;

	User* m_User;	/* ģ��һ��ȫ��User */
};
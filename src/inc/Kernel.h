#pragma once
#include "../inc/BufferManager.h"
#include "../inc/FileManager.h"
#include "../inc/FileSystem.h"
#include "../inc/User.h"

class Kernel {
public:
	Kernel();
	~Kernel();
	static Kernel& Instance();
	void Initialize();

	BufferManager& GetBufferManager();
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
	FileManager* m_FileManager;
	FileSystem* m_FileSystem;

	User* m_user;	/* ģ��һ��ȫ��User */
};
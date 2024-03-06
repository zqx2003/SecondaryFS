#include "../inc/FileManager.h"

/* ========================================FileManager======================================== */
void FileManager::Initialize()
{
}

/* ========================================DirectoryEntry======================================== */
DirectoryEntry::DirectoryEntry()
{
	this->m_ino = 0;
	this->m_name[0] = '\0';
}

DirectoryEntry::~DirectoryEntry()
{
}

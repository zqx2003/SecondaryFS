#pragma once

class FileManager {
public:
	void Initialize();
};

class DirectoryEntry {
public:
	static const int DIRSIZ = 28;

	int m_ino;				/* Inode�� */
	char m_name[DIRSIZ];	/* ·���� */

	DirectoryEntry();
	~DirectoryEntry();
};
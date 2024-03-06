#pragma once

class FileManager {
public:
	void Initialize();
};

class DirectoryEntry {
public:
	static const int DIRSIZ = 28;

	int m_ino;				/* InodeºÅ */
	char m_name[DIRSIZ];	/* Â·¾¶Ãû */

	DirectoryEntry();
	~DirectoryEntry();
};
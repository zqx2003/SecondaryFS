#include <iostream>
#include "../src/inc/BlockDevice.h"
#include "../src/inc/Buf.h"
#include "../src/inc/BufferManager.h"
#include "../src/inc/File.h"
#include "../src/inc/FileManager.h"
#include "../src/inc/FileSystem.h"
#include "../src/inc/INode.h"
#include "../src/inc/User.h"
using namespace std;

void test()
{
	cout << "SecondFSϵͳ�Լ�..." << endl;
	cout << "SuperBlock��СӦΪ : 1024B��ʵ��Ϊ : " << sizeof(SuperBlock) << "B." << endl;
	cout << "DiskInode��СӦΪ : 64B��ʵ��Ϊ : " << sizeof(DiskInode) << "B." << endl;
	cout << "DirectoryEntr��СӦΪ : 32B��ʵ��Ϊ : " << sizeof(DirectoryEntry) << "B." << endl;
}
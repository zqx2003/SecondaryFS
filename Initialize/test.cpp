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
	cout << "SecondFS系统自检..." << endl;
	cout << "SuperBlock大小应为 : 1024B，实际为 : " << sizeof(SuperBlock) << "B." << endl;
	cout << "DiskInode大小应为 : 64B，实际为 : " << sizeof(DiskInode) << "B." << endl;
	cout << "DirectoryEntr大小应为 : 32B，实际为 : " << sizeof(DirectoryEntry) << "B." << endl;
}
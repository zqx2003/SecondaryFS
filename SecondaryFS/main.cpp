#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "../src/inc/Kernel.h"
#include "../src/inc/Utility.h"
#include "fsapi.h"
#include "test.h"

using cmdFuncPtr = void(*)(const std::vector<std::string>&);

void cmdExec(std::vector<std::string>& cmdTokens, const std::map<std::string, cmdFuncPtr>& cmdMap)
{
	if (cmdTokens.empty()) {
		return;
	}

	auto it = cmdMap.find(cmdTokens[0]);
	if (it == cmdMap.end()) {
		std::cout << "Invalid command!" << std::endl;
		return;
	}

	cmdFuncPtr func = it->second;
	func(cmdTokens);
}

void logo(void)
{
	std::string logo =
		"________/\\\\\\________/\\\\\\_______/\\\\\\__/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_____/\\\\\\\\\\\\\\\\\\\\\\___        \n"
		" _____/\\\\\\\\/\\\\\\\\____\\///\\\\\\___/\\\\\\/__\\/\\\\\\///////////____/\\\\\\/////////\\\\\\_       \n"
		"  ___/\\\\\\//\\////\\\\\\____\\///\\\\\\\\\\\\/____\\/\\\\\\______________\\//\\\\\\______\\///__      \n"
		"   __/\\\\\\______\\//\\\\\\_____\\//\\\\\\\\______\\/\\\\\\\\\\\\\\\\\\\\\\_______\\////\\\\\\_________     \n"
		"    _\\//\\\\\\______/\\\\\\_______\\/\\\\\\\\______\/\\\\\\///////___________\\////\\\\\\______    \n"
		"     __\\///\\\\\\\\/\\\\\\\\/________/\\\\\\\\\\\\_____\\/\\\\\\_____________________\\////\\\\\\___   \n"
		"      ____\\////\\\\\\//________/\\\\\\////\\\\\\___\\/\\\\\\______________/\\\\\\______\\//\\\\\\__  \n"
		"       _______\\///\\\\\\\\\\\\___/\\\\\\/___\\///\\\\\\_\\/\\\\\\_____________\\///\\\\\\\\\\\\\\\\\\\\\\/___ \n"
		"        _________\\//////___\\///_______\\///__\\///________________\\///////////_____\n"

		;
	
	std::cout << logo << std::endl;
}

int main()
{
	logo();

	// 启动磁盘驱动
	DiskDriver::Open();

	// 操作系统初始化
	Kernel::Instance().Initialize();
	Kernel::Instance().GetFileSystem().LoadSuperBlock();
	std::cout << "Unix V6++ FileSystem Loaded......OK" << std::endl;;

	// 初始化rootDirInode和用户当前工作目录，以便NameI()正常工作
	FileManager& fileMgr = Kernel::Instance().GetFileManager();
	fileMgr.rootDirInode = g_InodeTable.IGet(DeviceManager::ROOTDEV, FileSystem::ROOTINO);
	fileMgr.rootDirInode->i_flag &= (~Inode::ILOCK);

	User& u = Kernel::Instance().GetUser();
	u.u_cdir = g_InodeTable.IGet(DeviceManager::ROOTDEV, FileSystem::ROOTINO);
	u.u_cdir->i_flag &= (~Inode::ILOCK);
	Utility::StringCopy("/", u.u_curdir);

	//test();

	//DiskDriver::Close();
	//return 0;

	const std::map<std::string, cmdFuncPtr> cmdMap = {
		{"ls",		Ls},
		{"fopen",	Fopen},
		{"fclose",	Fclose},
		{"fread",	Fread},
		{"fwrite",	Fwrite},
		{"flseek",	Flseek},
		{"fcreat",	Fcreat},
		{"mkdir",	Mkdir},
		{"fdelete", Fdelete},
		{"cd",		Cd},
		{"fin",		Fin},
		{"fout",	Fout},
		{"fdls",	Fdls},
		{"ferror",	Ferror},
		{"fclear",	Fclear},
		{"pwd",		Pwd},
		{"fsync",	Fsync},
		{"flink",	Flink},
		{"fsize",	Fsize},
		{"exit",	Exit},
		{"help",	Help},
	};

	int loop = 1;
	while (loop) {
		std::string cmd;
		std::cout << "[" << "root" << "@SecondFS " << u.u_curdir << "]# ";
		std::getline(std::cin, cmd);

		std::string token;
		std::vector<std::string> cmdTokens;
		std::istringstream sin(cmd);
		while (sin >> token) {
			cmdTokens.push_back(token);
		}

		cmdExec(cmdTokens, cmdMap);

		if (cmd == "exit") {
			loop = 0;
		}
	}

	// 关闭磁盘驱动
	DiskDriver::Close();

	return 0;
}
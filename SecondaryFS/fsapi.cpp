#include <iostream>
#include "fsapi.h"
#include "../src/inc/Kernel.h"
#include "../src/lib/inc/sys.h"

void Ls(const std::vector<std::string>& cmdTokens)
{
	/* 解析命令行参数 */
	std::string dirPath = ".";
	if (cmdTokens.size() == 2) {
		dirPath = cmdTokens[1];
	}
	else if (cmdTokens.size() > 2) {
		std::cout << "ls error: Incorrect number pf parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();
	unsigned fd = _fopen(dirPath.c_str(), FileManager::DirectorySearchMode::OPEN);

	

	std::cout << "exec ls" << std::endl;
}

void Fopen(const std::vector<std::string>& cmdTokens)
{
	// 检查参数
	if (cmdTokens.size() != 2) {
		std::cout << "fopen error: Incorrect number of parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	// 执行系统调用
	const char* file_path = cmdTokens[1].c_str();
	int fd = _fopen(file_path, FileManager::DirectorySearchMode::OPEN);
	
	if (u.u_error != User::_NOERROR) {
		std::cout << "文件[" << file_path << "]打开失败" << std::endl;
		u.u_error = User::_NOERROR;
		return;
	}
	std::cout << "fd : " << fd << std::endl;
}

void Fclose(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "fclose error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec fclose" << std::endl;
}

void Fread(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 3) {
		std::cout << "fread error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec fread" << std::endl;
}

void Fwrite(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 4) {
		std::cout << "fwrite error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec fwrite" << std::endl;
}

void Flseek(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 3) {
		std::cout << "flseek error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec flseek" << std::endl;
}

void Fcreat(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "fcreat error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec fcreate" << std::endl;
}

void Mkdir(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "mkdir error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec mkdir" << std::endl;
}

void Fdelete(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "fdelete error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec fdelete" << std::endl;
}

void Cd(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "cd error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec cd" << std::endl;
}

void Fin(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 3) {
		std::cout << "fin error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::cout << "exec fin" << std::endl;
}

void Fout(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 3) {
		std::cout << "fout error: Incorrect number of parameters!" << std::endl;
		return;
	}
	std::cout << "exec fout" << std::endl;
}

void Fdls(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 1) {
		std::cout << "exit error: Incorrect number of parameters!" << std::endl;
		return;
	}
	
	User& u = Kernel::Instance().GetUser();
	
	std::cout << "fd\toffset\ti_number" << std::endl;
	for (int fd = 0; fd < OpenFiles::NOFILES; fd++) {
		File* f = u.u_ofiles.GetF(fd);
		if (f) {
			std::cout << fd << "\t" << f->f_offset << "\t" << f->f_inode->i_number << std::endl;
		}
	}
}

void Exit(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 1) {
		std::cout << "exit error: Incorrect number of parameters!" << std::endl;
		return;
	}

	/* 关闭所有文件 */

	/* 缓存写回磁盘 */

	std::cout << "exec exit" << std::endl;
}

void Help(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() > 1) {
		std::cout << "help error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::string usage =
		"1.ls\n"
		"	Usage:ls\n"
		"	Discription:显示当前目录列表\n"
		"2.fopen\n"
		"	Usage:fopen [name]\n"
		"	Discription:打开名为name的文件，返回fd\n"
		"3.fclose\n"
		"	Usage:fclose [fd]\n"
		"	Discription:关闭文件描述符为fd的文件\n"
		"4.fread\n"
		"	Usage:fread [fd] [length]\n"
		"	Discription:从文件描述符为fd的文件中读取length个字节，输出读取的内容\n"
		"5.fwrite\n"
		"	Usage:fwrite [fd] [string] [length]\n"
		"	Discription:向文件描述符为fd的文件写入内容为string的length个字节，(不足截断，超过补0)\n"
		"6.flseek\n"
		"	Usage:flseek [fd] [offset]\n"
		"	Discription:将文件描述符为fd的文件的读写指针调整到距文件开头偏移量为offset的位置\n"
		"7.fcreat\n"
		"	Usage:fcreat [name]\n"
		"	Discription:创建名为name的普通文件\n"
		"8.mkdir\n"
		"	Usage:mkdir [name]\n"
		"	Discription:创建名为name的目录文件\n"
		"9.fdelete\n"
		"	Usage:fdelete\n"
		"	Discription:删除名为name的文件\n"
		"10.cd\n"
		"	Usage:ls\n"
		"	Discription:List files in current directory.\n"
		"11.fin\n"
		"	Usage:fin [extername] [intername]\n"
		"	Discription:将外部名为extername的文件内容存入内部名为intername的文件\n"
		"12.fout\n"
		"	Usage:fout [intername] [extername]\n"
		"	Discription:将内部名为intername的文件内容存入外部名为extername的文件\n"
		"13.exit\n"
		"	Usage:exit\n"
		"	Discription:退出系统\n"
		;
	std::cout << usage << std::endl;
}

#include <iostream>
#include <sstream>
#include <map>
#include <deque>
#include "fsapi.h"
#include "../src/inc/Kernel.h"
#include "../src/inc/Utility.h"
#include "../src/lib/inc/sys.h"

static std::map<int, std::string> fd2path;

static std::string compressPath(std::string path)
{
	if (path == "")
		return "";

	char lastch = 0;
	std::string dir;
	std::string cpath;
	std::deque<std::string> dirdq;

	auto it = path.begin();
	if (*it == '/') {
		dirdq.push_back("/");
		it++;
	}
	for (; it != path.end(); it++) {
		if (*it != '/') {
			dir.push_back(*it);
			continue;
		}

		if (dir == "" || dir == ".") {
			// nothing to do
		}
		else if (dir == "..") {
			if (dirdq.back() != "/") {
				dirdq.pop_back();
			}
		}
		else {
			dirdq.push_back(dir);
		}
		
		dir = "";
	} // end of for

	if (dir == "" || dir == ".") {
		// nothing to do
	}
	else if (dir == "..") {
		if (dirdq.back() != "/") {
			dirdq.pop_back();
		}
	}
	else {
		dirdq.push_back(dir);
	}

	while (!dirdq.empty()) {
		std::string dir = dirdq.front();
		dirdq.pop_front();
		cpath.append(dir);
		if (dir != "/" && !dirdq.empty()) {
			cpath.push_back('/');
		}
	}

	return cpath;
}

static std::string joinPath(std::string dir1, std::string dir2)
{
	std::string dir;
	dir = compressPath(dir1 + "/" + dir2);
	return dir;
}

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

	struct DirEntry {
		int m_ino;
		char m_name[DirectoryEntry::DIRSIZ];
	} dent;

	User& u = Kernel::Instance().GetUser();
	int fd = _fopen(dirPath.c_str(), File::FREAD);

	if (u.u_error != User::_NOERROR) {
		std::cout << "目录[" << dirPath << "]打开失败" << std::endl;
		u.u_error = User::_NOERROR;
		return;
	}

	int count = 0;
	while ((count = _fread(fd, (char*)&dent, sizeof(dent))) != 0) {
		if (dent.m_ino) {
			std::cout << dent.m_name << std::endl;
		}
	}

	_fclose(fd);
}

void Fopen(const std::vector<std::string>& cmdTokens)
{
	// 检查参数
	if (cmdTokens.size() != 2 && cmdTokens.size() != 3) {
		std::cout << "fopen error: Incorrect number of parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	// 执行系统调用
	const char* file_path = cmdTokens.back().c_str();
	int mode;
	if (cmdTokens.size() == 2) {
		mode = File::FREAD | File::FWRITE;
	}
	else {
		if (cmdTokens[1] == "-r") {
			mode = File::FREAD;
		}
		else if (cmdTokens[1] == "-w") {
			mode = File::FWRITE;
		}
		else if (cmdTokens[1] == "-wr") {
			mode = File::FREAD | File::FWRITE;
		}
		else {
			std::cout << "mode : [" << cmdTokens[1] << "] 打开方式不正确" << std::endl;
			return;
		}
	}

	int fd = _fopen(file_path, mode);
	
	if (u.u_error != User::_NOERROR) {
		std::cout << "文件[" << file_path << "]打开失败" << std::endl;
		u.u_error = User::_NOERROR;
		return;
	}
	std::cout << "fd : " << fd << std::endl;

	if (cmdTokens[1][0] == '/') {
		fd2path[fd] = compressPath(file_path);
	}
	else {
		fd2path[fd] = joinPath(u.u_curdir, file_path);
	}
}

void Fclose(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "fclose error: Incorrect number of parameters!" << std::endl;
		return;
	}

	int fd;
	std::stringstream sin(cmdTokens[1]);
	User& u = Kernel::Instance().GetUser();
	
	if (!(sin >> fd)) {
		std::cout << "fd : [" << cmdTokens[1] << "] 非法的文件描述符" << std::endl;
		return;
	}

	_fclose(fd);
	if (u.u_error != User::_NOERROR) {
		std::cout << "fd : [" << fd << "] 无效的文件描述符" << std::endl;
		u.u_error = User::_NOERROR;
		return;
	}

	fd2path.erase(fd);
	std::cout << "fd : [" << fd << "] 文件已关闭" << std::endl;
}

void Fread(const std::vector<std::string>& cmdTokens)
{
	// 校验参数
	if (cmdTokens.size() != 3 && cmdTokens.size() != 4) {
		std::cout << "fread error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// 读入参数
	int fd, len, hexflag = 0;
	if (cmdTokens.size() == 4) {
		if (cmdTokens[1] == "--hex") {
			hexflag = 1;
		}
		else {
			std::cout << "mode : [" << cmdTokens[1] << "] 非法的输出方式" << std::endl;
			return;
		}
	}

	std::istringstream sin(cmdTokens[cmdTokens.size() - 1]);
	User& u = Kernel::Instance().GetUser();
	if (!(sin >> fd)) {
		std::cout << "fd : [" << cmdTokens[cmdTokens.size() - 1] << "] 非法的文件描述符" << std::endl;
		return;
	}

	sin.clear();
	sin.str(cmdTokens[cmdTokens.size() - 2]);
	if (!(sin >> len)) {
		std::cout << sin.good() << std::endl;
		std::cout << "len : [" << cmdTokens[cmdTokens.size() - 2] << "] 非法的长度" << std::endl;
		return;
	}

	if (u.u_ofiles.GetF(fd) == nullptr) {
		u.u_error = User::_NOERROR;
		std::cout << "fd : [" << fd << "] 无效的文件描述符" << std::endl;
		return;
	}

	int realLen = 0;
	char buffer[Inode::BLOCK_SIZE + 1];
	std::ostringstream sout;
	
	while ((len + Inode::BLOCK_SIZE - 1) / Inode::BLOCK_SIZE) {
		// 最多读取一个缓冲区大小字节数
		int blen = _fread(fd, buffer, Utility::Min(Inode::BLOCK_SIZE, len));
		buffer[blen] = '\0';

		// 存入读取内容存入输出流对象
		sout << buffer;

		// 计算剩余读取长度
		realLen += blen;
		len -= blen;

		// 未读满一个缓冲区，或读取出错，说明文件已结束
		if (blen < Inode::BLOCK_SIZE || u.u_error != User::_NOERROR) {
			break;
		}
	}

	std::cout << "实际读取字节" << realLen << "B" << std::endl;
	if (u.u_error != User::_NOERROR) {
		std::cout << "读取过程出错" << std::endl;
	}
	std::cout << "/**********开始**********/" << std::endl;
	std::cout << sout.str() << std::endl;
	std::cout << "/**********结束**********/" << std::endl;
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
	// 校验参数
	if (cmdTokens.size() != 1) {
		std::cout << "fdls error: Incorrect number of parameters!" << std::endl;
		return;
	}
	
	// 获取内核对象
	User& u = Kernel::Instance().GetUser();
	
	// 输出文件描述符信息
	std::cout << "fd\toffset\tinode\tpath" << std::endl;
	for (int fd = 0; fd < OpenFiles::NOFILES; fd++) {
		File* f = u.u_ofiles.GetF(fd);
		if (f) {
			std::cout << fd << "\t" << f->f_offset << "\t" << f->f_inode->i_number << "\t" << fd2path[fd] << std::endl;
		}
	}

	// 遍历到空的File*会出错，此处清除避免影响正常功能
	u.u_error = User::_NOERROR;
}

void Ferror(const std::vector<std::string>& cmdTokens)
{
	// 校验参数
	if (cmdTokens.size() != 1) {
		std::cout << "ferror error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// 获取内核对象
	User& u = Kernel::Instance().GetUser();

	// 输出错误代码
	std::cout << "[" << u.u_error << "] " << (u.u_error == User::_NOERROR ? "运行正常" : "运行错误") << std::endl;
}

void Fclear(const std::vector<std::string>& cmdTokens)
{
	// 校验参数
	if (cmdTokens.size() != 1) {
		std::cout << "fclear error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// 获取内核对象
	User& u = Kernel::Instance().GetUser();

	// 清除错误代码
	std::cout << (u.u_error == User::_NOERROR ? "重复操作" : "错误码已清除") << std::endl;
	u.u_error = User::_NOERROR;
}

void Pwd(const std::vector<std::string>& cmdTokens)
{
	// 校验参数
	if (cmdTokens.size() != 1) {
		std::cout << "pwd error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// 获取内核对象
	User& u = Kernel::Instance().GetUser();

	// 输出当前目录
	std::cout << u.u_curdir << std::endl;
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
		"	Usage:ls [-l] [path]\n"
		"	Discription:显示当前目录列表\n"
		"2.fopen\n"
		"	Usage:fopen [-r/w/wr] [name]\n"
		"	Discription:以读、写、读写方式打开名为name的文件，返回fd\n"
		"3.fclose\n"
		"	Usage:fclose [fd]\n"
		"	Discription:关闭文件描述符为fd的文件\n"
		"4.fread\n"
		"	Usage:fread [--hex] [fd] [length]\n"
		"	Discription:从文件描述符为fd的文件中读取length个字节，以字符或16进制方式输出读取的内容\n"
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
		"13.fdls\n"
		"	Usage:fdls\n"
		"	Discription:查看当前所有文件描述符\n"
		"14.ferror\n"
		"	Usage:ferror\n"
		"	Discription:查看错误代码\n"
		"15.fclear\n"
		"	Usage:fclear\n"
		"	Discription:清除错误代码\n"
		"16.pwd\n"
		"	Usage:pwd\n"
		"	Discription:输出当前目录\n"
		"17.exit\n"
		"	Usage:exit\n"
		"	Discription:退出系统\n"
		;
	std::cout << usage << std::endl;
}

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
	/* ���������в��� */
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
		std::cout << "Ŀ¼[" << dirPath << "]��ʧ��" << std::endl;
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
	// ������
	if (cmdTokens.size() != 2 && cmdTokens.size() != 3) {
		std::cout << "fopen error: Incorrect number of parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	// ִ��ϵͳ����
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
			std::cout << "mode : [" << cmdTokens[1] << "] �򿪷�ʽ����ȷ" << std::endl;
			return;
		}
	}

	int fd = _fopen(file_path, mode);
	
	if (u.u_error != User::_NOERROR) {
		std::cout << "�ļ�[" << file_path << "]��ʧ��" << std::endl;
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
		std::cout << "fd : [" << cmdTokens[1] << "] �Ƿ����ļ�������" << std::endl;
		return;
	}

	_fclose(fd);
	if (u.u_error != User::_NOERROR) {
		std::cout << "fd : [" << fd << "] ��Ч���ļ�������" << std::endl;
		u.u_error = User::_NOERROR;
		return;
	}

	fd2path.erase(fd);
	std::cout << "fd : [" << fd << "] �ļ��ѹر�" << std::endl;
}

void Fread(const std::vector<std::string>& cmdTokens)
{
	// У�����
	if (cmdTokens.size() != 3 && cmdTokens.size() != 4) {
		std::cout << "fread error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// �������
	int fd, len, hexflag = 0;
	if (cmdTokens.size() == 4) {
		if (cmdTokens[1] == "--hex") {
			hexflag = 1;
		}
		else {
			std::cout << "mode : [" << cmdTokens[1] << "] �Ƿ��������ʽ" << std::endl;
			return;
		}
	}

	std::istringstream sin(cmdTokens[cmdTokens.size() - 1]);
	User& u = Kernel::Instance().GetUser();
	if (!(sin >> fd)) {
		std::cout << "fd : [" << cmdTokens[cmdTokens.size() - 1] << "] �Ƿ����ļ�������" << std::endl;
		return;
	}

	sin.clear();
	sin.str(cmdTokens[cmdTokens.size() - 2]);
	if (!(sin >> len)) {
		std::cout << sin.good() << std::endl;
		std::cout << "len : [" << cmdTokens[cmdTokens.size() - 2] << "] �Ƿ��ĳ���" << std::endl;
		return;
	}

	if (u.u_ofiles.GetF(fd) == nullptr) {
		u.u_error = User::_NOERROR;
		std::cout << "fd : [" << fd << "] ��Ч���ļ�������" << std::endl;
		return;
	}

	int realLen = 0;
	char buffer[Inode::BLOCK_SIZE + 1];
	std::ostringstream sout;
	
	while ((len + Inode::BLOCK_SIZE - 1) / Inode::BLOCK_SIZE) {
		// ����ȡһ����������С�ֽ���
		int blen = _fread(fd, buffer, Utility::Min(Inode::BLOCK_SIZE, len));
		buffer[blen] = '\0';

		// �����ȡ���ݴ������������
		sout << buffer;

		// ����ʣ���ȡ����
		realLen += blen;
		len -= blen;

		// δ����һ�������������ȡ����˵���ļ��ѽ���
		if (blen < Inode::BLOCK_SIZE || u.u_error != User::_NOERROR) {
			break;
		}
	}

	std::cout << "ʵ�ʶ�ȡ�ֽ�" << realLen << "B" << std::endl;
	if (u.u_error != User::_NOERROR) {
		std::cout << "��ȡ���̳���" << std::endl;
	}
	std::cout << "/**********��ʼ**********/" << std::endl;
	std::cout << sout.str() << std::endl;
	std::cout << "/**********����**********/" << std::endl;
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
	// У�����
	if (cmdTokens.size() != 1) {
		std::cout << "fdls error: Incorrect number of parameters!" << std::endl;
		return;
	}
	
	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();
	
	// ����ļ���������Ϣ
	std::cout << "fd\toffset\tinode\tpath" << std::endl;
	for (int fd = 0; fd < OpenFiles::NOFILES; fd++) {
		File* f = u.u_ofiles.GetF(fd);
		if (f) {
			std::cout << fd << "\t" << f->f_offset << "\t" << f->f_inode->i_number << "\t" << fd2path[fd] << std::endl;
		}
	}

	// �������յ�File*������˴��������Ӱ����������
	u.u_error = User::_NOERROR;
}

void Ferror(const std::vector<std::string>& cmdTokens)
{
	// У�����
	if (cmdTokens.size() != 1) {
		std::cout << "ferror error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();

	// ����������
	std::cout << "[" << u.u_error << "] " << (u.u_error == User::_NOERROR ? "��������" : "���д���") << std::endl;
}

void Fclear(const std::vector<std::string>& cmdTokens)
{
	// У�����
	if (cmdTokens.size() != 1) {
		std::cout << "fclear error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();

	// ����������
	std::cout << (u.u_error == User::_NOERROR ? "�ظ�����" : "�����������") << std::endl;
	u.u_error = User::_NOERROR;
}

void Pwd(const std::vector<std::string>& cmdTokens)
{
	// У�����
	if (cmdTokens.size() != 1) {
		std::cout << "pwd error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();

	// �����ǰĿ¼
	std::cout << u.u_curdir << std::endl;
}

void Exit(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 1) {
		std::cout << "exit error: Incorrect number of parameters!" << std::endl;
		return;
	}

	/* �ر������ļ� */

	/* ����д�ش��� */

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
		"	Discription:��ʾ��ǰĿ¼�б�\n"
		"2.fopen\n"
		"	Usage:fopen [-r/w/wr] [name]\n"
		"	Discription:�Զ���д����д��ʽ����Ϊname���ļ�������fd\n"
		"3.fclose\n"
		"	Usage:fclose [fd]\n"
		"	Discription:�ر��ļ�������Ϊfd���ļ�\n"
		"4.fread\n"
		"	Usage:fread [--hex] [fd] [length]\n"
		"	Discription:���ļ�������Ϊfd���ļ��ж�ȡlength���ֽڣ����ַ���16���Ʒ�ʽ�����ȡ������\n"
		"5.fwrite\n"
		"	Usage:fwrite [fd] [string] [length]\n"
		"	Discription:���ļ�������Ϊfd���ļ�д������Ϊstring��length���ֽڣ�(����ضϣ�������0)\n"
		"6.flseek\n"
		"	Usage:flseek [fd] [offset]\n"
		"	Discription:���ļ�������Ϊfd���ļ��Ķ�дָ����������ļ���ͷƫ����Ϊoffset��λ��\n"
		"7.fcreat\n"
		"	Usage:fcreat [name]\n"
		"	Discription:������Ϊname����ͨ�ļ�\n"
		"8.mkdir\n"
		"	Usage:mkdir [name]\n"
		"	Discription:������Ϊname��Ŀ¼�ļ�\n"
		"9.fdelete\n"
		"	Usage:fdelete\n"
		"	Discription:ɾ����Ϊname���ļ�\n"
		"10.cd\n"
		"	Usage:ls\n"
		"	Discription:List files in current directory.\n"
		"11.fin\n"
		"	Usage:fin [extername] [intername]\n"
		"	Discription:���ⲿ��Ϊextername���ļ����ݴ����ڲ���Ϊintername���ļ�\n"
		"12.fout\n"
		"	Usage:fout [intername] [extername]\n"
		"	Discription:���ڲ���Ϊintername���ļ����ݴ����ⲿ��Ϊextername���ļ�\n"
		"13.fdls\n"
		"	Usage:fdls\n"
		"	Discription:�鿴��ǰ�����ļ�������\n"
		"14.ferror\n"
		"	Usage:ferror\n"
		"	Discription:�鿴�������\n"
		"15.fclear\n"
		"	Usage:fclear\n"
		"	Discription:����������\n"
		"16.pwd\n"
		"	Usage:pwd\n"
		"	Discription:�����ǰĿ¼\n"
		"17.exit\n"
		"	Usage:exit\n"
		"	Discription:�˳�ϵͳ\n"
		;
	std::cout << usage << std::endl;
}

#include <iostream>
#include <sstream>
#include <fstream>
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
		std::cout << "error : [" << u.u_error << "] Ŀ¼[" << dirPath << "]��ʧ��" << std::endl;
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

	if (cmdTokens[1][0] == '/') {	// ����·��
		fd2path[fd] = compressPath(file_path);
	}
	else {							// ���·��
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

	std::istringstream sin(cmdTokens.end()[-2]);
	User& u = Kernel::Instance().GetUser();
	if (!(sin >> fd)) {
		std::cout << "fd : [" << cmdTokens.end()[-2] << "] �Ƿ����ļ�������" << std::endl;
		return;
	}

	sin.clear();
	sin.str(cmdTokens.end()[-1]);
	if (!(sin >> len)) {
		std::cout << sin.good() << std::endl;
		std::cout << "len : [" << cmdTokens.end()[-1] << "] �Ƿ��ĳ���" << std::endl;
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
		int count = _fread(fd, buffer, Utility::Min(Inode::BLOCK_SIZE, len));
		buffer[count] = '\0';

		// �����ȡ���ݴ������������
		sout << buffer;

		// ����ʣ���ȡ����
		realLen += count;
		len -= count;

		// δ����һ�������������ȡ����˵���ļ��ѽ���
		if (count < Inode::BLOCK_SIZE || u.u_error != User::_NOERROR) {
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
	// У�����
	if (cmdTokens.size() != 4) {
		std::cout << "fwrite error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// �������
	int fd, len;
	std::string content(cmdTokens.end()[-2]);

	std::istringstream sin(cmdTokens.end()[-3]);
	User& u = Kernel::Instance().GetUser();
	if (!(sin >> fd)) {
		std::cout << "fd : [" << cmdTokens.end()[-3] << "] �Ƿ����ļ�������" << std::endl;
		return;
	}

	sin.clear();
	sin.str(cmdTokens.end()[-1]);
	if (!(sin >> len)) {
		std::cout << sin.good() << std::endl;
		std::cout << "len : [" << cmdTokens.end()[-1] << "] �Ƿ��ĳ���" << std::endl;
		return;
	}

	if (u.u_ofiles.GetF(fd) == nullptr) {
		u.u_error = User::_NOERROR;
		std::cout << "fd : [" << fd << "] ��Ч���ļ�������" << std::endl;
		return;
	}

	content.resize(len, '\0');

	int realLen = 0;
	const char* buffer = content.c_str();

	while ((len + Inode::BLOCK_SIZE - 1) / Inode::BLOCK_SIZE) {
		// ����ȡһ����������С�ֽ���
		int count = _fwrite(fd, buffer, Utility::Min(Inode::BLOCK_SIZE, len));

		// ����ʣ���ȡ����
		realLen += count;
		len -= count;
		buffer += count;

		// δ����һ�������������ȡ����˵���ļ��ѽ���
		if (count < Inode::BLOCK_SIZE || u.u_error != User::_NOERROR) {
			break;
		}
	}

	std::cout << "ʵ��д���ֽ�" << realLen << "B" << std::endl;
	if (u.u_error != User::_NOERROR) {
		std::cout << "д����̳���" << std::endl;
	}
}

void Flseek(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 3 && cmdTokens.size() != 4) {
		std::cout << "flseek error: Incorrect number of parameters!" << std::endl;
		return;
	}

	int fd, position, whence = 0;	// Ĭ�������ʼλ���ƶ�
	if (cmdTokens.size() == 4) {
		if (cmdTokens[1] == "--beg") {
			whence = 0;
		}
		else if (cmdTokens[1] == "--cur") {
			whence = 1;
		}
		else if (cmdTokens[1] == "--end") {
			whence = 2;
		}
		else if (cmdTokens[1] == "--beg_block") {
			whence = 3;
		}
		else if (cmdTokens[1] == "--cur_block") {
			whence = 4;
		}
		else if (cmdTokens[1] == "--end_block") {
			whence = 5;
		}
		else {
			std::cout << "whence : [" << cmdTokens[1] << "] �Ƿ����ƶ�" << std::endl;
			return;
		}
	}

	std::istringstream sin(cmdTokens.end()[-2]);
	if (!(sin >> fd)) {
		std::cout << "fd : [" << cmdTokens.end()[-2] << "] �Ƿ����ļ�������" << std::endl;
		return;
	}

	sin.clear();
	sin.str(cmdTokens.end()[-1]);
	if (!(sin >> position)) {
		std::cout << "position : [" << cmdTokens.end()[-1] << "] �Ƿ���ƫ����" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	File* pFile = u.u_ofiles.GetF(fd);
	if (pFile == nullptr) {
		u.u_error = User::_NOERROR;
		std::cout << "fd : [" << fd << "] ��Ч���ļ�������" << std::endl;
		return;
	}

	_flseek(fd, position, whence);

	if (pFile->f_offset < 0) {
		std::cout << "offset : [" << pFile->f_offset << "] ������Χ����С����ʼƫ����������Ϊ0" << std::endl;
		pFile->f_offset = 0;
	}
	if (pFile->f_offset > pFile->f_inode->i_size) {
		std::cout << "offset : [" << pFile->f_offset << "] ������Χ��������ĩβƫ����������Ϊ�ļ���С" << std::endl;
		pFile->f_offset = pFile->f_inode->i_size;
	}

	std::cout << "fd : [" << fd << "] ��ǰ�����ļ� : [" << fd2path[fd] << "]" << std::endl;
	std::cout << "��Ӧ�ļ�ָ���޸�Ϊ : [" << pFile->f_offset << "]" << std::endl;
}

void Fcreat(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "fcreat error: Incorrect number of parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	// Ĭ���Զ�д��ʽ��
	int mode = Inode::IREAD | Inode::IWRITE;
	std::string file_path = cmdTokens.end()[-1];

	int fd = _fcreat(file_path.c_str(), mode);

	if (u.u_error != User::_NOERROR) {
		std::cout << "�ļ�[" << file_path << "]����ʧ��" << std::endl;
		u.u_error = User::_NOERROR;
		return;
	}
	std::cout << "fd : " << fd << std::endl;

	if (cmdTokens[1][0] == '/') {	// ����·��
		fd2path[fd] = compressPath(file_path);
	}
	else {							// ���·��
		fd2path[fd] = joinPath(u.u_curdir, file_path);
	}
}

void Mkdir(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "mkdir error: Incorrect number of parameters!" << std::endl;
		return;
	}

	struct DirEntry {
		int m_ino;
		char m_name[DirectoryEntry::DIRSIZ];
	} dent = { 0, "" };

	User& u = Kernel::Instance().GetUser();
	FileManager& fileMgr = Kernel::Instance().GetFileManager();

	std::string dir_path = cmdTokens.end()[-1];

	_mkdir(dir_path.c_str());
	
	// �ֶ���.��..д��Ŀ¼�ļ���
	int fd = _fopen(dir_path.c_str(), File::FREAD);
	int pfd = _fopen(joinPath(dir_path, "/..").c_str(), File::FREAD);

	Inode* pFile = u.u_ofiles.GetF(fd)->f_inode;
	Inode* ppFile = u.u_ofiles.GetF(pfd)->f_inode;

	dent.m_ino = pFile->i_number;
	Utility::StringCopy(".", dent.m_name);
	u.u_IOParam.m_Count = sizeof(DirEntry);
	u.u_IOParam.m_Base = (char*)&dent;

	pFile->WriteI();

	dent.m_ino = ppFile->i_number;
	Utility::StringCopy("..", dent.m_name);
	u.u_IOParam.m_Count = sizeof(DirEntry);
	u.u_IOParam.m_Base = (char*)&dent;

	pFile->WriteI();

	_fclose(fd);
	_fclose(pfd);

	if (u.u_error != User::_NOERROR) {
		if (u.u_error == User::_EEXIST) {
			std::cout << "dir : [" << cmdTokens.end()[-1] << "] Ŀ¼�Ѵ���" << std::endl;
		}
		else {
			std::cout << "dir : [" << cmdTokens.end()[-1] << "] Ŀ¼����ʧ��" << std::endl;
		}
		
		u.u_error = User::_NOERROR;
		return;
	}
}

void Fdelete(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 2) {
		std::cout << "fdelete error: Incorrect number of parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	std::string path = cmdTokens.end()[-1];

	_fdelete(path.c_str());

	if (u.u_error == User::_NOERROR) {
		if (u.u_error == User::_ENOENT) {
			std::cout << "error : [" << u.u_error << "] �ļ�[" << path << "]������" << std::endl;
		}
		else {
			std::cout << "error : [" << u.u_error << "] �ļ�[" << path << "]ɾ��ʧ��" << std::endl;
		}
		return;
	}

	std::cout << "�ļ�[" << path << "]ɾ���ɹ�" << std::endl;
}

void Cd(const std::vector<std::string>& cmdTokens)
{
	// У�����
	if (cmdTokens.size() != 2) {
		std::cout << "cd error: Incorrect number of parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	std::string dstDir;
	if (cmdTokens.end()[-1][0] == '/') {
		dstDir = compressPath(cmdTokens.end()[-1]);
	}
	else {
		dstDir = joinPath(u.u_curdir, cmdTokens.end()[-1]);
	}

	// ִ��ϵͳ����
	_cd(dstDir.c_str());

	// ������
	if (u.u_error != User::_NOERROR) {
		
		if (u.u_error == User::_ENOTDIR) {
			std::cout << "dir : [" << cmdTokens.end()[-1] << "] ����һ��Ŀ¼" << std::endl;
		}
		else {
			std::cout << "dir : [" << cmdTokens.end()[-1] << "] ��Ŀ¼������" << std::endl;
		}
		
		u.u_error = User::_NOERROR;
	}
}

void Fin(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 3) {
		std::cout << "fin error: Incorrect number of parameters!" << std::endl;
		return;
	}

	std::string extraname = cmdTokens.end()[-2];
	std::string intraname = cmdTokens.end()[-1];

	std::ifstream fin(extraname, std::ios::in | std::ios::binary);
	if (!fin.is_open()) {
		std::cout << "�ⲿ�ļ�[" << extraname << "]��ʧ��" << std::endl;
		return;
	}

	char buffer[Inode::BLOCK_SIZE];
	User& u = Kernel::Instance().GetUser();

	int fd = _fcreat(intraname.c_str(), File::FWRITE);
	if (u.u_error != User::_NOERROR) {
		std::cout << "�ļ�[" << intraname << "]��ʧ��" << std::endl;
		u.u_error = User::_NOERROR;
		fin.close();
		return;
	}

	fin.seekg(0, std::ios::end);
	int len = (int)fin.tellg();
	fin.seekg(0, std::ios::beg);
	while (len) {
		// ���ⲿ�ļ�����
		fin.read(buffer, Utility::Min(Inode::BLOCK_SIZE, len));

		// ���ڲ��ļ�д��
		_fwrite(fd, buffer, (int)fin.gcount());

		// ����ʣ���ȡ�ַ���
		len -= (int)fin.gcount();

		if (u.u_error != User::_NOERROR) {
			break;
		}
	}

	// �ر��ļ�
	fin.close();
	_fclose(fd);

	if (u.u_error != User::_NOERROR) {
		std::cout << "�ⲿ�ļ�[" << extraname << "]д���ڲ��ļ�[" << intraname << "]ʧ��" << std::endl;
		return;
	}
	std::cout << "�ⲿ�ļ�[" << extraname << "]д���ڲ��ļ�[" << intraname << "]�ɹ�" << std::endl;
}

void Fout(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 3) {
		std::cout << "fout error: Incorrect number of parameters!" << std::endl;
		return;
	}
	
	std::string intraname = cmdTokens.end()[-2];
	std::string extraname = cmdTokens.end()[-1];

	std::ofstream fout(extraname, std::ios::out | std::ios::binary);
	if (!fout.is_open()) {
		std::cout << "�ⲿ�ļ�[" << extraname << "]��ʧ��" << std::endl;
		return;
	}

	char buffer[Inode::BLOCK_SIZE];
	User& u = Kernel::Instance().GetUser();

	int fd = _fopen(intraname.c_str(), File::FREAD);
	if (u.u_error != User::_NOERROR) {
		std::cout << "�ļ�[" << intraname << "]��ʧ��" << std::endl;
		u.u_error = User::_NOERROR;
		fout.close();
		return;
	}

	_flseek(fd, 0, 2);
	int len = u.u_ofiles.GetF(fd)->f_offset;
	_flseek(fd, 0, 0);
	while (len) {
		// ���ڲ��ļ�����
		int count = _fread(fd, buffer, Utility::Min(Inode::BLOCK_SIZE, len));

		// ���ⲿ�ļ�д��
		fout.write(buffer, count);

		// ����ʣ���ȡ�ַ���
		len -= count;

		if (u.u_error != User::_NOERROR) {
			break;
		}
	}

	// �ر��ļ�
	fout.close();
	_fclose(fd);

	if (u.u_error != User::_NOERROR) {
		std::cout << "�ڲ��ļ�[" << intraname << "]д���ⲿ�ļ�[" << extraname << "]ʧ��" << std::endl;
		return;
	}
	std::cout << "�ڲ��ļ�[" << intraname << "]д���ⲿ�ļ�[" << extraname << "]�ɹ�" << std::endl;
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

void Fsync(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 1) {
		std::cout << "exit error: Incorrect number of parameters!" << std::endl;
		return;
	}

	/* ͬ���ļ�ϵͳ��Inode, superblockд�ػ��棬�����ӳ�д�Ļ���д�ش��� */
	_fsync();

	std::cout << "��ͬ���ļ�ϵͳ������" << std::endl;
}

void Exit(const std::vector<std::string>& cmdTokens)
{
	if (cmdTokens.size() != 1) {
		std::cout << "exit error: Incorrect number of parameters!" << std::endl;
		return;
	}

	// ��ȡ�ں˶���
	User& u = Kernel::Instance().GetUser();

	/* �ر������ļ� */
	for (int fd = 0; fd < OpenFiles::NOFILES; fd++) {
		File* f = u.u_ofiles.GetF(fd);
		if (f) {
			_fclose(fd);
			std::cout << "ϵͳ�Զ��ر��ļ�[" << fd2path[fd] << "]" << std::endl;
			fd2path.erase(fd);
		}
	}

	// �������յ�File*������˴��������Ӱ����������
	u.u_error = User::_NOERROR;

	/* Inode, superblockд�ػ��棬�����ӳ�д�Ļ���д�ش��� */
	_fsync();

	std::cout << "��ͬ���ļ�ϵͳ������" << std::endl;
	std::cout << "���˳��ļ�ϵͳ����ӭ�ٴη���" << std::endl;
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
		"	Discription:�Զ���д����д��ʽ����Ϊname���ļ�������fd��Ĭ��Ϊ��д��ʽ��\n"
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
		"	Usage:flseek [--beg/cur/end/beg_block/cur_block/end_block] [fd] [offset]\n"
		"	Discription:���ļ�������Ϊfd���ļ��Ķ�дָ����������ļ���ͷ����ǰ��ĩβƫ����Ϊoffset��λ�ã�Ĭ��Ϊ��ͷ����block�����ƶ�1�ֽڱ���ƶ�512�ֽ�\n"
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
		"17.fsync\n"
		"	Usage:fsync\n"
		"	Discription:ͬ���ļ�ϵͳ������\n"
		"18.exit\n"
		"	Usage:exit\n"
		"	Discription:�˳�ϵͳ\n"
		;
	std::cout << usage << std::endl;
}

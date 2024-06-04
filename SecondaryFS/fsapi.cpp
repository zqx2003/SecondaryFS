#include <iostream>
#include "fsapi.h"
#include "../src/inc/Kernel.h"
#include "../src/lib/inc/sys.h"

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

	User& u = Kernel::Instance().GetUser();
	unsigned fd = _fopen(dirPath.c_str(), FileManager::DirectorySearchMode::OPEN);

	

	std::cout << "exec ls" << std::endl;
}

void Fopen(const std::vector<std::string>& cmdTokens)
{
	// ������
	if (cmdTokens.size() != 2) {
		std::cout << "fopen error: Incorrect number of parameters!" << std::endl;
		return;
	}

	User& u = Kernel::Instance().GetUser();

	// ִ��ϵͳ����
	const char* file_path = cmdTokens[1].c_str();
	int fd = _fopen(file_path, FileManager::DirectorySearchMode::OPEN);
	
	if (u.u_error != User::_NOERROR) {
		std::cout << "�ļ�[" << file_path << "]��ʧ��" << std::endl;
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
		"	Usage:ls\n"
		"	Discription:��ʾ��ǰĿ¼�б�\n"
		"2.fopen\n"
		"	Usage:fopen [name]\n"
		"	Discription:����Ϊname���ļ�������fd\n"
		"3.fclose\n"
		"	Usage:fclose [fd]\n"
		"	Discription:�ر��ļ�������Ϊfd���ļ�\n"
		"4.fread\n"
		"	Usage:fread [fd] [length]\n"
		"	Discription:���ļ�������Ϊfd���ļ��ж�ȡlength���ֽڣ������ȡ������\n"
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
		"13.exit\n"
		"	Usage:exit\n"
		"	Discription:�˳�ϵͳ\n"
		;
	std::cout << usage << std::endl;
}

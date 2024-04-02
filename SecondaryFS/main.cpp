#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "../src/inc/Kernel.h"
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

int main()
{
	//DiskDriver::Open();

	//Kernel::Instance().Initialize();
	//Kernel::Instance().GetFileSystem().LoadSuperBlock();
	//std::cout << "Unix V6++ FileSystem Loaded......OK" << std::endl;;

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
		{"exit",	Exit},
		{"help",	Help},
	};

	int loop = 1;
	while (loop) {
		std::string cmd;
		std::cout << "[" << "user" << "@SecondFS " << "~]# ";
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

	return 0;
}
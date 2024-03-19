#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "fsapi.h"
#include "test.h"

using cmdFuncPtr = void(*)(const std::vector<std::string>&);

void cmdExec(std::vector<std::string>& cmdTokens, const std::map<std::string, cmdFuncPtr>& cmdMap)
{
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
	//test();
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
		std::cout << "[" << "@" << "]# ";
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
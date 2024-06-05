#pragma once
#include <string>
#include <vector>
struct option {
	char* shortopt;
	char* longopt;
	int hasarg;
	int val;
};

void Ls		(const std::vector<std::string>& cmdTokens);
void Fopen	(const std::vector<std::string>& cmdTokens);
void Fclose	(const std::vector<std::string>& cmdTokens);
void Fread	(const std::vector<std::string>& cmdTokens);
void Fwrite	(const std::vector<std::string>& cmdTokens);
void Flseek	(const std::vector<std::string>& cmdTokens);
void Fcreat	(const std::vector<std::string>& cmdTokens);
void Mkdir	(const std::vector<std::string>& cmdTokens);
void Fdelete(const std::vector<std::string>& cmdTokens);
void Cd		(const std::vector<std::string>& cmdTokens);
void Fin	(const std::vector<std::string>& cmdTokens);
void Fout	(const std::vector<std::string>& cmdTokens);
void Fdls	(const std::vector<std::string>& cmdTokens);
void Ferror	(const std::vector<std::string>& cmdTokens);
void Fclear	(const std::vector<std::string>& cmdTokens);
void Pwd	(const std::vector<std::string>& cmdTokens);
void Exit	(const std::vector<std::string>& cmdTokens);
void Help	(const std::vector<std::string>& cmdTokens);
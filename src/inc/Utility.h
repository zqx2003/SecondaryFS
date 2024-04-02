#pragma once

class Utility {
public:
	static void StringCopy(char* src, char* dst);

	static int StringLength(char* pString);

	/* 提取参数dev中的主设备号major，高8bit */
	static short GetMajor(const short dev);
	/* 提取参数dev中的从设备号minor，第8bit */
	static short GetMinor(const short dev);

	/* 以src为源地址，dst为目的地址，复制count个双字 */
	static void DWordCopy(int* src, int* dst, int count);

	static int Min(int a, int b);

	static int Max(int a, int b);

	/* 用于在读、写文件时，高速缓存与用户指定目标内存区域之间数据传送 */
	static void IOMove(char* from, char* to, int count);
};
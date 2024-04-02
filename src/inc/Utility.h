#pragma once

class Utility {
public:
	static void StringCopy(char* src, char* dst);

	static int StringLength(char* pString);

	/* ��ȡ����dev�е����豸��major����8bit */
	static short GetMajor(const short dev);
	/* ��ȡ����dev�еĴ��豸��minor����8bit */
	static short GetMinor(const short dev);

	/* ��srcΪԴ��ַ��dstΪĿ�ĵ�ַ������count��˫�� */
	static void DWordCopy(int* src, int* dst, int count);

	static int Min(int a, int b);

	static int Max(int a, int b);

	/* �����ڶ���д�ļ�ʱ�����ٻ������û�ָ��Ŀ���ڴ�����֮�����ݴ��� */
	static void IOMove(char* from, char* to, int count);
};
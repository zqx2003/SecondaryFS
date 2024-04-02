#include "../inc/Utility.h"

void Utility::StringCopy(char* src, char* dst)
{
	while ((*dst++ = *src++) != 0);
}

int Utility::StringLength(char* pString)
{
	int length = 0;
	char* pChar = pString;

	while (*pChar++)
	{
		length++;
	}

	/* ·µ»Ø×Ö·û´®³¤¶È */
	return length;
}

short Utility::GetMajor(const short dev)
{
	return dev >> 8;
}

short Utility::GetMinor(const short dev)
{
	return dev & 0x00FF;
}

void Utility::DWordCopy(int* src, int* dst, int count)
{
	while (count--)
	{
		*dst++ = *src++;
	}
	return;
}

int Utility::Min(int a, int b)
{
	return a < b ? a : b;
}

int Utility::Max(int a, int b)
{
	return a > b ? a : b;
}

void Utility::IOMove(char* from, char* to, int count)
{
	while (count--)
	{
		*to++ = *from++;
	}
	return;
}

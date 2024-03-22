#include "../inc/Utility.h"

short Utility::GetMajor(const short dev)
{
	return dev >> 8;
}

short Utility::GetMinor(const short dev)
{
	return dev & 0x00FF;
}

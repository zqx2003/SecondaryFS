#pragma once

class Utility {
public:
	/* ��ȡ����dev�е����豸��major����8bit */
	static short GetMajor(const short dev);
	/* ��ȡ����dev�еĴ��豸��minor����8bit */
	static short GetMinor(const short dev);
};
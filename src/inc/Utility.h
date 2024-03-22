#pragma once

class Utility {
public:
	/* 提取参数dev中的主设备号major，高8bit */
	static short GetMajor(const short dev);
	/* 提取参数dev中的从设备号minor，第8bit */
	static short GetMinor(const short dev);
};
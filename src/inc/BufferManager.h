#pragma once
#include "../inc/Buf.h"
#include "../inc/DeviceManager.h"

class BufferManager {
public:
	static const int NBUF = 15;
	static const int BUFFER_SIZE = 512;

public:
	void Initialize();

private:
	Buf bFreeList;					/* 自由缓存队列控制块 */
	Buf m_Buf[NBUF];				/* 缓存控制块数组 */
	char Buffer[NBUF][BUFFER_SIZE];	/* 缓冲区数组 */

	DeviceManager* m_DeviceManager;	/* 指向设别管理模块的全局对象 */
};
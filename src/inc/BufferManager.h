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
	Buf bFreeList;					/* ���ɻ�����п��ƿ� */
	Buf m_Buf[NBUF];				/* ������ƿ����� */
	char Buffer[NBUF][BUFFER_SIZE];	/* ���������� */

	DeviceManager* m_DeviceManager;	/* ָ��������ģ���ȫ�ֶ��� */
};
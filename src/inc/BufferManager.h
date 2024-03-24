#pragma once
#include <mutex>
#include "../inc/BlockDevice.h"
#include "../inc/Buf.h"
#include "../inc/DeviceManager.h"
#include "../inc/DiskDriver.h"

class BufferManager {
	friend class BlockDevice;
	friend class DiskBlockDevice;

	friend class DiskDriver;

public:
	static const int NBUF = 15;			/* ������ƿ顢������������ */
	static const int BUFFER_SIZE = 512;	/* ��������С�����ֽ�Ϊ��λ */

public:
	BufferManager();
	~BufferManager();

	void Initialize();					/* ������ƶ��еĳ�ʼ�� */

	Buf* GetBlk(short dev, int blkno);	/* ����һ�黺�棬���ڶ�д�豸dev�ϵ��ַ���blkno */
	void Brelse(Buf* bp);				/* �ͷŻ�����ƿ�buf */
	void IOWait(Buf* bp);				/* ͬ����ʽI/O���ȴ�I/O���� */
	void IODone(Buf* bp);				/* I/O�������ƺ��� */

	Buf* Bread(short dev, int blkno);	/* ��һ�����̿� */
	Buf* Breada(short adev, int blkno, int rablkno);	/* ��һ�����̿飬����Ԥ����ʽ��
														 * adevΪ�������豸�š�blknoΪĿ����̿��߼���ţ�ͬ����ʽ��blkno��
														 * rablknoΪԤ�����̿��߼���ţ��첽��ʽ��rablkno�� */

	void Bwrite(Buf* bp);				/* дһ�����̿� */
	void Bdwrite(Buf* bp);				/* �ӳ�д���̿� */
	void Bawrite(Buf* bp);				/* �첽д���̿� */

	void ClrBuf(Buf* bp);				/* ��ջ��������� */
	void Bflush(short dev);				/* ��dev�ƶ��豸�������ӳ�д�Ļ���ȫ����������� */

	Buf& GetBFreeList();				/* ��ȡ���ɻ�����п��ƿ�Buf�������� */

	int GetBufId(Buf* bp);				/* ��ȡ������ID */

private:
	void NotAvail(Buf* bp);				/* �����ɶ�����ժ��ָ���Ļ�����ƿ�buf */
	Buf* InCore(short adev, int blkno);	/* ���ָ���ַ����Ƿ����ڻ����� */

private:
	Buf bFreeList;						/* ���ɻ�����п��ƿ� */
	Buf m_Buf[NBUF];					/* ������ƿ����� */
	char Buffer[NBUF][BUFFER_SIZE];		/* ���������� */

	DeviceManager* m_DeviceManager;		/* ָ��������ģ���ȫ�ֶ��� */
	static std::recursive_mutex mtx_av;			/* ά�����ɶ��У�I/O������е��� */
};
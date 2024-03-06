#pragma once

class SuperBlock {
public:
	static const int NFREE = 100;
	static const int NINODE = 100;

	int s_isize;		/* Inode���ߴ磬���ݿ����� */
	int s_fsize;		/* �������ݿ����� */

	int s_nfree;		/* ֱ�ӹ���Ŀ����̿����� */
	int s_free[NFREE];	/* ֱ�ӹ���Ŀ����̿������� */
	int s_flock;		/* �����̿��������������δʹ�� */

	int s_ninode;		/* ֱ�ӹ���Ŀ������Inode���� */
	int s_inode[NINODE];	/* ֱ�ӹ���Ŀ������Inode������ */
	int ilock;			/* ����Inode���������δʹ�� */

	int s_fmod;			/* �ڴ�SuperBlock�������޸ı�־��ж��ʱ��д�ش��� */
	int s_ronly;		/* ���ļ�ϵͳֻ�ܶ�������δʹ�� */
	int s_time;			/* ���һ�θ���ʱ�䣬��δʹ�� */
	int padding[47];	/* ���SuperBlock���С����1024�ֽڣ�ռ��2������ */
};

class FileSystem {
public:
	static const int INODE_NUMBER_PER_SECTOR = 8;

	void Initialize();
};
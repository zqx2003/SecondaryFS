#pragma once
#include <mutex>
#include <condition_variable>

class Buf {
public:
	enum BufFlag {	/* b_flags�б�־λ */
		B_WRITE = 0x1,		/* д�������������е���Ϣд��Ӳ����ȥ */
		B_READ = 0x2,		/* �����������̶�ȡ��Ϣ�������� */
		B_DONE = 0x4,		/* I/O�������� */
		B_ERROR = 0x8,		/* I/O��������ֹ */
		B_BUSY = 0x10,		/* ��Ӧ��������ʹ���� */
		B_WANTED = 0x20,	/* �н������ڵȴ�ʹ�ø�buf�������Դ����B_BUSY��־ʱ��Ҫ�������ֽ��� */
		B_ASYNC = 0x40,		/* �첽I/O������Ҫ�ȴ������ */
		B_DELWRI = 0x80		/* �ӳ�д������Ӧ����Ҫ��������ʱ���ٽ�������д����Ӧ���豸�� */
	};

public:
	unsigned b_flags;		/* ������ƿ��־λ */
	
	int		padding;		/* 4�ֽ���䣬ʹ��b_forw��b_back��Buf���Devtab���е��ֶ�˳��һ�£�����ǿת�س��� */

	/* ������ƿ鹴��ָ�� */
	Buf*	b_forw;
	Buf*	b_back;
	Buf*	av_forw;
	Buf*	av_back;

	short	b_dev;			/* �����豸�ţ���8λ�����豸�ţ���8λ�Ǵ��豸�� */
	int		b_wcount;		/* �贫�͵��ֽ�������δʹ�� */
	char*	b_addr;			/* ָ��û�����ƿ�������Ļ������׵�ַ */
	int		b_blkno;		/* �����߼���� */
	int		b_error;		/* I/O����ʱ��Ϣ����δʹ�� */
	int		b_resid;		/* I/O����ʱ��δ���͵��ֽ�������δʹ�� */

	std::mutex	b_mtx;				/* ����Ļ����� */
	std::condition_variable b_cv;	/* ������������� */
};
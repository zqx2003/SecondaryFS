#pragma once

class Inode {
public:
	/* static const member */
	static const unsigned int IALLOC = 0x8000;		/* �ļ���ʹ�� */
	static const unsigned int IFMT = 0x6000;		/* �ļ��������� */
	static const unsigned int IFDIR = 0x4000;		/* �ļ����ͣ�Ŀ¼�ļ� */
	static const unsigned int IFCHR = 0x2000;		/* �ַ��豸���������ļ� */
	static const unsigned int IFBLK = 0x6000;		/* ���豸���������ļ���Ϊ0��ʾ���������ļ� */
	static const unsigned int ILARG = 0x1000;		/* �ļ��������ͣ����ͻ�����ļ� */
	static const unsigned int ISUID = 0x800;		/* ִ��ʱ�ļ�ʱ���û�����Ч�û�ID�޸�Ϊ�ļ������ߵ�User ID */
	static const unsigned int ISGID = 0x400;		/* ִ��ʱ�ļ�ʱ���û�����Ч��ID�޸�Ϊ�ļ������ߵ�Group ID */
	static const unsigned int ISVTX = 0x200;		/* ʹ�ú���Ȼλ�ڽ������ϵ����Ķ� */
	static const unsigned int IREAD = 0x100;		/* ���ļ��Ķ�Ȩ�� */
	static const unsigned int IWRITE = 0x80;		/* ���ļ���дȨ�� */
	static const unsigned int IEXEC = 0x40;			/* ���ļ���ִ��Ȩ�� */
	static const unsigned int IRWXU = (IREAD | IWRITE | IEXEC);		/* �ļ������ļ��Ķ���д��ִ��Ȩ�� */
	static const unsigned int IRWXG = ((IRWXU) >> 3);			/* �ļ���ͬ���û����ļ��Ķ���д��ִ��Ȩ�� */
	static const unsigned int IRWXO = ((IRWXU) >> 6);			/* �����û����ļ��Ķ���д��ִ��Ȩ�� */

	static const int BLOCK_SIZE = 512;		/* �ļ��߼����С: 512�ֽ� */
	static const int ADDRESS_PER_INDEX_BLOCK = BLOCK_SIZE / sizeof(int);	/* ÿ�����������(��������)�����������̿�� */

	static const int SMALL_FILE_BLOCK = 6;	/* С���ļ���ֱ������������Ѱַ���߼���� */
	static const int LARGE_FILE_BLOCK = 128 * 2 + 6;	/* �����ļ�����һ�μ������������Ѱַ���߼���� */
	static const int HUGE_FILE_BLOCK = 128 * 128 * 2 + 128 * 2 + 6;	/* �����ļ��������μ����������Ѱַ�ļ��߼���� */

	static const int PIPSIZ = SMALL_FILE_BLOCK * BLOCK_SIZE;

	static const int BNUM = 10;

	unsigned	i_flag;			/* ״̬��־λ����δʹ�� */
	unsigned	i_mode;			/* �ļ�������ʽ��Ϣ */

	int			i_count;		/* ���ü��� */
	int			i_nlink;		/* �ļ����Ӽ����������ļ���Ŀ¼���в�ͬ·���������� */

	short		i_dev;			/* ���inode���ڴ洢�豸���豸�ţ���δʹ�� */
	int			i_number;		/* ���inode���еı�ţ���δʹ�� */

	short		i_uid;			/* �ļ��������û��� */
	short		i_gid;			/* �ļ����������ʶ������δʹ�� */

	int			i_size;			/* �ļ���С���ֽ�Ϊ��λ */
	int			i_addr[BNUM];	/* �����ļ��߼���ź�������ת���Ļ��������� */

	int			i_lastr;		/* ������һ�ζ�ȡ�ļ����߼���ţ������ж��Ƿ���ҪԤ������δʹ�� */
};

class DiskInode {
public:
	static const int BNUM = 10;

	unsigned	d_mode;			/* �ļ����ͺͷ��ʿ���λ */
	int			d_nlink;		/* Ӳ���Ӽ����������ļ���Ŀ¼���в�ͬ·���������� */
	short		d_uid;			/* �ļ�������uid */
	short		d_gid;			/* �ļ�������gid����δʹ�� */
	
	int			d_size;			/* �ļ����ȣ��ֽ�Ϊ��λ */
	int			d_addr[BNUM];	/* ��ַӳ����Ǽ��߼���������֮���ӳ���ϵ */

	int			d_atime;		/* ������ʱ�䣬��δʹ�� */
	int			d_mtime;		/* ����޸�ʱ�䣬��δʹ�� */
};
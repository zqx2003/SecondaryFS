#pragma once
#include "File.h"
#include "INode.h"
#include "FileManager.h"

/*
 * @comment ������Unixv6�� struct user�ṹ��Ӧ�����ֻ�ı�
 * ���������޸ĳ�Ա�ṹ���֣������������͵Ķ�Ӧ��ϵ����:
 */
class User
{
public:
	/* ϵͳ������س�Ա */
	unsigned int u_ar0;			/* ָ�����ջ�ֳ���������EAX�Ĵ���
								��ŵ�ջ��Ԫ�����ֶδ�Ÿ�ջ��Ԫ�ĵ�ַ��
								��V6��r0���ϵͳ���õķ���ֵ���û�����
								x86ƽ̨��ʹ��EAX��ŷ���ֵ�����u.u_ar0[R0] */

	int u_arg[5];				/* ��ŵ�ǰϵͳ���ò��� */
	char* u_dirp;				/* ϵͳ���ò���(һ������Pathname)��ָ�� */

	/* �ļ�ϵͳ��س�Ա */
	Inode* u_cdir;		/* ָ��ǰĿ¼��Inodeָ�� */
	Inode* u_pdir;		/* ָ��Ŀ¼��Inodeָ�� */

	DirectoryEntry u_dent;					/* ��ǰĿ¼��Ŀ¼�� */
	char u_dbuf[DirectoryEntry::DIRSIZ];	/* ��ǰ·������ */
	char u_curdir[128];						/* ��ǰ����Ŀ¼����·�� */

	/* ���̵��û���ʶ */
	short u_uid;		/* ��Ч�û�ID */
	short u_gid;		/* ��Ч��ID */
	short u_ruid;		/* ��ʵ�û�ID */
	short u_rgid;		/* ��ʵ��ID */

	/* �ļ�ϵͳ��س�Ա */
	OpenFiles u_ofiles;		/* ���̴��ļ������������ */

	/* �ļ�I/O���� */
	IOParameter u_IOParam;	/* ��¼��ǰ����д�ļ���ƫ�������û�Ŀ�������ʣ���ֽ������� */

	/* Member Functions */
public:
	/* ����ϵͳ���ò���uid������Ч�û�ID����ʵ�û�ID�������û�ID(p_uid) */
	void Setuid();

	/* ��ȡ�û�ID����16����Ϊ��ʵ�û�ID(u_ruid)����16����Ϊ��Ч�û�ID(u_uid) */
	void Getuid();

	/* ����ϵͳ���ò���gid������Ч��ID����ʵ��ID */
	void Setgid();

	/* ��ȡ��ID, ��16����Ϊ��ʵ��ID(u_rgid)����16����Ϊ��Ч��ID(u_gid) */
	void Getgid();

	/* ��ȡ��ǰ�û�����Ŀ¼ */
	void Pwd();

	/* ��鵱ǰ�û��Ƿ��ǳ����û� */
	bool SUser();
};

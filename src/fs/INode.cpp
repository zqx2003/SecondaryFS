#include "../inc/BufferManager.h"
#include "../inc/DeviceManager.h"
#include "../inc/INode.h"
#include "../inc/Kernel.h"
#include "../inc/Utility.h"

/* ========================================Inode======================================== */
int Inode::rablock = 0;

Inode::Inode()
{
	/* ���Inode�����е����� */
// this->Clean(); 
/* ȥ��this->Clean();�����ɣ�
 * Inode::Clean()�ض�����IAlloc()������·���DiskInode��ԭ�����ݣ�
 * �����ļ���Ϣ��Clean()�����в�Ӧ�����i_dev, i_number, i_flag, i_count,
 * ���������ڴ�Inode����DiskInode�����ľ��ļ���Ϣ����Inode�๹�캯����Ҫ
 * �����ʼ��Ϊ��Чֵ��
 */

 /* ��Inode����ĳ�Ա������ʼ��Ϊ��Чֵ */
	this->i_flag = 0;
	this->i_mode = 0;
	this->i_count = 0;
	this->i_nlink = 0;
	this->i_dev = -1;
	this->i_number = -1;
	this->i_uid = -1;
	this->i_gid = -1;
	this->i_size = 0;
	this->i_lastr = -1;
	for (int i = 0; i < 10; i++)
	{
		this->i_addr[i] = 0;
	}
}

Inode::~Inode()
{
}

void Inode::ReadI()
{
	int lbn;	/* �ļ��߼���� */
	int bn;		/* lbn��Ӧ�������̿�� */
	int offset;	/* ��ǰ�ַ�������ʼ����λ�� */
	int nbytes;	/* �������û�Ŀ�����ֽ����� */
	short dev;
	Buf* pBuf;
	User& u = Kernel::Instance().GetUser();
	BufferManager& bufMgr = Kernel::Instance().GetBufferManager();
	DeviceManager& devMgr = Kernel::Instance().GetDeviceManager();

	if (0 == u.u_IOParam.m_Count)
	{
		/* ��Ҫ���ֽ���Ϊ�㣬�򷵻� */
		return;
	}

	this->i_flag |= Inode::IACC;

	///* ������ַ��豸�ļ� ���������������*/
	//if ((this->i_mode & Inode::IFMT) == Inode::IFCHR)
	//{
	//	short major = Utility::GetMajor(this->i_addr[0]);

	//	devMgr.GetCharDevice(major).Read(this->i_addr[0]);
	//	return;
	//}

	/* һ��һ���ַ���ض�������ȫ�����ݣ�ֱ�������ļ�β */
	while (u.u_IOParam.m_Count != 0)
	{
		lbn = bn = u.u_IOParam.m_Offset / Inode::BLOCK_SIZE;
		offset = u.u_IOParam.m_Offset % Inode::BLOCK_SIZE;
		/* ���͵��û������ֽ�������ȡ�������ʣ���ֽ����뵱ǰ�ַ�������Ч�ֽ�����Сֵ */
		nbytes = Utility::Min(Inode::BLOCK_SIZE - offset /* ������Ч�ֽ��� */, u.u_IOParam.m_Count);

		if ((this->i_mode & Inode::IFMT) != Inode::IFBLK)
		{	/* �������������豸�ļ� */

			int remain = this->i_size - u.u_IOParam.m_Offset;
			/* ����Ѷ��������ļ���β */
			if (remain <= 0)
			{
				return;
			}
			/* ���͵��ֽ�������ȡ����ʣ���ļ��ĳ��� */
			nbytes = Utility::Min(nbytes, remain);

			/* ���߼����lbnת���������̿��bn ��Bmap������Inode::rablock����UNIX��Ϊ��ȡԤ����Ŀ���̫��ʱ��
			 * �����Ԥ������ʱ Inode::rablock ֵΪ 0��
			 * */
			if ((bn = this->Bmap(lbn)) == 0)
			{
				return;
			}
			dev = this->i_dev;
		}
		else	/* �����������豸�ļ� */
		{
			dev = this->i_addr[0];	/* ������豸�ļ�i_addr[0]�д�ŵ����豸�� */
			Inode::rablock = bn + 1;
		}

		if (this->i_lastr + 1 == lbn)	/* �����˳����������Ԥ�� */
		{
			/* ����ǰ�飬��Ԥ����һ�� */
			pBuf = bufMgr.Breada(dev, bn, Inode::rablock);
		}
		else
		{
			pBuf = bufMgr.Bread(dev, bn);
		}
		/* ��¼�����ȡ�ַ�����߼���� */
		this->i_lastr = lbn;

		/* ������������ʼ��λ�� */
		char* start = pBuf->b_addr + offset;

		/* ������: �ӻ������������û�Ŀ����
		 * i386оƬ��ͬһ��ҳ��ӳ���û��ռ���ں˿ռ䣬��һ��Ӳ���ϵĲ��� ʹ��i386��ʵ�� iomove����
		 * ��PDP-11Ҫ�������*/
		Utility::IOMove(start, u.u_IOParam.m_Base, nbytes);

		/* �ô����ֽ���nbytes���¶�дλ�� */
		u.u_IOParam.m_Base += nbytes;
		u.u_IOParam.m_Offset += nbytes;
		u.u_IOParam.m_Count -= nbytes;

		bufMgr.Brelse(pBuf);	/* ʹ���껺�棬�ͷŸ���Դ */
	}
}

void Inode::WriteI()
{
}

int Inode::Bmap(int lbn)
{
	return 0;
}

void Inode::OpenI(int mode)
{
}

void Inode::CloseI(int mode)
{
}

void Inode::IUpdate(int time)
{
}

void Inode::ITrunc()
{
}

void Inode::Prele()
{
}

void Inode::Plock()
{
}

void Inode::NFrele()
{
}

void Inode::NFlock()
{
}

void Inode::Clean()
{
}

void Inode::ICopy(Buf* bp, int inumber)
{
}

/* ========================================DiskInode======================================== */
DiskInode::DiskInode()
{
	/*
 * ���DiskInodeû�й��캯�����ᷢ�����½��Ѳ���Ĵ���
 * DiskInode��Ϊ�ֲ�����ռ�ݺ���Stack Frame�е��ڴ�ռ䣬����
 * ��οռ�û�б���ȷ��ʼ�����Ծɱ�������ǰջ���ݣ����ڲ�����
 * DiskInode�����ֶζ��ᱻ���£���DiskInodeд�ص�������ʱ������
 * ����ǰջ����һͬд�أ�����д�ؽ������Ī����������ݡ�
 */
	this->d_mode = 0;
	this->d_nlink = 0;
	this->d_uid = -1;
	this->d_gid = -1;
	this->d_size = 0;
	for (int i = 0; i < 10; i++)
	{
		this->d_addr[i] = 0;
	}
	this->d_atime = 0;
	this->d_mtime = 0;
}

DiskInode::~DiskInode()
{
}

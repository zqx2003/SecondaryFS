#include "../inc/File.h"
#include "../inc/Kernel.h"

/* ========================================File======================================== */
File::File()
{
	this->f_count = 0;
	this->f_flag = 0;
	this->f_offset = 0;
	this->f_inode = nullptr;
}

File::~File()
{
}

/* ========================================OpenFiles======================================== */
OpenFiles::OpenFiles()
{
	for (int i = 0; i < OpenFiles::NOFILES; i++) {
		this->ProcessOpenFileTable[i] = nullptr;
	}
}

OpenFiles::~OpenFiles()
{
}

int OpenFiles::AllocFreeSlot()
{
	int i;
	User& u = Kernel::Instance().GetUser();

	for (i = 0; i < OpenFiles::NOFILES; i++)
	{
		/* ���̴��ļ������������ҵ�������򷵻�֮ */
		if (nullptr == this->ProcessOpenFileTable[i])
		{
			/* ���ú���ջ�ֳ��������е�EAX�Ĵ�����ֵ����ϵͳ���÷���ֵ */
			u.u_ar0 = i;
			return i;
		}
	}

	u.u_ar0 = -1;   /* Open1����Ҫһ����־�������ļ��ṹ����ʧ��ʱ�����Ի���ϵͳ��Դ*/
	u.u_error = User::_EMFILE;
	return -1;
}

File* OpenFiles::GetF(int fd)
{
	File* pFile;
	User& u = Kernel::Instance().GetUser();

	/* ������ļ���������ֵ�����˷�Χ */
	if (fd < 0 || fd >= OpenFiles::NOFILES)
	{
		u.u_error = User::_EBADF;
		return nullptr;
	}

	pFile = this->ProcessOpenFileTable[fd];
	if (nullptr == pFile)
	{
		u.u_error = User::_EBADF;
	}

	return pFile;	/* ��ʹpFile==NULLҲ���������ɵ���GetF�ĺ������жϷ���ֵ */
}

void OpenFiles::SetF(int fd, File* pFile)
{
	if (fd < 0 || fd >= OpenFiles::NOFILES)
	{
		return;
	}
	/* ���̴��ļ�������ָ��ϵͳ���ļ�������Ӧ��File�ṹ */
	this->ProcessOpenFileTable[fd] = pFile;
}

/* ========================================IOParameter======================================== */
IOParameter::IOParameter()
{
	this->m_Base = 0;
	this->m_Count = 0;
	this->m_Offset = 0;
}

IOParameter::~IOParameter()
{
}

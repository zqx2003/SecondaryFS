#include "../inc/Kernel.h"
#include "../inc/User.h"
#include "../inc/Utility.h"

void User::Setuid()
{
	short uid = this->u_arg[0];

	if (this->u_ruid == uid || this->SUser())
	{
		this->u_uid = uid;
		this->u_ruid = uid;
	}
}

void User::Getuid()
{
	unsigned int uid;

	uid = (this->u_uid << 16);
	uid |= (this->u_ruid & 0xFF);
	this->u_ar0 = uid;
}

void User::Setgid()
{
	short gid = this->u_arg[0];

	if (this->u_rgid == gid || this->SUser())
	{
		this->u_gid = gid;
		this->u_rgid = gid;
	}
}

void User::Getgid()
{
	unsigned int gid;

	gid = (this->u_gid << 16);
	gid |= (this->u_rgid & 0xFF);
	this->u_ar0 = gid;
}

void User::Pwd()
{
	Utility::StringCopy(this->u_curdir, this->u_dirp);
}

bool User::SUser()
{
	if (0 == this->u_uid)
	{
		return true;
	}
}

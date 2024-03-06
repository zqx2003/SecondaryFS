#include "../inc/sys.h"
#include "../../inc/DiskDriver.h"

void Format(const char* disk_file_path, int isize, int fsize)
{
	DiskDriver::DiskFormat(disk_file_path, isize, fsize);
}
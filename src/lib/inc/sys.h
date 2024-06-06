#pragma once

void Format(const char* disk_file_path, int isize, int fsize);

int	 _fopen		(const char* file_path, int mode);
void _fclose	(int fd);
int  _fread		(int fd, char* buffer, int length);
int  _fwrite	(int fd, const char* buffer, int length);
void _flseek	(int fd, int position, int whence);
int  _fcreat	(const char* file_path, int mode);
void _mkdir		(const char* dir_path);
void _fdelete	(const char* path);
void _cd		(const char* dir_path);
void _fsync		(void);
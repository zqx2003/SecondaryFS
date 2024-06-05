#pragma once

void Format(const char* disk_file_path, int isize, int fsize);

void _ls		(const char* dir_path);
int _fopen		(const char* file_path, int mode);
void _fclose	(int fd);
int _fread		(int fd, char* buffer, int length);
void _fwrite	();
void _flseek	(int fd, int position, int whence);
void _fcreat	();
void _mkdir		();
void _fdelete	();
void _cd		(const char* dir_path);
void _fin		();
void _fout		();
#pragma once

void Format(const char* disk_file_path, int isize, int fsize);

void _ls		(const char* dir_path);
int _fopen		(const char* file_path, int mode);
void _fclose	(int fd);
int _fread		(int fd, char* buffer, int length);
void _fwrite	();
void _flseek	();
void _fcreat	();
void _mkdir		();
void _fdelete	();
void _cd		();
void _fin		();
void _fout		();
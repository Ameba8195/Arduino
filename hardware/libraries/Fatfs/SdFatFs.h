#ifndef _FATFS_H_
#define _FATFS_H_

#include "SdFatFile.h"

#define MAX_FILENAME_LEN 256

class SdFatFs
{
public:
    SdFatFs();
    ~SdFatFs();
		
    int begin();
    int end();


    char *getRootPath();
    int readDir(char *path, char *result_buf, unsigned int bufsize);

    int mkdir(char *absolute_path);
    int rm(char *absolute_path);

    unsigned char isDir(char *absolute_path);
    unsigned char isFile(char *absolute_path);

    int getLastModTime(char *absolute_path, uint16_t *year, uint16_t *month, uint16_t *date, uint16_t *hour, uint16_t *minute, uint16_t *second);
    int setLastModTime(char *absolute_path, uint16_t year, uint16_t month, uint16_t date, uint16_t hour, uint16_t minute, uint16_t second);

    SdFatFile open(char *absolute_path);

    int status();

private:
    void *m_fs;
    char logical_drv[4];
    int drv_num;

    int getAttribute(char *absolute_path, unsigned char *attr);
};

#endif

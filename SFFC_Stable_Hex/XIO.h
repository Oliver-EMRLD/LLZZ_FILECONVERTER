#ifndef XIO_H_INCLUDED
#define XIO_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>




char hexTab[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F', };

int xopen(char *path, int mode)
    {
    int fd = 0;
    switch (mode)
        {
        case 0:
            fd = open64(path, O_RDONLY);
        case 1:
            fd = open64(path, O_WRONLY);
        case 2:
            fd = open64(path, O_RDWR);
        }

    if(fd == -1)
        {
        perror("OPEN");
        exit(8);
        }

    return fd;
    }



int xread(int fd, void *buffer, int sz)
    {
    int debug = -1000;
    debug = read(fd, buffer, sz);
    if(debug < 0)
        {
        perror("read");
        exit(8);
        }
    return debug;
    }



int xwrite(int fd, void *buffer, int sz)
    {
    int debug = -1000;
    debug = write(fd, buffer, sz);
    if(debug < 0)
        {
        perror("write");
        exit(8);
        }
    return debug;
    }

void chartohex(unsigned char n, char *hex)
    {
    *hex++ = hexTab[n >> 4];
    *hex++ = hexTab[n & 0xf];

    }


#endif // XIO_H_INCLUDED


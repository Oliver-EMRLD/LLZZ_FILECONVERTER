#ifndef XIO_H_INCLUDED
#define XIO_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>




char hexTab[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f', };

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

int denigma(char *buffer, char *output, int sz)
    {
    int x = 0;
    for(x = 0 ; x < sz ; x++)
        {
        sprintf(output[x*2], "%X", buffer[x]);
        }
    return x;
    }

void byteToHex(unsigned char n, char *hex)
/* convert byte to hexidecimal characters. 0 <= n <= 255. */
    {
    *hex++ = hexTab[n >> 4];
    *hex++ = hexTab[n & 0xf];
    }


#endif // XIO_H_INCLUDED


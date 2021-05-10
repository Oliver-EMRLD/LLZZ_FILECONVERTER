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
            fd = open64(path, O_RDONLY, 0777);
        case 1:
            fd = open64(path, O_CREAT | O_WRONLY, 0777);
        case 2:
            fd = open64(path, O_RDWR, 0777);
        //case 3:
         //   fd = open64(path, O_WRONLY, 0777);

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



void dectobyte(short int num, char *bytes[])
    {
    unsigned char bt[2];

    memcpy(&bt, &num, 2);
    bytes[0] = bt[1];
    bytes[1] = bt[0];
    }




void checkarg( char *arg[], int argsize , int fromwich , char *arg1 , char *arg2 )
    {
    int debug = -1;

    for(int x = fromwich ; x < argsize ; x++)
        {
        debug *= memcmp(arg[x], arg1, strlen(arg1));
        debug *= memcmp(arg[x], arg1, strlen(arg2));
        if(debug != 0)
            {
            printf("Illegal Argument!!!\n");
            exit(2);
            }
        }
    }


unsigned short int dectobin(unsigned short int k)
    {
    return (k == 0 || k == 1 ? k : ((k % 2) + 10 * dectobin(k / 2)));
    }


#endif // XIO_H_INCLUDED











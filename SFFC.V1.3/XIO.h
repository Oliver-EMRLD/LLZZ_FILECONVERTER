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

    /*
    Stores variables in file in the current working directory
    */
void savevar(void *var, size_t sz, size_t howmany, char *path)
    {
    FILE *hlFile;

    hlFile = fopen(path, "wb");
    if(hlFile == NULL)
        {
        printf("COULD NOT OPEN PATH ::::: %s\nABORTING\n", path);
        exit(1);
        }
    fwrite(var, sz, howmany+1, hlFile);
    fclose(hlFile);
    }

    /*
    Reads the variables saved by savevar
    */
void readvar(void *var, char *path, size_t varcount, size_t varsize)
    {
    FILE *hlFile;

    hlFile = fopen(path, "rb");
    if(hlFile == NULL)
        {
        printf("COULD NOT OPEN PATH ::::: %s\nABORTING\n", path);
        exit(1);
        }
    fread(var, varsize, varcount, hlFile);
    fclose(hlFile);
    }

    /*
    Cuts of the last part of a path FE.: /home/hello/x --> /home/hello/
    */
int cutname(char *path, char *dest)
    {
    for(int x = strlen(path); x >= 0 ; x--)
        {
        if(path[x] == '/')
            {
            strncat(dest, path, x+1);
            //printf("%s\n", dest);
            return 0;
        }
            }

    printf("couldent find / char in pathname\n");
    return 1;
    }

void getname(char *path, char *dest)
    {
    int sz = 0;
    printf("VAR : %s\n", path);
    for( int x = strlen(path) ; x >= 0 ; x-- )
        {
        //printf("%i\n", x);
        if(path[x] == '/')
            {
            sz = strlen(path)-x;
            strncat(dest, path+x, sz);
            //printf("%s\n", *dest);
            break;
            }
        }
    }




unsigned long hex2int(char *a, unsigned int len)
    {
    int i;
    unsigned long val = 0;

    for(i=0;i<len;i++)
       if(a[i] <= 57)
        val += (a[i]-48)*(1<<(4*(len-1-i)));
       else
        val += (a[i]-55)*(1<<(4*(len-1-i)));

    return val;
    }


unsigned char unhex(unsigned char upperdigit, unsigned char lowerdigit)
    {
    unsigned char unhexvalue = 0;
    unsigned char mask1 = 0;
    unsigned char mask2 = 0;
    unsigned char uprptr = upperdigit;
    unsigned char lwrotr = lowerdigit;

    switch (uprptr)
        {
        case '0':

            mask1 = 0x00;
            break;

        case '1':

            mask1 = 0x10;
            break;

        case '2':

            mask1 = 0x20;
            break;

        case '3':

            mask1 = 0x30;
            break;

        case '4':

            mask1 = 0x40;
            break;

        case '5':

            mask1 = 0x50;
            break;

        case '6':

            mask1 = 0x60;
            break;

        case '7':

            mask1 = 0x70;
            break;

        case '8':

            mask1 = 0x80;
            break;

        case '9':

            mask1 = 0x90;
            break;

        case 'A':

            mask1 = 0xA0;
            break;

        case 'B':

            mask1 = 0xB0;
            break;

        case 'C':

            mask1 = 0xC0;
            break;

        case 'D':

            mask1 = 0xD0;
            break;

        case 'E':

            mask1 = 0xE0;
            break;

        case 'F':

            mask1 = 0xF0;
            break;
        }






    switch (lwrotr)
        {
        case '0':

            mask2 = 0x00;
            break;

        case '1':

            mask2 = 0x01;
            break;

        case '2':

            mask2 = 0x02;
            break;

        case '3':

            mask2 = 0x03;
            break;

        case '4':

            mask2 = 0x04;
            break;

        case '5':

            mask2 = 0x05;
            break;

        case '6':

            mask2 = 0x06;
            break;

        case '7':

            mask2 = 0x07;
            break;

        case '8':

            mask2 = 0x08;
            break;

        case '9':

            mask2 = 0x09;
            break;

        case 'A':

            mask2 = 0x0A;
            break;

        case 'B':

            mask2 = 0x0B;
            break;

        case 'C':

            mask2 = 0x0C;
            break;

        case 'D':

            mask2 = 0x0D;
            break;

        case 'E':

            mask2 = 0x0E;
            break;

        case 'F':

            mask2 = 0x0F;
            break;
        }

    unhexvalue = unhexvalue | mask1;
    unhexvalue = unhexvalue | mask2;
    return unhexvalue;
    }



#endif // XIO_H_INCLUDED











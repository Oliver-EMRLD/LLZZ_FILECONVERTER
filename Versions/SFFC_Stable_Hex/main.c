#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "XIO.h"


#define NUMBER_OF_COMMANDLINE_ARGUMENTS 8
#define SUFFIX_LENGTH 19
/*
#####################################################################
#####################################################################
#####################################################################
AUTHOR  ::: OLIVER BALINT
EMAIL   ::: oliver.balint03@gmail.com


THIS PROGRAMM IS INTENDED TO CONVERT LLZZ AND FIXED FILE TYPES ON AIX
TO FIXED LENGTH FILES THAT CAN BE READ BY THE HESSLING EDITOR!


---------------------------------------------------------------------
---------------------------------------------------------------------


INPUT       ::::
1       ::::::::
2       ::::::::
3       ::::::::
4       ::::::::
5       ::::::::        "YES" OR "NO" --> SHALL THE HEX FORMAT BE WRITTEN TO STDOUT
6       ::::::::        "YES" OR "NO" --> (SHOULD BE CHANGED TO "BRW" AND "OW" == BROWSE AND OWEWRITE) --> IS THE PROGRAMM ALLOWD TO CHANGE CONTENT OF FILE
7       ::::::::        SIZE OF THE RECORD






IN CASE OF ANY PROBLEMS PLEASE CONTACT ME.
*/

/*
short int rec_len[42949650970];
int rec_len_index = 0;
long int rec_loc[42949650970];
int rec_loc_index = 0;
*/



int main(int argc, char *argv[])
{


short int           largest_record_size = 0;
short int           length_of_record = 0;
short int           rec_len[5000];

int                 file_to_convert = 0;
int                 file_converted = 0;
int                 debug = -1000;
int                 x = 0;
int                 rec_len_index = 0;
int                 rec_loc_index = 0;

int                 PointerIncrementation = 0;

long int            location_of_recordlength = 0;
long int            rec_loc[5000];

long long int       count_record = 0;


char                *suffix = "<EOL><Lrecl:      >";
char                tmp_char[19];
char                record_len_in_str[10];
char                *path = "/home/oliver/OFFICIAL_PROJECTS/SFFC/Examplefile raw and converteed file/PAE.M.ADB2.EHD";
char                *path_to_converted_file = "/home/oliver/binfile";
unsigned char        LineInHex[40000];
unsigned char       *buffer_for_file;
unsigned char       *arr_converted_bytes;

struct              stat statbf;






if(argc < NUMBER_OF_COMMANDLINE_ARGUMENTS)
    {

    }

printf("ARGV:::: %s\n", argv[1]);
printf("ARGC:::: %i\n", argc);

//if browse only then open in read ans if not then readwrite

file_to_convert = xopen(path, 0);
stat(path, &statbf);

file_converted = open(path_to_converted_file, O_WRONLY);


buffer_for_file = malloc(statbf.st_size);


debug = read(file_to_convert, buffer_for_file, statbf.st_size);
printf("Size of file: %zu \n", statbf.st_size);



while(location_of_recordlength < statbf.st_size)
    {
   //*((short int *)&rec_len[rec_len_index]) =  buffer_for_file[location_of_recordlength];

    rec_len[rec_len_index] = (buffer_for_file[location_of_recordlength]*256)+buffer_for_file[location_of_recordlength+1];
    rec_loc[rec_loc_index] = location_of_recordlength;


    length_of_record = rec_len[rec_len_index];


    rec_len_index++;
    rec_loc_index++;



    printf("Recordsize                      :::: %i\n", length_of_record);
    printf("Whereis 2 byte  :::: %li\n", location_of_recordlength);
    location_of_recordlength += length_of_record;

    if(length_of_record > largest_record_size)
        {
        largest_record_size = length_of_record;
        }
    count_record++;
    }

PointerIncrementation = largest_record_size-4+SUFFIX_LENGTH;


arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
printf("MALOC : %lli\n", (largest_record_size-4+SUFFIX_LENGTH)*count_record);
printf("Largest record size :::::::::: %i\n", largest_record_size);
printf("How many records: %lli\n", count_record);


memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record);


for(x = 0 ; x < rec_len_index ; x++)
    {
    printf("%i\n", x); //kell egy fix hosszusagu buffer es abba kell berakni a cuccokat

    memset(&tmp_char, 0, 20);
    memset(record_len_in_str, 0, 10);

    sprintf(&record_len_in_str, "%d", rec_len[x]-4);

    strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
  //  printf("TMP_CHAR    : %s\n", tmp_char);
    strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
  //  printf("TMP_CHAR    : %s\n", tmp_char);
    strncat(tmp_char, ">", 2);

    printf("WRITING     : %s\n", tmp_char);

    printf("Write location: %li\n", rec_loc[x]+4);
    printf("Writing %i long \n", rec_len[x]-4);


    for(int HexInd = 0 ; HexInd < rec_len[x]-4 ; HexInd++)
        {
        //printf("%i\n", HexInd);
        //printf("CHAR ::: %X\n", *(buffer_for_file+4+rec_loc[x]+HexInd));

        chartohex(*(buffer_for_file+4+rec_loc[x]+HexInd),  &LineInHex[HexInd*2]);
        }

        printf("\n\n HEX: %s \n\n", LineInHex);
        printf("REC_LEN ::: %i\n", rec_len[x]-4);
        //exit(1);

    memcpy(arr_converted_bytes+(largest_record_size-4+SUFFIX_LENGTH)*x, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
    memcpy(arr_converted_bytes+(largest_record_size-4+SUFFIX_LENGTH)*x+rec_len[x]-4, tmp_char, strlen(tmp_char));

    for(int UpprByte = 0 ; UpprByte <= (rec_len[x]-4)*2 ; UpprByte+=2)
        {

        printf("%c", LineInHex[UpprByte]);
        }

        printf("\n");

    for(int LowrByte = 1 ; LowrByte <= (rec_len[x]-4)*2 ; LowrByte+=2)
        {
        printf("%c", LineInHex[LowrByte]);
        }
sleep(5);
    }


write(file_converted, arr_converted_bytes, (largest_record_size-4+SUFFIX_LENGTH)*count_record);


close(file_to_convert);
close(file_converted);
return 0;
}


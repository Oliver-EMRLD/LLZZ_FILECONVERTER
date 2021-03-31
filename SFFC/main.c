#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>



#define NUMBER_OF_COMMANDLINE_ARGUMENTS 8
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
size_t size_record = 0;
short int largest_record_size = 0;
int file_to_convert = 0;
char *path = "/home/oliver/OFFICIAL_PROJECTS/SFFC/Examplefile raw and converteed file/PAE.M.ADB2.EHD";
char *path_to_converted_file = "/home/oliver/OFFICIAL_PROJECTS/SFFC/Examplefile raw and converteed file/convertedfile_by_programm";
struct stat statbf;
int debug = -1000;
unsigned char *buffer_for_file;

long int index_of_recordlength = 0;
short int length_of_record = 0;
short int tmp = 0;
int x = 0;





if(argc < NUMBER_OF_COMMANDLINE_ARGUMENTS)
    {

    }

printf("ARGV:::: %s\n", argv[1]);
printf("ARGC:::: %i\n", argc);

//if browse only then open in read ans if not then readwrite

file_to_convert = open64(path, O_RDONLY);
stat(path, &statbf);



buffer_for_file = malloc(statbf.st_size);
debug = read(file_to_convert, buffer_for_file, statbf.st_size);
printf("Size of file: %zu \n", statbf.st_size);



while(index_of_recordlength < statbf.st_size)
    {
/*    rec_len[rec_len_index] = (buffer_for_file[index_of_recordlength]*256)+buffer_for_file[index_of_recordlength+1];
    rec_loc[rec_loc_index] = index_of_recordlength;
    rec_len_index++;
    rec_loc_index++;
*/
    length_of_record = (buffer_for_file[index_of_recordlength]*256)+buffer_for_file[index_of_recordlength+1];


    printf("Recordsize                      :::: %i\n", length_of_record);
    printf("Whereis 2 byte : %i\n", index_of_recordlength);
    index_of_recordlength += length_of_record;

    if(length_of_record > largest_record_size)
        {
        largest_record_size = length_of_record;
        }
        x++;

    }


printf("Largest record size :::::::::: %i\n", largest_record_size);
printf("How many records: %i\n", x);

close(file_to_convert);
return 0;
}


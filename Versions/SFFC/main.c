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
short int largest_record_size = 0;
int file_to_convert = 0;
int file_converted = 0;
char *path = "/home/oliver/OFFICIAL_PROJECTS/SFFC/Examplefile raw and converteed file/PAE.M.ADB2.EHD";
char *path_to_converted_file = "/home/oliver/binfile";
struct stat statbf;
int debug = -1000;
unsigned char *buffer_for_file;
char *suffix = "<EOL><Lrecl:      >";
char *tmp_char;
char record_len_in_str[10];
long int location_of_recordlength = 0;
short int length_of_record = 0;
int x = 0;
char *converted_line;
tmp_char = malloc(2048);
short int rec_len[5000];
int rec_len_index = 0;
long int rec_loc[5000];
int rec_loc_index = 0;

long long int count_record = 0;


unsigned char *arr_converted_bytes;

if(argc < NUMBER_OF_COMMANDLINE_ARGUMENTS)
    {

    }

printf("ARGV:::: %s\n", argv[1]);
printf("ARGC:::: %i\n", argc);

//if browse only then open in read ans if not then readwrite

file_to_convert = open(path, O_RDONLY);
stat(path, &statbf);

file_converted = open(path_to_converted_file, O_WRONLY);


buffer_for_file = malloc(statbf.st_size);


debug = read(file_to_convert, buffer_for_file, statbf.st_size);
printf("Size of file: %zu \n", statbf.st_size);



while(location_of_recordlength < statbf.st_size)
    {
    rec_len[rec_len_index] =  (buffer_for_file[location_of_recordlength]*256)+buffer_for_file[location_of_recordlength+1];
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
    x++;

    }

arr_converted_bytes = malloc((largest_record_size-4)*x);

printf("Largest record size :::::::::: %i\n", largest_record_size);
printf("How many records: %i\n", x);

printf("rec_len[] ::::: %i\n", rec_len[0]);
printf("rec_loc[] ::::: %li\n", rec_loc[0]);

converted_line = malloc(largest_record_size+1);
memset(converted_line, '\n', largest_record_size+1);

memset(arr_converted_bytes, '\n', (largest_record_size-4)*x);

count_record = x;

for(x = 0 ; x < rec_len_index ; x++)
    {
    memset(tmp_char, 0, 2048);
    memset(record_len_in_str, 0, 10);

    sprintf(record_len_in_str, "%d", rec_len[x]-4);

    strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
    //printf("TMP_CHAR : %s\n", tmp_char);
    strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
    //printf("TMP_CHAR : %s\n", tmp_char);
    strncat(tmp_char, (char *)">", 2);


    printf("%i\n", x); //kell egy fix hosszusagu buffer es abba kell berakni a cuccokat
    //debug = write(file_converted, buffer_for_file+4+rec_loc[x], rec_len[x]-4);

    memcpy(&arr_converted_bytes[x], buffer_for_file+4+rec_loc[x], rec_len[x]-4);
    memcpy(&arr_converted_bytes[x+rec_len[x]-4], tmp_char, strlen(tmp_char));

    //printf("LOCATION ::: %li\n", 4+rec_loc[x]);
    //printf("LENGTH   ::: %i\n", rec_len[x]-4);

    printf("WRITING : %s\n", tmp_char);
    //debug = write(file_converted, tmp_char, strlen(tmp_char));
    //debug = write(file_converted, converted_line, largest_record_size-rec_len[x]-strlen(tmp_char)+1);
    //debug = write(file_converted, '\n', 1);
    }

write(file_converted, arr_converted_bytes, (largest_record_size-4)*count_record);


close(file_to_convert);
close(file_converted);
return 0;
}


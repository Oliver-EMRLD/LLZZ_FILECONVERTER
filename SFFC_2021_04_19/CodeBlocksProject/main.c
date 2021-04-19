#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "XIO.h"


#define NUMBER_OF_COMMANDLINE_ARGUMENTS 8
#define SUFFIX_LENGTH 19



char HexadecimalTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', };





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

int convertllzz( int file_converted , int file_to_convert , long long int count_record , short int largest_record_size , short int rec_len[] , long int rec_loc[] , int rec_len_index , unsigned char *arr_converted_bytes , unsigned char *buffer_for_file, char *HEX);

            /*
            TAKES 2 BUFFERS, CONVERTS THE CONTENTS OF THE FIRST INTO FIXWIDTH APPENDS SUFFIX AND PUTS IT INTO THE SECOND W OR WOUT HEX ACCORDING TO ARGUMENT "HEX"
            FINALLY IT WRITES THE CONVERTED BUFFER TO THE FILE AND CLOSES THE FILES
            */

long long int findrecords( unsigned char *buffer_for_file , long int llzzlocation , off_t flsize , short int rec_len[] , long int rec_loc[] , int *RecArrIndex , short int *MaxRecordSize);

            /*
            FUNCTION TAKES THE INPUTBUFFER, PUTS LLZZ AND LLZZ LOCATION IN 2 ARRAYS(long int short int rec_len[] , long int rec_loc[])
            FINDS MAXIMAL RECORD LENGTH
            RETURNS HOW MANY RECORDS ARE IN BUFFER - 1
            */

int convertfixline( unsigned char *buffer , unsigned char *output , short int Arrreclen[] , long int Arrrecloc[] , int Arrindex , short int Largestrecordsize , char *HEX , int bfsz);

            /*
            CONVERTS THE CONTEXT OF "buffer"(fixedwidth) INTO REGULAR RECORDS "output"
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
int                 TmpPointerIncrement = 0;
int                 PointerIncrementation = 0;
int                 UpprByte = 0;
int                 LowrByte = 0;
int                 CharLineLoc = 0;

long int            location_of_recordlength = 0;
long int            rec_loc[5000];

int                 afaszomkivanmarkesovan = 0;

long long int       count_record = 0;

char                *suffix = "<EOL><Lrecl:      >";
char                tmp_char[19];
char                record_len_in_str[10];
char                *path = "/home/oliver/inputfile";
char                *path_to_converted_file = "/home/oliver/binfile";

char                *HEX = "ys";

unsigned char        LineInHex[40000];
unsigned char       *buffer_for_file;
unsigned char       *arr_converted_bytes;
unsigned char       *bufferreconver;
unsigned char       *anyad;

struct              stat statbf;






if(argc < NUMBER_OF_COMMANDLINE_ARGUMENTS)
    {
    printf("Not the right amount of arguments gotten!\n");
    //exit(1);
    }



checkarg(argv, argc, 1, "yes", "no");                                               //CHECKING IF CLI ARGUMENTS ARE WHAT THEY SUPPOSED TO BE

//if browse only then open in read ans if not then readwrite

file_to_convert = xopen(argv[2], 0);
file_converted = xopen(argv[3], 1);
stat(argv[2], &statbf);

/*
file_to_convert = xopen(path, 0);
stat(path, &statbf);
file_converted = xopen(path_to_converted_file, 1);                                  ONLY FOR TESTING!!!!!!!
buffer_for_file = malloc(statbf.st_size);
*/

arr_converted_bytes = malloc(statbf.st_size);
debug = read(file_to_convert, buffer_for_file, statbf.st_size);
                                                                                    /*
                                                                                    OPENING OF THE 2 PATH GOTTEN ON THE CLI
                                                                                    STAT ON THE INPUTFILE FOR CONVERSION
                                                                                    ALLOCATING BUFFER FOR INPUTFILE AND READ THE FILE
                                                                                    */





count_record = findrecords(buffer_for_file, location_of_recordlength , statbf.st_size, rec_len, rec_loc, &rec_len_index, &largest_record_size);

printf("Count record : %lli\n", count_record);
printf("Rec_len_index : %i\n", rec_len_index);

PointerIncrementation = largest_record_size-4+SUFFIX_LENGTH;
printf("LARGEST RECORD : %i\n", largest_record_size);

convertllzz(file_converted, file_to_convert , count_record, largest_record_size , rec_len, rec_loc, rec_len_index, arr_converted_bytes, buffer_for_file, HEX);



/*          TESTING FOR RECONVERSION!!!!


file_converted = open("/home/oliver/backconvert", O_CREAT | O_WRONLY , 0777);
file_to_convert = open(path_to_converted_file, O_RDONLY);
stat(path_to_converted_file, &statbf);

bufferreconver = malloc(statbf.st_size);
anyad = malloc(statbf.st_size);

read(file_to_convert, bufferreconver, statbf.st_size);

debug = convertfixline(bufferreconver, anyad, rec_len,rec_loc, rec_len_index,largest_record_size, HEX, (int )statbf.st_size);

write(file_converted, anyad, debug);

close(file_converted);
close(file_to_convert);
*/

return 0;
}




int convertfixline( unsigned char *buffer , unsigned char *output , short int Arrreclen[] , long int Arrrecloc[] , int Arrindex , short int Largestrecordsize , char *HEX , int bfsz)
    {
    unsigned char bytes[2];
    unsigned char tmp[2];
    int x = 0;
    int debug = 0;
    int increment = 0;

    memset(output, 0, bfsz);

    printf("Arrindex : %i \n", Arrindex);

    for( x = 0 ; x < Arrindex ; x++)
        {
        printf("%i\n", x);
        memcpy(&tmp, &Arrreclen[x], 2);
        bytes[1] = tmp[0];
        bytes[0] = tmp[1];
        memcpy(output+Arrrecloc[x], &bytes , 2);
        increment = (Largestrecordsize+SUFFIX_LENGTH-4)*x;
        memcpy(output+Arrrecloc[x]+4, buffer+increment, Arrreclen[x]-4);

        debug += Arrreclen[x];

        }
    return debug;
    }






long long int findrecords( unsigned char *buffer_for_file , long int llzzlocation , off_t flsize , short int rec_len[] , long int rec_loc[] , int *RecArrIndex , short int *MaxRecordSize)
    {
    short int length_of_record = 0;

    long long int CurrentRecord = 0;

    while(llzzlocation < flsize)
    {
    rec_len[*RecArrIndex] = (buffer_for_file[llzzlocation]*256)+buffer_for_file[llzzlocation+1];
    rec_loc[*RecArrIndex] = llzzlocation;

    printf("Recordsize                      :::: %i\n", rec_len[*RecArrIndex]);

    length_of_record = rec_len[*RecArrIndex];
    printf("Length of record : %i\n", length_of_record);
    *RecArrIndex+=1;


    //printf("Record index : %si\n", RecArrIndex);
    printf("Whereis 2 byte  :::: %li\n", llzzlocation);
    llzzlocation += length_of_record;
    if(length_of_record > *MaxRecordSize)
        {
        *MaxRecordSize = length_of_record;
        printf("MAXRECORD : %i\n", *MaxRecordSize);
        }
    CurrentRecord++;
   }



    return CurrentRecord;
    }




int convertllzz( int file_converted , int file_to_convert , long long int count_record , short int largest_record_size , short int rec_len[] , long int rec_loc[] , int rec_len_index , unsigned char *arr_converted_bytes , unsigned char *buffer_for_file, char *HEX)
    {


    int                 TmpPointerIncrement = 0;
    int                 PointerIncrementation = 0;
    int                 UpprByte = 0;
    int                 LowrByte = 0;
    int                 x = 0;
    int                 CharLineLoc = 0;


    char                tmp_char[19];
    char                record_len_in_str[10];
    char                *suffix = "<EOL><Lrecl:      >";
    unsigned char        LineInHex[40000];
    int                 afaszomkivanmarkesovan = 0;




    PointerIncrementation = largest_record_size-4+SUFFIX_LENGTH;




    if(strncmp("yes", HEX, 3) == 0)
        {
        printf("\n\nHEX\n\n");
        //sleep(2);
        arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
        printf("MALOC : %lli\n", (largest_record_size-4+SUFFIX_LENGTH)*count_record);
        printf("Largest record size :::::::::: %i\n", largest_record_size);
        printf("How many records: %lli\n", count_record);
        printf("Last record location : %i\n", rec_loc[rec_len_index-1]);


        memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record*3);



        for(x = 0 ; x < rec_len_index ; x++)
            {

            printf("%i\n", x);

            memset(&LineInHex, 0, 40000);
            memset(&tmp_char, 0, 20);
            memset(record_len_in_str, 0, 10);

            sprintf(&record_len_in_str, "%d", rec_len[x]-4);

            strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
            strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
            strncat(tmp_char, ">", 2);

            printf("WRITING     : %s\n", tmp_char);

            printf("Write location: %li\n", rec_loc[x]+4);
            printf("Writing %i long \n", rec_len[x]-4);


            for(int HexInd = 0 ; HexInd <= rec_len[x]-4 ; HexInd++)
                {
                //printf("%i\n", HexInd);
                //printf("CHAR ::: %X\n", *(buffer_for_file+4+rec_loc[x]+HexInd));

                chartohex(*(buffer_for_file+4+rec_loc[x]+HexInd),  &LineInHex[HexInd*2]);
                }

                //printf("\n\n HEX: %s \n\n", LineInHex);
                //printf("REC_LEN ::: %i\n", rec_len[x]-4);

            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc+rec_len[x]-4, tmp_char, strlen(tmp_char));

            for(UpprByte = 0 ; UpprByte <= (rec_len[x]-4)*2+1 ; UpprByte+=2)
                {
                TmpPointerIncrement = (PointerIncrementation*(CharLineLoc+1)+afaszomkivanmarkesovan);
                arr_converted_bytes[TmpPointerIncrement] = LineInHex[UpprByte];
                //printf("%c", arr_converted_bytes[TmpPointerIncrement]);
                //printf("Upper byte %i\n", TmpPointerIncrement);
                afaszomkivanmarkesovan++;
                }

                //printf("\n");
                memcpy(arr_converted_bytes+TmpPointerIncrement, tmp_char, strlen(tmp_char));
                afaszomkivanmarkesovan = 0;

            for(LowrByte = 1 ; LowrByte <= (rec_len[x]-4)*2+1 ; LowrByte+=2)     //largest_record_size-4 ; LowrByte+=2)
                {
                TmpPointerIncrement = (PointerIncrementation*(CharLineLoc+2)+afaszomkivanmarkesovan);
                arr_converted_bytes[TmpPointerIncrement] = LineInHex[LowrByte];
                //printf("%c", arr_converted_bytes[TmpPointerIncrement]);
                //printf("Lower byte: %i\n", TmpPointerIncrement);
                afaszomkivanmarkesovan++;
                }

                memcpy(arr_converted_bytes+TmpPointerIncrement, tmp_char, strlen(tmp_char));

        CharLineLoc += 3;
        }


        write(file_converted, arr_converted_bytes, (PointerIncrementation)*count_record*3);

        close(file_to_convert);
        close(file_converted);
        return 0;
        }








    arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record);
    printf("MALOC : %lli\n", (largest_record_size-4+SUFFIX_LENGTH)*count_record);
    printf("Largest record size :::::::::: %i\n", largest_record_size);
    printf("How many records: %lli\n", count_record);
    printf("Last record location : %i\n", rec_loc[rec_len_index-1]);


    memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record);



    for(x = 0 ; x < rec_len_index ; x++)
        {
        printf("%i\n", x);

        memset(&tmp_char, 0, 20);
        memset(record_len_in_str, 0, 10);

        sprintf(&record_len_in_str, "%d", rec_len[x]-4);

        strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
        strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
        strncat(tmp_char, ">", 2);

        printf("WRITING     : %s\n", tmp_char);

        printf("Write location: %li\n", rec_loc[x]+4);
        printf("Writing %i long \n", rec_len[x]-4);


        memcpy(arr_converted_bytes+PointerIncrementation*x, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
        memcpy(arr_converted_bytes+PointerIncrementation*x+rec_len[x]-4, tmp_char, strlen(tmp_char));
        }


    write(file_converted, arr_converted_bytes, (PointerIncrementation)*count_record);


    close(file_to_convert);
    close(file_converted);
    return 0;
    }












/*
while(location_of_recordlength < statbf.st_size)
    {
    rec_len[rec_len_index] = (buffer_for_file[location_of_recordlength]*256)+buffer_for_file[location_of_recordlength+1];
    rec_loc[rec_loc_index] = location_of_recordlength;


    length_of_record = rec_len[rec_len_index];

    printf("Recordsize                      :::: %i\n", rec_len[rec_len_index]);
    rec_len_index++;
    rec_loc_index++;




    printf("Whereis 2 byte  :::: %li\n", location_of_recordlength);
    location_of_recordlength += length_of_record;
    if(length_of_record > largest_record_size)
        {
        largest_record_size = length_of_record;
        }
    count_record++;
    }

*/











/*

if(strncmp("yes", HEX, 3) == 0)
    {
    printf("\n\nHEX\n\n");
    //sleep(2);
    arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
    printf("MALOC : %lli\n", (largest_record_size-4+SUFFIX_LENGTH)*count_record);
    printf("Largest record size :::::::::: %i\n", largest_record_size);
    printf("How many records: %lli\n", count_record);
    printf("Last record location : %i\n", rec_loc[rec_loc_index-1]);


    memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record*3);



    for(x = 0 ; x < rec_len_index ; x++)
        {

        printf("%i\n", x);

        memset(&LineInHex, 0, 40000);
        memset(&tmp_char, 0, 20);
        memset(record_len_in_str, 0, 10);

        sprintf(&record_len_in_str, "%d", rec_len[x]-4);

        strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
        strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
        strncat(tmp_char, ">", 2);

        printf("WRITING     : %s\n", tmp_char);

        printf("Write location: %li\n", rec_loc[x]+4);
        printf("Writing %i long \n", rec_len[x]-4);


        for(int HexInd = 0 ; HexInd <= rec_len[x]-4 ; HexInd++)
            {
            //printf("%i\n", HexInd);
            //printf("CHAR ::: %X\n", *(buffer_for_file+4+rec_loc[x]+HexInd));

            chartohex(*(buffer_for_file+4+rec_loc[x]+HexInd),  &LineInHex[HexInd*2]);
            }

            //printf("\n\n HEX: %s \n\n", LineInHex);
            //printf("REC_LEN ::: %i\n", rec_len[x]-4);

        memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
        memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc+rec_len[x]-4, tmp_char, strlen(tmp_char));

        for(UpprByte = 0 ; UpprByte <= (rec_len[x]-4)*2+1 ; UpprByte+=2)
            {
            TmpPointerIncrement = (PointerIncrementation*(CharLineLoc+1)+afaszomkivanmarkesovan);
            arr_converted_bytes[TmpPointerIncrement] = LineInHex[UpprByte];
            //printf("%c", arr_converted_bytes[TmpPointerIncrement]);
            //printf("Upper byte %i\n", TmpPointerIncrement);
            afaszomkivanmarkesovan++;
            }

            //printf("\n");
            memcpy(arr_converted_bytes+TmpPointerIncrement, tmp_char, strlen(tmp_char));
            afaszomkivanmarkesovan = 0;

        for(LowrByte = 1 ; LowrByte <= (rec_len[x]-4)*2+1 ; LowrByte+=2)     //largest_record_size-4 ; LowrByte+=2)
            {
            TmpPointerIncrement = (PointerIncrementation*(CharLineLoc+2)+afaszomkivanmarkesovan);
            arr_converted_bytes[TmpPointerIncrement] = LineInHex[LowrByte];
            //printf("%c", arr_converted_bytes[TmpPointerIncrement]);
            //printf("Lower byte: %i\n", TmpPointerIncrement);
            afaszomkivanmarkesovan++;
            }

            memcpy(arr_converted_bytes+TmpPointerIncrement, tmp_char, strlen(tmp_char));

    CharLineLoc += 3;
    }


    write(file_converted, arr_converted_bytes, (PointerIncrementation)*count_record*3);

    close(file_to_convert);
    close(file_converted);
    return 0;
    }








arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record);
printf("MALOC : %lli\n", (largest_record_size-4+SUFFIX_LENGTH)*count_record);
printf("Largest record size :::::::::: %i\n", largest_record_size);
printf("How many records: %lli\n", count_record);
printf("Last record location : %i\n", rec_loc[rec_loc_index-1]);


memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record);



for(x = 0 ; x < rec_len_index ; x++)
    {
    printf("%i\n", x);

    memset(&tmp_char, 0, 20);
    memset(record_len_in_str, 0, 10);

    sprintf(&record_len_in_str, "%d", rec_len[x]-4);

    strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
    strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
    strncat(tmp_char, ">", 2);

    printf("WRITING     : %s\n", tmp_char);

    printf("Write location: %li\n", rec_loc[x]+4);
    printf("Writing %i long \n", rec_len[x]-4);


    memcpy(arr_converted_bytes+PointerIncrementation*x, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
    memcpy(arr_converted_bytes+PointerIncrementation*x+rec_len[x]-4, tmp_char, strlen(tmp_char));
    }


write(file_converted, arr_converted_bytes, (PointerIncrementation)*count_record);



close(file_to_convert);
close(file_converted);
return 0;
}


*/

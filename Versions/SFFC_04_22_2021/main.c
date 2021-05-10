#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "XIO.h"


#define NUMBER_OF_COMMANDLINE_ARGUMENTS 5
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

char                varflname[4097];
char                wrdir[4097];
char                tmppath[4097];




unsigned char        LineInHex[40000];
unsigned char       *buffer_for_file;
unsigned char       *arr_converted_bytes;
unsigned char       *bufferreconver;
unsigned char       *anyad;

struct              stat statbf;




char *pathin = "/home/oliver/tst/input";
char *pathout = "/home/oliver/tst/hex";
char *mode = "FIXED";
char *hex = "YES";
char *browse = "NO";

argv[1] = mode;
argv[2] = pathin;
argv[3] = pathout;
argv[5] = browse;
argv[4] = hex;
argc += 5;

    getcwd(&wrdir, 4096);
    memcpy(&varflname, argv[2], 4096);

    printf("ARG : %s\n", argv[2]);
    printf("VAR : %s\n", varflname);
    printf("Wrdir %s\n", wrdir);

printf("\n\n                                     Hello \n\n\n\n\n");



if(argc < NUMBER_OF_COMMANDLINE_ARGUMENTS)
    {
    printf("Not the right amount of arguments gotten!\n");
    printf("Usage : CVT2NTHE || IN_TYP || P_IN || P_OUT || HEX || BROWSE || MAXRECLEN\n");
    printf("\n1 ::IN_TYP              (""LLZZ"" // ""FIXED"") --> CONVERT FROM LLZZ OR FIXED FORMAT\n");
    printf("\n2 ::P_IN                PATH TO THE FILE TO BE CONVERTED\n");
    printf("\n3 ::P_OUT               PATH TO THE OUTPUTFILE\n");
    printf("\n3 ::HEX ==(YES/NO)      CONVERT TO HEX?\n");
    printf("\n4 ::BROWSE == (YES/NO)  CAN THE CONTEXT BE CHANGED?\n");
    printf("\n5 ::MAXRECLEN           (!!OPTIONAL!!) MAXIMUM RECORDSIZE IN THE INPUTFILE\n");
    exit(1);
    }

for(int fuck = 0 ; fuck < argc ; fuck++)
    {
    printf("Arg %i :::::: ", fuck);
    printf("%s\n", argv[fuck]);
    }
//checkarg(argv, argc, 4, "YES", "NO");                                               //CHECKING IF CLI ARGUMENTS ARE WHAT THEY SUPPOSED TO BE

if(strncmp(argv[1], "LLZZ", 5) == 0)
    {
    //if browse only then open in read ans if not then readwrite

    file_to_convert = xopen(argv[2], 0);
    file_converted = xopen(argv[3], 1);
    stat(argv[2], &statbf);

    buffer_for_file = malloc(statbf.st_size);
    debug = read(file_to_convert, buffer_for_file, statbf.st_size);
                                                                                        /*
                                                                                        OPENING OF THE 2 PATH GOTTEN ON THE CLI
                                                                                        STAT ON THE INPUTFILE FOR CONVERSION
                                                                                        ALLOCATING BUFFER FOR INPUTFILE AND READ THE FILE
                                                                                        */



    count_record = findrecords(buffer_for_file, location_of_recordlength , statbf.st_size, rec_len, rec_loc, &rec_len_index, &largest_record_size);

    printf("Amount of records : %lli\n", count_record);
    printf("LARGEST RECORD : %i\n", largest_record_size);

    PointerIncrementation = largest_record_size-4+SUFFIX_LENGTH;

    convertllzz(file_converted, file_to_convert , count_record, largest_record_size , rec_len, rec_loc, rec_len_index, arr_converted_bytes, buffer_for_file, argv[4]);



    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "reclen", 7);
    printf("tmppath : %s\n", tmppath);
    savevar(&rec_len, sizeof(short int), (size_t)rec_len_index, tmppath);

    memset(&tmppath, 0, 4096);

    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "recloc", 7);
    printf("tmppath : %s\n", tmppath);
    savevar(&rec_loc, sizeof(long int), (size_t)rec_len_index, tmppath);

    memset(&tmppath, 0, 4096);

    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "ind", 4);
    printf("tmppath : %s\n", tmppath);
    savevar(&rec_len_index, sizeof(int), (size_t)0, tmppath);

    memset(&tmppath, 0, 4096);

    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "max", 4);
    printf("tmppath : %s\n", tmppath);
    savevar(&largest_record_size, sizeof(short int), (size_t)0, tmppath);



    return 0;
    }

else if(strncmp(argv[1], "FIXED", 6) == 0)
    {
    printf("Fixed\n");
    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "ind", 4);
    printf("TMPPATH : %s\n", tmppath);

    readvar(&rec_len_index, tmppath, (size_t)1, sizeof(int));
    printf("Rec_len_index: %i\n", rec_len_index);

    memset(&tmppath, 0, 4096);

    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "max", 4);

    readvar(&largest_record_size, tmppath, (size_t)1, sizeof(short int));
    printf("Largest record: %hi\n", largest_record_size);

    memset(&tmppath, 0, 4096);

    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "recloc", 7);

    readvar(&rec_loc, tmppath, 541, sizeof(long int));
    printf("REC_LOC0 : %li\n", rec_loc[2]);

    memset(&tmppath, 0, 4096);

    memcpy(&tmppath, varflname, strlen(varflname));
    strncat(&tmppath, "reclen", 7);

    readvar(&rec_len, tmppath, 541, sizeof(short int));
    printf("RECLEN : %hi\n", rec_len[2]);


    file_to_convert = xopen(argv[2], 0);
    file_converted = xopen(argv[3], 1);

    stat(argv[2], &statbf);

    bufferreconver = malloc(statbf.st_size);
    anyad = malloc(statbf.st_size);

    read(file_to_convert, bufferreconver, statbf.st_size);
    printf("Here\n");

    debug = convertfixline(bufferreconver, anyad, rec_len,rec_loc, rec_len_index,largest_record_size, argv[4], (int )statbf.st_size);

    write(file_converted, anyad, debug);

    close(file_converted);
    close(file_to_convert);
    return 0;

    }

printf("Not valid argument!\n");
return 0;
}
































int convertfixline( unsigned char *buffer , unsigned char *output , short int Arrreclen[] , long int Arrrecloc[] , int Arrindex , short int Largestrecordsize , char *HEX , int bfsz)
// You need to check if EOL is where it is, and check the length, compare with actual length
    {
    unsigned char bytes[2];
    unsigned char tmp[2];
    int x = 0;
    int debug = 0;
    int increment = 0;

    memset(output, 0, bfsz);
    if(memcmp( HEX , "NO", 3) == 0)
        {
        printf("After memset\n");

        for( x = 0 ; x < Arrindex ; x++)
            {
            printf("X : %i\n", x);
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
    if(memcmp( HEX , "YES" , 4 ) == 0)
        {
        for( int x = 0 ; x < Arrindex ; x++ )
            {
            printf("X in HEX : %i\n", x);
            memcpy(&tmp, &Arrreclen[x], 2);
            bytes[1] = tmp[0];
            bytes[0] = tmp[1];
            memcpy(output+Arrrecloc[x], &bytes , 2);
            for( int y = 0 ; y <= Arrreclen[x] ; y++ )
                {
                tmp[0] = buffer+(Largestrecordsize+SUFFIX_LENGTH-4)*(x+1)+y;
                tmp[1] = buffer+(Largestrecordsize+SUFFIX_LENGTH-4)*(x+2)+y;
                output[Arrrecloc[x+4]] = hex2int(&tmp, 2);
                }
            increment = (Largestrecordsize+SUFFIX_LENGTH-4)*(x+3);
            debug += Arrreclen[x];
            }
        return debug;
        }

    }






long long int findrecords( unsigned char *buffer_for_file , long int llzzlocation , off_t flsize , short int rec_len[] , long int rec_loc[] , int *RecArrIndex , short int *MaxRecordSize)
    {
    short int length_of_record = 0;

    long long int CurrentRecord = 0;

    while(llzzlocation < flsize)
    {
    rec_len[*RecArrIndex] = (buffer_for_file[llzzlocation]*256)+buffer_for_file[llzzlocation+1];
    rec_loc[*RecArrIndex] = llzzlocation;


    length_of_record = rec_len[*RecArrIndex];
    *RecArrIndex+=1;



    llzzlocation += length_of_record;
    if(length_of_record > *MaxRecordSize)
        {
        *MaxRecordSize = length_of_record;
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


    printf("HEX : %s\n", HEX);
    sleep(6);

    PointerIncrementation = largest_record_size-4+SUFFIX_LENGTH;




    if(memcmp(HEX, "YES", 4) == 0)
        {
        printf("\n\nHEX\n\n");
        arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record*3);

        memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record*3);



        for(x = 0 ; x < rec_len_index ; x++)
            {


            memset(&LineInHex, 0, 40000);
            memset(&tmp_char, 0, 20);
            memset(record_len_in_str, 0, 10);

            sprintf(&record_len_in_str, "%d", rec_len[x]-4);

            strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
            strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
            strncat(tmp_char, ">", 2);




            for(int HexInd = 0 ; HexInd <= rec_len[x]-4 ; HexInd++)
                {
                chartohex(*(buffer_for_file+4+rec_loc[x]+HexInd),  &LineInHex[HexInd*2]);
                }


            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc+rec_len[x]-4, tmp_char, strlen(tmp_char));

            for(UpprByte = 0 ; UpprByte <= (rec_len[x]-4)*2+1 ; UpprByte+=2)
                {
                TmpPointerIncrement = (PointerIncrementation*(CharLineLoc+1)+afaszomkivanmarkesovan);
                arr_converted_bytes[TmpPointerIncrement] = LineInHex[UpprByte];
                afaszomkivanmarkesovan++;
                }
                tmp_char[3] = 'H';
                memcpy(arr_converted_bytes+TmpPointerIncrement, tmp_char, strlen(tmp_char));
                afaszomkivanmarkesovan = 0;

            for(LowrByte = 1 ; LowrByte <= (rec_len[x]-4)*2+1 ; LowrByte+=2)
                {
                TmpPointerIncrement = (PointerIncrementation*(CharLineLoc+2)+afaszomkivanmarkesovan);
                arr_converted_bytes[TmpPointerIncrement] = LineInHex[LowrByte];
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



    memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record);



    for(x = 0 ; x < rec_len_index ; x++)
        {

        memset(&tmp_char, 0, 20);
        memset(record_len_in_str, 0, 10);

        sprintf(&record_len_in_str, "%d", rec_len[x]-4);

        strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
        strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
        strncat(tmp_char, ">", 2);




        memcpy(arr_converted_bytes+PointerIncrementation*x, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
        memcpy(arr_converted_bytes+PointerIncrementation*x+rec_len[x]-4, tmp_char, strlen(tmp_char));
        }


    write(file_converted, arr_converted_bytes, (PointerIncrementation)*count_record);


    close(file_to_convert);
    close(file_converted);
    return 0;
    }










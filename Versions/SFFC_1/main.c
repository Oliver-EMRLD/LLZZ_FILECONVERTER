#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "XIO.h"


#define NUMBER_OF_COMMANDLINE_ARGUMENTS 5
#define SUFFIX_LENGTH 19
#define MAXPATHLEN 4096



struct range
    {
    size_t from;
    size_t till;
    int    whoami;
    };




/*
short int rec_len[42949650970];
int rec_len_index = 0;
long int rec_loc[42949650970];
int rec_loc_index = 0;
*/





unsigned char       *buffer_for_file;
unsigned char       *arr_converted_bytes;
unsigned char       *bufferreconver;
unsigned char       *fixtolzzbuffer;

unsigned char       *bufferinput;
unsigned char       *bufferoutput;

static char         *suffix = "<EOL><Lrecl:      >";
char                varflname[MAXPATHLEN];
char                tmppath[MAXPATHLEN];

char                *HEX;

struct              stat statbf;

size_t              llzzlocation = 0;
size_t              count_record = 0;
size_t              rec_loc[100000000];

size_t              writesize = 0;

size_t              rec_len_index = 0;
int                 file_to_convert = 0;
int                 file_converted = 0;


short int           rec_len[100000000];
short int           largest_record_size = 0;

int                 threadcount = 16;
pthread_mutex_t     bufferlock;


int convertllzz( void *from );  //int file_converted , int file_to_convert , long long int count_record , short int largest_record_size , short int rec_len[] , long int rec_loc[] , int rec_len_index , unsigned char *arr_converted_bytes , unsigned char *buffer_for_file, char *HEX);

            /*
            TAKES 2 BUFFERS, CONVERTS THE CONTENTS OF THE FIRST INTO FIXWIDTH APPENDS SUFFIX AND PUTS IT INTO THE SECOND W OR WOUT HEX ACCORDING TO ARGUMENT "HEX"
            FINALLY IT WRITES THE CONVERTED BUFFER TO THE FILE AND CLOSES THE FILES
            */

long long int findrecords(void *from); //unsigned char *buffer_for_file , long int llzzlocation , off_t flsize , short int rec_len[] , long int rec_loc[] , int *RecArrIndex , short int *MaxRecordSize);

            /*
            FUNCTION TAKES THE INPUTBUFFER, PUTS LLZZ AND LLZZ LOCATION IN 2 ARRAYS(long int short int rec_len[] , long int rec_loc[])
            FINDS MAXIMAL RECORD LENGTH
            RETURNS HOW MANY RECORDS ARE IN BUFFER - 1
            */

int convertfixline(void *from); //unsigned char *buffer , unsigned char *output , short int Arrreclen[] , long int Arrrecloc[] , int Arrindex , short int Largestrecordsize , char *HEX , int bfsz);

            /*
            CONVERTS THE CONTEXT OF "buffer"(fixedwidth) INTO REGULAR RECORDS "output"
            */
void variablesave();
void variableread();


int main(int argc, char *argv[])
{


int                 debug = -1000;

int                 thread = 0;
int                 currentthread = 0;
//size_t              from = 0;
//size_t              till = 0;
size_t              mod = 0;
pthread_t           IDthread[threadcount];

struct range        threadarg[threadcount];

void                *threadres;



debug = pthread_mutex_init(&bufferlock, NULL);

if(debug != 0)
    {
    perror("Mutex_init failed!\n");
    exit(1);
    }

/*
char *pathin = "/home/oliver/tst/inputfile";
char *pathout = "/home/oliver/tst/out";
char *mode = "LLZZ";
char *hex = "YES";
char *browse = "NO";


argv[1] = mode;
argv[2] = pathin;
argv[3] = pathout;
argv[5] = browse;
argv[4] = hex;
argc += 5;
*/


HEX = argv[4];


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

//checkarg(argv, argc-1, 4, "YES", "NO");                                               //CHECKING IF CLI ARGUMENTS ARE WHAT THEY SUPPOSED TO BE


printf("\n\n                                     Hello \n\n\n\n\n");




    file_to_convert = xopen(argv[2], 0);
    file_converted = xopen(argv[3], 1);
    stat(argv[2], &statbf);



if(strncmp(argv[1], "LLZZ", 5) == 0)
    {
    memcpy(&varflname, argv[3], strnlen(argv[3], MAXPATHLEN));
    //if browse only then open in read ans if not then readwrite



    buffer_for_file = malloc(statbf.st_size);
    debug = read(file_to_convert, buffer_for_file, statbf.st_size);
                                                                                        /*
                                                                                        OPENING OF THE 2 PATH GOTTEN ON THE CLI
                                                                                        STAT ON THE INPUTFILE FOR CONVERSION
                                                                                        ALLOCATING BUFFER FOR INPUTFILE AND READ THE FILE
                                                                                        */



    count_record = findrecords((void *)count_record);

    printf("Amount of records : %zu\n", count_record);
    printf("LARGEST RECORD : %i\n", largest_record_size);



    if(HEX[0] == 'N')
        {
        arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record);
        memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record);
        }
    else
        {
        printf("HEX\n");
        arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
        memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
        }


    mod = rec_len_index%threadcount;


    for(thread = 0 ; thread < threadcount ; thread++)
        {
        printf("%i\n", thread);
        threadarg[thread].from = currentthread*(rec_len_index/threadcount);
        threadarg[thread].till = threadarg[thread].from+(rec_len_index/threadcount);

        if(thread == threadcount-1)
            {
            threadarg[thread].till += mod;
            }

        printf("FROM  %zu\n", threadarg[thread].from);
        printf("TILL  %zu\n\n", threadarg[thread].till);

        debug = pthread_create(&IDthread[thread], NULL, convertllzz, (void *)&threadarg[thread]);
        printf("Debug: %i\n", debug);
        currentthread++;
        }



    for( currentthread = threadcount-1 ; currentthread >= 0 ; currentthread-- )
        {
        printf("%i\n", currentthread);
        debug = pthread_join(IDthread[currentthread], threadres);
        if( debug != 0 )
            {
            perror("Pthread_join\n");
            exit(1);
            }
        }


    write(file_converted, arr_converted_bytes, writesize);
    close(file_converted);
    close(file_to_convert);

    variablesave();

    return 0;
    }

else if(strncmp(argv[1], "FIXED", 6) == 0)
    {
    memcpy(&varflname, argv[2], strnlen(argv[2], MAXPATHLEN));



    bufferreconver = malloc(statbf.st_size);
    fixtolzzbuffer = malloc(statbf.st_size);

    memset(fixtolzzbuffer, 0, statbf.st_size);

    read(file_to_convert, bufferreconver, statbf.st_size);

    variableread();



    memset(fixtolzzbuffer, 0, statbf.st_size);


    mod = rec_len_index%threadcount;

        for(thread = 0 ; thread < threadcount ; thread++)
        {
        printf("%i\n", thread);
        threadarg[thread].from = currentthread*(rec_len_index/threadcount);
        threadarg[thread].till = threadarg[thread].from+(rec_len_index/threadcount);

        if(thread == threadcount-1)
            {
            threadarg[thread].till += mod;
            }

        printf("FROM  %zu\n", threadarg[thread].from);
        printf("TILL  %zu\n\n", threadarg[thread].till);

        debug = pthread_create(&IDthread[thread], NULL, convertfixline, (void *)&threadarg[thread]);
        printf("Debug: %i\n", debug);
        currentthread++;
        }


    for( currentthread = threadcount-1 ; currentthread >= 0 ; currentthread-- )
        {
        printf("%i\n", currentthread);
        debug = pthread_join(IDthread[currentthread], threadres);
        if( debug != 0 )
            {
            perror("Pthread_join\n");
            exit(1);
            }
        }


    pthread_mutex_destroy(&bufferlock);

    write(file_converted, fixtolzzbuffer, writesize);

    close(file_converted);
    close(file_to_convert);
    return 0;

    }

printf("Not valid argument!\n");
return 1;
}













int convertfixline(void *from) //unsigned char *buffer , unsigned char *output , short int Arrreclen[] , long int Arrrecloc[] , int Arrindex , short int Largestrecordsize , char *HEX , int bfsz)
// You need to check if EOL is where it is, and check the length, compare with actual length
    {
    struct range  *tmparg = from;

    struct range  funcarg = *tmparg;

    unsigned char bytes[2];
    unsigned char tmp[2];
    unsigned char upper;
    unsigned char lower;
    int x = 0;
    int debug = 0;
    size_t increment = 0;
    unsigned char hextocharptr;


    //printf("FROM : %zu\n", funcarg.from);
    //printf("TILL : %zu\n", funcarg.till);

    if(memcmp( HEX , "NO", 3) == 0)
        {

        for( x = funcarg.from ; x < funcarg.till ; x++)
            {
            memcpy(&tmp, &rec_len[x], 2);
            bytes[1] = tmp[0];
            bytes[0] = tmp[1];

            memcpy(fixtolzzbuffer+rec_loc[x], &bytes , 2);
            increment = (largest_record_size+SUFFIX_LENGTH-4)*x;
            memcpy(fixtolzzbuffer+rec_loc[x]+4, bufferreconver+increment, rec_len[x]-4);

            debug += rec_len[x];

            }


        }


    if(memcmp( HEX , "YES" , 4 ) == 0)
        {
        printf("HEX\n");


        for( x = funcarg.from ; x < funcarg.till  ; x++ )
            {


            memcpy(&tmp, &rec_len[x], 2);
            bytes[1] = tmp[0];
            bytes[0] = tmp[1];

            //pthread_mutex_lock(&bufferlock);
            memcpy(fixtolzzbuffer+rec_loc[x], &bytes , 2);
            //pthread_mutex_unlock(&bufferlock);

            for( int y = 0 ; y < rec_len[x]-4 ; y++ )
                {

                upper = *(bufferreconver+(largest_record_size+SUFFIX_LENGTH-4)*(x*3+1)+y);
                lower = *(bufferreconver+(largest_record_size+SUFFIX_LENGTH-4)*(x*3+2)+y);
                hextocharptr = unhex(upper, lower);


                //pthread_mutex_lock(&bufferlock);
                fixtolzzbuffer[rec_loc[x]+4+y] = hextocharptr;
                //pthread_mutex_unlock(&bufferlock);


                }

            debug += rec_len[x];
            }

        }



    pthread_mutex_lock(&bufferlock);
    writesize += debug;
    pthread_mutex_unlock(&bufferlock);

    pthread_exit(0);
    }






long long int findrecords(void *from) //unsigned char *buffer_for_file , long int llzzlocation , off_t flsize , short int rec_len[] , long int rec_loc[] , int *RecArrIndex , short int *MaxRecordSize)
    {
    short int length_of_record = 0;

    long long int CurrentRecord = 0;

    while(llzzlocation < statbf.st_size)
    {
    rec_len[rec_len_index] = (buffer_for_file[llzzlocation]*256)+buffer_for_file[llzzlocation+1];
    rec_loc[rec_len_index] = llzzlocation;


    length_of_record = rec_len[rec_len_index];
    rec_len_index += 1;



    llzzlocation += length_of_record;
    if(length_of_record > largest_record_size)
        {
        largest_record_size = length_of_record;
        }
    CurrentRecord++;
   }



    return CurrentRecord;
    }




int convertllzz(void *from) //int file_converted , int file_to_convert , long long int count_record , short int largest_record_size , short int rec_len[] , long int rec_loc[] , int rec_len_index , unsigned char *arr_converted_bytes , unsigned char *buffer_for_file, char *HEX)
    {
    struct  range       *tmparg = from;
    struct  range       funcarg = *tmparg;

    printf("FROM : %zu\n", funcarg.from);
    printf("TILL : %zu\n", funcarg.till);

    size_t              PointerIncrementation = 0;
    size_t              x = 0;
    int                 CharLineLoc = funcarg.from*3;
    int                 HexInd = 0;


    char                tmp_char[19];
    char                record_len_in_str[10];
    unsigned char        LineInHex[40000];



    PointerIncrementation = largest_record_size-4+SUFFIX_LENGTH;



    //BY HEX THE CHARLINELOCKATION INCREMENTATION AND CHARTHEX LOOPVAR IS NOT PROPERLY DONE!

    if(memcmp(HEX, "YES", 4) == 0)
        {
        printf("\n\nHEX\n\n");


        for(x = funcarg.from ; x < funcarg.till ; x++)
            {


            memset(&LineInHex, 0, 40000);
            memset(&tmp_char, 0, 20);
            memset(record_len_in_str, 0, 10);

            sprintf((char *)&record_len_in_str, "%d", rec_len[x]-4);

            strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
            strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
            strncat(tmp_char, ">", 2);

            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc, buffer_for_file+4+rec_loc[x], rec_len[x]-4);
            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc+rec_len[x]-4, tmp_char, strlen(tmp_char));




        pthread_mutex_lock(&bufferlock);


            for( HexInd = 0 ; HexInd < rec_len[x]-4 ; HexInd++)
                {
                chartohex(*(buffer_for_file+4+rec_loc[x]+HexInd),  (char *)&LineInHex);
                arr_converted_bytes[PointerIncrementation*(CharLineLoc+1)+HexInd] = LineInHex[0];
                arr_converted_bytes[PointerIncrementation*(CharLineLoc+2)+HexInd] = LineInHex[1];
                }

                tmp_char[3] = 'H';
                memcpy(arr_converted_bytes+PointerIncrementation*(CharLineLoc+1)+HexInd, tmp_char, SUFFIX_LENGTH);
                memcpy(arr_converted_bytes+PointerIncrementation*(CharLineLoc+2)+HexInd, tmp_char, SUFFIX_LENGTH);

        pthread_mutex_unlock(&bufferlock);

        CharLineLoc += 3;







        }

        pthread_mutex_lock(&bufferlock);

        writesize = (PointerIncrementation)*count_record*3;

        pthread_mutex_unlock(&bufferlock);

        pthread_exit(0);
        }




    for(x = funcarg.from ; x < funcarg.till ; x++)
        {

        memset(&tmp_char, 0, 20);
        memset(record_len_in_str, 0, 10);

        sprintf((char *)&record_len_in_str, "%d", rec_len[x]-4);

        strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
        strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
        strncat(tmp_char, ">", 2);


        //printf("%zu\n", PointerIncrementation*x);
        //printf("%zu\n", rec_loc[x]+4);
        //printf("%hi\n", rec_len[x]-4);

        memcpy(arr_converted_bytes+PointerIncrementation*x, buffer_for_file+4+rec_loc[x], rec_len[x]-4); //Problem by big files

        memcpy(arr_converted_bytes+PointerIncrementation*x+rec_len[x]-4, tmp_char, strlen(tmp_char));
        //printf("\ndone %zu\n", x);
        }


    pthread_mutex_lock(&bufferlock);
    writesize = (PointerIncrementation)*count_record;
    pthread_mutex_unlock(&bufferlock);

    pthread_exit(0);
    }


void variablesave()
    {


    memcpy((char *)&tmppath, varflname, strlen(varflname));
    strncat((char *)&tmppath, "reclen", 7);
    savevar(&rec_len, sizeof(short int), (size_t)rec_len_index, tmppath);

    memset((char *)&tmppath, 0, MAXPATHLEN);

    memcpy((char *)&tmppath, varflname, strlen(varflname));
    strncat((char *)&tmppath, "recloc", 7);
    savevar(&rec_loc, sizeof(long int), (size_t)rec_len_index, tmppath);

    memset((char *)&tmppath, 0, MAXPATHLEN);

    memcpy((char *)&tmppath, varflname, strlen(varflname));
    strncat((char *)&tmppath, "ind", 4);
    savevar(&rec_len_index, sizeof(int), (size_t)0, tmppath);

    memset((char *)&tmppath, 0, MAXPATHLEN);

    memcpy((char *)&tmppath, varflname, strlen(varflname));
    strncat((char *)&tmppath, "max", 4);
    savevar(&largest_record_size, sizeof(short int), (size_t)0, tmppath);

    }

void variableread()
    {

    memcpy((char *)&tmppath, &varflname, strlen(varflname));
    strncat((char *)&tmppath, "ind", 4);

    readvar(&rec_len_index, tmppath, (size_t)1, sizeof(int));

    memset((char *)&tmppath, 0, 4096);

    memcpy((char *)&tmppath, varflname, strlen(varflname));
    strncat((char *)&tmppath, "max", 4);

    readvar(&largest_record_size, tmppath, (size_t)1, sizeof(short int));

    memset((char *)&tmppath, 0, 4096);

    memcpy((char *)&tmppath, varflname, strlen(varflname));
    strncat((char *)&tmppath, "recloc", 7);
    readvar(&rec_loc, tmppath, rec_len_index, sizeof(long int));

    memset((char *)&tmppath, 0, 4096);

    memcpy((char *)&tmppath, varflname, strlen(varflname));
    strncat((char *)&tmppath, "reclen", 7);

    readvar(&rec_len, tmppath, rec_len_index, sizeof(short int));

    }

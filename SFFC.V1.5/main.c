#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "XIO.h"


#define NUMBER_OF_COMMANDLINE_ARGUMENTS 7
#define SUFFIX_LENGTH 19
#define MAXPATHLEN 4096



struct range
    {
    size_t from;
    size_t till;
    int    whoami;
    };


static char         *version = "1.5 -- 2021.05.23";

unsigned char       *arr_converted_bytes;
unsigned char       *bufferinput;
unsigned char       *bufferoutput;

static char         *suffix = "<EOL><Lrecl:      >";
char                varflname[MAXPATHLEN];
char                tmppath[MAXPATHLEN];

char                *HEX;

struct              stat statbf;

size_t              llzzlocation = 0;
size_t              count_record = 0;
size_t              rec_loc[134217728];

size_t              writesize = 0;

size_t              rec_len_index = 0;
int                 file_to_convert = 0;
int                 file_converted = 0;


short int           rec_len[134217728];
short int           largest_record_size = 0;

int                 threadcount = 16;
pthread_mutex_t     bufferlock;


void *convertllzz( void *from );

            /*
            TAKES 2 BUFFERS, CONVERTS THE CONTENTS OF THE FIRST INTO FIXWIDTH APPENDS SUFFIX AND PUTS IT INTO THE SECOND W OR WOUT HEX ACCORDING TO ARGUMENT "HEX"
            FINALLY IT WRITES THE CONVERTED BUFFER TO THE FILE AND CLOSES THE FILES
            */

size_t findrecords( void *from );

            /*
            FUNCTION TAKES THE INPUTBUFFER, PUTS LLZZ AND LLZZ LOCATION IN 2 ARRAYS(long int short int rec_len[] , long int rec_loc[])
            FINDS MAXIMAL RECORD LENGTH
            RETURNS HOW MANY RECORDS ARE IN BUFFER - 1
            */

size_t countrecords();

            /*
            Same as findrecords, but dosent search for biggest record
            */


void *convertfixline( void *from );

            /*
            CONVERTS THE CONTEXT OF "buffer"(fixedwidth) INTO REGULAR RECORDS "output"
            */

void variablesave();
            /*
            SAVES THE VARIABLES TO FILES IN WORKING DIRECTORY FOR THE RECONVERSION
            */
void variableread();
            /*
            READS THE AFFORMENTIOND VARIABLES
            */


int main(int argc, char *argv[])
{

int                 debug = -1000;
int                 thread = 0;
int                 currentthread = 0;

size_t              mod = 0;
pthread_t           IDthread[threadcount];

struct range        threadarg[threadcount];

//void                *threadres;

char                *pathInput = getenv("DD_FILEINP");
char                *pathOutput = getenv("DD_FILEOUT");
char                *current_user = getenv("HOME");

char                *logfilepath = strcat(current_user, "/SFFC_log");

FILE                *logfile_handel;




size_t              linecount = 0;



char                string_suffix[19];
char                lenstr[10];


  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

checkarg(argv, 7, 3, "YES", "NO");

logfile_handel = fopen(logfilepath, "w");

strcut(pathInput, ',');
strcut(pathOutput, ',');







char *logo = "####################\n ##################\n  ################\n   ##############\n    ############\n     ##########\n      ########\n       ######\n        ####\n         ##\n";





fprintf(logfile_handel, "%s\n\n\n", logo);
fprintf(logfile_handel ,"SFFC Version           %s\n", version);
fprintf(logfile_handel ,"Intellectual property of Balint Oliver\n");
fprintf(logfile_handel ,"In case of any issues/requests please contact me:\n");
fprintf(logfile_handel ,"Email                  oliver.balint03@gmail.com\n");
fprintf(logfile_handel ,"GitHub                 https://github.com/princefromplanetmoon/LLZZ_FILECONVERTER\n");
fprintf(logfile_handel ,"Website                emrld.sytes.net\n");

fprintf(logfile_handel ,"\n%s\n", asctime (timeinfo));

fprintf(logfile_handel ,"Inputfile              %s\n", pathInput);
fprintf(logfile_handel ,"Outputfile             %s\n", pathOutput);
for(int n = 0 ; n < argc ; n++)
    {
    fprintf(logfile_handel ,"%i""               %s\n", n,argv[n]);
    }












debug = pthread_mutex_init(&bufferlock, NULL);

if(debug != 0)
    {
    perror("Mutex_init failed!\n");
    exit(1);
    }



HEX = argv[5];


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







    file_to_convert = xopen(pathInput, 0);
    file_converted = xopen(pathOutput, 1);
    debug = stat(pathInput, &statbf);
    if(debug == -1)
        {
        perror("STAT");
        exit(1);
        }



if(strncmp(argv[1], "CVT2NTHE", 5) == 0)
    {
    memcpy(&varflname, pathOutput, strnlen(pathOutput, MAXPATHLEN));



    bufferinput = malloc(statbf.st_size);
    debug = read(file_to_convert, bufferinput, statbf.st_size);


    if(strncmp(argv[2], "LLZZ", 4) == 0 )
        {


    if(strncmp(argv[6], "YES", 3) == 0)
        {
        count_record = countrecords();
        largest_record_size = (short int)atoi(argv[7]);
        }
    if(strncmp(argv[6], "NO", 2) == 0)
        {
        count_record = findrecords((void *)count_record);
        }


    fprintf(logfile_handel ,"Amount of records              %zu\n", count_record);
    fprintf(logfile_handel ,"Largest recordsize             %i\n", largest_record_size-4);
    printf("%i\n", largest_record_size-4);



    if(HEX[0] == 'N')
        {
        arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record);
        memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record);
        }
    else
        {
        //printf("HEX\n");
        //fprintf(logfile_handel ,"Buffersize             %zu\n", (largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
        arr_converted_bytes = malloc((largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
        memset(arr_converted_bytes, 0, (largest_record_size-4+SUFFIX_LENGTH)*count_record*3);
        }


    mod = rec_len_index%threadcount;


    for(thread = 0 ; thread < threadcount ; thread++)
        {
        //printf("%i\n", thread);
        threadarg[thread].from = currentthread*(rec_len_index/threadcount);
        threadarg[thread].till = threadarg[thread].from+(rec_len_index/threadcount);

        if(thread == threadcount-1)
            {
            threadarg[thread].till += mod;
            }

        //printf("FROM  %zu\n", threadarg[thread].from);
        //printf("TILL  %zu\n\n", threadarg[thread].till);

        debug = pthread_create(&IDthread[thread], NULL, convertllzz, (void *)&threadarg[thread]);
        //printf("Debug: %i\n", debug);
        currentthread++;
        }



    for( currentthread = threadcount-1 ; currentthread >= 0 ; currentthread-- )
        {
  //      printf("%i\n", currentthread);
        debug = pthread_join(IDthread[currentthread], NULL);
        if( debug != 0 )
            {
            perror("Pthread_join\n");
            exit(1);
            }
        }


    write(file_converted, arr_converted_bytes, writesize);
    close(file_converted);
    close(file_to_convert);
    fclose(logfile_handel);

    variablesave();
    exit(0);
        }




    if(strncmp(argv[2], "FIXED", 5) == 0)
        {
        if(argc < 8)
            {
            printf("Not enough CLI arguments!\n(record lenth?)\n");
            exit(1);
            }

        largest_record_size = (short int)atoi(argv[7]);
        printf("%i\n", largest_record_size);



        linecount = statbf.st_size/largest_record_size;
        //printf("Linecount : %zu\n", linecount);

        bufferoutput = malloc(linecount*(largest_record_size+SUFFIX_LENGTH+1));

        memset(string_suffix,0, SUFFIX_LENGTH+1);
        memset(lenstr,0, 10);
        memset(bufferoutput, 0, linecount*(largest_record_size+SUFFIX_LENGTH+1));

        sprintf((char *)&lenstr, "%d", largest_record_size);

        //printf("%s\n", lenstr);

        strncpy((char *)&string_suffix, suffix, strlen(suffix)-strlen(lenstr));
        strncat((char *)&string_suffix, lenstr, strlen(lenstr));
        strncat((char *)&string_suffix, ">", 2);
        //printf("SUffix : %s\n", string_suffix);


        for(int n = 0 ; n < linecount ; n++)
            {
            //printf("%i\n", n);

            memcpy(bufferoutput+(n*(largest_record_size+SUFFIX_LENGTH+1)), bufferinput+(n*(largest_record_size)), largest_record_size);
            memcpy(bufferoutput+(n*(largest_record_size+SUFFIX_LENGTH+1)+largest_record_size), string_suffix, SUFFIX_LENGTH+1);
            }

        write(file_converted ,bufferoutput, linecount*(largest_record_size+SUFFIX_LENGTH+1));
        close(file_converted);
        close(file_to_convert);
        fclose(logfile_handel);
        exit(0);
        }
    }



else if(strncmp(argv[1], "CVT4NTHE", 6) == 0)
    {
    memcpy(&varflname, pathInput, strnlen(pathInput, MAXPATHLEN));



    bufferinput = malloc(statbf.st_size);
    bufferoutput = malloc(statbf.st_size);

    //memset(fixtolzzbuffer, 0, statbf.st_size);

    read(file_to_convert, bufferinput, statbf.st_size);

    variableread();


    //memset(fixtolzzbuffer, 0, statbf.st_size);

    printf("%i\n", largest_record_size);
    fprintf(logfile_handel ,"Amount of records              %zu\n", count_record);
    fprintf(logfile_handel ,"Largest recordsize             %i\n", largest_record_size-4);

    mod = rec_len_index%threadcount;

        for(thread = 0 ; thread < threadcount ; thread++)
        {
        //printf("%i\n", thread);
        threadarg[thread].from = currentthread*(rec_len_index/threadcount);
        threadarg[thread].till = threadarg[thread].from+(rec_len_index/threadcount);

        if(thread == threadcount-1)
            {
            threadarg[thread].till += mod;
            }

        //printf("FROM  %zu\n", threadarg[thread].from);
        //printf("TILL  %zu\n\n", threadarg[thread].till);

        debug = pthread_create(&IDthread[thread], NULL, convertfixline, (void *)&threadarg[thread]);
        //printf("Debug: %i\n", debug);
        currentthread++;
        }



    for( currentthread = threadcount-1 ; currentthread >= 0 ; currentthread-- )
        {
        //printf("%i\n", currentthread);
        debug = pthread_join(IDthread[currentthread], NULL);
        if( debug != 0 )
            {
            perror("Pthread_join\n");
            exit(1);
            }
        }


    pthread_mutex_destroy(&bufferlock);

    write(file_converted, bufferoutput, writesize);

    close(file_converted);
    close(file_to_convert);
    fclose(logfile_handel);
    return 0;

    }

printf("Not valid argument!\n");
return 1;
}








void *convertfixline(void *from)
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

            memcpy(bufferoutput+rec_loc[x], &bytes , 2);
            increment = (largest_record_size+SUFFIX_LENGTH-4)*x;
            memcpy(bufferoutput+rec_loc[x]+4, bufferinput+increment, rec_len[x]-4);

            debug += rec_len[x];

            }


        }


    if(memcmp( HEX , "YES" , 4 ) == 0)
        {
        //printf("HEX\n");


        for( x = funcarg.from ; x < funcarg.till  ; x++ )
            {


            memcpy(&tmp, &rec_len[x], 2);
            bytes[1] = tmp[0];
            bytes[0] = tmp[1];

            //pthread_mutex_lock(&bufferlock);
            memcpy(bufferoutput+rec_loc[x], &bytes , 2);
            //pthread_mutex_unlock(&bufferlock);

            for( int y = 0 ; y < rec_len[x]-4 ; y++ )
                {

                upper = *(bufferinput+(largest_record_size+SUFFIX_LENGTH-4)*(x*3+1)+y);
                lower = *(bufferinput+(largest_record_size+SUFFIX_LENGTH-4)*(x*3+2)+y);
                hextocharptr = unhex(upper, lower);


                //pthread_mutex_lock(&bufferlock);
                bufferoutput[rec_loc[x]+4+y] = hextocharptr;
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



size_t countrecords()
    {
    short int length_of_record = 0;

    size_t CurrentRecord = 0;

    while(llzzlocation < statbf.st_size)
        {
        rec_len[rec_len_index] = (bufferinput[llzzlocation]*256)+bufferinput[llzzlocation+1];
        rec_loc[rec_len_index] = llzzlocation;
        length_of_record = rec_len[rec_len_index];
        rec_len_index += 1;
        llzzlocation += length_of_record;
        CurrentRecord++;
       }



    return CurrentRecord;
    }


size_t findrecords(void *from)
   {
    short int length_of_record = 0;

    size_t CurrentRecord = 0;

    while(llzzlocation < statbf.st_size)
    {
    rec_len[rec_len_index] = (bufferinput[llzzlocation]*256)+bufferinput[llzzlocation+1];
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




void *convertllzz(void *from)
    {
    struct  range       *tmparg = from;
    struct  range       funcarg = *tmparg;

   // printf("FROM : %zu\n", funcarg.from);
    //printf("TILL : %zu\n", funcarg.till);

    size_t              PointerIncrementation = 0;
    size_t              x = 0;
    int                 CharLineLoc = funcarg.from*3;
    int                 HexInd = 0;


    char                tmp_char[19];
    char                record_len_in_str[10];
    unsigned char        LineInHex[40000];



    PointerIncrementation = largest_record_size-4+SUFFIX_LENGTH;




    if(memcmp(HEX, "YES", 4) == 0)
        {
        //printf("\n\nHEX\n\n");


        for(x = funcarg.from ; x < funcarg.till ; x++)
            {


            memset(&tmp_char, 0, 20);
            memset(record_len_in_str, 0, 10);

            sprintf((char *)&record_len_in_str, "%d", rec_len[x]-4);

            strncpy(tmp_char, suffix, strlen(suffix)-strlen(record_len_in_str)-1);
            strncat(tmp_char, record_len_in_str, strlen(record_len_in_str));
            strncat(tmp_char, ">", 2);

            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc, bufferinput+4+rec_loc[x], rec_len[x]-4);
            memcpy(arr_converted_bytes+PointerIncrementation*CharLineLoc+rec_len[x]-4, tmp_char, strlen(tmp_char));




        //pthread_mutex_lock(&bufferlock);


            for( HexInd = 0 ; HexInd < rec_len[x]-4 ; HexInd++)
                {
                chartohex(*(bufferinput+4+rec_loc[x]+HexInd),  (char *)&LineInHex);
                arr_converted_bytes[PointerIncrementation*(CharLineLoc+1)+HexInd] = LineInHex[0];
                arr_converted_bytes[PointerIncrementation*(CharLineLoc+2)+HexInd] = LineInHex[1];
                }

                tmp_char[3] = 'H';
                memcpy(arr_converted_bytes+PointerIncrementation*(CharLineLoc+1)+HexInd, tmp_char, SUFFIX_LENGTH);
                memcpy(arr_converted_bytes+PointerIncrementation*(CharLineLoc+2)+HexInd, tmp_char, SUFFIX_LENGTH);

        //pthread_mutex_unlock(&bufferlock);

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

        //printf("Suffix : %s\n", tmp_char);
        //printf("%zu\n", x);


        //printf("%zu\n", PointerIncrementation*x);
        //printf("%zu\n", rec_loc[x]+4);
        //printf("%hi\n", rec_len[x]-4);

        memcpy(arr_converted_bytes+PointerIncrementation*x, bufferinput+4+rec_loc[x], rec_len[x]-4);

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

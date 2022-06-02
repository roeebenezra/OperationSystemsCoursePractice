// Named pipe - Arr Filler

//-----------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h> 

//----------------------------------------
#define ARR_SIZE 10000
#define NUM_OF_GEN 3
#define ARGC 5

typedef enum {false, true} bool;

//------------prototypes------------------

FILE* open_file(char *file_name, char* mode);
void make_fifo(char fifo_name[]);
void do_filler();
void producers_wait(FILE* fdr, FILE* fdw[]);
void print(bool primeCount[]);

//======== main ===========
int main(int argc, char* argv[])
{
	if(argc != ARGC)
    {
		printf("wrong argc num program %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
	do_filler(argv);
	
    return EXIT_SUCCESS;
}

//-------------------------
void do_filler(char* argv[])
{
    int i,arr[ARR_SIZE],id,prime;
    bool primeCount[ARR_SIZE] = {0};
    
	make_fifo(argv[1]);
    FILE* fdw[NUM_OF_GEN], *fdr = open_file(argv[1],"r"); 

    sleep(5);
    for(i = 2 ; i < ARGC ; i++)     //open to write fifo's
        fdw[i-2] = open_file(argv[i],"w");
    
    producers_wait(fdr,fdw);
    
    i = 0;
    while(i < ARR_SIZE)
    {
        fscanf(fdr, "%d",&id);
        fscanf(fdr, "%d",&prime);
        
        arr[i] = prime;
        fprintf(fdw[id-1]," %d",primeCount[prime]);
        primeCount[prime]++;
        fflush(fdw[id-1]);    
        i++;
    }
    
    for(i = 0 ; i< NUM_OF_GEN ; i++)
    {
        fprintf(fdw[i]," %d",-1);
        fflush(fdw[i]);
    }

    fclose(fdr);
    for(i = 0 ; i< NUM_OF_GEN ; i++)
        fclose(fdw[i]);

    print(primeCount);
    unlink(argv[1]);
}
//----------------------------------------------------
void print(bool primeCount[])
{
    int i, counter = 0, min = ARR_SIZE , max = 0;

    for(i = 0 ; i < ARR_SIZE ; i++)
    {
        if(primeCount[i] > 0){
            counter++;
            if(i > max)
                max = i;
            if(i < min)
                min = i;
        }
    }
    printf("diffrent:%d min:%d max:%d",counter,min,max);
}

//------------------------------------------
void producers_wait(FILE* fdr, FILE* fdw[])
{
    int i, id_num;
    for(i = 0 ; i < NUM_OF_GEN ; i++)
        fscanf(fdr, "%d", &id_num);
    for(i = 0 ; i < NUM_OF_GEN ; i++)
        fprintf(fdw[i]," %d",1);
}

//--------------------------------------------------------
void make_fifo(char fifo_name[])
{
    if((mkfifo(fifo_name,S_IFIFO|0644) == -1 && errno != EEXIST))
    {
		perror("cannot create fifo file\n");
		exit(EXIT_FAILURE);
	}
}

//------------------------------------------
FILE* open_file(char *file_name, char* mode)
{
    FILE* fp = open(file_name, mode);
    if(!fp)
    {
        perror("cannot open file\n");
        exit(EXIT_FAILURE);
    }

    return fp;
}

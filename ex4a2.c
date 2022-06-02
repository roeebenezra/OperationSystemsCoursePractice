//Named pipe - Numbers Creator

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h> 
//-----------------------------

#define ARGC 3
#define F_NAME_SIZE 6
#define FIFO_NAME "fifo"

//--------------------------------

typedef enum {false, true} bool;

//---------------------------------

bool is_prime(int num);
FILE* open_file(char* file_name, char* mode);
void make_fifo(char fifo_name[]);
void do_creator(char my_fifo[], char filler_fifo[], int id);
void start_signal(FILE* fdw, FILE* fdr, int id);

//----------- main ----------------
int main(int argc, char* argv[])
{
    if(argc != ARGC)
    {
        printf("wrong argc num prog %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char fifo_name[F_NAME_SIZE];
    strcpy(fifo_name,FIFO_NAME);
    strcat(fifo_name,argv[2]);  //fifo name now = "fifox"
    
    int id = atoi(argv[2]);
	srand(atoi(argv[2]));       //sent id seed to srand

    do_creator(fifo_name,argv[1],id);


    return EXIT_SUCCESS;
}

//----------------------------------------------------------
void do_creator(char my_fifo[], char filler_fifo[] ,int id)
{
    int max_shows = 0, new_shows = 0, num, shows;

    make_fifo(my_fifo);

    FILE* fdw = open_file(my_fifo,"r");         //read from filler
    FILE* fdr = open_file(filler_fifo,"w");     //write to filler
    
    start_signal(fdw,fdr,id);
   
    while(1)
    {
        num = rand() % 999+2;
        if(is_prime(num))
        {
            fprintf(fdw, " %d",id);  //write to fifo0 prime num
            fprintf(fdw," %d",num);
            fflush(fdw);

            fscanf(fdr,"%d",&shows);
            
            if(shows == 0)        
                new_shows++;
            if(shows > max_shows)
                max_shows = shows;
            if(shows == -1)
                break;
        }
    }
    printf("creator %d- new:%d,max:%d\n",id,new_shows,max_shows);
    unlink(my_fifo);
}
//---------------------------------------------
void start_signal(FILE* fdw, FILE* fdr, int id)
{
    int sign;
    fprintf(fdw," %d",id);
    fscanf(fdr,"%d",&sign);
}

//------------------------------------
//prime number function
bool is_prime(int num)
{
    int div;
    for(div = 2 ; div <= num/2 ; div++)
    {
		if (num % div == 0)
			return false;
	}
    return true;
}
//-------------------------------------------
FILE* open_file(char *file_name, char* mode)
{
    FILE* fp = fopen(file_name,mode);
    if(!fp) 
    {
        perror("cannot open file\n");
        exit(EXIT_FAILURE);
    }
    
    return fp;
}
//---------------------------------------------------------
void make_fifo(char fifo_name[])
{
    if((mkfifo(fifo_name,S_IFIFO|0644) == -1 && errno != EEXIST))
    {
		perror("cannot create fifo file\n");
		exit(EXIT_FAILURE);
	}
}

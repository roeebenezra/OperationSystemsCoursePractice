// *** Array Owner ***
// SHARED MEMORY

//-------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//-----------------------------------------

#define SHM_SIZE 1000
#define ZERO 0
#define ONE 1
#define LOCK_POS 4
#define FAIL_SIGN -1
#define ACCESS_KIND 0600
#define PRIMES_START_CELL 5

//-------------------------------------------

void sig_handler(int );
key_t key_tok();
int shm_id_get(key_t);
int *shm_ptr_get(int);
void print_shmem_data(int* );
int count_distinct(int *shm_ptr, int n);

//------------------------------------------------------------------
int main()
{
    signal(SIGUSR1,sig_hendler);

    key_t key = key_tok();              //ftok
    int shm_id = shm_id_get(key);       //shmget
    int *shm_ptr = shm_ptr_get(shm_id); //shmat

    pause();             //until get signal that memory filled
    
    print_shmem_data(shm_ptr);

	shmdt(shm_ptr);                 //delete pointer
	shmctl(shm_id,IPC_RMID,NULL);   //remove shm

    return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
void print_shmem_data(int *shm_ptr)
{
    int min = shm_ptr[PRIMES_START_CELL],
        max = ZERO, diff = ZERO;

    diff = count_distinct(shm_ptr,SHM_SIZE);    //diff values num search

    for(int i = PRIMES_START_CELL; i < SHM_SIZE ; i++){  //max & min search
        if(shm_ptr[i] < min)
            min = shm_ptr[i];
        if(shm_ptr[i] > max)
            max = shm_ptr[i];
    }
    printf("\ndifferent values = %d | minimum value = %d| maximum value = %d\n",diff,min,max);
}
//-----------------------------------------------------------------------------
// search for different values number in shared memory
int count_distinct(int *shm_ptr, int n)
{
    int diff = 1;
 
    for (int i = PRIMES_START_CELL ; i < SHM_SIZE; i++) {
        int j = PRIMES_START_CELL;
        for ( ; j < i; j++){
            if (shm_ptr[i] == shm_ptr[j])   //if allready seen in memory
                break;
        }
        if (i == j)   //if first showen
            diff++;
    }

    return diff;
}
//-------------------------------------------------------
void sig_handler(int sig_num)
{
    return;
}
//-------------------------------------------------------
//key produce func
key_t key_tok()
{
    key_t key = ftok(".",'5');
    if(key == FAIL_SIGN){
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    return key;
}
//-------------------------------------------------------------------
//shared memory id get
int shm_id_get(key_t key)
{
    int shm_id = shmget(key,SHM_SIZE,IPC_CREAT|IPC_EXCL|ACCESS_KIND);
    if(shm_id == FAIL_SIGN){
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    
    return shm_id;
}
//-------------------------------------------------------------------
//shared memory pointer get
int *shm_ptr_get(int shm_id)
{
    int *shm_ptr = (int*)shmat(shm_id,NULL,0);
    if(shm_ptr == (int*)FAIL_SIGN){
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    shm_ptr[ZERO] = getpid();   //put pid in first cell
    shm_ptr[LOCK_POS] = ONE;    //open lock cell

    return shm_ptr;
}


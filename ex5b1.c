// *** prime server ***
// 
//-----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//-----------------------------

#define ZERO 0
#define ONE 1
#define TWO 2

#define SHM_SIZE     4
#define FAIL_SIGN    -1
#define MY_PID       0
#define CLIENT_PID   1
#define PRIME_PLACE  2
#define TEST_RESULT  3
#define ACCESS_KIND  0600

//--------------------------------
typedef enum {false, true} bool;
//--------------------------------
int INT_SIGNAL = 0;
//--------------------------------

bool is_prime(int num);
key_t key_tok();
int shm_id_get(key_t);
int *shm_ptr_get(int);
void sigusr1_handler(int sig_num){}
void sigint_handler(int sig_num);
void primes_check(int shm_id, int* shm_ptr);


//-------------------------------------------
int main()
{
    signal(SIGUSR1,sigusr1_handler);
    signal(SIGINT,sigint_handler);

    key_t key = key_tok();
    int shm_id = shm_id_get(key);
    int* shm_ptr = shm_ptr_get(shm_id);

    primes_check(shm_id,shm_ptr);


    return EXIT_SUCCESS;
}
//------------------------------------------
void primes_check(int shm_id, int* shm_ptr)
{

    while(!INT_SIGNAL)
    {
        pause();
        shm_ptr[TEST_RESULT] = is_prime(shm_ptr[PRIME_PLACE]);
        kill(shm_ptr[CLIENT_PID],SIGUSR1);
    }

	shmdt(shm_ptr);                 //delete pointer
	shmctl(shm_id,IPC_RMID,NULL);   //remove shm
}

//-------------------------------
void sigint_handler(int sig_num)
{
    INT_SIGNAL = ONE;
}

//---------------------------------
//prime number function
bool is_prime(int num)
{
    if(num < TWO)
        return false;

    int div;
    for(div = TWO ; div <= num / TWO ; div++)
    {
		if (num % div == ZERO)
			return false;
	}
    return true;
}
//----------------------------
//key produce func
key_t key_tok()
{
    key_t key = ftok(".",'p');
    if(key == FAIL_SIGN){
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    return key;
}
//------------------------------------------------
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
//------------------------------------------------
//shared memory pointer get
int *shm_ptr_get(int shm_id)
{
    int *shm_ptr = (int*)shmat(shm_id,NULL,ZERO);
    if(shm_ptr == (int*)FAIL_SIGN){
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    shm_ptr[MY_PID] = getpid();   //put pid in first cell

    return shm_ptr;
}

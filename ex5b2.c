// *** palindrome server ***

//-------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//---------------------

#define ZERO 0
#define ONE 1
#define TWO 2

#define SHM_SIZE    22
#define FAIL_SIGN   -1
#define MY_PID       0
#define CLIENT_PID   1
#define SERIES_START 2
#define SERIES_END   21
#define TEST_RESULT  21
#define ACCESS_KIND  0600

//----------------------------------
typedef enum {false, true} bool;
//----------------------------------
int INT_SIGNAL = 0;
//----------------------------------

key_t key_tok();
int shm_id_get(key_t);
int *shm_ptr_get(int);
void sigusr2_handler(int sig_num){}
void sigint_handler(int sig_num);
void palindrome_check(int shm_id, int* shm_ptr);
bool is_palindrome(int *shm_ptr ,int size);

//---------------------------------------------------

int main()
{
    signal(SIGUSR2,sigusr2_handler);
    signal(SIGINT,sigint_handler);

    key_t key = key_tok();
    int shm_id = shm_id_get(key);
    int* shm_ptr = shm_ptr_get(shm_id);

    pilandrome_check(shm_id,shm_ptr);

    return EXIT_SUCCESS;
}

//------------------------------------------------------
void palindrome_check(int shm_id, int* shm_ptr)
{
    int i = SERIES_START, palindrome_size = ZERO;

    while(!INT_SIGNAL)
    {
        pause();
        i = SERIES_START;
        palindrome_size = ZERO;
        while (shm_ptr[i] != ZERO && i < SERIES_END){
            palindrome_size++;
            i++;
        }
        shm_ptr[TEST_RESULT] = is_palindrome(shm_ptr, palindrome_size);
        kill(shm_ptr[CLIENT_PID],SIGUSR2);
    }

    shmdt(shm_ptr);                 //delete pointer
	shmctl(shm_id,IPC_RMID,NULL);   //remove shm

}
//------------------------------------------------------------------------------
//palindrome check
bool is_palindrome(int *shm_ptr ,int size)
{
    int i, j, flag = ZERO ;
    size = size+TWO;

    for(i = SERIES_START, j = size - ONE ; i < size / TWO, j >= size / TWO ; i++, j--){
        if(shm_ptr[i] != shm_ptr[j]) {
            flag = ONE;
            break;
        }
    }
    if (flag == ONE)
        return false;
    else
        return true;
}
//-------------------------------------------------------------------------------
void sigint_handler(int sig_num)
{
    INT_SIGNAL = ONE;
}
//------------------------------------------------------
//key produce func
key_t key_tok()
{
    key_t key = ftok(".",'q');
    if(key == FAIL_SIGN){
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    return key;
}
//-------------------------------------------------------------------
//shered memory id get
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
    int *shm_ptr = (int*)shmat(shm_id,NULL,ZERO);
    if(shm_ptr == (int*)FAIL_SIGN){
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    shm_ptr[MY_PID] = getpid();   //put pid in first cell

    return shm_ptr;
}

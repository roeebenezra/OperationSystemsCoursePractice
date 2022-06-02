// Numbers Creator
//----------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//-----------------------------------------

#define SHM_SIZE 1000
#define FAIL_SIGN -1
#define ZERO 0
#define ONE 1
#define TWO 2
#define TREE 3
#define LOCK_POS 4
#define PRIMES_START_CELL 5
#define ACCESS_KIND 0600
#define ARGC 2
#define RANGE 999

//-----------------------------------------
typedef enum {false, true} bool;
//------------------------------------------

void add_primes_to_shmem(int *shm_ptr,int index);
bool is_prime(int num);
int is_in_shmem(int *ptr,int fill_place, int num);
key_t key_tok();
int shm_id_get(key_t);
int *shm_ptr_get(int);

//-----------------------------------------------
int main(int argc, char* argv[])
{
    if(argc != ARGC){
        perror("wrong arg number\n");
        exit(EXIT_FAILURE);
    }
    int index = atoi(argv[ONE]);
    key_t key = key_tok();
    int shm_id = shm_id_get(key);
    int *shm_ptr = shm_ptr_get(shm_id);

    shm_ptr[index] = ONE; //insert one to his creator cell 

    add_primes_to_shmem(shm_ptr,index);   //MAIN MISSION
	
	shmdt(shm_ptr); //delete pointer
 
    return EXIT_SUCCESS;
}
//----------------------------------------------
//primes add to shared memory
void add_primes_to_shmem(int *shm_ptr,int index)
{
    int i = PRIMES_START_CELL,
        num,
        new = ZERO,
        max = ZERO;
    srand(index);

    //wait for two other processes to start
    while ( !(shm_ptr[ONE]) || !(shm_ptr[TWO]) || !(shm_ptr[TREE]) ){} 
    
    while(ONE){
        num = rand() % RANGE+TWO ;
        if(is_prime(num))
        {
            while(!shm_ptr[LOCK_POS]){} //wait until lock = 1

            shm_ptr[LOCK_POS] = ZERO; 
            i = PRIMES_START_CELL;
             //search for empty cell
            while (shm_ptr[i] != ZERO && i < SHM_SIZE){
                i++;
            }
            if(i == SHM_SIZE){              //if shared memory full - finish
                kill(shm_ptr[ZERO],SIGUSR1);//signal to array owner
                shm_ptr[LOCK_POS] = ONE;    //release lock
                break;
            }
            else 
            {    //place prime in shared memory
                int shows = is_in_shmem(shm_ptr,i,num);
                if(shows == ZERO)
                    new++;
                if(shows > max)
                    max = shows;
                shm_ptr[i] = num;
            }       
            shm_ptr[LOCK_POS] = ONE;    //open lock to other process
        }
    }
    printf("new primes added:%d | maximum shows:%d\n",new,max);
}
//-----------------------------------------------------------------------------
//random number check if in shared memory
int is_in_shmem(int *ptr,int fill_place, int num)
{
    int counter = ZERO;
    for(int i = PRIMES_START_CELL ; i < fill_place ; i++){
        if(num == ptr[i])
            counter++;
    }
    return counter;
}
//-------------------------------------------------------------------
//prime number function
bool is_prime(int num)
{
    int div;
    for(div = 2 ; div <= num/2 ; div++)
    {
		if (num % div == ZERO)
			return false;
	}
    return true;
}
//-------------------------------------------------------
key_t key_tok()
{
    key_t key = ftok(".",'5');
    if(key == FAIL_SIGN){
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    return key;
}
//-------------------------------------------------------
int shm_id_get(key_t key)
{

    int shm_id = shmget(key,ZERO,ACCESS_KIND);
    if(shm_id == FAIL_SIGN){
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    return shm_id;
}
//------------------------------------------------------
int *shm_ptr_get(int shm_id)
{
    int *shm_ptr = (int*)shmat(shm_id,NULL,ZERO);
    if(shm_ptr == (int*)FAIL_SIGN){
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    return shm_ptr;
}

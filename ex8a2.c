/*
  ex8a2.c creator
  Written by Yinon Tzomi 208489369
  yinontz & roibe
  
   This program creates prime numbers,
   writes them in shared memory.
   when the program wants to write number,
   it waits to the semaphore.
   when the shared memory is full, the program
   prints how many new numbers it sent and
   how many times it sent the same number.

   compile:
   gcc -Wall -o ex8a2 -lpthread ex8a2.c

   run:
   ***before running you must run ex8a1
   and the run it 3 times***

   ./ex8a2 ID

   input - NON
   output -  how many new numbers it sent and
   	   	   	 how many times it sent the same number.

 */

//---------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <semaphore.h>

//----------------------
#define OPEN 1
#define CLOSE 0
#define ID 1
#define MAX_PRIME 999
#define INSERT 2
#define MANNAGING_PLACES 4
#define FILLER_PLACE 0
#define ARR_SIZE 1000
#define SEMAPHORE_NAME "/my_mutex20"

//----------------------
void do_creator(int primes[], int child_num, sem_t *mutex);

int wait_and_push_back(int primes[], int prime_num, sem_t *mutex);

int is_prime(int num);

int howManyTimesIsExist(const int numbers[], int num);

void wait_for_others(const int numbers[]);

sem_t *connect_to_semaphore();

int *connect_to_shm();

void start(int *ptr, int child_num);

//-------------------------------
int main(int argc, char **argv) {

    int child_num;
    int *ptr;
    sem_t *mutex;

    if (argc != 2) {
        fprintf(stderr, "arguments problems");
        exit(EXIT_FAILURE);
    }

    child_num = atoi(argv[ID]);

    ptr = connect_to_shm();
    mutex = connect_to_semaphore();

    start(ptr, child_num);

    srand(child_num);

    wait_for_others(ptr);

    do_creator(ptr, child_num, mutex);

    kill(ptr[FILLER_PLACE], SIGUSR1);

    shmdt(ptr);

    sem_close(mutex);

    return EXIT_SUCCESS;
}

//---------------------------------------------------------
//Receives Shared memory - array of primes, ID and semaphore
//runs until the shm is full,
//creates prime numbers and writes them in the shm
//when finishes, write the output as written
void do_creator(int primes[], int child_num, sem_t *mutex) {

    int prime_num, answer
    , howMenyNew = 0, the_most_times = 0;
    int run = 1;

    while (run) {

        prime_num = rand();

        if (is_prime(prime_num)) {
            run = wait_and_push_back(primes, prime_num, mutex);

            if (run) {
                answer = (howManyTimesIsExist(primes, prime_num) - 1);
                if (answer == 0)
                    howMenyNew++;
                if (answer > the_most_times)
                    the_most_times = answer;
            }
        }
    }

    printf("creator %d sent %d new numbers\n"
           "the most times is: %d\n",
           child_num, howMenyNew, the_most_times);
}
//-----------------------------------------------------
/*receives shm - array of number, number and semaphore.
  returns if the task is done or not (because the shared is full)
  wait to the lock to opens, closes it writes the number
  in the shm and then opens the lock again
 */
int wait_and_push_back(int primes[], int prime_num, sem_t *mutex) {
    int i;

    sem_wait(mutex);

    for (i = MANNAGING_PLACES; i < ARR_SIZE; i++)
        if (primes[i] == 0) {
            primes[i] = prime_num;
            break;
        }

    sem_post(mutex);
    if (i == ARR_SIZE)
        return 0;

    return 1;
}

//--------------------
//checks if number is prime
int is_prime(int num) {
    int i;

    for (i = 2; (i * i) <= num; i++)
        if ((num % i) == 0)
            return 0; //not prime
    return 1;
}

//-------------------------
//receives array and number.
//return how many times this number shown inside the array.
int howManyTimesIsExist(const int numbers[], int num) {

    int i;
    int count = 0;

    for (i = MANNAGING_PLACES; i < ARR_SIZE; i++)
        if (numbers[i] == num)
            count++;

    return count;
}

//---------------------------------------
void wait_for_others(const int numbers[]) {

    int i;
    int wait = 1;

    while (wait == 1) {
        wait = 0;
        for (i = 0; i < MANNAGING_PLACES; i++)
            if (numbers[i] == CLOSE)
                wait = 1;
    }
}

//----------------------
sem_t *connect_to_semaphore() {

    sem_t *mutex = NULL;

    mutex = sem_open(SEMAPHORE_NAME, 0);
    if (mutex == SEM_FAILED) {
        perror("sem open failed");
        exit(EXIT_FAILURE);
    }
    return mutex;
}

//---------------------
int *connect_to_shm() {

    int *ptr;
    key_t key;
    int shm_id;

    key = ftok(".", '8');

    if ((shm_id = shmget(key, 0, 0600)) == -1) {
        perror("Failed to get memory");
        exit(EXIT_FAILURE);
    }

    if ((ptr = (int *) shmat(shm_id, NULL, 0)) < 0) {
        perror("Failed to attach memory");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

//-------------------------------------------
//this function means that all the producers
//must wait one to others. when all the producers up
//they can go.
void start(int *ptr, int child_num) {
    ptr[child_num] = OPEN;
}


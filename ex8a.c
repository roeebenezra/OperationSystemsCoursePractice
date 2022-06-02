/*
 * ex8a1.c -filler
   Written by Yinon Tzomi 208489369
   yinontz & roibe

   This program Creates shared memory of numbers,
   and semaphore
   then goes to pause.
   When the program gets signal (sigusr1),
   it wakes up and prints how much different numbers he has,
   who is the biggest and who is the smallest

   compile: gcc -Wall -o ex8a -lpthread ex8a.c
   run: ./ex8a1

   input: NONE
   output: as written

   race conditions: it depends on which process get the processor most of the time

 */
//------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

//------------------
#define ARR_SIZE 1000
#define MANNAGING_PLACES 4
#define OWNER_PLACE 0
#define INITIAL_VALUE MANNAGING_PLACES
#define SEMAPHORE_NAME "/my_mutex20"

//------------------
struct Data {
    int
            _bigger,
            _smaller;
};

//----------------------------------------------------
struct Data find_biggest_and_smallest(const int arr[]);

void initial_data(struct Data *data, int init);

int find_amount_different(const int numbers[]);

void catch_term(int sig_num);

void catch_usr1(int sig_num);

sem_t *create_semaphore();

void start();

int *create_shm();

//------shm_date--------
int *ptr;
int shm_id;

//======== main ==========
int main() {

    sem_t *mutex;
    struct Data data;
    int how_many_diff;


    signal(SIGINT, catch_term);
    signal(SIGUSR1, catch_usr1);


    ptr = create_shm();
    mutex = create_semaphore();

    start();

    pause();

    data = find_biggest_and_smallest(ptr);
    how_many_diff = find_amount_different(ptr);

    printf("There are %d different numbers\n"
           "The bigger is %d\nThe smaller is: %d\n",
           how_many_diff, data._bigger, data._smaller);

    shmdt(ptr);
    shmctl(shm_id, IPC_RMID, NULL);

    sem_close(mutex);
    sem_unlink(SEMAPHORE_NAME);

    return EXIT_SUCCESS;
}

//======================================
//parameter: array of numbers
//return: Data
//goes over the array and checks who is the biggest and the smallest
struct Data find_biggest_and_smallest(const int numbers[]) {
    struct Data data;
    int i;
    initial_data(&data, numbers[INITIAL_VALUE]);

    for (i = MANNAGING_PLACES; i < ARR_SIZE; i++) {
        if (numbers[i] < data._smaller)
            data._smaller = numbers[i];
        if (numbers[i] > data._bigger)
            data._bigger = numbers[i];
    }
    return data;
}

//---------------------------------
//assume that our array is not empty
void initial_data(struct Data *data, int initial) {
    data->_bigger = initial;
    data->_smaller = initial;
}

//--------------------------
//parameter: array of numbers
//return: how much different numbers there are in the array
int find_amount_different(const int numbers[]) {

    int i, j;
    int res = 1;

    for (i = MANNAGING_PLACES + 1; i < ARR_SIZE; i++) {

        j = MANNAGING_PLACES;
        for (j = MANNAGING_PLACES; j < i; j++)
            if (numbers[i] == numbers[j])
                break;

        if (i == j)
            res++;
    }
    return res;
}

//----------------------------
void catch_term(int sig_num) {

    shmdt(ptr);
    shmctl(shm_id, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

//--------------------------
void catch_usr1(int sig_num) {

}

//------------------------
sem_t *create_semaphore() {
    sem_t *mutex;
    mutex = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem open failed");
        exit(EXIT_FAILURE);
    }
    return mutex;
}

//---------------------
int *create_shm() {

    int *ptr_to_shm;
    key_t key;
    key = ftok(".", '8');

    if ((shm_id = shmget(key, ARR_SIZE * sizeof(int),
                         IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perror("Failed to get memory");
        exit(EXIT_FAILURE);
    }

    if ((ptr_to_shm = (int *) shmat(shm_id, NULL, 0)) < 0) {
        perror("Failed to attach memory");
        exit(EXIT_FAILURE);
    }
    return ptr_to_shm;
}

//---------------------------
void start() {
    ptr[OWNER_PLACE] = getpid();
}


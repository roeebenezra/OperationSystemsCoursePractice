//   ex8b.c - threads fill array with semaphore
//   Written by Roee Ben Ezra 206123994
//   yinontz & roibe

//___________________
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//____________________________
#define NUM_OF_THREADS  3
#define ARR_SIZE        50000
#define SEED            17

//______________________________
typedef enum {
    false, true
} bool;

//______________Global__________________
int arr[ARR_SIZE] = {0};
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_once_t threads_init = PTHREAD_ONCE_INIT;

//_______Structs_____
struct Data {
    int
            _bigger,
            _smaller;
};

//_______prototype__________
bool is_prime(int num);

void *prime_producer(void *);

int howManyTimesIsExist(int num);

int push_to_arr(int prime_num);

struct Data find_biggest_and_smallest();

void initial_data(struct Data *data, int initial);

int find_amount_different();

void printDone();

//__________________________
int main() {
    int i, rc,
            diff = 0;
    pthread_t threads[NUM_OF_THREADS];  // 3 fill array threads
    struct Data data;
    srand(SEED);    //srand(17)

    for (i = 0; i < NUM_OF_THREADS; i++) {
        rc = pthread_create(&threads[i], NULL, prime_producer, NULL);
        if (rc != 0) {
            fputs("pthread_create faild in main tread", stderr);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < NUM_OF_THREADS; i++)
        pthread_join(threads[i], NULL);  //main thread wait to filles to finish

    pthread_mutex_destroy(&mtx);

    data = find_biggest_and_smallest(); //collect data form arr
    diff = find_amount_different();
    //print data
    printf("There are %d different numbers\n"
           "The bigger is %d\nThe smaller is: %d\n",
           diff, data._bigger, data._smaller);

    return EXIT_SUCCESS;
}

//_____________________________
// 3 sub threads function
void *prime_producer(void *arg) {
    int num, answer, run = 1,
            new_values = 0, highest_times = 0;


    while (run) {
        num = rand();
        if (is_prime(num)) {
            pthread_mutex_lock(&mtx);  //critical part
            run = push_to_arr(num);
            pthread_mutex_unlock(&mtx); //finish critical part
            if (run) {
                answer = (howManyTimesIsExist(num) - 1);
                if (answer == 0)
                    new_values++;
                else if (answer > highest_times)
                    highest_times = answer;
            }
        }
    }
    printf("producer %lu sent %d new numbers\n"
           "the highest times is: %d\n",
           (unsigned long) pthread_self(), new_values, highest_times);

    pthread_once(&threads_init, printDone); //one thread prints "done"

    pthread_exit(NULL);
}

//________________________________
//critical part of the threads job
int push_to_arr(int prime_num) {
    int i;

    for (i = 0; i < ARR_SIZE; i++)
        if (arr[i] == 0) {
            arr[i] = prime_num;
            break;
        }
    if (i == ARR_SIZE)
        return 0;

    return 1;
}

//______________________________
int howManyTimesIsExist(int num) {
    int i;
    int count = 0;

    for (i = 0; i < ARR_SIZE; i++)
        if (arr[i] == num)
            count++;

    return count;
}

//________________________________________________
//parameter: array of numbers (primes from Global)
//return: Data
//goes over the array and checks who is the biggest and the smallest
struct Data find_biggest_and_smallest() {
    struct Data data;
    int i;
    initial_data(&data, arr[0]);

    for (i = 0; i < ARR_SIZE; i++) {
        if (arr[i] < data._smaller)
            data._smaller = arr[i];
        if (arr[i] > data._bigger)
            data._bigger = arr[i];
    }
    return data;
}

//---------------------
//assume that our array is not empty
void initial_data(struct Data *data, int initial) {
    data->_bigger = initial;
    data->_smaller = initial;
}

//___________________________
//parameter: array of numbers
//return: how many different numbers there are in the array
//I don't have any assumption of the numbers in the array,
//so the complexity is N^2.
int find_amount_different() {
    int i, j;
    int res = 1;

    for (i = 1; i < ARR_SIZE; i++) {
        for (j = 0; j < i; j++)
            if (arr[i] == arr[j])
                break;
        if (i == j)
            res++;
    }
    return res;
}

//_____________________
//prime number function
bool is_prime(int num) {
    int div;
    for (div = 2; (div * div) <= num; div++) {
        if (num % div == 0)
            return false;
    }
    return true;
}

//______________
void printDone() {
    printf("done\n");
}
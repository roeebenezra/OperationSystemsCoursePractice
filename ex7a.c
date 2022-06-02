/*
 ex7a - fill array
 Yinon Tzomi
 208489369
 yinontz

 This program fills array with prime numbers.

 The program creates global array then,
 the program creates NUM_OF_PRODUCERS threads,
 each producer create prime number, then pushes it to the array.
 when the array is full, the threads show their results and finish,
 the main process print its data.

 compile:
 gcc -Wall ex7a.c -lpthread -o ex7a
 run:
 ./ex7

 input: NON
 output:
 each thread prints how many new numbers it sends and how many times it
 creates same number
 and the main process prints how much different
 numbers there are in general and which is the bigger and the smaller.
 */

//------Includes------
#include  <pthread.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>

//-----Consts------
#define ARR_SIZE 50000
#define NUM_OF_PRODUCERS 3
#define OPEN 1
#define CLOSE 0
#define LOCK_PLACE 0
#define MANNAGING_PLACES 1
#define INITIAL_VALUE MANNAGING_PLACES
#define SEED 17

//-----Globals-----
int primes[ARR_SIZE] = {0};

//-------Structs----
struct Data {
    int
            _bigger,
            _smaller;
};

//------functions-------
//threads functions
void *do_producer();

int wait_and_push_back(int prime_num);

int is_prime(int num);

int howManyTimesIsExist(int num);

//main process function
struct Data find_biggest_and_smallest();

void initial_data(struct Data *data, int init);

int find_amount_different();

//-------------------------------------
int main() {
    pthread_t threads[NUM_OF_PRODUCERS];
    int rc;
    int i;

    struct Data data;
    int how_many_diff;

    srand(SEED);

    for (i = 0; i < NUM_OF_PRODUCERS; i++) {
        rc = pthread_create(&threads[i], NULL, do_producer, NULL);
        if (rc != 0) {
            fputs("pthread failed\n", stderr);
            exit(EXIT_FAILURE);
        }
    }
    primes[LOCK_PLACE] = OPEN;

    for (i = 0; i < NUM_OF_PRODUCERS; i++) {
        pthread_join(threads[i], NULL);
    }

    data = find_biggest_and_smallest();
    how_many_diff = find_amount_different();

    printf("There are %d different numbers\n"
           "The bigger is %d\nThe smaller is: %d\n",
           how_many_diff, data._bigger, data._smaller);


    return EXIT_SUCCESS;
}

//=====threads function=========
//runs until the array is full,
//creates prime numbers and writes them in the array
//when finishes, write the output as written
//note: prime number is number > 1 so I must "fix it" to
//prime number
void *do_producer() {

    int prime_num, answer
    , howMenyNew = 0, the_most_times = 0;
    int run = 1;
    int fix_to_prime = 2;
    while (run) {

        prime_num = (rand() + fix_to_prime);

        if (is_prime(prime_num)) {
            run = wait_and_push_back(prime_num);

            if (run) {
                answer = (howManyTimesIsExist(prime_num) - 1);
                if (answer == 0)
                    howMenyNew++;
                if (answer > the_most_times)
                    the_most_times = answer;
            }
        }
    }

    printf("producer %d sent %d new numbers\n"
           "the most times is: %d\n",
           (int) pthread_self(), howMenyNew, the_most_times);

    pthread_exit(NULL);
}
//--------------------------------------------------
/*receives array of numbers(from global) and number.
  returns if the task is done or not (because the array is full)
  wait to the lock to opens, closes it, writes the number
  in the array and then opens the lock again
 */
int wait_and_push_back(int prime_num) {
    int i;
    while (primes[LOCK_PLACE] != OPEN)
        sleep(1);

    primes[LOCK_PLACE] = CLOSE;

    for (i = MANNAGING_PLACES; i < ARR_SIZE; i++)
        if (primes[i] == 0) {
            primes[i] = prime_num;
            break;
        }

    primes[LOCK_PLACE] = OPEN;
    if (i == ARR_SIZE)
        return 0;

    return 1;
}

//-------------------------
//checks if number is prime
int is_prime(int num) {
    int i;

    for (i = 2; (i * i) <= num; i++)
        if ((num % i) == 0)
            return 0; //not prime
    return 1;
}

//----------------------------------------
//receives array and number. (from global)
//return how many times this number shown inside the array.
int howManyTimesIsExist(int num) {

    int i;
    int count = 0;

    for (i = MANNAGING_PLACES; i < ARR_SIZE; i++)
        if (primes[i] == num)
            count++;

    return count;
}

//=========main process function==================
//parameter: array of numbers (primes from Global)
//return: Data
//goes over the array and checks who is the biggest and the smallest
struct Data find_biggest_and_smallest() {
    struct Data data;
    int i;
    initial_data(&data, primes[INITIAL_VALUE]);

    for (i = MANNAGING_PLACES; i < ARR_SIZE; i++) {
        if (primes[i] < data._smaller)
            data._smaller = primes[i];
        if (primes[i] > data._bigger)
            data._bigger = primes[i];
    }
    return data;
}

//---------------------
//assume that our array is not empty
void initial_data(struct Data *data, int initial) {
    data->_bigger = initial;
    data->_smaller = initial;
}

//------------------
//parameter: array of numbers
//return: how many different numbers there are in the array
//I don't have any assumption of the numbers in the array,
//so the complexity is N^2.
//for batter solution (with assumption) you can see ex5a1
int find_amount_different() {

    int i, j;
    int res = 1;

    for (i = MANNAGING_PLACES + 1; i < ARR_SIZE; i++) {

        j = MANNAGING_PLACES;
        for (j = MANNAGING_PLACES; j < i; j++)
            if (primes[i] == primes[j])
                break;

        if (i == j)
            res++;
    }
    return res;
}

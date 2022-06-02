/*=========================== EX3 - pipe ======================================
program : ex3b

Name: Roee Ben Ezra
id: 206123994

    This program make 3 childes and the father get from childes random numbers
    through pipe.
    Each prime number stores in 1000 cells arr and in the end he
    prints how much different primes he got.

    Each son prints how many new primes he sent to parent via pipe

//===========================================================================*/

//--------include section-------------
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//-------------const section-----------
#define ARR_SIZE            1000     //prime_arr_size
#define NUM_OF_CHILDREN     3
#define DESCS_SIZE          2

typedef enum {
    false, true
} bool;

//----------global variables-----------
int NEW_PRIME = 0;  //global var new prime use by sons 
int ID;             //global son id var

//----------structs----------
struct prime_num {
    unsigned int prime;
    int id;
    int new_prime;
};

//--------------------------------prototypes-----------------------------------

bool is_prime(int num);

pid_t do_fork();

void create_pipe(int pipe_descs[]);

void do_son(int pipe_descs[], struct prime_num p, int special_pipe[]);

void do_parent(int pipe_descs[],
               int childes_pipes[NUM_OF_CHILDREN][DESCS_SIZE], pid_t *processes);

void catch_int(int sig_num);

void stop_childes_n_print(pid_t processes[], int primes_counter[]);

//======= main =================
int main() {
    int pipe_descs[DESCS_SIZE];                     //main pipe descs
    pid_t processes[NUM_OF_CHILDREN];               //processes pid arr
    int childs_pipes[NUM_OF_CHILDREN][DESCS_SIZE];  //special pipe descs arr
    struct prime_num p;
    int i;

    create_pipe(pipe_descs);    //create main pipe 
    for (i = 0; i < NUM_OF_CHILDREN; i++) {
        create_pipe(childs_pipes[i]);   //create special pipe for son i
        processes[i] = do_fork();
        if (processes[i] == 0) {
            p.id = i;   //save in p struct i index
            p.new_prime = 0;
            do_son(pipe_descs, p, childs_pipes[i]);
        }
        close(childs_pipes[i][0]);
    }

    do_parent(pipe_descs, childs_pipes, processes);

    return EXIT_SUCCESS;
}

//=============================================================================
//parent job function
void do_parent(int pipe_descs[], int childes_pipes[NUM_OF_CHILDREN][DESCS_SIZE],
               pid_t *processes) {
    int primes_counter[ARR_SIZE] = {0}; //count primes number
    struct prime_num p;                 //to store data gets from childs
    int i = 0, arr[ARR_SIZE];          //arr to store 1000 primes

    close(pipe_descs[1]);   //prent close write on main pipe

    while (i < ARR_SIZE) {
        read(pipe_descs[0], &p, sizeof(p));//gets prime_num struct from child
        arr[i] = p.prime;
        write(childes_pipes[p.id][1], &primes_counter[p.prime], sizeof(int));
        primes_counter[p.prime]++;
        i++;
    }
    stop_childes_n_print(processes, primes_counter);  //kill childes & print

    close(pipe_descs[0]);   //parent close read from main pipe
}

//-----------------------------------------------------------------
//sons job function
void do_son(int pipe_descs[], struct prime_num p, int special_pipe[]) {
    int num, shows;    //num random & number shows of prime

    signal(SIGINT, catch_int);
    srand(17);
    ID = p.id;  //current son id save globally

    close(pipe_descs[0]);   //son close main pipe for reading

    while (1) {
        num = (rand() % 999 + 2);
        if (is_prime(num)) {
            p.prime = num;
            write(pipe_descs[1], &p, sizeof(p));//sent to parent p struct
            //read num of shows until now from parent on special pipe
            read(special_pipe[0], &shows, sizeof(int));
            if (shows == 0)
                NEW_PRIME++;     //count in global for each son
        }
    }
}

//-------------------------------------------------------------------
//stop childes processes & print data
void stop_childes_n_print(pid_t processes[], int primes_counter[]) {
    int i, counter = 0;
    for (i = 0; i < NUM_OF_CHILDREN; i++)
        kill(processes[i], SIGINT);  //kill all 3 childes

    for (i = 0; i < ARR_SIZE; i++) {
        if (primes_counter[i] > 0)
            counter++;      //count how much different primes got
    }
    printf("parent different primes = %d\n", counter);
}

//--------------------------------------------------
//signal handler
void catch_int(int sig_num) {
    printf("son %d new primes = %d\n", ID, NEW_PRIME);
    exit(EXIT_SUCCESS);
}

//--------------------------------------
//prime number function
bool is_prime(int num) {
    int div;
    for (div = 2; div <= num / 2; div++) {
        if (num % div == 0)
            return false;
    }
    return true;
}

//-----------------------------------------
//fork function
pid_t do_fork() {
    pid_t status = fork();
    if (status < 0) {
        perror("cannot fork\n");
        exit(EXIT_FAILURE);
    }
    return status;
}

//-----------------------------------------
//create pipe function
void create_pipe(int pipe_descs[]) {
    if (pipe(pipe_descs) == -1) {
        perror("cannot open pipe\n");
        exit(EXIT_FAILURE);
    }
}

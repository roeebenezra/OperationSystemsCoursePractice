/*
 ex6a1- filler.

 This program creates sockets and fillers its array
 with numbers from the socket.
 The program waits to 3 producers then it sent them msg to start.

 When the array is full, the program tells to the producers to end and
 finishes.

 compile:
 gcc -Wall ex6a1.c -o ex6a1
 run:
 ./ex6a1 12345 (port)

 input - numbers from producers
 output - how much different numbers there are
         which The bigger and The smaller.

 */
//------includes--------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

//------------------consts---------------------
#define MAX_LISTEN 5
#define MY_PORT 1
#define NUM_OF_CREATORS 3
#define ARR_SIZE 1000
#define MAX 1000
#define MIN 0
#define NUM_OF_ARGUMENTS 2

//-----------------structs---------------------
struct data {
    int from,
            msg;
};
struct prime_data {
    int
            _bigger,
            _smaller,
            _how_many_diff;
};
struct DS {
    int _primes[ARR_SIZE];
    int _index;
};

//------globals------
struct addrinfo *add_info_res;

//-----signals---------
void catch_term(int sig_num);

//-------------prototypes-----------
int prepare_socket(const char *filler_port);

struct data read_msg(int *main_socket, fd_set *rfd);

void return_answer(const struct data *answer);

void wait_for_other(int *main_socket, fd_set *rfd, struct data flags[]);

void send_everybody_msg(struct data flags[], int msg);

void initial_prime_data(struct prime_data *data);

void initial_primes(struct DS *primes);

void do_creator(int *main_socket, fd_set *rfd);

int howManyTimesIsExist(const struct DS *primes,
                        struct prime_data *data,
                        int prime_num);

void bigger_or_smaller(struct prime_data *data, int prime_num);

void insert(struct DS *primes, int prime_num);

void wait_for_last(int *main_socket, fd_set *rfd);

//-------------------------------
int main(int argc, char *argv[]) {

    int main_socket;
    int START = 1;
    int FINISH = -1;
    struct data flags[NUM_OF_CREATORS];

    if (argc != NUM_OF_ARGUMENTS) {
        fprintf(stderr, "arguments problems");
        exit(EXIT_FAILURE);
    }

    memset(flags, -1, sizeof(flags));

    signal(SIGINT, catch_term);

    main_socket = prepare_socket(argv[MY_PORT]);
    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(main_socket, &rfd);

    wait_for_other(&main_socket, &rfd, flags);
    send_everybody_msg(flags, START);

    do_creator(&main_socket, &rfd);

    wait_for_last(&main_socket, &rfd);
    send_everybody_msg(flags, FINISH);

    freeaddrinfo(add_info_res);
    return EXIT_SUCCESS;
}

//--------------------------------------------
void do_creator(int *main_socket, fd_set *rfd) {

    struct data data, answer;
    struct prime_data prime_data;
    struct DS primes;

    initial_primes(&primes);
    initial_prime_data(&prime_data);

    while (primes._index < ARR_SIZE) {

        data = read_msg(main_socket, rfd);
        answer.from = data.from;
        answer.msg = howManyTimesIsExist(&primes, &prime_data, data.msg);
        bigger_or_smaller(&prime_data, data.msg);
        insert(&primes, data.msg);
        return_answer(&answer);
    }
    printf("There are %d different numbers\n"
           "The bigger is %d\nThe smaller is: %d\n",
           prime_data._how_many_diff,
           prime_data._bigger, prime_data._smaller);
}

//--------------------------------------------------
void send_everybody_msg(struct data flags[], int msg) {

    int i;
    for (i = 0; i < NUM_OF_CREATORS; i++) {
        flags[i].msg = msg;
        return_answer(&flags[i]);
    }
}

//-------------------------------------------------------------------
void wait_for_other(int *main_socket, fd_set *rfd, struct data flags[]) {

    struct data data;
    int i;
    int max_id = NUM_OF_CREATORS, min_id = 1;
    int start = 0;

    while (!start) {
        start = 1;
        data = read_msg(main_socket, rfd);

        if (data.msg <= max_id && data.msg >= min_id) {
            flags[data.msg - 1] = data;
        }

        for (i = 0; i < NUM_OF_CREATORS; i++)
            if (flags[i].msg != i + 1)
                start = 0;
    }

}

//-----------------------------------------------
void wait_for_last(int *main_socket, fd_set *rfd) {

    int i;
    for (i = 0; i < NUM_OF_CREATORS; i++)
        read_msg(main_socket, rfd);
}

//--------------------------------------------
void return_answer(const struct data *answer) {
    write(answer->from, &answer->msg, sizeof(answer->msg));
}

//-------------------------------------------------
struct data read_msg(int *main_socket, fd_set *rfd) {

    struct data data;
    int rc;
    int serving_socket;
    fd_set c_rfd;
    c_rfd = *rfd;

    memset(&data, -1, sizeof(data));

    rc = select(getdtablesize(), &c_rfd, NULL, NULL, NULL);

    if (FD_ISSET(*main_socket, &c_rfd)) {
        serving_socket = accept(*main_socket, NULL, NULL);
        if (serving_socket >= 0) {
            FD_SET(serving_socket, rfd);
        }
    }

    for (data.from = (*main_socket + 1); data.from < getdtablesize();
         data.from++) {
        if (FD_ISSET(data.from, &c_rfd)) {

            rc = read(data.from, &data.msg, sizeof(data.msg));

            if (rc == 0) {
                close(data.from);
                FD_CLR(data.from, rfd);
            } else if (rc > 0) {
                return data;
            } else {
                perror("read faild");
                exit(EXIT_FAILURE);
            }
        }
    }
    return data;
}

//-----------------------------------------
int prepare_socket(const char *filler_port) {

    int main_socket;
    int rc;
    struct addrinfo con_kind;
    memset(&con_kind, 0, sizeof(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;
    con_kind.ai_flags = AI_PASSIVE;

    if ((rc = getaddrinfo(NULL,
                          filler_port,
                          &con_kind,
                          &add_info_res)) != 0) {
        fprintf(stderr, "(getaddrinfo() failed \n");
        exit(EXIT_FAILURE);
    }

    main_socket = socket(add_info_res->ai_family,
                         add_info_res->ai_socktype,
                         add_info_res->ai_protocol);
    if (main_socket < 0) {
        perror("socket:allocation failed");
        exit(EXIT_FAILURE);
    }

    rc = bind(main_socket,
              add_info_res->ai_addr,
              add_info_res->ai_addrlen);
    if (rc) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    rc = listen(main_socket, MAX_LISTEN);

    if (rc) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    return main_socket;
}

//---------------------------------
//catch the signal handler and release the queue
void catch_term(int sig_num) {
    freeaddrinfo(add_info_res);

    exit(EXIT_SUCCESS);
}

//-----------------------------------
void initial_primes(struct DS *primes) {
    primes->_index = 0;
}

//-----------------------------------------------
//assume that our primes will be between MAX to MIN
void initial_prime_data(struct prime_data *data) {
    data->_bigger = MIN;
    data->_smaller = MAX;
    data->_how_many_diff = 0;
}

//-------------------------
//receives array and number.
//return how many times this number shown inside the array.
int howManyTimesIsExist(const struct DS *primes,
                        struct prime_data *data,
                        int prime_num) {

    int i;
    int count = 0;

    for (i = 0; i < primes->_index; i++) {
        if (primes->_primes[i] == prime_num)
            count++;
    }

    if (count == 0)
        data->_how_many_diff++;

    return count;
}

//-------------------------
//check if this number is bigger or smaller than others in our array
void bigger_or_smaller(struct prime_data *data, int prime_num) {

    if (prime_num < data->_smaller)
        data->_smaller = prime_num;
    if (prime_num > data->_bigger)
        data->_bigger = prime_num;
}

//---------------------
//insert number do my DS
void insert(struct DS *primes, int prime_num) {
    primes->_primes[primes->_index] = prime_num;
    primes->_index++;
}

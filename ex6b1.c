/*
 Yinon Tzomi 208489369
 yinontz

 ex6b1 prime

 This program checks if number is prime

 The program creates socket and receives messages to it.
 Then the program does its checking and returns the answer
 to the socket.

 When the program gets ^c it close its socket and finishes.

 compile:
 gcc -Wall ex6b1 -o prime
 run:
 ./prime and port ex: ./prime 12345
 input:
 from client - some number
 output to client: 0 if not prime else 1

 */
//--------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

//-------consts----------
#define MAX_LISTEN 5
#define MAX_ADDR_LEN 100
#define MY_PORT 1
#define NUM_OF_ARGUMENTS 2

//------structs------
struct data {
    int from,
            msg;
};

//------globals------
struct addrinfo *add_info_res;

//------------prototypes---------------------
int prepare_socket(const char *filler_port);

struct data read_msg(int *main_socket, fd_set *rfd);

void return_answer(const struct data *answer);

void do_prime(int *main_socket, fd_set *rfd);

int is_prime(int num);


//-----signals---------
void catch_term(int sig_num);


//-------main---------
int main(int argc, char *argv[]) {

    int main_socket;

    if (argc != NUM_OF_ARGUMENTS) {
        fprintf(stderr, "arguments problems");
        exit(EXIT_FAILURE);
    }


    signal(SIGINT, catch_term);

    main_socket = prepare_socket(argv[MY_PORT]);
    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(main_socket, &rfd);

    do_prime(&main_socket, &rfd);

    return EXIT_SUCCESS;
}

//--------------------
//Does the main work- read numbers and returns the answers
void do_prime(int *main_socket, fd_set *rfd) {

    struct data data, answer;
    data = read_msg(main_socket, rfd);

    while (1) {
        data = read_msg(main_socket, rfd);
        answer.from = data.from;
        answer.msg = is_prime(data.msg);
        return_answer(&answer);
    }
}

//-----------------
// checks if the number is prime
int is_prime(int num) {
    int i;

    for (i = 2; (i * i) <= num; i++)
        if ((num % i) == 0)
            return 0; //not prime
    return 1;
}

//------------------------
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
        fprintf(stderr, "(getaddinfo() failed \n");
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

//------------------------
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
                perror("read failed");
                exit(EXIT_FAILURE);
            }
        }
    }
    return data;
}

//--------------------------------------------
void return_answer(const struct data *answer) {
    write(answer->from, &answer->msg, sizeof(answer->msg));
}

//--------------------------
//catch the signal handler and release the queue
void catch_term(int sig_num) {
    freeaddrinfo(add_info_res);

    exit(EXIT_SUCCESS);
}

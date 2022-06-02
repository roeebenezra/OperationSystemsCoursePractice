/*
 Yinon Tzomi 208489369
 yinontz

 ex6b2 palindrome

 This program checks if string is palindrome

 The program creates socket and receives messages to it.
 Then the program does its checking and returns the answer
 to the socket.

 When the program gets ^c it closes the socket and finishes.

 compile:
 gcc -Wall ex6b2 -o palindrome
 run:
 ./palindrome and port ex: ./palindrome 12346
 input:
 from client - some string
 output to client: 0 if not palindrome else 1

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

//-------consts----------
#define MAX_LISTEN 5
#define MAX_STR_LEN 100
#define MY_PORT 1
#define NUM_OF_ARGUMENTS 2

//------structs------
struct data {
    int from;
    char msg[MAX_STR_LEN + 1];
};
struct ans {
    int from;
    int msg;
};

//------globals------
struct addrinfo *add_info_res;

//------functions------
int prepare_socket(const char *filler_port);

struct data read_msg(int *main_socket, fd_set *rfd);

void return_answer(const struct ans *answer);

void do_palindrome(int *main_socket, fd_set *rfd);

int is_palindrome(char str[]);

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


    do_palindrome(&main_socket, &rfd);

    return EXIT_SUCCESS;
}

//-----------------------
//Does the main work- reads strings and return answers
void do_palindrome(int *main_socket, fd_set *rfd) {

    struct data data;
    struct ans answer;

    data = read_msg(main_socket, rfd);

    while (1) {
        data = read_msg(main_socket, rfd);

        answer.from = data.from;
        answer.msg = is_palindrome(data.msg);
        return_answer(&answer);
    }
}

//-----------------
// checks if the string is palindrome
int is_palindrome(char str[]) {

    int r = (strlen(str) - 1), l = 0;

    if (r > 0)
        r--;
    while (r > l) {
        if (str[l] != str[r])
            return 0; //not palindrome
        l++;
        r--;
    }
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

            rc = read(data.from, &data.msg, MAX_STR_LEN);
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


//------------------------------------------
void return_answer(const struct ans *answer) {
    write(answer->from, &answer->msg, sizeof(answer->msg));
}

//--------------------------
//catch the signal handler and release the queue
void catch_term(int sig_num) {
    freeaddrinfo(add_info_res);

    exit(EXIT_SUCCESS);
}

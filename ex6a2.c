/*
 	 ex6a2 producers

 	 This program create prime numbers and send it to the filler.
 	 The program connect to the socket, then create the numbers,
 	 send them and wait to answer.

 	 input- NON
 	 output:
 	 how many new numbers the program created and sent

 	 compile:
 	 gcc -Wall ex6a2.c -o ex6a2
 	 run:
 	 first run filler then:
 	 ./ex6a2 1(ID),localhost (filler IP), 12345 (filler port)
 */

//----------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

//--------------------
#define NUM_CHILD 1
#define FILLER_IP 2
#define FILLER_PORT 3
#define MODULO 999
#define INSERT_TO_OUR_SCALE 2
#define NUM_OF_ARGUMENTS 4

//---------------------prototypes-------------------
void do_creator(const int *my_socket, const int num_child);

int is_prime(int num);

int prepare_socket(const char *filler_ip, const char *filler_port);

//---------------------
struct addrinfo *add_info_res;

//---------------------
int main(int argc, char *argv[]) {

    int my_socket;
    int ans;
    int num_child = atoi(argv[NUM_CHILD]);
    char *filler_ip = argv[FILLER_IP];
    char *filler_port = argv[FILLER_PORT];

    if (argc != NUM_OF_ARGUMENTS) {
        fprintf(stderr, "arguments problems");
        exit(EXIT_FAILURE);
    }

    srand(num_child);

    my_socket = prepare_socket(filler_ip, filler_port);

    write(my_socket, &num_child, sizeof(num_child));
    read(my_socket, &ans, sizeof(ans));

    do_creator(&my_socket, num_child);

    close(my_socket);
    freeaddrinfo(add_info_res);

    return EXIT_SUCCESS;
}

//-------------------------
//create prime number, send to the creator and check its answer
void do_creator(const int *my_socket, const int num_child) {

    int prime_num, answer,
            howManyNews = 0, the_most_times = 0;
    int FINISH = -1;
    int NEW_NUMBER = 0;

    while (1) {
        while (1) {
            prime_num = (rand() % MODULO) + INSERT_TO_OUR_SCALE;

            if (is_prime(prime_num)) {
                write(*my_socket, &prime_num, sizeof(prime_num));
                prime_num = 0;
                break;
            }
        }

        read(*my_socket, &answer, sizeof(answer));

        if (answer == NEW_NUMBER)
            howManyNews++;
        if (answer > the_most_times)
            the_most_times = answer;
        if (answer == FINISH)
            break;
    }

    printf("child %d sent %d new numbers\n"
           "the most times is: %d\n",
           num_child, howManyNews, the_most_times);
}

//------------------------
//checks if number is prime
int is_prime(int num) {
    int i;

    for (i = 2; i < num; i++)
        if ((num % i) == 0)
            return 0; //not prime
    return 1;
}

//----------------------------------------------------------------
int prepare_socket(const char *filler_ip, const char *filler_port) {

    struct addrinfo con_kind, *p;
    int my_socket;
    int rc;

    memset(&con_kind, 0, sizeof(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;

    if ((rc = getaddrinfo(filler_ip, filler_port,
                          &con_kind,
                          &add_info_res)) != 0) {
        fprintf(stderr, "(getaddrinfo() failed \n");
        exit(EXIT_FAILURE);
    }

    for (p = add_info_res; p != NULL; p = p->ai_next) {

        my_socket = socket(p->ai_family,
                           p->ai_socktype,
                           p->ai_protocol);
        if (my_socket < 0) {
            perror("socket : allocation failed");
            exit(EXIT_FAILURE);
        }

        rc = connect(my_socket,
                     p->ai_addr,
                     p->ai_addrlen);
        if (rc < 0) {
            close(my_socket);
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");
        exit(EXIT_FAILURE);
    }
    return my_socket;
}

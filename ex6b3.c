/*
	ex6b3 client

	This program is the client.
	The program connects to the palindrome and prime sockets,
	then read inputs from the user and prints the answers to the screen.

	compile:
	gcc -Wall ex6b3.c -o client
	run:
	before running you must run the palindrome and prime servers
	./client prime ip and prime port,
	palindrome ip and palindrome port.
	ex:
	./client localhost 12345 localhost 12346

	input:
	p for check if number is prime and number
	q for check if string is palindrome and string
	ex:
	p 1
	q str
	output:
	1 if true, 0 if not

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

//--------------------
#define PRIME_IP 1
#define PRIME_PORT 2
#define PALINDROM_IP 3
#define PALINDROM_PORT 4
#define NUM_OF_ARGUMENTS 5
#define MAX_STR_LEN 100

//---------------------
void do_client(const int *prime_socket, const int *palindrome_socket);

int prepare_socket(const char *filler_ip, const char *filler_port,
                   struct addrinfo *add_info_res);

//---------------------
int main(int argc, char *argv[]) {

    int prime_socket;
    int palindrome_socket;

    struct addrinfo *prime_add_info_res = NULL;
    struct addrinfo *palindrome_add_info_res = NULL;

    if (argc != NUM_OF_ARGUMENTS) {
        fprintf(stderr, "arguments problems");
        exit(EXIT_FAILURE);
    }

    prime_socket = prepare_socket(argv[PRIME_IP],
                                  argv[PRIME_PORT], prime_add_info_res);

    palindrome_socket = prepare_socket(argv[PALINDROM_IP],
                                      argv[PALINDROM_PORT], palindrome_add_info_res);

    do_client(&prime_socket, &palindrome_socket);

    close(prime_socket);
    freeaddrinfo(prime_add_info_res);
    close(palindrome_socket);
    freeaddrinfo(palindrome_add_info_res);

    return EXIT_SUCCESS;
}

//--------------------------------------------------------------
//create prime number, send to the creator and check its answer
void do_client(const int *prime_socket, const int *palindrome_socket) {

    char c;
    char buff[MAX_STR_LEN + 1];
    int num, answer;


    while (1) {

        c = getchar();
        if (c == 'p') {
            scanf("%d", &num);
            getchar();

            write(*prime_socket, &num, sizeof(num));
            read(*prime_socket, &answer, sizeof(answer));
            printf("answer: %d\n", answer);
        } else if ((c) == 'q') {
            getchar();
            fgets(buff, MAX_STR_LEN, stdin);

            write(*palindrom_socket, buff, MAX_STR_LEN);
            read(*palindrom_socket, &answer, sizeof(answer));
            printf("answer: %d\n", answer);
        } else if ((c) == 'e')
            break;
        else
            puts("wrong command");
        fflush(stdin);
    }
}

//------------------------------------------------------------
int prepare_socket(const char *filler_ip, const char *filler_port,
                   struct addrinfo *add_info_res) {

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
            perror("socket:allocation failed");
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

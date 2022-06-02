/*
 * ex4a.c
 *
 *  gets file name with rows of numbers
 *  maps this file to memory and through the memory
 *  write to file the averages of each row.
 *
 *  Compile: gcc ex4a.c -Wall -o ex4a
 *  Run: ./ex4a FILE_NAME
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>


#define ARGC    2

//_________prototypes_________
void average_rows(char *, int);

void handle_end_row(char *, char *, int);

char *convert_to_str(int);

//_________main________________
int main(int argc, char **argv) {
    int fd;
    unsigned file_size;
    char *data;

    if (argc != ARGC) {
        fprintf(stderr, "usage: %s <file name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if ((fd = open(argv[1], O_RDWR)) == -1) {
        fprintf(stderr, "cannot open %s for r+w\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    file_size = lseek(fd, 0, SEEK_END);

    if ((data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == (caddr_t) - 1) {
        fprintf(stderr, "mmap() for %s failed\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    close(fd);

    average_rows(data, file_size);

    return EXIT_SUCCESS;
}

//_________________________________________
void average_rows(char *data, int file_size) {
    char number[2], *avg;
    int row_sum = 0, nums_in_row = 0, i;

    for (i = 0; i < file_size; i++) {
        if (!isspace(data[i]) && data[i] != '\n') {
            number[0] = data[i];
            number[1] = data[i + 1];
            int num = atoi(number);
            row_sum += num;
            nums_in_row++;
            i++;
        }
        if (data[i] == '\n' || i == file_size - 1) {
            avg = convert_to_str(row_sum / (nums_in_row - 1));
            handle_end_row(data, avg, i);
            row_sum = 0;
            nums_in_row = 0;
        }
    }
}

//_________________________________________________
void handle_end_row(char *data, char *avg, int place) {
    if (data[place] == '\n') {
        data[place - 2] = avg[0];
        data[place - 1] = avg[1];
    } else {
        data[place - 1] = avg[0];
        data[place] = avg[1];
    }
}

//_________________________
char *convert_to_str(int avg) {
    char *res = malloc(sizeof(char) * 2);
    int a = avg % 10;
    res[1] = a + '0';
    int b = (avg - a) / 10;
    res[0] = b + '0';

    return res;
}
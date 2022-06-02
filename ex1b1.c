#include <stdio.h>
#include <stdlib.h>

//========== history ========================

int main(int argc,char* argv[])
{
    if(argc > 2){
        perror("history-files number");
        exit(1);
    }
    FILE* fptr = fopen(argv[1],"r");
    if(!fptr){
        perror("history-open file");
        exit(1);
    }
    char *line ;    //for read line from file
    size_t size = 1;
    line = (char *) malloc(size);
    while(!feof(fptr)){
        size = 1;
        int read_line = getline(&line, &size, fptr);
        if(read_line == -1){
            perror("history-read line");
            exit(1);
        }
        puts(line);
    }
    free(line);
    fclose(fptr);

    return EXIT_SUCCESS;
}
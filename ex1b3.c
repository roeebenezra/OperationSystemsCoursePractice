#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//================ ps ========================

int main(int argc,char* argv[])
{
    if(argc > 2)
    {
        perror("too many arguments");
        exit(1);
    }
    if(argc == 1){
        printf("parent process id:%d\n",getppid());
        exit(0);
    }
    if(!strcmp(argv[1],"-a"))
    {
        printf("my process id:%d\n",getpid());
        printf("parent process id:%d\n",getppid());
    }

    return EXIT_SUCCESS;
}
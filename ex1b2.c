#include <stdio.h>
#include <stdlib.h>

//========== echo ========================

int main(int argc,char* argv[])
{
    int i = 1;
    while (i < argc){
       puts(argv[i]);
       i++;
    }
    return EXIT_SUCCESS;
}
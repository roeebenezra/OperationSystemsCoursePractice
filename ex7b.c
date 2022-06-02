// ex7b.c
// prime & palindrome servers use by front-end thread
/*

 This program creates 3 threads who work with each other,
        each thread run in infinite loop and wakes up with signal get.

    thread 1 - front_end server: read from user the input and
                                 store in global variables the data.

    server 2 - prime server: check the data that the front_end
                              read from user and stores in global array.
    
    server 3 - palindrome server: check the palindrome that the front_end
                                  read from user and stores in global array.                          

 compile:
 gcc ./ex7b.c -Wall -o ex7b -lpthread 
 run: ./ex7b

 input:  p - to check if number is prime and then the number.
         q - to check if numbers series is palindrome and then the series.
 
 output: 1 - The number/series is prime/palindrome
         0 - Not prime/palindrome
 */
//-------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

//----------Const----------------
#define ZERO                0
#define ONE                 1
#define TWO                 2
#define FAIL_SIGN           -1
#define PALI_SIZE           20

//threads const names
#define FRONT               0
#define PRIME               1
#define PALINDROME          2
#define CREATOR             3

//threads servers consts
#define NUM                 0
#define PRIME_RESULT        1
#define TEST_RESULT         19
#define PRIME_CHECK         'p'
#define PALINDROME_CHECK    'q'
#define NUM_OF_THREADS      3    

//-----------Enum------------------------
typedef enum {false, true} bool;

//----------Prototypes--------------------
//void* servers funcs
void* do_threads_creator();
void* is_prime_server();
void* is_palindrome_server();
void* front_end_server();
//other help servers funcs
bool is_palindrome(char *arr ,int size);
bool is_prime(int num);
void create_thread(int thread_cell, void* func);
void sent_signal(pthread_t thread,int sig);
//handlers funcs
void handler_sigint(int signal_num);
void handler_sigusr1(int signal_num){}
void handler_sigusr2(int signal_num){}

//-------------Global---------------------------
int sigint = ZERO ;                 //when SIGINT pressed changes to 1 & exit;                         
pthread_t threads[NUM_OF_THREADS];  //pthreads_t id array 
int prime[TWO];                    //spacial array for prime server
char palindrome[PALI_SIZE];        //spacial array for pilandrome server

//-------------Main---------------------------------
int main()
{   //signal handlers define
    signal(SIGINT,handler_sigint);
    signal(SIGUSR1,handler_sigusr1);
    signal(SIGUSR2,handler_sigusr2);

    //create creator thread 
    create_thread(CREATOR,do_threads_creator);

    while (!sigint){}   //until get SIGINT

    return EXIT_SUCCESS;
}

//-----------------------------------------------------
void* do_threads_creator()
{
    //create 3 threads servers
    create_thread(PRIME,is_prime_server);
    create_thread(FRONT,front_end_server);
    create_thread(PALINDROME,is_palindrome_server);

    pthread_exit(NULL);
}

//-----------------------------------------------------
//prime server
void* is_prime_server()
{
    while(true)
    {
        pause();
        prime[PRIME_RESULT] = is_prime(prime[NUM]);
        pthread_kill(threads[FRONT],SIGUSR1);
    }

    pthread_exit(NULL);
}

//-------------------------------------------------------
//palindrome server
void* is_palindrome_server()
{
    while(true)
    {    
        pause();
        palindrome[TEST_RESULT] = is_palindrome(palindrome,strlen(palindrome));
        pthread_kill(threads[FRONT],SIGUSR2);
    }

    pthread_exit(NULL);
}

//-----------------------------------------------------
//front-end server
void* front_end_server()
{
    int num;

    while(true)
    {
        char c = getchar();
        switch(c)
        {
        case PRIME_CHECK:
            scanf("%d",&num);
            prime[NUM] = num;
            sent_signal(threads[PRIME],SIGUSR1);
            pause();
            printf("%d\n",prime[PRIME_RESULT]);
            break;
        
        case PALINDROME_CHECK:
            scanf("%s",palindrome);
            sent_signal(threads[PALINDROME],SIGUSR2);
            pause();
            printf("%d\n",palindrome[TEST_RESULT]);
            memset(palindrome,'\0',sizeof(palindrome));  //clear piladrome array
            break;
        
        default:
            break;
        }       
    }

    pthread_exit(NULL);
}

//-------------------------------------------------------
//create 1 thread function
void create_thread(int thread_cell, void* func)
{
    int status;
    status = pthread_create(&threads[thread_cell],NULL,func,NULL);
    if(status != ZERO)
    {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }
}

//------------------------------------------------------
//is prime number check function
bool is_prime(int num)
{
    if(num < TWO)
        return false;

    int div;

    for(div = TWO ; div <= num / TWO ; div++)
    {
		if (num % div == ZERO)
			return false;
	}

    return true;
}

//---------------------------------------------------------------------
//is palindrome series check
bool is_palindrome(char *arr ,int size)
{
    int i, j, flag = ZERO ;

    for(i = ZERO, j = size - ONE ; j >= size / TWO ; i++, j--){
        if(arr[i] != arr[j]) {
            flag = ONE;
            break;
        }
    }
    if (flag == ONE)
        return false;
    else
        return true;
}

//---------------------------------------------------
//signal sent function
void sent_signal(pthread_t thread,int sig)
{
    int status;

    status = pthread_kill(thread,sig);
        if(status != ZERO)
        {
            perror("palindrome signal failed");
            exit(EXIT_FAILURE);
        }

}

//---------------------------------------------
//SIGINT handler
void handler_sigint(int signal_num)
{
    sigint = 1;
}

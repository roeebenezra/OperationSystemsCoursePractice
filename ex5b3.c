// *** front end server ***
// 
//-------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//----------------------------------------
#define FAIL_SIGN             -1
#define SERVER_PID             0
#define MY_PID                 1

#define ZERO 0
#define ONE 1

#define PRIME_PLACE            2
#define PRIME_TEST_RESULT      3

#define PIL_SERIES_START       2
#define PIL_SERIES_END         21
#define PIL_TEST_RESULT        21
#define ACCESS_KIND            0600

//-----------------------------------------
int INT_SIGNAL = 0;

//-----------------------------------------
key_t key_tok(char);
int shm_id_get(key_t);
int *shm_ptr_get(int);

void memory_set(int *q_shm_ptr,int *p_shm_ptr);
void init_memory(int* shm_ptr);
void sigusr1_handler(int sig_num){}
void sigusr2_handler(int sig_num){}
void sigint_handler(int sig_num);

//-----------------------------------------
int main()
{
    signal(SIGUSR1,sigusr1_handler);
    signal(SIGUSR2,sigusr2_handler);
    signal(SIGINT,sigint_handler);

    key_t p_key = key_tok('p'),
          q_key = key_tok('q');

    int p_shmid = shm_id_get(p_key),
        q_shmid = shm_id_get(q_key);
    
    int *p_shm_ptr = shm_ptr_get(p_shmid),
        *q_shm_ptr = shm_ptr_get(q_shmid);

    memory_set(q_shm_ptr,p_shm_ptr);

    return EXIT_SUCCESS;
}
//--------------------------------------------------------
void memory_set(int *q_shm_ptr,int *p_shm_ptr)
{
    int num;

    p_shm_ptr[MY_PID] = getpid();
    q_shm_ptr[MY_PID] = getpid();

    while(!INT_SIGNAL)
    {
        char c = getchar();
        switch (c)
        {
        case 'p':
            scanf("%d",&num);
            p_shm_ptr[PRIME_PLACE] = num;
            kill(p_shm_ptr[SERVER_PID],SIGUSR1);     //send signal to prime server
            pause();
            printf("%d\n",p_shm_ptr[PRIME_TEST_RESULT]); //print result
            break;
        case 'q':
            num = ONE;
            for(int i = PIL_SERIES_START ; i < PIL_SERIES_END && num != ZERO ; i++){
                scanf("%d",&num);
                q_shm_ptr[i] = num;
            }
            kill(q_shm_ptr[SERVER_PID],SIGUSR2);//send signal to pilandrome server
            pause();
            printf("%d\n",q_shm_ptr[PIL_TEST_RESULT]);  //print result   
            init_memory(q_shm_ptr); //set palindrome memory to 0
            break;
        default:
            break;
        }
    }

    shmdt(p_shm_ptr);        
    shmdt(q_shm_ptr);        
}
//-------------------------------------------------------------
void init_memory(int* shm_ptr)
{
    for(int i = PIL_SERIES_START ; i < PIL_SERIES_END ; i++)
        shm_ptr[i] = ZERO ;
}
//--------------------------------------------------------
void sigint_handler(int sig_num)
{
    INT_SIGNAL = ONE;
}
//-------------------------------------------------------
key_t key_tok(char server)
{
    key_t key = ftok(".",server);
    if(key == FAIL_SIGN){
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    return key;
}
//-------------------------------------------------------
int shm_id_get(key_t key)
{

    int shm_id = shmget(key,ZERO,ACCESS_KIND);
    if(shm_id == FAIL_SIGN){
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    return shm_id;
}
//------------------------------------------------------
int *shm_ptr_get(int shm_id)
{
    int *shm_ptr = (int*)shmat(shm_id,NULL,ZERO);
    if(shm_ptr == (int*)FAIL_SIGN){
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    return shm_ptr;
}
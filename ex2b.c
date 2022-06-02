/*================= EX2b ============================
name: Roee Ben-Ezra
id: 206123994

====================================================*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

//=========== globals section =========

pid_t STATUS ;  //global fork status var
int SON_COUNTER_SIGUSR1 = 0, SON_COUNTER_SIGUSR2 = 0;
int PARENT_COUNTER_SIGUSR1 = 0, PARENT_COUNTER_SIGUSR2 = 0;

//======== prototype section ====
void catch_sigusr(int sig_num);
void catch_sigtrm(int sig_num);
pid_t do_fork();
void do_parent();
void do_son();
void parent_sigusr_handler(int sig_num);
void son_sigusr_handler(int sig_num);

//============ main =================
int main()
{
    signal(SIGUSR1,catch_sigusr);
    signal(SIGUSR2,catch_sigusr);
    signal(SIGTERM,catch_sigtrm);    

    STATUS = do_fork();
    if(STATUS == 0)
        do_son();       
    else
        do_parent();
    //race modes - the parent & the son do action in the same time if the system
    // choose one over the other, this process could send signal to process who
    // don't even started. one more thing is that parent process random num will
    // be 0 and that end both process.

    exit(EXIT_SUCCESS);
}   
//=============================================================================
//parent action function
void do_parent()
{
    int count_sigusr1 = 0,  //counters
        count_sigusr2 = 0;
    int num;
    srand(17);
    
    while(1)
    {
        sleep(rand() % 3);
        num = rand()% 3;
        
        switch (num)    //actions accord to num value
        {
        case 0:
            exit(EXIT_SUCCESS);
            break;
        case 1:
            kill(STATUS,SIGUSR1);
            count_sigusr1++;
            break;
        case 2:
            kill(STATUS,SIGUSR2);
            count_sigusr2++;
            break;        
        }
        //if some signal user = 7 end process
        if(count_sigusr1 == 7 || count_sigusr2 == 7){
            printf("you probably ended\n");
            exit(EXIT_SUCCESS);
        }
    
    }
}
//-----------------------------------------------------------------------------
//son action function
void do_son()
{
    int count_sigusr1 = 0,
        count_sigusr2 = 0;
    int num;
    srand(18);
    
    while(1)
    {
        sleep(rand() % 3);
        num = rand() % 3;    
       
        switch (num)     //actions accord to num value
        {
        case 0:
            exit(EXIT_SUCCESS);
            break;
        case 1:
            kill(getppid(),SIGUSR1);
            count_sigusr1++;
            break;
        case 2:
            kill(getppid(),SIGUSR2);
            count_sigusr2++;
            break;        
        }
        //if some signal user = 7 end process
        if(count_sigusr1 == 7 || count_sigusr2 == 7){
            printf("you probably ended\n");
            exit(EXIT_SUCCESS);
        }
    
    }
}
//-----------------------------------------------------------------------------
//signal handler for SIGUSR1/SIGUSR2
void catch_sigusr(int sig_num)
{
    if(STATUS == 0)
        son_sigusr_handler(sig_num);
    else 
        parent_sigusr_handler(sig_num); 
}

//-----------------------------------------------------------------------------
//parent SIGUSR handler
void parent_sigusr_handler(int sig_num)
{
    if(sig_num == SIGUSR1)
        PARENT_COUNTER_SIGUSR1++;
    if(sig_num == SIGUSR2)
        PARENT_COUNTER_SIGUSR2++;
    if(PARENT_COUNTER_SIGUSR1 == 5 || PARENT_COUNTER_SIGUSR2 == 5){
        printf("process %d surrender\n",getpid());
        kill(STATUS,SIGTERM);
        exit(EXIT_SUCCESS);
    }
}

//-----------------------------------------------------------------------------
//son SIGUSR handler
void son_sigusr_handler(int sig_num)
{
    if(sig_num == SIGUSR1)
            SON_COUNTER_SIGUSR1++;
    if(sig_num == SIGUSR2)
        SON_COUNTER_SIGUSR2++;
    if(SON_COUNTER_SIGUSR1 == 5 || SON_COUNTER_SIGUSR2 == 5){
        printf("process %d surrender\n",getpid());
        kill(getppid(),SIGTERM);
        exit(EXIT_SUCCESS);
    }
}
//------------------------------------------------------------------------------
//signal hadler for SIGTERM
void catch_sigtrm(int sig_num)
{
    printf("process %d win",getpid());
    exit(EXIT_SUCCESS);
}
//-----------------------------------------------------------------------------
//fork function
pid_t do_fork()
{
    pid_t status = fork();
        if(status < 0){
            perror("cannot fork\n");
            exit(EXIT_FAILURE);
        }
    return status;
}
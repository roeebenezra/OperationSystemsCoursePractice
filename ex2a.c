/*================= EX2a ============================
name: Roee Ben-Ezra
id: 206123994

====================================================*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

typedef enum {false, true} bool;
#define PRIMES_NUM 100000

//=================================================
bool is_prime(int num);
void dad_mission(pid_t son_pid, int sec);
void child_mission();
pid_t do_fork();
//=============================================================================
int main()
{
    pid_t status = do_fork();

    if(status == 0)
    {
        child_mission();  //race mode - if the system choose first to do child 
                          // process and then continue with parent - output:1 
    }                     // if the opposite way - output:0 
    dad_mission(status,1); //if child finish in 1 sec
    dad_mission(status,2); //if child finish in 2 sec
   
    if(waitpid(status,NULL,WNOHANG) < 0)//if child not finish in 2 sec
        kill(status,SIGKILL);
    
    printf("%d\n",0);

    exit(EXIT_SUCCESS);
}   
//-------------------------------------------------------------------
//parent mission to check if child finish in 2 sec
void dad_mission(pid_t son_pid, int sec)
{
    sleep(1);
    if(waitpid(son_pid,EXIT_SUCCESS,WNOHANG) > 0)
    {    //if child finish in 1 sec
        printf("%d\n",sec);
        exit(EXIT_SUCCESS);
    }
}
//-----------------------------------------------------------------------------
//child mission to lottery 100k prime numbers
void child_mission()
{
    srand(17);
    int num, primes_counter = 0;

    while(primes_counter < PRIMES_NUM+1)
    {
        num = rand();
        if(is_prime(num))
            primes_counter++;
    }

    exit(EXIT_SUCCESS);
}

//---------------------------------------------
//prime number function
bool is_prime(int num)
{
    int div;
    for(div = 2 ; num <= num/2 ; div++){
		if (num % div == 0)
			return false;
	}
    return true;
}

//-----------------------------------------------
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
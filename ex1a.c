/*=============================ex1a===============================
 * Name: Roee Ben Ezra
 * Id:   206123994

 * summery:
    program A : This program makes 2 Sub-processes who sorts array in
    two different algorithms:  1.bubble sort
                               2.quick sort
                 
    Every Sub-process prints to the output file the time that take him to
    do his mission.
    The parent use all this data and calculate the following members:
        1. the average running of each sort.
        2. the fastest time of quick/bubble sort.
        3. the slowest time of quick/bubble sort.

    Then the father prints the avg time all the missions took.
    
    avg: 300.9525656
==============================================================*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>

//--------------------------------------------
#define SIZE 50000
#define MISSION 50

//------------------------------prototypes-------------------------------------
void bubble_sort(int arr[]);

void quick_sort(int arr[], int from, int to);

int partition(int arr[], int from, int to);

void swap(int num1, int num2);

int do_fork();

void do_parent(FILE *fp);

void do_child_1(FILE *fp, int arr[], struct timeval t1, struct timeval t2);

void do_child_2(FILE *fp, int arr[], struct timeval t1, struct timeval t2);

void bubble_times_cal(FILE *fp, double *b_sum, double *bouble,
                      double *min, double *max);

void quick_times_cal(FILE *fp, double *q_sum, double *quik,
                    double *min, double *max);

void parent_wait(int status1, int status2);

void main_mission(FILE *fp, char *num_arg);

//========================== main =============================================
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("wrong arguments number, usage: ./<file name> file1 file2\n");
        exit(1);
    }

    struct timeval t0, t5;
    FILE *fp = fopen(argv[1], "w+"); //open file for read/write
    t0.tv_sec = t5.tv_sec = 0; //initialize time values

    gettimeofday(&t0, NULL);
    srand(17);

    main_mission(fp, argv[2]);

    rewind(fp);  //move file pt to beginning of file
    do_parent(fp);  //parent calculate all times data 
    gettimeofday(&t5, NULL);  //finish time measure of parent
    fclose(fp);
    //prints parent time
    printf("%f\n", (double) (t5.tv_usec - t0.tv_usec) / 1000000 +
                   (double) (t5.tv_sec - t0.tv_sec));

    return EXIT_SUCCESS;
}

//=============================================================================
//parent main mission, sorting arr with 2 childes at a time
void main_mission(FILE *fp, char *num_arg) {
    int i, j;

    for (i = 0; i < MISSION; i++) {
        int arr[SIZE];
        for (j = 0; j < SIZE; j++)
            arr[j] = rand() % atoi(num_arg); //put random values in arr

        int status1 = do_fork();    //create first child
        if (status1 == 0) {
            struct timeval t1, t2;
            t1.tv_sec = t2.tv_sec = 0;  //initialize time values
            do_child_1(fp, arr, t1, t2);
        }
        int status2 = do_fork();    //create second child
        if (status2 == 0) {
            struct timeval t3, t4;
            t3.tv_sec = t4.tv_sec = 0;  //initialize time values
            do_child_2(fp, arr, t3, t4);
        }
        parent_wait(status1, status2);
    }
}

//=============================================================================
//child 1 sorts arr with bubble sort
void do_child_1(FILE *fp, int arr[], struct timeval t1, struct timeval t2) {
    gettimeofday(&t1, NULL);
    bubble_sort(arr);
    gettimeofday(&t2, NULL);
    fprintf(fp, "b %f\n", (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
                          (double) (t2.tv_sec - t1.tv_sec));

    exit(EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
//child 2 sorts arr with quick sort
void do_child_2(FILE *fp, int arr[], struct timeval t1, struct timeval t2) {
    gettimeofday(&t1, NULL);
    quick_sort(arr, 0, SIZE - 1);
    gettimeofday(&t2, NULL);
    fprintf(fp, "q %f\n", (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
                          (double) (t2.tv_sec - t1.tv_sec));

    exit(EXIT_SUCCESS);
}

//-----------------------------------------------------------------------------
//parent calculate all sorts data & prints
void do_parent(FILE *fp) {
    double quickSum = 0, quick = 0, minQuick = 10, maxQuick = 0,
            bubbleSum = 0, bubble = 0, minBubble = 10, maxBubble = 0;
    int i = 0;
    while (i < MISSION) {
        quick_times_cal(fp, &quickSum, &quick, &minQuick, &maxQuick);
        bubble_times_cal(fp, &bubbleSum, &bubble, &minBubble, &maxBubble);
        i++;
    }
    printf("%lf %lf %lf %lf %lf %lf\n", bubbleSum / MISSION, quickSum / MISSION,
           minBubble, minQuick, maxBubble, maxQuick);

    unlink("out.txt");
}

//-----------------------------------------------------------------------------
//calculate the data time of bubble sort
void bubble_times_cal(FILE *fp, double *b_sum, double *bubble,
                      double *min, double *max) {
    fseeko(fp, 2, SEEK_CUR);
    fscanf(fp, "%lf", bubble);
    *b_sum += *bubble;
    if (*bubble < *min)
        *min = *bubble;
    if (*bubble > *max)
        *max = *bubble;
}

//-----------------------------------------------------------------------------
//calculate the data time of quick sort
void quick_times_cal(FILE *fp, double *q_sum, double *quick,
                     double *min, double *max) {
    fseeko(fp, 2, SEEK_CUR);
    fscanf(fp, "%lf", quick);
    *q_sum += *quick;
    if (*quick < *min)
        *min = *quick;
    if (*quick > *max)
        *max = *quick;
}

//-----------------------------bubble sort-------------------------------------
void bubble_sort(int arr[]) {
    int round, place, temp;

    for (round = 0; round < SIZE - 1; round++) {
        for (place = 0; place < SIZE - round - 1; place++) {
            if (arr[place] > arr[place + 1]) {
                temp = arr[place];
                arr[place] = arr[place + 1];
                arr[place + 1] = temp;
            }
        }
    }
}

//----------------------------quick sort----------------------------------------
void quick_sort(int arr[], int from, int to) {
    int pivot_place = partition(arr, from, to);

    if (from < pivot_place - 1)
        quick_sort(arr, from, pivot_place - 1);

    if (to > pivot_place + 1)    // (-)
        quick_sort(arr, pivot_place + 1, to);
}

//-------------------------------------------------------------
//partition function for quick_sort
int partition(int arr[], int from, int to) {
    int i, pivot_place = from;

    for (i = from + 1; i <= to; i++)
        if (arr[i] < arr[pivot_place]) {
            swap(arr[i], arr[pivot_place + 1]);
            swap(arr[pivot_place + 1], arr[pivot_place]);
            pivot_place++;
        }
    return pivot_place;
}

//---------------------------------------------------------------
//swap function for quick_sort
void swap(int num1, int num2) {
    int temp = num1;
    num1 = num2;
    num2 = temp;
}

//---------------------------------------------
//make fork & check if failed
int do_fork() {
    int status = fork();
    if (status < 0) {
        puts("cannot fork\n");
        exit(EXIT_FAILURE);
    }
    return status;
}

//--------------------------------------------------------------
//parent wait for childes to finish their process
void parent_wait(int child1, int child2) {
    wait(&child1);
    if (WEXITSTATUS(child1)) {
        puts("child 1 fail");
        exit(EXIT_FAILURE);
    }
    wait(&child2);
    if (WEXITSTATUS(child2)) {
        puts("child 2 fail");
        exit(EXIT_FAILURE);
    }
}
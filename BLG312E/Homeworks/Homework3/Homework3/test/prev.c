/*
 *  Lecture: Computer Operation Systems
 *  Project: 2
 *  Author: Kadir Emre Oto
 *  Student ID: 150140032
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>

#define SEMKEY 7        // special key
#define SEMKEY2 8       // special key 2
#define MAXAPL 10000    // maximum applicant count
#define NINT 3          // number of interviews

int waitR = 0;          // wait time for receptionist
int semid, semid2;      // semid: synchronizes interviewers,  semid2: waits for an applicant from queue
int processed = 1;      // number of processed applicants
char filename[100];

// increment operation
void semsignal(int semid, int val){
    struct sembuf semaphore;
    semaphore.sem_num = 0;
    semaphore.sem_op = val;
    semaphore.sem_flg = 1;
    semop(semid, &semaphore, 1) ;
}

// decrement operation
void semwait(int semid, int val){
    struct sembuf semaphore;
    semaphore.sem_num = 0;
    semaphore.sem_op = (-1*val);
    semaphore.sem_flg = 1;
    semop (semid, &semaphore, 1) ;
}

struct Queue{
    int precursor;
    int array[MAXAPL];
} queue;

void init(){                // initilizes queue
    queue.precursor = 0;
}

int pop(){                  // pops
    semwait(semid2, 1);
    semwait(semid, 1);

    queue.precursor++;
    semsignal(semid, 1);
    return queue.precursor;
}

void* interview(void* s){
    int applicant;
    int sid = (int) s;
    while (1){
        applicant = pop() - 1;
        printf("Interviewer %d started interview with Applicant %d\n", sid, applicant+1);
        sleep(queue.array[applicant]);
        printf("Interviewer %d finished interview with Applicant %d\n", sid, applicant+1);
        processed++;
    }
}

void* readfile(void* t){       // reads from file
    FILE *input = fopen(filename, "r");
    int number = 0;
    int cursor = 1;

    while (!feof(input)) {
        fscanf(input, "%d", &number);
        if (feof(input)) break;

        queue.array[cursor-1] = number;
        printf("Applicant %d applied to the receptionist\n", cursor++);
    }

    int i;

    for (i = 1; i < cursor; i++){
        sleep(waitR);
        printf("Applicant %d’s registeration is done\n", i);
        semsignal(semid2, 1);          // increase semaphore: represents size of queue
    }

    while (cursor > processed);         // waits for all applicants done
    pthread_exit(NULL);
}

int main(int argc, const char * argv[]){
    strcpy(filename, argv[1]);

    semid = semget(SEMKEY, 0, 0700|IPC_CREAT);              // create a semaphore for syncronizing interviewers
    semid2 = semget(SEMKEY2, 1, 0700|IPC_CREAT);            // create a semaphore for waiting queue

    semctl(semid, 0 , SETVAL, 1);       // set the value 1 for first interviewer
    semctl(semid2, 0 , SETVAL, 0);      // set the value 0 because queue is empty initially

    init();
    waitR = atoi(argv[2]);

    pthread_t receptionist;             // receptionist thread
    pthread_t interviewers [NINT+1];      // interviewers threads

    pthread_create(&receptionist, NULL, readfile, NULL);    // run receptionist

    int i;
    for (i = 1; i <= NINT; i++)
        pthread_create(interviewers+i, NULL, interview, (void *)i);

    pthread_join(receptionist, NULL);   // wait for all applicants done

    //    for (i = 1; i <= NINT; i++)         // it means
    //      pthread_cancel(interviewers[i]);

    semctl(semid, 0, IPC_RMID, 0);      // remove semaphores
    //semctl(semid2, 0, IPC_RMID, 0);      // remove semaphores
    printf("All applicants have interviewed successfully.\n");

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>
pthread_mutex_t mutex;
typedef struct {
    int pid ;
    int burst_time ;
    int priority ;
    int arrival_time ;
} Process ;
typedef struct {
    Process *a;
    int n;
    int mode;
} Args;

void initProcesses(Process queue[], int n) {
    for (int i = 0; i < 10; i++) {
        queue[i].pid = i+1;
        queue[i].burst_time = rand()%(25-5+1)+5;
        queue[i].priority = rand()%(10)+1;
        queue[i].arrival_time = rand()%(20+1);
        printf("PID: %d Burst: %d Priority: %2d Arrivel : %d |\n",
       queue[i].pid,
       queue[i].burst_time,
       queue[i].priority,
       queue[i].arrival_time);
    }


}
void sortProcesses(Process queue[], int n, int mode) {
    int i, j;
    if (mode == 1) {
        for (i = 1; i < n; i++) {
            Process key = queue[i];
            j = i - 1;


            while (j >= 0 && queue[j].arrival_time > key.arrival_time) {
                queue[j + 1] = queue[j];
                j--;
            }

            queue[j + 1]= key;
        }


    }
    else {
        for (int i = 1; i < n; i++) {
            int j = i - 1;
            Process temp = queue[i];
            while (j >= 0 && (queue[j].priority < temp.priority ||( queue[j].priority == temp.priority && queue[j].arrival_time > temp.arrival_time))) {
                queue[j + 1] = queue[j];
                j--;
            }
            queue[j + 1] = temp;


        }
    }
        if (mode == 1) {
            for (i = 0; i < n; i++) {
                printf("%d ", queue[i].arrival_time);
            }
        }
        else {
            for (i = 0; i < n; i++) {
                printf("%d ", queue[i].priority);
            }
        }


    };



void executeProcesses(Process queue[], int n, int algorithm) {
    Process Arrvied[10];

    int time = 0;
    int counter = 0;
    int finsh = 0;
    int added[10];

    int finishTime[10];
    int originalBurst[10];

    for (int i = 0; i < n; i++) {
        added[i] = 0;
        originalBurst[i] = queue[i].burst_time;
    }

    if (algorithm == 1) {

        while (finsh != n) {

            for (int i = 0; i < n; i++) {
                if (queue[i].arrival_time <= time && added[i] == 0) {
                    Arrvied[counter] = queue[i];
                    printf("Process %d arrived at time %d\n", queue[i].pid, time);
                    added[i] = 1;
                    counter++;
                }
            }

            if (counter == 0) {
                printf("time %d: IDLE\n", time);
                time++;
                continue;
            }


            for (int i = 1; i < counter; i++) {
                int j = i - 1;
                Process temp = Arrvied[i];
                while (j >= 0 && Arrvied[j].burst_time > temp.burst_time) {
                    Arrvied[j + 1] = Arrvied[j];
                    j--;
                }
                Arrvied[j + 1] = temp;
            }

            printf("Selected PID: %d burst %d\n", Arrvied[0].pid, Arrvied[0].burst_time);

            while (Arrvied[0].burst_time > 0) {

                printf("time %d: process %d running remaining %d\n",
                       time,
                       Arrvied[0].pid,
                       Arrvied[0].burst_time);

                Arrvied[0].burst_time--;
                time++;
            }

            finishTime[Arrvied[0].pid - 1] = time;

            for (int i = 1; i < counter; i++) {
                Arrvied[i - 1] = Arrvied[i];
            }

            counter--;
            finsh++;
        }
    }
    else {
        while (finsh != n) {

            for (int i = 0; i < n; i++) {
                if (queue[i].arrival_time <= time && added[i] == 0) {
                    Arrvied[counter] = queue[i];
                    printf("Process %d arrived at time %d\n", queue[i].pid, time);
                    added[i] = 1;
                    counter++;
                }
            }

            if (counter == 0) {
                printf("time %d: IDLE\n", time);
                time++;
                continue;
            }

            sortProcesses(Arrvied, counter, 2);

            printf("time %d: process %d running remaining %d\n",
                   time,
                   Arrvied[0].pid,
                   Arrvied[0].burst_time);

            Arrvied[0].burst_time--;
            time++;

            if (Arrvied[0].burst_time == 0) {

                finishTime[Arrvied[0].pid - 1] = time;

                for (int i = 1; i < counter; i++) {
                    Arrvied[i - 1] = Arrvied[i];
                }

                counter--;
                finsh++;
            }
        }
    }


    int index,waiting,turnaround;
    for (int i = 0; i < n; i++) {

        index = queue[i].pid - 1;
         turnaround = finishTime[index] - queue[i].arrival_time;
         waiting = turnaround - queue[i].burst_time;

        printf("PID %d Turnaround: %d Waiting: %d\n",queue[i].pid,turnaround,waiting);
    }
}
void* threadSort(void* arg) {
    Args*data = (Args*)arg;
    sortProcesses(data->a, data->n, data->mode);
    return NULL;
}

void* threadExecute(void* arg) {
    Args *data = (Args*)arg;
    executeProcesses(data->a, data->n, data->mode);
    return NULL;
}


int main(void) {
    srand(time(NULL));
    int n=10;
    Process *arr = mmap(NULL, n * sizeof(Process),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    if (arr == MAP_FAILED) {
        printf("Mapping failed\n");
        return 1;
    }
    initProcesses(arr, n);
    pid_t pid = fork();
    if (pid == 0) {


        sortProcesses(arr, n, 2);
        printf("children\n");
        for (int i = 0; i < 10; i++) {

            printf(" PID: %d  Burst: %d Priority: %d Arrivel : %d |\n",
           arr[i].pid,
           arr[i].burst_time,
           arr[i].priority,
           arr[i].arrival_time);

        }

        exit(0);
    }
    else {
        wait(NULL);

        printf("parent \n");
        for (int i = 0; i < 10; i++) {

            printf(" PID: %d  Burst: %d Priority: %d Arrivel : %d |\n",
           arr[i].pid,
           arr[i].burst_time,
           arr[i].priority,
           arr[i].arrival_time);

        }
        executeProcesses(arr, n, 2);


    }


    printf("threads part\n");
    Args args;
    args.a = arr;
    args.n = n;
    args.mode = 2;
    pthread_t t1,t2;

    pthread_mutex_init(&mutex,NULL);

    pthread_create(&t1, NULL, threadSort, &args);
    pthread_join(t1, NULL);

    pthread_create(&t2, NULL, threadExecute, &args);
    pthread_join(t2, NULL);


















    return 0;
}
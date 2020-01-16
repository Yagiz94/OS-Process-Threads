#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

/*
globals
*/
int minvalue;
int maxvalue;
int bincount;
int w; // bin-width
int N;
FILE *outputfile;
int *randomarr;
char line[256];

/*
generate random values here
*/
int randomval(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

// Main function
int main(int argc, char *argv[]) {
    minvalue = atoi(argv[1]);
    maxvalue = atoi(argv[2]);
    bincount = atoi(argv[3]);
    N = atoi(argv[4]);
    w = (maxvalue-minvalue) / bincount;

    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    int count[bincount],i;
    for(i = 0; i < bincount; i++) {
        count[i] = 0;
    }

    char *shm_file = "memory";
    int sharedMemoryKey = shm_open(shm_file, O_RDWR|O_CREAT, 0644);
    ftruncate(sharedMemoryKey, bincount*4);
    int * sharedMemory = mmap(NULL, bincount*4, PROT_READ|PROT_WRITE, MAP_SHARED, sharedMemoryKey, 0);


    char *fillSemName = "fill";
    char *doneSemName = "done";
    sem_t* fillSem = sem_open(fillSemName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    sem_t* doneSem = sem_open(doneSemName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);


    for(i = 0; i < N; i++) {
        char* file = argv[i+5];

        if(fork()==0) { // It means child process
            printf("fork child process with file name %s\n",file);

            FILE *inputFile = fopen(file,"r"); // r mode
            while (fgets (line, 256, inputFile) != NULL) {
                double inputNumber = atof(line);
                if(inputNumber >=  minvalue && inputNumber <=  maxvalue) {
                    int countIndex = (int)((inputNumber - minvalue)/w);
                    if(countIndex >= bincount)
                        countIndex = bincount-1;
                    count[countIndex]++;
                }
            }
            fclose(inputFile);

            fillSem = sem_open(fillSemName, O_RDWR);
            doneSem = sem_open(doneSemName, O_RDWR);
            sem_wait(fillSem);
            for (i = 0; i < bincount ; i++) {
                sharedMemory[i] = count[i];
            }
            sem_post(doneSem);

            sem_close(fillSem);
            sem_close(doneSem);
            exit(0);

        }//end of -child process
    }//end of for loop

    for(i = 0 ; i < N; i++) {
        sem_wait(doneSem);
        for (int j = 0; j < bincount ; j++) {
            count[j] += sharedMemory[j];
        }
        sem_post(fillSem);
    }

    for(i =0; i < N ; i++)
    {
        wait(NULL);// wait until all child processes end.
    }
    // Write the phistogram result into outputfile
    printf("Going to write\n");
    outputfile = fopen(argv[N+5], "w");
    for(i = 0; i < bincount; i++) {
        fprintf(outputfile, "%d: %d\n", (i+1),count[i]);
        printf("%d : %d\n", (i+1),count[i]);
    }
    gettimeofday(&end, NULL);
    double runtime = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    printf("total runtime with parameter N = %d", N);
    printf(":");
    printf(" %f", runtime);
    printf(" microseconds\n");

    sem_close(fillSem);
    sem_close(doneSem);
    sem_unlink(fillSemName);
    sem_unlink(doneSemName);
    return 0;
}

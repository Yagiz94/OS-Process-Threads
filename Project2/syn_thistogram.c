#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
/*
globals
*/
int minvalue;
int maxvalue;
int bincount;
int w; // bin-width
int N;
int batchsize;
int runningThreads;

FILE *outputfile;
int *binarray; /*Binarray stores the int counts for each bin */
int *FINAL_ARR;
pthread_t *threads;
int a = 0; // represents file indexes

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
/*
generate random values here
*/
int randomval(int min, int max) {
    return rand() % (max + 1 - min) + min;
}
typedef struct arg {
    double value;
    struct arg * next;
} Arguments;

Arguments* head = NULL;
void addElement(double element) {
    Arguments* node = malloc(sizeof(Arguments));
    node->value = element;
    node->next = head;
    head = node;
}
void *histogram(void *fileName) {
    FILE *inputFile = fopen(fileName,"r"); // r mode
    int elementNumber = 0;
    double batch[batchsize];
    char line[256];
    while (fgets (line, 256, inputFile) != NULL) {
        double inputNumber = atof(line);
        //printf("inputNumber = %f\n", inputNumber);

        batch[elementNumber] = inputNumber;
        elementNumber++;

        if(elementNumber == batchsize) {
            pthread_mutex_lock( &lock );
            while(elementNumber > 0) {
                elementNumber--;
                addElement(batch[elementNumber]);
            }
            pthread_cond_signal( &cond );
            pthread_mutex_unlock( &lock);
        }
    }
    fclose(inputFile);

    pthread_mutex_lock( &lock );
    while(elementNumber > 0) {
        elementNumber--;
        addElement(batch[elementNumber]);
    }
    //End
    runningThreads--;
    pthread_cond_signal( &cond );
    pthread_mutex_unlock( &lock);
    pthread_exit(0);

}//end of function

// Main function
int main(int argc, char *argv[]) {
    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    minvalue = atoi(argv[1]);
    maxvalue = atoi(argv[2]);
    bincount = atoi(argv[3]);
    N = atoi(argv[4]);
    runningThreads = N;
    w = (maxvalue-minvalue) / bincount;
    batchsize = atoi(argv[N+6]);

    FINAL_ARR = malloc(sizeof(int) * bincount); // Dynamic mem. alloc.
    threads = malloc(sizeof(pthread_t)*N);
    for(int con = 0; con < bincount; con++) {
        FINAL_ARR[con] = 0;
    }

    //needed values for thread creation
    for(int i = 0; i < N; i++) {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, histogram, (void *) argv[i+5]);
        threads[i] = tid;
    }

    while(runningThreads > 0 || head != NULL) {
        pthread_mutex_lock( &lock );
        if(head == NULL)
            pthread_cond_wait( &cond, &lock );
        int size = batchsize;
        while(head != NULL && size > 0) {
            size--;
            double inputNumber = head->value;
            head = head->next;
            if(inputNumber >=  minvalue && inputNumber <=  maxvalue) {
                int countIndex = (int)((inputNumber - minvalue)/w);
                if(countIndex >= bincount)
                    countIndex = bincount-1;
                //printf("inputNumber = %f index = %d\n", inputNumber,countIndex);
                FINAL_ARR[countIndex]++;
            }
        }
        pthread_mutex_unlock( &lock);

    }


    for(int y = 0; y < N; y++) {
        pthread_join(threads[y], NULL);
    } //wait until all threads finish

    outputfile = fopen(argv[N+5], "w");
    for(int i = 0; i < bincount; i++) {
        fprintf(outputfile, "%d: %d\n", (i+1),FINAL_ARR[i]);
        printf("%d : %d\n", (i+1),FINAL_ARR[i]);
    }
    gettimeofday(&end, NULL);
    double runtime = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    printf("total runtime with parameter N = %d", N);
    printf(":");
    printf(" %f", runtime);
    printf(" microseconds\n");

    return 0;
}

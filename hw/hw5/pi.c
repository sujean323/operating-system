#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/syscall.h> 
#include <assert.h>
#include <string.h>
#include <math.h>


long long int loopCount = 100000000;
int numCPU=-1;
char* exename;
int precision = -1;

/*
long gettid() {
    return (long int)syscall(__NR_gettid);
}
*/

//注意，我使用了「volatile」
volatile double score[100];

//long score[100];

void thread(void *givenName) {
    int id = (intptr_t)givenName;
    double x, y;
    for (int i=id; i<loopCount; i+=numCPU) {
		x = ((double)i/loopCount);
		y = sqrt(1 - x*x);
		score[id] += y;
    }
}

int main(int argc, char **argv) {
    exename = argv[0];
    long double up = 0, low = 0;
    long double x = 0, y = 0, z = 0;
    long long int tmp_up, tmp_low;
    
    precision = atoi(argv[1]);
    if(argc == 2) {
    	numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    }
    else{ 
    	numCPU = atoi(argv[2]);
    }
    pthread_t* tid = (pthread_t*)malloc(sizeof(pthread_t) * numCPU);

    while(1){
    	for(long i=0; i< numCPU; i++)
        	pthread_create(&tid[i], NULL, (void *) thread, (void*)i);

    	for(int i=0; i< numCPU; i++)
	    	pthread_join(tid[i], NULL);

    	double total=0;
    	for(int i=0; i< numCPU; i++)
        	total += score[i];
        
        x = (long double)1 - ((long double)1/loopCount);
        y = x*x;
        z = sqrt(1-y);
        
        up = ((long double)total)/(loopCount);
        low = up - ((long double)1.0/loopCount) + ((long double)1.0/loopCount)*z;
        
        up *= 4;
        low *= 4;
        
        tmp_up = up*pow(10, precision);
        tmp_low = low*pow(10, precision);
        
        if(tmp_up != tmp_low){
        	loopCount = loopCount * 5;
        	for(int i=0; i<numCPU; i++) score[i] = 0;
        }
        else break;
        
    }
    printf("num_CPU %d\n", numCPU);
    printf("pi = %.*Lf\n",precision ,up);
    //這一行讓我知道飛鏢射中幾次，除錯用
    //printf("\n%ld\n", total);
    //printf("pi = %f\n", ((double)total)/(loopCount*numCPU)*4);
}


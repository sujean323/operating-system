#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

//這裡和c11 spinlock一樣
/*
static inline int my_spin_lock (atomic_int *lock) {
    int val=0;													
    if (likely(atomic_exchange_explicit(lock, 1, memory_order_acq_rel) == 0))
        return 0;
    do {
        do {
            asm("pause");
        } while (*lock != 0);
        val = 0;
    } while (!atomic_compare_exchange_weak_explicit(lock, &val, 1, memory_order_acq_rel, memory_order_relaxed));
    return 0;
}
static inline int my_spin_unlock(atomic_int *lock) {
    atomic_store_explicit(lock, 0, memory_order_release);
    return 0;
}

atomic_int a_lock;
atomic_long count_array[256];
int numCPU;

void sigHandler(int signo) {
    for (int i=0; i<numCPU; i++) {
        printf("%i, %ld\n", i, count_array[i]);
    }
    exit(0);
}

atomic_int in_cs=0;
atomic_int wait=1;
*/

void thread(void *givenName) {
    int givenID = (intptr_t)givenName;							//從0依序編號
    srand((unsigned)time(NULL));								//把使用時間當成srand的seed
    unsigned int rand_seq;
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(givenID, &set);		//依thread編號在對應的cpu core上執行
    sched_setaffinity(gettid(), sizeof(set), &set);
    while(atomic_load_explicit(&wait, memory_order_acquire));	//wait,讓所有thread可以同時執行
    while(1) {
        my_spin_lock(&a_lock);									//lock
        atomic_fetch_add(&in_cs, 1);							//進入就+1,紀錄有幾個thread在cs
        atomic_fetch_add_explicit(&count_array[givenID], 1, memory_order_relaxed);
        //用array來計cs數
        if (in_cs != 1) {										//cs不只一個人
            printf("violation: mutual exclusion\n");			//錯誤提示
            exit(0);											//退出程式
        }
        atomic_fetch_add(&in_cs, -1);							//出來-1,紀錄有幾個thread在cs
        my_spin_unlock(&a_lock);								//unlock
        int delay_size = rand_r(&rand_seq)%7300;
        for (int i=0; i<delay_size; i++)						//random sleep一段時間
            ;
    }
}

int main(int argc, char **argv) {
    signal(SIGALRM, sigHandler);
    alarm(5);
    numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    pthread_t* tid = (pthread_t*)malloc(sizeof(pthread_t) * numCPU);

    for (long i=0; i< numCPU; i++)
        pthread_create(&tid[i],NULL,(void *) thread, (void*)i);
    atomic_store(&wait,0);

    for (int i=0; i< numCPU; i++)
        pthread_join(tid[i],NULL);
}

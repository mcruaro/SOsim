#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[2];
int counter;

void* doSomeThing(void *tid){
    unsigned long i = 0;
    counter += 1;
    printf("\nThread %d started - counter = %d\n", tid, counter);

    for(i=0; i<(0x2FFFFFFF); i++);
    printf("\nThread %d finished - counter = %d\n", tid, counter);

    return NULL;
}

int main(void){
    int i = 0;
    int err;

    while(i < 2) {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, (void *)i);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;
}


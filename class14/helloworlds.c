#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define N_THREADS 16

typedef struct thread_info {
    int num;
    pthread_t thread;
} thread_info_t;

void *thread_start(void *user) {
    thread_info_t *info = user;
    printf("Hello world from %d!\n", info->num);
    return NULL;
}

int main(int argc, char **argv) {
    thread_info_t thread_infos[N_THREADS];
    for (int i = 0; i < N_THREADS; i++) {
        thread_infos[i].num = i;
        pthread_create(&thread_infos[i].thread, NULL, thread_start, &thread_infos[i]);
    }
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(thread_infos[i].thread, NULL);
    }
    return 0;
}
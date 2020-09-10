#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define BUFFER_CAPACITY 8
#define MAX_MSG_LENGTH 30

int buffer[BUFFER_CAPACITY];
int buffer_size = 0;
bool producer_finished = false;
bool b_mutex = false;

// add this new global variable to your program
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer_thread(void *user) {
    for (int i = 0; i < MAX_MSG_LENGTH; i++) {
        while (buffer_size >= BUFFER_CAPACITY) {
            // wait a nanosecond, or as little as possible
            struct timespec interval = {0, 1}; // the former number is seconds, the latter is nanoseconds
            nanosleep(&interval, NULL);
        }
        if (b_mutex) {
            pthread_mutex_lock(&mutex);
        }
        buffer[buffer_size] = i;
        buffer_size += 1;
        if (b_mutex) {
            pthread_mutex_unlock(&mutex);
        }
    }
    producer_finished = true;
    return NULL;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        if (!strcmp(argv[1], "mutex")) {
            b_mutex = true;
        }
    }
    // Create Producer Thread;
    pthread_t thread;
    pthread_create(&thread, NULL, producer_thread, NULL);
    // In Main Thread Execute Consumer
    while (true) {
        if (buffer_size == 0 && producer_finished) {
            break;
        }
        if (buffer_size != 0) {
            if (b_mutex) {
                pthread_mutex_lock(&mutex);
            }
            buffer_size -= 1;
            int data_pop = buffer[buffer_size];
            buffer[buffer_size] = 0;
            if (b_mutex) {
                pthread_mutex_unlock(&mutex);
            }
            printf("%d\n", data_pop);
        }
    }
    return 0;
}
#define ARRAY_SIZE 20
#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5 
#define MAX_NAME_LENGTH 1024
#include <pthread.h>
#include <semaphore.h>

struct request{
    char* hello;
    char** shared_buffer;
    char** files_array;
    pthread_mutex_t* file_mutexes;
    pthread_mutex_t finished_mutex;
    pthread_mutex_t shared_requesters;
    int* finished;
    sem_t shared_sem;
    sem_t requester_sem;
    int num_files;
    FILE** file_pointers;
    int index;
};

struct resolve{
    char** shared_buffer;
    int* finished;
    sem_t shared_sem;
    pthread_mutex_t shared_resolvers;

};
// #define MAX_IP_LENGTH INET6 ADDRSTRLEN


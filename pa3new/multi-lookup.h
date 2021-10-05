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
    pthread_mutex_t *finished_mutex;
    pthread_mutex_t shared_requesters;
    pthread_mutex_t serviced_lock;
    int* finished;
    sem_t *shared_sem;
    pthread_mutex_t shared_sem_requesters;
    pthread_mutex_t requester_files;
    int num_files;
    FILE** file_pointers;
    int index;
    FILE* f_serviced;
    FILE* f_results;
    pthread_mutex_t shared_resolvers;
    pthread_mutex_t *shared_mutex;
    sem_t *resolver_starter;
    int num_resolvers;
    int *num_finished;
    int *finished_with_requesters;
    int num_requesters;
    //pthread_mutex_t requester_mutex;
    //pthread_mutex_t requester_shared_mutex;





};

struct resolve{
    char** shared_buffer;
    int* finished;
    sem_t *shared_sem;
    pthread_mutex_t results_lock;
    FILE* f_results;
    pthread_mutex_t shared_lock;
    pthread_mutex_t *shared_mutex;
    pthread_mutex_t *finished_mutex;
    sem_t *resolver_starter;
    int *finished_with_requesters;


};
// #define MAX_IP_LENGTH INET6 ADDRSTRLEN


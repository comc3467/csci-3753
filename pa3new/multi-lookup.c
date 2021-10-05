#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <sys/time.h>
#include "multi-lookup.h"







void * requester(struct request *thread);
void * resolver();
int finished_requests = 0;

int main(int argc, char* argv[]){
//timer start
    struct timeval start, end;
    gettimeofday(&start, NULL);



    int requester_threads = atoi(argv[1]);
    int resolver_threads = atoi(argv[2]);
    while(requester_threads > MAX_REQUESTER_THREADS){
        printf("Too many requester threads, give a number <= 5 \n");
        scanf("%d", &requester_threads);
    }
    while(resolver_threads > MAX_RESOLVER_THREADS){
        printf("Too many requester threads, give a number <= 5 \n");
        scanf("%d", &resolver_threads);
    }

    
    //use ./multi-lookup request# resolve# outputfile servicefile file[0] file[1]...
    

    char **shared_buffer = (char**)malloc(20 * sizeof(char*));
    for(int i = 0; i<ARRAY_SIZE; i++){
        shared_buffer[i] = (char*)malloc(1024);
    }
    char **files_array = (char**)malloc(11 * sizeof(char*));
    for(int i = 0; i<(argc-5); i++){
        files_array[i] = (char*)malloc(1024);
        strcpy(files_array[i], argv[5+i]);
    }

    sem_t *resolver_starter = malloc(sizeof(sem_t));
    sem_init(resolver_starter,0,0);

    sem_t *shared_sem = malloc(sizeof(sem_t));
    sem_init(shared_sem, 0,0);

    //sem_t requester_sem;
    //sem_init(&requester_sem,0,1);

    //sem_t shared_sem_requesters;
    //sem_init(&shared_sem_requesters,0,1);

    for(int i = 0; i < ARRAY_SIZE; i++){
        printf("%s\n", shared_buffer[i]);
    }

    pthread_mutex_t *shared_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(shared_mutex, NULL);
    pthread_mutex_t file_mutexes[argc-5];

    pthread_mutex_t *finished_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(finished_mutex, NULL);

    pthread_mutex_t shared_requesters;
    pthread_mutex_init(&shared_requesters, NULL);

    pthread_mutex_t shared_resolvers;
    pthread_mutex_init(&shared_resolvers, NULL);

    pthread_mutex_t shared_sem_requesters;
    pthread_mutex_init(&shared_sem_requesters,NULL);

    pthread_mutex_t requester_files;
    pthread_mutex_init(&requester_files,NULL);

    pthread_mutex_t shared_lock;
    if(pthread_mutex_init(&shared_lock, NULL) != 0){
        printf("Houston...\n");
    }


    for(int i = 0; i < (argc-5); i++){
        pthread_mutex_init(&file_mutexes[i], NULL);
    }

    int *finished_with_file = (int*)malloc((argc - 5) * sizeof(int));
    for(int i = 0; i < (argc-5); i++){
        finished_with_file[i] = 1;
    }

    int *num_finished = malloc(sizeof(int));
    *num_finished = 0;

    int *finished_with_requesters = malloc(sizeof(int));
    *finished_with_requesters = 0;
    pthread_t requester_id[requester_threads];
    pthread_t resolver_id[resolver_threads];

    //int *ind = malloc(sizeof(int));
    //ind = 0;
    FILE *f_results = fopen(argv[3], "w");
    FILE *f_serviced = fopen(argv[4], "w");
    FILE *file_pointers[argc-5];
    for(int i = 0; i < (argc-5); i++){
        file_pointers[i] = fopen(files_array[i], "r");
        if(file_pointers[i] == NULL){
            printf("Error! File cannot be opened.\n");
            printf("%s \n", files_array[i]);
        }
    }
    struct request n1;
    //n1 = (struct request *) malloc(sizeof(struct request));
    n1.files_array = files_array;

    n1.shared_buffer = shared_buffer;

    n1.file_mutexes = file_mutexes;

    n1.finished = finished_with_file;
    n1.shared_sem = shared_sem;
    n1.finished_mutex = finished_mutex;
    n1.num_files = (argc-5);
    n1.file_pointers = file_pointers;
    n1.index = 0;
    n1.requester_files = requester_files;
    n1.shared_requesters = shared_requesters;
    n1.shared_sem_requesters = shared_sem_requesters;
    char hello[5] = "hello";
    n1.hello = hello;
    n1.f_serviced = f_serviced;
    n1.f_results = f_results;
    n1.shared_resolvers = shared_resolvers;
    n1.serviced_lock = shared_lock;
    n1.shared_mutex = shared_mutex;
    n1.finished_with_requesters = finished_with_requesters;
    n1.num_finished = num_finished;
    n1.num_requesters = atoi(argv[1]);

    struct resolve n2;
    //n2 = (struct resolve *) malloc(sizeof(struct resolve));
    n2.shared_buffer = shared_buffer;
    n2.finished = finished_with_file;
    n2.results_lock = shared_resolvers;
    n2.f_results = f_results;
    n2.shared_sem = shared_sem;
    n2.shared_lock = shared_lock;
    n2.shared_mutex = shared_mutex;
    n2.finished_mutex = finished_mutex;
    n2.finished_with_requesters = finished_with_requesters;


    for(int i=0;i<requester_threads;i++){
        printf("(Before thread creation %d)\n",n1.num_files);
        pthread_create(&requester_id[i],NULL,(void *)requester, &n1);

        printf("Requester %d is thinking\n",i+1);
    }
    for(int j=0;j<resolver_threads;j++){
        pthread_create(&resolver_id[j],NULL,(void *)resolver,&n2);
        printf("Resolver %d is thinking\n",j+1);
    }
    for(int k=0;k<requester_threads;k++){
        pthread_join(requester_id[k],NULL);
    }
    for(int l=0;l<resolver_threads;l++){
        pthread_join(resolver_id[l],NULL);
    }
    //int *new;
    //scanf("%d", new);
    for(int m = 0; m<ARRAY_SIZE; m++){
        free(shared_buffer[m]);
    }
    
    free(shared_buffer);
    
    for(int i = 0; i<(argc-5); i++){
        fclose(file_pointers[i]);
        free(files_array[i]);
    }
    free(files_array);
    sem_destroy(shared_sem);
    free(shared_sem);
    free(shared_mutex);
    
    gettimeofday(&end, NULL);
    long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec); 
    //time help found at https://www.techiedelight.com/find-execution-time-c-program/
    printf("./multi-lookup %lu.%lu \n",seconds,micros);
}


void *requester(struct request *thread){
    //printf("%d \n",thread->index);
    //printf("%d \n",thread->num_files);
    //printf("%s \n", thread->hello);
    int counter = 0;
    while(thread->index < thread->num_files){
        pthread_mutex_lock(&thread->requester_files);
        if(thread->index < thread->num_files){
            counter++;         
            pthread_mutex_lock(&thread->file_mutexes[thread->index]);
            int remember = thread->index;
            thread->index += 1;
            pthread_mutex_unlock(&thread->requester_files);
            char line[1024];
            while (fgets(line, sizeof(line), thread->file_pointers[remember])){    
                line[strcspn(line, "\n")] = 0;
                int done = 0;
                pthread_mutex_lock(&thread->shared_sem_requesters);
                while(done == 0){ //while we look for a place to put the hostname
                    pthread_mutex_lock(thread->shared_mutex);
                    for(int i = 0; i < ARRAY_SIZE; i++){
                        if((strncmp(thread->shared_buffer[i] , "",4) == 0) && (done == 0)){ //empty
                            strcpy(thread->shared_buffer[i], line); //put line in
                            sem_post(thread->shared_sem);
                            pthread_mutex_unlock(thread->shared_mutex);
                            pthread_mutex_unlock(&thread->shared_sem_requesters);
                            done++; //found a spot            
                            break; //get out of the for loop
                        }
                    } //if it gets here without done incrementing, the array was full...
                    if(done == 0){
                        pthread_mutex_unlock(thread->shared_mutex);
                        //printf("buffer is full, requester waiting..\n");
                        sleep(3); //sleep it off a bit, hoping a resovler does its thing first               
                    }
                }
            }
            

            
        }
        else{
            pthread_mutex_unlock(&thread->requester_files);
        }
        //sem_post(&thread->requester_sem);
    }
    pthread_mutex_lock(&thread->serviced_lock);
    fprintf(thread->f_serviced,"Thread %zu serviced %d files. \n", pthread_self(), counter);
    *thread->num_finished += 1;
    if(*thread->num_finished == thread->num_requesters){
        *thread->finished_with_requesters = 1;
    }
    pthread_mutex_unlock(&thread->serviced_lock);
    return 0;
}

void *resolver(struct resolve *thread){
    sem_wait(thread->shared_sem); //we will only ever get into this loop, and only loop back if we are called to
    while(1){
        int done = 0;
        //int rest_empty = 0;
        pthread_mutex_lock(thread->shared_mutex);
        for(int i = 0; i < ARRAY_SIZE; i++){
            if((strncmp(thread->shared_buffer[i], "",4) != 0) && (done == 0)){
                char line[1024];
                strcpy(line, thread->shared_buffer[i]);
                //printf("Gotcha! %s \n", line);
                pthread_mutex_unlock(thread->shared_mutex);
                strcpy(thread->shared_buffer[i], "");
                done++;
                char ip[1024];
                int dns_return = dnslookup(line,ip, sizeof(ip));
                if(!dns_return){
                    pthread_mutex_lock(&thread->results_lock);
                    fprintf(thread->f_results, "%s,%s\n",line,ip);
                    pthread_mutex_unlock(&thread->results_lock);
                }
                else{
                    pthread_mutex_lock(&thread->results_lock);
                    fprintf(thread->f_results, "%s,\n",line);
                    pthread_mutex_unlock(&thread->results_lock);
                }
            }//if done is still 0 by now, we didn't find a line to ready, i.e. empty
        }
        if((*thread->finished_with_requesters == 1) && (done == 0)){ //we are done with the requesters, and its empty
            pthread_mutex_unlock(thread->shared_mutex); //allows the other threads to come to the same conclusion
            break; //get out of while(1)
        }
        else if(done == 0){ //just empty, but the requesters haven't finished yet...
            pthread_mutex_unlock(thread->shared_mutex);
            sem_wait(thread->shared_sem);
        } 
        //so at this point done == 1, meaning we've already unlocked
        else if(*thread->finished_with_requesters == 1){ //so we found something, but requesters are done
            if(sem_trywait(thread->shared_sem) != 0){ //we try to see if we can go back, meaning there's another thing back in there
                break;//don't go back, because the requesters are done and we don't have anything to go to
            }
        }

        else{ //meaning the requesters are not finished yet
            //printf("waiting here...\n");
            sem_wait(thread->shared_sem); //we wait here for them to go
        }
    }
    printf("Got to here \n");
    sem_post(thread->shared_sem); //just to finish things up
    return 0;
}


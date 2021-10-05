#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include "multi-lookup.h"


pthread_mutex_t shared_mutex;




void * requester(struct request *thread);
void * resolver();
int finished_requests = 0;

int main(int argc, char* argv[]){
    int requester_threads = atoi(argv[1]);
    int resolver_threads = atoi(argv[2]);
    while(requester_threads >= MAX_REQUESTER_THREADS){
        printf("Too many requester threads, give a number <= 5 \n");
        scanf("%d", &requester_threads);
    }
    while(resolver_threads >= MAX_RESOLVER_THREADS){
        printf("Too many requester threads, give a number <= 5 \n");
        scanf("%d", &resolver_threads);
    }
    
    //use ./multi-lookup request# resolve# outputfile servicefile file[0] file[1]...
    

    char **shared_buffer = (char**)malloc(10 * sizeof(char*));
    for(int i = 0; i<ARRAY_SIZE; i++){
        shared_buffer[i] = malloc(1024);
    }
    char **files_array = (char**)malloc(11 * sizeof(char*));
    for(int i = 0; i<11; i++){
        files_array[i] = (char*)malloc(1024);
        files_array[i] = argv[5+i];
    }
    sem_t shared_sem;
    sem_init(&shared_sem, 1,0);

    sem_t requester_sem;
    sem_init(&requester_sem,1,1);

    


    pthread_mutex_t file_mutexes[argc-5];

    pthread_mutex_t finished_mutex;
    pthread_mutex_init(&finished_mutex, NULL);

    pthread_mutex_t shared_requesters;
    pthread_mutex_init(&shared_requesters, NULL);

    pthread_mutex_t shared_resolvers;
    pthread_mutex_init(&shared_resolvers, NULL);


    for(int i = 0; i < (argc-5); i++){
        pthread_mutex_init(&file_mutexes[i], NULL);
    }

    int *finished_with_file = (int*)malloc((argc - 5) * sizeof(int));
    for(int i = 0; i < (argc-5); i++){
        finished_with_file[i] = 1;
    }    
    pthread_t requester_id[requester_threads];
    pthread_t resolver_id[resolver_threads];

    //int *ind = malloc(sizeof(int));
    //ind = 0;
    FILE *file_pointers[argc-5];
    for(int i = 0; i < (argc-5); i++){
        file_pointers[i] = fopen(files_array[i], "r");
    }
    struct request *n1;
    n1 = (struct request *) malloc(sizeof(struct request));
    n1->files_array = files_array;

    n1->shared_buffer = shared_buffer;

    n1->file_mutexes = file_mutexes;

    n1->finished = finished_with_file;
    n1->shared_sem = shared_sem;
    n1->finished_mutex = finished_mutex;
    n1->num_files = (argc-5);
    n1->file_pointers = file_pointers;
    n1->index = 0;
    n1->requester_sem = requester_sem;
    n1->shared_requesters = shared_requesters;
    char hello[5] = "hello";
    n1->hello = hello;
    //struct request new_request;
    //printf("%s",files_array[i]);
    //new_request.files_array = files_array;
    //new_request.shared_buffer = shared_buffer;
    //new_request.file_mutexes = file_mutexes;
    //new_request.finished = finished_with_file;
    //new_request.shared_sem = shared_sem;
    //new_request.finished_mutex = finished_mutex;
    //new_request.num_files = (argc-5);
    //new_request.file_pointers = file_pointers;
    //new_request.index = 0;
    //new_request.requester_sem = requester_sem;
    //new_request.shared_requesters = shared_requesters;
    for(int i=0;i<requester_threads;i++){
        printf("(Before thread creation %d)\n",n1->num_files);
        pthread_create(&requester_id[i],NULL,(void *)requester, n1);

        printf("Requester %d is thinking\n",i+1);
    }
    //int five;
    //scanf("%d", &five);
    struct resolve *n2;
    n2 = (struct resolve *) malloc(sizeof(struct resolve));
    n2->shared_buffer = shared_buffer;
    n2->finished = finished_with_file;
    n2->shared_resolvers = shared_resolvers;
    //struct resolve new_resolve;
    //new_resolve.shared_buffer = shared_buffer;
    //new_resolve.finished = finished_with_file;
    //new_resolve.shared_resolvers = shared_resolvers;
    for(int j=0;j<resolver_threads;j++){
        //pthread_create(&resolver_id[j],NULL,(void *)resolver,&n2);
        printf("Resolver %d is thinking\n",j+1);
    }
    for(int k=0;k<requester_threads;k++){
        pthread_join(requester_id[k],NULL);
    }
    for(int l=0;l<resolver_threads;l++){
        //pthread_join(resolver_id[l],NULL);
    }
    
    for(int m = 0; m<ARRAY_SIZE; m++){
        free(shared_buffer[m]);
    }
    free(shared_buffer);
    for(int i = 0; i<(argc-5); i++){
        fclose(file_pointers[i]);
        free(files_array[i]);
    }
    free(files_array);
    sem_destroy(&shared_sem);
    free(n1);
    free(n2);
}


void *requester(struct request *thread){
    int five;
    printf("%d \n",thread->index);
    printf("%d \n",thread->num_files);
    printf("%s \n", thread->hello);
    while(thread->index < thread->num_files){
        printf("Yes? \n");
        sem_wait(&thread->requester_sem);
        if(thread->index < thread->num_files){
            char hostname[MAX_NAME_LENGTH];
            printf("This is the index: %d \n", thread->index);
            scanf("%d", &five);
            pthread_mutex_lock(&thread->file_mutexes[thread->index]);
            scanf("%d", &five);
            char* line = NULL;
            ssize_t read;
            size_t len = 0;
            printf("Hello?");
            scanf("%d", &five);
            while ((read = getline(&line, &len, thread->file_pointers[thread->index])) != -1) {
                printf("Retrieved line of length %zu:\n", read);
                printf("%s", line);
            }
            scanf("%d", &five);
            while(fscanf(thread->file_pointers[thread->index],"%s", hostname) > 0){
                scanf("%d", &five);
                char* line = strdup(hostname);
                int done = 0;
                while(done == 0){
                    for(int i = 0; i < ARRAY_SIZE; i++){
                        if(!thread->shared_buffer[i]){
                            thread->shared_buffer[i] = line;
                            done++;
                            sem_post(&thread->shared_sem);
                        }
                    } //if it gets here without done incrementing, the array was full...
                    sleep(1); //sleep it off a bit, hoping a resovler does its thing first
                }
            }
            thread->index += 1;
        }
        sem_post(&thread->requester_sem);
    }
    //printf("HelloNow");
    //pthread_mutex_lock(&thread->finished_mutex); //lock so we are the only ones in the finished array
    //printf("Inside the requester mutex lock for checking the finished array");
    //printf("(First index: %d",thread->finished[0]);
    //int to_remember = -1;
    //int i = 0;
    //while(to_remember == -1){
    //    if(thread->finished[i] == 1){
    //        printf("HELLOP!!!\n");
    //        thread->finished[i] = 0; //make it zero so we know we've done that one
    //        to_remember = i; //remember that index for later
    //        printf("To_remember %d",to_remember);
    //        i++;
    //    }
    //    if(i > thread->num_files){
    //        printf("Find this error!?");
    //    }
    //}
    //printf("%s", thread->files_array[to_remember]);
    //FILE *fd = fopen(thread->files_array[to_remember], "r");
    //pthread_mutex_unlock(&thread->finished_mutex); //unlock mutex
    //printf("Unlocked the mutex after locking it) \n");
    //sem_wait(&thread->shared_sem);
    //while(fscanf(fd,"%s", hostname) > 0){
    //    char* line = strdup(hostname);
    //    pthread_mutex_lock(&thread->shared_requesters);
    //    int done = 0;
    //    while(done == 0){
    //        for(int i = 0; i < ARRAY_SIZE; i++){
    //            if(!thread->shared_buffer[i]){
    //                thread->shared_buffer[i] = line;
    //                done++;
    //                sem_post(&thread->shared_sem);
    //            }
    //        } //if it gets here without done incrementing, the array was full...
    //        pthread_mutex_unlock(&thread->shared_requesters);
    //        sleep(1); //sleep it off a bit, hoping a resovler does its thing first
    //        pthread_mutex_lock(&thread->shared_requesters);
    //    }
    //}//have to run back and see if there are still more files to look at
    return 0;
}

void *resolver(struct resolve *thread){
    int done = 0;
    sem_wait(&thread->shared_sem);
    while(done == 0){
        int empty = 0;
        pthread_mutex_lock(&shared_mutex);
        for(int i = 0; i < ARRAY_SIZE; i++){
            if(thread->shared_buffer[i]){
                char *line = thread->shared_buffer[i];
                printf("%s", line);
                done++;
            }
        }
        empty = 1; //if it gets here without done incrementing, the array was empty...
        pthread_mutex_unlock(&shared_mutex);
        if(empty && finished_requests){
            done++; //another way to escape, since the array could be empty but never going to be filled anymore
        }
        else{
            sleep(1);
        }
    }
    //get the ip address of the hostname, write on results.txt
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "main.h"

void *thread_task(void *x_void_ptr);
int start_thread_test(int sched);
void light_task(void);
void heavy_task(int id);
void print_result(void);

int res[NUM_THREAD];
pthread_mutex_t lock;

int main(int argc, char **argv)
{
    printf("Test will run with %d threads during %d seconds\n"
					, NUM_THREAD, TEST_TIME);
    printf("Initiating test... wait a bit\n");
    start_thread_test(0);
    start_thread_test(1);
    start_thread_test(2);
    
	return 0;
}

/*
 *Start_thread_test initate a scheduling policy test
 *The test starts by setting the policy
 *Followed by initiating the threads
 *The releases a lock to allow the threads to run
 *Lastly it printouts the result
 */
int start_thread_test(int sched){
    pthread_t p[NUM_THREAD];
    pthread_mutex_init(&lock, NULL);
    int id[NUM_THREAD];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    if(sched == 1){
	printf("Running on FIFO schedular\n");
    	if(pthread_attr_setschedpolicy(&attr, SCHED_FIFO) != 0){
            perror("Unable to set policy.\n");
 	    exit(1);
    	}
    }else if(sched == 2){
	printf("Running on ROUND ROBIN schedular\n");
	if(pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0){
            perror("Unable to set policy.\n");
 	    exit(1);
    	}
    }else{
	printf("Running on Standard schedular (should be CFS)\n");
	
    }

    pthread_mutex_lock(&lock);
    printf("Making threads\n");
    for(int i=0; i<NUM_THREAD; i++){
        id[i]=i;
        if(pthread_create(&p[i], &attr, thread_task, &id[i])) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    printf("running test...\n");
    pthread_mutex_unlock(&lock);
    for(int i=0; i<NUM_THREAD; i++){
        if(pthread_join(p[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }
    printf("Test complete, displaying result\n\n");
    
    print_result();
    
    return 0;
}

/*Prints out the result of the test run in a useful manner
 */
void print_result(void){
    int total_light=0;
    int max_light=0;
    int min_light=0;
    int total_heavy=0;
    int max_heavy=0;
    int min_heavy=0;

    pthread_mutex_lock(&lock);
    
    for(int i=0; i<NUM_THREAD/2; i++){
        if(min_light == 0 ){
            min_light = res[i];
        }
	
	if(min_light > res[i]){
            min_light = res[i];
        }
	
	if(max_light < res[i]){
            max_light = res[i];
        }
        total_light = total_light +res[i];
        //printf("Thread %d completed %d work laps\n", i+1, res[i]);
    }

    for(int i = NUM_THREAD/2; i < NUM_THREAD; i++){
	if(min_heavy == 0 ){
            min_heavy = res[i];
        }

	if(min_heavy > res[i]){
            min_heavy = res[i];
        }

	if(max_heavy < res[i]){
            max_heavy = res[i];
        }
        total_heavy = total_heavy +res[i];
        //printf("Thread %d completed %d work laps\n", i+1, res[i]);
    }

    pthread_mutex_unlock(&lock);
    total_light=total_light/(NUM_THREAD/2);
    printf("Average task laps completion for light task is: %d\n"
							, total_light);
    printf("Lowest amount of completed laps for light task: %d\n", min_light);
    printf("Highest amount of completed laps for light task: %d\n", max_light);
    printf("Gap between highest and lowest completions for light task: %d\n\n"
							, max_light-min_light);

    total_heavy=total_heavy/(NUM_THREAD/2);
    printf("Average task laps completion for heavy task is: %d\n", total_heavy);
    printf("Lowest amount of completed laps for heavy task: %d\n", min_heavy);
    printf("Highest amount of completed laps for heavy task: %d\n", max_heavy);
    printf("Gap between highest and lowest completions for heavy task: %d\n\n"
							, max_heavy-min_heavy);
}

/*The threads main function.
 *It runs a while loop and completes tasks given based on its ID
 *until the time runs out then it stops
 */
void *thread_task(void *x_void_ptr)
{
    int id = *(int *) x_void_ptr;
    int running=1;
    double msec=0;
    int laps = 0;
    struct timeval stop, start;

    pthread_mutex_lock(&lock);
    pthread_mutex_unlock(&lock);
    gettimeofday(&start, NULL);

    while(running){
	if(id < NUM_THREAD/2){
            light_task();
	}else{
            heavy_task(id);
	}
        
        gettimeofday(&stop, NULL);
        msec = ((double)stop.tv_sec*1000 + (double)stop.tv_usec * 1E-3)
		-((double)start.tv_sec*1000 + (double)start.tv_usec * 1E-3);
        
        if(msec >= 1000*TEST_TIME){
            running =0;
        }else{
            laps++;
        }
    }

    pthread_mutex_lock(&lock);
    res[id]=laps;
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

/*Simple light nonsense task
 */
void light_task(void)
{
    for(int i=0; i<10000; i++){
        double temp = 70000*8121/(10012+102121) * 0.10212121;
	temp=temp+temp;
    }
}

/* A heavier nonsense task using I/O operations.
 * can make files with it's id as name
 */
void heavy_task(int id)
{
    char buf[256];
    sprintf(buf, "%d", id);
    
    FILE *fp = fopen(buf, "ab+");
    if(!fp){
        perror("failed to open file");
        exit(1);
    }
    fputs(buf, fp);
    fgets(buf, 255, (FILE*)fp);
    
    fclose(fp);
}

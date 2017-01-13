#ifndef MAIN_H_   
#define MAIN_H_
/*Must be a even number*/
#define NUM_THREAD 10

/*Not that the test time is multiplied by three, since three threads runs*/
#define TEST_TIME 30

void *thread_task(void *x_void_ptr);
int start_thread_test(int sched);
void task1(void);
void task2(void);
int main(int argc, char **argv);
void task3(int id);
void print_result(void);

int res[NUM_THREAD];
pthread_mutex_t lock;

#endif 

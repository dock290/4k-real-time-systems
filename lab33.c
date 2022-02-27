#include <native/sem.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

// Установка начального значения семафора
#define SEM_INIT 0

// Установка режима работы семафора
// First In First Out (Queue)
#define SEM_MODE S_FIFO

// Объявление семафора
RT_SEM sem_desc1;
RT_SEM sem_desc2;

#define ITER 10

static RT_TASK t1;
static RT_TASK t2;
static RT_TASK t3;

int global = 0;

void taskOne(void *arg) {
  // Уменьшение счетчика семафора, если счетчик = 0 то включается блокировка
  // процесса
  // M_INFINITE causes the caller to block indefinitely until the request is
  // satisfied
  rt_sem_p(&sem_desc1, TM_INFINITE);
  int i;
  for (i = 0; i < ITER; i++) {
    rt_printf("I am taskOne and global = %d................\n", ++global);
  }
}

void taskTwo(void *arg) {
  rt_sem_p(&sem_desc2, TM_INFINITE);
  int i;
  for (i = 0; i < ITER; i++) {
    rt_printf("I am taskTwo and global = %d----------------\n", --global);
  }
  rt_sem_v(&sem_desc1);
}

void taskThree(void *arg) {
  int i;
  for (i = 0; i < ITER; i++) {
    rt_printf("I am taskThree and global = %d................\n", ++global);
  }
  rt_sem_v(&sem_desc2);
}

int main(int argc, char *argv[]) {
  /* Perform auto-init of rt_print buffers if the task doesn't do so */
  rt_print_auto_init(1);
  /* Avoids memory swapping for this program */
  mlockall(MCL_CURRENT | MCL_FUTURE);
  /* create the tasks */
  rt_task_create(&t1, "task1", 0, 1, 0);
  rt_task_create(&t2, "task2", 0, 1, 0);
  rt_task_create(&t3, "task3", 0, 1, 0);

  // Создать семафор...
  rt_sem_create(&sem_desc1, "MySemaphore1", SEM_INIT, SEM_MODE);
  rt_sem_create(&sem_desc2, "MySemaphore2", SEM_INIT, SEM_MODE);

  // Увеличение счетчика семафора
  // rt_sem_v(&sem_desc2);

  /* start the tasks */
  rt_task_start(&t1, &taskOne, 0);
  rt_task_start(&t2, &taskTwo, 0);
  rt_task_start(&t3, &taskThree, 0);
  return 0;
}

#include <native/task.h>
#include <native/timer.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

unsigned long SECONDS(unsigned long time) { return time * 1000000000; }

RT_TASK demo_task;
RT_TASK names_task;

/* NOTE: error handling omitted. */
void demo(void* arg) {
  RTIME now, previous;
  /*
   * Arguments: &task (NULL=self),
   * start time,
   * period (here: 1 s)
   */
  rt_task_set_periodic(NULL, TM_NOW, 500000000);
  previous = rt_timer_read();
  while (1) {
    rt_task_wait_period(NULL);
    now = rt_timer_read();
    /*
     * NOTE: printf may have unexpected impact on the timing of
     * your program. It is used here in the critical loop
     * only for demonstration purposes.
     */
    printf("Time since last turn: %ld.%06ld ms\n",
           (long)(now - previous) / 1000000, (long)(now - previous) % 1000000);
    previous = now;
  }
}

void names(void* arg) {
  RTIME now, previous;
  /*
   * Arguments: &task (NULL=self),
   * start time,
   * period (here: 1 s)
   */
  rt_task_set_periodic(NULL, TM_NOW, SECONDS(2));
  previous = rt_timer_read();
  while (1) {
    rt_task_wait_period(NULL);
    now = rt_timer_read();
    /*
     * NOTE: printf may have unexpected impact on the timing of
     * your program. It is used here in the critical loop
     * only for demonstration purposes.
     */
    printf("Kirgizov Kuznetcov Dunaeva. Time since last turn: %ld.%06ld ms\n",
           (long)(now - previous) / 1000000, (long)(now - previous) % 1000000);
    previous = now;
  }
}

void catch_signal(int sig) {}

int main(int argc, char* argv[]) {
  signal(SIGTERM, catch_signal);
  signal(SIGINT, catch_signal);
  /* Avoids memory swapping for this program */
  mlockall(MCL_CURRENT | MCL_FUTURE);
  /*
   * Arguments: &task,
   * name,
   * stack size (0=default),
   * priority,
   * mode (FPU, start suspended, ...)
   */
  rt_task_create(&demo_task, "trivial", 0, 1, 0);
  rt_task_create(&names_task, "names", 0, 99, 0);
  /*
   * Arguments: &task,
   * task function,
   * function argument
   */
  rt_task_start(&demo_task, &demo, NULL);
  rt_task_start(&names_task, &names, NULL);
  pause();
  rt_task_delete(&demo_task);
  rt_task_delete(&names_task);
}

#include <native/mutex.h>
#include <native/queue.h>
#include <native/sem.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define NTASKS 4
RT_TASK task_struct[NTASKS];

#define QUEUE_SIZE 255
#define QUEUE_SIZE 255
#define MAX_MESSAGE_LENGTH 40
RT_QUEUE myqueue;

void taskOne(void *arg) {
  int retval;
  char message[] = "Message from taskOne";
  /* send message */
  retval = rt_queue_write(&myqueue, message, sizeof(message), Q_NORMAL);
  if (retval < 0) {
    rt_printf("Sending error\n");
  } else {
    rt_printf("taskOne sent message to mailbox\n");
  }
}

void taskTwo(void *arg) {
  int retval;
  char msgBuf[MAX_MESSAGE_LENGTH];
  /* receive message */
  retval = rt_queue_read(&myqueue, msgBuf, sizeof(msgBuf), TM_INFINITE);
  if (retval < 0) {
    rt_printf("Receiving error\n");
  } else {
    rt_printf("taskTwo received message: %s\n", msgBuf);
    rt_printf("with length %d\n", retval);
  }
}

void taskSF(void *arg) {
  int retval;
  char message[] = "Kirgizov Dunaeva Kuznetcov";
  /* send message */
  retval = rt_queue_write(&myqueue, message, sizeof(message), Q_NORMAL);
  if (retval < 0) {
    rt_printf("Sending error\n");
  } else {
    rt_printf("taskSF sent message to mailbox\n");
  }
}

void taskRF(void *arg) {
  int retval;
  char msgBuf[MAX_MESSAGE_LENGTH];
  /* receive message */
  retval = rt_queue_read(&myqueue, msgBuf, sizeof(msgBuf), TM_INFINITE);
  if (retval < 0) {
    rt_printf("Receiving error\n");
  } else {
    rt_printf("taskRF received message: %s\n", msgBuf);
    rt_printf("with length %d\n", retval);
  }
}

// startup code
void startup() {
  int i;
  char str[10];
  void (*task_func[NTASKS])(void *arg);
  task_func[0] = taskOne;
  task_func[1] = taskTwo;
  task_func[2] = taskSF;
  task_func[3] = taskRF;
  rt_queue_create(&myqueue, "myqueue", QUEUE_SIZE, 10, Q_FIFO);
  rt_timer_set_mode(0);  // set timer to tick in nanoseconds and not in jiffies
  for (i = 0; i < NTASKS; i++) {
    rt_printf("start task : %d\n", i);
    sprintf(str, "task%d", i);
    rt_task_create(&task_struct[i], str, 0, 50, 0);
    rt_task_start(&task_struct[i], task_func[i], &i);
  }
}

void init_xenomai() {
  /* Avoids memory swapping for this program */
  mlockall(MCL_CURRENT | MCL_FUTURE);
  /* Perform auto-init of rt_print buffers if the task doesn't do so */
  rt_print_auto_init(1);
}

int main(int argc, char *argv[]) {
  printf("\nType CTRL-C to end this program\n\n");
  // code to set things to run xenomai
  init_xenomai();
  // startup code
  startup();
  pause();
}

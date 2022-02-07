#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

RT_TASK demo_task;
RT_TASK names_task;

void demo(void *arg) {
  RT_TASK *curtask;
  RT_TASK_INFO curtaskinfo;
  // hello world
  rt_printf("Hello World!\n");
  // запрос текущей задачи
  curtask = rt_task_self();
  rt_task_inquire(curtask, &curtaskinfo);
  // печатаем имя задачи
  rt_printf("Task name : %s \n", curtaskinfo.name);
}

void names(void *arg) {
  RT_TASK *curtask;
  RT_TASK_INFO curtaskinfo;
  rt_printf("Kirgizov Kuznetcov\n");
  // запрос текущей задачи
  curtask = rt_task_self();
  rt_task_inquire(curtask, &curtaskinfo);
  // печатаем имя задачи
  rt_printf("Task name : %s \n", curtaskinfo.name);
}

int main(int argc, char *argv[]) {
  char str[64];

  // выполняем авто-инициализацию rt_print buffers если задача этого не
  // сделала...
  rt_print_auto_init(1);

  // Запрет использования swap для программы...
  mlockall(MCL_CURRENT | MCL_FUTURE);
  rt_printf("start task\n");

  // Arguments: &task,
  //    name,
  //    stack size (0=default),
  //    priority,
  //    mode (FPU, start suspended, ...)
  sprintf(str, "hello");
  rt_task_create(&demo_task, str, 0, 50, 0);

  sprintf(str, "Kirgizov Kuznetcov task name");
  rt_task_create(&names_task, str, 0, 20, 0);

  // Arguments: &task,
  //    task function,
  //    function argument
  rt_task_start(&demo_task, &demo, 0);
  rt_task_start(&names_task, &names, 0);
}

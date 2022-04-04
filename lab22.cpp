#include <Arduino_FreeRTOS.h>
#include <MultiFunctionShield.h>

MultiFunctionShield MFS;

TaskHandle_t tasks[3];
TaskHandle_t taskManual;

void TaskPrint(void *pvParams);

void setup() {
  Serial.begin(9600);
  MFS.begin();

  pinMode(BUTTON_1_PIN, INPUT);

  xTaskCreate(TaskPrintFirst, "PrintFirst", 128, NULL, 0, &tasks[0]);

  xTaskCreate(TaskPrintSecond, "PrintSecond", 128, NULL, 0, &tasks[1]);
  vTaskSuspend(tasks[1]);

  xTaskCreate(TaskPrintThird, "PrintThird", 128, NULL, 0, &tasks[2]);
  vTaskSuspend(tasks[2]);

  xTaskCreate(TaskPrintManual, "PrintManual", 128, NULL, 0, &taskManual);
  vTaskSuspend(taskManual);
}

void loop() {}

void noize() {
  digitalWrite(BUZZER_PIN, LOW);
  delay(20);
  digitalWrite(BUZZER_PIN, HIGH);
}

bool handleInitialMode(int printMode) {
  printNumber(printMode);

  bool buttonHold = false;
  long startTime = xTaskGetTickCount();
  while (true) {
    bool button1Pressed = !digitalRead(BUTTON_1_PIN);
    if (button1Pressed && !buttonHold) {
      return true;
      buttonHold = true;
    } else if (!button1Pressed) {
      buttonHold = false;
    }

    if (Serial.available() > 0) {
      if (Serial.read() == 'q') {
        noize();
        return true;
      }
    }

    long currentTime = xTaskGetTickCount();
    long elapsedTime = currentTime - startTime;
    if (elapsedTime > 100) {
      return false;
    }
  }
  return false;
}

void printNumber(int printMode) {
  switch (printMode) {
    case 0: {
      Serial.println(5, DEC);
      MFS.Display(05);
    } break;

    case 1: {
      Serial.println(8, DEC);
      MFS.Display(8);
    } break;

    case 2: {
      Serial.println(10, DEC);
      MFS.Display(10);
    } break;
  }
}

void TaskPrintFirst(void *pvParams) {
  int printMode = 0;
  while (true) {
    bool toManual = handleInitialMode(printMode);
    if (toManual) {
      vTaskResume(taskManual);
    } else {
      vTaskResume(tasks[1]);
    }
    vTaskSuspend(tasks[0]);
  }
}

void TaskPrintSecond(void *pvParams) {
  int printMode = 1;
  while (true) {
    bool toManual = handleInitialMode(printMode);
    if (toManual) {
      vTaskResume(taskManual);
    } else {
      vTaskResume(tasks[2]);
    }
    vTaskSuspend(tasks[1]);
  }
}

void TaskPrintThird(void *pvParams) {
  int printMode = 2;
  while (true) {
    bool toManual = handleInitialMode(printMode);
    if (toManual) {
      vTaskResume(taskManual);
    } else {
      vTaskResume(tasks[0]);
    }
    vTaskSuspend(tasks[2]);
  }
}

void TaskPrintManual(void *pvParams) {
  int printMode = 0;
  bool buttonHold = false;
  long startTime = xTaskGetTickCount();
  while (true) {
    bool button1Pressed = !digitalRead(BUTTON_1_PIN);
    if (button1Pressed && !buttonHold) {
      printMode++;
      if (printMode > 2) {
        printMode = 0;
      }
      printNumber(printMode);
      buttonHold = true;
    } else if (!button1Pressed) {
      buttonHold = false;
    }

    if (Serial.available() > 0) {
      char ch = Serial.read();
      if (ch == '1') {
        noize();
        printMode = 0;
      } else if (ch == '2') {
        noize();
        printMode = 1;
      } else if (ch == '3') {
        noize();
        printMode = 2;
      } else if (ch == 'q') {
        noize();
        printMode++;
      } else if (ch == '0') {
        noize();
        printMode = -1;
      }
    }

    if (printMode > 2) {
      printMode = 0;
    }

    long currentTime = xTaskGetTickCount();
    long elapsedTime = currentTime - startTime;
    if (elapsedTime > 100) {
      printNumber(printMode);
      startTime = currentTime;
    }

    if (printMode == -1) {
      vTaskResume(tasks[0]);
      vTaskSuspend(taskManual);
    }
  }
}

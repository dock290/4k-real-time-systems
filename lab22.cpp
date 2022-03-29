#include <MultiFunctionShield.h>
#include <Arduino_FreeRTOS.h>

MultiFunctionShield MFS;

TaskHandle_t tasks[3];
TaskHandle_t taskManual;

void TaskPrint(void *pvParams);

void setup() {
  Serial.begin(9600);
  MFS.begin();

  pinMode(BUTTON_1_PIN, INPUT);

  xTaskCreate(TaskPrintFirst,
              "PrintFirst",
              128,
              NULL,
              0,
              &tasks[0]
             );
             
    xTaskCreate(TaskPrintSecond,
                "PrintSecond",
                128,
                NULL,
                0,
              &tasks[1]
      );
    vTaskSuspend(tasks[1]);
    
    xTaskCreate(TaskPrintThird,
                "PrintThird",
                128,
                NULL,
                0,
              &tasks[2]
      );
    vTaskSuspend(tasks[2]);
    
    xTaskCreate(TaskPrintManual,
                "PrintManual",
                128,
                NULL,
                0,
              &taskManual
      );
    vTaskSuspend(taskManual);
}


void loop () {
}

// 0 - Not pressed
// 1 - Pressed
// 2 - Was read
int updateButtonMode() {
  int buttonPressed = 0;
  // If not pressed or if was read
  if (buttonPressed == 0 || buttonPressed == 2) {
    int newButtonPressed = !digitalRead(BUTTON_1_PIN);
    if (newButtonPressed == 1 && buttonPressed == 0) {
      digitalWrite(BUZZER_PIN, LOW);
      delay(20);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
    
      buttonPressed = 1;
    }
  }
  return buttonPressed;
}

bool handleInitialMode(int printMode) {
  int buttonPressed = updateButtonMode();
  
  if (buttonPressed == 1) {
    printMode = 1;
    return true;
  }

  if (Serial.available() > 0) {
    if (Serial.read() == 'q') {
      digitalWrite(BUZZER_PIN, LOW);
      delay(20);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      printMode = 1;
      return true;
    }
  }

  if (buttonPressed == 1) {
    buttonPressed = 2;
  }

  printNumber(printMode);
  return false;
}

int handleManualMode(int printMode) {
  int buttonPressed = updateButtonMode();
  if (buttonPressed) {
    return printMode + 1;
  }
  
  if (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == '1') {
      digitalWrite(BUZZER_PIN, LOW);
      delay(20);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      return 0;
    } else if (ch == '2') {
      digitalWrite(BUZZER_PIN, LOW);
      delay(20);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      return 1;
    } else if (ch == '3') {
      digitalWrite(BUZZER_PIN, LOW);
      delay(20);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      return 2;
    } else if (ch == 'q') {
      digitalWrite(BUZZER_PIN, LOW);
      delay(20);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      return printMode + 1;
    }
  }

  printNumber(printMode);
  return printMode;
}

void printNumber(int printMode) {
  switch (printMode) {
    case 0: {
        Serial.println(5, DEC);
        MFS.Display(0);
        delay(500);
        MFS.Display(5);
        delay(500);
      } break;

    case 1: {
        Serial.println(8, DEC);
        MFS.Display(0);
        delay(500);
        MFS.Display(8);
        delay(500);
      } break;

    case 2: {
        Serial.println(10, DEC);
        MFS.Display(1);
        delay(500);
        MFS.Display(0);
        delay(500);
      } break;

    default: {
        Serial.println("printNumber() wrong param");
        MFS.Display(1111);
        delay(500);
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
  while (true) {
    printMode = handleManualMode(printMode);
    
      if (printMode > 2) {
        printMode = 0;
      }
  }
}

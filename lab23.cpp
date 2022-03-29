#include <MultiFunctionShield.h>
#include <Arduino_FreeRTOS.h>

MultiFunctionShield MFS;

TaskHandle_t taskStandby;
TaskHandle_t taskSettings;
TaskHandle_t taskHeat;

const int LED_D4 = 10;
const int LED_D3 = 11;
const int LED_D2 = 12;
const int LED_D1 = 13;

void setup() {
  MFS.begin();

  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  pinMode(BUTTON_3_PIN, INPUT);
  
  pinMode(LED_D4, OUTPUT);
  analogWrite(LED_D4, 255);
  pinMode(LED_D3, OUTPUT);
  analogWrite(LED_D3, 255);
  pinMode(LED_D2, OUTPUT);
  analogWrite(LED_D2, 255);
  pinMode(LED_D1, OUTPUT);
  analogWrite(LED_D1, 255);

  xTaskCreate(TaskStandby,
              "TaskStandby",
              128,
              NULL,
              0,
              &taskStandby
             );
             
    xTaskCreate(TaskSettings,
                "TaskSettings",
                128,
                NULL,
                0,
              &taskSettings
      );
    vTaskSuspend(taskSettings);
    
    xTaskCreate(TaskHeat,
                "TaskHeat",
                128,
                NULL,
                0,
              &taskHeat
      );
    vTaskSuspend(taskHeat);
}


void loop () {
}

void makeSomeNoize() {
  digitalWrite(BUZZER_PIN, LOW);
  delay(20);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
}

bool handleWaitMode(int printMode) {
  
  return false;
}


void TaskStandby(void *pvParams) {
  int printMode = 0;
  long standbyTime = xTaskGetTickCount();
  long heatModeTime = 0;
  bool ledOn = false;
  do {
    bool button1Pressed = !digitalRead(BUTTON_1_PIN);
    if (button1Pressed) {
      analogWrite(LED_D1, 255);
      vTaskResume(taskSettings);
      vTaskSuspend(taskStandby);
    }

    bool button2Pressed = !digitalRead(BUTTON_2_PIN);
    if (button2Pressed) {
      if (heatModeTime == 0) {
        heatModeTime = xTaskGetTickCount();
      }
    } else {
      long currentTime = xTaskGetTickCount();
      long elapsedTime = currentTime - heatModeTime;
      if (heatModeTime >= 100) {
        analogWrite(LED_D1, 255);
        vTaskResume(taskHeat);
        vTaskSuspend(taskStandby);
      }
      heatModeTime = 0;
    }

    long currentTime = xTaskGetTickCount();
    long elapsedTime = currentTime - standbyTime;
    if (elapsedTime >= 100 /* * 60 */) {
      int minutes = (currentTime / 100 /* / 60 */) % 1000;
 
      MFS.Display(minutes);

      ledOn = !ledOn;
      analogWrite(LED_D1, ledOn ? 0 : 255);

      standbyTime = currentTime;
    }
  } while (true);
}

int getPowerByMode(int powerMode) {
  switch (powerMode) {
    case 0: 
      return 20;
    case 1: 
      return 60;
    case 2: 
    default:
      return 100;
  }
}

int powerMode = 2;
int heatTime = 3;

void TaskSettings(void *pvParams) {
  powerMode = 0;
  heatTime = 0;
  
  bool heatMode = false;  

  long menuButtonPressedTime = 0;
  long modeButtonPressedTime = 0;

  MFS.Display(heatMode ? heatTime : getPowerByMode(powerMode));
  analogWrite(heatMode ? LED_D3 : LED_D2, 0);
  do {
    bool button1Pressed = !digitalRead(BUTTON_1_PIN);
    if (button1Pressed) {
      if (menuButtonPressedTime == 0) {
        menuButtonPressedTime = xTaskGetTickCount();
      }
    }
    
    bool button2Pressed = !digitalRead(BUTTON_2_PIN);
    if (button2Pressed) {
      if (modeButtonPressedTime == 0) {
        modeButtonPressedTime = xTaskGetTickCount();
      }
    }

    if (!button1Pressed) {
      if (menuButtonPressedTime != 0) {
        long currentTime = xTaskGetTickCount();
        long elapsedTime = currentTime - menuButtonPressedTime;
        if (elapsedTime > 0 && elapsedTime < 100) {
          heatMode = !heatMode;
          MFS.Display(heatMode ? heatTime : getPowerByMode(powerMode));
        }
        analogWrite(heatMode ? LED_D2 : LED_D3, 255);
        analogWrite(heatMode ? LED_D3 : LED_D2, 0);
        menuButtonPressedTime = 0;
      }
    }
    if (!button2Pressed) {
      if (modeButtonPressedTime != 0) {
        long currentTime = xTaskGetTickCount();
        long elapsedTime = currentTime - modeButtonPressedTime;
        if (elapsedTime > 0 && elapsedTime < 100) {
          if (heatMode) {
            heatTime++;
            if (heatTime > 9) {
              heatTime = 0;
            }
          } else {
            powerMode++;
            if (powerMode > 2) {
              powerMode = 0;
            }
          }
          MFS.Display(heatMode ? heatTime : getPowerByMode(powerMode));
        } else if (elapsedTime > 0 && elapsedTime >= 100) {
          analogWrite(LED_D2, 255);
          analogWrite(LED_D3, 255);
          vTaskResume(taskHeat);
          vTaskSuspend(taskSettings);
        }
        modeButtonPressedTime = 0;
      }
    }
  } while(true);
}

void TaskHeat(void *pvParams) {
  analogWrite(LED_D4, 255);
  
  MFS.Display(getPowerByMode(powerMode));
  delay(500);
  MFS.Display(heatTime);

  bool noize = true;

  bool buttonHold = false;
  
  long heatStartTime = xTaskGetTickCount();
  do {
    long currentTime = xTaskGetTickCount();
    long elapsedTime = currentTime - heatStartTime;
    if (elapsedTime > 100) {
      MFS.Display(heatTime);
      heatStartTime = currentTime;

      if (heatTime == 0 && noize == true) {
        noize = false;
        makeSomeNoize();
      }
      
      if (heatTime > 0) {
        heatTime--;
      }
    }

    if (heatTime != 0) {
      bool button2Pressed = !digitalRead(BUTTON_2_PIN);
      if (button2Pressed && !buttonHold) {
        buttonHold = true;
        heatTime += 2;
        MFS.Display(heatTime);
      } else if (!button2Pressed) {
        buttonHold = false;
      }
    }
  } while(true);
}

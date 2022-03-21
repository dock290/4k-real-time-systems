#include <MultiFunctionShield.h>

MultiFunctionShield MFS;

void setup() {
  MFS.begin();

  pinMode(BUTTON_1_PIN, INPUT);
}

bool manualMode = false;
int printMode = 0;

void loop() {
  if (printMode >= 3) {
    printMode = 0;
  }

  if (manualMode) {
    handleManualMode();
  } else {
    handleInitialMode();
  }
}

void handleInitialMode() {
  if (!digitalRead(BUTTON_1_PIN)) {
    digitalWrite(BUZZER_PIN, LOW);
    delay(20);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);

    manualMode = true;
    printMode = 0;

    return;
  }

  printNumber(printMode);
  
  printMode++;
}

void handleManualMode() {
  if (!digitalRead(BUTTON_1_PIN)) {
    digitalWrite(BUZZER_PIN, LOW);
    delay(20);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);

    printMode++;
    if (printMode >= 3) {
      printMode = 0;
    }
  }

  printNumber(printMode);
}

void printNumber(int printMode) {
  switch (printMode) {
    case 0: {
      MFS.Display(0);
      delay(500);
      MFS.Display(5);
      delay(500);
    } break;

    case 1: {
      MFS.Display(0);
      delay(500);
      MFS.Display(8);
      delay(500);
    } break;

    case 2: {
      MFS.Display(1);
      delay(500);
      MFS.Display(0);
      delay(500);
    } break;

    default:
      break;
  }
}

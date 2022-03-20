#include <MultiFunctionShield.h>

#define LATCH_DIO 4
#define CLK_DIO 7
#define DATA_DIO 8

// typedef char byte;

// Number codes to print digit on mfs' display
const byte SEGMENT_MAP[] = {
    0xC0,  // 0
    0xF9,  // 1
    0xA4,  // 2
    0xB0,  // 3
    0x99,  // 4
    0x92,  // 5
    0x82,  // 6
    0xF8,  // 7
    0X80,  // 8
    0X90,  // 9
};

// Codes to select mfs' display
const byte SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void setup() {
  uint8_t i;

  MultiFunctionShield MFS;
  MFS.begin();

  // pinMode(PIN_BUTTON, INPUT);

  pinMode(LATCH_DIO, OUTPUT);
  pinMode(CLK_DIO, OUTPUT);
  pinMode(DATA_DIO, OUTPUT);
}

bool manualMode = false;
int printMode = 0;

void loop() {
  if (printMode >= 4) {
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
    digitalWrite(BUTTON_1_PIN, LOW);
    delay(20);
    digitalWrite(BUTTON_1_PIN, HIGH);
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
    digitalWrite(BUTTON_1_PIN, LOW);
    delay(20);
    digitalWrite(BUTTON_1_PIN, HIGH);
    delay(500);

    printMode++;
    if (printMode >= 4) {
      printMode = 0;
    }
  }

  printNumber(printMode);
}

void printNumber(int printMode) {
  switch (printMode) {
    case 0: {
      printNumberToDisplay(0, 0);
      printNumberToDisplay(1, 5);
    } break;

    case 1: {
      printNumberToDisplay(0, 0);
      printNumberToDisplay(1, 8);
    } break;

    case 2: {
      printNumberToDisplay(0, 1);
      printNumberToDisplay(1, 0);
    } break;

    default:
      break;
  }
}

void printNumberToDisplay(byte display, byte number) {
  digitalWrite(LATCH_DIO, LOW);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_MAP[number]);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_SELECT[display]);
  digitalWrite(LATCH_DIO, HIGH);
}

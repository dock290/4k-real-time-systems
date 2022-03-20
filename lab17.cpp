// C++ code

#include <Servo.h>
#include <IRremote.h>
#include <Adafruit_NeoPixel.h>

// Buttons
const double POWER_BUTTON = 16580863;
const double FUNC_BUTTON = 16597183;
const double ONE_BUTTON = 16582903;
const double FOUR_BUTTON = 16591063;
const double TWO_BUTTON = 16615543;
const double FIVE_BUTTON = 16623703;

// Ports
const int IR_SENSOR = 11;
const int RING_LEFT = 3;
const int RING_RIGHT = 2;
const int SERVO_LEFT = 10;
const int SERVO_RIGHT = 9;
const int POTENZ = A0;

// Hertz
const int HERTZ_1 = 1000;

// Mode
const int MODE_WAIT = 0;
const int MODE_ONE = 1;
const int MODE_TWO = 2;
int mode = 0;

// Servos
Servo servoLeft;
Servo servoRight;

// Rings
Adafruit_NeoPixel ringLeft  = Adafruit_NeoPixel(16, RING_LEFT,  NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel ringRight = Adafruit_NeoPixel(16, RING_RIGHT, NEO_RGB + NEO_KHZ800);

// IR sensor receiver
IRrecv irrecv(IR_SENSOR);
double inputButtonCode = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(IR_SENSOR, INPUT);
  irrecv.enableIRIn();
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(RING_LEFT, OUTPUT);
  pinMode(RING_RIGHT, OUTPUT);
  
  servoLeft.attach(SERVO_LEFT);
  servoRight.attach(SERVO_RIGHT);
  
  ringLeft.begin();
  ringRight.begin();
}

void handleModeWait() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(HERTZ_1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(HERTZ_1);
}

void handleModeOne() {
  // 0 - 1023
  int potenzRotateValue = analogRead(POTENZ);
  
  int servoRotation = map(potenzRotateValue, 0, 1023, 0, 180);
  servoLeft.write(servoRotation);
  servoRight.write(servoRotation);
  
  int ringRotation = map(potenzRotateValue, 0, 1023, 0, 15);
  ringLeft.clear();
  ringLeft.setPixelColor(ringRotation, 255, 0, 0);
  ringLeft.show();
  ringRight.clear();
  ringRight.setPixelColor(ringRotation, 255, 0, 0);
  ringRight.show();
  
  delay(500);
}

const int ROTATION_STEP = 10;

int servoLeftRotation = 0;
int servoRightRotation = 0;

void handleModeTwo() {
  if (inputButtonCode != 0) {
    if (inputButtonCode == ONE_BUTTON) {
      servoLeftRotation += ROTATION_STEP;
    } else if (inputButtonCode == TWO_BUTTON) {
      servoLeftRotation -= ROTATION_STEP;
    } else if (inputButtonCode == FOUR_BUTTON) {
      servoRightRotation += ROTATION_STEP;
    } else if (inputButtonCode == FIVE_BUTTON) {
      servoRightRotation -= ROTATION_STEP;
    }
  }
  
  if (servoLeftRotation < 0) {
    servoLeftRotation = 0;
  }
  if (servoLeftRotation > 180) {
    servoLeftRotation = 180;
  }
  
  if (servoRightRotation < 0) {
    servoRightRotation = 0;
  }
  if (servoRightRotation > 180) {
    servoRightRotation = 180;
  }
  
  servoLeft.write(servoLeftRotation);
  servoRight.write(servoRightRotation);
  
  int ringLeftRotation = map(servoLeftRotation, 0, 180, 0, 15);
  ringLeft.clear();
  ringLeft.setPixelColor(ringLeftRotation, 0, 0, 255);
  ringLeft.show();
  
  int ringRightRotation = map(servoRightRotation, 0, 180, 0, 15);
  ringRight.clear();
  ringRight.setPixelColor(ringRightRotation, 0, 0, 255);
  ringRight.show();
  
  delay(500);
}

void loop() {
  decode_results inputButtonRes;
  if (irrecv.decode(&inputButtonRes)) {
    inputButtonCode = inputButtonRes.value;
    Serial.print("Button code:");
    Serial.println(inputButtonCode);
    if (inputButtonCode == POWER_BUTTON) {
      mode = (mode & MODE_ONE) ? MODE_WAIT : MODE_ONE;
    } else if (inputButtonCode == FUNC_BUTTON) {
      mode = (mode & MODE_TWO) ? MODE_WAIT : MODE_TWO;
    }
    irrecv.resume();
  } else {
    inputButtonCode = 0;
  }
  
  switch (mode) {
   case MODE_WAIT:
      handleModeWait();
      break;
   case MODE_ONE:
      handleModeOne();
      break;
   case MODE_TWO:
      handleModeTwo();
      break;
  }
}

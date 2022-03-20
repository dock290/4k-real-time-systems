// C++ code


// 2, 3, 4, 5, 6, 7, 8
const int DIGITS_CODE[][7] = {
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},   // 0
  {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},       // 1
  {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH},    // 2
  {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH},    // 3
  {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},     // 4
  {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH},    // 5
  {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH},   // 6
  {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW},      // 7
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH},  // 8
  {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH},   // 9
};

void displayDigit(int digit){
  if (digit < 0 || digit > 9) {
    return;
  }
  for (int i = 0; i < 7; i++) {
    digitalWrite(i + 2, DIGITS_CODE[digit][i]);
  }
}

void resetDisplay() {
  for (int i = 2; i <= 8; i++) {
  	digitalWrite(i, LOW);
  }
}

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}

void displayTeamMember(int number, int delayMs) {
  int first = (number / 10) % 10;
  int second = number % 10;
  
  resetDisplay();
  displayDigit(first);
  delay(delayMs);
  resetDisplay();
  displayDigit(second);
  delay(delayMs);
}

const int DELAY_MS = 1000;

void loop() {
  resetDisplay();
  delay(1000);
  
  displayTeamMember(5, DELAY_MS);
  displayTeamMember(8, DELAY_MS);
  displayTeamMember(10, DELAY_MS);
}

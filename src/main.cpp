#include <Arduino.h>
#include <Motor.h>
int motor1_A = PB9;
int motor1_B = PB8;
int motor2_A = PB7;
int motor2_B = PB6;

int trigger = PB10;
int echo = PB11;

int isBlackF = PA8;
int isBlackB = PB12;
bool status = false;
bool bottleFind = false;
unsigned long timer = 0;
unsigned long resetSide = 0;

Motor motorA(motor1_A, motor1_B);
Motor motorB(motor2_A, motor2_B);
Car lilFord = {&motorA, &motorB};
void blackFront();
void blackBack();
void flip(bool side);
int distance();
void startSteps(bool side);
void serialEvent() {
  if (Serial.available() > 0) {
    char opc = Serial.read();
    switch (opc) {
    case '0':
      Serial.println("OFF");
      status = false;
      lilFord.stop();
      break;
    case '1':
      status = true;
      Serial.println("Right");
      startSteps(true);
      break;
    case '2':
      status = true;
      Serial.println("Left");
      startSteps(true);
      break;
    default:
      Serial.println("0.OFF\n1.Right\n2.Left");
      Serial.println(distance());
      break;
    }
  }
}

int distance() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  int duration = pulseIn(echo, HIGH);
  int distanceRadar = (duration * 0.034) / 2;
  return distanceRadar;
}

void setup() {
  Serial.begin(9600);
  pinMode(PC13, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(isBlackF, INPUT);
  pinMode(isBlackB, INPUT);
  attachInterrupt(digitalPinToInterrupt(isBlackF), blackFront, HIGH);
  attachInterrupt(digitalPinToInterrupt(isBlackB), blackBack, HIGH);
}

void loop() {
  if (status) {
    int bottle = distance();
    if (bottle <= 40) {
      Serial.print("Loop\nBotella detectada a ");
      Serial.print(bottle);
      Serial.println("cm");
      timer = millis();
      bottleFind = true;
    } else if (bottle > 40 && !bottleFind) {
      lilFord.aroundTheWorld();
      if (millis() - resetSide >= 10000) {
        lilFord.stop();
        lilFord.go();
        delay(200);
        resetSide = millis();
      }
    }
    while (bottleFind) {
      lilFord.go();
      if (millis() - timer >= 5000) {
        timer = millis();
        bottleFind = false;
      }
    }
  }
}
void blackFront() { flip(true); }
void blackBack() { flip(false); }

void startSteps(bool side) {
  lilFord.go();
  delay(400);
  if (side) {
    lilFord.left();
  } else {
    lilFord.right();
  }
  delay(80);
  lilFord.go();
  delay(150);
  lilFord.aroundTheWorld();
}

void flip(bool side) {
  if (status) {
    if (side) { // Linea negra de frente
      lilFord.back();
      delay(300);
    } else {
      lilFord.go();
      delay(100);
      lilFord.aroundTheWorld();
      delay(100);
    }
    for (int i = 0; i < 2; i++) {
      int bottle = distance();
      lilFord.spin(300); // Gira por 300ms
      lilFord.stop();
      if (bottle <= 40) {
        bottleFind = true;
        Serial.print("INT0\nBotella detectada a ");
        Serial.print(bottle);
        Serial.println("cm");
        lilFord.go();
        i = 2;
      }
    }
  }
}
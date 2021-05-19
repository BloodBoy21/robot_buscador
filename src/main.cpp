#include <Arduino.h>
#include <Motor.h>
#include <NewPing.h>

int radar;
int motor1_A = PB9;
int motor1_B = PB8;
int motor2_A = PB7;
int motor2_B = PB6;

int trigger = PB11;
int echo = PB10;

int isBlackF = PA8;
int isBlackB = PB12;
bool change = true;
bool status = false;

unsigned long timer = 0;

NewPing sonar(trigger, echo, 200);
Motor motorA(motor1_A, motor1_B);
Motor motorB(motor2_A, motor2_B);
Car lilFord = {&motorA, &motorB};

bool searching();
void blackFront();
void blackBack();
void flip(bool side);

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
      Serial.println("ON");
      status = true;
      lilFord.go();
      break;
    default:
      Serial.println("0.OFF\n1.ON");
      break;
    }
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(PC13, OUTPUT);
  pinMode(isBlackF, INPUT_PULLDOWN);
  pinMode(isBlackB, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(isBlackF), blackFront, RISING);
  attachInterrupt(digitalPinToInterrupt(isBlackB), blackBack, RISING);
}

void loop() {
  if (!searching() && status) {
    if (change) {
      lilFord.go();
    } else {
      lilFord.back();
    }
    if (millis() - timer >= 30000) {
      lilFord.stop();
      timer = millis();
      change = !change;
    }
  }
}
void blackFront() { flip(true); }
void blackBack() { flip(false); }

bool searching() {
  radar = sonar.ping_cm();
  if (radar <= 99 && status) {
    lilFord.go();
    Serial.println(radar);
    return true;
  }
  return false;
}
void flip(bool side) {
  if (status) {
    if (side) { // Linea negra de frente
      lilFord.back();
    } else {
      lilFord.go();
    }
    delay(2000);
    lilFord.spin(500);
    for (int i = 0; i < 10; i++) {
      lilFord.spin(500); // Gira por 500ms
      lilFord.go();
      delay(400);
    }
  }
}
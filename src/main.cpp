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
int maxDistance = 50;
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
void debugDistance();

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
void debugDistance() {
  int mamada = distance();
  Serial.println(mamada);
  delay(1000);
}
int distance() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
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
  attachInterrupt(digitalPinToInterrupt(isBlackF), blackFront, RISING);
  attachInterrupt(digitalPinToInterrupt(isBlackB), blackBack, RISING);
}

void loop() {
  if (status) {
    int bottle = distance();
    if (bottle <= maxDistance) {
      Serial.print("Loop\nBotella detectada a ");
      Serial.print(bottle);
      Serial.println("cm");
      timer = millis();
      bottleFind = true;
      lilFord.go();
    } else if (bottle > maxDistance && !bottleFind) {
      lilFord.aroundTheWorld();
      delay(50);
      if (millis() - resetSide >= 10000) {
        Serial.println("Hay que seguir");
        lilFord.stop();
        lilFord.go();
        delay(200);
        resetSide = millis();
      }
    }
    while (bottleFind) {
      lilFord.go();
      if ((millis() - timer >= 5000) && distance() > maxDistance) {
        Serial.println("Falsa alarma");
        timer = millis();
        bottleFind = false;
      }
    }
  } else {
    debugDistance();
  }
}
void blackFront() {
  if (status) {
    lilFord.back();
    delay(100);
    flip(true);
  }
}
void blackBack() {
  if (status) {
    lilFord.go();
    delay(100);
    flip(false);
  }
}

void startSteps(bool side) {
  int bottle = distance();
  lilFord.go();
  if (bottle <= maxDistance) {
    bottleFind = true;
  } else {
    delay(400);
    if (side) {
      lilFord.left();
    } else {
      lilFord.right();
    }
    delay(85);
    lilFord.go();
    delay(150);
    lilFord.aroundTheWorld();
  }
}
/*
 *@param bool side: Determinar que sensor fue activado
 *
 */
void flip(bool side) {
  if (status) {
    if (side) { // Linea negra de frente
      Serial.println("Linea enfrente");
      lilFord.back();
      delay(200);
    } else {
      Serial.println("Linea detras");
      lilFord.go();
      delay(80);
      lilFord.aroundTheWorld();
      delay(90);
    }
    if (bottleFind) {
      bottleFind = false;
      lilFord.spin(200);
      lilFord.go();
    }
    for (int i = 0; i < 2; i++) { //! Seguir estudiando el tiempo de giro
      int bottle = distance();
      if (bottle <= maxDistance) {
        bottleFind = true;
        Serial.print("INT0\nBotella detectada a ");
        Serial.print(bottle);
        Serial.println("cm");
        lilFord.go();
        i = 2;
      } else {
        Serial.println("INT0\nGirando");
        lilFord.spin(180); // Gira por 300ms
        lilFord.stop();
      }
    }
  }
}
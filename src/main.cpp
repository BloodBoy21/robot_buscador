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
int maxDistance = 80;
bool status = false;
bool bottleFind = false;
unsigned long resetSide = 0;

Motor motorA(motor1_A, motor1_B);
Motor motorB(motor2_A, motor2_B);
Car lilFord = {&motorA, &motorB};

int distance();
void blackFront();
void blackBack();
void flip(bool side);
void startSteps(bool side);
void debugDistance();
void aroundTheWorld(int time);
bool radar();

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
  int bottleDistance = distance();
  Serial.println(bottleDistance);
  delay(1000);
}

void aroundTheWorld(int time) {
  for (int i = 1; i <= time; i++) {
    lilFord.aroundTheWorld(1);
    if (radar()) {
      break;
    }
  }
}

bool radar() {
  int bottleDistance = distance();
  if (bottleDistance <= maxDistance) {
    Serial.print("Loop\nBotella detectada a ");
    Serial.print(bottleDistance);
    Serial.println("cm");
    resetSide = millis();
    bottleFind = true;
    lilFord.go();
    return true;
  }
  return false;
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
    bool checkRadar = radar();
    if (!checkRadar && !bottleFind) {
      aroundTheWorld(200);
      lilFord.go();
      delay(300);
      radar();
      if (millis() - resetSide >= 1000) { //*Si girando no encuentra nada avanza
        Serial.println("Hay que seguir");
        lilFord.right();
        delay(200);
        lilFord.go();
        delay(1500);
        resetSide = millis();
      }
    }
    while (bottleFind) { //*Si encuentra un objeto cercano
      if (!checkRadar) {
        Serial.println("No hay objeto de frente");
        bottleFind = false;
        break;
      }
      lilFord.go();
    }
  } else {
    debugDistance();
  }
}
void blackFront() { flip(true); }
void blackBack() { flip(false); }

void startSteps(bool side) {
  int bottle = radar();
  if (!bottle) {
    if (side) {
      lilFord.right();
      delay(350);
      radar();
    } else {
      lilFord.left();
      delay(350);
      radar();
    }
    lilFord.go();
    delay(400);
  }
}
/*
 *@param bool side: Determinar que sensor fue activado
 *
 */
void flip(bool side) {
  if (status) {
    if (side) { //*Linea negra de frente
      Serial.println("Linea enfrente");
      lilFord.back();
      delay(300);
      if (bottleFind) {
        lilFord.left();
        delay(250); //? 200 funciona bien
      }
    } else {
      Serial.println("Linea detras");
      lilFord.go();
      delay(300);
      if (bottleFind) {
        lilFord.right();
        delay(250); //? 200 funciona bien
      }
    }
    if (!bottleFind) {
      for (int i = 0; i < 4; i++) { //! Seguir estudiando el tiempo de giro
        int checkRadar = radar();
        if (checkRadar) {
          Serial.println("INT0 Botella encontrada");
          break;
        } else {
          Serial.println("INT0\nGirando");
          lilFord.spin(200);
        }
      }
      lilFord.go();
    }
    bottleFind = false;
  }
}
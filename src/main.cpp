#include <Arduino.h>
#include <Motor.h>
#define maxDistance 85
#define timeToSwipe 500
#define IRFRONTAL !digitalRead(isBlackF1) && !digitalRead(isBlackF2)

int motor1_A = PB9;
int motor1_B = PB8;
int motor2_A = PB7;
int motor2_B = PB6;

int trigger = PB10;
int echo = PB11;

int isBlackF1 = PA8;
int isBlackF2 = PA12;
int isBlackB = PB12;

int bottleDistance = 0;
bool status = false;
bool bottleFind = false;
unsigned long resetSide = 0;
unsigned long resetGo = 0;

Motor motorA(motor1_A, motor1_B);
Motor motorB(motor2_A, motor2_B);
Car lilFord = {&motorA, &motorB};

int distance();
bool radar(int);
void flip(bool);
void blackBack();
void blackFront();
void debugDistance();
void startSteps(bool);
void aroundTheWorld(int, int);

/**
 * @brief Funcion para leer los comandos enviados por bluetooth
 */
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
      startSteps(false);
      break;
    default:
      Serial.println("0.OFF\n1.Right\n2.Left");
      break;
    }
  }
}
/**
 * @brief Funcion para debugear el sensor de distancia
 *
 */
void debugDistance() {
  bottleDistance = distance();
  Serial.println(bottleDistance);
  delay(1000);
}
/**
 * @brief Funcion para girar y verificar si la botella esta cerca
 * @param time tiempo de giro
 * @param radarDistance Rango de medicion en cm
 */
void aroundTheWorld(int time, int radarDistance = maxDistance) {
  for (int i = 1; i <= time; i++) {
    bool checkRadar = radar(radarDistance);
    lilFord.aroundTheWorld(1);
    if (checkRadar) {
      break;
    }
  }
}
/**
 * @brief
 * Funcion para aplicar paramtros de reseteo cuando detecta una botella
 * @param awayBottle Distancia a la que se encotro la botella
 */
void radarData(int awayBottle) {
  Serial.print("Loop\nBotella detectada a ");
  Serial.print(awayBottle);
  Serial.println("cm");
  resetSide = millis();
  resetGo = millis();
  bottleFind = true;
  lilFord.go();
}
/**
 * @brief Funcion para leer la distancia de la botella
 *
 * @param radarDistance Rango de medicion en cm
 * @return true Si la botella esta en el rango de medicion
 * @return false Si la botella no esta en el rango de medicion
 */
bool radar(int radarDistance = maxDistance) {
  bottleDistance = distance();
  if (bottleDistance <= maxDistance) {
    radarData(bottleDistance);
    return true;
  }
  return false;
}
/**
 * @brief Funcion para leer el sensor de distancia
 *
 * @return int
 * Lectura del sensor en cm
 */
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
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(isBlackF1, INPUT);
  pinMode(isBlackF2, INPUT);
  pinMode(isBlackB, INPUT);
  attachInterrupt(digitalPinToInterrupt(isBlackF1), blackFront, RISING);
  attachInterrupt(digitalPinToInterrupt(isBlackF2), blackFront, RISING);
  attachInterrupt(digitalPinToInterrupt(isBlackB), blackBack, RISING);
}

void loop() {
  if (status) {
    bool checkRadar = radar(); //*Lee la distancia
    if (!checkRadar && !bottleFind) {
      bottleFind = false;
      aroundTheWorld(150);
      lilFord.go();
      delay(300);
      radar();
    }
    if (bottleFind) { //*Si encuentra un objeto cercano
      lilFord.go();
    }
  } else {
    debugDistance();
  }
}
void blackFront() { flip(true); }
void blackBack() { flip(false); }

void startSteps(bool side) {
  int bottle = radar(100);
  if (!bottle) {
    if (side) {
      lilFord.right();
    } else {
      lilFord.left();
    }
    delay(650);
    lilFord.stop();
    radar(100);
    lilFord.go();
    delay(600);
  }
}
/**
 * @brief Funcion para cuando una linea es detectada
 *
 * @param side Sensor que fue activado
 */
void flip(bool side) {
  if (status) {
    bottleFind = false; //*Reseteamos el estado de avanzar
    if (side) {         //*Linea negra de frente
      Serial.println("Linea enfrente");
      lilFord.back();
      delay(timeToSwipe);
    } else {
      Serial.println("Linea detras");
      lilFord.go();
      delay(timeToSwipe);
    }
    aroundTheWorld(100);
    for (int i = 0; i < 10; i++) { //! Seguir estudiando el tiempo de giro
      if (radar(15)) {
        break;
      } else {
        aroundTheWorld(10, 20);
      }
    }
  }
}

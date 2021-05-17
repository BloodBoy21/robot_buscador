#include <Arduino.h>
#include <Motor.h>

Motor::Motor(int sideA, int sideB) {
  a = sideA, b = sideB;
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
}
void Motor::go() {
  digitalWrite(a, 1);
  digitalWrite(b, 0);
}
void Motor::back() {
  digitalWrite(a, 0);
  digitalWrite(b, 1);
}
void Motor::stop() {
  digitalWrite(a, 0);
  digitalWrite(b, 0);
}

#ifndef __MOTOR
#define __MOTOR
#include <Arduino.h>
class Motor {
 private:
  int a, b;

 public:
  Motor(int, int);
  void go();
  void back();
  void stop();
};
struct Car {
  Motor *wheelA;
  Motor *wheelB;
  void stop() {
    Serial.println("Car Stop");
    wheelA->stop();
    wheelB->stop();
  }
  void go() {
    wheelA->go();
    wheelB->go();
  }
  void back() {
    wheelA->back();
    wheelB->back();
  }
  void spin(int time) {
    wheelB->stop();
    wheelA->go();
    delay(time);
    wheelA->stop();
    delay(100);
    back();
  }
  void aroundTheWorld() {
    wheelA->go();
    wheelB->back();
  }
  void right() {
    wheelB->stop();
    wheelA->go();
  }
  void left() {
    wheelA->stop();
    wheelB->go();
  }
};
#endif
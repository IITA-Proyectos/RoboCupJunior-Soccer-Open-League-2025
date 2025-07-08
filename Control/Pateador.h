#ifndef PATEADOR_H
#define PATEADOR_H

#include <Servo.h>

class Pateador {
  private:
    Servo servo;
    int pin;

  public:
    Pateador(int pinServo);
    void patear();
};

#endif
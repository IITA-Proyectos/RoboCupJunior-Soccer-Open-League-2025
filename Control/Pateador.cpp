#include "Pateador.h"
#include <Arduino.h>

Pateador::Pateador(int pinServo) {
    pin = pinServo;
    servo.attach(pin);
}

void Pateador::patear() {
    for (int angulo = 0; angulo <= 180; angulo += 5) {
        servo.write(angulo);
        delay(100);
    }

    for (int angulo = 180; angulo >= 0; angulo -= 5) {
        servo.write(angulo);
        delay(100);
    }
}

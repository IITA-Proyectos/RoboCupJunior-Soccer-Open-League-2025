#include <Arduino.h>
#include <zirconLib.h>
unsigned long TiempoAhora = 0;
int periodo = 9000
int periodo1 = 47

void setup(void){

  InitializeZircon();
  Serial.begin(9600);

}


void loop(void)
{      
  motor1(100, 1);
  motor2(100, 1);
  motor3(100, 1);
  if(millis() > TiempoAhora + periodo1) {
    TiempoAhora = millis();
  }
  motor1(0, 1);
  motor2(0, 1);
  motor3(0, 1);
    if(millis() > TiempoAhora + periodo) {
    TiempoAhora = millis();
  }
}

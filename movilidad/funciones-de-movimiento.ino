#include <Arduino.h>
#include <zirconLib.h>
int sentido=1;
float tiempo;
float asda
float mover (float centrimetros, int eje) {
  float tiempo = centimetros  s *1000 / 59,7
  if (eje==3){
  motor1(100, 0);
  motor2(100, 1);
  delay(tiempo);
  motor1(0, 0);
  motor2(0, 1);
  }
  if(eje==1){
    motor3(100, 0);
    motor2(100, 1);
    delay(tiempo);
    motor3(0, 0);
    motor2(0, 1);
  }
  if(eje==2){
    motor3(100, 0);
    motor1(100, 1);
    delay(tiempo);
    motor3(0, 0);
    motor1(0, 1);
  }
  return tiempo;
}

void setup(void)
{
  InitializeZircon();
}
void loop(void)
{  
mover(80,eje3);
 
}

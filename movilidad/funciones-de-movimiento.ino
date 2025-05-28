#include <Arduino.h>
#include <zirconLib.h>
int sentido=1;
int eje; 
int mover1 (int eje){
  motor1(100,1);
  motor2(100,1);
  delay(5000);
  motor1(0,0);
  motor2(0,0);
}
int mover2 (int eje){
  motor1(100,1);
  motor3(100,1);
  delay(5000);
  motor1(0,0);
  motor3(0,0);
}
int mover3 (int eje){
  motor2(100,1);
  motor3(100,1);
  delay(5000);
  motor2(0,0);
  motor3(0,0);
}
float angulo;


float girar (float angulo) {
  
  
  float tiempo = angulo * 0.075 / 90;
  
  motor1(100, 1);
  motor2(100, 1);
  motor3(100, 1);
  delay(tiempo);
  motor1(0, 1);
  motor2(0,1);
  motor3(0,1);
  delay(2000);
  return tiempo;
}
void setup(void)
{
  Serial.begin(9600);
  InitializeZircon();

}

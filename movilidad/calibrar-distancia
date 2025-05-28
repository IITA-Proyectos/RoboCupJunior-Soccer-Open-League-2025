#include <Arduino.h>
#include <zirconLib.h>
int sentido=1;
float tiempo;
//la posicion de la pelota el angulo 
//ahora el motor 2 es el motor 3

float girar (float tiempo) {
  
  motor1(100, 0);
  motor3(100, 1); // es motor 2
  delay(tiempo);
  motor1(0, 0);
  motor3(0,1);
  delay(2000);
  return tiempo;
}
void setup(void)
{
  Serial.begin(9600);
  InitializeZircon();
}
void loop(void)
{  
 // condiciones para saber para que lugar se mueve el robot 
 tiempo=250;
 girar(tiempo);   
 
}

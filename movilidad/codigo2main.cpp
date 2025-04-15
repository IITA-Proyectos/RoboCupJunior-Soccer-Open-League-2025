#include <Arduino.h>
#include <zirconLib.h>


void setup(void)
{
  Serial.begin(9600);

  InitializeZircon();

}


void loop(void)
{   
    //Definiciom de variables
    int v1;     //Potencia del motor 1
    v1=100; //velocidad
    //v1=100 que es la potencia 
    for(v1=255; v1>=0; v1=v1-10){ //esto si funciona 
      Serial.print("v1: ");
      Serial.println(v1);
      motor1(v1, 1);
      delay(1000);
    }
    
}

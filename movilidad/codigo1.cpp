#include <Arduino.h>
#include <zirconLib.h>


void setup(void)
{

  InitializeZircon();

}


void loop(void)
{   
    //Definiciom de variables
    int v1;     //Potencia del motor 1
    int v2;    //potencia del motor 2
    int v3;      //potencia del motor 3
    v1 = 100;
    v2 = 100,
    v3 = 100,
    // iniciar el movimiento
    motor1(100, 0);
    motor2(100, 1);
    //disminuir movimiento
    for (v1 = 100; v1 > 0; v1=v1-20){


   }
    for (v2 = 100; v2 > 0; v2=v1-20){

    }
    // mantenerlo por 2 segundos
    delay(2000);
    
    // detener el movimiento
    motor1(0, 0);
    motor2(0, 1);
    //mantenerlo quito por 5sg
    delay(5000);
    //retroseder
    motor1(100, 1);
    motor2(100, 0);
    //disminuye velocida
    for (v1 = 100; v1 > 0; v1=v1-20){

        
    }
     for (v2 = 100; v2 > 0; v2=v1-20){
 
     }
    //mantiene retroceso por 2 sg
    delay(2000);
    //detiene 
    motor1(0, 1);
    motor2(0, 0);

    delay(2000);

}

#include <Arduino.h>
#include "MPU9250.h"

MPU9250 mpu;


#define INA1 2
#define INB1 5
#define PWM1 3

#define INA2 8
#define INB2 7
#define PWM2 6

#define INA3 11
#define INB3 12
#define PWM3 4
#define START_BYTE 0xAA

float correccion;
float error = 0;
float initialYaw = 72;
float currentYaw;
float kp = 0.4;
long cronometro;
int izqder;
int lado;
int last_Yaw = 10;

void setup(void) {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
        Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
        delay(5000);
    }
  }
  delay(1000);
  mpu.update();
  currentYaw = mpu.getYaw();
  Serial.print ("X Inicial: ");
  Serial.println(currentYaw);
  while(initialYaw - currentYaw > 2	){
	  if (mpu.update()) {
		currentYaw = mpu.getYaw();
	}
  }
  initialYaw = currentYaw;
  cronometro = millis();
  izqder=1;
  lado= 1;
}

//funciones
void ad() {
  digitalWrite(INA3, 0);
  digitalWrite(INB3, 1);
  analogWrite(PWM3, 0);

  digitalWrite(INA2, 1);
  digitalWrite(INB2, 0);
  analogWrite(PWM2, 0);

  digitalWrite(INA1, 0);
  digitalWrite(INB1, 1);
  analogWrite(PWM1, 99);
}
// FUNCION AVANCE DERRCHO PROPORCIONAL 
void adproporcional() {
  if (error>-2 && error <-2){
  digitalWrite(INA3, 0);
  digitalWrite(INB3, 1);
  analogWrite(PWM3, 50);//60

  digitalWrite(INA2, 0);
  digitalWrite(INB2, 1);
  analogWrite(PWM2, 50);//60

  digitalWrite(INA1, 1);
  digitalWrite(INB1, 0);
  analogWrite(PWM1, 89);//99
  }
  else if (error > 0){ 
   //motor izquierdo
   digitalWrite(INA3, 0);
   digitalWrite(INB3, 1);
   analogWrite(PWM3, 50 );//60
   //motor derecho
   digitalWrite(INA2, 0);
   digitalWrite(INB2, 1);
   analogWrite(PWM2, 50 );//60
   //motor atras
   digitalWrite(INA1, 1);
   digitalWrite(INB1, 0);
   analogWrite(PWM1, 40);//50
  }
  else if (error < 0){
   //motor izquierdo
   digitalWrite(INA3, 0);
   digitalWrite(INB3, 1);
   analogWrite(PWM3, 65 );//75
   //motor derecho
   digitalWrite(INA2, 0);
   digitalWrite(INB2, 1);
   analogWrite(PWM2, 40 );//50
   //motor atras
   digitalWrite(INA1, 1);
   digitalWrite(INB1, 0);
   analogWrite(PWM1, 100);//120
  }
}
  //FUNCION AVANCE IZQUIERDO 
void aiproporcional() {
  if (error>-2 && error <-2){
  digitalWrite(INA3, 1);
  digitalWrite(INB3, 0);
  analogWrite(PWM3, 50);//60

  digitalWrite(INA2, 1);
  digitalWrite(INB2, 0);
  analogWrite(PWM2, 50);//60

  digitalWrite(INA1, 0);
  digitalWrite(INB1, 1);
  analogWrite(PWM1, 89);//99
  }
  else if (error > 0){ 
   //motor izquierdo
   digitalWrite(INA3, 1);
   digitalWrite(INB3, 0);
   analogWrite(PWM3, 50 );//60
   //motor derecho
   digitalWrite(INA2, 1);
   digitalWrite(INB2, 0);
   analogWrite(PWM2, 50 );//60
   //motor atras
   digitalWrite(INA1, 0);
   digitalWrite(INB1, 1);
   analogWrite(PWM1, 100);//120
  }
  else if (error < 0){
   //motor izquierdo
   digitalWrite(INA3, 1);
   digitalWrite(INB3, 0);
   analogWrite(PWM3, 65 );//75
   //motor derecho
   digitalWrite(INA2, 1);
   digitalWrite(INB2, 0);
   analogWrite(PWM2, 40);//50
   //motor atras
   digitalWrite(INA1, 0);
   digitalWrite(INB1, 1);
   analogWrite(PWM1, 40);//50
  }
}
void izquierda(){
  digitalWrite(INA3, 1);
  digitalWrite(INB3, 0);
  analogWrite(PWM3, 50);//60

  digitalWrite(INA2, 1);
  digitalWrite(INB2, 0);
  analogWrite(PWM2, 50);//60

  digitalWrite(INA1, 0);
  digitalWrite(INB1, 1);
  analogWrite(PWM1, 89);//99
}

void derecho(){
  digitalWrite(INA3, 0);
  digitalWrite(INB3, 1);
  analogWrite(PWM3, 50);//60

  digitalWrite(INA2, 0);
  digitalWrite(INB2, 1);
  analogWrite(PWM2, 50);//60

  digitalWrite(INA1, 1);
  digitalWrite(INB1, 0);
  analogWrite(PWM1, 89);//99
}


void loop(void) 
{  
  if (mpu.update()) {
        currentYaw = mpu.getYaw();
    }
 Serial.print ("Target X: ");
  Serial.print(initialYaw);
  Serial.print ("  current X: ");
  Serial.print(currentYaw);
  // sacamos error y la correccion
  error =  currentYaw - initialYaw;
  Serial.print ("  Error: ");
  Serial.print(error);
  if ( error > 180 ) error = error - 360;
  if (error < -180) error = error + 360;
  if (error>-3 && error < 3){
   error = 0;
   }

  correccion= error * kp;	
  Serial.print ("  Correcion: ");
  Serial.println(correccion);
  if((millis()-cronometro)>2000)
             {
              if(lado == 1){
              lado=0;
              cronometro=millis();
              Serial.println(lado);
              }
              else {
                lado = 1;
                Serial.println(lado);
                cronometro=millis();                
              }
              
             }
   //Serial.println( (millis()-cronometro));
   //Serial.println(lado);
   //Serial.println(correccion);
   if(lado == 1){
    adproporcional();
   }
   if(lado == 0){
    aiproporcional();
   }
}
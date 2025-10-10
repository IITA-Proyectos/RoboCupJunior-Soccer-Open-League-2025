#include <Arduino.h>

// Esta funcion es util por que te permite que no se llene el buffer
// y actualiza siempreel valor de la variable con lo que llega
int ball_state = 0;

void serialEvent1()
{
    int data = Serial1.read();
    Serial.println("Reading serial 1");
    if (data == 97){
      ball_state = 1;
    }  
    else {
      ball_state = 0;
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(19200);
  Serial.begin(115200);
  pinMode(13, OUTPUT);
 }

void loop() {
  // put your main code here, to run repeatedly:
  if (ball_state == 1) {
    digitalWrite(13, HIGH);
  }
  else{
  digitalWrite(13, LOW);
  }
 }
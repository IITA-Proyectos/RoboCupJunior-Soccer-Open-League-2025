int motorizquierdo1=3;
int motorizquierdo2=4;
int motorderecho1=5;
int motorderecho2=6;
void setup() {
  pinMode(motorizquierdo1,OUTPUT);
  pinMode(motorizquierdo2,OUTPUT);
  pinMode(motorderecho1,OUTPUT);
  pinMode(motorderecho2,OUTPUT);
}

void loop() {
digitalWrite(motorizquierdo1,HIGH);
digitalWrite(motorizquierdo2,LOW);
// SI SOLO FUNCIONA UN LADO HARA UN GIRO a la derecha
digitalWrite(motorderecho1,HIGH);
digitalWrite(motorderecho2,LOW);
//giro hacia la izquierda 
digitalWrite(motorizquierdo1,HIGH);
digitalWrite(motorizquierdo2,LOW);
digitalWrite(motorderecho1,HIGH);
digitalWrite(motorderecho2,LOW);
//avanza hacia adelante
}
//FUNCIONES
void girarIzquierda() {
  digitalWrite(motorizquierdo1, HIGH);
  digitalWrite(motorizquierdo2, LOW);
void girarDerecha(){
digitalWrite(motorderecho1,HIGH);
digitalWrite(motorderecho2,LOW);}
void haciaAtras(){
digitalWrite(motorizquierdo1,HIGH);
digitalWrite(motorizquierdo2,LOW);
digitalWrite(motorderecho1,HIGH);
digitalWrite(motorderecho2,LOW);
}
void seguirAdelante(){
digitalWrite(motorizquierdo1,LOW);
digitalWrite(motorizquierdo2,HIGH);
digitalWrite(motorderecho1,LOW);
digitalWrite(motorderecho2,HIGH);
//probar con ambos motores sino cambiar de nombre a la funcion adelante o atras, izquierda derecha
}
}

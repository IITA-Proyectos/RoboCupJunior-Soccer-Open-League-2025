int tipo_dato=-1;

int posicion_x=0;
int posicion_y=0;
int distancia=0;
int angulo=0;

float posicion_x_cm=0;
float posicion_y_cm=0;
float distancia_pelota_cm=0;
float angulo_pelota_grados=0;

void setup(){
  Serial.begin(115200);
  Serial.begin(115200);
}

void loop(){
  Serial.print("Posicion x: ");
  Serial.print(posicion_x_cm);
  Serial.print("cm | ");
  Serial.print("Posicion y: ");
  Serial.print(posicion_y_cm);
  Serial.print("cm | ");
  Serial.print("Distancia: ");
  Serial.print(distancia_pelota_cm);
  Serial.print("cm | ");
  Serial.print("Angulo: ");
  Serial.print(angulo_pelota_grados);
  Serial.print("grados:");
  delay(10000);
}

void serialEvent1(){
  while (Serial1.available()){
    int byte_recibido=Serial1.read();
    Serial.print("Byte recibido: ");
  Serial.println(byte_recibido);
    switch (byte_recibido){
      case 201:
        tipo_dato=1;
        break;
      case 202:
        tipo_dato=2;
        break;
      case 203:
        tipo_dato=3;
        break;
      case 204:
        tipo_dato=4;
        break;
      default:
        if (tipo_dato==0){
          posicion_x= byte_recibido;
          posicion_x_cm= posicion_x/2.0;
        }
        else if (tipo_dato==1){
          posicion_y= byte_recibido;
          posicion_y_cm= posicion_y/2.0-50.0;
        }
        else if (tipo_dato==2){
          distancia= byte_recibido;
          distancia_pelota_cm= distancia*1.0;
        }
        else if (tipo_dato==3){
          angulo= byte_recibido;
          angulo_pelota_grados= angulo-100.0;
        }
        tipo_dato=-1;
        break;
    }
  }
}

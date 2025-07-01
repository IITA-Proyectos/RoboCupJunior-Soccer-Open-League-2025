int serial5state = -1;
int raw_x = 0;
int raw_y = 0;
int raw_ang = 0;
int sentido = 0;
 
float x_cm = 0.0;
float y_cm = 0.0;
float angulo_deg = 0.0;
 
void setup() {
  Serial.begin(115200);   // Monitor Serial
  Serial1.begin(115200);  // UART conectado a OpenMV
}
 
void loop() {
  // Mostrar valores físicos cada 200 ms
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) {
    lastPrint = millis();
 
    Serial.print("x = "); Serial.print(x_cm, 2); Serial.print(" cm");
    Serial.print(" | y = "); Serial.print(y_cm, 2); Serial.print(" cm");
    Serial.print(" | ángulo = "); Serial.print(angulo_deg, 2); Serial.print("°");
    Serial.print(" | sentido = "); Serial.println(sentido);
  }
}

void serialEvent1() {
  while (Serial1.available()) {
    int data = Serial1.read();
 
    // Ver los bytes que llegan (opcional para debug)
    // Serial.print("Byte recibido: "); Serial.println(data);
 
    // Identificar TAGs
    if (data == 201) {
      serial5state = 0;  // x
    } else if (data == 202) {
      serial5state = 1;  // y
    } else if (data == 203) {
      serial5state = 2;  // ángulo
    } else if (data == 204) {
      serial5state = 3;  // sentido
    }
 
    // Procesar datos según TAG
    else {
      if (serial5state == 0) {
        raw_x = data;
        x_cm = raw_x / 2.0;  // Conversión: x = v / 2
      } else if (serial5state == 1) {
        raw_y = data;
        y_cm = raw_y / 2.0 - 50.0;  // Conversión: y = v / 2 - 50
      } else if (serial5state == 2) {
        raw_ang = data;
        angulo_deg = raw_ang - 100.0;  // Conversión: ángulo = v - 100
      } else if (serial5state == 3) {
        sentido = data;
      }
 
      serial5state = -1;  // Reset después de leer un valor
    }
  }
}

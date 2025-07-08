int serial3state = -1;
int raw_x = 0;
int raw_y = 0;
int raw_ang = 0;
int sentido = 0;

float x_cm = 0.0;
float y_cm = 0.0;
float angulo_deg = 0.0;

void setup() {
  Serial.begin(115200);   // Salida al Monitor Serial
  Serial3.begin(115200);  // UART3 conectado a OpenMV (ajusta si usas otro puerto)
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

void serialEvent3() {  // ← Cambiar esto si usás otro puerto (Serial1, Serial2, etc.)
  while (Serial3.available()) {
    int data = Serial3.read();

    if (data == 201) {
      serial3state = 0;  // Esperando byte de X
    } else if (data == 202) {
      serial3state = 1;  // Esperando byte de Y
    } else if (data == 203) {
      serial3state = 2;  // Esperando byte de ángulo
    } else if (data == 204) {
      serial3state = 3;  // Esperando byte de sentido
    } else {
      switch (serial3state) {
        case 0:
          raw_x = data;
          x_cm = raw_x / 2.0;
          break;
        case 1:
          raw_y = data;
          y_cm = (raw_y / 2.0) - 50.0;
          break;
        case 2:
          raw_ang = data;
          angulo_deg = raw_ang - 100.0;
          break;
        case 3:
          sentido = data;
          break;
      }
      serial3state = -1;  // Reiniciar el estado luego de leer el valor
    }
  }
}

#include <Arduino.h>

// Estados para indicar qué dato esperamos
enum EstadoUART {
  ESPERAR_TAG = -1,
  ESPERAR_X = 201,
  ESPERAR_Y = 202,
  ESPERAR_ANG = 203,
  ESPERAR_SENTIDO = 204
};

int estado = ESPERAR_TAG;

// Variables crudas (0–200)
int rawX = 0;
int rawY = 0;
int rawAng = 0;
int rawSentido = 0;

// Variables físicas
float X_cm = 0.0;       // 0–100 cm
float Y_cm = 0.0;       // -50 a +50 cm
float angulo_deg = 0.0; // -100 a +100 grados

void setup() {
  Serial.begin(115200);    // para debug por USB
  Serial1.begin(115200);   // UART1 ← TX de OpenMV
}

void loop() {
  // Imprime cada 200 ms los valores convertidos
  static uint32_t last = millis();
  if (millis() - last >= 200) {
    last = millis();
    Serial.print("X = "); Serial.print(X_cm, 2); Serial.print(" cm | ");
    Serial.print("Y = "); Serial.print(Y_cm, 2); Serial.print(" cm | ");
    Serial.print("Ángulo = "); Serial.print(angulo_deg, 2); Serial.println("°");
  }
}

// Se llama automáticamente cuando Serial1 recibe datos
void serialEvent1() {
  while (Serial1.available()) {
    int b = Serial1.read();
    
    // ¿Es un TAG?
    if (b == 201 || b == 202 || b == 203 || b == 204) {
      estado = b;
      continue;
    }
    
    // Si no es TAG, es valor: procesamos según el estado actual
    switch (estado) {
      case ESPERAR_X:
        rawX = b;
        // conversión inversa de rawX (0–200) → X_cm (0–100 cm)
        X_cm = rawX / 2.0;
        break;
        
      case ESPERAR_Y:
        rawY = b;
        // rawY 0–200 → Y_cm en -50..+50 cm
        Y_cm = rawY / 2.0 - 50.0;
        break;
        
      case ESPERAR_ANG:
        rawAng = b;
        // rawAng 0–200 → -100..+100 grados
        angulo_deg = rawAng - 100.0;
        break;
        
      case ESPERAR_SENTIDO:
        rawSentido = b;
        // rawSentido es 0 o 1 → puedes usarlo según necesites
        // por ejemplo, invertir signo de ángulo si es 0:
        if (rawSentido == 0) angulo_deg = -fabs(angulo_deg);
        else                angulo_deg = fabs(angulo_deg);
        break;
        
      default:
        // Si no estabas esperando nada, ignoramos
        break;
    }
    
    // Después de leer el valor, volvemos a esperar un TAG
    estado = ESPERAR_TAG;
  }
}

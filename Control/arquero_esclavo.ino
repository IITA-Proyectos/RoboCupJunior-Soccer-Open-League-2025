#include <SoftwareSerial.h>

// Configuración del puerto serial para recibir datos desde otro Arduino
SoftwareSerial mySerial(0, 1); // RX, TX

// Función para calcular el desplazamiento lateral
float calcularDesplazamiento(float anguloGrados, float distanciaCm) {
  float anguloRad = radians(anguloGrados);
  return distanciaCm * tan(anguloRad);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Espera a que se abra el monitor serie (opcional)
  Serial.println("Iniciando Receptor (modo arquero activo)...");
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.available() > 0) {
    String mensaje = mySerial.readStringUntil('\n');
    mensaje.trim();

    if (mensaje.length() > 0) {
      procesarMensaje(mensaje);
    }
  }
}

void procesarMensaje(String mensaje) {
  if (mensaje.charAt(0) != '$') return;
  mensaje = mensaje.substring(1);

  // Separación de campos por comas
  int i1 = mensaje.indexOf(',');
  if (i1 == -1) return;
  String tipo = mensaje.substring(0, i1);

  int i2 = mensaje.indexOf(',', i1 + 1);
  int i3 = mensaje.indexOf(',', i2 + 1);
  int i4 = mensaje.indexOf(',', i3 + 1);
  int i5 = mensaje.indexOf(',', i4 + 1);
  if (i2 == -1 || i3 == -1 || i4 == -1 || i5 == -1) return;

  // Extraemos los valores
  String valor1Str = mensaje.substring(i2 + 1, i3);  // radio (distancia)
  String valor2Str = mensaje.substring(i3 + 1, i4);  // theta (ángulo)
  // String valor3Str = mensaje.substring(i4 + 1, i5);  // phi (no usado)
  // String checksum = mensaje.substring(i5 + 1);       // no usado

  if (tipo == "IITATRA") {
    float distancia = valor1Str.toFloat();  // radio
    float angulo = valor2Str.toFloat();     // theta

    float desplazamiento = calcularDesplazamiento(angulo, distancia);

    // Mostrar resultados con formato claro
    Serial.print("Ángulo: ");
    Serial.print(angulo, 2);
    Serial.print("°, Distancia: ");
    Serial.print(distancia, 2);
    Serial.print(" cm → Desplazamiento lateral: ");
    Serial.print(desplazamiento, 2);
    Serial.println(" cm");
  }
}

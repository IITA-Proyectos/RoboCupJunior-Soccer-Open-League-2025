#include <SoftwareSerial.h> // Incluimos la librería SoftwareSerial

// Definimos los pines RX y TX para la comunicación serial
// ¡Importante! El pin RX de este Arduino debe conectarse al pin TX del Arduino emisor.
// El pin TX de este Arduino debe conectarse al pin RX del Arduino emisor.
// Si el Arduino emisor usa pin 2 como RX y pin 3 como TX,
// entonces este Arduino receptor debería usar pin 3 como RX y pin 2 como TX.
SoftwareSerial mySerial(3, 2); // RX, TX. Cambia estos pines si es necesario.

void setup() {
  Serial.begin(9600);    // Inicializa la comunicación con el Monitor Serie (para ver los datos)
  while (!Serial);       // Espera a que el Monitor Serie esté conectado (solo para placas con Serial nativo USB como Leonardo, Micro)

  mySerial.begin(9600);  // Inicializa la comunicación con el puerto SoftwareSerial
                         // Asegúrate de que la velocidad de baudios coincida con la del Arduino emisor
  Serial.println("Arduino Receptor Iniciado.");
  Serial.println("Esperando datos...");
}

void loop() {
  // Comprobamos si hay datos disponibles para leer en el puerto SoftwareSerial
  if (mySerial.available()) {
    // Si hay datos, leemos el byte. Como el emisor envía uint8, lo leemos como int
    // y luego lo imprimimos, ya que println() lo convierte a su representación legible.
    uint8_t receivedByte = mySerial.read();
    
    // Imprimimos el byte recibido en el Monitor Serie
    Serial.print("Dato recibido: ");
    Serial.println(receivedByte); // Imprime el valor numérico
  }
}

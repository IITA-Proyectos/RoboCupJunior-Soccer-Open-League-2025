// --- Parámetros del Atractor de Lorenz ---
float sigma = 10.0;
float rho = 28.0;
float beta = 8.0 / 3.0;

// --- Estado Inicial del Atractor ---
float x = 0.1;
float y = 0.1;
float z = 0.1;

// --- Factor de Tiempo y Frecuencia de Muestreo ---
float timeStep = 0.01; // Este factor controla la velocidad de la simulación
unsigned long sampleRateMillis = 100; // Frecuencia de muestreo en milisegundos (100 ms = 10 Hz)
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600); // Inicializar la comunicación serial
  Serial.println("--- Inicialización del Simulador de Sensor ---");
  Serial.println("Atractor de Lorenz:");
  Serial.print("  sigma = "); Serial.println(sigma);
  Serial.print("  rho = "); Serial.println(rho);
  Serial.print("  beta = "); Serial.println(beta);
  Serial.print("  timeStep = "); Serial.println(timeStep);
  Serial.print("  sampleRate = "); Serial.print(1000.0 / sampleRateMillis); Serial.println(" Hz");
  Serial.println("Estado Inicial: x = 0.1, y = 0.1, z = 0.1");
  Serial.println("--- Formato de Salida (Monitor Serie y RxTx) ---");
  Serial.println("Formato NMEA: $ITTA,<tipo>,<valor1>,<valor2>,<valor3>*CC\\r\\n");
  Serial.println("Donde:");
  Serial.println("  $ITTA: Identificador del sensor simulado.");
  Serial.println("  <tipo>: Tipo de valores ('ITAAOR' para originales x,y,z; 'IITATR' para transformados X,Y,Angulo).");
  Serial.println("  <valor1>, <valor2>, <valor3>: Los valores numéricos.");
  Serial.println("  *CC: Placeholder para el checksum NMEA (ej: *00).");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sampleRateMillis) {
    previousMillis = currentMillis;

    // 1. Calcular el siguiente estado del Atractor de Lorenz
    calcularLorenz();

    // 2. Enviar los valores originales del atractor
    enviarDatos("ITAAOR", x, y, z);

    // 3. Transformar a Vector (X, Y, Ángulo) y Enviar los valores transformados
    transformarYEnviar();
  }
}

void calcularLorenz() {
  float dx = (sigma * (y - x)) * timeStep;
  float dy = (x * (rho - z) - y) * timeStep;
  float dz = (x * y - beta * z) * timeStep;
  x = x + dx;
  y = y + dy;
  z = z + dz;
}

void transformarYEnviar() {
  // --- Ejemplo de Transformación a Vector (X, Y, Ángulo) ---

  // Componente X: Mapeamos la variable 'x' a un rango
  float coordenadaX = map(x * 100, -5000, 5000, -100, 100); // Rango de -100 a 100

  // Componente Y: Mapeamos la variable 'y' a otro rango
  float coordenadaY = map(y * 100, -5000, 5000, -150, 150); // Rango de -150 a 150

  // Ángulo en Radianes: Usamos la variable 'z' y la escalamos con una función seno
  float anguloRadianes = sin(z * 0.5) * PI + PI; // Rango de 0 a 2*PI

  // --- Enviar los valores transformados ---
  enviarDatos("IITATR", coordenadaX, coordenadaY, anguloRadianes);
}

void enviarDatos(String tipo, float val1, float val2, float val3) {
  String nmeaMessage = "$ITTA," + tipo + "," + String(val1, 6) + "," + String(val2, 6) + "," + String(val3, 6) + "*00\r\n";
  // Nota: Usamos String(valor, 6) para tener una precisión de 6 decimales.
  // "*00" es un checksum de ejemplo muy simple (inválido para la mayoría de los analizadores NMEA).

  Serial.print(nmeaMessage); // Mostrar en el monitor serie
  for (int i = 0; i < nmeaMessage.length(); i++) {
    Serial.write(nmeaMessage[i]); // Enviar por los pines RxTx
  }
}

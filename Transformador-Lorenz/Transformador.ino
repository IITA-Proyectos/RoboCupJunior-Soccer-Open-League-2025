// --- Parámetros del Atractor de Lorenz ---
float sigma = 10.0;
float rho = 28.0;
float beta = 8.0 / 3.0;
dfhdchcgh
// --- Estado Inicial del Atractor ---
float t = 0.0;
float x = 0.1;
float y = 0.1;
float z = 0.1;

// --- Centro aproximado del atractor, se ira actualizando iterativamente ---
float cx = 8.485;
float cy = 8.485;
float cz = 27.0;
// pesos para actualizar el centro
const float weight_old = 0.995;
const float weight_new = 1 -weight_old;

// --- Factor de Tiempo y Frecuencia de Muestreo ---
float timeStep = 0.01; // Este factor controla la velocidad de la simulación
unsigned long sampleRateMillis = 40; // Frecuencia de muestreo en milisegundos (40 ms = 25 Hz)
unsigned long previousMillis = 0;

// --- Estructura con las variables de esfericas ---
struct Esfericas {
  float r;      // distancia radial
  float theta;  // ángulo polar en grados
  float phi;    // ángulo azimutal en grados
};

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
  Serial.println("Formato NMEA: $IITAxxx,<tiempo>,<valor1>,<valor2>,<valor3>,CC\\r\\n");
  Serial.println("Donde:");
  Serial.println("  $IITAxxx: Identificador xxx del sensor simulado.");
  Serial.println("  <tiempo>: Tiempo en milisegundos desde que se encendio el sensor.");
  Serial.println("  <tipo>: Tipo de valores ('IITAORI' para originales x,y,z; 'IITATRA' para transformada a esfericas: radio,theta,phi).");
  Serial.println("  <valor1>, <valor2>, <valor3>: Los valores numéricos.");
  Serial.println("  CC: Placeholder para el checksum NMEA (ej: 00).");
}

void loop() {  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sampleRateMillis) {
    previousMillis = currentMillis;

    // 1. Calcular el siguiente estado del Atractor de Lorenz
    calcularLorenz();

    // 2. Enviar los valores originales del atractor
    enviarDatos("IITAORI", x, y, z);

    // 3. Transformar a Vector (X, Y, Ángulo) y Enviar los valores transformados
    // --- Transformación a coordenadas esfericas ---
    Esfericas rTP = calcularEsfericas(x, y, z);

    // --- Enviar los valores transformados ---
    enviarDatos("IITATRA", rTP.r, rTP.theta, rTP.phi);
  }
}

// --- La dinamica del atractor de Lorentz ---
void calcularLorenz() {
  // los incrementos
  float dx = (sigma * (y - x)) * timeStep;
  float dy = (x * (rho - z) - y) * timeStep;
  float dz = (x * y - beta * z) * timeStep;

  // aplicar los incrementos con euler
  x = x + dx;
  y = y + dy;
  z = z + dz;

  // aplico el promedio movil para hacer la actualizacion del centro aproximado
  cx = weight_old * cx + weight_new * x;
  cy = weight_old * cy + weight_new * y;
  cz = weight_old * cz + weight_new * z;

  // guardar tiempo actual "wall clock" del cuando se simulo el dato (no es el tiempo simulado)
  t = millis(); 
}

// --- Funcion que calcula las variables esfericas ---
Esfericas calcularEsfericas(float x, float y, float z) {
  // Vector trasladado al centro del atractor
  float dx = x - cx;
  float dy = y - cy;
  float dz = z - cz;

  // Distancia radial
  float r = sqrt(dx*dx + dy*dy + dz*dz);

  // Evitar división por cero
  if (r == 0) {
    return {0.0, 0.0, 0.0};
  }

  // Ángulo polar (theta): ángulo respecto al eje Z
  float thetaRad = acos(dz / r);
  float thetaDeg = thetaRad * 180.0 / PI;

  // Ángulo azimutal (phi): ángulo en el plano XY respecto al eje X
  float phiRad = atan2(dy, dx);
  float phiDeg = phiRad * 180.0 / PI;
  
  // Asegurar que phi esté en [0, 360)
  if (phiDeg < 0) {
    phiDeg += 360.0;
  }

  return {r, thetaDeg, phiDeg};
}

void enviarDatos(String tipo, float val1, float val2, float val3) {
  String nmeaMessage = "$" + tipo + "," + String(t, 0) + "," + String(val1, 6) + "," + String(val2, 6) + "," + String(val3, 6) + "," + "00\r\n";
  // Nota: Usamos String(valor, 6) para tener una precisión de 6 decimales.
  // "00" es un checksum de ejemplo muy simple (inválido para la mayoría de los analizadores NMEA).
  // TODO implementar un checksum?

  Serial.print(nmeaMessage); // Mostrar en el monitor serie
  for (int i = 0; i < nmeaMessage.length(); i++) {
    Serial.write(nmeaMessage[i]); // Enviar por los pines RxTx // NOTE creo que esto sale tambien por el monitor serie? en mi arduino nano sale
  }
}

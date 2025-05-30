// se implementan dos atractores de lorentz para generar seis numeros con una dinamica
// suave pero caotica. se los imprime por monitor serie 

#include <SoftwareSerial.h>

// instancia de SoftwareSerial
// El primer parámetro es el pin RX, el segundo es el pin TX
// Asegúrate de elegir pines que no sean los de Serial (0 y 1)
// Por ejemplo, para un Arduino UNO, puedes usar 2 para RX y 3 para TX
SoftwareSerial mySerial(15, 14); // Rx en pin 15 y TX en pin 14


// --- Parámetros del Atractor de Lorenz ---
float sigma = 10.0;
float rho = 28.0;
float beta = 8.0 / 3.0;

// --- Estado Inicial del Atractor ---
float x1 = 1.0; // valores iniciales distintos a los dos atractores
float x2 = 2.0;
float y1 = 3.0;
float y2 = 4.0;
float z1 = 30.0;
float z2 = 31.0;

// --- Factor de Tiempo y Frecuencia de Muestreo ---
float timeStep = 0.01; // Este factor controla la velocidad de la simulación
unsigned long sampleRateMillis = 2000; // Frecuencia de muestreo en milisegundos (40 ms = 25 Hz)
unsigned long previousMillis = 0;


void setup() {
  Serial.begin(9600); // Inicializar la comunicación serial
  mySerial.begin(9600);  // Paso 3: Inicializa la comunicación con el puerto SoftwareSerial
  Serial.println("--- Inicialización del Simulador de Sensor ---");
  Serial.println("Atractor de Lorenz:");
}

void loop() {  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sampleRateMillis) {
    previousMillis = currentMillis;

    // 1. Calcular el siguiente estado del Atractor de Lorenz
    calcularLorenz1();
    calcularLorenz2();

    // 2. Enviar por uart
    enviarDatos();
  }
}

// --- La dinamica del atractor de Lorentz 1 ---
void calcularLorenz1() {
  // los incrementos
  float dx = (sigma * (y1 - x1)) * timeStep;
  float dy = (x1 * (rho - z1) - y1) * timeStep;
  float dz = (x1 * y1 - beta * z1) * timeStep;

  // aplicar los incrementos con euler
  x1 = x1 + dx;
  y1 = y1 + dy;
  z1 = z1 + dz;
}

// --- La dinamica del atractor de Lorentz 2 ---
void calcularLorenz2() {
  // los incrementos
  float dx = (sigma * (y2 - x2)) * timeStep;
  float dy = (x2 * (rho - z2) - y2) * timeStep;
  float dz = (x2 * y2 - beta * z2) * timeStep;

  // aplicar los incrementos con euler
  x2 = x2 + dx;
  y2 = y2 + dy;
  z2 = z2 + dz;
}


void enviarDatos() {

    
    enviar(255, 0); // flag de inicio
  
    uint8_t tag, value;
    
    // Transformar a la escala 0-200
    // las coordenadas x,y de lorentz se convierten tal que el intervalo [-30,30] se mapea a [0,200]
    // x -> (x + 30) / 0.3
    // y -> (y + 30) / 0.3
    // la coordenada z se convierte tal que el intervalo [0,60] se mapea a [0,200]
    // z -> z / 0.3


//    TAG  Nombre
//    201 Pelota – coordenada X
    tag = 201;
    value = uint8_t((x1 + 30) / 0.3);
    enviar(tag, value);

//    202 Pelota – coordenada Y
    tag = 202; 
    value = uint8_t((y1 + 30) / 0.3);
    enviar(tag, value);

//    203 Arco enemigo – coordenada X
    tag = 203; 
    value = uint8_t(z1 / 0.3);
    enviar(tag, value);

//    204 Arco enemigo – coordenada Y
    tag = 204; 
    value = uint8_t((x2 + 30) / 0.3);
    enviar(tag, value);

//    205 Arco propio – coordenada X
    tag = 205; 
    value = uint8_t((y2 + 30) / 0.3);
    enviar(tag, value);

//    206 Arco propio – coordenada Y
    tag = 206; 
    value = uint8_t(z2 / 0.3);
    enviar(tag, value);

    
    enviar(254, 0); // flag de fin
}

void enviar(uint8_t tag, uint8_t value){
    Serial.println(tag);
    Serial.println(value);
    mySerial.println(tag);   // Enviar por UART
    mySerial.println(value); // Enviar por UART
  }

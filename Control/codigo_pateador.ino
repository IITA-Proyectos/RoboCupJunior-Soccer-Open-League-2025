#include "Pateador.h"

Pateador miPateador(6);  // Pin del servo

void setup() {
  // Nada que inicializar adicionalmente
}

void loop() {
  miPateador.patear();
  delay(1000);
}

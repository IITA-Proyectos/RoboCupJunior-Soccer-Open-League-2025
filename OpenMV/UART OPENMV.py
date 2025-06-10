from pyb import UART
import time

uart = UART(3, 115200)

# Variables simuladas (fake data)
x_map = 0        # Centro X mapeado (0 a 100)
ang = 0          # Ángulo mapeado (0 a 180)
sentido = 0      # Sentido (0: SV, 1: VI, 2: VD, 3: DV)

while True:
    # Actualizar valores de prueba
    x_map = (x_map + 10) % 101
    ang = (ang + 30) % 181
    sentido = (sentido + 1) % 4

    # Armar el paquete de datos
    packet = [
        255, x_map,       # Marca de x_map
        254, ang,         # Marca de ang
        253, sentido      # Marca de sentido
    ]

    print("Enviando paquete:", packet)       # Mostrar qué se envía
    uart.write(bytearray(packet))            # Enviar por UART

    time.sleep_ms(500)  # Esperar medio segundo entre envíos
from pyb import UART
import sensor, image, time
import math

uart = UART(3, 115200)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_whitebal(False)
clock = time.clock()

h = 10
pi = math.pi
r = 13.5 / (pi * 2)

orange_threshold = (30, 60, 20, 60, 10, 50)

def transformarcoordenadas(u, v):
    H = [[-2.18880937e-03, -2.63497709e-01, -2.44260579e+02],
         [1.02845724e+00,  1.44625829e-03, -1.92956221e+02],
         [4.01073113e-04, -5.44887132e-02,  1.00000000e+00]]
    denominator = H[2][0]*u + H[2][1]*v + H[2][2]
    x = (H[0][0]*u + H[0][1]*v + H[0][2]) / denominator
    y = (H[1][0]*u + H[1][1]*v + H[1][2]) / denominator
    return x, y

while(True):
    clock.tick()
    img = sensor.snapshot()

    blobs = img.find_blobs([orange_threshold], pixels_threshold=100, area_threshold=100, merge=True)

    if blobs:
        largest_blob = max(blobs, key=lambda b: b.pixels())
        img.draw_rectangle(largest_blob.rect(), color=(255, 0, 0))
        img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=(0, 255, 0))

        u, v = largest_blob.cx(), largest_blob.cy()
        x, y = transformarcoordenadas(u, v)
        X = x * (h - r) / h
        Y = y * (h - r) / h

        a_rad = math.atan2(Y, X)
        angulo = math.degrees(a_rad)

        if angulo > 0:
            sentido = 1
        elif angulo < 0:
            sentido = 0
        else:
            sentido = 0  # Por defecto

        print("Coordenadas físicas: X = %.2f cm, Y = %.2f cm" % (X, Y))
        print("Ángulo: %.2f°  | Sentido:", angulo, sentido)

        # ----------------------------
        # ⚠️ Escalar a bytes (valores enteros entre 0–200)
        byteX = min(max(int(X * 2), 0), 200)             # X → 0–100 cm → *2 → 0–200
        byteY = min(max(int((Y + 50) * 2), 0), 200)      # Y → -50 a 50 → +50, *2 → 0–200
        byteAng = min(max(int(angulo + 100), 0), 200)    # Ángulo → -100 a 100 → +100 → 0–200

        # Armar paquete
        packet = [
            201, byteX,
            202, byteY,
            203, byteAng,
            204, sentido
        ]

        print("Enviando:", packet)
        uart.write(bytearray(packet))
    
    time.sleep_ms(500)

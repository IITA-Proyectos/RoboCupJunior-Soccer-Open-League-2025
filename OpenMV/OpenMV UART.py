import sensor, image, time
import math
from pyb import UART

# Inicializar UART3 a 115200 baudios
uart = UART(3, 115200, timeout_char=1000)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_whitebal(False)
clock = time.clock()

orange_threshold = (30, 60, 20, 60, 10, 50)

while(True):
    clock.tick()
    img = sensor.snapshot()
    blobs = img.find_blobs([orange_threshold], pixels_threshold=100, area_threshold=100, merge=True)

    if blobs:
        b = max(blobs, key=lambda b: b.pixels())
        x, y = b.cx(), b.cy()

        # mapear x de 0–320 a 0–100
        x_map = int((100 * x) / 320)

        # calcular ángulo
        if y != 145:
            a_rad = math.atan2((160 - x), (160 - y + 120))
            ang = math.degrees(a_rad)
        else:
            ang = 0.0

        sentido = 1 if ang > 0 else 0  # 1: derecha, 0: izquierda

        # armar y enviar tren de bytes: [255,x_map, 254,ang+90, 253,sentido]
        packet = [255, x_map,
                  254, int(ang + 90),
                  253, sentido]
        uart.write(bytearray(packet))

        img.draw_rectangle(b.rect(), color=(255, 0, 0))
        img.draw_cross(x, y, color=(0, 255, 0))

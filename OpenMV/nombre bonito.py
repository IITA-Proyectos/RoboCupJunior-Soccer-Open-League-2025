import sensor
import image
import time

# Inicializa el sensor de la cámara
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
clock = time.clock()
# Parámetros de color para la pelota (ajustar según el color de tu pelota)
color = (30, 100, 15, 100, 15, 100)  # (min_l, max_l, min_a, max_a, min_b, max_b) para color en HSV
# Define la posición inicial
center_x = 0
center_y = 0

while(True):
    # Captura una imagen
    img = sensor.snapshot()

    # Detecta los objetos del color especificado
    blobs = img.find_blobs([color], pixels_threshold=100, area_threshold=100, merge=True)

    # Si se detecta una pelota
    if blobs:
        # Ordena los blobs por área (el más grande debería ser la pelota)
        largest_blob = max(blobs, key=lambda b: b.pixels())

        # coordenadas
        center_x = largest_blob.cx()
        center_y = largest_blob.cy()

        # Dibuja un rectángulo alrededor del blob más grande
        img.draw_rectangle(largest_blob.rect())
        img.draw_cross(center_x, center_y)

        # Imprime las coordenadas del centro del blob (pelota)
        print("Centro de la pelota: x={}, y={}".format(center_x, center_y))

m = int(input("Ingrese el valor de m(desde el centro de la pantalla hasta los pies de la camara: "))
w = int(input("resolucion horizontal de la camara:"))
h = int(input("resolucion vertical de la camara:"))
a = math.atan((w-center_x)/(m-center_y+(h/2)))
println(a)
    # Muestra la imagen en vivo
    img.compress(quality=35)

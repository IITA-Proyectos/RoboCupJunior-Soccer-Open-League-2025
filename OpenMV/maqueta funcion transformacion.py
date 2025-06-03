import sensor, image, time
import cv2
import numpy as np
import math
# Inicializar la cámara
sensor.reset()
sensor.set_pixformat(sensor.RGB565)  # Formato de color RGB
sensor.set_framesize(sensor.QVGA)    # Resolución 320x240
sensor.skip_frames(time=2000)       # Esperar a que los ajustes se estabilicen
sensor.set_auto_whitebal(False)     # Desactivar balance de blancos automático
clock = time.clock()                # Para medir FPS

# Definir umbrales para el color naranja en LAB (ajustar según necesidad)
# Puedes usar el Tools -> Machine Vision -> Threshold Editor en el IDE OpenMV para encontrar los valores correctos
orange_threshold = (30, 60, 20, 60, 10, 50)  # (L Min, L Max, A Min, A Max, B Min, B Max)

def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min

def tranformarcoordenadas(u, v):
    
    pt_pelota = np.array([u, v])
    
    #CALCULO DE DISTANCIA 
    H = np.array([ [-2.18880937e-03, -2.63497709e-01, -2.44260579e+02]
                   [ 1.02845724e+00,  1.44625829e-03, -1.92956221e+02]
                   [ 4.01073113e-04, -5.44887132e-02,  1.00000000e+00]])
    # Aplicar la transformación a la pelota 
    x, y = cv2.perspectiveTransform(pt_pelota, H)

    return x, y

while(True):
    clock.tick()
    img = sensor.snapshot()         # Capturar imagen

    m= 160

    # Encontrar blobs (objetos) que coincidan con el color naranja
    blobs = img.find_blobs([orange_threshold], pixels_threshold=100, area_threshold=100, merge=True)


    if blobs:
        print("pelota encontrada")

        # Seleccionar el blob más grande (asumimos que es la pelota)
        largest_blob = max(blobs, key=lambda b: b.pixels())

        # Dibujar un rectángulo alrededor del blob detectado
        img.draw_rectangle(largest_blob.rect(), color=(255, 0, 0))

        # Dibujar una cruz en el centro del blob
        img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=(0, 255, 0))

        
        # Obtener coordenadas del centro
        u, v = largest_blob.cx(), largest_blob.cy()
        
        # transformamos a centimetros
        x, y = transformarcoordenadas(u,v)

        print("Coordenadas físicas de la pelota:", crds_pelota_fisicas[0][0])
        


    # Mostrar FPS
    #print("FPS:", clock.fps())

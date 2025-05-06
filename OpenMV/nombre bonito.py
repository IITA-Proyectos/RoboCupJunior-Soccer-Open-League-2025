import sensor, image, time
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

while(True):
    clock.tick()
    img = sensor.snapshot()         # Capturar imagen
    # Dibujar línea vertical blanca centrada
    img.draw_line((160, 0, 160, 240), color=(255, 255, 255))

    m= 160

    # Encontrar blobs (objetos) que coincidan con el color naranja
    blobs = img.find_blobs([orange_threshold], pixels_threshold=100, area_threshold=100, merge=True)

    if blobs:
        # Seleccionar el blob más grande (asumimos que es la pelota)
        largest_blob = max(blobs, key=lambda b: b.pixels())

        # Dibujar un rectángulo alrededor del blob detectado
        img.draw_rectangle(largest_blob.rect(), color=(255, 0, 0))

        # Dibujar una cruz en el centro del blob
        img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=(0, 255, 0))

        # Obtener coordenadas del centro
        x, y = largest_blob.cx(), largest_blob.cy()

        # Mostrar coordenadas en la terminal
        print("Coordenadas Pelota Naranja: X = %d, Y = %d" % (x, y))

        if largest_blob.cy()!=145:
            a_rad = math.atan2((160-largest_blob.cx()),(m-largest_blob.cy()+120))
            angulo = math.degrees(a_rad) 
            print("angulo", abs(angulo))
            
            if angulo > 0:
                sentido = 1
                print(sentido)
            elif angulo < 0:
                sentido = 0
                print(sentido)
            else:
                print("")     
                
                


    # Mostrar FPS
    print("FPS:", clock.fps())
    

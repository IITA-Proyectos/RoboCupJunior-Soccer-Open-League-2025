import sensor, image, time

def calibrar_threshold_por_teclas(roi=(100, 100, 50, 50), muestras_objetivo=20):
    """
    Toma muestras CADA VEZ que presionas 'a'
    roi = (x, y, ancho, alto) - área de análisis
    """
    # ----- Inicializar cámara -----
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)

    # Calibrar balance de blancos automáticamente primero
    
    # ⚠️ DESACTIVAR ajustes automáticos durante calibración
    sensor.set_auto_whitebal(False)
    sensor.set_auto_gain(False) 
    
    #sensor.set_auto_whitebal(True)
    #sensor.set_auto_gain(True)
    #sensor.set_auto_exposure(True)

    # Esperar para que se ajusten bien los valores de color
    sensor.skip_frames(time=2000)

    # Ajustá exposure_us si la imagen queda muy oscura o muy brillante.
    #sensor.set_auto_exposure(False, exposure_us=37000)  # (opcional: exposición fija 15ms)

    # invertir imagen
    sensor.set_hmirror(True)
    sensor.set_vflip(True)
    
    print("=== CALIBRACIÓN POR TECLAS ===")
    print(f"Área de análisis: {roi}")
    print("INSTRUCCIONES:")
    print("1. Apunta al objeto que quieres detectar")
    print("2. Presiona 'a' para TOMAR MUESTRA")
    print("3. Mueve el objeto a diferentes posiciones/ángulos")
    print("4. Presiona 'b' para TERMINAR")
    print("")
    
    l_samples, a_samples, b_samples = [], [], []
    muestra_actual = 0
    
    while muestra_actual < muestras_objetivo:
        img = sensor.snapshot()
        
        # Dibujar área de análisis
        img.draw_rectangle(roi, color=(0, 255, 0), thickness=2)
        img.draw_string(roi[0], roi[1] - 15, "ZONA ANALISIS", color=(255,255,0))
        
        # Mostrar información
        img.draw_string(0, 0, f"Muestras: {muestra_actual}/{muestras_objetivo}", color=(255,255,255))
        img.draw_string(0, 10, "Presiona 'a' para tomar muestra", color=(255,255,255))
        img.draw_string(0, 20, "Presiona 'b' para terminar", color=(255,255,255))
        
        # Mostrar valores actuales del área (para referencia)
        stats_actual = img.get_statistics(roi=roi)
        img.draw_string(0, 35, f"L: {stats_actual.l_mean():.1f}", color=(255,255,255))
        img.draw_string(0, 45, f"A: {stats_actual.a_mean():.1f}", color=(255,255,255)) 
        img.draw_string(0, 55, f"B: {stats_actual.b_mean():.1f}", color=(255,255,255))
        
        # Verificar teclas
        if uart.any():
            tecla = uart.read(1).decode()
            
            if tecla == 'a':  # TOMAR MUESTRA
                stats = img.get_statistics(roi=roi)
                l_samples.append(stats.l_mean())
                a_samples.append(stats.a_mean())
                b_samples.append(stats.b_mean())
                muestra_actual += 1
                
                # Feedback visual
                img.draw_rectangle(roi, color=(255, 255, 0), thickness=3)  # Amarillo = muestra tomada
                img.draw_string(roi[0], roi[1] + roi[3] + 5, f"MUESTRA {muestra_actual}", color=(255,255,0))
                print(f"Muestra {muestra_actual} tomada - L:{stats.l_mean():.1f} A:{stats.a_mean():.1f} B:{stats.b_mean():.1f}")
                
                # Pequeña pausa para feedback visual
                for i in range(10):
                    img = sensor.snapshot()
                    img.draw_rectangle(roi, color=(255, 255, 0), thickness=3)
                    time.sleep(50)
                    
            elif tecla == 'b':  # TERMINAR
                print("Calibración terminada por usuario")
                break
    
    if not l_samples:
        print("ERROR: No se tomaron muestras")
        return None
    
    # Calcular threshold
    l_mean = sum(l_samples) / len(l_samples)
    a_mean = sum(a_samples) / len(a_samples) 
    b_mean = sum(b_samples) / len(b_samples)
    
    # Calcular márgenes con tolerancia
    l_margin = (max(l_samples) - min(l_samples)) / 2 * 1.5
    a_margin = (max(a_samples) - min(a_samples)) / 2 * 1.5
    b_margin = (max(b_samples) - min(b_samples)) / 2 * 1.5
    
    threshold = (
        int(max(0, l_mean - l_margin)), int(min(255, l_mean + l_margin)),
        int(max(0, a_mean - a_margin)), int(min(255, a_mean + a_margin)),
        int(max(0, b_mean - b_margin)), int(min(255, b_mean + b_margin))
    )
    
    # Mostrar resultados detallados
    print("\n" + "="*50)
    print("🎯 RESULTADOS DE CALIBRACIÓN")
    print("="*50)
    print(f"Muestras tomadas: {len(l_samples)}")
    print(f"Color promedio: L:{l_mean:.1f} A:{a_mean:.1f} B:{b_mean:.1f}")
    print(f"Rango detectado:")
    print(f"  L: {min(l_samples):.1f} - {max(l_samples):.1f}")
    print(f"  A: {min(a_samples):.1f} - {max(a_samples):.1f}") 
    print(f"  B: {min(b_samples):.1f} - {max(b_samples):.1f}")
    print("")
    print(f"✅ THRESHOLD FINAL:")
    print(f"   {threshold}")
    print("")
    print("Copia este threshold a tu código principal")
    
    return threshold

# PROGRAMA PRINCIPAL MEJORADO
def main():
    # CONFIGURACIÓN - Ajusta el área de análisis según tu necesidad
    # Formato: (x, y, ancho, alto)
    
    # Opciones predefinidas:
    area_centro = (135, 95, 50, 50)        # Área central
    area_esquina = (10, 10, 60, 60)        # Esquina superior izquierda  
    area_derecha = (200, 80, 40, 40)       # Lado derecho
    
    print("Selecciona área de análisis:")
    print("1. Centro (135, 95, 50, 50)")
    print("2. Esquina (10, 10, 60, 60)")
    print("3. Derecha (200, 80, 40, 40)")
    print("Usando área CENTRO por defecto...")
    
    # Iniciar calibración
    mi_threshold = calibrar_threshold_por_teclas(
        roi=area_centro, 
        muestras_objetivo=15  # Número de muestras que quieres tomar
    )
    
    if mi_threshold:
        print("\n" + "="*50)
        print("🎉 CALIBRACIÓN EXITOSA!")
        print("="*50)
        print("Ahora probando el threshold...")
        print("Apunta al objeto y verifica la detección")
        print("Presiona 'q' para salir")
        print("")
        
        # Modo de prueba
        while True:
            img = sensor.snapshot()
            
            # Buscar blobs con el threshold calculado
            blobs = img.find_blobs([mi_threshold], pixels_threshold=50, area_threshold=50)
            
            # Dibujar resultados
            for blob in blobs:
                img.draw_rectangle(blob.rect(), color=(255, 0, 0))
                img.draw_cross(blob.cx(), blob.cy(), color=(0, 255, 0))
                img.draw_string(blob.x(), blob.y() - 10, f"Area: {blob.area()}", color=(255,255,255))
            
            # Mostrar información
            img.draw_string(0, 0, f"Threshold: {mi_threshold}", color=(255,255,255))
            img.draw_string(0, 10, f"Blobs: {len(blobs)}", color=(255,255,255))
            img.draw_string(0, 20, "Presiona 'q' para salir", color=(255,255,255))
            
            # Verificar salida
            if uart.any() and uart.read(1).decode() == 'q':
                break

# Ejecutar programa
if __name__ == "__main__":
    main()

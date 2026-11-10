#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

const float DISTANCIA_SEGURA = 30.0; // cm

bool enMarcha = false;
float distanciaAnterior = 100.0;
unsigned long ultimoChequeoBloqueo = 0;

unsigned long tiempoUltimaEvasion = 0;
int deteccionesConsecutivas = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  iniciarUltrasonido();
  iniciarServo();
  iniciarMotores();

  delay(1000);
}

void loop() {
  float distanciaFrente = medirDistancia();

  // ========================================================
  // 1. DETECCIÓN DE PATINAJE / ATASCAMIENTO FÍSICO
  // ========================================================
  if (enMarcha) {
    if (millis() - ultimoChequeoBloqueo >= 1400) {
      // Solo consideramos atascamiento si estamos frente a una pared cercana (< 50 cm)
      // y la distancia prácticamente no cambió en 1.4 segundos
      if (abs(distanciaFrente - distanciaAnterior) < 1.5 && distanciaFrente < 50.0) {
        frenar();
        enMarcha = false;
        delay(600);
        
        desatorarRuedaLoca();
        delay(600);
        
        distanciaAnterior = medirDistancia();
        ultimoChequeoBloqueo = millis();
        return;
      }
      distanciaAnterior = distanciaFrente;
      ultimoChequeoBloqueo = millis();
    }
  }

  // ========================================================
  // 2. CICLO NORMAL DE NAVEGACIÓN
  // ========================================================
  if (distanciaFrente > DISTANCIA_SEGURA) {
    if (!enMarcha) {
      avanzarSuave();
      enMarcha = true;
      ultimoChequeoBloqueo = millis();
      distanciaAnterior = distanciaFrente;
    }
    delay(40);
  } else {
    // --- OBSTÁCULO FRONTAL DETECTADO ---

    // Detección de callejones cerrados
    if (millis() - tiempoUltimaEvasion < 2200) {
      deteccionesConsecutivas++;
    } else {
      deteccionesConsecutivas = 1;
    }
    tiempoUltimaEvasion = millis();

    // FASE 1: Detención y retroceso de despeje
    frenar();
    enMarcha = false;
    delay(600); // Reposo

    retrocederConTorque(280);
    delay(600); // Motores en 0V

    // FASE 2: Escape de 180° si ya encontró 2 paredes seguidas
    if (deteccionesConsecutivas >= 2) {
      girarDerecha(650); // 650 ms calibrados a 4.0V para 180° reales
      deteccionesConsecutivas = 0;
      delay(600);
      return;
    }

    // FASE 3: Escaneo lateral amplio (0° y 175°)
    mirarDerecha();
    delay(120);
    float distDerecha = medirDistancia();

    mirarIzquierda();
    delay(120);
    float distIzquierda = medirDistancia();

    mirarCentro();
    delay(100);

    // FASE 4: Reposo obligatorio post-servo
    delay(600);

    // FASE 5: Decisión de giro (400 ms ≈ 90° con batería cargada)
    if (distDerecha <= DISTANCIA_SEGURA && distIzquierda <= DISTANCIA_SEGURA) {
      girarDerecha(650); // 180° de escape
      deteccionesConsecutivas = 0;
    } else if (distDerecha >= distIzquierda) {
      girarDerecha(380);  // 90° a la derecha
    } else {
      girarIzquierda(380); // 90° a la izquierda
    }

    // FASE 6: Reposo antes de reanudar marcha
    frenar();
    delay(600);
  }
}
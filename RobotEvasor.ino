#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

// Umbrales de detección y callejón
const float DISTANCIA_SEGURA   = 38.0; // cm
const float DISTANCIA_CALLEJON = 48.0; // cm

bool enMarcha = false;
float distanciaAnterior = 100.0;
unsigned long ultimoChequeoBloqueo = 0;
unsigned long inicioAvanceContinuo = 0;

unsigned long tiempoUltimaEvasion = 0;
int deteccionesConsecutivas = 0;

void setup() {
  // Deshabilitar detector interno de brownout
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  iniciarUltrasonido();
  iniciarServo();
  iniciarMotores();

  delay(1000);
}

void loop() {
  float distanciaFrente = medirDistancia();

  // ========================================================
  // 1. CONTROL DE SEGURIDAD (ATASCAMIENTO / TROMPO OBLICUO)
  // ========================================================
  if (enMarcha) {
    // Si la trompa se clava o detecta impacto inminente (< 10 cm)
    if (distanciaFrente < 10.0 && distanciaFrente > 0.5) {
      frenar();
      enMarcha = false;
      delay(400);
      retrocederConTorque(450);
      delay(500);
      return;
    }

    // Chequeo de avance cada 1.0 s
    if (millis() - ultimoChequeoBloqueo >= 1000) {
      bool distanciaInvariable = (abs(distanciaFrente - distanciaAnterior) < 2.5);
      bool cercaObstaculo = (distanciaFrente < 60.0);
      bool tiempoExcedido = (millis() - inicioAvanceContinuo > 2000 && distanciaFrente < 75.0);

      if ((distanciaInvariable && cercaObstaculo) || tiempoExcedido) {
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
  // 2. NAVEGACIÓN Y DETECCIÓN
  // ========================================================
  if (distanciaFrente > DISTANCIA_SEGURA && !hayReboteEspecular()) {
    if (!enMarcha) {
      avanzarSuave();
      enMarcha = true;
      inicioAvanceContinuo = millis();
      ultimoChequeoBloqueo = millis();
      distanciaAnterior = distanciaFrente;
    }
    delay(30);
  } else {
    // --- OBSTÁCULO FRONTAL O DIAGONAL DETECTADO ---
    frenar();
    enMarcha = false;
    delay(600); // Reposo eléctrico con motores en 0V

    if (millis() - tiempoUltimaEvasion < 2500) {
      deteccionesConsecutivas++;
    } else {
      deteccionesConsecutivas = 1;
    }
    tiempoUltimaEvasion = millis();

    // Retroceso firme de despeje
    retrocederConTorque(350);
    delay(600); // Motores en 0V antes de mover el servo

    // ESCAPE OBLICUO (480 ms): rompe el bucle si detecta 2 paredes consecutivas
    if (deteccionesConsecutivas >= 2) {
      girarDerecha(480);
      deteccionesConsecutivas = 0;
      delay(600);
      return;
    }

    // --- ESCANEO DE 5 PUNTOS ---
    mirarDiagDerecha();
    delay(90);
    float distDiagDer = medirDistancia();

    mirarDerecha();
    delay(90);
    float distDer = medirDistancia();

    mirarDiagIzq();
    delay(100);
    float distDiagIzq = medirDistancia();

    mirarIzquierda();
    delay(90);
    float distIzq = medirDistancia();

    mirarCentro();
    delay(90);

    // Reposo post-servo antes de exigir corriente a motores
    delay(600);

    float espacioDerecha   = min(distDer, distDiagDer);
    float espacioIzquierda = min(distIzq, distDiagIzq);

    // TOMA DE DECISIÓN
    if (espacioDerecha <= DISTANCIA_CALLEJON && espacioIzquierda <= DISTANCIA_CALLEJON) {
      // Espacio cerrado: giro oblicuo de 480 ms para no rebotar en paralelo
      girarDerecha(480);
      deteccionesConsecutivas = 0;
    } else if (espacioDerecha >= espacioIzquierda) {
      int tiempoGiro = (distDiagDer < DISTANCIA_SEGURA) ? 460 : 380;
      girarDerecha(tiempoGiro);
    } else {
      int tiempoGiro = (distDiagIzq < DISTANCIA_SEGURA) ? 460 : 380;
      girarIzquierda(tiempoGiro);
    }

    frenar();
    delay(600);
  }
}
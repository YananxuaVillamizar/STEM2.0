#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

// Umbrales de detección ampliados
const float DISTANCIA_SEGURA   = 42.0; // cm: amplia anticipación contra paredes
const float DISTANCIA_CALLEJON = 50.0; // cm para encajonamientos

bool enMarcha = false;
float distanciaAnterior = 100.0;
unsigned long ultimoChequeoBloqueo = 0;
unsigned long inicioAvanceContinuo = 0;

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
  // 1. CONTROL DE SEGURIDAD (ATASCAMIENTO / BLOQUEO)
  // ========================================================
  if (enMarcha) {
    // Si la trompa se clava o se acerca a menos de 10 cm, freno inmediato
    if (distanciaFrente < 10.0 && distanciaFrente > 0.5) {
      frenar();
      enMarcha = false;
      delay(400);
      retrocederConTorque(450);
      delay(500);
      return;
    }

    if (millis() - ultimoChequeoBloqueo >= 1000) {
      bool distanciaInvariable = (abs(distanciaFrente - distanciaAnterior) < 2.5);
      bool cercaObstaculo = (distanciaFrente < 65.0);
      bool tiempoExcedido = (millis() - inicioAvanceContinuo > 2200 && distanciaFrente < 80.0);

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
  // 2. NAVEGACIÓN Y EVASIÓN PREVENTIVA
  // ========================================================
  if (distanciaFrente > DISTANCIA_SEGURA) {
    if (!enMarcha) {
      avanzarConFuerza();
      enMarcha = true;
      inicioAvanceContinuo = millis();
      ultimoChequeoBloqueo = millis();
      distanciaAnterior = distanciaFrente;
    }
    delay(30);
  } else {
    // --- OBSTÁCULO DETECTADO A 42 CM O MENOS ---
    frenar();
    enMarcha = false;
    delay(600); // Reposo para recuperar la batería de 1 celda

    // Memoria de callejón
    if (millis() - tiempoUltimaEvasion < 2500) {
      deteccionesConsecutivas++;
    } else {
      deteccionesConsecutivas = 1;
    }
    tiempoUltimaEvasion = millis();

    // Despeje con retroceso firme
    retrocederConTorque(320);
    delay(600); // Motores en 0V antes de mover el servo

    // Escape directo de 180° si ya chocó con 2 paredes consecutivas
    if (deteccionesConsecutivas >= 2) {
      girarDerecha(650);
      deteccionesConsecutivas = 0;
      delay(600);
      return;
    }

    // Escaneo lateral
    mirarDerecha();
    delay(100);
    float distDerecha = medirDistancia();

    mirarIzquierda();
    delay(100);
    float distIzquierda = medirDistancia();

    mirarCentro();
    delay(90);

    // Reposo post-servo antes de exigir corriente a motores
    delay(600);

    // Toma de decisión hacia el camino libre
    if (distDerecha <= DISTANCIA_CALLEJON && distIzquierda <= DISTANCIA_CALLEJON) {
      girarDerecha(650); // Giro de 180° para escapar del cajón
      deteccionesConsecutivas = 0;
    } else if (distDerecha >= distIzquierda) {
      girarDerecha(380);  // 90° derecha
    } else {
      girarIzquierda(380); // 90° izquierda
    }

    frenar();
    delay(600);
  }
}
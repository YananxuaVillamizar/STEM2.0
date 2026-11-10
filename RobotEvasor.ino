#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

// Umbral ampliado: previene choques en los extremos de las ruedas
const float DISTANCIA_SEGURA = 32.0; // cm

bool enMarcha = false;
float distanciaAnterior = 100.0;
unsigned long ultimoChequeoBloqueo = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  iniciarUltrasonido();
  iniciarServo();
  iniciarMotores();

  // Pausa de estabilización inicial
  delay(1200);
}

void loop() {
  float distanciaFrente = medirDistancia();

  // ========================================================
  // 1. CONTROL DE TRABAMIENTO (RUEDA LOCA O PISO IRREGULAR)
  // ========================================================
  if (enMarcha) {
    if (millis() - ultimoChequeoBloqueo >= 1000) {
      // Si la distancia frontal no cambia y hay una pared a menos de 75 cm, está patinando/atascado
      if (abs(distanciaFrente - distanciaAnterior) < 2.0 && distanciaFrente < 75.0) {
        frenar();
        enMarcha = false;
        delay(800);
        
        // Ejecutar rutina de pivote para destrabar la rueda loca
        desatorarRuedaLoca();
        delay(800);
        
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
    // --- OBSTÁCULO ENCONTRADO ---

    // PASO 1: Frenar en seco inmediatamente
    frenar();
    enMarcha = false;
    delay(800); // Reposo para que la celda se recupere

    // PASO 2: RETROCESO OBLIGATORIO DE DESPEJE (320 ms con torque)
    retrocederConTorque(320);
    delay(800); // 800 ms con motores en 0V antes de activar el servo

    // PASO 3: ESCANEO LATERAL AMPLIO (MOTORES COMPLETAMENTE APAGADOS)
    mirarDerecha();
    delay(250);
    float distDerecha = medirDistancia();

    mirarIzquierda();
    delay(250);
    float distIzquierda = medirDistancia();

    mirarCentro();
    delay(200);

    // PASO 4: Tiempo muerto obligatorio entre servo y motores
    delay(800);

    // PASO 5: GIRO CON ESPACIO DESPEJADO TRAS EL RETROCESO
    if (distDerecha >= distIzquierda && distDerecha > DISTANCIA_SEGURA) {
      girarDerecha(450);
    } else if (distIzquierda > distDerecha && distIzquierda > DISTANCIA_SEGURA) {
      girarIzquierda(450);
    } else {
      // Si ambos lados están comprometidos: giro de escape
      girarDerecha(850);
    }

    // PASO 6: Reposo final antes de volver a evaluar el avance
    delay(800);
  }
}
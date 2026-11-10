#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

const float DISTANCIA_SEGURA = 25.0; // cm

bool enMarcha = false;
unsigned long tiempoInicioAvance = 0;
float distanciaAnterior = 100.0;
unsigned long ultimoChequeoBloqueo = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  iniciarUltrasonido();

  // 1. Iniciar servo primero para no colisionar con PWM de motores
  iniciarServo();

  // 2. Iniciar etapa de motores (en reposo)
  iniciarMotores();

  // 3. Estabilización completa de la batería de 1 celda
  delay(1200);
}

void loop() {
  float distanciaFrente = medirDistancia();

  // ========================================================
  // 1. DETECCIÓN DE ATASCAMIENTO O RUEDAS TRABADAS
  // ========================================================
  if (enMarcha) {
    // Cada 1200 ms revisamos si el robot realmente se ha desplazado
    if (millis() - ultimoChequeoBloqueo >= 1200) {
      // Si la distancia sigue siendo prácticamente la misma (menos de 2 cm de variación)
      // y estamos a menos de 70 cm de una pared o referencia, estamos atascados
      if (abs(distanciaFrente - distanciaAnterior) < 2.0 && distanciaFrente < 70.0) {
        frenar();
        enMarcha = false;
        delay(800); // Reposo para que la celda se recupere
        
        // Ejecutar rutina de desenganche físico
        sacudidaEscape();
        delay(800);
        
        distanciaAnterior = medirDistancia();
        ultimoChequeoBloqueo = millis();
        return; // Salir de esta iteración para reevaluar entorno
      }
      distanciaAnterior = distanciaFrente;
      ultimoChequeoBloqueo = millis();
    }
  }

  // ========================================================
  // 2. LÓGICA DE NAVEGACIÓN Y EVASIÓN
  // ========================================================
  if (distanciaFrente > DISTANCIA_SEGURA) {
    if (!enMarcha) {
      avanzarSuave();
      enMarcha = true;
      tiempoInicioAvance = millis();
      ultimoChequeoBloqueo = millis();
      distanciaAnterior = distanciaFrente;
    }
    delay(40);
  } else {
    // --- OBSTÁCULO FRONTAL DETECTADO ---
    frenar();
    enMarcha = false;
    delay(800); // 800 ms de reposo para recuperar la tensión de la 18650

    // FASE 1: Despeje previo (retroceso corto con torque)
    retrocederConTorque(280);
    delay(800); // Reposo post-retroceso

    // FASE 2: Escaneo angular amplio (MOTORES 100% INACTIVOS)
    mirarDerecha();
    delay(250);
    float distDerecha = medirDistancia();

    mirarIzquierda();
    delay(250);
    float distIzquierda = medirDistancia();

    mirarCentro();
    delay(200);

    // FASE 3: Tiempo muerto obligatorio entre servo y motores
    delay(800);

    // FASE 4: Giro del chasis hacia el sector con mayor distancia
    if (distDerecha >= distIzquierda && distDerecha > DISTANCIA_SEGURA) {
      girarDerecha(420);
    } else if (distIzquierda > distDerecha && distIzquierda > DISTANCIA_SEGURA) {
      girarIzquierda(420);
    } else {
      // Encajonado o pared continua: giro amplio de escape
      girarDerecha(780);
    }

    // FASE 5: Pausa de estabilización previa a reanudar marcha
    delay(800);
  }
}
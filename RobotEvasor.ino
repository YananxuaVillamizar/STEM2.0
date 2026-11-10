#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

const float DISTANCIA_SEGURA = 25.0; // cm
bool enMarcha = false;

void setup() {
  // Desactivar detección interna de brownout
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  iniciarUltrasonido();

  // 1. Iniciar servo PRIMERO para evitar conflictos de timers
  iniciarServo();

  // 2. Iniciar etapa de motores (en estado frenado)
  iniciarMotores();

  // 3. Pausa crucial: estabilización eléctrica completa de la celda antes de mover nada
  delay(1500);
}

void loop() {
  float distanciaFrente = medirDistancia();

  if (distanciaFrente > DISTANCIA_SEGURA) {
    // Si no estaba avanzando, arrancar con rampa suave
    if (!enMarcha) {
      avanzarSuave();
      enMarcha = true;
    }
    delay(40); // Avance continuo sin sobrecargar el procesador
  } else {
    // --- OBSTÁCULO DETECTADO ---

    // FASE 1: Detener tracción al 100%
    frenar();
    enMarcha = false;
    delay(800); // 800 ms de reposo para recuperar el voltaje de la batería

    // FASE 2: Escaneo angular (MOTORES TOTALMENTE APAGADOS)
    mirarDerecha();
    delay(200);
    float distDerecha = medirDistancia();

    mirarIzquierda();
    delay(200);
    float distIzquierda = medirDistancia();

    mirarCentro();
    delay(200);

    // FASE 3: Reposo post-servo antes de pedir corriente a los motores
    delay(800);

    // FASE 4: Giro del chasis hacia el lado más despejado (SERVO QUIETO AL FRENTE)
    if (distDerecha >= distIzquierda && distDerecha > DISTANCIA_SEGURA) {
      girarDerecha();
      delay(420); // Ajustar duración según agarre
    } else if (distIzquierda > distDerecha && distIzquierda > DISTANCIA_SEGURA) {
      girarIzquierda();
      delay(420);
    } else {
      // Encajonado: rotación amplia
      girarDerecha();
      delay(800);
    }

    // FASE 5: Frenar y estabilizar antes de evaluar de nuevo la marcha recta
    frenar();
    delay(800);
  }
}
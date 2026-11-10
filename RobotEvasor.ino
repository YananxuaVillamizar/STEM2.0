#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

const float DISTANCIA_SEGURA = 25.0; // cm

void setup() {
  // Desactivar detector de brownout por software
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  iniciarMotores();
  iniciarUltrasonido();

  // 1. Iniciar y posicionar servo al centro (85°)
  iniciarServo();

  // 2. Tiempo muerto obligatorio de 1 s con servo quieto antes de mover motores
  delay(1000);
}

void loop() {
  float distanciaFrente = medirDistancia();

  if (distanciaFrente > DISTANCIA_SEGURA) {
    avanzar();
    delay(50);
  } else {
    // --- FASE 1: FRENAR TRACCIÓN ---
    frenar();
    delay(1000); // 1 s de reposo para recuperar voltaje en la celda

    // --- FASE 2: MANIOBRA DE DESPEJE ---
    retroceder();
    delay(200);
    frenar();
    delay(1000); // 1 s obligatorio antes de encender el servo

    // --- FASE 3: ESCANEO LATERAL (MOTORES APAGADOS) ---
    mirarDerecha();
    delay(200);
    float distDerecha = medirDistancia();

    mirarIzquierda();
    delay(200);
    float distIzquierda = medirDistancia();

    mirarCentro();
    
    // --- FASE 4: TRANSICIÓN SERVO -> MOTORES ---
    // 1 s obligatorio tras terminar el escaneo antes de mover las ruedas
    delay(1000); 

    // --- FASE 5: GIRO DEL CHASIS (SERVO INMÓVIL AL FRENTE) ---
    if (distDerecha >= distIzquierda && distDerecha > DISTANCIA_SEGURA) {
      girarDerecha();
      delay(380);
    } else if (distIzquierda > distDerecha && distIzquierda > DISTANCIA_SEGURA) {
      girarIzquierda();
      delay(380);
    } else {
      girarDerecha();
      delay(750);
    }

    // Frenar y esperar 1 s antes de retornar a la marcha normal
    frenar();
    delay(1000);
  }
}
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "Motores.h"
#include "SensorUltrasonido.h"
#include "ServoCabezal.h"

// Umbral preventivo ampliado contra rebotes oblicuos y choques en diagonal
const float DISTANCIA_SEGURA = 35.0; // cm
const float DISTANCIA_CALLEJON = 45.0; // cm para espacios de 3 paredes más anchos

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
  // 1. PROTECCIÓN ANTI-STALL Y CONTROL DE SUELO TRABADO
  // ========================================================
  if (enMarcha) {
    // Si lleva más de 1.8 s avanzando sin interrupción frente a un obstáculo
    // o la distancia no disminuye, cortamos potencia para evitar pico de corriente
    if (millis() - ultimoChequeoBloqueo >= 1000) {
      bool distanciaInvariable = (abs(distanciaFrente - distanciaAnterior) < 2.5);
      bool cercaObstaculo = (distanciaFrente < 60.0);
      bool tiempoExcedido = (millis() - inicioAvanceContinuo > 2500 && distanciaFrente < 80.0);

      if ((distanciaInvariable && cercaObstaculo) || tiempoExcedido) {
        // CORTE DE SEGURIDAD INMEDIATO
        frenar();
        enMarcha = false;
        delay(600); // Reposo para recuperar la celda
        
        // Destrabar mecánicamente la rueda loca con sacudón en pivote
        desatorarRuedaLoca();
        delay(600);
        
        distanciaAnterior = medirDistancia();
        ultimoChequeoBloqueo = millis();
        return; // Salir sin escanear para no agotar la batería innecesariamente
      }
      distanciaAnterior = distanciaFrente;
      ultimoChequeoBloqueo = millis();
    }
  }

  // ========================================================
  // 2. NAVEGACIÓN Y EVASIÓN
  // ========================================================
  if (distanciaFrente > DISTANCIA_SEGURA) {
    if (!enMarcha) {
      avanzarSuave();
      enMarcha = true;
      inicioAvanceContinuo = millis();
      ultimoChequeoBloqueo = millis();
      distanciaAnterior = distanciaFrente;
    }
    delay(35);
  } else {
    // --- OBSTÁCULO DETECTADO ---

    // Memoria de callejón: evaluar si cayó en 2 obstáculos en menos de 2.5 s
    if (millis() - tiempoUltimaEvasion < 2500) {
      deteccionesConsecutivas++;
    } else {
      deteccionesConsecutivas = 1;
    }
    tiempoUltimaEvasion = millis();

    // FASE 1: Frenar y retroceso de despeje
    frenar();
    enMarcha = false;
    delay(600); // 600 ms de reposo en 0V

    retrocederConTorque(320);
    delay(600); // Desacoplo de motores antes del servo

    // FASE 2: Escape de 180° si ya chocó con 2 paredes consecutivas
    if (deteccionesConsecutivas >= 2) {
      girarDerecha(680); // Giro de 180° limpio a 4.0V
      deteccionesConsecutivas = 0;
      delay(600);
      return;
    }

    // FASE 3: Escaneo lateral angular rápido (0° y 175°)
    mirarDerecha();
    delay(100);
    float distDerecha = medirDistancia();

    mirarIzquierda();
    delay(100);
    float distIzquierda = medirDistancia();

    mirarCentro();
    delay(90);

    // FASE 4: Reposo obligatorio post-servo
    delay(600);

    // FASE 5: Decisión de giro con rango de callejón ampliado
    // Si ambos lados están por debajo de 45 cm, estamos en un espacio cerrado de 3 paredes
    if (distDerecha <= DISTANCIA_CALLEJON && distIzquierda <= DISTANCIA_CALLEJON) {
      girarDerecha(680); // Giro de 180° obligatorio para salir del fondo
      deteccionesConsecutivas = 0;
    } else if (distDerecha >= distIzquierda) {
      girarDerecha(380);  // 90° derecha
    } else {
      girarIzquierda(380); // 90° izquierda
    }

    // FASE 6: Pausa previa a la reanudación
    frenar();
    delay(600);
  }
}
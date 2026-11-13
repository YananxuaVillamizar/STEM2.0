#ifndef SENSOR_ULTRASONIDO_H
#define SENSOR_ULTRASONIDO_H

#include <Arduino.h>

const int PIN_TRIG = 4;
const int PIN_ECHO = 22;

static float ultimaDistanciaValida = 100.0;
static bool reboteEspecularDetectado = false;

inline void iniciarUltrasonido() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
}

inline float medirDistanciaCruda() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duracion = pulseIn(PIN_ECHO, HIGH, 22000); // ~3.7 metros
  if (duracion == 0) {
    return 400.0;
  }
  return (duracion * 0.0343) / 2.0;
}

inline float medirDistancia() {
  float d1 = medirDistanciaCruda();
  delay(10);
  float d2 = medirDistanciaCruda();
  float distMedida = min(d1, d2);

  // DETECCIÓN DE REBOTE ESPECULAR EN DIAGONAL:
  // Si estábamos a menos de 55 cm de algo y súbitamente la señal se pierde marcando >250 cm,
  // la pared no desapareció: el haz rebotó hacia un lado en diagonal.
  if (ultimaDistanciaValida < 55.0 && distMedida > 250.0) {
    reboteEspecularDetectado = true;
    // Forzamos una distancia crítica virtual para activar frenado preventivo
    return ultimaDistanciaValida; 
  } else {
    reboteEspecularDetectado = false;
  }

  if (distMedida < 300.0) {
    ultimaDistanciaValida = distMedida;
  }
  return distMedida;
}

inline bool hayReboteEspecular() {
  return reboteEspecularDetectado;
}

#endif
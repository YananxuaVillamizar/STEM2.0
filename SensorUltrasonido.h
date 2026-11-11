#ifndef SENSOR_ULTRASONIDO_H
#define SENSOR_ULTRASONIDO_H

#include <Arduino.h>

const int PIN_TRIG = 4;
const int PIN_ECHO = 22;

static float ultimaLecturaValida = 100.0;

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

  // Timeout de 20 ms (~3.4 metros max)
  long duracion = pulseIn(PIN_ECHO, HIGH, 20000);

  // Si no hay eco (0 us), puede ser camino libre (>3m) O zona ciega por choque pegado (<2cm)
  if (duracion == 0) {
    return 400.0;
  }
  return (duracion * 0.0343) / 2.0;
}

// Filtro de confirmación: si una lectura detecta obstáculo, manda la menor
inline float medirDistancia() {
  float d1 = medirDistanciaCruda();
  delay(12);
  float d2 = medirDistanciaCruda();

  // Si una lectura detectó obstáculo cercano y la otra dio 400 (rebote perdido),
  // se prioriza el obstáculo real para no chocar
  float menor = min(d1, d2);
  float mayor = max(d1, d2);

  if (menor < 50.0 && mayor > 250.0) {
    ultimaLecturaValida = menor;
    return menor;
  }

  ultimaLecturaValida = d1;
  return d1;
}

#endif
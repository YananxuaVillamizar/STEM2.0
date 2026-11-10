#ifndef SENSOR_ULTRASONIDO_H
#define SENSOR_ULTRASONIDO_H

#include <Arduino.h>

const int PIN_TRIG = 4;
const int PIN_ECHO = 22;

inline void iniciarUltrasonido() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
}

inline float medirDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Timeout de 25000 us (~4.2 metros)
  long duracion = pulseIn(PIN_ECHO, HIGH, 25000);

  if (duracion == 0) {
    return 400.0; // Espacio libre si no hay eco
  }
  return (duracion * 0.0343) / 2.0;
}

#endif
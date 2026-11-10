#ifndef MOTORES_H
#define MOTORES_H

#include <Arduino.h>

// Pines Motor Izquierdo (GTT-Left)
const int PIN_EN1 = 27;
const int PIN_IN1 = 13;
const int PIN_IN2 = 26;

// Pines Motor Derecho (GTT-Right)
const int PIN_EN2 = 25;
const int PIN_IN3 = 33;
const int PIN_IN4 = 32;

// Velocidades moderadas para cuidar la celda 18650
const int VELOCIDAD_CRUCERO = 150; 
const int VELOCIDAD_GIRO    = 145; 

inline void frenar() {
  analogWrite(PIN_EN1, 0);
  analogWrite(PIN_EN2, 0);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
}

inline void iniciarMotores() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_EN1, OUTPUT);
  pinMode(PIN_EN2, OUTPUT);
  frenar();
}

// Arranque progresivo para no colapsar la batería
inline void avanzarSuave() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  for (int v = 70; v <= VELOCIDAD_CRUCERO; v += 15) {
    analogWrite(PIN_EN1, v);
    analogWrite(PIN_EN2, v);
    delay(15);
  }
}

inline void girarDerecha() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  
  analogWrite(PIN_EN1, VELOCIDAD_GIRO);
  delay(15);
  analogWrite(PIN_EN2, VELOCIDAD_GIRO);
}

inline void girarIzquierda() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  analogWrite(PIN_EN1, VELOCIDAD_GIRO);
  delay(15);
  analogWrite(PIN_EN2, VELOCIDAD_GIRO);
}

#endif
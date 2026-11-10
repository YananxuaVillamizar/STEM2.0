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

// 60% de velocidad: 255 * 0.60 ≈ 153
const int VELOCIDAD_CRUCERO = 155; 
// Ligero desfase para giros más controlados si hace falta
const int VELOCIDAD_GIRO    = 150; 

inline void frenar() {
  analogWrite(PIN_EN1, 0);
  analogWrite(PIN_EN2, 0);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
}

inline void iniciarMotores() {
  pinMode(PIN_EN1, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_EN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  frenar();
}

inline void aplicarVelocidad(int vel) {
  // Encendido escalonado con PWM limitado
  analogWrite(PIN_EN1, vel);
  delay(15); // Desfase de corriente de arranque
  analogWrite(PIN_EN2, vel);
}

inline void avanzar() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);
  aplicarVelocidad(VELOCIDAD_CRUCERO);
}

inline void retroceder() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  aplicarVelocidad(VELOCIDAD_CRUCERO);
}

inline void girarDerecha() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  aplicarVelocidad(VELOCIDAD_GIRO);
}

inline void girarIzquierda() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);
  aplicarVelocidad(VELOCIDAD_GIRO);
}

#endif
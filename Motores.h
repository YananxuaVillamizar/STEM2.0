#ifndef MOTORES_H
#define MOTORES_H

#include <Arduino.h>

// Pines Driver L293D
const int PIN_EN1 = 27;
const int PIN_IN1 = 13;
const int PIN_IN2 = 26;

const int PIN_EN2 = 25;
const int PIN_IN3 = 33;
const int PIN_IN4 = 32;

// Velocidades de operación
const int VELOCIDAD_CRUCERO = 160; 
const int VELOCIDAD_GIRO    = 155; 
const int VELOCIDAD_RESCATE = 185;

// Calibración de línea recta
const float TRIM_IZQ = 0.96; 
const float TRIM_DER = 1.00;

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

// Función requerida por RobotEvasor.ino
inline void avanzarSuave() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  // Impulso inicial breve (Kick-start) para romper la inercia del suelo y la caster
  analogWrite(PIN_EN1, (int)(180 * TRIM_IZQ));
  analogWrite(PIN_EN2, (int)(180 * TRIM_DER));
  delay(40);

  // Velocidad de crucero balanceada
  analogWrite(PIN_EN1, (int)(VELOCIDAD_CRUCERO * TRIM_IZQ));
  analogWrite(PIN_EN2, (int)(VELOCIDAD_CRUCERO * TRIM_DER));
}

// Alias de seguridad
inline void avanzarConFuerza() {
  avanzarSuave();
}

inline void retrocederConTorque(int duracionMs) {
  duracionMs = min(duracionMs, 500);

  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  analogWrite(PIN_EN1, VELOCIDAD_RESCATE);
  delay(15);
  analogWrite(PIN_EN2, VELOCIDAD_RESCATE);
  delay(duracionMs);
  frenar();
}

inline void girarDerecha(int duracionMs) {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  
  analogWrite(PIN_EN1, VELOCIDAD_GIRO);
  delay(15);
  analogWrite(PIN_EN2, VELOCIDAD_GIRO);
  delay(duracionMs);
  frenar();
}

inline void girarIzquierda(int duracionMs) {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  analogWrite(PIN_EN1, VELOCIDAD_GIRO);
  delay(15);
  analogWrite(PIN_EN2, VELOCIDAD_GIRO);
  delay(duracionMs);
  frenar();
}

inline void desatorarRuedaLoca() {
  retrocederConTorque(380);
  delay(120);
  girarDerecha(380);
  delay(120);
  frenar();
}

#endif
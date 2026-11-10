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

// Velocidades balanceadas
const int VELOCIDAD_CRUCERO = 160; 
const int VELOCIDAD_GIRO    = 155; 
const int VELOCIDAD_RESCATE = 185;

// ============================================================================
// COMPENSACIÓN DE LÍNEA RECTA
// Con 0.90 se iba a la izquierda. Nivelamos con 0.96 para trayectoria recta
// ============================================================================
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

inline void avanzarSuave() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  int targetIzq = (int)(VELOCIDAD_CRUCERO * TRIM_IZQ);
  int targetDer = (int)(VELOCIDAD_CRUCERO * TRIM_DER);

  for (int v = 95; v <= VELOCIDAD_CRUCERO; v += 20) {
    analogWrite(PIN_EN1, (int)(v * TRIM_IZQ));
    analogWrite(PIN_EN2, (int)(v * TRIM_DER));
    delay(8);
  }
  analogWrite(PIN_EN1, targetIzq);
  analogWrite(PIN_EN2, targetDer);
}

inline void retrocederConTorque(int duracionMs) {
  // Limitar duración máxima para proteger la celda de sobrecorriente
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

// Maniobra física de destrabe rápido (sin mover el servo)
inline void desatorarRuedaLoca() {
  // Sacudón diferencial en pivote para reorientar la rueda loca en el suelo
  girarDerecha(180);
  delay(120);
  girarIzquierda(220);
  delay(120);
  retrocederConTorque(280);
  delay(120);
  frenar();
}

#endif
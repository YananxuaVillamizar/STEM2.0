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
const int VELOCIDAD_RESCATE = 180;

// ============================================================================
// CORRECCIÓN DE TRAYECTORIA
// Si vira a la DERECHA: bajamos TRIM_IZQ para que el izquierdo no empuje de más
// ============================================================================
const float TRIM_IZQ = 0.90; // Reducido para evitar el desvío a la derecha
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

  for (int v = 90; v <= VELOCIDAD_CRUCERO; v += 25) {
    analogWrite(PIN_EN1, (int)(v * TRIM_IZQ));
    analogWrite(PIN_EN2, (int)(v * TRIM_DER));
    delay(8);
  }
  analogWrite(PIN_EN1, targetIzq);
  analogWrite(PIN_EN2, targetDer);
}

inline void retrocederConTorque(int duracionMs) {
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
  girarDerecha(160);
  delay(100);
  girarIzquierda(200);
  delay(100);
  retrocederConTorque(300);
  delay(100);
  frenar();
}

#endif
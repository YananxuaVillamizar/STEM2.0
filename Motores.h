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

// Velocidad de crucero controlada
const int VELOCIDAD_CRUCERO = 160; 
const int VELOCIDAD_GIRO    = 155; 
// Pulso con algo más de fuerza para desatascar las ruedas
const int VELOCIDAD_RESCATE = 175;

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

  // Rampa de aceleración con piso en 90 para romper la fricción del piso
  for (int v = 90; v <= VELOCIDAD_CRUCERO; v += 15) {
    analogWrite(PIN_EN1, v);
    analogWrite(PIN_EN2, v);
    delay(12);
  }
}

inline void retrocederConTorque(int duracionMs) {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  // Se arranca con un pequeño escalón para liberar la rueda loca
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

// Rutina de sacudón para liberar ruedas trabadas
inline void sacudidaEscape() {
  retrocederConTorque(350);
  delay(200);
  girarDerecha(250);
  delay(200);
  girarIzquierda(250);
  frenar();
}

#endif
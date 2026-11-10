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

// Velocidades calibradas para la celda 18650
const int VELOCIDAD_CRUCERO = 165; 
const int VELOCIDAD_GIRO    = 160; 
const int VELOCIDAD_RESCATE = 180;

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

  // Rampa de aceleración para no tirar abajo el voltaje de la batería
  for (int v = 95; v <= VELOCIDAD_CRUCERO; v += 15) {
    analogWrite(PIN_EN1, v);
    analogWrite(PIN_EN2, v);
    delay(10);
  }
}

// Retroceso con suficiente fuerza para librar obstáculos
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

// Maniobra oscilatoria para obligar a la rueda loca a alinearse
inline void desatorarRuedaLoca() {
  // Sacudón en pivote a la derecha
  girarDerecha(180);
  delay(150);
  // Sacudón en pivote a la izquierda (la rueda loca pivota y se orienta)
  girarIzquierda(220);
  delay(150);
  // Retroceso firme una vez orientada la rueda loca
  retrocederConTorque(350);
  delay(150);
  frenar();
}

#endif
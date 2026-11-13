#ifndef SERVO_CABEZAL_H
#define SERVO_CABEZAL_H

#include <Arduino.h>
#include <ESP32Servo.h>

const int PIN_SERVO = 23;

// Calibración angular con centro físico real (92°)
const int POS_FRENTE       = 92; 
const int POS_DIAG_DER     = 45;   // 45° Diagonal derecha
const int POS_DERECHA      = 0;    // 0° Perpendicular derecha
const int POS_DIAG_IZQ     = 138;  // 45° Diagonal izquierda
const int POS_IZQUIERDA    = 180;  // 180° Perpendicular izquierda

static Servo servoSensor;
static int anguloActual = POS_FRENTE;

inline void moverRapido(int anguloDestino) {
  anguloDestino = constrain(anguloDestino, 0, 180);
  if (anguloDestino == anguloActual) return;

  int paso = (anguloDestino > anguloActual) ? 6 : -6;
  while (abs(anguloDestino - anguloActual) > 6) {
    anguloActual += paso;
    servoSensor.write(anguloActual);
    delay(4);
  }
  
  anguloActual = anguloDestino;
  servoSensor.write(anguloActual);
  delay(85);
}

inline void iniciarServo() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  servoSensor.setPeriodHertz(50);

  anguloActual = POS_FRENTE;
  servoSensor.write(POS_FRENTE);
  servoSensor.attach(PIN_SERVO, 500, 2400);
  servoSensor.write(POS_FRENTE);
  
  delay(400);
}

inline void mirarCentro()      { moverRapido(POS_FRENTE); }
inline void mirarDiagDerecha() { moverRapido(POS_DIAG_DER); }
inline void mirarDerecha()     { moverRapido(POS_DERECHA); }
inline void mirarDiagIzq()     { moverRapido(POS_DIAG_IZQ); }
inline void mirarIzquierda()   { moverRapido(POS_IZQUIERDA); }

#endif
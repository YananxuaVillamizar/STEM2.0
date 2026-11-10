#ifndef SERVO_CABEZAL_H
#define SERVO_CABEZAL_H

#include <Arduino.h>
#include <ESP32Servo.h>

const int PIN_SERVO = 23;

// Tu centro físico real
const int POS_FRENTE    = 85; 
// 90 grados a cada lado respecto al centro
const int POS_DERECHA   = 0;   // 85 - 85 grados (tope derecho real)
const int POS_IZQUIERDA = 175; // 85 + 90 grados (tope izquierdo real)

static Servo servoSensor;
static int anguloActual = POS_FRENTE;

inline void moverRapido(int anguloDestino) {
  anguloDestino = constrain(anguloDestino, 0, 180);
  if (anguloDestino == anguloActual) return;

  int paso = (anguloDestino > anguloActual) ? 6 : -6;
  while (abs(anguloDestino - anguloActual) > 6) {
    anguloActual += paso;
    servoSensor.write(anguloActual);
    delay(4); // Movimiento ágil y continuo
  }
  
  anguloActual = anguloDestino;
  servoSensor.write(anguloActual);
  delay(90); // Asentamiento
}

inline void iniciarServo() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  servoSensor.setPeriodHertz(50);

  anguloActual = POS_FRENTE;
  servoSensor.write(POS_FRENTE);
  // Rango 500-2400 us para que el SG90 alcance los 180 grados reales
  servoSensor.attach(PIN_SERVO, 500, 2400);
  servoSensor.write(POS_FRENTE);
  
  delay(400);
}

inline void mirarCentro() {
  moverRapido(POS_FRENTE);
}

inline void mirarDerecha() {
  moverRapido(POS_DERECHA);
}

inline void mirarIzquierda() {
  moverRapido(POS_IZQUIERDA);
}

#endif
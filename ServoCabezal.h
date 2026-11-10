#ifndef SERVO_CABEZAL_H
#define SERVO_CABEZAL_H

#include <Arduino.h>
#include <ESP32Servo.h>

const int PIN_SERVO = 23;

// Tu frente real exacto
const int POS_FRENTE    = 85; 
// Cobertura angular abierta para proteger las llantas en esquinas
const int POS_DERECHA   = 15;  // 85 - 70 grados (visión total hacia la llanta derecha)
const int POS_IZQUIERDA = 155; // 85 + 70 grados (visión total hacia la llanta izquierda)

static Servo servoSensor;
static int anguloActual = POS_FRENTE;

inline void moverSuave(int anguloDestino) {
  anguloDestino = constrain(anguloDestino, 10, 170);
  if (anguloDestino == anguloActual) return;

  int paso = (anguloDestino > anguloActual) ? 1 : -1;
  
  while (anguloActual != anguloDestino) {
    anguloActual += paso;
    servoSensor.write(anguloActual);
    delay(12); // Movimiento suave
  }
  
  delay(120); // Asentamiento
}

inline void iniciarServo() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  servoSensor.setPeriodHertz(50);

  // Inicializar en 85° sin latigazo previo
  anguloActual = POS_FRENTE;
  servoSensor.write(POS_FRENTE);
  servoSensor.attach(PIN_SERVO, 1000, 2000);
  servoSensor.write(POS_FRENTE);
  
  delay(800);
}

inline void mirarCentro() {
  moverSuave(POS_FRENTE);
}

inline void mirarDerecha() {
  moverSuave(POS_DERECHA);
}

inline void mirarIzquierda() {
  moverSuave(POS_IZQUIERDA);
}

#endif
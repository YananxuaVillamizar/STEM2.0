#ifndef SERVO_CABEZAL_H
#define SERVO_CABEZAL_H

#include <Arduino.h>
#include <ESP32Servo.h>

const int PIN_SERVO = 23;

// Tu centro real exacto calibrado (Frente = 85°)
const int POS_FRENTE    = 85; 
const int POS_DERECHA   = 30;  // 85 - 55 grados
const int POS_IZQUIERDA = 140; // 85 + 55 grados

static Servo servoSensor;
static int anguloActual = POS_FRENTE;

inline void moverSuave(int anguloDestino) {
  anguloDestino = constrain(anguloDestino, 20, 160);
  if (anguloDestino == anguloActual) return;

  int paso = (anguloDestino > anguloActual) ? 1 : -1;
  
  while (anguloActual != anguloDestino) {
    anguloActual += paso;
    servoSensor.write(anguloActual);
    delay(12); // Movimiento gradual grado a grado para limitar corriente
  }
  
  delay(120); // Asentamiento mecánico
}

inline void iniciarServo() {
  // Asignar todos los temporizadores para que ESP32Servo tome uno libre sin chocar con los motores
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  servoSensor.setPeriodHertz(50);

  // Pre-cargar el ángulo objetivo antes de enganchar el pin
  anguloActual = POS_FRENTE;
  servoSensor.write(POS_FRENTE);

  // Vincular pin
  servoSensor.attach(PIN_SERVO, 1000, 2000);

  // Reafirmar posición central al frente
  servoSensor.write(POS_FRENTE);
  delay(800); // Espera a que el mecanismo se alinee al frente
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
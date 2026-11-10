#ifndef SERVO_CABEZAL_H
#define SERVO_CABEZAL_H

#include <Arduino.h>
#include <ESP32Servo.h>

const int PIN_SERVO = 23;

// Tu frente real exacto (centro de referencia)
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
    delay(15); // Movimiento gradual grado a grado
  }
  
  delay(150); // Asentamiento mecánico
}

inline void iniciarServo() {
  ESP32PWM::allocateTimer(0);
  servoSensor.setPeriodHertz(50);

  // Vincular pin al temporizador
  servoSensor.attach(PIN_SERVO, 1000, 2000);

  // Escribir de inmediato el pulso de 85 grados
  anguloActual = POS_FRENTE;
  servoSensor.write(POS_FRENTE);
  
  // Tiempo de estabilización mecánica y eléctrica inicial
  delay(1000);
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
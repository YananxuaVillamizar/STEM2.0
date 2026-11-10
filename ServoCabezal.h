#ifndef SERVO_CABEZAL_H
#define SERVO_CABEZAL_H

#include <Arduino.h>
#include <ESP32Servo.h>

const int PIN_SERVO = 23;

// Tu frente real calibrado (centro)
const int POS_FRENTE    = 85; 
// Arco ampliado para mayor campo de visión
const int POS_DERECHA   = 20;  // 85 - 65 grados (visión lateral derecha amplia)
const int POS_IZQUIERDA = 150; // 85 + 65 grados (visión lateral izquierda amplia)

static Servo servoSensor;
static int anguloActual = POS_FRENTE;

inline void moverSuave(int anguloDestino) {
  anguloDestino = constrain(anguloDestino, 15, 165);
  if (anguloDestino == anguloActual) return;

  int paso = (anguloDestino > anguloActual) ? 1 : -1;
  
  while (anguloActual != anguloDestino) {
    anguloActual += paso;
    servoSensor.write(anguloActual);
    delay(14); // 14 ms por grado: movimiento continuo sin sobrecorrientes
  }
  
  delay(150); // Asentamiento mecánico
}

inline void iniciarServo() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  servoSensor.setPeriodHertz(50);

  // Forzar que el canal inicie directamente en el frente calibrado
  anguloActual = POS_FRENTE;
  servoSensor.write(POS_FRENTE);
  servoSensor.attach(PIN_SERVO, 1000, 2000);
  servoSensor.write(POS_FRENTE);
  
  delay(800); // Estabilización
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
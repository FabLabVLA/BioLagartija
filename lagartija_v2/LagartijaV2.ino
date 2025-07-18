#include <Arduino.h>
#include <ESP32Servo.h>

// Pines del receptor RF RX480-E-WQ
#define BUTTON_A_PIN 14  // D0
#define BUTTON_B_PIN 27  // D1
#define BUTTON_C_PIN 26  // D2
#define BUTTON_D_PIN 25  // D3

// Servos
Servo servo1; // Cabeza
Servo servo2; // Cola

// Ángulos y dirección
int angle1 = 90;
int angle2 = 90;
int direction1 = -1;
int direction2 = -1;

unsigned long previousMillis = 0;
unsigned long waitStartMillis = 0;
const int interval = 4;
const int waitDuration = 2500;

const int minAngle = 70;
const int maxAngle = 90;

// Estados para movimiento individual
int moveCount1 = 0;
int moveCount2 = 0;
bool isServo1Active = false;
bool isServo2Active = false;
bool isWaiting = false;

// 🔁 Modo alternado infinito (botón C)
bool modoAlternado = false;
bool moverCabezaAhora = true; // para alternar entre cabeza y cola

void setup() {
  Serial.begin(115200);

  // Entradas del módulo RF
  pinMode(BUTTON_A_PIN, INPUT);
  pinMode(BUTTON_B_PIN, INPUT);
  pinMode(BUTTON_C_PIN, INPUT);
  pinMode(BUTTON_D_PIN, INPUT);

  // Conectar servos
  servo1.attach(2);
  servo2.attach(4);

  // Posición inicial
  servo1.write(angle1);
  servo2.write(angle2);
}

void loop() {
  unsigned long currentMillis = millis();

  // Leer botones
  bool buttonA = digitalRead(BUTTON_A_PIN);
  bool buttonB = digitalRead(BUTTON_B_PIN);
  bool buttonC = digitalRead(BUTTON_C_PIN);
  bool buttonD = digitalRead(BUTTON_D_PIN);

  // --- Botón A: solo cabeza
  if (buttonA) {
    modoAlternado = false;
    isServo1Active = true;
    isServo2Active = false;
    isWaiting = false;
    moveCount1 = 0;
    Serial.println("🟢 Activado solo Cabeza");
    delay(500);
  }

  // --- Botón B: solo cola
  if (buttonB) {
    modoAlternado = false;
    isServo1Active = false;
    isServo2Active = true;
    isWaiting = false;
    moveCount2 = 0;
    Serial.println("🟢 Activado solo Cola");
    delay(500);
  }

  // --- Botón C: modo alternado 10 ciclos ↔ 10 ciclos
  if (buttonC) {
    modoAlternado = true;
    moverCabezaAhora = true;
    isServo1Active = true;
    isServo2Active = false;
    isWaiting = false;
    moveCount1 = 0;
    moveCount2 = 0;
    Serial.println("🔁 Activado Cabeza 10 ciclos ➜ Cola 10 ciclos ➜ loop...");
    delay(500);
  }

  // --- Botón D: detener todo
  if (buttonD) {
    modoAlternado = false;
    isServo1Active = false;
    isServo2Active = false;
    isWaiting = false;
    Serial.println("🔴 Movimiento detenido");
    delay(500);
  }

  // --- Espera entre bloques de movimiento
  if (isWaiting) {
    if (currentMillis - waitStartMillis >= waitDuration) {
      isWaiting = false;

      if (modoAlternado) {
        moverCabezaAhora = !moverCabezaAhora;
        isServo1Active = moverCabezaAhora;
        isServo2Active = !moverCabezaAhora;
        moveCount1 = 0;
        moveCount2 = 0;
      }
    }
    return;
  }

  // --- Movimiento de servos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // 🔁 Modo alternado (C)
    if (modoAlternado) {
      if (isServo1Active) {
        angle1 += direction1;
        if (angle1 <= minAngle || angle1 >= maxAngle) {
          direction1 *= -1;
          moveCount1++;
        }
        servo1.write(angle1);

        if (moveCount1 >= 10) {
          isWaiting = true;
          waitStartMillis = currentMillis;
        }
      }

      if (isServo2Active) {
        angle2 += direction2;
        if (angle2 <= minAngle || angle2 >= maxAngle) {
          direction2 *= -1;
          moveCount2++;
        }
        servo2.write(angle2);

        if (moveCount2 >= 10) {
          isWaiting = true;
          waitStartMillis = currentMillis;
        }
      }

    } else {
      // Modos A y B normales
      if (isServo1Active) {
        angle1 += direction1;
        if (angle1 <= minAngle || angle1 >= maxAngle) {
          direction1 *= -1;
          moveCount1++;
        }
        servo1.write(angle1);

        if (moveCount1 >= 10) {
          isWaiting = true;
          waitStartMillis = currentMillis;
          moveCount1 = 0;
        }
      }

      if (isServo2Active) {
        angle2 += direction2;
        if (angle2 <= minAngle || angle2 >= maxAngle) {
          direction2 *= -1;
          moveCount2++;
        }
        servo2.write(angle2);

        if (moveCount2 >= 10) {
          isWaiting = true;
          waitStartMillis = currentMillis;
          moveCount2 = 0;
        }
      }
    }
  }
}

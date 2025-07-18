#include <Arduino.h>
#include <ESP32Servo.h>


Servo servo1; // Objeto para el servo motor 1
Servo servo2; // Objeto para el servo motor 2


int angle1 = 90; // Ángulo actual del servo 1
int angle2 = 90; // Ángulo actual del servo 2


int direction1 = -1; // Dirección del movimiento del servo 1 (-1 = izquierda, 1 = derecha)
int direction2 = -1; // Dirección del movimiento del servo 2 (-1 = izquierda, 1 = derecha)


unsigned long previousMillis = 0;  // Almacena el último tiempo registrado
unsigned long waitStartMillis = 0; // Almacena el tiempo al iniciar la espera
const int interval = 5;            // Intervalo de tiempo para ambos servos (en ms)
const int waitDuration = 2500;     // Duración de la espera (10 segundos en ms)


const int minAngle = 70; // Ángulo mínimo para los servos
const int maxAngle = 90; // Ángulo máximo para los servos


int moveCount1 = 0; // Contador de movimientos para el servo1
int moveCount2 = 0; // Contador de movimientos para el servo2


bool isServo1Active = true; // Indica si el servo1 está activo (controlando)
bool isWaiting = false;     // Indica si se está realizando la espera de 10 segundos


void setup()
{
  servo1.attach(2); // Conecta el servo motor 1 al pin 9
  servo2.attach(4); // Conecta el servo motor 2 al pin 10


  // Inicializar ambos servos en la posición central
  servo1.write(angle1);
  servo2.write(angle2);
}


void loop()
{
  unsigned long currentMillis = millis(); // Obtener el tiempo actual


  if (isWaiting)
  {
    // Comprobar si la espera de 10 segundos ha finalizado
    if (currentMillis - waitStartMillis >= waitDuration)
    {
      isWaiting = false;                // Finalizar la espera
      isServo1Active = !isServo1Active; // Cambiar al otro servo
    }
    return; // Detener ejecución hasta que termine la espera
  }


  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis; // Actualizar el último tiempo


    if (isServo1Active)
    {
      // Control del movimiento del servo 1
      angle1 += direction1;
      if (angle1 <= minAngle || angle1 >= maxAngle)
      {
        direction1 *= -1; // Cambiar dirección si se alcanza el límite
        moveCount1++;     // Incrementar el contador de movimientos del servo 1
      }
      servo1.write(angle1); // Mover el servo 1 al nuevo ángulo


      // Cambiar al estado de espera después de 5 movimientos completos
      if (moveCount1 >= 10)
      {
        moveCount1 = 0;                  // Reiniciar el contador del servo 1
        isWaiting = true;                // Activar la espera
        waitStartMillis = currentMillis; // Registrar el tiempo de inicio de la espera
      }
    }
    else
    {
      // Control del movimiento del servo 2
      angle2 += direction2;
      if (angle2 <= minAngle || angle2 >= maxAngle)
      {
        direction2 *= -1; // Cambiar dirección si se alcanza el límite
        moveCount2++;     // Incrementar el contador de movimientos del servo 2
      }
      servo2.write(angle2); // Mover el servo 2 al nuevo ángulo


      // Cambiar al estado de espera después de 5 movimientos completos
      if (moveCount2 >= 10)
      {
        moveCount2 = 0;                  // Reiniciar el contador del servo 2
        isWaiting = true;                // Activar la espera
        waitStartMillis = currentMillis; // Registrar el tiempo de inicio de la espera
      }
    }
  }
}

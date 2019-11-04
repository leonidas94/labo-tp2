#include <TimerOne.h> // Libreria para usar el objeto Timer1

#define PIN_PWM 5
#define PIN_INTERRUPT 2

// Defino variables globales:

int cont; // Contador de pulsos
float Tw=0.1; // Tiempo de ventana
int vel=0; // Velocidad del motor
int vel_anterior=0;
int ppv=20; // Pulsos por vuelta
float Kp=0;
float Kd=0;
float Ki=0;
int N=10;
float Pk, Ik, Dk;
float uk;
int pwm_;
int ref;
bool interrupt_on=false;

void setup() 
{
  pinMode(PIN_PWM,OUTPUT); // Seteo el pin 5 como salida PWM
}

void loop() 
{
  analogWrite(PIN_PWM,255); // Ajusta el valor del PWM
}

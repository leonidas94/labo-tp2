#include <TimerOne.h> // Libreria para usar el objeto Timer1

#define PIN_PWM 5
#define PIN_INTERRUPT 2

// Defino variables globales:

int cont; // Contador de pulsos
float Tw=0.1; // Tiempo de ventana
int vel=100; // Velocidad del motor
int vel_anterior=100;
int ppv=20; // Pulsos por vuelta
uint8_t Kp=1;
uint8_t Kd=1;
uint8_t Ki=1;
uint8_t N=10;
float Pk, Ik=0, Dk;
float uk;
uint8_t pwm_;
int ref=900;
bool interrupt_on=false;

void setup() 
{
  Serial.begin(115200);
  pinMode(PIN_PWM,OUTPUT); // Seteo el pin 5 como salida PWM
  pinMode(PIN_INTERRUPT,INPUT); // Seteo el pin 2 para interrupciones
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT),contador_pulsos,CHANGE); // Configuro la interrupción del pin 2 para que cuente los pulsos
                                                                                // CHANGE to trigger the interrupt whenever the pin changes value
  Timer1.initialize(Tw*1e6); // Inicializo timer. Esta función toma us como argumento
  Timer1.attachInterrupt(calcular_velocidad); // Activo la interrupción y la asocio a calcular_velocidad
}

void loop() 
{
  if(interrupt_on==true)
  {
    Serial.print("Speed = ");
    Serial.println(vel); 
  }
  analogWrite(PIN_PWM,255); // Ajusta el valor del PWM
}

void contador_pulsos() // Función que cuenta los pulsos cuando salta la interrupción
{ 
  cont=cont+1;
}

void calcular_velocidad(){
  vel=(60*cont)/(Tw*ppv); // Hago la conversión a RPM
  cont=0; // Inicializo el contador
  interrupt_on=true;
}

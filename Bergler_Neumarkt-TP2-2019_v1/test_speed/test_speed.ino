#include <TimerOne.h> // Libreria para usar el objeto Timer1

#define PIN_PWM 5
#define PIN_INTERRUPT 2

// Defino variables globales:

int cont; // Contador de pulsos
float Tw=0.01; // Tiempo de ventana
float vel=0;
int ppv=32; // Pulsos por vuelta
bool interrupt_on=false;

typedef union
{
  float number;
  byte bytes[4];
} FLOATUNION_t;


void setup() 
{
  Serial.begin(9600);
  pinMode(PIN_PWM,OUTPUT); // Seteo el pin 5 como salida PWM
  pinMode(PIN_INTERRUPT,INPUT); // Seteo el pin 2 para interrupciones
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT),contador_pulsos,RISING); // Configuro la interrupción del pin 2 para que cuente los pulsos
                                                                                // CHANGE to trigger the interrupt whenever the pin changes value
  Timer1.initialize(Tw*1e6); // Inicializo timer. Esta función toma us como argumento
  Timer1.attachInterrupt(calcular_velocidad); // Activo la interrupción y la asocio a calcular_velocidad
}

void loop() 
{
  if(interrupt_on==true)
  {
    enviar_trama();
  }
  analogWrite(PIN_PWM,255); // Ajusta el valor del PWM
//  Serial.println(vel);
//  delay(Tw);
}

void contador_pulsos() // Función que cuenta los pulsos cuando salta la interrupción
{ 
  cont=cont+1;
}

void calcular_velocidad()
{
  vel=(60*cont)/(Tw*ppv); // Hago la conversión a RPM
  cont=0; // Inicializo el contador
  interrupt_on=true;
}

void enviar_trama()
{
  FLOATUNION_t vel_aux;
  vel_aux.number=vel;
  
  Serial.write(vel_aux.bytes, 4);
  Serial.flush(); // Espera a que la transmision serial de datos se complete.
}

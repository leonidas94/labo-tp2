#include "Timer.h"

#define PIN_PWM 5    // Pin de salida del PWM
#define PIN_INTERRUPT 2 //Tiene que ser el pin 2 o 3 porque es una interrupcion
#define CANT_RANURAS 32 //La cantidad de RANURAS del encoder
#define TW 0.2

Timer tiempo;

typedef union
{
  float number;
  byte bytes[4];
} FLOATUNION_t;

int cont=0;
float vel = 1, vel_anterior = 1;
float Kp=1, Kd=0, Ki=0; 
int N=10; 
float Pk=0, Dk=0, Ik=0, uk=0;
int pwm_=0;
volatile bool interrupt_on = false;
int ref=0; //Referencia del controlador


void setup() 
{
  pinMode(PIN_INTERRUPT, INPUT_PULLUP);
  pinMode(PIN_PWM, OUTPUT); 
  Serial.begin(9600);
  
  // Indico que cada vez que se detecte un flanco ascendente por el pin PIN_INTERRUPT,
  // entre a la función contador_pulsos(), la cuál suma un 1 a la variable cont:
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), contador_pulsos, RISING); 

  // Indico que cada TW*1000=200ms se realice una interrupcion que llame a la
  // función calcular_velocidad():
  tiempo.every(TW*1000,calcular_velocidad); 
}


void loop() 
{    
  tiempo.update(); // Se llama a update para corroborrar si el intervalo de tiempo especificado termino
                   // y en caso afirmativo se llama a la función calcular_velocidad()
  if (Serial.available()>0)
  {
    recibir_trama();
  }

  if (interrupt_on==true)
  {
    interrupt_on = false;
    pid();
    enviar_trama();
  }
  
  analogWrite(PIN_PWM, pwm_); // Envio el valor del pwm al pin correspondiente
}


void contador_pulsos()
{
  cont++;
}


void calcular_velocidad()
{
  interrupt_on = true; 
  vel = ((float)cont*60)/(CANT_RANURAS*TW);
  cont = 0; 
}


void recibir_trama() // PC al Arduino
{
  char header[4]="abcd";
  char header_recibido[4];
  FLOATUNION_t Kp_aux, Kd_aux, Ki_aux, ref_aux, N_aux;
  
  Serial.readBytes(header_recibido,4); // Leo el header para saber si comenzo una nueva trama
  // Comparo el header con los datos recibidos:
  if(header[0]==header_recibido[0]&&header[1]==header_recibido[1]&&header[2]==header_recibido[2]&&header[3]==header_recibido[3])
  {
    // Leo cada variable recibida
    Serial.readBytes(Kp_aux.bytes,4);
    Serial.readBytes(Kd_aux.bytes,4);
    Serial.readBytes(Ki_aux.bytes,4);
    Serial.readBytes(ref_aux.bytes,4);
    Serial.readBytes(N_aux.bytes,4);
  }

  // Reasigno los valores de las variables  
  Kp=Kp_aux.number;
  Kd=Kd_aux.number;
  Ki=Ki_aux.number;
  ref=int(ref_aux.number);
  if(ref>2400) // Ajusto a este valor la referencia, ya que el motor no alcanza valores mayores
    ref=2400;
  else if(ref<0)
    ref==0;

  N=int(N_aux.bytes);
}


void enviar_trama() // Arduino a la PC
{
  // Convierto las variables pwm_ y ref a tipo float, para realizar
  // operaciones con otras variables de este tipo.
  float pwm_float=float(pwm_)*100/255; 
  float ref_float=float(ref);
  
  FLOATUNION_t Kp_aux, Kd_aux, Ki_aux, ref_aux, pwm_aux, vel_aux;
  char header[4]="efgh";
  
  Kp_aux.number=Kp;
  Kd_aux.number=Kd;
  Ki_aux.number=Ki;
  ref_aux.number=ref_float;
  pwm_aux.number=pwm_float;
  vel_aux.number=vel;
  
  Serial.write(header,4);
  Serial.write(Kp_aux.bytes, 4);
  Serial.write(Kd_aux.bytes, 4);
  Serial.write(Ki_aux.bytes, 4);
  Serial.write(ref_aux.bytes, 4);
  Serial.write(pwm_aux.bytes, 4);
  Serial.write(vel_aux.bytes, 4);
  Serial.flush(); // Espera a que la transmisión serial de datos este se compelte.
}


void pid()
{
  float gamma = Kd/N;
  Pk = Kp*(ref - vel);
  Dk = (gamma/(gamma + TW))*Dk - (Kp*Kd/(gamma + TW))*(vel-vel_anterior);
  uk = Pk + Ik + Dk;
  if (uk>1)
    uk = 1;
  else if (uk<0)
    uk = 0;

  Ik = Kp*Ki*TW*(ref - vel) + Ik;
  vel_anterior = vel;
  pwm_ = 255*uk;
}

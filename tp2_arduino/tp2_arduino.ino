#include <TimerOne.h> // Libreria para usar el objeto Timer1

#define PIN_PWM 5
#define PIN_INTERRUPT 2

//#####################
typedef union
{
  float number;
  byte bytes[4];
} FLOATUNION_t;
//#####################
// Defino variables globales:

int cont=0; // Contador de pulsos
float Tw=0.2; // Tiempo de ventana
int vel=100; // Velocidad del motor
int vel_anterior=100;
int ppv=32; // Pulsos por vuelta. Cantidad de ranuras
uint8_t Kp=1;
uint8_t Kd=1;
uint8_t Ki=1;
uint8_t N=10;
float Pk, Ik=0, Dk;
int uk;
uint8_t pwm_;
int ref=0;
volatile bool interrupt_on=false;

void setup() 
{
  Serial.begin(9600);
  pinMode(PIN_PWM,OUTPUT); // Seteo el pin 5 como salida PWM
  pinMode(PIN_INTERRUPT,INPUT); // Seteo el pin 2 para interrupciones
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT),contador_pulsos,RISING); // Configuro la interrupción del pin 2 para que cuente los pulsos
  Timer1.initialize(Tw*1e6); // Inicializo timer. Esta función toma us como argumento
  Timer1.attachInterrupt(calcular_velocidad); // Activo la interrupción y la asocio a calcular_velocidad
}

void loop() 
{
  if(Serial.available() > 0) // send data only when you receive data.
  {
    recibir_trama();
  }
  if(interrupt_on==true)
  {
      interrupt_on=false;
      pwm_=pid();
      enviar_trama();
  } 
  analogWrite(PIN_PWM,pwm_); // Ajusta el valor del PWM
}

void contador_pulsos() // Función que cuenta los pulsos cuando salta la interrupción
{ 
  cont=cont+1;
}

void calcular_velocidad(){
  vel=(1000*60*cont)/(Tw*ppv); // Hago la conversión a RPM
  cont=0; // Inicializo el contador
  interrupt_on=true;
}

void recibir_trama() // Envio de datos de la PC al Arduino
{
  char h_trama[4];
  char header_recibido[4];
  char datos_recibidos[6];
  int ref_high, ref_low;
  
  h_trama[0]='a';
  h_trama[1]='b';
  h_trama[2]='c';
  h_trama[3]='d';

 
  Serial.readBytes(header_recibido,4);

  // Me fijo si empezo una nueva trama
  if(h_trama[0]==header_recibido[0] && h_trama[1]==header_recibido[1] && h_trama[2]==header_recibido[2] && h_trama[3]==header_recibido[3])
  {
    Serial.readBytes(datos_recibidos,6);
    Kp=datos_recibidos[0];
    Kd=datos_recibidos[1];
    Ki=datos_recibidos[2];
    ref_high=datos_recibidos[3];
    ref_low=datos_recibidos[4];
    
    N=datos_recibidos[5];
  }
}

void enviar_trama() // Envio de datos del Arduino a la PC
{ 
  
  byte trama[12];
  
  trama[0]='e';
  trama[1]='f';
  trama[2]='g';
  trama[3]='h';
  trama[4]=Kp;
  trama[5]=Kd;
  trama[6]=Ki;
  trama[7]=highByte(ref);
  trama[8]=lowByte(ref);
  trama[9]=pwm_;
  trama[10]=highByte(vel);
  trama[11]=lowByte(vel);  

  //Serial.write(trama,12);
  Serial.write((uint8_t*) trama, sizeof(trama));
}

int pid()
{
  float gamma=Kd/N;  
  
  Pk = Kp*(ref-vel);
 
  Dk =  (gamma/gamma+Tw)*Dk-(Kp*Kd)/(gamma+Tw)*(vel-vel_anterior);
  vel_anterior=vel;
  uk=Pk+Ik+Dk;
  
  if(uk>255)
    uk=255;
  else if(uk<0)
    uk=0;
    
  Ik = Ik+Kp*Ki*Tw*(ref-vel); 
  
  return uk;
  
}

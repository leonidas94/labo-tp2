#include <TimerOne.h> // Libreria para usar el objeto Timer1

#define PIN_PWM 5
#define PIN_INTERRUPT
// Defino variables globales:

int cont; // Contador de pulsos
float Tw=0.1; // Tiempo de ventana
float vel=0; // Velocidad del motor
float vel_anterior=0;
int ppv=20 // Pulsos por vuelta
float Kp=0;
float Kd=0;
float Ki=0;
int N=10;
float Pk, Ik, Dk;
float uk;
bool interrupt_on=false;

void setup() 
{
  Serial.begin(115200);
  pinMode(PIN_PWM,OUTPUT); // Seteo el pin 5 como salida PWM
  pinMode(PIN_INTERRUPT,INPUT); // Seteo el pin 2 para interrupciones
 // digitalWrite(2,HIGH); // Escribe un valor alto (5V) en el pin 2
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT),contador_pulsos,CHANGE); // Configuro la interrupción del pin 2 para que cuente los pulsos
  Timer1.initialize(Tw*1e6); // Esta función toma us como argumento
  Timer1.attachInterrupt(calcular_velocidad); // Activo la interrupción y la asocio a calcular_velocidad
}

void loop() 
{
  if(Serial.available() > 0)
  {
    recibir_trama();
  }
  if(interrupt_on==true)
  {
    interrupt_on=false;
    pwm=pid();
    enviar_trama();
  }
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

void recibir_trama() // Envio de datos de la PC al Arduino
{
  char h_trama[4];
  char header_recibido[4];
  char datos_recibidos[6];
  
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
  
  trama[4]=Kp;
  trama[5]=Kd;
  trama[6]=Ki;
  trama[7]=highByte(ref);
  trama[8]=lowByte(ref);
  trama[9]=N;    
}

void enviar_trama() // Envio de datos del Arduino a la PC
{ 
  
  float trama[12];
  
  trama[0]='e';
  trama[1]='f';
  trama[2]='g';
  trama[3]='h';
  trama[4]=Kp;
  trama[5]=Kd;
  trama[6]=Ki;
  trama[7]=highByte(ref);
  trama[8]=lowByte(ref);
  trama[9]=pwm;
  trama[10]=highByte(vel);
  trama[11]=lowByte(vel);  

  Serial.write(trama,12);
}

float pid()
{
  float gamma=Kd/N;  
  
  Pk = Kp*(ref-vel);
  //Ik = Ik+Kp*Ki*Tw(ref-vel); // HAY QUE INCLUIR ESTO?? PORQUE EN REALIDAD ES Ik+1=Ik+Kp*Ki*Tw(ref-vel), pero no podemos obtener datos futuros
  Ik = 0;
  Dk =  (gamma/gamma+Tw)*Dk-(Kp*Kd)/(gamma+Tw)*(vel-vel_anterior);
  vel_anterior=vel;
  uk=Pk+Ik+Dk;

  return uk;
  
}


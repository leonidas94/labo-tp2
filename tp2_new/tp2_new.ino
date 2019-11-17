#include <TimerOne.h> // Libreria para usar el objeto Timer1

#define PIN_PWM 9
#define PIN_INTERRUPT 3

typedef union
{
  float number;
  byte bytes[4];
} FLOATUNION_t;

// Defino variables globales:

int cont=0; // Contador de pulsos
float Tw=0.2; // Tiempo de ventana
float vel=100; // Velocidad del motor
float vel_anterior=100;
int ppv=32; // Pulsos por vuelta. Cantidad de ranuras
float Kp=0;
float Kd=0;
float Ki=0;
int N=5;
float Pk, Ik=0, Dk=0;
float uk;
int pwm_;
int ref=0;
volatile bool interrupt_on=false;

void setup() 
{
  Serial.begin(9600);
  pinMode(PIN_PWM,OUTPUT); // Seteo el pin 5 como salida PWM
  //pinMode(PIN_INTERRUPT,INPUT); // Seteo el pin 2 para interrupciones
  pinMode(PIN_INTERRUPT,INPUT_PULLUP); // Seteo el pin 2 para interrupciones
  
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT),contador_pulsos,RISING); // Configuro la interrupción del pin 2 para que cuente los pulsos
  Timer1.initialize(Tw*1e6); // Inicializo timer. Esta función toma us como argumento
  Timer1.attachInterrupt(calcular_velocidad); // Activo la interrupción y la asocio a calcular_velocidad
}

void loop() 
{
  if(Serial.available() > 0) // send data only when you receive data.
  {
    recibir_trama();
    Serial.write("...");
  }
  if(interrupt_on==true)
  {
      interrupt_on=false;
      pid();
      enviar_trama();
  } 
  analogWrite(PIN_PWM,pwm_); // Ajusta el valor del PWM
}

void contador_pulsos() // Función que cuenta los pulsos cuando salta la interrupción
{ 
  cont=cont+1;
}

void calcular_velocidad(){
  vel=((float)cont*1000*60)/(Tw*ppv); // Hago la conversión a RPM
  cont=0; // Inicializo el contador
  interrupt_on=true;
}

void recibir_trama() // Envio de datos de la PC al Arduino
{
  char h_trama[4]="abcd"; 
  char header_recibido[4];
  
  char datos_recibidos[6];
  int ref_high, ref_low;

  FLOATUNION_t Kp_;
  FLOATUNION_t Kd_;
  FLOATUNION_t Ki_;
  FLOATUNION_t ref_;
  FLOATUNION_t N_;
       
  Serial.readBytes(header_recibido,4);

  // Me fijo si empezo una nueva trama
  if(h_trama[0]==header_recibido[0] && h_trama[1]==header_recibido[1] && h_trama[2]==header_recibido[2] && h_trama[3]==header_recibido[3])
  {
    Serial.readBytes(Kp_.bytes,4);
    Serial.readBytes(Kd_.bytes,4);
    Serial.readBytes(Ki_.bytes,4); 
    Serial.readBytes(ref_.bytes,4);
    Serial.readBytes(N_.bytes,4);           
  }

  Kp=Kp_.number;
  Kd=Kd_.number;
  Ki=Ki_.number;
  ref=int(ref_.number);
  N=int(N_.bytes);      
}

void enviar_trama() // Envio de datos del Arduino a la PC
{ 
  float pwm_aux=float(pwm_)*100/255; // Porcentaje de duty cycle
  float ref_aux=float(ref);

  FLOATUNION_t Kp_;
  FLOATUNION_t Kd_;
  FLOATUNION_t Ki_;
  FLOATUNION_t ref_;
  FLOATUNION_t pwm__;
  FLOATUNION_t vel_;

  Kp_.number=Kp;
  Kd_.number=Kd;
  Ki_.number=Ki;
  ref_.number=ref_aux;
  pwm__.number=pwm_aux;
  vel_.number=vel;
  
  Serial.write("efgh");
  Serial.write(Kp_.bytes,4);
  Serial.write(Kd_.bytes,4);
  Serial.write(Ki_.bytes,4);
  Serial.write(ref_.bytes,4);                                     
  Serial.write(pwm__.bytes,4);
  Serial.write(vel_.bytes,4);
  Serial.flush(); // Borra buffer
}

void pid()
{
  float gamma=Kd/N;  
  
  Pk = Kp*(ref-vel);
 
 
  Dk = (gamma/(gamma + Tw))*Dk - (Kp*Kd/(gamma + Tw))*(vel-vel_anterior);
  vel_anterior=vel;
  uk=Pk+Ik+Dk;
  
  if(uk>1)
    uk=1;
  else if(uk<0)
    uk=0;
    
  Ik = Ik+Kp*Ki*Tw*(ref-vel); 
/*  Serial.write("Pk: ");
  Serial.println(Pk);
  Serial.write("Dk: ");
  Serial.println(Dk);
  Serial.write("Ik: ");
  Serial.println(Ik);
  Serial.println(uk*100/255);
  */

  pwm_=uk*255; 
}

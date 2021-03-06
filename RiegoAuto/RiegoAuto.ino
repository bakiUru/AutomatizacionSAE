/***************************************
****************************************
Autor: Marcelo Rodriguez
Fecha: 18/01/2022
Version: 1.0 
Objetivo: Se busca realizar el ON/OFF Bomba centrifuga -- Calculo de Caudal para Riego estander -- ON/OFF Solenoide -- Uso de Sensor de LLuvia -- Sensor de Tierra -- Control de fechas con RTC-- 
***************************************
***************************************/

#include <ThreeWire.h> 
#include <RtcDS1302.h>
#include "Tiempo.h"
#include <Time.h>
#include <TimeLib.h>

//Define Pines Digitales
//solenoides
#define sec1 1 
#define caudalRiego 2 
//Define Pines Analogicos
#define sensLLuvia A0
#define sensTanque A1
#define sensPresion A2


// TIEMPO FECHA
ThreeWire myWire(4,5,2); //DAT-CLK-RST pines de conexion reloj
RtcDS1302<ThreeWire> Rtc(myWire);

time_t proxRiego, hora_Riego;

//Variables de ingreso por el teclado 
bool proxriegoSet = false;

int op = "s";
int dRIEGO, mRIEGO;
boolean riegoManual = false;
/******************************************************/
/******************************************************/
//Solucion alternativa al problema de la creacion de la nueva fecha del proximo riego
uint8_t diaRiego, horaRiego;
/******************************************************/
/******************************************************/

/*/HORARIO DE RIego primera Hora de la Mañana. Se puede implementar un sensor fotosensible para captar la luz del sol
o generar tres variables constantes que guarden la hora del amanecer segun las distintas estaciones  
*/
//Prueba con constantes de estacion solo dos
const byte h_Invierno = 7;
const byte h_Verano = 5;

//Frecuencia de Riego Variable por el Cliente 
uint8_t frecRiego = 1;

//Sensor de Lluvia
uint16_t valorLLuvia = 0;
//sensor de Tanque
uint16_t valorTanque= 0;

//Variables para tiempo de datos
unsigned long tiempoStart = 0;
unsigned long timeLLuvia = 0;


//Nuevo objeto Reloj
RtcDateTime myRTC;


//Cabezales de Funciones y Procedimientos main 
bool SalDigital_ON (uint8_t);
bool SalDigital_OFF (uint8_t);

char buffer_string[7];
void setup(){
    Serial.begin(57600);
//Mostramos Fecha del sistema  
    Serial.print("Fecha de Sistema: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    //Captura la Hora del PC
    myRTC=RtcDateTime(__DATE__, __TIME__);
    
    Rtc.Begin();

 //************CONTROLES DEL RELOJ************//
    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("No encuentro una Fecha CORRECTA!");
        //seteamos de nuevo el reloj
        Rtc.SetDateTime(myRTC);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC protegido para escritura, Se desbloquea Reloj ");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("Reloj en Funcionamiento");
        Rtc.SetIsRunning(true);
    }
    
    RtcDateTime now = Rtc.GetDateTime();
    if (now < myRTC) 
    {
        Serial.println("Necesario Actualizar la Hora del Reloj)");
        Rtc.SetDateTime(myRTC);
    }
    else if (now > myRTC) 
    {
        Serial.println("La hora es Correcta");
    }
    else if (now == myRTC) 
    {
        Serial.println("Está Bien Configurado el Reloj");
    }
//************FIN CONTROLES RELOJ************//
//Configuracion Caudal 
//setup_Caudal();

//Configuracion de Pines


//capturo tiempo del microProcesador
tiempoStart = millis();

//PruebaSETEOdeHORA   MANUAL
do
{
    Serial.flush();
    Serial.read();
       delay(1000);
      
  //Riego Manual?
  int ingresoManual;
    Serial.print("Ingreso Manual -> (si = 1 / no = 0)");
    while (Serial.available() == 0)
     //ESPERO LECTURA//
    {                Serial.read();             }
    if(Serial.available() > 0 )
    {
        ingresoManual = Serial.read();
        Serial.println(ingresoManual);

    }
        
    if (ingresoManual == 48)
    {
      riegoManual = false;
      op ='s';
      proxriegoSet = true;
    }
    else{
      riegoManual = true;  
      String diaR;
      String mesR;
      delay(1000);
      Serial.print("Ingrese Dia -> ");
      while (Serial.available() == 0)
       //ESPERO LECTURA//
      { 
        if(Serial.available() == 0 )           
          Serial.read(); 
       
                    
      }
      delay(1000);
      if(Serial.available() > 0 )
          diaR = Serial.readStringUntil('\n');
      Serial.println("Ingrese Mes -> ");
      while (Serial.available() == 0)
      //ESPERO LECTURA//
      {            Serial.read();                 }
         if(Serial.available() > 0)
          mesR = Serial.readStringUntil('\n');    
  
      Serial.println("Dia - Ingresado: "+String(diaR));
      Serial.println("MES - Ingresado: "+String(mesR));
      Serial.println("Es correcto el Proximo Riego? (s/n) : ");
      while (Serial.available() == 0)
      //ESPERO LECTURA//
      {                             }
      if (Serial.available() > 0)
           op = Serial.read();
      
      if (op == 115)
      {
        dRIEGO = diaR.toInt();
        mRIEGO = mesR.toInt();
        Serial.println(op);
        proxriegoSet = true;
      }
      else{
        proxriegoSet = false;
      }
      
    }
}while (op == 'n' && proxriegoSet == false);



//setTime(myRTC.Hour(),myRTC.Minute(),myRTC.Second(),myRTC.Month(),myRTC.Day(),myRTC.Year()); //Hora seteada a mano sin RTC 
hora_Riego = SetFechaCompleta(myRTC.Year(),myRTC.Month(),myRTC.Day(),06,00,00);


Serial.flush();
}

void loop(){
   unsigned int estacionActual;
   unsigned int mesActual = myRTC.Month();


   bool Riego= false, llueve = false, tanque = false;
   int aguaDiaria=5, aguaTot=0;

 
if (proxriegoSet == true)
{


Serial.println("------- DATOS FECHA Reloj -------");   
Serial.println("Dia: "+String(myRTC.Day()));
Serial.println("MES: "+String(myRTC.Month()));
Serial.println("Dia: "+String(myRTC.Year()));

//Switch control de estacion 
    switch(estacionActual){
        case (0):  
            if(hora_Riego >= h_Invierno)
                Serial.println("Regando en invierno a las "+String(h_Invierno)+"AM" );
                Riego = false;
        break;
        case (1):  
            if(hora_Riego >= h_Verano)
                Serial.println("Regando en Verano a las "+String(h_Verano)+" AM");
                Riego = false;
        break;
        default:
            Serial.println("No encontramos la hora del amanecer :(");

    }    
delay(5000);
        



Serial.println("----------------------------- DATOS SENSORES PURUEBAS -----------------------------");
Serial.println("--------------------------------------------------------------------------------------------------------");

//Comprobamos el Estado del sensor de TANQUE
valorTanque = analogRead(sensTanque);
Serial.println("Sensor de Tanque:" + String(valorTanque));
delay(1000);

//Comprobamos el Estado del sensor de LLUVIA
valorLLuvia = analogRead(sensLLuvia);



Serial.println("Sensor de LLuvia:" + String(valorLLuvia));
Serial.println("--------------------------------------------------------------------------------------------------------");
if(valorLLuvia > 500)
{
 llueve = false;
 Serial.println("No llueve, se puede Regar");
}else
    {
        llueve = true;
        Serial.println("Lloviendo! ");
    }

delay(5000);



//Asignacion de estacion 
    if(10 < mesActual > 4 )
       estacionActual = 0;
    else
        estacionActual = 1;

Mostrar_Hora(hora_Riego);

//CONTROL DE RIEGO
 if(myRTC.Day()== dRIEGO)
 {
    Serial.println("Se Riega :) es el Dia");
    Riego = true;   
 }else{
    Riego = false; 
    Serial.println("No Se Riega :( --- Mañana ansioso");
 
 }



 if ( Riego == false)
 {
     //FUNCION LLUVIA
    if(valorLLuvia > 500)
    {
    llueve = false;
    Serial.println("No llueve, se puede Regar");
    }else
        {
            llueve = true;
            Serial.println("Lloviendo! ");
        }
    //FUNCION TANQUE
    if(valorTanque > 200)
    {
    tanque = true;
    Serial.println("Tanque en nivel Optimo, se puede Regar");
    }else
        {
            tanque = false;
            Serial.println("Tanque Vacio! ");
        }

  Serial.println("Regando...");
  delay(1000);
    while(aguaTot <=aguaDiaria && (llueve == false && tanque == true)){
      aguaTot ++;
      Serial.println("Regando "+ String(aguaTot) + " lts.");
      delay(1500);
      valorLLuvia = analogRead(sensLLuvia);
        if (valorLLuvia < 500)
        {
            Serial.println("LLueve:"+ String(valorLLuvia));
            llueve = true;
        }
      
    }
  Serial.println("Se Regaron:  "+ String(aguaTot) + " lts.");
  if (riegoManual == false)
  {
    //cada vez que riego sumo un dia para hacerlo periodico // tambien podemos añadir frecuencia de riego // colocar despues
    dRIEGO = myRTC.Day() + 1 ;
    Serial.println("El Proximo Riego Automatico sera el dia: "+ String(dRIEGO) + " del " + String(mRIEGO));
  }
  else
    Serial.println("El Proximo Riego esta Programado Manualmente para "+ String(dRIEGO) + " de " + String(mRIEGO));
  

  Riego = true;
  
 }


}
 
}

//Funciones MAIN

//Funcion que recibe valor de ping y Prende ping especifico
bool SalDigital_ON (uint8_t pin){
   digitalWrite(pin, HIGH);   
   return true;
    }

//Funcion que recibe valor de ping y Apaga ping especifico
bool SalDigital_OFF (uint8_t pin){
   digitalWrite(pin, LOW);   
   return true;
    }

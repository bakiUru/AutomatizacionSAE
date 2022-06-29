#include "CalculoRiego.h"

void setup_Caudal ()
{
  pinMode(Caudal, INPUT);
  attachInterrupt(digitalPinToInterrupt(Caudal), CantPulsos, RISING);//RISING interpreta un cambio de estado de LOW a HIGH --> 0 a 5v
 
}

void CantPulsos ()
{
  pulsos ++;
}

float ObtenerFrecuencia (int tiempoPulsos)
{

  pulsos = 0;
  //Comienza interrupcion de microprocesador para captar pulsos
  interrupts();
  delay(tiempoPulsos);
  noInterrupts();
  
  return (float)pulsos*1000/tiempoPulsos;
  }

 float Caudal_LxM (float Frec, int FacCon)
 {
  float caudal= 0;
  caudal = Frec/FacCon;

  return caudal;
  }

//Necesitamos el tiempo en que se abrio la valvula millis() para calcular la cantidad de volumen de agua que paso
 float Volumen(float &TotAgua, float caudal)
 {
  /*Calculamos volumen total en el tiempo trascurrido, lo dividimos entre sesenta para que quedan en segundos (caudal L/min)
  y dividimos entre mil porque estan en m3, asi obtenemos los litros
  */
   TotAgua += caudal / 60 * (millis() - t0) / 1000.0;
   t0 = millis();
   return TotAgua;
  }

//TENEMOS QUE VER LAS CANTIDADES DE RIEGO AL DIA
////FUNCIOONES PARA ON // OFF RIEGO
//Aclarar estado de Riego en variable OPEN VALVULA, devuelve la fecha del proximo riego si el riego fue realizado con exito 
time_t Start_Riego(time_t Fecha, int diaR,float Frecuencia, float &TotalizadorAgua,float Riego_Actual, float caudal, float aguaNetaSector,uint8_t Error_Riego)
{
 if (Fecha.Day == diaR)


  SalDigital_ON (Ev);
 
  //Mientras no se alcance el Agua total Permitida. 

  while(Riego_Actual<aguaNetaSector)
  {
  Frecuencia = ObtenerFrecuencia(tiempoPulsos);
  unsigned long aux_tiempo = millis();
  
 
  //Una vez Utilizada la Funcion Obgtener frecuencia iniciar interrupciones nuevamente.
  interrupts();
  caudal = Caudal_LxM(Frecuencia,FactorConversion);
  //control de caudal ALERTA CAUDAL 0
    if (caudal <= 0 && millis() > (aux_tiempo + 3000))
    {
      //Fuerzo Solenoide para descartar problema de apertura
      AperturaEmergencia (EV);
      if (Error_Riego == 3)
        {
          //Envio el mensaje de alerta 
          msj_alerta +=  MsjRiego[3]+ "\nCaudal: " + caudal;
          SMS_Mensaje(SIM, String Num1,msj_alerta,E);
          delay(1000);
          SMS_Mensaje(SIM, String Num1,msj_alerta,E);
        }
      Error_Riego ++;
    }
  
  Riego_Actual = Volumen(TotalizadorAgua,caudal);
  //Sumamos al Totalizador de Agua 
  TotalizadorAgua += Riego_Actual;

  //Segunda Tanda
  Fase2_Riego(Riego_Actual, aguaNetaSector);
  //FIN de RIEGO
  Stop_Riego(Riego_Actual, aguaNetaSector, AguaTotal);
 
  }
  //salimos del while y cortamos el riego
  FechaRiego(tiempo.Day,tiempo.Month,tiempo.Year,Numero_DRiego) //devuelve la nueva fecha de riego
  Error_Riego = 0; //reiniciamos Errores
  
}



//Control de Parada Riego
void Stop_Riego(float TotalizadorAgua, float aguaNetaSector) {
    if ((TotalizadorAgua == Sector) || (TotalizadorAgua > Sector))
    {
        SalDigital_OFF(Ev);
        delay(500);
    }

}


void AperturaEmergencia (int EV)
{
      SalDigital_OFF (Ev);
      delay(3000);
      SalDigital_ON (Ev);
      
}













  

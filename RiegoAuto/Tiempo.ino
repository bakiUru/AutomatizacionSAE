#include "Tiempo.h"

//Logica de Riego Fechas
time_t riego_Actual (time_t r_Actual){

}


//Fecha Riego
time_t FechaRiego (uint8_t d, uint8_t m, uint16_t y, uint8_t Num_R)
{
  tmElements_t  Fecha;
  Fecha.Day = d + Num_R;
  Fecha.Month = m;
  Fecha.Year = y - 1970;
  return makeTime(Fecha); 
 }




//Constructor de Fecha Completa sin reloj
time_t SetFechaCompleta(int y, int m, int d, int h, int mi, int s)
{
  tmElements_t Fecha;
  Fecha.Second = s;
  Fecha.Minute = mi;
  Fecha.Hour = h;
  Fecha.Day = d;
  Fecha.Month = m;
  Fecha.Year = y - 1970;

  return makeTime(Fecha);
  }

time_t Set_Fecha(int d, int m, int y)
{
  tmElements_t Fecha;
  Fecha.Day = d;
  Fecha.Month = m;
  Fecha.Year = y - 1970;

  return makeTime(Fecha);
  }

time_t SetHora(int h, int mi, int s)
{
  tmElements_t Fecha;
  Fecha.Second = s;
  Fecha.Minute = mi;
  Fecha.Hour = h;
  
  return makeTime(Fecha);
  }


//IMPIRME Hora sin reloj

void Mostrar_Hora(time_t t)
{
  
  Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
  Serial.print(":");
  Serial.println(second(t));
}

//IMPIRME Fecha sin reloj

void Mostrar_Fecha(time_t t)
{
  
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.println(year(t));
}

bool retardo(unsigned long p)
  {
   //PRUEBA DE FUNCION RETARDO - TEMPORIZADOR
   unsigned long pausa = 0; //de forma local la quitamos del programa principal
   p = p*1000; //para convertirlos en milisegundos
  if (millis()>pausa + p)
    pausa = millis(); // p segundos + el valor de millis()
    else
      return true;
  }

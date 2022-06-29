#ifdef CalculoRiego_sensor_H_INCLUDED
#define CalculoRiego_H_INCLUDED


void setup_Caudal ();
void CantPulsos ();
int ObtenerFrecuencia (int);
float Caudal_LxM (int, int);
float Volumen(float , float );

time_t Start_Riego(time_t , int daR,float , float ,float , float , float ,uint8_t );
void Stop_Riego(float,float);
void AperturaEmergencia (int);
  
#endif //CalculoRiego_H_INCLUDED

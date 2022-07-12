# ifdef Tiempo_H_INCLUDED
#define Tiempo_H_INCLUDED


//Fecha Riego
time_t FechaRiego (int, int, int, int);

//Imprime Fecha con RELOJ
void Fecha_Reloj (myWire);

//Imprime Hora con RELOJ
void Hora_Reloj (myWire);

//Constructor de Fecha Completa sin reloj
time_t SetFechaCompleta(int, int, int, int, int, int );

time_t Set_Fecha(int, int, int);

time_t SetHora(int, int, int);

//IMPIRME Hora sin reloj

void Mostrar_Hora(time_t);

//IMPRIME Fecha sin reloj

void Mostrar_Fecha(time_t);

bool retardo(unsigned long p);

int Dia_Riego (time_t f); 

int Dar_Etapa(time_t inicio, time_t actual);


//Logica de Riego Fechas
time_t riego_Actual (time_t, time_t);

#endif //Tiempo_H_INCLUDED

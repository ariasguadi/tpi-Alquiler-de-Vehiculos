#ifndef FECHAS_H
#define FECHAS_H

#include <stdbool.h>
#include <stddef.h>

/* Funciones de validacion y aritmetica de fechas en formato AAAAMMDD. */

bool esBisiesto(int anio);
int diasEnMes(int anio, int mes);
bool esFechaValida(int fecha);
bool esFechaEnRango(int fecha, int fechaMinima, int fechaMaxima);
bool esFechaPasadaOHoy(int fecha);
bool esFechaFuturaOHoy(int fecha);
int fechaADias(int fecha);
int diasAFecha(int numeroDias);
int diasEntre(int fecha1, int fecha2);
int sumarDias(int fecha, int dias);
int fechaActual(void);

const char* fechaAString(int fecha);
const char* fechaALarga(int fecha);

/* Escribe DD/MM/AAAA en destino. El buffer debe tener al menos 11 bytes. */
void formatearFecha(int fecha, char* destino);

#endif /* FECHAS_H */

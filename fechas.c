#include "fechas.h"

#include <stdio.h>
#include <time.h>
#include <string.h>

bool esBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

int diasEnMes(int anio, int mes) {
    if (mes < 1 || mes > 12) {
        return 0;
    }
    if (mes == 2) {
        return esBisiesto(anio) ? 29 : 28;
    }
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        return 30;
    }
    return 31;
}

int fechaActual(void) {
    time_t ahora = time(NULL);
    struct tm* info = localtime(&ahora);
    if (info == NULL) {
        return 20260101;
    }
    return (info->tm_year + 1900) * 10000 + (info->tm_mon + 1) * 100 + info->tm_mday;
}

bool esFechaValida(int fecha) {
    if (fecha <= 0) {
        return false;
    }

    int anio = fecha / 10000;
    int mes = (fecha % 10000) / 100;
    int dia = fecha % 100;
    int anioActual = fechaActual() / 10000;

    if (anio < 1900 || anio > anioActual + 5) {
        return false;
    }
    if (mes < 1 || mes > 12) {
        return false;
    }
    if (dia < 1 || dia > diasEnMes(anio, mes)) {
        return false;
    }
    return true;
}

bool esFechaEnRango(int fecha, int fechaMinima, int fechaMaxima) {
    return esFechaValida(fecha) && fecha >= fechaMinima && fecha <= fechaMaxima;
}

bool esFechaPasadaOHoy(int fecha) {
    return fecha > 0 && fecha <= fechaActual();
}

bool esFechaFuturaOHoy(int fecha) {
    return fecha > 0 && fecha >= fechaActual();
}

int fechaADias(int fecha) {
    int y = fecha / 10000;
    int m = (fecha % 10000) / 100;
    int d = fecha % 100;

    y -= (m <= 2);
    int era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);
    unsigned doy = (unsigned)((153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1);
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int)doe - 719468;
}

int diasAFecha(int numeroDias) {
    numeroDias += 719468;
    int era = (numeroDias >= 0 ? numeroDias : numeroDias - 146096) / 146097;
    unsigned doe = (unsigned)(numeroDias - era * 146097);
    unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = (int)yoe + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    unsigned mp = (5 * doy + 2) / 153;
    unsigned d = doy - (153 * mp + 2) / 5 + 1;
    unsigned m = mp < 10 ? mp + 3 : mp - 9;
    int anioFinal = y + (m <= 2);
    return anioFinal * 10000 + (int)m * 100 + (int)d;
}

int diasEntre(int fecha1, int fecha2) {
    if (fecha1 <= 0 || fecha2 <= 0) {
        return 0;
    }
    return fechaADias(fecha2) - fechaADias(fecha1);
}

int sumarDias(int fecha, int dias) {
    if (fecha <= 0) {
        return 0;
    }
    return diasAFecha(fechaADias(fecha) + dias);
}

const char* fechaAString(int fecha) {
    static char buffers[4][16];
    static int idx = 0;
    idx = (idx + 1) % 4;

    if (fecha <= 0) {
        return "-";
    }
    snprintf(buffers[idx], sizeof(buffers[idx]), "%08d", fecha);
    return buffers[idx];
}

const char* fechaALarga(int fecha) {
    static char buffers[4][16];
    static int idx = 0;
    idx = (idx + 1) % 4;

    if (fecha <= 0) {
        return "-";
    }
    int anio = fecha / 10000;
    int mes = (fecha % 10000) / 100;
    int dia = fecha % 100;
    snprintf(buffers[idx], sizeof(buffers[idx]), "%02d/%02d/%04d", dia, mes, anio);
    return buffers[idx];
}

void formatearFecha(int fecha, char* destino) {
    if (destino == NULL) {
        return;
    }
    const char* texto = fechaALarga(fecha);
    strncpy(destino, texto, 10);
    destino[10] = '\0';
}

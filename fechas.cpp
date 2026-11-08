#include "fechas.h"

#include <cstddef>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

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

int fechaActual() {
    std::time_t ahora = std::time(0);
    std::tm* info = std::localtime(&ahora);
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

/* Algoritmo de dias civiles (Howard Hinnant), valido para el calendario gregoriano. */
int fechaADias(int fecha) {
    int y = fecha / 10000;
    int m = (fecha % 10000) / 100;
    int d = fecha % 100;

    y -= (m <= 2);
    int era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = static_cast<unsigned>(y - era * 400);
    unsigned doy = static_cast<unsigned>((153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1);
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<int>(doe) - 719468;
}

int diasAFecha(int numeroDias) {
    numeroDias += 719468;
    int era = (numeroDias >= 0 ? numeroDias : numeroDias - 146096) / 146097;
    unsigned doe = static_cast<unsigned>(numeroDias - era * 146097);
    unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = static_cast<int>(yoe) + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    unsigned mp = (5 * doy + 2) / 153;
    unsigned d = doy - (153 * mp + 2) / 5 + 1;
    unsigned m = mp < 10 ? mp + 3 : mp - 9;
    int anioFinal = y + (m <= 2);
    return anioFinal * 10000 + static_cast<int>(m) * 100 + static_cast<int>(d);
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

std::string fechaAString(int fecha) {
    if (fecha <= 0) {
        return "-";
    }
    std::ostringstream salida;
    salida << std::setfill('0') << std::setw(8) << fecha;
    return salida.str();
}

std::string fechaALarga(int fecha) {
    if (fecha <= 0) {
        return "-";
    }
    int anio = fecha / 10000;
    int mes = (fecha % 10000) / 100;
    int dia = fecha % 100;
    std::ostringstream salida;
    salida << std::setfill('0') << std::setw(2) << dia << "/"
           << std::setw(2) << mes << "/" << std::setw(4) << anio;
    return salida.str();
}

void formatearFecha(int fecha, char* destino) {
    if (destino == NULL) {
        return;
    }
    std::string texto = fechaALarga(fecha);
    std::strncpy(destino, texto.c_str(), 10);
    destino[10] = '\0';
}

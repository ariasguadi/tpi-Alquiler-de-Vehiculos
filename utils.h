#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    LEIDO,
    VOLVER,
    FIN_ENTRADA
} EstadoLectura;

/* Modo de prueba: evita limpiar pantalla y pausar para pruebas automaticas. */
extern bool modoPrueba;
extern bool finEntrada;

/* Entrada de datos por consola. */
void limpiarPantalla(void);
void pausarPantalla(void);

EstadoLectura leerTexto(const char* mensaje, char* salida, size_t tamSalida,
                        bool permitirVacio, size_t maximo);
EstadoLectura leerEntero(const char* mensaje, int minimo, int maximo, int* salida);
EstadoLectura leerDouble(const char* mensaje, double minimoExclusivo, double* salida);
EstadoLectura leerDoubleEnRango(const char* mensaje, double minimoExclusivo,
                               double maximoInclusivo, double* salida);
EstadoLectura leerDNI(const char* mensaje, int* salida);
EstadoLectura leerPatente(const char* mensaje, char* salida, size_t tamSalida);
EstadoLectura leerFecha(const char* mensaje, int* salida);
EstadoLectura leerFechaEnRango(const char* mensaje, int fechaMinima, int fechaMaxima, int* salida);
EstadoLectura leerSN(const char* mensaje, bool* salida);

/* Utilidades de texto. */
char* recortar(char* texto);
char* aMayusculas(char* texto);
char* aMinusculas(char* texto);
void normalizarPatente(const char* entrada, char* salida, size_t tamSalida);
void normalizarEmail(const char* entrada, char* salida, size_t tamSalida);
char* limitar(char* texto, size_t maximo);
void copiarCadena(char* destino, size_t tam, const char* origen);
int dividir(char* texto, char separador, char* partes[], int maxPartes);
bool soloDigitos(const char* texto);
bool esFinito(double valor);

/* Validaciones de formato. */
bool esPatenteVieja(const char* patente);
bool esPatenteMercosur(const char* patente);
bool esPatenteMotoVieja(const char* patente);
bool esPatenteMotoMercosur(const char* patente);
bool esPatenteValida(const char* patente);
bool validarPatente(const char* patente);
bool validarDNI(int dni);
bool validarNombre(const char* nombre);
bool validarTelefono(const char* telefono);
bool validarEmail(const char* email);
bool validarDireccion(const char* direccion);
bool validarMarca(const char* marca);
bool validarModelo(const char* modelo);
bool esTextoAlfanumericoValido(const char* texto, int longitudMinima);
bool validarTipoVehiculo(const char* tipo);
bool validarAnioVehiculo(int anio);
bool validarCapacidadVehiculo(int capacidad);
bool validarPrecioDia(double precio);
bool validarEstadoVehiculo(const char* estado);
bool validarEstadoAlquiler(const char* estado);
bool validarTipoMovimiento(const char* tipo);
bool validarMedioPago(const char* medio);

#endif /* UTILS_H */

#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <string>
#include <vector>

/* Modo de prueba: evita limpiar pantalla y pausar para pruebas automaticas. */
extern bool modoPrueba;
extern bool finEntrada;

/* Entrada de datos por consola. Devuelven false si se alcanzo el fin de entrada. */
void limpiarPantalla();
void pausarPantalla();
bool leerTexto(const std::string& mensaje, std::string& salida, bool permitirVacio = false,
               std::size_t maximo = 0);
bool leerEntero(const std::string& mensaje, int minimo, int maximo, int& salida);
bool leerDouble(const std::string& mensaje, double minimoExclusivo, double& salida);
bool leerDoubleEnRango(const std::string& mensaje, double minimoExclusivo,
                       double maximoInclusivo, double& salida);
bool leerDNI(const std::string& mensaje, int& salida);
bool leerPatente(const std::string& mensaje, std::string& salida);
bool leerFecha(const std::string& mensaje, int& salida);
bool leerFechaEnRango(const std::string& mensaje, int fechaMinima, int fechaMaxima, int& salida);
bool leerSN(const std::string& mensaje);

/* Utilidades de texto. */
std::string recortar(const std::string& texto);
std::string aMayusculas(const std::string& texto);
std::string aMinusculas(const std::string& texto);
std::string normalizarPatente(const std::string& entrada);
std::string normalizarEmail(const std::string& entrada);
std::string limitar(const std::string& texto, std::size_t maximo);
std::string cadenaAString(const char* origen, int tam);
void copiarCadena(char* destino, int tam, const std::string& origen);
std::vector<std::string> dividir(const std::string& texto, char separador);
bool soloDigitos(const std::string& texto);
std::string enteroAString(int valor);
bool esFinito(double valor);

/* Validaciones de formato. */
bool esPatenteVieja(const std::string& patente);
bool esPatenteMercosur(const std::string& patente);
bool esPatenteMotoVieja(const std::string& patente);
bool esPatenteMotoMercosur(const std::string& patente);
bool esPatenteValida(const std::string& patente);
bool validarPatente(const std::string& patente);
bool validarDNI(int dni);
bool validarNombre(const std::string& nombre);
bool validarTelefono(const std::string& telefono);
bool validarEmail(const std::string& email);
bool validarDireccion(const std::string& direccion);
bool validarMarca(const std::string& marca);
bool validarModelo(const std::string& modelo);
bool esTextoAlfanumericoValido(const std::string& texto, int longitudMinima);
bool validarTipoVehiculo(const std::string& tipo);
bool validarAnioVehiculo(int anio);
bool validarCapacidadVehiculo(int capacidad);
bool validarPrecioDia(double precio);
bool validarEstadoVehiculo(const std::string& estado);
bool validarEstadoAlquiler(const std::string& estado);
bool validarTipoMovimiento(const std::string& tipo);
bool validarMedioPago(const std::string& medio);

#endif /* UTILS_H */

#include "utils.h"
#include "comun.h"
#include "fechas.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>

bool modoPrueba = false;
bool finEntrada = false;

namespace {

bool esLetraAscii(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool esLetraExtendida(char c) {
    return static_cast<unsigned char>(c) >= 128;
}

bool esLetraMayusculaAscii(char c) {
    return c >= 'A' && c <= 'Z';
}

bool esDigitoAscii(char c) {
    return c >= '0' && c <= '9';
}

bool esEspacioAscii(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

} /* namespace */

void limpiarPantalla() {
    if (modoPrueba) {
        return;
    }
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pausarPantalla() {
    if (modoPrueba || finEntrada) {
        return;
    }
    std::cout << "\nPresione ENTER para continuar...";
    std::cout.flush();
    std::string linea;
    if (!std::getline(std::cin, linea)) {
        finEntrada = true;
    }
}

bool leerTexto(const std::string& mensaje, std::string& salida, bool permitirVacio,
               std::size_t maximo) {
    if (finEntrada) {
        return false;
    }
    while (true) {
        if (!mensaje.empty()) {
            std::cout << mensaje;
            std::cout.flush();
        }
        std::string linea;
        if (!std::getline(std::cin, linea)) {
            finEntrada = true;
            return false;
        }
        salida = recortar(linea);
        if (!permitirVacio && salida.empty()) {
            std::cout << "[ERROR] El dato ingresado no puede estar vacio. Reintente.\n";
            continue;
        }
        if (maximo > 0 && salida.size() > maximo) {
            std::cout << "[ERROR] El texto no puede superar " << maximo
                      << " caracteres. Reintente.\n";
            continue;
        }
        return true;
    }
}

bool leerEntero(const std::string& mensaje, int minimo, int maximo, int& salida) {
    if (finEntrada) {
        return false;
    }
    while (true) {
        std::string linea;
        if (!leerTexto(mensaje, linea, false)) {
            return false;
        }
        char* fin = NULL;
        errno = 0;
        long valor = std::strtol(linea.c_str(), &fin, 10);
        if (errno != 0 || fin == linea.c_str() || *fin != '\0') {
            std::cout << "[ERROR] Debe ingresar un numero entero valido. Reintente.\n";
            continue;
        }
        if (valor < static_cast<long>(minimo) || valor > static_cast<long>(maximo)) {
            std::cout << "[ERROR] El valor debe estar entre " << minimo
                      << " y " << maximo << ". Reintente.\n";
            continue;
        }
        salida = static_cast<int>(valor);
        return true;
    }
}

bool leerDouble(const std::string& mensaje, double minimoExclusivo, double& salida) {
    if (finEntrada) {
        return false;
    }
    while (true) {
        std::string linea;
        if (!leerTexto(mensaje, linea, false)) {
            return false;
        }
        std::replace(linea.begin(), linea.end(), ',', '.');
        char* fin = NULL;
        errno = 0;
        double valor = std::strtod(linea.c_str(), &fin);
        if (errno != 0 || fin == linea.c_str() || *fin != '\0' || !esFinito(valor)) {
            std::cout << "[ERROR] Debe ingresar un importe numerico valido. Reintente.\n";
            continue;
        }
        if (valor <= minimoExclusivo) {
            std::cout << "[ERROR] El importe debe ser mayor a " << minimoExclusivo
                      << ". Reintente.\n";
            continue;
        }
        salida = valor;
        return true;
    }
}

bool leerDoubleEnRango(const std::string& mensaje, double minimoExclusivo,
                       double maximoInclusivo, double& salida) {
    if (finEntrada) {
        return false;
    }
    while (true) {
        double valor = 0.0;
        if (!leerDouble(mensaje, minimoExclusivo, valor)) {
            return false;
        }
        if (valor > maximoInclusivo) {
            std::cout << "[ERROR] El importe no puede superar " << maximoInclusivo
                      << ". Reintente.\n";
            continue;
        }
        salida = valor;
        return true;
    }
}

bool leerDNI(const std::string& mensaje, int& salida) {
    int valor = 0;
    if (!leerEntero(mensaje, 1000000, 99999999, valor)) {
        return false;
    }
    if (!validarDNI(valor)) {
        return false;
    }
    salida = valor;
    return true;
}

bool leerPatente(const std::string& mensaje, std::string& salida) {
    if (finEntrada) {
        return false;
    }
    while (true) {
        std::string linea;
        if (!leerTexto(mensaje, linea, false)) {
            return false;
        }
        std::string normalizada = normalizarPatente(linea);
        if (!esPatenteValida(normalizada)) {
            std::cout << "[ERROR] Patente invalida. Formatos: AAA123 (auto viejo), "
                         "AA123AA (auto Mercosur), 123ABC (moto vieja) o "
                         "A123ABC (moto Mercosur). Reintente.\n";
            continue;
        }
        salida = normalizada;
        return true;
    }
}

bool leerFecha(const std::string& mensaje, int& salida) {
    if (finEntrada) {
        return false;
    }
    while (true) {
        int valor = 0;
        if (!leerEntero(mensaje, 19000101, 20991231, valor)) {
            return false;
        }
        if (!esFechaValida(valor)) {
            std::cout << "[ERROR] Fecha invalida. Use el formato AAAAMMDD y una fecha real. "
                         "Reintente.\n";
            continue;
        }
        salida = valor;
        return true;
    }
}

bool leerFechaEnRango(const std::string& mensaje, int fechaMinima, int fechaMaxima, int& salida) {
    if (finEntrada) {
        return false;
    }
    if (fechaMinima > fechaMaxima) {
        std::cout << "[ERROR] Rango de fechas invalido: la fecha minima es posterior a la maxima.\n";
        return false;
    }
    while (true) {
        int valor = 0;
        if (!leerFecha(mensaje, valor)) {
            return false;
        }
        if (valor < fechaMinima || valor > fechaMaxima) {
            std::cout << "[ERROR] La fecha debe estar entre " << fechaAString(fechaMinima)
                      << " y " << fechaAString(fechaMaxima) << ". Reintente.\n";
            continue;
        }
        salida = valor;
        return true;
    }
}

bool leerSN(const std::string& mensaje) {
    if (finEntrada) {
        return false;
    }
    while (true) {
        std::string linea;
        if (!leerTexto(mensaje, linea, false)) {
            return false;
        }
        std::string opcion = aMayusculas(linea);
        if (opcion == "S" || opcion == "SI") {
            return true;
        }
        if (opcion == "N" || opcion == "NO") {
            return false;
        }
        std::cout << "[ERROR] Ingrese 'S' para confirmar o 'N' para cancelar. Reintente.\n";
    }
}

std::string recortar(const std::string& texto) {
    std::size_t inicio = 0;
    while (inicio < texto.size() && esEspacioAscii(texto[inicio])) {
        ++inicio;
    }
    std::size_t fin = texto.size();
    while (fin > inicio && esEspacioAscii(texto[fin - 1])) {
        --fin;
    }
    return texto.substr(inicio, fin - inicio);
}

std::string aMayusculas(const std::string& texto) {
    std::string resultado = texto;
    for (std::size_t i = 0; i < resultado.size(); ++i) {
        resultado[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
}

std::string aMinusculas(const std::string& texto) {
    std::string resultado = texto;
    for (std::size_t i = 0; i < resultado.size(); ++i) {
        resultado[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
}

std::string normalizarPatente(const std::string& entrada) {
    std::string resultado;
    for (std::size_t i = 0; i < entrada.size(); ++i) {
        char c = entrada[i];
        if (esEspacioAscii(c) || c == '-' || c == '.') {
            continue;
        }
        resultado.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }
    return resultado;
}

std::string normalizarEmail(const std::string& entrada) {
    return aMinusculas(recortar(entrada));
}

std::string limitar(const std::string& texto, std::size_t maximo) {
    if (texto.size() <= maximo) {
        return texto;
    }
    return texto.substr(0, maximo);
}

std::string cadenaAString(const char* origen, int tam) {
    if (origen == NULL || tam <= 0) {
        return "";
    }
    int longitud = 0;
    while (longitud < tam && origen[longitud] != '\0') {
        ++longitud;
    }
    return std::string(origen, static_cast<std::size_t>(longitud));
}

void copiarCadena(char* destino, int tam, const std::string& origen) {
    if (destino == NULL || tam <= 0) {
        return;
    }
    std::strncpy(destino, origen.c_str(), static_cast<std::size_t>(tam - 1));
    destino[tam - 1] = '\0';
}

std::vector<std::string> dividir(const std::string& texto, char separador) {
    std::vector<std::string> partes;
    std::string actual;
    for (std::size_t i = 0; i < texto.size(); ++i) {
        if (texto[i] == separador) {
            partes.push_back(actual);
            actual.clear();
        } else {
            actual.push_back(texto[i]);
        }
    }
    partes.push_back(actual);
    return partes;
}

bool soloDigitos(const std::string& texto) {
    if (texto.empty()) {
        return false;
    }
    for (std::size_t i = 0; i < texto.size(); ++i) {
        if (!esDigitoAscii(texto[i])) {
            return false;
        }
    }
    return true;
}

std::string enteroAString(int valor) {
    std::ostringstream salida;
    salida << valor;
    return salida.str();
}

bool esFinito(double valor) {
    if (valor != valor) {
        return false;
    }
    const double maximo = std::numeric_limits<double>::max();
    return valor <= maximo && valor >= -maximo;
}

bool esPatenteVieja(const std::string& patente) {
    if (patente.size() != 6) {
        return false;
    }
    for (int i = 0; i < 3; ++i) {
        if (!esLetraMayusculaAscii(patente[static_cast<std::size_t>(i)])) {
            return false;
        }
    }
    for (int i = 3; i < 6; ++i) {
        if (!esDigitoAscii(patente[static_cast<std::size_t>(i)])) {
            return false;
        }
    }
    return true;
}

bool esPatenteMercosur(const std::string& patente) {
    if (patente.size() != 7) {
        return false;
    }
    if (!esLetraMayusculaAscii(patente[0]) || !esLetraMayusculaAscii(patente[1])) {
        return false;
    }
    for (int i = 2; i <= 4; ++i) {
        if (!esDigitoAscii(patente[static_cast<std::size_t>(i)])) {
            return false;
        }
    }
    return esLetraMayusculaAscii(patente[5]) && esLetraMayusculaAscii(patente[6]);
}

bool esPatenteMotoVieja(const std::string& patente) {
    if (patente.size() != 6) {
        return false;
    }
    for (int i = 0; i < 3; ++i) {
        if (!esDigitoAscii(patente[static_cast<std::size_t>(i)])) {
            return false;
        }
    }
    for (int i = 3; i < 6; ++i) {
        if (!esLetraMayusculaAscii(patente[static_cast<std::size_t>(i)])) {
            return false;
        }
    }
    return true;
}

bool esPatenteMotoMercosur(const std::string& patente) {
    if (patente.size() != 7) {
        return false;
    }
    if (!esLetraMayusculaAscii(patente[0])) {
        return false;
    }
    for (int i = 1; i <= 3; ++i) {
        if (!esDigitoAscii(patente[static_cast<std::size_t>(i)])) {
            return false;
        }
    }
    for (int i = 4; i <= 6; ++i) {
        if (!esLetraMayusculaAscii(patente[static_cast<std::size_t>(i)])) {
            return false;
        }
    }
    return true;
}

bool esPatenteValida(const std::string& patente) {
    return esPatenteVieja(patente) || esPatenteMercosur(patente) ||
           esPatenteMotoVieja(patente) || esPatenteMotoMercosur(patente);
}

bool validarPatente(const std::string& patente) {
    return esPatenteValida(patente);
}

bool validarDNI(int dni) {
    return dni >= 1000000 && dni <= 99999999;
}

bool validarNombre(const std::string& nombre) {
    std::string valor = recortar(nombre);
    if (valor.size() < 2 || valor.size() >= static_cast<std::size_t>(LON_NOMBRE)) {
        return false;
    }
    bool tieneLetra = false;
    for (std::size_t i = 0; i < valor.size(); ++i) {
        char c = valor[i];
        if (esLetraAscii(c) || esLetraExtendida(c)) {
            tieneLetra = true;
            continue;
        }
        if (c == ' ' || c == '\'' || c == '-' || c == '.') {
            continue;
        }
        return false;
    }
    return tieneLetra;
}

bool validarTelefono(const std::string& telefono) {
    std::string valor = recortar(telefono);
    if (valor.size() < 8 || valor.size() >= static_cast<std::size_t>(LON_TEL)) {
        return false;
    }
    int digitos = 0;
    for (std::size_t i = 0; i < valor.size(); ++i) {
        char c = valor[i];
        if (esDigitoAscii(c)) {
            ++digitos;
            continue;
        }
        if (c == ' ' || c == '-' || c == '(' || c == ')') {
            continue;
        }
        if (c == '+' && i == 0) {
            continue;
        }
        return false;
    }
    return digitos >= 8 && digitos <= 15;
}

bool validarEmail(const std::string& email) {
    std::string valor = recortar(email);
    if (valor.size() < 5 || valor.size() >= static_cast<std::size_t>(LON_EMAIL)) {
        return false;
    }
    if (std::count(valor.begin(), valor.end(), '@') != 1) {
        return false;
    }
    std::size_t arroba = valor.find('@');
    std::string local = valor.substr(0, arroba);
    std::string dominio = valor.substr(arroba + 1);
    if (local.empty() || dominio.empty()) {
        return false;
    }
    if (local[0] == '.' || local[local.size() - 1] == '.' ||
        dominio[0] == '.' || dominio[dominio.size() - 1] == '.') {
        return false;
    }
    if (valor.find("..") != std::string::npos) {
        return false;
    }
    if (dominio.find('.') == std::string::npos) {
        return false;
    }
    for (std::size_t i = 0; i < valor.size(); ++i) {
        char c = valor[i];
        if (esEspacioAscii(c)) {
            return false;
        }
        bool permitido = esLetraAscii(c) || esDigitoAscii(c) ||
                         c == '@' || c == '.' || c == '_' || c == '%' || c == '+' || c == '-';
        if (!permitido) {
            return false;
        }
    }
    return true;
}

bool validarDireccion(const std::string& direccion) {
    std::string valor = recortar(direccion);
    if (valor.size() < 5 || valor.size() >= static_cast<std::size_t>(LON_DIR)) {
        return false;
    }
    bool tieneLetra = false;
    for (std::size_t i = 0; i < valor.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(valor[i]);
        if (c < 32 || c == 127) {
            return false;
        }
        if (esLetraAscii(valor[i]) || esLetraExtendida(valor[i])) {
            tieneLetra = true;
        }
    }
    return tieneLetra;
}

bool validarMarca(const std::string& marca) {
    std::string valor = recortar(marca);
    if (valor.size() < 2 || valor.size() >= static_cast<std::size_t>(LON_MARCA)) {
        return false;
    }
    bool tieneLetra = false;
    for (std::size_t i = 0; i < valor.size(); ++i) {
        char c = valor[i];
        bool permitido = esLetraAscii(c) || esLetraExtendida(c) ||
                         c == ' ' || c == '-' || c == '.' || c == '&';
        if (!permitido) {
            return false;
        }
        if (esLetraAscii(c) || esLetraExtendida(c)) {
            tieneLetra = true;
        }
    }
    return tieneLetra;
}

bool esTextoAlfanumericoValido(const std::string& texto, int longitudMinima) {
    std::string valor = recortar(texto);
    if (longitudMinima < 0) {
        longitudMinima = 0;
    }
    if (valor.size() < static_cast<std::size_t>(longitudMinima)) {
        return false;
    }
    bool tieneAlfanumerico = false;
    for (std::size_t i = 0; i < valor.size(); ++i) {
        char c = valor[i];
        bool permitido = esLetraAscii(c) || esDigitoAscii(c) || esLetraExtendida(c) ||
                         c == ' ' || c == '-' || c == '.' || c == '/' || c == '&';
        if (!permitido) {
            return false;
        }
        if (esLetraAscii(c) || esDigitoAscii(c) || esLetraExtendida(c)) {
            tieneAlfanumerico = true;
        }
    }
    return tieneAlfanumerico;
}

bool validarModelo(const std::string& modelo) {
    std::string valor = recortar(modelo);
    if (valor.size() >= static_cast<std::size_t>(LON_MODELO)) {
        return false;
    }
    return esTextoAlfanumericoValido(valor, 2);
}

bool validarTipoVehiculo(const std::string& tipo) {
    std::string valor = aMayusculas(recortar(tipo));
    return valor == "AUTO" || valor == "CAMIONETA" || valor == "UTILITARIO" ||
           valor == "MOTO" || valor == "4X4";
}

bool validarAnioVehiculo(int anio) {
    int anioMaximo = fechaActual() / 10000 + 1;
    return anio >= ANIO_MIN_VEHICULO && anio <= anioMaximo;
}

bool validarCapacidadVehiculo(int capacidad) {
    return capacidad >= CAPACIDAD_MIN_VEHICULO && capacidad <= CAPACIDAD_MAX_VEHICULO;
}

bool validarPrecioDia(double precio) {
    return precio > 0.0 && esFinito(precio) && precio <= PRECIO_DIA_MAXIMO;
}

bool validarEstadoVehiculo(const std::string& estado) {
    return estado == EST_VEH_DISPONIBLE || estado == EST_VEH_ALQUILADO ||
           estado == EST_VEH_BAJA || estado == EST_VEH_MANT;
}

bool validarEstadoAlquiler(const std::string& estado) {
    return estado == EST_ALQ_ACTIVO || estado == EST_ALQ_FINALIZADO ||
           estado == EST_ALQ_CANCELADO;
}

bool validarTipoMovimiento(const std::string& tipo) {
    return tipo == MOV_PAGO || tipo == MOV_AJUSTE;
}

bool validarMedioPago(const std::string& medio) {
    return medio == MEDIO_EFECTIVO || medio == MEDIO_TARJETA ||
           medio == MEDIO_TRANSFERENCIA || medio == MEDIO_NINGUNO;
}

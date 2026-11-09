#include "utils.h"
#include "comun.h"
#include "fechas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <float.h>

bool modoPrueba = false;
bool finEntrada = false;

static bool esLetraAscii(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static bool esLetraExtendida(char c) {
    return (unsigned char)c >= 128;
}

static bool esLetraMayusculaAscii(char c) {
    return c >= 'A' && c <= 'Z';
}

static bool esDigitoAscii(char c) {
    return c >= '0' && c <= '9';
}

static bool esEspacioAscii(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

void limpiarPantalla(void) {
    if (modoPrueba) {
        return;
    }
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausarPantalla(void) {
    if (modoPrueba || finEntrada) {
        return;
    }
    printf("\nPresione ENTER para continuar...");
    fflush(stdout);
    char linea[128];
    if (fgets(linea, sizeof(linea), stdin) == NULL) {
        finEntrada = true;
    }
}

EstadoLectura leerTexto(const char* mensaje, char* salida, size_t tamSalida,
                        bool permitirVacio, size_t maximo) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    char linea[512];
    while (1) {
        if (mensaje && mensaje[0] != '\0') {
            printf("%s", mensaje);
            fflush(stdout);
        }
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            finEntrada = true;
            return FIN_ENTRADA;
        }

        size_t len = strlen(linea);
        while (len > 0 && (linea[len - 1] == '\n' || linea[len - 1] == '\r')) {
            linea[--len] = '\0';
        }

        char* trimmed = recortar(linea);
        len = strlen(trimmed);

        if ((trimmed[0] == 'V' || trimmed[0] == 'v') && trimmed[1] == '\0') {
            return VOLVER;
        }

        if (!permitirVacio && len == 0) {
            printf("[ERROR] El dato ingresado no puede estar vacio. Reintente.\n");
            continue;
        }

        if (maximo > 0 && len > maximo) {
            printf("[ERROR] El texto no puede superar %lu caracteres. Reintente.\n", (unsigned long)maximo);
            continue;
        }

        if (salida && tamSalida > 0) {
            strncpy(salida, trimmed, tamSalida - 1);
            salida[tamSalida - 1] = '\0';
        }
        return LEIDO;
    }
}

EstadoLectura leerEntero(const char* mensaje, int minimo, int maximo, int* salida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    char linea[128];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, linea, sizeof(linea), false, 0);
        if (est != LEIDO) {
            return est;
        }

        char* fin = NULL;
        errno = 0;
        long valor = strtol(linea, &fin, 10);
        if (errno != 0 || fin == linea || *fin != '\0') {
            printf("[ERROR] Debe ingresar un numero entero valido. Reintente.\n");
            continue;
        }

        if (valor < (long)minimo || valor > (long)maximo) {
            printf("[ERROR] El valor debe estar entre %d y %d. Reintente.\n", minimo, maximo);
            continue;
        }

        if (salida) {
            *salida = (int)valor;
        }
        return LEIDO;
    }
}

EstadoLectura leerDouble(const char* mensaje, double minimoExclusivo, double* salida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    char linea[128];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, linea, sizeof(linea), false, 0);
        if (est != LEIDO) {
            return est;
        }

        for (char* p = linea; *p; ++p) {
            if (*p == ',') {
                *p = '.';
            }
        }

        char* fin = NULL;
        errno = 0;
        double valor = strtod(linea, &fin);
        if (errno != 0 || fin == linea || *fin != '\0' || !esFinito(valor)) {
            printf("[ERROR] Debe ingresar un importe numerico valido. Reintente.\n");
            continue;
        }

        if (valor <= minimoExclusivo) {
            printf("[ERROR] El importe debe ser mayor a %.2f. Reintente.\n", minimoExclusivo);
            continue;
        }

        if (salida) {
            *salida = valor;
        }
        return LEIDO;
    }
}

EstadoLectura leerDoubleEnRango(const char* mensaje, double minimoExclusivo,
                               double maximoInclusivo, double* salida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    while (1) {
        double valor = 0.0;
        EstadoLectura est = leerDouble(mensaje, minimoExclusivo, &valor);
        if (est != LEIDO) {
            return est;
        }

        if (valor > maximoInclusivo) {
            printf("[ERROR] El importe no puede superar %.2f. Reintente.\n", maximoInclusivo);
            continue;
        }

        if (salida) {
            *salida = valor;
        }
        return LEIDO;
    }
}

EstadoLectura leerDNI(const char* mensaje, int* salida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    char linea[128];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, linea, sizeof(linea), false, 0);
        if (est != LEIDO) {
            return est;
        }

        char* fin = NULL;
        errno = 0;
        long valor = strtol(linea, &fin, 10);
        if (errno != 0 || fin == linea || *fin != '\0') {
            printf("[ERROR] Debe ingresar un numero entero valido. Reintente.\n");
            continue;
        }

        if (!validarDNI((int)valor)) {
            continue;
        }

        if (salida) {
            *salida = (int)valor;
        }
        return LEIDO;
    }
}

EstadoLectura leerPatente(const char* mensaje, char* salida, size_t tamSalida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    char linea[128];
    char normalizada[LON_PATENTE];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, linea, sizeof(linea), false, 0);
        if (est != LEIDO) {
            return est;
        }

        normalizarPatente(linea, normalizada, sizeof(normalizada));
        if (!esPatenteValida(normalizada)) {
            printf("[ERROR] Patente invalida. Formatos: AAA123 (auto viejo), "
                   "AA123AA (auto Mercosur), 123ABC (moto vieja) o "
                   "A123ABC (moto Mercosur). Reintente.\n");
            continue;
        }

        if (salida && tamSalida > 0) {
            strncpy(salida, normalizada, tamSalida - 1);
            salida[tamSalida - 1] = '\0';
        }
        return LEIDO;
    }
}

EstadoLectura leerFecha(const char* mensaje, int* salida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    while (1) {
        int valor = 0;
        EstadoLectura est = leerEntero(mensaje, 19000101, 20991231, &valor);
        if (est != LEIDO) {
            return est;
        }

        if (!esFechaValida(valor)) {
            printf("[ERROR] Fecha invalida. Use el formato AAAAMMDD y una fecha real. Reintente.\n");
            continue;
        }

        if (salida) {
            *salida = valor;
        }
        return LEIDO;
    }
}

EstadoLectura leerFechaEnRango(const char* mensaje, int fechaMinima, int fechaMaxima, int* salida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    if (fechaMinima > fechaMaxima) {
        printf("[ERROR] Rango de fechas invalido: la fecha minima es posterior a la maxima.\n");
        return FIN_ENTRADA;
    }
    while (1) {
        int valor = 0;
        EstadoLectura est = leerFecha(mensaje, &valor);
        if (est != LEIDO) {
            return est;
        }

        if (valor < fechaMinima || valor > fechaMaxima) {
            printf("[ERROR] La fecha debe estar entre %s y %s. Reintente.\n",
                   fechaAString(fechaMinima), fechaAString(fechaMaxima));
            continue;
        }

        if (salida) {
            *salida = valor;
        }
        return LEIDO;
    }
}

EstadoLectura leerSN(const char* mensaje, bool* salida) {
    if (finEntrada) {
        return FIN_ENTRADA;
    }
    char linea[128];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, linea, sizeof(linea), false, 0);
        if (est != LEIDO) {
            return est;
        }

        aMayusculas(linea);
        if (strcmp(linea, "S") == 0 || strcmp(linea, "SI") == 0) {
            if (salida) {
                *salida = true;
            }
            return LEIDO;
        }
        if (strcmp(linea, "N") == 0 || strcmp(linea, "NO") == 0) {
            if (salida) {
                *salida = false;
            }
            return LEIDO;
        }
        printf("[ERROR] Ingrese 'S' para confirmar o 'N' para cancelar. Reintente.\n");
    }
}

char* recortar(char* texto) {
    if (!texto) {
        return NULL;
    }
    char* inicio = texto;
    while (*inicio && esEspacioAscii(*inicio)) {
        inicio++;
    }
    size_t len = strlen(inicio);
    while (len > 0 && esEspacioAscii(inicio[len - 1])) {
        inicio[--len] = '\0';
    }
    if (inicio != texto) {
        memmove(texto, inicio, len + 1);
    }
    return texto;
}

char* aMayusculas(char* texto) {
    if (!texto) {
        return NULL;
    }
    for (char* p = texto; *p; ++p) {
        *p = (char)toupper((unsigned char)*p);
    }
    return texto;
}

char* aMinusculas(char* texto) {
    if (!texto) {
        return NULL;
    }
    for (char* p = texto; *p; ++p) {
        *p = (char)tolower((unsigned char)*p);
    }
    return texto;
}

void normalizarPatente(const char* entrada, char* salida, size_t tamSalida) {
    if (!salida || tamSalida == 0) {
        return;
    }
    salida[0] = '\0';
    if (!entrada) {
        return;
    }
    size_t pos = 0;
    for (size_t i = 0; entrada[i] != '\0' && pos + 1 < tamSalida; ++i) {
        char c = entrada[i];
        if (esEspacioAscii(c) || c == '-' || c == '.') {
            continue;
        }
        salida[pos++] = (char)toupper((unsigned char)c);
    }
    salida[pos] = '\0';
}

void normalizarEmail(const char* entrada, char* salida, size_t tamSalida) {
    if (!salida || tamSalida == 0) {
        return;
    }
    salida[0] = '\0';
    if (!entrada) {
        return;
    }
    copiarCadena(salida, tamSalida, entrada);
    recortar(salida);
    aMinusculas(salida);
}

char* limitar(char* texto, size_t maximo) {
    if (!texto) {
        return NULL;
    }
    if (strlen(texto) > maximo) {
        texto[maximo] = '\0';
    }
    return texto;
}

void copiarCadena(char* destino, size_t tam, const char* origen) {
    if (!destino || tam == 0) {
        return;
    }
    if (!origen) {
        destino[0] = '\0';
        return;
    }
    strncpy(destino, origen, tam - 1);
    destino[tam - 1] = '\0';
}

int dividir(char* texto, char separador, char* partes[], int maxPartes) {
    if (!texto || maxPartes <= 0) {
        return 0;
    }
    int cuenta = 0;
    partes[cuenta++] = texto;
    while (*texto && cuenta < maxPartes) {
        if (*texto == separador) {
            *texto = '\0';
            partes[cuenta++] = texto + 1;
        }
        texto++;
    }
    return cuenta;
}

bool soloDigitos(const char* texto) {
    if (!texto || texto[0] == '\0') {
        return false;
    }
    for (size_t i = 0; texto[i] != '\0'; ++i) {
        if (!esDigitoAscii(texto[i])) {
            return false;
        }
    }
    return true;
}

bool esFinito(double valor) {
    return isfinite(valor) != 0;
}

bool esPatenteVieja(const char* patente) {
    if (!patente || strlen(patente) != 6) {
        return false;
    }
    for (int i = 0; i < 3; ++i) {
        if (!esLetraMayusculaAscii(patente[i])) {
            return false;
        }
    }
    for (int i = 3; i < 6; ++i) {
        if (!esDigitoAscii(patente[i])) {
            return false;
        }
    }
    return true;
}

bool esPatenteMercosur(const char* patente) {
    if (!patente || strlen(patente) != 7) {
        return false;
    }
    if (!esLetraMayusculaAscii(patente[0]) || !esLetraMayusculaAscii(patente[1])) {
        return false;
    }
    for (int i = 2; i <= 4; ++i) {
        if (!esDigitoAscii(patente[i])) {
            return false;
        }
    }
    return esLetraMayusculaAscii(patente[5]) && esLetraMayusculaAscii(patente[6]);
}

bool esPatenteMotoVieja(const char* patente) {
    if (!patente || strlen(patente) != 6) {
        return false;
    }
    for (int i = 0; i < 3; ++i) {
        if (!esDigitoAscii(patente[i])) {
            return false;
        }
    }
    for (int i = 3; i < 6; ++i) {
        if (!esLetraMayusculaAscii(patente[i])) {
            return false;
        }
    }
    return true;
}

bool esPatenteMotoMercosur(const char* patente) {
    if (!patente || strlen(patente) != 7) {
        return false;
    }
    if (!esLetraMayusculaAscii(patente[0])) {
        return false;
    }
    for (int i = 1; i <= 3; ++i) {
        if (!esDigitoAscii(patente[i])) {
            return false;
        }
    }
    for (int i = 4; i <= 6; ++i) {
        if (!esLetraMayusculaAscii(patente[i])) {
            return false;
        }
    }
    return true;
}

bool esPatenteValida(const char* patente) {
    return esPatenteVieja(patente) || esPatenteMercosur(patente) ||
           esPatenteMotoVieja(patente) || esPatenteMotoMercosur(patente);
}

bool validarPatente(const char* patente) {
    return esPatenteValida(patente);
}

bool validarDNI(int dni) {
    if (dni < DNI_MIN || dni > DNI_MAX) {
        printf("[ERROR] El DNI debe estar entre 1.000.000 y 50.000.000\n");
        return false;
    }
    return true;
}

bool validarNombre(const char* nombre) {
    if (!nombre) {
        return false;
    }
    while (*nombre && esEspacioAscii(*nombre)) {
        nombre++;
    }
    size_t len = strlen(nombre);
    while (len > 0 && esEspacioAscii(nombre[len - 1])) {
        len--;
    }
    if (len < 2 || len >= LON_NOMBRE) {
        return false;
    }
    bool tieneLetra = false;
    for (size_t i = 0; i < len; ++i) {
        char c = nombre[i];
        if (esLetraAscii(c) || esLetraExtendida(c)) {
            tieneLetra = true;
            continue;
        }
        if (c == ' ' || c == '\'' || c == '-' || c == '.') {
            continue;
        }
        return false;
    }
    if (!tieneLetra) {
        return false;
    }
    /* Exigir al menos dos palabras: apellido Y nombre.
       Se busca un espacio entre dos secuencias de letras. */
    bool palabraAntes = false;
    bool espacioEncontrado = false;
    bool palabraDespues = false;
    for (size_t i = 0; i < len; ++i) {
        char c = nombre[i];
        bool esLetra = esLetraAscii(c) || esLetraExtendida(c) ||
                       c == '\'' || c == '-' || c == '.';
        if (!espacioEncontrado) {
            if (esLetra) {
                palabraAntes = true;
            } else if (c == ' ' && palabraAntes) {
                espacioEncontrado = true;
            }
        } else {
            if (esLetra) {
                palabraDespues = true;
                break;
            }
        }
    }
    return palabraAntes && espacioEncontrado && palabraDespues;
}

bool validarTelefono(const char* telefono) {
    if (!telefono) {
        return false;
    }
    while (*telefono && esEspacioAscii(*telefono)) {
        telefono++;
    }
    size_t len = strlen(telefono);
    while (len > 0 && esEspacioAscii(telefono[len - 1])) {
        len--;
    }
    if (len < 8 || len >= LON_TEL) {
        return false;
    }
    int digitos = 0;
    for (size_t i = 0; i < len; ++i) {
        char c = telefono[i];
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

bool validarEmail(const char* email) {
    if (!email) {
        return false;
    }
    while (*email && esEspacioAscii(*email)) {
        email++;
    }
    size_t len = strlen(email);
    while (len > 0 && esEspacioAscii(email[len - 1])) {
        len--;
    }
    if (len < 5 || len >= LON_EMAIL) {
        return false;
    }

    int arrobas = 0;
    const char* posArroba = NULL;
    for (size_t i = 0; i < len; ++i) {
        if (email[i] == '@') {
            arrobas++;
            posArroba = &email[i];
        }
    }
    if (arrobas != 1 || !posArroba) {
        return false;
    }

    size_t localLen = (size_t)(posArroba - email);
    const char* dominio = posArroba + 1;
    size_t dominioLen = len - localLen - 1;

    if (localLen == 0 || dominioLen == 0) {
        return false;
    }
    if (email[0] == '.' || email[localLen - 1] == '.' ||
        dominio[0] == '.' || dominio[dominioLen - 1] == '.') {
        return false;
    }

    for (size_t i = 0; i + 1 < len; ++i) {
        if (email[i] == '.' && email[i + 1] == '.') {
            return false;
        }
    }

    bool tienePuntoDominio = false;
    for (size_t i = 0; i < dominioLen; ++i) {
        if (dominio[i] == '.') {
            tienePuntoDominio = true;
            break;
        }
    }
    if (!tienePuntoDominio) {
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        char c = email[i];
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

bool validarDireccion(const char* direccion) {
    if (!direccion) {
        return false;
    }
    while (*direccion && esEspacioAscii(*direccion)) {
        direccion++;
    }
    size_t len = strlen(direccion);
    while (len > 0 && esEspacioAscii(direccion[len - 1])) {
        len--;
    }
    if (len < 5 || len >= LON_DIR) {
        return false;
    }
    bool tieneLetra = false;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)direccion[i];
        if (c < 32 || c == 127) {
            return false;
        }
        if (esLetraAscii(direccion[i]) || esLetraExtendida(direccion[i])) {
            tieneLetra = true;
        }
    }
    return tieneLetra;
}

bool validarMarca(const char* marca) {
    if (!marca) {
        return false;
    }
    while (*marca && esEspacioAscii(*marca)) {
        marca++;
    }
    size_t len = strlen(marca);
    while (len > 0 && esEspacioAscii(marca[len - 1])) {
        len--;
    }
    if (len < 2 || len >= LON_MARCA) {
        return false;
    }
    bool tieneLetra = false;
    for (size_t i = 0; i < len; ++i) {
        char c = marca[i];
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

bool esTextoAlfanumericoValido(const char* texto, int longitudMinima) {
    if (!texto) {
        return false;
    }
    while (*texto && esEspacioAscii(*texto)) {
        texto++;
    }
    size_t len = strlen(texto);
    while (len > 0 && esEspacioAscii(texto[len - 1])) {
        len--;
    }
    if (longitudMinima < 0) {
        longitudMinima = 0;
    }
    if (len < (size_t)longitudMinima) {
        return false;
    }
    bool tieneAlfanumerico = false;
    for (size_t i = 0; i < len; ++i) {
        char c = texto[i];
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

bool validarModelo(const char* modelo) {
    if (!modelo) {
        return false;
    }
    while (*modelo && esEspacioAscii(*modelo)) {
        modelo++;
    }
    size_t len = strlen(modelo);
    while (len > 0 && esEspacioAscii(modelo[len - 1])) {
        len--;
    }
    if (len >= LON_MODELO) {
        return false;
    }
    return esTextoAlfanumericoValido(modelo, 2);
}

bool validarTipoVehiculo(const char* tipo) {
    if (!tipo) {
        return false;
    }
    char buffer[64];
    copiarCadena(buffer, sizeof(buffer), tipo);
    recortar(buffer);
    aMayusculas(buffer);
    return strcmp(buffer, "AUTO") == 0 ||
           strcmp(buffer, "CAMIONETA") == 0 ||
           strcmp(buffer, "UTILITARIO") == 0 ||
           strcmp(buffer, "MOTO") == 0 ||
           strcmp(buffer, "4X4") == 0;
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

bool validarEstadoVehiculo(const char* estado) {
    if (!estado) {
        return false;
    }
    return strcmp(estado, EST_VEH_DISPONIBLE) == 0 ||
           strcmp(estado, EST_VEH_ALQUILADO) == 0 ||
           strcmp(estado, EST_VEH_BAJA) == 0 ||
           strcmp(estado, EST_VEH_MANT) == 0;
}

bool validarEstadoAlquiler(const char* estado) {
    if (!estado) {
        return false;
    }
    return strcmp(estado, EST_ALQ_ACTIVO) == 0 ||
           strcmp(estado, EST_ALQ_FINALIZADO) == 0 ||
           strcmp(estado, EST_ALQ_CANCELADO) == 0;
}

bool validarTipoMovimiento(const char* tipo) {
    if (!tipo) {
        return false;
    }
    return strcmp(tipo, MOV_PAGO) == 0 || strcmp(tipo, MOV_AJUSTE) == 0;
}

bool validarMedioPago(const char* medio) {
    if (!medio) {
        return false;
    }
    return strcmp(medio, MEDIO_EFECTIVO) == 0 ||
           strcmp(medio, MEDIO_TARJETA) == 0 ||
           strcmp(medio, MEDIO_TRANSFERENCIA) == 0 ||
           strcmp(medio, MEDIO_NINGUNO) == 0;
}

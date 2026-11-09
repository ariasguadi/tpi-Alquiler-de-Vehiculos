#include <stdio.h>
#include <string.h>

#include "comun.h"
#include "fechas.h"
#include "utils.h"

static int totalPruebas = 0;
static int totalFallos = 0;

static void comprobar(bool condicion, const char* descripcion) {
    ++totalPruebas;
    if (condicion) {
        printf("[PASS] %s\n", descripcion);
    } else {
        printf("[FAIL] %s\n", descripcion);
        ++totalFallos;
    }
}

static void probarPatentes(void) {
    printf("\n--- PATENTES ---\n");
    char buf[32];

    normalizarPatente("abc123", buf, sizeof(buf));
    comprobar(strcmp(buf, "ABC123") == 0, "Normaliza minusculas a mayusculas");

    normalizarPatente("ab 123 cd", buf, sizeof(buf));
    comprobar(strcmp(buf, "AB123CD") == 0, "Quita espacios intermedios");

    normalizarPatente("ab-123-cd", buf, sizeof(buf));
    comprobar(strcmp(buf, "AB123CD") == 0, "Quita guiones");

    normalizarPatente("ab.123.cd", buf, sizeof(buf));
    comprobar(strcmp(buf, "AB123CD") == 0, "Quita puntos");

    comprobar(validarPatente("ABC123"), "Acepta patente vieja AAA999");
    comprobar(validarPatente("AB123CD"), "Acepta patente Mercosur AA999AA");
    comprobar(validarPatente("123ABC"), "Acepta patente moto vieja 999AAA");
    comprobar(validarPatente("A123BCD"), "Acepta patente moto Mercosur A999AAA");

    comprobar(esPatenteVieja("ABC123"), "Identifica patente vieja");
    comprobar(esPatenteMercosur("AB123CD"), "Identifica patente Mercosur");
    comprobar(esPatenteMotoVieja("123ABC"), "Identifica patente moto vieja");
    comprobar(esPatenteMotoMercosur("A123BCD"), "Identifica patente moto Mercosur");

    comprobar(esPatenteValida("ABC123") && esPatenteValida("AB123CD") &&
              esPatenteValida("123ABC") && esPatenteValida("A123BCD"),
              "esPatenteValida acepta los cuatro formatos");

    comprobar(!validarPatente("abc123"), "Rechaza patente vieja en minusculas sin normalizar");
    comprobar(!validarPatente("ABC-123"), "Rechaza patente con guion sin normalizar");
    comprobar(!validarPatente("AB123C"), "Rechaza Mercosur con una letra final");
    comprobar(!validarPatente("ABC12D"), "Rechaza patente vieja con letra en los digitos");
    comprobar(!validarPatente("ABCD123"), "Rechaza patente de 7 caracteres con 4 letras iniciales");
    comprobar(!validarPatente("AB123CDE"), "Rechaza patente de 8 caracteres");
    comprobar(!validarPatente("AB123C1"), "Rechaza Mercosur con digito final");
    comprobar(!validarPatente("ABCDEF"), "Rechaza seis letras");
    comprobar(!validarPatente("1234567"), "Rechaza siete digitos");
    comprobar(!validarPatente(""), "Rechaza patente vacia");
}

static void probarDNI(void) {
    printf("\n--- DNI ---\n");
    comprobar(validarDNI(30111222), "Acepta DNI de 8 digitos");
    comprobar(validarDNI(1000000), "Acepta DNI minimo (1.000.000)");
    comprobar(validarDNI(50000000), "Acepta DNI maximo (50.000.000)");
    comprobar(!validarDNI(999999), "Rechaza DNI menor al minimo (999.999)");
    comprobar(!validarDNI(50000001), "Rechaza DNI mayor al maximo (50.000.001)");
    comprobar(!validarDNI(99999999), "Rechaza DNI fuera de rango (99.999.999)");
    comprobar(!validarDNI(0), "Rechaza DNI cero");
    comprobar(!validarDNI(-1), "Rechaza DNI negativo");
}

static void probarCliente(void) {
    printf("\n--- DATOS DE CLIENTE ---\n");
    comprobar(validarNombre("Gomez Martin"), "Acepta apellido y nombre");
    comprobar(validarNombre("O'Connor Maria"), "Acepta apostrofe");
    comprobar(validarNombre("  Gomez   Martin  "), "Acepta espacios extra");
    comprobar(validarNombre("Gomez"), "Acepta un solo nombre");
    comprobar(validarNombre("Ana"), "Acepta nombre de tres letras");
    comprobar(validarNombre("N\xC3\xBA\xC3\xB1" "ez"), "Acepta letras con tilde y enie");
    comprobar(!validarNombre("A"), "Rechaza nombre de un caracter");
    comprobar(!validarNombre("Gomez 123"), "Rechaza numeros en el nombre");
    comprobar(!validarNombre("123"), "Rechaza nombre solo numerico");
    comprobar(!validarNombre("--"), "Rechaza nombre solo de simbolos");
    comprobar(!validarNombre(""), "Rechaza nombre vacio");

    comprobar(validarTelefono("3764-100001"), "Acepta telefono con guion");
    comprobar(validarTelefono("3764100001"), "Acepta telefono de 10 digitos");
    comprobar(validarTelefono("+54 376 4100001"), "Acepta telefono internacional");
    comprobar(validarTelefono("(3764) 100001"), "Acepta parentesis y espacio");
    comprobar(!validarTelefono("abc123"), "Rechaza letras en telefono");
    comprobar(!validarTelefono("12345"), "Rechaza telefono con menos de 8 digitos");
    comprobar(!validarTelefono("1234567"), "Rechaza telefono de 7 digitos");
    comprobar(!validarTelefono("1234567890123456"), "Rechaza telefono de 16 digitos");
    comprobar(!validarTelefono("3764+100001"), "Rechaza signo + fuera del inicio");

    char emailBuf[64];
    comprobar(validarEmail("mgomez@mail.com"), "Acepta e-mail normal");
    comprobar(validarEmail("a@b.c"), "Acepta e-mail corto valido");
    normalizarEmail(" MGomez@Mail.COM ", emailBuf, sizeof(emailBuf));
    comprobar(strcmp(emailBuf, "mgomez@mail.com") == 0, "Normaliza e-mail a minusculas y sin espacios");
    comprobar(!validarEmail("sin-arroba.com"), "Rechaza e-mail sin arroba");
    comprobar(!validarEmail("a@mail"), "Rechaza e-mail sin dominio con punto");
    comprobar(!validarEmail("a b@mail.com"), "Rechaza e-mail con espacio");
    comprobar(!validarEmail("a@@mail.com"), "Rechaza e-mail con dos arrobas");
    comprobar(!validarEmail("a..b@mail.com"), "Rechaza dos puntos seguidos");
    comprobar(!validarEmail("a@mail..com"), "Rechaza dos puntos en el dominio");
    comprobar(!validarEmail(".a@mail.com"), "Rechaza local que inicia con punto");
    comprobar(!validarEmail("a.@mail.com"), "Rechaza local que termina con punto");
    comprobar(!validarEmail("a@.mail.com"), "Rechaza dominio que inicia con punto");
    comprobar(!validarEmail("a@mail.com."), "Rechaza dominio que termina con punto");

    comprobar(validarDireccion("Av. Mitre 123"), "Acepta direccion normal");
    comprobar(validarDireccion("Calle 123"), "Acepta direccion con letras y numeros");
    comprobar(!validarDireccion("12345"), "Rechaza direccion solo numerica");
    comprobar(!validarDireccion("-----"), "Rechaza direccion solo de simbolos");
    comprobar(!validarDireccion("Casa"), "Rechaza direccion de menos de 5 caracteres");
    comprobar(!validarDireccion("   "), "Rechaza direccion vacia");
    comprobar(!validarDireccion(""), "Rechaza direccion en blanco");
}

static void probarVehiculo(void) {
    printf("\n--- DATOS DE VEHICULO ---\n");
    comprobar(validarMarca("Toyota"), "Acepta marca Toyota");
    comprobar(validarMarca("Volkswagen"), "Acepta marca Volkswagen");
    comprobar(!validarMarca("123"), "Rechaza marca sin letras");
    comprobar(!validarMarca("A"), "Rechaza marca demasiado corta");

    comprobar(validarModelo("208"), "Acepta modelo numerico 208");
    comprobar(validarModelo("C4"), "Acepta modelo C4");
    comprobar(validarModelo("Corolla"), "Acepta modelo Corolla");
    comprobar(validarModelo("208 GTi"), "Acepta modelo con espacio");
    comprobar(!validarModelo(""), "Rechaza modelo vacio");
    comprobar(!validarModelo("A"), "Rechaza modelo de un caracter");
    comprobar(!validarModelo("@@@"), "Rechaza modelo con caracteres invalidos");

    comprobar(esTextoAlfanumericoValido("C4", 2), "Acepta texto C4 con minimo 2");
    comprobar(esTextoAlfanumericoValido("208", 2), "Acepta texto 208 con minimo 2");
    comprobar(esTextoAlfanumericoValido("Cronos", 2), "Acepta texto Cronos con minimo 2");
    comprobar(!esTextoAlfanumericoValido("A", 2), "Rechaza texto de un caracter");
    comprobar(!esTextoAlfanumericoValido("@@", 2), "Rechaza texto sin alfanumericos");
    comprobar(!esTextoAlfanumericoValido("  ", 2), "Rechaza texto solo de espacios");

    comprobar(validarTipoVehiculo("Auto"), "Acepta tipo Auto");
    comprobar(validarTipoVehiculo("Camioneta"), "Acepta tipo Camioneta");
    comprobar(validarTipoVehiculo("Utilitario"), "Acepta tipo Utilitario");
    comprobar(validarTipoVehiculo("Moto"), "Acepta tipo Moto");
    comprobar(validarTipoVehiculo("4x4"), "Acepta tipo 4x4");
    comprobar(!validarTipoVehiculo("123"), "Rechaza tipo fuera del menu");
    comprobar(!validarTipoVehiculo("Camion"), "Rechaza tipo no listado");

    comprobar(validarAnioVehiculo(1990), "Acepta anio minimo 1990");
    comprobar(!validarAnioVehiculo(1989), "Rechaza anio anterior a 1990");
    comprobar(validarAnioVehiculo(fechaActual() / 10000 + 1), "Acepta anio actual + 1");
    comprobar(!validarAnioVehiculo(fechaActual() / 10000 + 2), "Rechaza anio actual + 2");

    comprobar(validarCapacidadVehiculo(1), "Acepta capacidad minima 1");
    comprobar(validarCapacidadVehiculo(60), "Acepta capacidad maxima 60");
    comprobar(!validarCapacidadVehiculo(0), "Rechaza capacidad cero");
    comprobar(!validarCapacidadVehiculo(61), "Rechaza capacidad 61");

    comprobar(validarPrecioDia(0.01), "Acepta precio mayor a cero");
    comprobar(validarPrecioDia(10000000.0), "Acepta precio en el tope");
    comprobar(!validarPrecioDia(0.0), "Rechaza precio cero");
    comprobar(!validarPrecioDia(10000000.01), "Rechaza precio sobre el tope");

    comprobar(validarEstadoVehiculo(EST_VEH_DISPONIBLE), "Acepta estado Disponible");
    comprobar(validarEstadoVehiculo(EST_VEH_ALQUILADO), "Acepta estado Alquilado");
    comprobar(validarEstadoVehiculo(EST_VEH_BAJA), "Acepta estado Baja");
    comprobar(validarEstadoVehiculo(EST_VEH_MANT), "Acepta estado En mantenimiento");
    comprobar(!validarEstadoVehiculo("Otro"), "Rechaza estado de vehiculo invalido");

    comprobar(validarEstadoAlquiler(EST_ALQ_ACTIVO), "Acepta estado Activo");
    comprobar(validarEstadoAlquiler(EST_ALQ_FINALIZADO), "Acepta estado Finalizado");
    comprobar(validarEstadoAlquiler(EST_ALQ_CANCELADO), "Acepta estado Cancelado");
    comprobar(!validarEstadoAlquiler("Pendiente"), "Rechaza estado de alquiler invalido");

    comprobar(validarTipoMovimiento(MOV_PAGO), "Acepta movimiento Pago");
    comprobar(validarTipoMovimiento(MOV_AJUSTE), "Acepta movimiento Ajuste");
    comprobar(!validarTipoMovimiento("Devolucion"), "Rechaza tipo de movimiento invalido");

    comprobar(validarMedioPago(MEDIO_EFECTIVO), "Acepta medio Efectivo");
    comprobar(validarMedioPago(MEDIO_TARJETA), "Acepta medio Tarjeta");
    comprobar(validarMedioPago(MEDIO_TRANSFERENCIA), "Acepta medio Transferencia");
    comprobar(validarMedioPago(MEDIO_NINGUNO), "Acepta medio '-' para ajustes");
    comprobar(!validarMedioPago("Cheque"), "Rechaza medio de pago invalido");
}

static void probarFechas(void) {
    printf("\n--- FECHAS ---\n");
    comprobar(esBisiesto(2024), "2024 es bisiesto");
    comprobar(!esBisiesto(2026), "2026 no es bisiesto");
    comprobar(esBisiesto(2000), "2000 es bisiesto");
    comprobar(!esBisiesto(1900), "1900 no es bisiesto");
    comprobar(diasEnMes(2024, 2) == 29, "Febrero 2024 tiene 29 dias");
    comprobar(diasEnMes(2026, 2) == 28, "Febrero 2026 tiene 28 dias");
    comprobar(diasEnMes(2026, 4) == 30, "Abril tiene 30 dias");
    comprobar(diasEnMes(2026, 12) == 31, "Diciembre tiene 31 dias");

    comprobar(esFechaValida(20260101), "Acepta fecha valida");
    comprobar(esFechaValida(20240229), "Acepta 29/02/2024");
    comprobar(!esFechaValida(20260229), "Rechaza 29/02/2026");
    comprobar(!esFechaValida(20261301), "Rechaza mes 13");
    comprobar(!esFechaValida(20260132), "Rechaza dia 32");
    comprobar(!esFechaValida(18000101), "Rechaza anio anterior a 1900");
    comprobar(!esFechaValida(0), "Rechaza fecha cero");

    comprobar(sumarDias(20260131, 1) == 20260201, "Suma un dia al 31/01/2026");
    comprobar(sumarDias(20261231, 1) == 20270101, "Suma un dia al 31/12/2026");
    comprobar(sumarDias(20240228, 1) == 20240229, "Suma un dia al 28/02/2024");
    comprobar(diasEntre(20260101, 20260131) == 30, "Dias entre 01/01/2026 y 31/01/2026");
    comprobar(diasEntre(20260228, 20260301) == 1, "Dias entre 28/02/2026 y 01/03/2026");
    comprobar(diasEntre(20240228, 20240301) == 2, "Dias entre 28/02/2024 y 01/03/2024");

    comprobar(strcmp(fechaAString(20260105), "20260105") == 0, "Formatea fecha AAAAMMDD");
    comprobar(strcmp(fechaAString(0), "-") == 0, "Formatea fecha cero como guion");
    comprobar(strcmp(fechaALarga(20260105), "05/01/2026") == 0, "Formatea fecha DD/MM/AAAA");

    comprobar(esFechaEnRango(20260115, 20260101, 20260131), "Acepta fecha dentro del rango");
    comprobar(!esFechaEnRango(20260201, 20260101, 20260131), "Rechaza fecha fuera del rango");
    comprobar(!esFechaEnRango(20260230, 20260101, 20260331), "Rechaza fecha invalida dentro del rango");

    int hoy = fechaActual();
    comprobar(esFechaPasadaOHoy(hoy), "Acepta hoy como fecha pasada o igual");
    comprobar(esFechaPasadaOHoy(sumarDias(hoy, -1)), "Acepta ayer como fecha pasada");
    comprobar(!esFechaPasadaOHoy(sumarDias(hoy, 1)), "Rechaza maniana como fecha pasada");
    comprobar(!esFechaPasadaOHoy(0), "Rechaza fecha cero como pasada");
    comprobar(esFechaFuturaOHoy(hoy), "Acepta hoy como fecha futura o igual");
    comprobar(esFechaFuturaOHoy(sumarDias(hoy, 1)), "Acepta maniana como fecha futura");
    comprobar(!esFechaFuturaOHoy(sumarDias(hoy, -1)), "Rechaza ayer como fecha futura");

    char fechaFormateada[11];
    formatearFecha(20261108, fechaFormateada);
    comprobar(strcmp(fechaFormateada, "08/11/2026") == 0, "formatearFecha escribe DD/MM/AAAA en el buffer");
}

static void probarVolver(void) {
    printf("\n--- ESTADO VOLVER ('V' / 'v') ---\n");
    const char* tempPath = "test_volver_input.tmp";
    FILE* f = fopen(tempPath, "w");
    if (!f) {
        comprobar(false, "No se pudo crear archivo de prueba para VOLVER");
        return;
    }
    /*
     * Lineas enviadas a stdin:
     * 1: "V\n"    -> debe dar VOLVER
     * 2: "v\n"    -> debe dar VOLVER
     * 3: "  V  \n" -> con espacios debe recortar y dar VOLVER
     * 4: "Vega\n" -> palabra valida que empieza con V, debe dar LEIDO
     * 5: "123\n"  -> entero valido, debe dar LEIDO
     */
    fprintf(f, "V\nv\n  V  \nVega\n123\n");
    fclose(f);

    if (freopen(tempPath, "r", stdin) != NULL) {
        char buf[64];
        EstadoLectura e1 = leerTexto("", buf, sizeof(buf), false, 60);
        comprobar(e1 == VOLVER, "Ingresar 'V' en solitario devuelve estado VOLVER");

        EstadoLectura e2 = leerTexto("", buf, sizeof(buf), false, 60);
        comprobar(e2 == VOLVER, "Ingresar 'v' en solitario devuelve estado VOLVER");

        EstadoLectura e3 = leerTexto("", buf, sizeof(buf), false, 60);
        comprobar(e3 == VOLVER, "Ingresar '  V  ' con espacios devuelve estado VOLVER");

        EstadoLectura e4 = leerTexto("", buf, sizeof(buf), false, 60);
        comprobar(e4 == LEIDO && strcmp(buf, "Vega") == 0,
                  "Ingresar 'Vega' (comienza con V pero tiene mas letras) devuelve LEIDO");

        int valEntero = 0;
        EstadoLectura e5 = leerEntero("", 1, 999, &valEntero);
        comprobar(e5 == LEIDO && valEntero == 123,
                  "Ingresar '123' devuelve LEIDO con entero correcto");

#ifdef _WIN32
        freopen("CON", "r", stdin);
#endif
    } else {
        comprobar(false, "freopen no pudo redirigir stdin para probar VOLVER");
    }
    remove(tempPath);
}

static void probarUtilitarios(void) {
    printf("\n--- UTILITARIOS ---\n");
    char buf1[32] = "  hola  ";
    comprobar(strcmp(recortar(buf1), "hola") == 0, "Recorta espacios exteriores");

    char buf2[32] = "Abc";
    comprobar(strcmp(aMayusculas(buf2), "ABC") == 0, "Convierte a mayusculas");

    char buf3[32] = "AbC";
    comprobar(strcmp(aMinusculas(buf3), "abc") == 0, "Convierte a minusculas");

    char buf4[32] = "abcdef";
    comprobar(strcmp(limitar(buf4, 3), "abc") == 0, "Limita texto largo");

    char buf5[32] = "ab";
    comprobar(strcmp(limitar(buf5, 3), "ab") == 0, "No modifica texto corto");

    comprobar(soloDigitos("123"), "Reconoce solo digitos");
    comprobar(!soloDigitos("12a"), "Rechaza letras entre digitos");
    comprobar(!soloDigitos(""), "Rechaza cadena vacia en soloDigitos");

    char bufDiv[32] = "a;b;c";
    char* partes[5];
    int nPartes = dividir(bufDiv, ';', partes, 5);
    comprobar(nPartes == 3 && strcmp(partes[0], "a") == 0 &&
              strcmp(partes[1], "b") == 0 && strcmp(partes[2], "c") == 0,
              "Divide texto por punto y coma");

    char buffer[5];
    copiarCadena(buffer, 5, "abcdef");
    comprobar(strcmp(buffer, "abcd") == 0, "Copia y trunca cadena");
}

int main(void) {
    printf("==============================================\n");
    printf("   PRUEBAS DE VALIDACION DEL SISTEMA\n");
    printf("==============================================\n");

    probarPatentes();
    probarDNI();
    probarCliente();
    probarVehiculo();
    probarFechas();
    probarUtilitarios();
    probarVolver();

    printf("\n==============================================\n");
    printf("Total de pruebas: %d\n", totalPruebas);
    printf("Fallos:           %d\n", totalFallos);
    if (totalFallos == 0) {
        printf("RESULTADO: TODAS LAS PRUEBAS PASARON.\n");
        return 0;
    }
    printf("RESULTADO: HAY PRUEBAS FALLIDAS.\n");
    return 1;
}

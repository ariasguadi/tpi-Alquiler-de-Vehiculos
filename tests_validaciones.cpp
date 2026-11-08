#include <iostream>
#include <string>
#include <vector>

#include "comun.h"
#include "fechas.h"
#include "utils.h"

namespace {

int totalPruebas = 0;
int totalFallos = 0;

void comprobar(bool condicion, const std::string& descripcion) {
    ++totalPruebas;
    if (condicion) {
        std::cout << "[PASS] " << descripcion << "\n";
    } else {
        std::cout << "[FAIL] " << descripcion << "\n";
        ++totalFallos;
    }
}

void probarPatentes() {
    std::cout << "\n--- PATENTES ---\n";
    comprobar(normalizarPatente("abc123") == "ABC123",
              "Normaliza minusculas a mayusculas");
    comprobar(normalizarPatente("ab 123 cd") == "AB123CD",
              "Quita espacios intermedios");
    comprobar(normalizarPatente("ab-123-cd") == "AB123CD",
              "Quita guiones");
    comprobar(normalizarPatente("ab.123.cd") == "AB123CD",
              "Quita puntos");
    comprobar(validarPatente("ABC123"),
              "Acepta patente vieja AAA999");
    comprobar(validarPatente("AB123CD"),
              "Acepta patente Mercosur AA999AA");
    comprobar(validarPatente("123ABC"),
              "Acepta patente moto vieja 999AAA");
    comprobar(validarPatente("A123BCD"),
              "Acepta patente moto Mercosur A999AAA");
    comprobar(esPatenteVieja("ABC123"), "Identifica patente vieja");
    comprobar(esPatenteMercosur("AB123CD"), "Identifica patente Mercosur");
    comprobar(esPatenteMotoVieja("123ABC"), "Identifica patente moto vieja");
    comprobar(esPatenteMotoMercosur("A123BCD"), "Identifica patente moto Mercosur");
    comprobar(esPatenteValida("ABC123") && esPatenteValida("AB123CD") &&
                  esPatenteValida("123ABC") && esPatenteValida("A123BCD"),
              "esPatenteValida acepta los cuatro formatos");
    comprobar(!validarPatente("abc123"),
              "Rechaza patente vieja en minusculas sin normalizar");
    comprobar(!validarPatente("ABC-123"),
              "Rechaza patente con guion sin normalizar");
    comprobar(!validarPatente("AB123C"),
              "Rechaza Mercosur con una letra final");
    comprobar(!validarPatente("ABC12D"),
              "Rechaza patente vieja con letra en los digitos");
    comprobar(!validarPatente("ABCD123"),
              "Rechaza patente de 7 caracteres con 4 letras iniciales");
    comprobar(!validarPatente("AB123CDE"),
              "Rechaza patente de 8 caracteres");
    comprobar(!validarPatente("AB123C1"),
              "Rechaza Mercosur con digito final");
    comprobar(!validarPatente("ABCDEF"),
              "Rechaza seis letras");
    comprobar(!validarPatente("1234567"),
              "Rechaza siete digitos");
    comprobar(!validarPatente(""),
              "Rechaza patente vacia");
}

void probarDNI() {
    std::cout << "\n--- DNI ---\n";
    comprobar(validarDNI(30111222), "Acepta DNI de 8 digitos");
    comprobar(validarDNI(1000000), "Acepta DNI de 7 digitos");
    comprobar(validarDNI(99999999), "Acepta DNI maximo");
    comprobar(!validarDNI(999999), "Rechaza DNI de 6 digitos");
    comprobar(!validarDNI(100000000), "Rechaza DNI de 9 digitos");
    comprobar(!validarDNI(0), "Rechaza DNI cero");
    comprobar(!validarDNI(-1), "Rechaza DNI negativo");
}

void probarCliente() {
    std::cout << "\n--- DATOS DE CLIENTE ---\n";
    comprobar(validarNombre("Gomez Martin"), "Acepta apellido y nombre");
    comprobar(validarNombre("O'Connor Maria"), "Acepta apostrofe");
    comprobar(validarNombre("  Gomez   Martin  "), "Acepta espacios extra");
    comprobar(validarNombre("Gomez"), "Acepta un solo nombre");
    comprobar(validarNombre("Ana"), "Acepta nombre de tres letras");
    comprobar(validarNombre("N\xC3\xBA\xC3\xB1" "ez"),
              "Acepta letras con tilde y enie");
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
    comprobar(!validarTelefono("1234567890123456"),
              "Rechaza telefono de 16 digitos");
    comprobar(!validarTelefono("3764+100001"),
              "Rechaza signo + fuera del inicio");

    comprobar(validarEmail("mgomez@mail.com"), "Acepta e-mail normal");
    comprobar(validarEmail("a@b.c"), "Acepta e-mail corto valido");
    comprobar(normalizarEmail(" MGomez@Mail.COM ") == "mgomez@mail.com",
              "Normaliza e-mail a minusculas y sin espacios");
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

void probarVehiculo() {
    std::cout << "\n--- DATOS DE VEHICULO ---\n";
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
    comprobar(validarAnioVehiculo(fechaActual() / 10000 + 1),
              "Acepta anio actual + 1");
    comprobar(!validarAnioVehiculo(fechaActual() / 10000 + 2),
              "Rechaza anio actual + 2");

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

void probarFechas() {
    std::cout << "\n--- FECHAS ---\n";
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

    comprobar(sumarDias(20260131, 1) == 20260201,
              "Suma un dia al 31/01/2026");
    comprobar(sumarDias(20261231, 1) == 20270101,
              "Suma un dia al 31/12/2026");
    comprobar(sumarDias(20240228, 1) == 20240229,
              "Suma un dia al 28/02/2024");
    comprobar(diasEntre(20260101, 20260131) == 30,
              "Dias entre 01/01/2026 y 31/01/2026");
    comprobar(diasEntre(20260228, 20260301) == 1,
              "Dias entre 28/02/2026 y 01/03/2026");
    comprobar(diasEntre(20240228, 20240301) == 2,
              "Dias entre 28/02/2024 y 01/03/2024");

    comprobar(fechaAString(20260105) == "20260105", "Formatea fecha AAAAMMDD");
    comprobar(fechaAString(0) == "-", "Formatea fecha cero como guion");
    comprobar(fechaALarga(20260105) == "05/01/2026", "Formatea fecha DD/MM/AAAA");

    comprobar(esFechaEnRango(20260115, 20260101, 20260131),
              "Acepta fecha dentro del rango");
    comprobar(!esFechaEnRango(20260201, 20260101, 20260131),
              "Rechaza fecha fuera del rango");
    comprobar(!esFechaEnRango(20260230, 20260101, 20260331),
              "Rechaza fecha invalida dentro del rango");

    int hoy = fechaActual();
    comprobar(esFechaPasadaOHoy(hoy), "Acepta hoy como fecha pasada o igual");
    comprobar(esFechaPasadaOHoy(sumarDias(hoy, -1)),
              "Acepta ayer como fecha pasada");
    comprobar(!esFechaPasadaOHoy(sumarDias(hoy, 1)),
              "Rechaza maniana como fecha pasada");
    comprobar(!esFechaPasadaOHoy(0), "Rechaza fecha cero como pasada");
    comprobar(esFechaFuturaOHoy(hoy), "Acepta hoy como fecha futura o igual");
    comprobar(esFechaFuturaOHoy(sumarDias(hoy, 1)),
              "Acepta maniana como fecha futura");
    comprobar(!esFechaFuturaOHoy(sumarDias(hoy, -1)),
              "Rechaza ayer como fecha futura");

    char fechaFormateada[11];
    formatearFecha(20261108, fechaFormateada);
    comprobar(std::string(fechaFormateada) == "08/11/2026",
              "formatearFecha escribe DD/MM/AAAA en el buffer");
}

void probarUtilitarios() {
    std::cout << "\n--- UTILITARIOS ---\n";
    comprobar(recortar("  hola  ") == "hola", "Recorta espacios exteriores");
    comprobar(aMayusculas("Abc") == "ABC", "Convierte a mayusculas");
    comprobar(aMinusculas("AbC") == "abc", "Convierte a minusculas");
    comprobar(limitar("abcdef", 3) == "abc", "Limita texto largo");
    comprobar(limitar("ab", 3) == "ab", "No modifica texto corto");
    comprobar(soloDigitos("123"), "Reconoce solo digitos");
    comprobar(!soloDigitos("12a"), "Rechaza letras entre digitos");
    comprobar(!soloDigitos(""), "Rechaza cadena vacia en soloDigitos");

    std::vector<std::string> partes = dividir("a;b;c", ';');
    comprobar(partes.size() == 3 && partes[0] == "a" && partes[1] == "b" &&
                  partes[2] == "c",
              "Divide texto por punto y coma");

    char buffer[5];
    copiarCadena(buffer, 5, "abcdef");
    comprobar(std::string(buffer) == "abcd", "Copia y trunca cadena");
    comprobar(cadenaAString("Hola", 10) == "Hola", "Convierte char* a string");
}

} /* namespace */

int main() {
    std::cout << "==============================================\n";
    std::cout << "   PRUEBAS DE VALIDACION DEL SISTEMA\n";
    std::cout << "==============================================\n";

    probarPatentes();
    probarDNI();
    probarCliente();
    probarVehiculo();
    probarFechas();
    probarUtilitarios();

    std::cout << "\n==============================================\n";
    std::cout << "Total de pruebas: " << totalPruebas << "\n";
    std::cout << "Fallos:           " << totalFallos << "\n";
    if (totalFallos == 0) {
        std::cout << "RESULTADO: TODAS LAS PRUEBAS PASARON.\n";
        return 0;
    }
    std::cout << "RESULTADO: HAY PRUEBAS FALLIDAS.\n";
    return 1;
}

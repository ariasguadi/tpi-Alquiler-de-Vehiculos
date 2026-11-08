#include "alquileres.h"

#include <cmath>
#include <iomanip>
#include <iostream>

#include "archivos.h"
#include "clientes.h"
#include "fechas.h"
#include "movimientos.h"
#include "utils.h"
#include "vehiculos.h"

namespace {

std::string nombreCliente(const std::vector<Cliente>& clientes, int dni) {
    int indice = buscarIndiceCliente(clientes, dni);
    if (indice < 0) {
        return "(Desconocido)";
    }
    return cadenaAString(clientes[static_cast<std::size_t>(indice)].apellidoNombre, LON_NOMBRE);
}

std::string descripcionVehiculo(const std::vector<Vehiculo>& vehiculos,
                                const std::string& patente) {
    int indice = buscarIndiceVehiculo(vehiculos, patente);
    if (indice < 0) {
        return "(Desconocido)";
    }
    const Vehiculo& v = vehiculos[static_cast<std::size_t>(indice)];
    return cadenaAString(v.marca, LON_MARCA) + " " + cadenaAString(v.modelo, LON_MODELO);
}

bool guardarAlquilerYVehiculos(const std::vector<Alquiler>& alquileres,
                               const std::vector<Vehiculo>& vehiculos,
                               const std::vector<Alquiler>& alquileresPrevios,
                               const std::vector<Vehiculo>& vehiculosPrevios) {
    if (!guardarAlquileres(alquileres)) {
        return false;
    }
    if (!guardarVehiculos(vehiculos)) {
        std::cout << "[ERROR] No se pudo actualizar " << ARCH_VEHICULOS
                  << ". Revirtiendo los cambios en " << ARCH_ALQUILERES << "...\n";
        guardarAlquileres(alquileresPrevios);
        guardarVehiculos(vehiculosPrevios);
        return false;
    }
    return true;
}

int buscarAlquilerActivoEnVector(const std::vector<Alquiler>& alquileres, int dni) {
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (alquileres[i].dni == dni &&
            cadenaAString(alquileres[i].estado, LON_ESTADO) == EST_ALQ_ACTIVO) {
            return alquileres[i].numAlquiler;
        }
    }
    return 0;
}

int buscarAlquilerActivoVehiculoEnVector(const std::vector<Alquiler>& alquileres,
                                         const std::string& patente) {
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (cadenaAString(alquileres[i].patente, LON_PATENTE) == patente &&
            cadenaAString(alquileres[i].estado, LON_ESTADO) == EST_ALQ_ACTIVO) {
            return alquileres[i].numAlquiler;
        }
    }
    return 0;
}

int primeraFechaPago(int numAlquiler) {
    std::vector<Movimiento> movimientos;
    if (!cargarMovimientos(movimientos)) {
        return -1;
    }
    int primera = 0;
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        if (movimientos[i].numAlquiler == numAlquiler &&
            cadenaAString(movimientos[i].tipoMov, LON_TIPO_MOV) == MOV_PAGO) {
            if (primera == 0 || movimientos[i].fecha < primera) {
                primera = movimientos[i].fecha;
            }
        }
    }
    return primera;
}

} /* namespace */

bool cargarAlquileres(std::vector<Alquiler>& alquileres) {
    std::string error;
    if (!cargarRegistros(ARCH_ALQUILERES, alquileres, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        alquileres[i].patente[LON_PATENTE - 1] = '\0';
        alquileres[i].estado[LON_ESTADO - 1] = '\0';
    }
    return true;
}

bool guardarAlquileres(const std::vector<Alquiler>& alquileres) {
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        std::string errorRegistro;
        if (!validarDatosAlquiler(alquileres[i], errorRegistro)) {
            std::cout << "[ERROR] Alquiler Nro " << alquileres[i].numAlquiler
                      << " inconsistente: " << errorRegistro << "\n";
            return false;
        }
    }
    std::string error;
    if (!guardarRegistros(ARCH_ALQUILERES, alquileres, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    return true;
}

int buscarIndiceAlquiler(const std::vector<Alquiler>& alquileres, int numero) {
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (alquileres[i].numAlquiler == numero) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool buscarAlquilerPorNumero(int numero, Alquiler& alquiler) {
    std::vector<Alquiler> alquileres;
    if (!cargarAlquileres(alquileres)) {
        return false;
    }
    int indice = buscarIndiceAlquiler(alquileres, numero);
    if (indice < 0) {
        return false;
    }
    alquiler = alquileres[static_cast<std::size_t>(indice)];
    return true;
}

bool buscarAlquilerActivoPorPatente(const std::string& patente, Alquiler& alquiler) {
    std::vector<Alquiler> alquileres;
    if (!cargarAlquileres(alquileres)) {
        return false;
    }
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (cadenaAString(alquileres[i].patente, LON_PATENTE) == patente &&
            cadenaAString(alquileres[i].estado, LON_ESTADO) == EST_ALQ_ACTIVO) {
            alquiler = alquileres[i];
            return true;
        }
    }
    return false;
}

int clienteTieneAlquilerActivo(int dni) {
    std::vector<Alquiler> alquileres;
    if (!cargarAlquileres(alquileres)) {
        return 0;
    }
    return buscarAlquilerActivoEnVector(alquileres, dni);
}

bool vehiculoTieneAlquilerActivo(const std::string& patente) {
    std::vector<Alquiler> alquileres;
    if (!cargarAlquileres(alquileres)) {
        return false;
    }
    return buscarAlquilerActivoVehiculoEnVector(alquileres, patente) > 0;
}

bool validarDatosAlquiler(const Alquiler& alquiler, std::string& error) {
    if (alquiler.numAlquiler <= 0) {
        error = "El numero de alquiler debe ser mayor a cero.";
        return false;
    }
    if (!validarDNI(alquiler.dni)) {
        error = "El DNI del cliente no es valido.";
        return false;
    }
    if (!esPatenteValida(cadenaAString(alquiler.patente, LON_PATENTE))) {
        error = "La patente del vehiculo no es valida.";
        return false;
    }
    if (!esFechaValida(alquiler.fechaInicio)) {
        error = "La fecha de inicio no es valida.";
        return false;
    }
    if (alquiler.cantDias < 1 || alquiler.cantDias > DIAS_MAX_ALQUILER) {
        error = "La cantidad de dias debe estar entre 1 y " +
                enteroAString(DIAS_MAX_ALQUILER) + ".";
        return false;
    }
    if (!esFechaValida(alquiler.fechaPrevista)) {
        error = "La fecha prevista de devolucion no es valida.";
        return false;
    }
    if (alquiler.fechaPrevista != sumarDias(alquiler.fechaInicio, alquiler.cantDias)) {
        error = "La fecha prevista no coincide con el inicio mas la cantidad de dias.";
        return false;
    }
    if (!esFinito(alquiler.importeTotal) || alquiler.importeTotal < 0.0) {
        error = "El importe total no es un numero valido.";
        return false;
    }
    if (!esFinito(alquiler.importePagado) || alquiler.importePagado < 0.0) {
        error = "El importe pagado no es un numero valido.";
        return false;
    }
    if (alquiler.importePagado > alquiler.importeTotal + TOLERANCIA_SALDO) {
        error = "El importe pagado no puede superar el importe total.";
        return false;
    }
    if (!validarEstadoAlquiler(cadenaAString(alquiler.estado, LON_ESTADO))) {
        error = "El estado del alquiler no es valido.";
        return false;
    }
    if (alquiler.fechaEfectiva != 0) {
        if (!esFechaValida(alquiler.fechaEfectiva)) {
            error = "La fecha efectiva de devolucion no es valida.";
            return false;
        }
        if (alquiler.fechaEfectiva < alquiler.fechaInicio) {
            error = "La fecha efectiva no puede ser anterior a la fecha de inicio.";
            return false;
        }
        if (!esFechaPasadaOHoy(alquiler.fechaEfectiva)) {
            error = "La fecha efectiva no puede ser posterior a la fecha actual.";
            return false;
        }
    }

    std::string estado = cadenaAString(alquiler.estado, LON_ESTADO);
    if (estado == EST_ALQ_FINALIZADO && alquiler.fechaEfectiva == 0) {
        error = "Un alquiler Finalizado debe tener fecha efectiva de devolucion.";
        return false;
    }
    if (estado != EST_ALQ_FINALIZADO && alquiler.fechaEfectiva != 0) {
        error = "Solo un alquiler Finalizado puede tener fecha efectiva de devolucion.";
        return false;
    }
    return true;
}

int proximoNumeroAlquiler() {
    std::vector<Alquiler> alquileres;
    if (!cargarAlquileres(alquileres)) {
        return 1;
    }
    int maximo = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (alquileres[i].numAlquiler > maximo) {
            maximo = alquileres[i].numAlquiler;
        }
    }
    return maximo + 1;
}

void imprimirEncabezadoAlquileres() {
    std::cout << std::left << std::setw(5) << "NRO" << " | "
              << std::setw(28) << "CLIENTE (DNI - NOMBRE)" << " | "
              << std::setw(22) << "VEHICULO (PAT - MARCA)" << " | "
              << std::setw(10) << "INICIO" << " | "
              << std::setw(10) << "PREVISTA" << " | "
              << std::setw(10) << "DEV." << " | "
              << std::setw(4) << "DIAS" << " | "
              << std::setw(12) << "TOTAL" << " | "
              << std::setw(12) << "PAGADO" << " | "
              << "ESTADO\n";
    std::cout << "--------------------------------------------------------------------------------"
                 "-------------------------------------------------------------\n";
}

void imprimirFilaAlquiler(const Alquiler& alquiler, const std::vector<Cliente>& clientes,
                          const std::vector<Vehiculo>& vehiculos) {
    std::string cliente = enteroAString(alquiler.dni) + " - " +
                          nombreCliente(clientes, alquiler.dni);
    std::string vehiculo = cadenaAString(alquiler.patente, LON_PATENTE) + " - " +
                           descripcionVehiculo(vehiculos, cadenaAString(alquiler.patente, LON_PATENTE));
    std::string devolucion = alquiler.fechaEfectiva > 0
                                 ? fechaAString(alquiler.fechaEfectiva)
                                 : "-";
    std::cout << std::left << std::setw(5) << alquiler.numAlquiler << " | "
              << std::setw(28) << limitar(cliente, 28) << " | "
              << std::setw(22) << limitar(vehiculo, 22) << " | "
              << std::setw(10) << fechaAString(alquiler.fechaInicio) << " | "
              << std::setw(10) << fechaAString(alquiler.fechaPrevista) << " | "
              << std::setw(10) << devolucion << " | "
              << std::setw(4) << alquiler.cantDias << " | $"
              << std::setw(11) << std::fixed << std::setprecision(2) << alquiler.importeTotal << " | $"
              << std::setw(11) << std::fixed << std::setprecision(2) << alquiler.importePagado << " | "
              << cadenaAString(alquiler.estado, LON_ESTADO) << "\n";
}

void realizarAlquiler() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "           NUEVO ALQUILER\n";
    std::cout << "========================================\n";

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarPantalla();
        return;
    }

    int dni = 0;
    if (!leerDNI("Ingrese el DNI del cliente: ", dni)) {
        return;
    }
    int indiceCliente = buscarIndiceCliente(clientes, dni);
    if (indiceCliente < 0) {
        std::cout << "\n[ERROR] El cliente con DNI " << dni << " no existe en el sistema.\n";
        pausarPantalla();
        return;
    }
    const Cliente& cliente = clientes[static_cast<std::size_t>(indiceCliente)];
    if (cliente.estado != CLI_ACTIVO) {
        std::cout << "\n[ERROR] El cliente se encuentra INACTIVO. Operacion rechazada.\n";
        pausarPantalla();
        return;
    }

    int alquilerActivo = buscarAlquilerActivoEnVector(alquileres, dni);
    if (alquilerActivo > 0) {
        std::cout << "\n[ERROR] El cliente ya posee el alquiler Nro " << alquilerActivo
                  << " en estado ACTIVO.\n";
        std::cout << "Cada cliente puede tener como maximo un alquiler activo a la vez.\n";
        pausarPantalla();
        return;
    }

    std::string patente;
    if (!leerPatente("Ingrese la patente del vehiculo a alquilar: ", patente)) {
        return;
    }
    int indiceVehiculo = buscarIndiceVehiculo(vehiculos, patente);
    if (indiceVehiculo < 0) {
        std::cout << "\n[ERROR] El vehiculo con patente " << patente
                  << " no existe en el sistema.\n";
        pausarPantalla();
        return;
    }
    const Vehiculo& vehiculo = vehiculos[static_cast<std::size_t>(indiceVehiculo)];
    if (cadenaAString(vehiculo.estado, LON_ESTADO) != EST_VEH_DISPONIBLE) {
        std::cout << "\n[ERROR] El vehiculo no esta disponible para alquiler. "
                     "Estado actual: " << cadenaAString(vehiculo.estado, LON_ESTADO) << ".\n";
        pausarPantalla();
        return;
    }
    int alquilerVehiculo = buscarAlquilerActivoVehiculoEnVector(alquileres, patente);
    if (alquilerVehiculo > 0) {
        std::cout << "\n[ERROR] El vehiculo ya tiene el alquiler Nro " << alquilerVehiculo
                  << " en estado ACTIVO.\n";
        pausarPantalla();
        return;
    }

    int fechaInicio = 0;
    if (!leerFechaEnRango("Ingrese la fecha de inicio del alquiler (AAAAMMDD, desde hoy "
                          "hasta hoy + " + enteroAString(DIAS_MAX_INICIO_FUTURO) + " dias): ",
                          fechaActual(),
                          sumarDias(fechaActual(), DIAS_MAX_INICIO_FUTURO),
                          fechaInicio)) {
        return;
    }
    int cantidadDias = 0;
    if (!leerEntero("Ingrese la cantidad de dias contratados (1-" +
                        enteroAString(DIAS_MAX_ALQUILER) + "): ",
                    1, DIAS_MAX_ALQUILER, cantidadDias)) {
        return;
    }

    Alquiler nuevo = Alquiler();
    nuevo.numAlquiler = proximoNumeroAlquiler();
    nuevo.dni = dni;
    copiarCadena(nuevo.patente, LON_PATENTE, patente);
    nuevo.fechaInicio = fechaInicio;
    nuevo.fechaPrevista = sumarDias(fechaInicio, cantidadDias);
    nuevo.fechaEfectiva = 0;
    nuevo.cantDias = cantidadDias;
    nuevo.importeTotal = cantidadDias * vehiculo.precioDia;
    nuevo.importePagado = 0.0;
    copiarCadena(nuevo.estado, LON_ESTADO, EST_ALQ_ACTIVO);

    std::vector<Alquiler> alquileresPrevios = alquileres;
    std::vector<Vehiculo> vehiculosPrevios = vehiculos;

    alquileres.push_back(nuevo);
    copiarCadena(vehiculos[static_cast<std::size_t>(indiceVehiculo)].estado,
                 LON_ESTADO, EST_VEH_ALQUILADO);

    if (!guardarAlquilerYVehiculos(alquileres, vehiculos, alquileresPrevios, vehiculosPrevios)) {
        std::cout << "\n[ERROR] No se pudo registrar el alquiler.\n";
        pausarPantalla();
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << ">>> ALQUILER REGISTRADO EXITOSAMENTE\n";
    std::cout << "Numero de Alquiler: " << nuevo.numAlquiler << "\n";
    std::cout << "Cliente:            " << cadenaAString(cliente.apellidoNombre, LON_NOMBRE)
              << " (DNI: " << cliente.dni << ")\n";
    std::cout << "Vehiculo:           " << cadenaAString(vehiculo.marca, LON_MARCA) << " "
              << cadenaAString(vehiculo.modelo, LON_MODELO)
              << " (Patente: " << patente << ")\n";
    std::cout << "Fecha de Inicio:    " << fechaALarga(nuevo.fechaInicio) << "\n";
    std::cout << "Fecha Prevista:     " << fechaALarga(nuevo.fechaPrevista) << "\n";
    std::cout << "Dias Contratados:   " << nuevo.cantDias << "\n";
    std::cout << "Importe Total:      $" << std::fixed << std::setprecision(2)
              << nuevo.importeTotal << "\n";
    std::cout << "Estado Alquiler:    " << EST_ALQ_ACTIVO << "\n";
    std::cout << "Estado Vehiculo:    " << EST_VEH_ALQUILADO << "\n";
    std::cout << "========================================\n";
    pausarPantalla();
}

void modificarAlquiler() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "          MODIFICAR ALQUILER\n";
    std::cout << "========================================\n";

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarPantalla();
        return;
    }

    int numero = 0;
    if (!leerEntero("Ingrese el numero de alquiler a modificar: ", 1, 9999999, numero)) {
        return;
    }
    int indiceAlquiler = buscarIndiceAlquiler(alquileres, numero);
    if (indiceAlquiler < 0) {
        std::cout << "\n[ERROR] El alquiler Nro " << numero << " no existe.\n";
        pausarPantalla();
        return;
    }
    Alquiler& alquiler = alquileres[static_cast<std::size_t>(indiceAlquiler)];
    if (cadenaAString(alquiler.estado, LON_ESTADO) != EST_ALQ_ACTIVO) {
        std::cout << "\n[ERROR] Solo se pueden modificar alquileres en estado ACTIVO. "
                     "Estado actual: " << cadenaAString(alquiler.estado, LON_ESTADO) << ".\n";
        pausarPantalla();
        return;
    }

    std::string patenteActual = cadenaAString(alquiler.patente, LON_PATENTE);
    int indiceVehiculo = buscarIndiceVehiculo(vehiculos, patenteActual);
    if (indiceVehiculo < 0) {
        std::cout << "\n[ERROR] No se encontro el vehiculo con patente " << patenteActual
                  << " asociado al alquiler.\n";
        pausarPantalla();
        return;
    }

    const Vehiculo& vehiculoActual = vehiculos[static_cast<std::size_t>(indiceVehiculo)];
    std::cout << "\nAlquiler Nro: " << alquiler.numAlquiler
              << " | Cliente: " << alquiler.dni << " - "
              << nombreCliente(clientes, alquiler.dni) << "\n";
    std::cout << "Vehiculo: " << patenteActual << " | "
              << cadenaAString(vehiculoActual.marca, LON_MARCA) << " "
              << cadenaAString(vehiculoActual.modelo, LON_MODELO)
              << " | Tarifa/dia: $" << std::fixed << std::setprecision(2)
              << vehiculoActual.precioDia << "\n";
    std::cout << "Inicio: " << fechaAString(alquiler.fechaInicio)
              << " | Dias: " << alquiler.cantDias
              << " | Prevista: " << fechaAString(alquiler.fechaPrevista)
              << " | Total: $" << std::fixed << std::setprecision(2) << alquiler.importeTotal
              << " | Pagado: $" << alquiler.importePagado << "\n";
    std::cout << "--------------------------------------------------------------------\n";

    std::vector<Alquiler> alquileresPrevios = alquileres;
    std::vector<Vehiculo> vehiculosPrevios = vehiculos;

    bool cambiarVehiculo = leerSN("Desea cambiar el vehiculo asignado (S/N)?: ");
    if (finEntrada) {
        return;
    }
    if (cambiarVehiculo) {
        std::string nuevaPatente;
        if (!leerPatente("Ingrese la nueva patente: ", nuevaPatente)) {
            return;
        }
        int indiceNuevo = buscarIndiceVehiculo(vehiculos, nuevaPatente);
        if (indiceNuevo < 0) {
            std::cout << "\n[ERROR] El vehiculo con patente " << nuevaPatente
                      << " no existe.\n";
            pausarPantalla();
            return;
        }
        if (nuevaPatente == patenteActual) {
            std::cout << "\n[AVISO] La patente ingresada es la misma que la actual.\n";
        } else {
            const Vehiculo& vehiculoNuevo = vehiculos[static_cast<std::size_t>(indiceNuevo)];
            if (cadenaAString(vehiculoNuevo.estado, LON_ESTADO) != EST_VEH_DISPONIBLE) {
                std::cout << "\n[ERROR] El nuevo vehiculo no esta disponible. "
                             "Estado actual: " << cadenaAString(vehiculoNuevo.estado, LON_ESTADO)
                          << ".\n";
                pausarPantalla();
                return;
            }
            copiarCadena(vehiculos[static_cast<std::size_t>(indiceVehiculo)].estado,
                         LON_ESTADO, EST_VEH_DISPONIBLE);
            copiarCadena(vehiculos[static_cast<std::size_t>(indiceNuevo)].estado,
                         LON_ESTADO, EST_VEH_ALQUILADO);
            copiarCadena(alquiler.patente, LON_PATENTE, nuevaPatente);
            indiceVehiculo = indiceNuevo;
            patenteActual = nuevaPatente;
            std::cout << "\nVehiculo cambiado a " << nuevaPatente << " ("
                      << cadenaAString(vehiculos[static_cast<std::size_t>(indiceNuevo)].marca, LON_MARCA)
                      << " "
                      << cadenaAString(vehiculos[static_cast<std::size_t>(indiceNuevo)].modelo, LON_MODELO)
                      << ").\n";
        }
    }

    bool cambiarFecha = leerSN("Desea cambiar la fecha de inicio (S/N)?: ");
    if (finEntrada) {
        return;
    }
    if (cambiarFecha) {
        int primeraPago = primeraFechaPago(alquiler.numAlquiler);
        if (primeraPago < 0) {
            std::cout << "\n[ERROR] No se pudieron cargar los movimientos del alquiler.\n";
            pausarPantalla();
            return;
        }
        int fechaMinima = fechaActual();
        int fechaMaxima = sumarDias(fechaMinima, DIAS_MAX_INICIO_FUTURO);
        if (primeraPago > 0 && primeraPago < fechaMaxima) {
            fechaMaxima = primeraPago;
        }
        if (fechaMaxima < fechaMinima) {
            std::cout << "\n[ERROR] No se puede cambiar la fecha de inicio: ya existen "
                         "pagos registrados con fecha anterior a hoy ("
                      << fechaAString(primeraPago) << ").\n";
            pausarPantalla();
            return;
        }
        int nuevaFecha = 0;
        if (!leerFechaEnRango("Ingrese la nueva fecha de inicio (AAAAMMDD): ",
                              fechaMinima, fechaMaxima, nuevaFecha)) {
            return;
        }
        if (primeraPago > 0 && nuevaFecha > primeraPago) {
            std::cout << "\n[ERROR] La fecha de inicio no puede ser posterior al primer "
                         "pago registrado (" << fechaAString(primeraPago) << ").\n";
            pausarPantalla();
            return;
        }
        alquiler.fechaInicio = nuevaFecha;
    }

    bool cambiarDias = leerSN("Desea cambiar la cantidad de dias contratados (S/N)?: ");
    if (finEntrada) {
        return;
    }
    if (cambiarDias) {
        int nuevosDias = 0;
        if (!leerEntero("Ingrese la nueva cantidad de dias (1-" +
                            enteroAString(DIAS_MAX_ALQUILER) + "): ",
                        1, DIAS_MAX_ALQUILER, nuevosDias)) {
            return;
        }
        alquiler.cantDias = nuevosDias;
    }

    alquiler.fechaPrevista = sumarDias(alquiler.fechaInicio, alquiler.cantDias);
    const Vehiculo& vehiculoFinal = vehiculos[static_cast<std::size_t>(indiceVehiculo)];
    double nuevoTotal = alquiler.cantDias * vehiculoFinal.precioDia;
    if (nuevoTotal + TOLERANCIA_SALDO < alquiler.importePagado) {
        std::cout << "\n[ERROR] El nuevo importe total ($" << std::fixed << std::setprecision(2)
                  << nuevoTotal << ") no puede ser menor a lo ya pagado ($"
                  << alquiler.importePagado << ").\n";
        std::cout << "Modificacion cancelada.\n";
        pausarPantalla();
        return;
    }
    if (nuevoTotal < alquiler.importePagado) {
        nuevoTotal = alquiler.importePagado;
    }
    double diferencia = nuevoTotal - alquiler.importeTotal;
    alquiler.importeTotal = nuevoTotal;

    if (!guardarAlquilerYVehiculos(alquileres, vehiculos, alquileresPrevios, vehiculosPrevios)) {
        std::cout << "\n[ERROR] No se pudo guardar la modificacion del alquiler.\n";
        pausarPantalla();
        return;
    }

    if (std::fabs(diferencia) > TOLERANCIA_SALDO) {
        std::string error;
        if (!registrarMovimiento(alquiler.numAlquiler, MOV_AJUSTE, diferencia,
                                 fechaActual(), MEDIO_NINGUNO, error)) {
            std::cout << "[AVISO] El alquiler se modifico, pero no se pudo registrar "
                         "el movimiento de ajuste: " << error << "\n";
        }
    }

    std::cout << "\n>>> Alquiler modificado exitosamente.\n";
    std::cout << "Nueva Fecha Prevista: " << fechaALarga(alquiler.fechaPrevista) << "\n";
    std::cout << "Nuevo Importe Total:  $" << std::fixed << std::setprecision(2)
              << alquiler.importeTotal << "\n";
    pausarPantalla();
}

void listarAlquileres() {
    limpiarPantalla();
    std::cout << "\n====================================================================================="
                 "====================================================\n";
    std::cout << "                                           LISTADO GENERAL DE ALQUILERES\n";
    std::cout << "====================================================================================="
                 "====================================================\n";

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarPantalla();
        return;
    }
    if (alquileres.empty()) {
        std::cout << "No hay alquileres registrados.\n";
        pausarPantalla();
        return;
    }

    imprimirEncabezadoAlquileres();
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        imprimirFilaAlquiler(alquileres[i], clientes, vehiculos);
    }
    std::cout << "--------------------------------------------------------------------------------"
                 "-------------------------------------------------------------\n";
    std::cout << "Total de alquileres registrados: " << alquileres.size() << "\n";
    pausarPantalla();
}

void registrarDevolucion() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "        REGISTRAR DEVOLUCION\n";
    std::cout << "========================================\n";
    std::cout << "Seleccione metodo de identificacion:\n";
    std::cout << "1. Por patente de vehiculo\n";
    std::cout << "2. Por numero de alquiler\n";
    std::cout << "0. Cancelar\n";

    int metodo = 0;
    if (!leerEntero("Opcion: ", 0, 2, metodo)) {
        return;
    }
    if (metodo == 0) {
        return;
    }

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarPantalla();
        return;
    }

    int indiceAlquiler = -1;
    if (metodo == 1) {
        std::string patente;
        if (!leerPatente("Ingrese la patente del vehiculo devuelto: ", patente)) {
            return;
        }
        for (std::size_t i = 0; i < alquileres.size(); ++i) {
            if (cadenaAString(alquileres[i].patente, LON_PATENTE) == patente &&
                cadenaAString(alquileres[i].estado, LON_ESTADO) == EST_ALQ_ACTIVO) {
                indiceAlquiler = static_cast<int>(i);
                break;
            }
        }
        if (indiceAlquiler < 0) {
            std::cout << "\n[ERROR] No existe ningun alquiler ACTIVO para el vehiculo "
                         "con patente " << patente << ".\n";
            pausarPantalla();
            return;
        }
    } else {
        int numero = 0;
        if (!leerEntero("Ingrese el numero de alquiler: ", 1, 9999999, numero)) {
            return;
        }
        indiceAlquiler = buscarIndiceAlquiler(alquileres, numero);
        if (indiceAlquiler < 0) {
            std::cout << "\n[ERROR] El alquiler Nro " << numero << " no existe.\n";
            pausarPantalla();
            return;
        }
        if (cadenaAString(alquileres[static_cast<std::size_t>(indiceAlquiler)].estado,
                          LON_ESTADO) != EST_ALQ_ACTIVO) {
            std::cout << "\n[ERROR] El alquiler Nro " << numero
                      << " no se encuentra en estado ACTIVO. Estado actual: "
                      << cadenaAString(alquileres[static_cast<std::size_t>(indiceAlquiler)].estado,
                                       LON_ESTADO) << ".\n";
            pausarPantalla();
            return;
        }
    }

    Alquiler& alquiler = alquileres[static_cast<std::size_t>(indiceAlquiler)];
    std::string patente = cadenaAString(alquiler.patente, LON_PATENTE);
    int indiceVehiculo = buscarIndiceVehiculo(vehiculos, patente);
    if (indiceVehiculo < 0) {
        std::cout << "\n[ERROR] No se encontro el vehiculo con patente " << patente
                  << " asociado al alquiler.\n";
        pausarPantalla();
        return;
    }
    const Vehiculo& vehiculo = vehiculos[static_cast<std::size_t>(indiceVehiculo)];

    std::cout << "\nAlquiler ACTIVO localizado: Nro " << alquiler.numAlquiler << "\n";
    std::cout << "Cliente: " << alquiler.dni << " - "
              << nombreCliente(clientes, alquiler.dni) << "\n";
    std::cout << "Vehiculo: " << patente << " | "
              << cadenaAString(vehiculo.marca, LON_MARCA) << " "
              << cadenaAString(vehiculo.modelo, LON_MODELO)
              << " | Tarifa/dia: $" << std::fixed << std::setprecision(2)
              << vehiculo.precioDia << "\n";
    std::cout << "Fecha Inicio: " << fechaAString(alquiler.fechaInicio)
              << " | Fecha Prevista: " << fechaAString(alquiler.fechaPrevista) << "\n";
    std::cout << "--------------------------------------------------------------------\n";

    int fechaDevolucion = 0;
    if (alquiler.fechaInicio > fechaActual()) {
        std::cout << "\n[ERROR] El alquiler comienza el "
                  << fechaAString(alquiler.fechaInicio)
                  << ", por lo que no puede devolverse antes de esa fecha.\n";
        pausarPantalla();
        return;
    }
    if (!leerFechaEnRango("Ingrese la fecha efectiva de devolucion (AAAAMMDD): ",
                          alquiler.fechaInicio, fechaActual(), fechaDevolucion)) {
        return;
    }

    int diasAdicionales = diasEntre(alquiler.fechaPrevista, fechaDevolucion);
    double recargo = 0.0;
    if (diasAdicionales > 0) {
        recargo = diasAdicionales * vehiculo.precioDia;
        std::cout << "\n[AVISO] Se detectaron " << diasAdicionales
                  << " dia(s) de demora. Recargo: $" << std::fixed << std::setprecision(2)
                  << recargo << "\n";
    } else {
        std::cout << "\nVehiculo devuelto dentro del plazo o de forma anticipada. "
                     "Sin recargo.\n";
    }

    std::vector<Alquiler> alquileresPrevios = alquileres;
    std::vector<Vehiculo> vehiculosPrevios = vehiculos;

    alquiler.importeTotal += recargo;
    alquiler.fechaEfectiva = fechaDevolucion;
    copiarCadena(alquiler.estado, LON_ESTADO, EST_ALQ_FINALIZADO);
    copiarCadena(vehiculos[static_cast<std::size_t>(indiceVehiculo)].estado,
                 LON_ESTADO, EST_VEH_DISPONIBLE);

    if (!guardarAlquilerYVehiculos(alquileres, vehiculos, alquileresPrevios, vehiculosPrevios)) {
        std::cout << "\n[ERROR] No se pudo registrar la devolucion.\n";
        pausarPantalla();
        return;
    }

    if (recargo > TOLERANCIA_SALDO) {
        std::string error;
        if (!registrarMovimiento(alquiler.numAlquiler, MOV_AJUSTE, recargo,
                                 fechaDevolucion, MEDIO_NINGUNO, error)) {
            std::cout << "[AVISO] La devolucion se registro, pero no se pudo guardar "
                         "el movimiento de ajuste: " << error << "\n";
        }
    }

    std::cout << "\n========================================\n";
    std::cout << ">>> DEVOLUCION REGISTRADA EXITOSAMENTE\n";
    std::cout << "Alquiler Nro:            " << alquiler.numAlquiler << "\n";
    std::cout << "Fecha Efectiva:          " << fechaALarga(alquiler.fechaEfectiva) << "\n";
    std::cout << "Importe Total Final:     $" << std::fixed << std::setprecision(2)
              << alquiler.importeTotal << "\n";
    std::cout << "Importe Pagado:          $" << alquiler.importePagado << "\n";
    std::cout << "Saldo Pendiente:         $" << alquiler.importeTotal - alquiler.importePagado
              << "\n";
    std::cout << "Estado del Vehiculo:     " << EST_VEH_DISPONIBLE << "\n";
    std::cout << "========================================\n";
    pausarPantalla();
}

void cancelarAlquiler() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "          CANCELAR ALQUILER\n";
    std::cout << "========================================\n";

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarPantalla();
        return;
    }

    int numero = 0;
    if (!leerEntero("Ingrese el numero de alquiler a cancelar: ", 1, 9999999, numero)) {
        return;
    }
    int indiceAlquiler = buscarIndiceAlquiler(alquileres, numero);
    if (indiceAlquiler < 0) {
        std::cout << "\n[ERROR] El alquiler Nro " << numero << " no existe.\n";
        pausarPantalla();
        return;
    }
    Alquiler& alquiler = alquileres[static_cast<std::size_t>(indiceAlquiler)];
    if (cadenaAString(alquiler.estado, LON_ESTADO) != EST_ALQ_ACTIVO) {
        std::cout << "\n[ERROR] Solo se pueden cancelar alquileres en estado ACTIVO. "
                     "Estado actual: " << cadenaAString(alquiler.estado, LON_ESTADO) << ".\n";
        pausarPantalla();
        return;
    }

    std::string patente = cadenaAString(alquiler.patente, LON_PATENTE);
    int indiceVehiculo = buscarIndiceVehiculo(vehiculos, patente);
    if (indiceVehiculo < 0) {
        std::cout << "\n[ERROR] No se encontro el vehiculo con patente " << patente
                  << " asociado al alquiler.\n";
        pausarPantalla();
        return;
    }

    std::cout << "\nAlquiler Nro: " << alquiler.numAlquiler
              << " | Cliente: " << alquiler.dni << " - "
              << nombreCliente(clientes, alquiler.dni) << "\n";
    std::cout << "Vehiculo: " << patente << " | "
              << cadenaAString(vehiculos[static_cast<std::size_t>(indiceVehiculo)].marca, LON_MARCA)
              << " "
              << cadenaAString(vehiculos[static_cast<std::size_t>(indiceVehiculo)].modelo, LON_MODELO)
              << "\n";
    std::cout << "Importe Total: $" << std::fixed << std::setprecision(2)
              << alquiler.importeTotal << " | Importe Pagado: $" << alquiler.importePagado
              << "\n";
    std::cout << "--------------------------------------------------------------------\n";

    if (!leerSN("Desea confirmar la cancelacion del alquiler (S/N)?: ")) {
        if (finEntrada) {
            return;
        }
        std::cout << "\nOperacion cancelada por el usuario.\n";
        pausarPantalla();
        return;
    }

    std::vector<Alquiler> alquileresPrevios = alquileres;
    std::vector<Vehiculo> vehiculosPrevios = vehiculos;

    double totalAnterior = alquiler.importeTotal;
    alquiler.importeTotal = alquiler.importePagado;
    copiarCadena(alquiler.estado, LON_ESTADO, EST_ALQ_CANCELADO);
    copiarCadena(vehiculos[static_cast<std::size_t>(indiceVehiculo)].estado,
                 LON_ESTADO, EST_VEH_DISPONIBLE);

    if (!guardarAlquilerYVehiculos(alquileres, vehiculos, alquileresPrevios, vehiculosPrevios)) {
        std::cout << "\n[ERROR] No se pudo cancelar el alquiler.\n";
        pausarPantalla();
        return;
    }

    if (totalAnterior - alquiler.importePagado > TOLERANCIA_SALDO) {
        double ajuste = alquiler.importePagado - totalAnterior;
        std::string error;
        if (!registrarMovimiento(alquiler.numAlquiler, MOV_AJUSTE, ajuste,
                                 fechaActual(), MEDIO_NINGUNO, error)) {
            std::cout << "[AVISO] El alquiler se cancelo, pero no se pudo registrar "
                         "el movimiento de ajuste: " << error << "\n";
        }
    }

    std::cout << "\n========================================\n";
    std::cout << ">>> ALQUILER CANCELADO EXITOSAMENTE\n";
    std::cout << "Estado del Alquiler: " << EST_ALQ_CANCELADO << "\n";
    std::cout << "Importe Retenido:    $" << std::fixed << std::setprecision(2)
              << alquiler.importePagado << "\n";
    std::cout << "Saldo Pendiente:     $0.00\n";
    std::cout << "Estado del Vehiculo: " << EST_VEH_DISPONIBLE << "\n";
    std::cout << "========================================\n";
    pausarPantalla();
}

void submenuAlquileres() {
    int opcion = -1;
    do {
        limpiarPantalla();
        std::cout << "\n========================================\n";
        std::cout << "           SUBMENU ALQUILERES\n";
        std::cout << "========================================\n";
        std::cout << "1. Realizar nuevo alquiler\n";
        std::cout << "2. Modificar alquiler\n";
        std::cout << "3. Listar alquileres\n";
        std::cout << "4. Cancelar alquiler\n";
        std::cout << "0. Volver al Menu Principal\n";
        std::cout << "========================================\n";
        if (!leerEntero("Seleccione una opcion (0-4): ", 0, 4, opcion)) {
            break;
        }
        switch (opcion) {
            case 1: realizarAlquiler(); break;
            case 2: modificarAlquiler(); break;
            case 3: listarAlquileres(); break;
            case 4: cancelarAlquiler(); break;
            case 0: break;
            default:
                std::cout << "\n[ERROR] Opcion no valida.\n";
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

#include "movimientos.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "alquileres.h"
#include "archivos.h"
#include "clientes.h"
#include "fechas.h"
#include "utils.h"

namespace {

bool escribirArchivoCliente(int dni, const std::vector<Movimiento>& movimientos,
                            std::string& error) {
    std::string ruta = nombreArchivoMovimientos(dni);
    std::ofstream archivo(ruta.c_str(), std::ios::trunc);
    if (!archivo.is_open()) {
        error = "No se pudo crear o sobrescribir el archivo " + ruta + ".";
        return false;
    }
    archivo << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        const Movimiento& m = movimientos[i];
        if (m.dni != dni) {
            continue;
        }
        archivo << m.numMovimiento << ';'
                << m.numAlquiler << ';'
                << cadenaAString(m.tipoMov, LON_TIPO_MOV) << ';'
                << m.monto << ';'
                << m.fecha << ';'
                << m.dni << ';'
                << cadenaAString(m.patente, LON_PATENTE) << ';'
                << cadenaAString(m.medioPago, LON_MEDIO_PAGO) << '\n';
    }
    archivo.flush();
    if (!archivo.good()) {
        error = "No se pudo escribir correctamente en el archivo " + ruta + ".";
        return false;
    }
    archivo.close();
    return true;
}

} /* namespace */

bool cargarMovimientos(std::vector<Movimiento>& movimientos) {
    std::string error;
    if (!cargarRegistros(ARCH_MOVIMIENTOS, movimientos, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        movimientos[i].tipoMov[LON_TIPO_MOV - 1] = '\0';
        movimientos[i].patente[LON_PATENTE - 1] = '\0';
        movimientos[i].medioPago[LON_MEDIO_PAGO - 1] = '\0';
    }
    return true;
}

bool guardarMovimientos(const std::vector<Movimiento>& movimientos) {
    std::string error;
    if (!guardarRegistros(ARCH_MOVIMIENTOS, movimientos, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    return true;
}

int proximoNumeroMovimiento() {
    std::vector<Movimiento> movimientos;
    if (!cargarMovimientos(movimientos)) {
        return 1;
    }
    int maximo = 0;
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        if (movimientos[i].numMovimiento > maximo) {
            maximo = movimientos[i].numMovimiento;
        }
    }
    return maximo + 1;
}

std::string nombreArchivoMovimientos(int dni) {
    return "movimientos_" + enteroAString(dni) + ".txt";
}

bool regenerarArchivoMovimientosCliente(int dni, std::string& error) {
    std::vector<Movimiento> movimientos;
    if (!cargarMovimientos(movimientos)) {
        error = "No se pudo cargar " + std::string(ARCH_MOVIMIENTOS) + ".";
        return false;
    }
    return escribirArchivoCliente(dni, movimientos, error);
}

bool regenerarTodosLosArchivosTexto(std::string& error) {
    std::vector<Cliente> clientes;
    std::vector<Movimiento> movimientos;
    if (!cargarClientes(clientes)) {
        error = "No se pudo cargar " + std::string(ARCH_CLIENTES) + ".";
        return false;
    }
    if (!cargarMovimientos(movimientos)) {
        error = "No se pudo cargar " + std::string(ARCH_MOVIMIENTOS) + ".";
        return false;
    }

    std::vector<int> dnis;
    for (std::size_t i = 0; i < clientes.size(); ++i) {
        bool repetido = false;
        for (std::size_t j = 0; j < dnis.size(); ++j) {
            if (dnis[j] == clientes[i].dni) {
                repetido = true;
                break;
            }
        }
        if (!repetido) {
            dnis.push_back(clientes[i].dni);
        }
    }
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        bool repetido = false;
        for (std::size_t j = 0; j < dnis.size(); ++j) {
            if (dnis[j] == movimientos[i].dni) {
                repetido = true;
                break;
            }
        }
        if (!repetido) {
            dnis.push_back(movimientos[i].dni);
        }
    }

    for (std::size_t i = 0; i < dnis.size(); ++i) {
        std::string errorCliente;
        if (!escribirArchivoCliente(dnis[i], movimientos, errorCliente)) {
            error = errorCliente;
            return false;
        }
    }
    return true;
}

bool registrarMovimiento(int numAlquiler, const std::string& tipo, double monto,
                         int fecha, const std::string& medio, std::string& error) {
    if (!validarTipoMovimiento(tipo)) {
        error = "El tipo de movimiento debe ser 'Pago' o 'Ajuste'.";
        return false;
    }
    if (!esFinito(monto)) {
        error = "El monto del movimiento no es un numero valido.";
        return false;
    }
    if (tipo == MOV_PAGO) {
        if (monto <= 0.0) {
            error = "El monto del pago debe ser mayor a cero.";
            return false;
        }
        if (medio != MEDIO_EFECTIVO && medio != MEDIO_TARJETA &&
            medio != MEDIO_TRANSFERENCIA) {
            error = "El medio de pago debe ser Efectivo, Tarjeta o Transferencia.";
            return false;
        }
    } else {
        if (std::fabs(monto) < TOLERANCIA_SALDO) {
            error = "El monto del ajuste no puede ser cero.";
            return false;
        }
    }
    if (!esFechaValida(fecha)) {
        error = "La fecha del movimiento no es valida.";
        return false;
    }
    if (!esFechaPasadaOHoy(fecha)) {
        error = "La fecha del movimiento no puede ser posterior a la fecha actual.";
        return false;
    }

    std::vector<Alquiler> alquileres;
    if (!cargarAlquileres(alquileres)) {
        error = "No se pudo cargar " + std::string(ARCH_ALQUILERES) + ".";
        return false;
    }
    int indiceAlquiler = buscarIndiceAlquiler(alquileres, numAlquiler);
    if (indiceAlquiler < 0) {
        error = "No se encontro el alquiler Nro " + enteroAString(numAlquiler) + ".";
        return false;
    }
    const Alquiler& alquiler = alquileres[static_cast<std::size_t>(indiceAlquiler)];

    if (tipo == MOV_PAGO && fecha < alquiler.fechaInicio) {
        error = "La fecha del pago no puede ser anterior al inicio del alquiler (" +
                fechaAString(alquiler.fechaInicio) + ").";
        return false;
    }

    std::vector<Movimiento> movimientos;
    if (!cargarMovimientos(movimientos)) {
        error = "No se pudo cargar " + std::string(ARCH_MOVIMIENTOS) + ".";
        return false;
    }

    Movimiento movimiento = Movimiento();
    int maximo = 0;
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        if (movimientos[i].numMovimiento > maximo) {
            maximo = movimientos[i].numMovimiento;
        }
    }
    movimiento.numMovimiento = maximo + 1;
    movimiento.numAlquiler = numAlquiler;
    copiarCadena(movimiento.tipoMov, LON_TIPO_MOV, tipo);
    movimiento.monto = monto;
    movimiento.fecha = fecha;
    movimiento.dni = alquiler.dni;
    copiarCadena(movimiento.patente, LON_PATENTE,
                 cadenaAString(alquiler.patente, LON_PATENTE));
    copiarCadena(movimiento.medioPago, LON_MEDIO_PAGO,
                 tipo == MOV_PAGO ? medio : MEDIO_NINGUNO);

    movimientos.push_back(movimiento);
    if (!guardarMovimientos(movimientos)) {
        error = "No se pudo guardar el movimiento en " + std::string(ARCH_MOVIMIENTOS) + ".";
        return false;
    }

    std::string errorTexto;
    if (!regenerarArchivoMovimientosCliente(movimiento.dni, errorTexto)) {
        std::cout << "[AVISO] El movimiento se guardo, pero no se pudo actualizar "
                     "el archivo de texto del cliente: " << errorTexto << "\n";
    }
    return true;
}

void registrarPago() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "            REGISTRAR PAGO\n";
    std::cout << "========================================\n";

    std::vector<Cliente> clientes;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarAlquileres(alquileres)) {
        pausarPantalla();
        return;
    }

    int numero = 0;
    if (!leerEntero("Ingrese el numero de alquiler a pagar: ", 1, 9999999, numero)) {
        return;
    }
    int indice = buscarIndiceAlquiler(alquileres, numero);
    if (indice < 0) {
        std::cout << "\n[ERROR] El alquiler Nro " << numero << " no existe en el sistema.\n";
        pausarPantalla();
        return;
    }
    Alquiler& alquiler = alquileres[static_cast<std::size_t>(indice)];
    if (cadenaAString(alquiler.estado, LON_ESTADO) == EST_ALQ_CANCELADO) {
        std::cout << "\n[ERROR] No se pueden registrar pagos para alquileres CANCELADOS.\n";
        pausarPantalla();
        return;
    }

    double saldoPendiente = alquiler.importeTotal - alquiler.importePagado;
    if (saldoPendiente <= TOLERANCIA_SALDO) {
        std::cout << "\n[AVISO] El alquiler Nro " << numero
                  << " ya se encuentra totalmente saldado.\n";
        std::cout << "Importe Total:  $" << std::fixed << std::setprecision(2)
                  << alquiler.importeTotal << "\n";
        std::cout << "Importe Pagado: $" << alquiler.importePagado << "\n";
        pausarPantalla();
        return;
    }

    int indiceCliente = buscarIndiceCliente(clientes, alquiler.dni);
    std::string nombre = indiceCliente >= 0
                             ? cadenaAString(clientes[static_cast<std::size_t>(indiceCliente)].apellidoNombre,
                                             LON_NOMBRE)
                             : "(Desconocido)";
    std::cout << "\n--- DATOS DEL ALQUILER ---\n";
    std::cout << "Nro Alquiler:    " << alquiler.numAlquiler << "\n";
    std::cout << "Cliente:         " << alquiler.dni << " - " << nombre << "\n";
    std::cout << "Patente:         " << cadenaAString(alquiler.patente, LON_PATENTE) << "\n";
    std::cout << "Fecha de Inicio: " << fechaALarga(alquiler.fechaInicio) << "\n";
    std::cout << "Importe Total:   $" << std::fixed << std::setprecision(2)
              << alquiler.importeTotal << "\n";
    std::cout << "Importe Pagado:  $" << alquiler.importePagado << "\n";
    std::cout << "SALDO PENDIENTE: $" << saldoPendiente << "\n";
    std::cout << "---------------------------\n";

    double monto = 0.0;
    while (true) {
        if (!leerDouble("Ingrese el monto a pagar: ", 0.0, monto)) {
            return;
        }
        if (monto > saldoPendiente + TOLERANCIA_SALDO) {
            std::cout << "[ERROR] El monto no puede superar el saldo pendiente ($"
                      << std::fixed << std::setprecision(2) << saldoPendiente
                      << "). Reintente.\n";
            continue;
        }
        if (monto > saldoPendiente) {
            monto = saldoPendiente;
        }
        break;
    }

    std::cout << "\nSeleccione medio de pago:\n";
    std::cout << "1. " << MEDIO_EFECTIVO << "\n";
    std::cout << "2. " << MEDIO_TARJETA << "\n";
    std::cout << "3. " << MEDIO_TRANSFERENCIA << "\n";
    int opcionMedio = 1;
    if (!leerEntero("Opcion: ", 1, 3, opcionMedio)) {
        return;
    }
    std::string medio = MEDIO_EFECTIVO;
    if (opcionMedio == 2) {
        medio = MEDIO_TARJETA;
    } else if (opcionMedio == 3) {
        medio = MEDIO_TRANSFERENCIA;
    }

    int fechaPago = 0;
    if (alquiler.fechaInicio > fechaActual()) {
        std::cout << "\n[ERROR] El alquiler comienza el "
                  << fechaAString(alquiler.fechaInicio)
                  << "; no se pueden registrar pagos antes de esa fecha.\n";
        pausarPantalla();
        return;
    }
    if (!leerFechaEnRango("Ingrese la fecha del pago (AAAAMMDD): ",
                          alquiler.fechaInicio, fechaActual(), fechaPago)) {
        return;
    }

    double importePagadoAnterior = alquiler.importePagado;
    alquiler.importePagado += monto;
    if (!guardarAlquileres(alquileres)) {
        std::cout << "\n[ERROR] No se pudo actualizar el importe pagado en "
                  << ARCH_ALQUILERES << ".\n";
        pausarPantalla();
        return;
    }

    std::string error;
    if (!registrarMovimiento(alquiler.numAlquiler, MOV_PAGO, monto, fechaPago, medio, error)) {
        alquiler.importePagado = importePagadoAnterior;
        guardarAlquileres(alquileres);
        std::cout << "\n[ERROR] No se pudo registrar el pago: " << error << "\n";
        std::cout << "El importe pagado fue revertido.\n";
        pausarPantalla();
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << ">>> PAGO REGISTRADO EXITOSAMENTE\n";
    std::cout << "Alquiler Nro:          " << alquiler.numAlquiler << "\n";
    std::cout << "Monto Abonado:         $" << std::fixed << std::setprecision(2)
              << monto << "\n";
    std::cout << "Medio de Pago:         " << medio << "\n";
    std::cout << "Nuevo Saldo Pendiente: $" << alquiler.importeTotal - alquiler.importePagado
              << "\n";
    std::cout << "========================================\n";
    pausarPantalla();
}

void listarMovimientos() {
    limpiarPantalla();
    std::cout << "\n========================================================================================\n";
    std::cout << "                        LISTADO GENERAL DE MOVIMIENTOS\n";
    std::cout << "========================================================================================\n";

    std::vector<Movimiento> movimientos;
    if (!cargarMovimientos(movimientos)) {
        pausarPantalla();
        return;
    }
    if (movimientos.empty()) {
        std::cout << "No hay movimientos registrados.\n";
        pausarPantalla();
        return;
    }

    std::cout << std::left << std::setw(6) << "NRO" << " | "
              << std::setw(6) << "ALQ" << " | "
              << std::setw(8) << "TIPO" << " | "
              << std::setw(12) << "MONTO" << " | "
              << std::setw(10) << "FECHA" << " | "
              << std::setw(10) << "DNI" << " | "
              << std::setw(10) << "PATENTE" << " | "
              << "MEDIO\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    double totalPagos = 0.0;
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        const Movimiento& m = movimientos[i];
        std::cout << std::left << std::setw(6) << m.numMovimiento << " | "
                  << std::setw(6) << m.numAlquiler << " | "
                  << std::setw(8) << cadenaAString(m.tipoMov, LON_TIPO_MOV) << " | $"
                  << std::setw(11) << std::fixed << std::setprecision(2) << m.monto << " | "
                  << std::setw(10) << fechaAString(m.fecha) << " | "
                  << std::setw(10) << m.dni << " | "
                  << std::setw(10) << cadenaAString(m.patente, LON_PATENTE) << " | "
                  << cadenaAString(m.medioPago, LON_MEDIO_PAGO) << "\n";
        if (cadenaAString(m.tipoMov, LON_TIPO_MOV) == MOV_PAGO) {
            totalPagos += m.monto;
        }
    }
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "Total de movimientos: " << movimientos.size()
              << " | Suma total de pagos: $" << std::fixed << std::setprecision(2)
              << totalPagos << "\n";
    pausarPantalla();
}

void submenuMovimientos() {
    int opcion = -1;
    do {
        limpiarPantalla();
        std::cout << "\n========================================\n";
        std::cout << "             SUBMENU PAGOS\n";
        std::cout << "========================================\n";
        std::cout << "1. Registrar pago\n";
        std::cout << "2. Listar todos los movimientos\n";
        std::cout << "0. Volver al Menu Principal\n";
        std::cout << "========================================\n";
        if (!leerEntero("Seleccione una opcion (0-2): ", 0, 2, opcion)) {
            break;
        }
        switch (opcion) {
            case 1: registrarPago(); break;
            case 2: listarMovimientos(); break;
            case 0: break;
            default:
                std::cout << "\n[ERROR] Opcion no valida.\n";
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

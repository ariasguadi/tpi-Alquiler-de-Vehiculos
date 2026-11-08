#include "reportes.h"

#include <iomanip>
#include <iostream>

#include "alquileres.h"
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

void pausarReporte() {
    pausarPantalla();
}

struct DeudaAcumulada {
    int dni;
    int cantidadAlquileres;
    double total;
    double pagado;
    double saldo;
    std::string alquileres;
};

} /* namespace */

void reporteVehiculosDisponibles() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                       REPORTE 1: VEHICULOS DISPONIBLES\n";
    std::cout << "=====================================================================================\n";

    std::vector<Vehiculo> vehiculos;
    if (!cargarVehiculos(vehiculos)) {
        pausarReporte();
        return;
    }

    std::cout << std::left << std::setw(9) << "PATENTE" << " | "
              << std::setw(12) << "MARCA" << " | "
              << std::setw(14) << "MODELO" << " | "
              << std::setw(6) << "ANIO" << " | "
              << std::setw(12) << "TIPO" << " | "
              << std::setw(5) << "CAP." << " | "
              << "PRECIO/DIA\n";
    std::cout << "-------------------------------------------------------------------------------------\n";

    int cantidad = 0;
    for (std::size_t i = 0; i < vehiculos.size(); ++i) {
        const Vehiculo& v = vehiculos[i];
        if (cadenaAString(v.estado, LON_ESTADO) != EST_VEH_DISPONIBLE) {
            continue;
        }
        std::cout << std::left << std::setw(9) << cadenaAString(v.patente, LON_PATENTE) << " | "
                  << std::setw(12) << limitar(cadenaAString(v.marca, LON_MARCA), 12) << " | "
                  << std::setw(14) << limitar(cadenaAString(v.modelo, LON_MODELO), 14) << " | "
                  << std::setw(6) << v.anio << " | "
                  << std::setw(12) << limitar(cadenaAString(v.tipo, LON_TIPO_VEH), 12) << " | "
                  << std::setw(5) << v.capacidad << " | $"
                  << std::fixed << std::setprecision(2) << v.precioDia << "\n";
        ++cantidad;
    }
    std::cout << "-------------------------------------------------------------------------------------\n";
    if (cantidad == 0) {
        std::cout << "No hay vehiculos disponibles en este momento.\n";
    } else {
        std::cout << "Total de vehiculos disponibles: " << cantidad << "\n";
    }
    pausarReporte();
}

void reporteVehiculosAlquilados() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                        REPORTE 2: VEHICULOS ALQUILADOS\n";
    std::cout << "=====================================================================================\n";

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }

    std::cout << std::left << std::setw(9) << "PATENTE" << " | "
              << std::setw(20) << "MARCA Y MODELO" << " | "
              << std::setw(30) << "CLIENTE" << " | "
              << std::setw(10) << "INICIO" << " | "
              << "PREVISTA\n";
    std::cout << "-------------------------------------------------------------------------------------\n";

    int cantidad = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        const Alquiler& a = alquileres[i];
        if (cadenaAString(a.estado, LON_ESTADO) != EST_ALQ_ACTIVO) {
            continue;
        }
        std::string patente = cadenaAString(a.patente, LON_PATENTE);
        std::string cliente = enteroAString(a.dni) + " - " + nombreCliente(clientes, a.dni);
        std::cout << std::left << std::setw(9) << patente << " | "
                  << std::setw(20) << limitar(descripcionVehiculo(vehiculos, patente), 20) << " | "
                  << std::setw(30) << limitar(cliente, 30) << " | "
                  << std::setw(10) << fechaAString(a.fechaInicio) << " | "
                  << fechaAString(a.fechaPrevista) << "\n";
        ++cantidad;
    }
    std::cout << "-------------------------------------------------------------------------------------\n";
    if (cantidad == 0) {
        std::cout << "No hay vehiculos alquilados en este momento.\n";
    } else {
        std::cout << "Total de vehiculos actualmente alquilados: " << cantidad << "\n";
    }
    pausarReporte();
}

void reporteAlquileresPorRango() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                 REPORTE 3: ALQUILERES POR RANGO DE FECHAS\n";
    std::cout << "=====================================================================================\n";

    int fechaInicial = 0;
    int fechaFinal = 0;
    int fechaMaximaReporte = sumarDias(fechaActual(), DIAS_MAX_INICIO_FUTURO);
    if (!leerFechaEnRango("Ingrese la fecha inicial del rango (AAAAMMDD): ",
                          19000101, fechaMaximaReporte, fechaInicial)) {
        return;
    }
    if (!leerFechaEnRango("Ingrese la fecha final del rango (AAAAMMDD): ",
                          fechaInicial, fechaMaximaReporte, fechaFinal)) {
        return;
    }

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }

    std::cout << "\nAlquileres cuya fecha de inicio esta entre "
              << fechaAString(fechaInicial) << " y " << fechaAString(fechaFinal) << ":\n\n";
    imprimirEncabezadoAlquileres();
    int cantidad = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (alquileres[i].fechaInicio >= fechaInicial &&
            alquileres[i].fechaInicio <= fechaFinal) {
            imprimirFilaAlquiler(alquileres[i], clientes, vehiculos);
            ++cantidad;
        }
    }
    std::cout << "--------------------------------------------------------------------------------"
                 "-------------------------------------------------------------\n";
    std::cout << "Total de alquileres encontrados en el rango: " << cantidad << "\n";
    pausarReporte();
}

void reporteAlquileresPorCliente() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                    REPORTE 4: ALQUILERES POR CLIENTE\n";
    std::cout << "=====================================================================================\n";

    int dni = 0;
    if (!leerDNI("Ingrese el DNI del cliente: ", dni)) {
        return;
    }

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }
    int indice = buscarIndiceCliente(clientes, dni);
    if (indice < 0) {
        std::cout << "\n[ERROR] El cliente con DNI " << dni << " no existe en el sistema.\n";
        pausarReporte();
        return;
    }
    const Cliente& cliente = clientes[static_cast<std::size_t>(indice)];
    std::cout << "\nCliente: " << cadenaAString(cliente.apellidoNombre, LON_NOMBRE)
              << " (DNI: " << cliente.dni << ") | Telefono: "
              << cadenaAString(cliente.telefono, LON_TEL) << " | Estado: "
              << (cliente.estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO") << "\n\n";

    imprimirEncabezadoAlquileres();
    int cantidad = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (alquileres[i].dni == dni) {
            imprimirFilaAlquiler(alquileres[i], clientes, vehiculos);
            ++cantidad;
        }
    }
    std::cout << "--------------------------------------------------------------------------------"
                 "-------------------------------------------------------------\n";
    std::cout << "Total de alquileres registrados para este cliente: " << cantidad << "\n";
    pausarReporte();
}

void reporteAlquileresPorVehiculo() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                   REPORTE 5: ALQUILERES POR VEHICULO\n";
    std::cout << "=====================================================================================\n";

    std::string patente;
    if (!leerPatente("Ingrese la patente del vehiculo: ", patente)) {
        return;
    }

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }
    int indice = buscarIndiceVehiculo(vehiculos, patente);
    if (indice < 0) {
        std::cout << "\n[ERROR] El vehiculo con patente " << patente
                  << " no existe en el sistema.\n";
        pausarReporte();
        return;
    }
    const Vehiculo& vehiculo = vehiculos[static_cast<std::size_t>(indice)];
    std::cout << "\nVehiculo: " << cadenaAString(vehiculo.marca, LON_MARCA) << " "
              << cadenaAString(vehiculo.modelo, LON_MODELO)
              << " (Patente: " << patente << ") | Tipo: "
              << cadenaAString(vehiculo.tipo, LON_TIPO_VEH) << " | Tarifa/dia: $"
              << std::fixed << std::setprecision(2) << vehiculo.precioDia
              << " | Estado: " << cadenaAString(vehiculo.estado, LON_ESTADO) << "\n\n";

    imprimirEncabezadoAlquileres();
    int cantidad = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (cadenaAString(alquileres[i].patente, LON_PATENTE) == patente) {
            imprimirFilaAlquiler(alquileres[i], clientes, vehiculos);
            ++cantidad;
        }
    }
    std::cout << "--------------------------------------------------------------------------------"
                 "-------------------------------------------------------------\n";
    std::cout << "Total de alquileres historicos para este vehiculo: " << cantidad << "\n";
    pausarReporte();
}

void reporteAlquileresPorEstado() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "     REPORTE 6: ALQUILERES POR ESTADO\n";
    std::cout << "========================================\n";
    std::cout << "1. Activos\n";
    std::cout << "2. Finalizados\n";
    std::cout << "3. Cancelados\n";
    std::cout << "0. Cancelar\n";

    int opcion = 0;
    if (!leerEntero("Seleccione un estado: ", 0, 3, opcion)) {
        return;
    }
    if (opcion == 0) {
        return;
    }
    std::string estado = EST_ALQ_ACTIVO;
    if (opcion == 2) {
        estado = EST_ALQ_FINALIZADO;
    } else if (opcion == 3) {
        estado = EST_ALQ_CANCELADO;
    }

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }

    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                 LISTADO DE ALQUILERES EN ESTADO: " << estado << "\n";
    std::cout << "=====================================================================================\n";
    imprimirEncabezadoAlquileres();
    int cantidad = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        if (cadenaAString(alquileres[i].estado, LON_ESTADO) == estado) {
            imprimirFilaAlquiler(alquileres[i], clientes, vehiculos);
            ++cantidad;
        }
    }
    std::cout << "--------------------------------------------------------------------------------"
                 "-------------------------------------------------------------\n";
    std::cout << "Total de alquileres en estado " << estado << ": " << cantidad << "\n";
    pausarReporte();
}

void reporteAlquileresPorImporte() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "    REPORTE 7: ALQUILERES POR IMPORTE\n";
    std::cout << "========================================\n";

    double montoReferencia = 0.0;
    if (!leerDouble("Ingrese el importe de referencia: ", 0.0, montoReferencia)) {
        return;
    }
    std::cout << "\nSeleccione el criterio de comparacion:\n";
    std::cout << "1. Superiores al importe ingresado (> $" << std::fixed << std::setprecision(2)
              << montoReferencia << ")\n";
    std::cout << "2. Inferiores al importe ingresado (< $" << montoReferencia << ")\n";
    int criterio = 1;
    if (!leerEntero("Opcion: ", 1, 2, criterio)) {
        return;
    }

    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }

    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    if (criterio == 1) {
        std::cout << "              ALQUILERES CON IMPORTE TOTAL SUPERIOR A $"
                  << std::fixed << std::setprecision(2) << montoReferencia << "\n";
    } else {
        std::cout << "              ALQUILERES CON IMPORTE TOTAL INFERIOR A $"
                  << std::fixed << std::setprecision(2) << montoReferencia << "\n";
    }
    std::cout << "=====================================================================================\n";
    imprimirEncabezadoAlquileres();
    int cantidad = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        bool coincide = (criterio == 1 && alquileres[i].importeTotal > montoReferencia) ||
                        (criterio == 2 && alquileres[i].importeTotal < montoReferencia);
        if (coincide) {
            imprimirFilaAlquiler(alquileres[i], clientes, vehiculos);
            ++cantidad;
        }
    }
    std::cout << "--------------------------------------------------------------------------------"
                 "-------------------------------------------------------------\n";
    std::cout << "Total de alquileres que cumplen la condicion: " << cantidad << "\n";
    pausarReporte();
}

void reportePagosPorPeriodo() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                  REPORTE 8: PAGOS REALIZADOS POR PERIODO\n";
    std::cout << "=====================================================================================\n";

    int fechaInicial = 0;
    int fechaFinal = 0;
    int fechaHoy = fechaActual();
    if (!leerFechaEnRango("Ingrese la fecha inicial del periodo (AAAAMMDD): ",
                          19000101, fechaHoy, fechaInicial)) {
        return;
    }
    if (!leerFechaEnRango("Ingrese la fecha final del periodo (AAAAMMDD): ",
                          fechaInicial, fechaHoy, fechaFinal)) {
        return;
    }

    std::vector<Movimiento> movimientos;
    if (!cargarMovimientos(movimientos)) {
        pausarReporte();
        return;
    }

    std::cout << "\nPagos registrados entre " << fechaAString(fechaInicial)
              << " y " << fechaAString(fechaFinal) << ":\n\n";
    std::cout << std::left << std::setw(6) << "MOV" << " | "
              << std::setw(6) << "ALQ" << " | "
              << std::setw(10) << "DNI" << " | "
              << std::setw(10) << "FECHA" << " | "
              << std::setw(12) << "MONTO" << " | "
              << std::setw(10) << "PATENTE" << " | "
              << "MEDIO\n";
    std::cout << "-------------------------------------------------------------------------------------\n";

    int cantidad = 0;
    double totalPeriodo = 0.0;
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        const Movimiento& m = movimientos[i];
        if (cadenaAString(m.tipoMov, LON_TIPO_MOV) == MOV_PAGO &&
            m.fecha >= fechaInicial && m.fecha <= fechaFinal) {
            std::cout << std::left << std::setw(6) << m.numMovimiento << " | "
                      << std::setw(6) << m.numAlquiler << " | "
                      << std::setw(10) << m.dni << " | "
                      << std::setw(10) << fechaAString(m.fecha) << " | $"
                      << std::setw(11) << std::fixed << std::setprecision(2) << m.monto << " | "
                      << std::setw(10) << cadenaAString(m.patente, LON_PATENTE) << " | "
                      << cadenaAString(m.medioPago, LON_MEDIO_PAGO) << "\n";
            totalPeriodo += m.monto;
            ++cantidad;
        }
    }
    std::cout << "-------------------------------------------------------------------------------------\n";
    std::cout << "Cantidad de pagos en el periodo: " << cantidad
              << " | Monto total acumulado: $" << std::fixed << std::setprecision(2)
              << totalPeriodo << "\n";
    pausarReporte();
}

void reporteClientesConDeuda() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                      REPORTE 9: CLIENTES CON DEUDA\n";
    std::cout << "=====================================================================================\n";

    std::vector<Cliente> clientes;
    std::vector<Alquiler> alquileres;
    if (!cargarClientes(clientes) || !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }

    std::vector<DeudaAcumulada> deudas;
    int totalAlquileresConDeuda = 0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        const Alquiler& a = alquileres[i];
        if (cadenaAString(a.estado, LON_ESTADO) == EST_ALQ_CANCELADO) {
            continue;
        }
        double saldo = a.importeTotal - a.importePagado;
        if (saldo <= TOLERANCIA_SALDO) {
            continue;
        }

        int posicion = -1;
        for (std::size_t j = 0; j < deudas.size(); ++j) {
            if (deudas[j].dni == a.dni) {
                posicion = static_cast<int>(j);
                break;
            }
        }
        if (posicion < 0) {
            DeudaAcumulada nueva;
            nueva.dni = a.dni;
            nueva.cantidadAlquileres = 0;
            nueva.total = 0.0;
            nueva.pagado = 0.0;
            nueva.saldo = 0.0;
            deudas.push_back(nueva);
            posicion = static_cast<int>(deudas.size()) - 1;
        }

        DeudaAcumulada& deuda = deudas[static_cast<std::size_t>(posicion)];
        ++deuda.cantidadAlquileres;
        deuda.total += a.importeTotal;
        deuda.pagado += a.importePagado;
        deuda.saldo += saldo;
        if (!deuda.alquileres.empty()) {
            deuda.alquileres += ", ";
        }
        deuda.alquileres += enteroAString(a.numAlquiler);
        ++totalAlquileresConDeuda;
    }

    std::cout << std::left << std::setw(10) << "DNI" << " | "
              << std::setw(24) << "APELLIDO Y NOMBRE" << " | "
              << std::setw(18) << "ALQUILERES" << " | "
              << std::setw(12) << "TOTAL" << " | "
              << std::setw(12) << "PAGADO" << " | "
              << "SALDO\n";
    std::cout << "-------------------------------------------------------------------------------------\n";

    double deudaTotal = 0.0;
    for (std::size_t i = 0; i < deudas.size(); ++i) {
        const DeudaAcumulada& d = deudas[i];
        std::cout << std::left << std::setw(10) << d.dni << " | "
                  << std::setw(24) << limitar(nombreCliente(clientes, d.dni), 24) << " | "
                  << std::setw(18) << limitar(d.alquileres, 18) << " | $"
                  << std::setw(11) << std::fixed << std::setprecision(2) << d.total << " | $"
                  << std::setw(11) << d.pagado << " | $"
                  << std::setw(11) << d.saldo << "\n";
        deudaTotal += d.saldo;
    }
    std::cout << "-------------------------------------------------------------------------------------\n";
    std::cout << "Total de clientes con deuda: " << deudas.size() << "\n";
    std::cout << "Total de alquileres con saldo pendiente: " << totalAlquileresConDeuda << "\n";
    std::cout << "DEUDA TOTAL GENERAL ACUMULADA: $" << std::fixed << std::setprecision(2)
              << deudaTotal << "\n";
    pausarReporte();
}

void reporteEstadisticas() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                     REPORTE 10: ESTADISTICAS DE LA EMPRESA\n";
    std::cout << "=====================================================================================\n";

    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    if (!cargarVehiculos(vehiculos) || !cargarAlquileres(alquileres)) {
        pausarReporte();
        return;
    }

    int totalVehiculos = static_cast<int>(vehiculos.size());
    int disponibles = 0;
    int alquilados = 0;
    int mantenimiento = 0;
    int baja = 0;
    for (std::size_t i = 0; i < vehiculos.size(); ++i) {
        std::string estado = cadenaAString(vehiculos[i].estado, LON_ESTADO);
        if (estado == EST_VEH_DISPONIBLE) {
            ++disponibles;
        } else if (estado == EST_VEH_ALQUILADO) {
            ++alquilados;
        } else if (estado == EST_VEH_MANT) {
            ++mantenimiento;
        } else if (estado == EST_VEH_BAJA) {
            ++baja;
        }
    }

    int totalAlquileres = static_cast<int>(alquileres.size());
    int activos = 0;
    int finalizados = 0;
    int cancelados = 0;
    double cobrado = 0.0;
    double pendiente = 0.0;
    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        const Alquiler& a = alquileres[i];
        std::string estado = cadenaAString(a.estado, LON_ESTADO);
        if (estado == EST_ALQ_ACTIVO) {
            ++activos;
        } else if (estado == EST_ALQ_FINALIZADO) {
            ++finalizados;
        } else if (estado == EST_ALQ_CANCELADO) {
            ++cancelados;
        }
        cobrado += a.importePagado;
        if (estado != EST_ALQ_CANCELADO) {
            double saldo = a.importeTotal - a.importePagado;
            if (saldo > TOLERANCIA_SALDO) {
                pendiente += saldo;
            }
        }
    }

    std::cout << "\n--- PARQUE AUTOMOTOR ---\n";
    std::cout << "Total de vehiculos:             " << totalVehiculos << "\n";
    std::cout << "  Vehiculos disponibles:        " << disponibles << "\n";
    std::cout << "  Vehiculos alquilados:         " << alquilados << "\n";
    std::cout << "  Vehiculos en mantenimiento:   " << mantenimiento << "\n";
    std::cout << "  Vehiculos dados de baja:      " << baja << "\n";

    std::cout << "\n--- OPERACIONES DE ALQUILER ---\n";
    std::cout << "Total de alquileres historicos: " << totalAlquileres << "\n";
    std::cout << "  Alquileres activos:           " << activos << "\n";
    std::cout << "  Alquileres finalizados:       " << finalizados << "\n";
    std::cout << "  Alquileres cancelados:        " << cancelados << "\n";

    std::cout << "\n--- SITUACION ECONOMICA Y FINANCIERA ---\n";
    std::cout << "Importe Total Cobrado:          $" << std::fixed << std::setprecision(2)
              << cobrado << "\n";
    std::cout << "Importe Pendiente de Cobro:     $" << pendiente << "\n";
    std::cout << "=====================================================================================\n";
    pausarReporte();
}

void submenuReportes() {
    int opcion = -1;
    do {
        limpiarPantalla();
        std::cout << "\n========================================\n";
        std::cout << "     SUBMENU CONSULTAS Y REPORTES\n";
        std::cout << "========================================\n";
        std::cout << " 1. Vehiculos disponibles\n";
        std::cout << " 2. Vehiculos alquilados\n";
        std::cout << " 3. Alquileres por rango de fechas\n";
        std::cout << " 4. Alquileres por cliente\n";
        std::cout << " 5. Alquileres por vehiculo\n";
        std::cout << " 6. Alquileres por estado\n";
        std::cout << " 7. Alquileres por importe\n";
        std::cout << " 8. Pagos realizados por periodo\n";
        std::cout << " 9. Clientes con deuda\n";
        std::cout << "10. Estadisticas de la empresa\n";
        std::cout << " 0. Volver al Menu Principal\n";
        std::cout << "========================================\n";
        if (!leerEntero("Seleccione una opcion (0-10): ", 0, 10, opcion)) {
            break;
        }
        switch (opcion) {
            case 1: reporteVehiculosDisponibles(); break;
            case 2: reporteVehiculosAlquilados(); break;
            case 3: reporteAlquileresPorRango(); break;
            case 4: reporteAlquileresPorCliente(); break;
            case 5: reporteAlquileresPorVehiculo(); break;
            case 6: reporteAlquileresPorEstado(); break;
            case 7: reporteAlquileresPorImporte(); break;
            case 8: reportePagosPorPeriodo(); break;
            case 9: reporteClientesConDeuda(); break;
            case 10: reporteEstadisticas(); break;
            case 0: break;
            default:
                std::cout << "\n[ERROR] Opcion no valida.\n";
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

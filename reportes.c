#include "reportes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alquileres.h"
#include "clientes.h"
#include "fechas.h"
#include "movimientos.h"
#include "utils.h"
#include "vehiculos.h"

typedef struct {
    int dni;
    int cantidadAlquileres;
    double total;
    double pagado;
    double saldo;
    char alquileres[256];
} DeudaAcumulada;

static void obtenerNombreCliente(const Cliente* clientes, int cantClientes, int dni, char* salida, size_t tamSalida) {
    int indice = buscarIndiceCliente(clientes, cantClientes, dni);
    if (indice < 0) {
        copiarCadena(salida, tamSalida, "(Desconocido)");
    } else {
        copiarCadena(salida, tamSalida, clientes[indice].apellidoNombre);
    }
}

static void obtenerDescripcionVehiculo(const Vehiculo* vehiculos, int cantVehiculos,
                                      const char* patente, char* salida, size_t tamSalida) {
    int indice = buscarIndiceVehiculo(vehiculos, cantVehiculos, patente);
    if (indice < 0) {
        copiarCadena(salida, tamSalida, "(Desconocido)");
    } else {
        snprintf(salida, tamSalida, "%s %s", vehiculos[indice].marca, vehiculos[indice].modelo);
    }
}

static void pausarReporte(void) {
    pausarPantalla();
}

void reporteVehiculosDisponibles(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                       REPORTE 1: VEHICULOS DISPONIBLES\n");
    printf("=====================================================================================\n");

    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    if (!cargarVehiculos(&vehiculos, &cantVehiculos)) {
        pausarReporte();
        return;
    }

    printf("%-9s | %-12s | %-14s | %-6s | %-12s | %-5s | PRECIO/DIA\n",
           "PATENTE", "MARCA", "MODELO", "ANIO", "TIPO", "CAP.");
    printf("-------------------------------------------------------------------------------------\n");

    int cantidad = 0;
    for (int i = 0; i < cantVehiculos; ++i) {
        const Vehiculo* v = &vehiculos[i];
        if (strcmp(v->estado, EST_VEH_DISPONIBLE) != 0) {
            continue;
        }
        printf("%-9s | %-12.12s | %-14.14s | %-6d | %-12.12s | %-5d | $%.2f\n",
               v->patente, v->marca, v->modelo, v->anio, v->tipo, v->capacidad, v->precioDia);
        cantidad++;
    }
    printf("-------------------------------------------------------------------------------------\n");
    if (cantidad == 0) {
        printf("No hay vehiculos disponibles en este momento.\n");
    } else {
        printf("Total de vehiculos disponibles: %d\n", cantidad);
    }

    liberarVehiculos(vehiculos);
    pausarReporte();
}

void reporteVehiculosAlquilados(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                        REPORTE 2: VEHICULOS ALQUILADOS\n");
    printf("=====================================================================================\n");

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) ||
        !cargarVehiculos(&vehiculos, &cantVehiculos) ||
        !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    printf("%-9s | %-20s | %-30s | %-10s | PREVISTA\n",
           "PATENTE", "MARCA Y MODELO", "CLIENTE", "INICIO");
    printf("-------------------------------------------------------------------------------------\n");

    int cantidad = 0;
    for (int i = 0; i < cantAlquileres; ++i) {
        const Alquiler* a = &alquileres[i];
        if (strcmp(a->estado, EST_ALQ_ACTIVO) != 0) {
            continue;
        }
        char descVeh[LON_MARCA + LON_MODELO + 2];
        obtenerDescripcionVehiculo(vehiculos, cantVehiculos, a->patente, descVeh, sizeof(descVeh));

        char nomCli[LON_NOMBRE];
        obtenerNombreCliente(clientes, cantClientes, a->dni, nomCli, sizeof(nomCli));
        char cliStr[128];
        snprintf(cliStr, sizeof(cliStr), "%d - %s", a->dni, nomCli);

        printf("%-9s | %-20.20s | %-30.30s | %-10s | %s\n",
               a->patente, descVeh, cliStr,
               fechaALarga(a->fechaInicio), fechaALarga(a->fechaPrevista));
        cantidad++;
    }
    printf("-------------------------------------------------------------------------------------\n");
    if (cantidad == 0) {
        printf("No hay vehiculos alquilados en este momento.\n");
    } else {
        printf("Total de vehiculos actualmente alquilados: %d\n", cantidad);
    }

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void reporteAlquileresPorRango(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                 REPORTE 3: ALQUILERES POR RANGO DE FECHAS\n");
    printf("=====================================================================================\n");

    int fechaInicial = 0;
    int fechaFinal = 0;
    int fechaMaximaReporte = sumarDias(fechaActual(), DIAS_MAX_INICIO_FUTURO);

    if (leerFechaEnRango("Ingrese la fecha inicial del rango (AAAAMMDD) (V = volver): ",
                         19000101, fechaMaximaReporte, &fechaInicial) != LEIDO) {
        return;
    }
    if (leerFechaEnRango("Ingrese la fecha final del rango (AAAAMMDD) (V = volver): ",
                         fechaInicial, fechaMaximaReporte, &fechaFinal) != LEIDO) {
        return;
    }

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) ||
        !cargarVehiculos(&vehiculos, &cantVehiculos) ||
        !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    printf("\nAlquileres cuya fecha de inicio esta entre %s y %s:\n\n",
           fechaALarga(fechaInicial), fechaALarga(fechaFinal));
    imprimirEncabezadoAlquileres();

    int cantidad = 0;
    for (int i = 0; i < cantAlquileres; ++i) {
        if (alquileres[i].fechaInicio >= fechaInicial && alquileres[i].fechaInicio <= fechaFinal) {
            imprimirFilaAlquiler(&alquileres[i], clientes, cantClientes, vehiculos, cantVehiculos);
            cantidad++;
        }
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres encontrados en el rango: %d\n", cantidad);

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void reporteAlquileresPorCliente(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                    REPORTE 4: ALQUILERES POR CLIENTE\n");
    printf("=====================================================================================\n");

    int dni = 0;
    if (leerDNI("Ingrese el DNI del cliente (entre 1.000.000 y 50.000.000) (V = volver): ", &dni) != LEIDO) {
        return;
    }

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) ||
        !cargarVehiculos(&vehiculos, &cantVehiculos) ||
        !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    int indice = buscarIndiceCliente(clientes, cantClientes, dni);
    if (indice < 0) {
        printf("\n[ERROR] El cliente con DNI %d no existe en el sistema.\n", dni);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }
    const Cliente* cliente = &clientes[indice];
    printf("\nCliente: %s (DNI: %d) | Telefono: %s | Estado: %s\n\n",
           cliente->apellidoNombre, cliente->dni, cliente->telefono,
           cliente->estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO");

    imprimirEncabezadoAlquileres();
    int cantidad = 0;
    for (int i = 0; i < cantAlquileres; ++i) {
        if (alquileres[i].dni == dni) {
            imprimirFilaAlquiler(&alquileres[i], clientes, cantClientes, vehiculos, cantVehiculos);
            cantidad++;
        }
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres registrados para este cliente: %d\n", cantidad);

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void reporteAlquileresPorVehiculo(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                   REPORTE 5: ALQUILERES POR VEHICULO\n");
    printf("=====================================================================================\n");

    char patente[LON_PATENTE];
    if (leerPatente("Ingrese la patente del vehiculo (V = volver): ", patente, sizeof(patente)) != LEIDO) {
        return;
    }

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) ||
        !cargarVehiculos(&vehiculos, &cantVehiculos) ||
        !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    int indice = buscarIndiceVehiculo(vehiculos, cantVehiculos, patente);
    if (indice < 0) {
        printf("\n[ERROR] El vehiculo con patente %s no existe en el sistema.\n", patente);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }
    const Vehiculo* vehiculo = &vehiculos[indice];
    printf("\nVehiculo: %s %s (Patente: %s) | Tipo: %s | Tarifa/dia: $%.2f | Estado: %s\n\n",
           vehiculo->marca, vehiculo->modelo, patente,
           vehiculo->tipo, vehiculo->precioDia, vehiculo->estado);

    imprimirEncabezadoAlquileres();
    int cantidad = 0;
    char normPatente[LON_PATENTE];
    normalizarPatente(patente, normPatente, sizeof(normPatente));

    for (int i = 0; i < cantAlquileres; ++i) {
        char normActual[LON_PATENTE];
        normalizarPatente(alquileres[i].patente, normActual, sizeof(normActual));
        if (strcmp(normActual, normPatente) == 0) {
            imprimirFilaAlquiler(&alquileres[i], clientes, cantClientes, vehiculos, cantVehiculos);
            cantidad++;
        }
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres historicos para este vehiculo: %d\n", cantidad);

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void reporteAlquileresPorEstado(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("     REPORTE 6: ALQUILERES POR ESTADO\n");
    printf("========================================\n");
    printf("1. Activos\n");
    printf("2. Finalizados\n");
    printf("3. Cancelados\n");
    printf("0. Cancelar\n");

    int opcion = 0;
    if (leerEntero("Seleccione un estado (0-3) (V = volver): ", 0, 3, &opcion) != LEIDO || opcion == 0) {
        return;
    }
    const char* estado = EST_ALQ_ACTIVO;
    if (opcion == 2) {
        estado = EST_ALQ_FINALIZADO;
    } else if (opcion == 3) {
        estado = EST_ALQ_CANCELADO;
    }

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) ||
        !cargarVehiculos(&vehiculos, &cantVehiculos) ||
        !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                 LISTADO DE ALQUILERES EN ESTADO: %s\n", estado);
    printf("=====================================================================================\n");
    imprimirEncabezadoAlquileres();

    int cantidad = 0;
    for (int i = 0; i < cantAlquileres; ++i) {
        if (strcmp(alquileres[i].estado, estado) == 0) {
            imprimirFilaAlquiler(&alquileres[i], clientes, cantClientes, vehiculos, cantVehiculos);
            cantidad++;
        }
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres en estado %s: %d\n", estado, cantidad);

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void reporteAlquileresPorImporte(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("    REPORTE 7: ALQUILERES POR IMPORTE\n");
    printf("========================================\n");

    double montoReferencia = 0.0;
    if (leerDouble("Ingrese el importe de referencia (V = volver): ", 0.0, &montoReferencia) != LEIDO) {
        return;
    }
    printf("\nSeleccione el criterio de comparacion:\n");
    printf("1. Superiores al importe ingresado (> $%.2f)\n", montoReferencia);
    printf("2. Inferiores al importe ingresado (< $%.2f)\n", montoReferencia);
    int criterio = 1;
    if (leerEntero("Opcion (1-2) (V = volver): ", 1, 2, &criterio) != LEIDO) {
        return;
    }

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) ||
        !cargarVehiculos(&vehiculos, &cantVehiculos) ||
        !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    limpiarPantalla();
    printf("\n=====================================================================================\n");
    if (criterio == 1) {
        printf("              ALQUILERES CON IMPORTE TOTAL SUPERIOR A $%.2f\n", montoReferencia);
    } else {
        printf("              ALQUILERES CON IMPORTE TOTAL INFERIOR A $%.2f\n", montoReferencia);
    }
    printf("=====================================================================================\n");
    imprimirEncabezadoAlquileres();

    int cantidad = 0;
    for (int i = 0; i < cantAlquileres; ++i) {
        bool coincide = (criterio == 1 && alquileres[i].importeTotal > montoReferencia) ||
                        (criterio == 2 && alquileres[i].importeTotal < montoReferencia);
        if (coincide) {
            imprimirFilaAlquiler(&alquileres[i], clientes, cantClientes, vehiculos, cantVehiculos);
            cantidad++;
        }
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres que cumplen la condicion: %d\n", cantidad);

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void reportePagosPorPeriodo(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                  REPORTE 8: PAGOS REALIZADOS POR PERIODO\n");
    printf("=====================================================================================\n");

    int fechaInicial = 0;
    int fechaFinal = 0;
    int fechaHoy = fechaActual();

    if (leerFechaEnRango("Ingrese la fecha inicial del periodo (AAAAMMDD) (V = volver): ",
                         19000101, fechaHoy, &fechaInicial) != LEIDO) {
        return;
    }
    if (leerFechaEnRango("Ingrese la fecha final del periodo (AAAAMMDD) (V = volver): ",
                         fechaInicial, fechaHoy, &fechaFinal) != LEIDO) {
        return;
    }

    Movimiento* movimientos = NULL;
    int cantMovimientos = 0;
    if (!cargarMovimientos(&movimientos, &cantMovimientos)) {
        pausarReporte();
        return;
    }

    printf("\nPagos registrados entre %s y %s:\n\n",
           fechaALarga(fechaInicial), fechaALarga(fechaFinal));
    printf("%-6s | %-6s | %-10s | %-10s | %-12s | %-10s | MEDIO\n",
           "MOV", "ALQ", "DNI", "FECHA", "MONTO", "PATENTE");
    printf("-------------------------------------------------------------------------------------\n");

    int cantidad = 0;
    double totalPeriodo = 0.0;
    for (int i = 0; i < cantMovimientos; ++i) {
        const Movimiento* m = &movimientos[i];
        if (strcmp(m->tipoMov, MOV_PAGO) == 0 && m->fecha >= fechaInicial && m->fecha <= fechaFinal) {
            printf("%-6d | %-6d | %-10d | %-10s | $%11.2f | %-10s | %s\n",
                   m->numMovimiento, m->numAlquiler, m->dni,
                   fechaALarga(m->fecha), m->monto, m->patente, m->medioPago);
            totalPeriodo += m->monto;
            cantidad++;
        }
    }
    printf("-------------------------------------------------------------------------------------\n");
    printf("Cantidad de pagos en el periodo: %d | Monto total acumulado: $%.2f\n", cantidad, totalPeriodo);

    liberarMovimientos(movimientos);
    pausarReporte();
}

void reporteClientesConDeuda(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                      REPORTE 9: CLIENTES CON DEUDA\n");
    printf("=====================================================================================\n");

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) || !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    DeudaAcumulada* deudas = (DeudaAcumulada*)malloc((size_t)(cantAlquileres + 1) * sizeof(DeudaAcumulada));
    if (!deudas) {
        liberarClientes(clientes);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }
    int totalDeudas = 0;
    int totalAlquileresConDeuda = 0;

    for (int i = 0; i < cantAlquileres; ++i) {
        const Alquiler* a = &alquileres[i];
        if (strcmp(a->estado, EST_ALQ_CANCELADO) == 0) {
            continue;
        }
        double saldo = a->importeTotal - a->importePagado;
        if (saldo <= TOLERANCIA_SALDO) {
            continue;
        }

        int posicion = -1;
        for (int j = 0; j < totalDeudas; ++j) {
            if (deudas[j].dni == a->dni) {
                posicion = j;
                break;
            }
        }
        if (posicion < 0) {
            posicion = totalDeudas++;
            deudas[posicion].dni = a->dni;
            deudas[posicion].cantidadAlquileres = 0;
            deudas[posicion].total = 0.0;
            deudas[posicion].pagado = 0.0;
            deudas[posicion].saldo = 0.0;
            deudas[posicion].alquileres[0] = '\0';
        }

        DeudaAcumulada* d = &deudas[posicion];
        d->cantidadAlquileres++;
        d->total += a->importeTotal;
        d->pagado += a->importePagado;
        d->saldo += saldo;

        char numBuf[32];
        snprintf(numBuf, sizeof(numBuf), "%s%d", (d->alquileres[0] != '\0') ? ", " : "", a->numAlquiler);
        size_t actLen = strlen(d->alquileres);
        if (actLen + strlen(numBuf) + 1 < sizeof(d->alquileres)) {
            strcat(d->alquileres, numBuf);
        }
        totalAlquileresConDeuda++;
    }

    printf("%-10s | %-24s | %-18s | %-12s | %-12s | SALDO\n",
           "DNI", "APELLIDO Y NOMBRE", "ALQUILERES", "TOTAL", "PAGADO");
    printf("-------------------------------------------------------------------------------------\n");

    double deudaTotal = 0.0;
    for (int i = 0; i < totalDeudas; ++i) {
        const DeudaAcumulada* d = &deudas[i];
        char nomCli[LON_NOMBRE];
        obtenerNombreCliente(clientes, cantClientes, d->dni, nomCli, sizeof(nomCli));

        printf("%-10d | %-24.24s | %-18.18s | $%11.2f | $%11.2f | $%11.2f\n",
               d->dni, nomCli, d->alquileres, d->total, d->pagado, d->saldo);
        deudaTotal += d->saldo;
    }
    printf("-------------------------------------------------------------------------------------\n");
    printf("Total de clientes con deuda: %d\n", totalDeudas);
    printf("Total de alquileres con saldo pendiente: %d\n", totalAlquileresConDeuda);
    printf("DEUDA TOTAL GENERAL ACUMULADA: $%.2f\n", deudaTotal);

    free(deudas);
    liberarClientes(clientes);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void reporteEstadisticas(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                     REPORTE 10: ESTADISTICAS DE LA EMPRESA\n");
    printf("=====================================================================================\n");

    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarVehiculos(&vehiculos, &cantVehiculos) || !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarReporte();
        return;
    }

    int disponibles = 0;
    int alquilados = 0;
    int mantenimiento = 0;
    int baja = 0;
    for (int i = 0; i < cantVehiculos; ++i) {
        if (strcmp(vehiculos[i].estado, EST_VEH_DISPONIBLE) == 0) {
            disponibles++;
        } else if (strcmp(vehiculos[i].estado, EST_VEH_ALQUILADO) == 0) {
            alquilados++;
        } else if (strcmp(vehiculos[i].estado, EST_VEH_MANT) == 0) {
            mantenimiento++;
        } else if (strcmp(vehiculos[i].estado, EST_VEH_BAJA) == 0) {
            baja++;
        }
    }

    int activos = 0;
    int finalizados = 0;
    int cancelados = 0;
    double cobrado = 0.0;
    double pendiente = 0.0;

    for (int i = 0; i < cantAlquileres; ++i) {
        const Alquiler* a = &alquileres[i];
        if (strcmp(a->estado, EST_ALQ_ACTIVO) == 0) {
            activos++;
        } else if (strcmp(a->estado, EST_ALQ_FINALIZADO) == 0) {
            finalizados++;
        } else if (strcmp(a->estado, EST_ALQ_CANCELADO) == 0) {
            cancelados++;
        }
        cobrado += a->importePagado;
        if (strcmp(a->estado, EST_ALQ_CANCELADO) != 0) {
            double saldo = a->importeTotal - a->importePagado;
            if (saldo > TOLERANCIA_SALDO) {
                pendiente += saldo;
            }
        }
    }

    printf("\n--- PARQUE AUTOMOTOR ---\n");
    printf("Total de vehiculos:             %d\n", cantVehiculos);
    printf("  Vehiculos disponibles:        %d\n", disponibles);
    printf("  Vehiculos alquilados:         %d\n", alquilados);
    printf("  Vehiculos en mantenimiento:   %d\n", mantenimiento);
    printf("  Vehiculos dados de baja:      %d\n", baja);

    printf("\n--- OPERACIONES DE ALQUILER ---\n");
    printf("Total de alquileres historicos: %d\n", cantAlquileres);
    printf("  Alquileres activos:           %d\n", activos);
    printf("  Alquileres finalizados:       %d\n", finalizados);
    printf("  Alquileres cancelados:        %d\n", cancelados);

    printf("\n--- SITUACION ECONOMICA Y FINANCIERA ---\n");
    printf("Importe Total Cobrado:          $%.2f\n", cobrado);
    printf("Importe Pendiente de Cobro:     $%.2f\n", pendiente);
    printf("=====================================================================================\n");

    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarReporte();
}

void submenuReportes(void) {
    int opcion = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("     SUBMENU CONSULTAS Y REPORTES\n");
        printf("========================================\n");
        printf(" 1. Vehiculos disponibles\n");
        printf(" 2. Vehiculos alquilados\n");
        printf(" 3. Alquileres por rango de fechas\n");
        printf(" 4. Alquileres por cliente\n");
        printf(" 5. Alquileres por vehiculo\n");
        printf(" 6. Alquileres por estado\n");
        printf(" 7. Alquileres por importe\n");
        printf(" 8. Pagos realizados por periodo\n");
        printf(" 9. Clientes con deuda\n");
        printf("10. Estadisticas de la empresa\n");
        printf(" 0. Volver al Menu Principal\n");
        printf("========================================\n");

        if (leerEntero("Seleccione una opcion (0-10): ", 0, 10, &opcion) != LEIDO) {
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
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

#include "movimientos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "alquileres.h"
#include "archivos.h"
#include "clientes.h"
#include "fechas.h"
#include "utils.h"

static bool escribirArchivoCliente(int dni, const Movimiento* movimientos, int cantMovimientos,
                                  char* error, size_t tamError) {
    const char* ruta = nombreArchivoMovimientos(dni);
    FILE* archivo = fopen(ruta, "w");
    if (!archivo) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "No se pudo crear o sobrescribir el archivo %s.", ruta);
        }
        return false;
    }
    for (int i = 0; i < cantMovimientos; ++i) {
        const Movimiento* m = &movimientos[i];
        if (m->dni != dni) {
            continue;
        }
        fprintf(archivo, "%d;%d;%s;%.2f;%d;%d;%s;%s\n",
                m->numMovimiento, m->numAlquiler, m->tipoMov,
                m->monto, m->fecha, m->dni, m->patente, m->medioPago);
    }
    fclose(archivo);
    return true;
}

bool cargarMovimientos(Movimiento** movimientos, int* cantidad) {
    char error[256];
    if (!cargarRegistrosGenerico(ARCH_MOVIMIENTOS, sizeof(Movimiento), (void**)movimientos, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    if (*movimientos && *cantidad > 0) {
        for (int i = 0; i < *cantidad; ++i) {
            (*movimientos)[i].tipoMov[LON_TIPO_MOV - 1] = '\0';
            (*movimientos)[i].patente[LON_PATENTE - 1] = '\0';
            (*movimientos)[i].medioPago[LON_MEDIO_PAGO - 1] = '\0';
        }
    }
    return true;
}

bool guardarMovimientos(const Movimiento* movimientos, int cantidad) {
    char error[256];
    if (!guardarRegistrosGenerico(ARCH_MOVIMIENTOS, sizeof(Movimiento), movimientos, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    return true;
}

void liberarMovimientos(Movimiento* movimientos) {
    if (movimientos) {
        free(movimientos);
    }
}

int proximoNumeroMovimiento(void) {
    Movimiento* movimientos = NULL;
    int cantidad = 0;
    if (!cargarMovimientos(&movimientos, &cantidad)) {
        return 1;
    }
    int maximo = 0;
    for (int i = 0; i < cantidad; ++i) {
        if (movimientos[i].numMovimiento > maximo) {
            maximo = movimientos[i].numMovimiento;
        }
    }
    liberarMovimientos(movimientos);
    return maximo + 1;
}

const char* nombreArchivoMovimientos(int dni) {
    static char buffer[64];
    snprintf(buffer, sizeof(buffer), "movimientos_%d.txt", dni);
    return buffer;
}

bool regenerarArchivoMovimientosCliente(int dni, char* error, size_t tamError) {
    Movimiento* movimientos = NULL;
    int cantidad = 0;
    if (!cargarMovimientos(&movimientos, &cantidad)) {
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo cargar %s.", ARCH_MOVIMIENTOS);
        return false;
    }
    bool ok = escribirArchivoCliente(dni, movimientos, cantidad, error, tamError);
    liberarMovimientos(movimientos);
    return ok;
}

bool regenerarTodosLosArchivosTexto(char* error, size_t tamError) {
    Cliente* clientes = NULL;
    int cantClientes = 0;
    Movimiento* movimientos = NULL;
    int cantMovimientos = 0;

    if (!cargarClientes(&clientes, &cantClientes)) {
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo cargar %s.", ARCH_CLIENTES);
        return false;
    }
    if (!cargarMovimientos(&movimientos, &cantMovimientos)) {
        liberarClientes(clientes);
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo cargar %s.", ARCH_MOVIMIENTOS);
        return false;
    }

    int capDnis = cantClientes + cantMovimientos + 1;
    int* dnis = (int*)malloc((size_t)capDnis * sizeof(int));
    if (!dnis) {
        liberarClientes(clientes);
        liberarMovimientos(movimientos);
        if (error && tamError > 0) snprintf(error, tamError, "Memoria insuficiente.");
        return false;
    }
    int totalDnis = 0;

    for (int i = 0; i < cantClientes; ++i) {
        bool repetido = false;
        for (int j = 0; j < totalDnis; ++j) {
            if (dnis[j] == clientes[i].dni) {
                repetido = true;
                break;
            }
        }
        if (!repetido) {
            dnis[totalDnis++] = clientes[i].dni;
        }
    }
    for (int i = 0; i < cantMovimientos; ++i) {
        bool repetido = false;
        for (int j = 0; j < totalDnis; ++j) {
            if (dnis[j] == movimientos[i].dni) {
                repetido = true;
                break;
            }
        }
        if (!repetido) {
            dnis[totalDnis++] = movimientos[i].dni;
        }
    }

    for (int i = 0; i < totalDnis; ++i) {
        char errorCliente[256];
        if (!escribirArchivoCliente(dnis[i], movimientos, cantMovimientos, errorCliente, sizeof(errorCliente))) {
            if (error && tamError > 0) snprintf(error, tamError, "%s", errorCliente);
            free(dnis);
            liberarClientes(clientes);
            liberarMovimientos(movimientos);
            return false;
        }
    }

    free(dnis);
    liberarClientes(clientes);
    liberarMovimientos(movimientos);
    return true;
}

bool registrarMovimiento(int numAlquiler, const char* tipo, double monto,
                         int fecha, const char* medio, char* error, size_t tamError) {
    if (!validarTipoMovimiento(tipo)) {
        if (error && tamError > 0) snprintf(error, tamError, "El tipo de movimiento debe ser 'Pago' o 'Ajuste'.");
        return false;
    }
    if (!esFinito(monto)) {
        if (error && tamError > 0) snprintf(error, tamError, "El monto del movimiento no es un numero valido.");
        return false;
    }
    if (strcmp(tipo, MOV_PAGO) == 0) {
        if (monto <= 0.0) {
            if (error && tamError > 0) snprintf(error, tamError, "El monto del pago debe ser mayor a cero.");
            return false;
        }
        if (strcmp(medio, MEDIO_EFECTIVO) != 0 && strcmp(medio, MEDIO_TARJETA) != 0 &&
            strcmp(medio, MEDIO_TRANSFERENCIA) != 0) {
            if (error && tamError > 0) snprintf(error, tamError, "El medio de pago debe ser Efectivo, Tarjeta o Transferencia.");
            return false;
        }
    } else {
        if (fabs(monto) < TOLERANCIA_SALDO) {
            if (error && tamError > 0) snprintf(error, tamError, "El monto del ajuste no puede ser cero.");
            return false;
        }
    }
    if (!esFechaValida(fecha)) {
        if (error && tamError > 0) snprintf(error, tamError, "La fecha del movimiento no es valida.");
        return false;
    }
    if (!esFechaPasadaOHoy(fecha)) {
        if (error && tamError > 0) snprintf(error, tamError, "La fecha del movimiento no puede ser posterior a la fecha actual.");
        return false;
    }

    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;
    if (!cargarAlquileres(&alquileres, &cantAlquileres)) {
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo cargar %s.", ARCH_ALQUILERES);
        return false;
    }
    int indiceAlquiler = buscarIndiceAlquiler(alquileres, cantAlquileres, numAlquiler);
    if (indiceAlquiler < 0) {
        liberarAlquileres(alquileres);
        if (error && tamError > 0) snprintf(error, tamError, "No se encontro el alquiler Nro %d.", numAlquiler);
        return false;
    }
    const Alquiler* alquiler = &alquileres[indiceAlquiler];

    if (strcmp(tipo, MOV_PAGO) == 0 && fecha < alquiler->fechaInicio) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "La fecha del pago no puede ser anterior al inicio del alquiler (%s).",
                     fechaALarga(alquiler->fechaInicio));
        }
        liberarAlquileres(alquileres);
        return false;
    }

    Movimiento* movimientos = NULL;
    int cantMovimientos = 0;
    if (!cargarMovimientos(&movimientos, &cantMovimientos)) {
        liberarAlquileres(alquileres);
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo cargar %s.", ARCH_MOVIMIENTOS);
        return false;
    }

    Movimiento movimiento;
    memset(&movimiento, 0, sizeof(movimiento));

    int maximo = 0;
    for (int i = 0; i < cantMovimientos; ++i) {
        if (movimientos[i].numMovimiento > maximo) {
            maximo = movimientos[i].numMovimiento;
        }
    }
    movimiento.numMovimiento = maximo + 1;
    movimiento.numAlquiler = numAlquiler;
    copiarCadena(movimiento.tipoMov, sizeof(movimiento.tipoMov), tipo);
    movimiento.monto = monto;
    movimiento.fecha = fecha;
    movimiento.dni = alquiler->dni;
    copiarCadena(movimiento.patente, sizeof(movimiento.patente), alquiler->patente);
    copiarCadena(movimiento.medioPago, sizeof(movimiento.medioPago),
                 (strcmp(tipo, MOV_PAGO) == 0) ? medio : MEDIO_NINGUNO);

    liberarAlquileres(alquileres);

    Movimiento* tempMov = (Movimiento*)realloc(movimientos, (size_t)(cantMovimientos + 1) * sizeof(Movimiento));
    if (!tempMov) {
        liberarMovimientos(movimientos);
        if (error && tamError > 0) snprintf(error, tamError, "Memoria insuficiente para registrar movimiento.");
        return false;
    }
    movimientos = tempMov;
    movimientos[cantMovimientos++] = movimiento;

    if (!guardarMovimientos(movimientos, cantMovimientos)) {
        liberarMovimientos(movimientos);
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo guardar el movimiento en %s.", ARCH_MOVIMIENTOS);
        return false;
    }
    liberarMovimientos(movimientos);

    char errorTexto[256];
    if (!regenerarArchivoMovimientosCliente(movimiento.dni, errorTexto, sizeof(errorTexto))) {
        printf("[AVISO] El movimiento se guardo, pero no se pudo actualizar el archivo de texto del cliente: %s\n", errorTexto);
    }
    return true;
}

void registrarPago(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("            REGISTRAR PAGO\n");
    printf("========================================\n");

    Cliente* clientes = NULL;
    int cantClientes = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;

    if (!cargarClientes(&clientes, &cantClientes) || !cargarAlquileres(&alquileres, &cantAlquileres)) {
        liberarClientes(clientes);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    int numero = 0;
    int indiceAlquiler = -1;
    double saldoPendiente = 0.0;
    double monto = 0.0;
    int opcionMedio = 1;
    const char* medio = MEDIO_EFECTIVO;
    int fechaPago = 0;
    char nombreCliente[LON_NOMBRE];
    nombreCliente[0] = '\0';

    int paso = 0;
    EstadoLectura est;

    while (1) {
        switch (paso) {
            case 0: {
                est = leerEntero("Ingrese el numero de alquiler a pagar (V = volver): ", 1, 9999999, &numero);
                if (est == VOLVER) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    return;
                }
                indiceAlquiler = buscarIndiceAlquiler(alquileres, cantAlquileres, numero);
                if (indiceAlquiler < 0) {
                    printf("\n[ERROR] El alquiler Nro %d no existe en el sistema. Reintente.\n", numero);
                    continue;
                }
                Alquiler* alq = &alquileres[indiceAlquiler];
                if (strcmp(alq->estado, EST_ALQ_CANCELADO) == 0) {
                    printf("\n[ERROR] No se pueden registrar pagos para alquileres CANCELADOS. Reintente.\n");
                    continue;
                }
                saldoPendiente = alq->importeTotal - alq->importePagado;
                if (saldoPendiente <= TOLERANCIA_SALDO) {
                    printf("\n[AVISO] El alquiler Nro %d ya se encuentra totalmente saldado ($%.2f pagados). Reintente.\n",
                           numero, alq->importePagado);
                    continue;
                }

                /*
                 * Regla de negocio / Pagos anticipados:
                 * Los movimientos de tipo "Pago" quedan registrados con una fecha de operacion
                 * valida asociada a la ejecucion del contrato. Debido a que registrarMovimiento()
                 * exige que la fecha del pago no sea anterior a la fecha de inicio del alquiler ni
                 * posterior a hoy, si el alquiler tiene fecha de inicio futura (fechaInicio > hoy),
                 * no es posible registrar pagos hasta llegada dicha fecha. Para registrar pagos o
                 * senas inmediatas al momento de contratar, el alquiler debe tener fecha de inicio
                 * fijada en el dia actual.
                 */
                if (alq->fechaInicio > fechaActual()) {
                    printf("\n[ERROR] El alquiler comienza el %s; no se pueden registrar pagos antes de esa fecha. Reintente.\n",
                           fechaALarga(alq->fechaInicio));
                    continue;
                }

                int indCli = buscarIndiceCliente(clientes, cantClientes, alq->dni);
                if (indCli >= 0) {
                    copiarCadena(nombreCliente, sizeof(nombreCliente), clientes[indCli].apellidoNombre);
                } else {
                    copiarCadena(nombreCliente, sizeof(nombreCliente), "(Desconocido)");
                }

                printf("\n--- DATOS DEL ALQUILER ---\n");
                printf("Nro Alquiler:    %d\n", alq->numAlquiler);
                printf("Cliente:         %d - %s\n", alq->dni, nombreCliente);
                printf("Patente:         %s\n", alq->patente);
                printf("Fecha de Inicio: %s\n", fechaALarga(alq->fechaInicio));
                printf("Importe Total:   $%.2f\n", alq->importeTotal);
                printf("Importe Pagado:  $%.2f\n", alq->importePagado);
                printf("SALDO PENDIENTE: $%.2f\n", saldoPendiente);
                printf("---------------------------\n");

                paso++;
                break;
            }
            case 1: {
                char promptMonto[128];
                snprintf(promptMonto, sizeof(promptMonto),
                         "Ingrese el monto a pagar (max $%.2f) (V = volver): ", saldoPendiente);
                est = leerDouble(promptMonto, 0.0, &monto);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    return;
                }
                if (monto <= 0.0) {
                    printf("[ERROR] El monto debe ser mayor a cero. Reintente.\n");
                    continue;
                }
                if (monto > saldoPendiente + TOLERANCIA_SALDO) {
                    printf("[ERROR] El monto no puede superar el saldo pendiente ($%.2f). Reintente.\n", saldoPendiente);
                    continue;
                }
                if (monto > saldoPendiente) {
                    monto = saldoPendiente;
                }
                paso++;
                break;
            }
            case 2: {
                printf("\nSeleccione medio de pago:\n");
                printf("1. %s\n", MEDIO_EFECTIVO);
                printf("2. %s\n", MEDIO_TARJETA);
                printf("3. %s\n", MEDIO_TRANSFERENCIA);
                est = leerEntero("Opcion (1-3) (V = volver): ", 1, 3, &opcionMedio);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    return;
                }
                if (opcionMedio == 2) {
                    medio = MEDIO_TARJETA;
                } else if (opcionMedio == 3) {
                    medio = MEDIO_TRANSFERENCIA;
                } else {
                    medio = MEDIO_EFECTIVO;
                }
                paso++;
                break;
            }
            case 3: {
                Alquiler* alq = &alquileres[indiceAlquiler];
                char promptFecha[128];
                snprintf(promptFecha, sizeof(promptFecha),
                         "Ingrese la fecha del pago (AAAAMMDD, entre %s y %s) (V = volver): ",
                         fechaALarga(alq->fechaInicio), fechaALarga(fechaActual()));
                est = leerFechaEnRango(promptFecha, alq->fechaInicio, fechaActual(), &fechaPago);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    return;
                }
                paso++;
                break;
            }
            case 4: {
                Alquiler* alquiler = &alquileres[indiceAlquiler];
                printf("\n---------------- RESUMEN DEL PAGO ----------------\n");
                printf("Alquiler Nro:          %d\n", alquiler->numAlquiler);
                printf("Cliente:               %d - %s\n", alquiler->dni, nombreCliente);
                printf("Monto a pagar:         $%.2f\n", monto);
                printf("Medio de Pago:         %s\n", medio);
                printf("Fecha del Pago:        %s\n", fechaALarga(fechaPago));
                printf("Saldo Restante:        $%.2f\n", saldoPendiente - monto);
                printf("--------------------------------------------------\n");

                bool confirmar = false;
                est = leerSN("¿Confirma el registro del pago? (S/N) (V = volver): ", &confirmar);
                if (est == VOLVER) {
                    paso = 3;
                    break;
                }
                if (est != LEIDO || !confirmar) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }

                double importePagadoAnterior = alquiler->importePagado;
                alquiler->importePagado += monto;
                if (!guardarAlquileres(alquileres, cantAlquileres)) {
                    printf("\n[ERROR] No se pudo actualizar el importe pagado en %s.\n", ARCH_ALQUILERES);
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }

                char error[256];
                if (!registrarMovimiento(alquiler->numAlquiler, MOV_PAGO, monto, fechaPago, medio, error, sizeof(error))) {
                    alquiler->importePagado = importePagadoAnterior;
                    guardarAlquileres(alquileres, cantAlquileres);
                    printf("\n[ERROR] No se pudo registrar el pago: %s\n", error);
                    printf("El importe pagado fue revertido.\n");
                    liberarClientes(clientes);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }

                printf("\n========================================\n");
                printf(">>> PAGO REGISTRADO EXITOSAMENTE\n");
                printf("Alquiler Nro:          %d\n", alquiler->numAlquiler);
                printf("Monto Abonado:         $%.2f\n", monto);
                printf("Medio de Pago:         %s\n", medio);
                printf("Nuevo Saldo Pendiente: $%.2f\n", alquiler->importeTotal - alquiler->importePagado);
                printf("========================================\n");

                liberarClientes(clientes);
                liberarAlquileres(alquileres);
                pausarPantalla();
                return;
            }
        }
    }
}

void listarMovimientos(void) {
    limpiarPantalla();
    printf("\n========================================================================================\n");
    printf("                        LISTADO GENERAL DE MOVIMIENTOS\n");
    printf("========================================================================================\n");

    Movimiento* movimientos = NULL;
    int cantidad = 0;
    if (!cargarMovimientos(&movimientos, &cantidad)) {
        pausarPantalla();
        return;
    }
    if (cantidad == 0) {
        printf("No hay movimientos registrados.\n");
        liberarMovimientos(movimientos);
        pausarPantalla();
        return;
    }

    printf("%-6s | %-6s | %-8s | %-12s | %-10s | %-10s | %-10s | MEDIO\n",
           "NRO", "ALQ", "TIPO", "MONTO", "FECHA", "DNI", "PATENTE");
    printf("----------------------------------------------------------------------------------------\n");

    double totalPagos = 0.0;
    for (int i = 0; i < cantidad; ++i) {
        const Movimiento* m = &movimientos[i];
        printf("%-6d | %-6d | %-8s | $%11.2f | %-10s | %-10d | %-10s | %s\n",
               m->numMovimiento, m->numAlquiler, m->tipoMov, m->monto,
               fechaALarga(m->fecha), m->dni, m->patente, m->medioPago);
        if (strcmp(m->tipoMov, MOV_PAGO) == 0) {
            totalPagos += m->monto;
        }
    }
    printf("----------------------------------------------------------------------------------------\n");
    printf("Total de movimientos: %d | Suma total de pagos: $%.2f\n", cantidad, totalPagos);

    liberarMovimientos(movimientos);
    pausarPantalla();
}

void submenuMovimientos(void) {
    int opcion = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("             SUBMENU PAGOS\n");
        printf("========================================\n");
        printf("1. Registrar pago\n");
        printf("2. Listar todos los movimientos\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");

        if (leerEntero("Seleccione una opcion (0-2): ", 0, 2, &opcion) != LEIDO) {
            break;
        }

        switch (opcion) {
            case 1: registrarPago(); break;
            case 2: listarMovimientos(); break;
            case 0: break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

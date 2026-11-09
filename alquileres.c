#include "alquileres.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "archivos.h"
#include "clientes.h"
#include "fechas.h"
#include "movimientos.h"
#include "utils.h"
#include "vehiculos.h"

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

static bool guardarAlquilerYVehiculos(const Alquiler* alquileres, int cantAlq,
                                     const Vehiculo* vehiculos, int cantVeh,
                                     const Alquiler* alquileresPrevios, int cantAlqPrev,
                                     const Vehiculo* vehiculosPrevios, int cantVehPrev) {
    if (!guardarAlquileres(alquileres, cantAlq)) {
        return false;
    }
    if (!guardarVehiculos(vehiculos, cantVeh)) {
        printf("[ERROR] No se pudo actualizar %s. Revirtiendo los cambios en %s...\n",
               ARCH_VEHICULOS, ARCH_ALQUILERES);
        guardarAlquileres(alquileresPrevios, cantAlqPrev);
        guardarVehiculos(vehiculosPrevios, cantVehPrev);
        return false;
    }
    return true;
}

static int buscarAlquilerActivoEnArray(const Alquiler* alquileres, int cantidad, int dni) {
    for (int i = 0; i < cantidad; ++i) {
        if (alquileres[i].dni == dni && strcmp(alquileres[i].estado, EST_ALQ_ACTIVO) == 0) {
            return alquileres[i].numAlquiler;
        }
    }
    return 0;
}

static int buscarAlquilerActivoVehiculoEnArray(const Alquiler* alquileres, int cantidad, const char* patente) {
    char normPatente[LON_PATENTE];
    normalizarPatente(patente, normPatente, sizeof(normPatente));
    for (int i = 0; i < cantidad; ++i) {
        char normActual[LON_PATENTE];
        normalizarPatente(alquileres[i].patente, normActual, sizeof(normActual));
        if (strcmp(normActual, normPatente) == 0 && strcmp(alquileres[i].estado, EST_ALQ_ACTIVO) == 0) {
            return alquileres[i].numAlquiler;
        }
    }
    return 0;
}

static int primeraFechaPago(int numAlquiler) {
    Movimiento* movimientos = NULL;
    int cantidad = 0;
    if (!cargarMovimientos(&movimientos, &cantidad)) {
        return -1;
    }
    int primera = 0;
    for (int i = 0; i < cantidad; ++i) {
        if (movimientos[i].numAlquiler == numAlquiler && strcmp(movimientos[i].tipoMov, MOV_PAGO) == 0) {
            if (primera == 0 || movimientos[i].fecha < primera) {
                primera = movimientos[i].fecha;
            }
        }
    }
    liberarMovimientos(movimientos);
    return primera;
}

bool cargarAlquileres(Alquiler** alquileres, int* cantidad) {
    char error[256];
    if (!cargarRegistrosGenerico(ARCH_ALQUILERES, sizeof(Alquiler), (void**)alquileres, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    if (*alquileres && *cantidad > 0) {
        for (int i = 0; i < *cantidad; ++i) {
            (*alquileres)[i].patente[LON_PATENTE - 1] = '\0';
            (*alquileres)[i].estado[LON_ESTADO - 1] = '\0';
        }
    }
    return true;
}

bool guardarAlquileres(const Alquiler* alquileres, int cantidad) {
    for (int i = 0; i < cantidad; ++i) {
        char errorRegistro[256];
        if (!validarDatosAlquiler(&alquileres[i], errorRegistro, sizeof(errorRegistro))) {
            printf("[ERROR] Alquiler Nro %d inconsistente: %s\n",
                   alquileres[i].numAlquiler, errorRegistro);
            return false;
        }
    }
    char error[256];
    if (!guardarRegistrosGenerico(ARCH_ALQUILERES, sizeof(Alquiler), alquileres, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    return true;
}

void liberarAlquileres(Alquiler* alquileres) {
    if (alquileres) {
        free(alquileres);
    }
}

int buscarIndiceAlquiler(const Alquiler* alquileres, int cantidad, int numero) {
    if (!alquileres) return -1;
    for (int i = 0; i < cantidad; ++i) {
        if (alquileres[i].numAlquiler == numero) {
            return i;
        }
    }
    return -1;
}

bool buscarAlquilerPorNumero(int numero, Alquiler* alquiler) {
    Alquiler* alquileres = NULL;
    int cantidad = 0;
    if (!cargarAlquileres(&alquileres, &cantidad)) {
        return false;
    }
    int indice = buscarIndiceAlquiler(alquileres, cantidad, numero);
    if (indice < 0) {
        liberarAlquileres(alquileres);
        return false;
    }
    if (alquiler) {
        *alquiler = alquileres[indice];
    }
    liberarAlquileres(alquileres);
    return true;
}

bool buscarAlquilerActivoPorPatente(const char* patente, Alquiler* alquiler) {
    Alquiler* alquileres = NULL;
    int cantidad = 0;
    if (!cargarAlquileres(&alquileres, &cantidad)) {
        return false;
    }
    char normPatente[LON_PATENTE];
    normalizarPatente(patente, normPatente, sizeof(normPatente));
    for (int i = 0; i < cantidad; ++i) {
        char normActual[LON_PATENTE];
        normalizarPatente(alquileres[i].patente, normActual, sizeof(normActual));
        if (strcmp(normActual, normPatente) == 0 && strcmp(alquileres[i].estado, EST_ALQ_ACTIVO) == 0) {
            if (alquiler) {
                *alquiler = alquileres[i];
            }
            liberarAlquileres(alquileres);
            return true;
        }
    }
    liberarAlquileres(alquileres);
    return false;
}

int clienteTieneAlquilerActivo(int dni) {
    Alquiler* alquileres = NULL;
    int cantidad = 0;
    if (!cargarAlquileres(&alquileres, &cantidad)) {
        return 0;
    }
    int resultado = buscarAlquilerActivoEnArray(alquileres, cantidad, dni);
    liberarAlquileres(alquileres);
    return resultado;
}

bool vehiculoTieneAlquilerActivo(const char* patente) {
    Alquiler* alquileres = NULL;
    int cantidad = 0;
    if (!cargarAlquileres(&alquileres, &cantidad)) {
        return false;
    }
    int resultado = buscarAlquilerActivoVehiculoEnArray(alquileres, cantidad, patente);
    liberarAlquileres(alquileres);
    return resultado > 0;
}

bool validarDatosAlquiler(const Alquiler* alquiler, char* error, size_t tamError) {
    if (!alquiler) {
        if (error && tamError > 0) snprintf(error, tamError, "Registro nulo.");
        return false;
    }
    if (alquiler->numAlquiler <= 0) {
        if (error && tamError > 0) snprintf(error, tamError, "El numero de alquiler debe ser mayor a cero.");
        return false;
    }
    if (alquiler->dni < DNI_MIN || alquiler->dni > DNI_MAX) {
        if (error && tamError > 0) snprintf(error, tamError, "El DNI del cliente no es valido.");
        return false;
    }
    if (!esPatenteValida(alquiler->patente)) {
        if (error && tamError > 0) snprintf(error, tamError, "La patente del vehiculo no es valida.");
        return false;
    }
    if (!esFechaValida(alquiler->fechaInicio)) {
        if (error && tamError > 0) snprintf(error, tamError, "La fecha de inicio no es valida.");
        return false;
    }
    if (alquiler->cantDias < 1 || alquiler->cantDias > DIAS_MAX_ALQUILER) {
        if (error && tamError > 0) snprintf(error, tamError, "La cantidad de dias debe estar entre 1 y %d.", DIAS_MAX_ALQUILER);
        return false;
    }
    if (!esFechaValida(alquiler->fechaPrevista)) {
        if (error && tamError > 0) snprintf(error, tamError, "La fecha prevista de devolucion no es valida.");
        return false;
    }
    if (alquiler->fechaPrevista != sumarDias(alquiler->fechaInicio, alquiler->cantDias)) {
        if (error && tamError > 0) snprintf(error, tamError, "La fecha prevista no coincide con el inicio mas la cantidad de dias.");
        return false;
    }
    if (!esFinito(alquiler->importeTotal) || alquiler->importeTotal < 0.0) {
        if (error && tamError > 0) snprintf(error, tamError, "El importe total no es un numero valido.");
        return false;
    }
    if (!esFinito(alquiler->importePagado) || alquiler->importePagado < 0.0) {
        if (error && tamError > 0) snprintf(error, tamError, "El importe pagado no es un numero valido.");
        return false;
    }
    if (alquiler->importePagado > alquiler->importeTotal + TOLERANCIA_SALDO) {
        if (error && tamError > 0) snprintf(error, tamError, "El importe pagado no puede superar el importe total.");
        return false;
    }
    if (!validarEstadoAlquiler(alquiler->estado)) {
        if (error && tamError > 0) snprintf(error, tamError, "El estado del alquiler no es valido.");
        return false;
    }
    if (alquiler->fechaEfectiva != 0) {
        if (!esFechaValida(alquiler->fechaEfectiva)) {
            if (error && tamError > 0) snprintf(error, tamError, "La fecha efectiva de devolucion no es valida.");
            return false;
        }
        if (alquiler->fechaEfectiva < alquiler->fechaInicio) {
            if (error && tamError > 0) snprintf(error, tamError, "La fecha efectiva no puede ser anterior a la fecha de inicio.");
            return false;
        }
        if (!esFechaPasadaOHoy(alquiler->fechaEfectiva)) {
            if (error && tamError > 0) snprintf(error, tamError, "La fecha efectiva no puede ser posterior a la fecha actual.");
            return false;
        }
    }

    if (strcmp(alquiler->estado, EST_ALQ_FINALIZADO) == 0 && alquiler->fechaEfectiva == 0) {
        if (error && tamError > 0) snprintf(error, tamError, "Un alquiler Finalizado debe tener fecha efectiva de devolucion.");
        return false;
    }
    if (strcmp(alquiler->estado, EST_ALQ_FINALIZADO) != 0 && alquiler->fechaEfectiva != 0) {
        if (error && tamError > 0) snprintf(error, tamError, "Solo un alquiler Finalizado puede tener fecha efectiva de devolucion.");
        return false;
    }
    return true;
}

int proximoNumeroAlquiler(void) {
    Alquiler* alquileres = NULL;
    int cantidad = 0;
    if (!cargarAlquileres(&alquileres, &cantidad)) {
        return 1;
    }
    int maximo = 0;
    for (int i = 0; i < cantidad; ++i) {
        if (alquileres[i].numAlquiler > maximo) {
            maximo = alquileres[i].numAlquiler;
        }
    }
    liberarAlquileres(alquileres);
    return maximo + 1;
}

void imprimirEncabezadoAlquileres(void) {
    printf("%-5s | %-28s | %-22s | %-10s | %-10s | %-10s | %-4s | %-12s | %-12s | ESTADO\n",
           "NRO", "CLIENTE (DNI - NOMBRE)", "VEHICULO (PAT - MARCA)", "INICIO", "PREVISTA", "DEV.", "DIAS", "TOTAL", "PAGADO");
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void imprimirFilaAlquiler(const Alquiler* alquiler, const Cliente* clientes, int cantClientes,
                          const Vehiculo* vehiculos, int cantVehiculos) {
    char nomCli[LON_NOMBRE];
    obtenerNombreCliente(clientes, cantClientes, alquiler->dni, nomCli, sizeof(nomCli));
    char clienteStr[128];
    snprintf(clienteStr, sizeof(clienteStr), "%d - %s", alquiler->dni, nomCli);

    char descVeh[LON_MARCA + LON_MODELO + 2];
    obtenerDescripcionVehiculo(vehiculos, cantVehiculos, alquiler->patente, descVeh, sizeof(descVeh));
    char vehiculoStr[128];
    snprintf(vehiculoStr, sizeof(vehiculoStr), "%s - %s", alquiler->patente, descVeh);

    const char* devStr = (alquiler->fechaEfectiva > 0) ? fechaALarga(alquiler->fechaEfectiva) : "-";

    printf("%-5d | %-28.28s | %-22.22s | %-10s | %-10s | %-10s | %-4d | $%11.2f | $%11.2f | %s\n",
           alquiler->numAlquiler, clienteStr, vehiculoStr,
           fechaALarga(alquiler->fechaInicio), fechaALarga(alquiler->fechaPrevista),
           devStr, alquiler->cantDias, alquiler->importeTotal, alquiler->importePagado,
           alquiler->estado);
}

void realizarAlquiler(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("           NUEVO ALQUILER\n");
    printf("========================================\n");

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
        pausarPantalla();
        return;
    }

    int dni = 0;
    int indiceCliente = -1;
    char patente[LON_PATENTE];
    int indiceVehiculo = -1;
    int fechaInicio = 0;
    int cantidadDias = 0;
    char prompt[128];
    int paso = 0;
    EstadoLectura est;

    while (1) {
        switch (paso) {
            case 0: {
                est = leerDNI("Ingrese el DNI del cliente (entre 1.000.000 y 50.000.000) (V = volver): ", &dni);
                if (est == VOLVER) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    return;
                }
                indiceCliente = buscarIndiceCliente(clientes, cantClientes, dni);
                if (indiceCliente < 0) {
                    printf("\n[ERROR] El cliente con DNI %d no existe en el sistema. Reintente.\n", dni);
                    continue;
                }
                if (clientes[indiceCliente].estado != CLI_ACTIVO) {
                    printf("\n[ERROR] El cliente se encuentra INACTIVO. Operacion rechazada. Reintente.\n");
                    continue;
                }
                int alquilerActivo = buscarAlquilerActivoEnArray(alquileres, cantAlquileres, dni);
                if (alquilerActivo > 0) {
                    printf("\n[ERROR] El cliente ya posee el alquiler Nro %d en estado ACTIVO.\n", alquilerActivo);
                    printf("Cada cliente puede tener como maximo un alquiler activo a la vez. Reintente.\n");
                    continue;
                }
                paso++;
                break;
            }
            case 1: {
                est = leerPatente("Ingrese la patente del vehiculo a alquilar (V = volver): ",
                                  patente, sizeof(patente));
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    return;
                }
                indiceVehiculo = buscarIndiceVehiculo(vehiculos, cantVehiculos, patente);
                if (indiceVehiculo < 0) {
                    printf("\n[ERROR] El vehiculo con patente %s no existe en el sistema. Reintente.\n", patente);
                    continue;
                }
                if (strcmp(vehiculos[indiceVehiculo].estado, EST_VEH_DISPONIBLE) != 0) {
                    printf("\n[ERROR] El vehiculo no esta disponible. Estado actual: %s. Reintente.\n",
                           vehiculos[indiceVehiculo].estado);
                    continue;
                }
                int alquilerVehiculo = buscarAlquilerActivoVehiculoEnArray(alquileres, cantAlquileres, patente);
                if (alquilerVehiculo > 0) {
                    printf("\n[ERROR] El vehiculo ya tiene el alquiler Nro %d en estado ACTIVO. Reintente.\n", alquilerVehiculo);
                    continue;
                }
                paso++;
                break;
            }
            case 2: {
                snprintf(prompt, sizeof(prompt),
                         "Ingrese la fecha de inicio del alquiler (AAAAMMDD, desde hoy hasta hoy + %d dias) (V = volver): ",
                         DIAS_MAX_INICIO_FUTURO);
                est = leerFechaEnRango(prompt, fechaActual(),
                                       sumarDias(fechaActual(), DIAS_MAX_INICIO_FUTURO), &fechaInicio);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    return;
                }
                paso++;
                break;
            }
            case 3: {
                snprintf(prompt, sizeof(prompt), "Ingrese la cantidad de dias contratados (1-%d) (V = volver): ",
                         DIAS_MAX_ALQUILER);
                est = leerEntero(prompt, 1, DIAS_MAX_ALQUILER, &cantidadDias);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    return;
                }
                paso++;
                break;
            }
            case 4: {
                const Cliente* cliente = &clientes[indiceCliente];
                Vehiculo* vehiculo = &vehiculos[indiceVehiculo];
                int fechaPrevista = sumarDias(fechaInicio, cantidadDias);
                double importeTotal = cantidadDias * vehiculo->precioDia;

                printf("\n---------------- RESUMEN DEL ALQUILER ----------------\n");
                printf("Cliente:            %d - %s\n", cliente->dni, cliente->apellidoNombre);
                printf("Vehiculo:           %s - %s %s\n", vehiculo->patente, vehiculo->marca, vehiculo->modelo);
                printf("Fecha de Inicio:    %s\n", fechaALarga(fechaInicio));
                printf("Fecha Prevista:     %s\n", fechaALarga(fechaPrevista));
                printf("Dias Contratados:   %d\n", cantidadDias);
                printf("Tarifa por dia:     $%.2f\n", vehiculo->precioDia);
                printf("Importe Total:      $%.2f\n", importeTotal);
                printf("-------------------------------------------------------\n");

                bool confirmar = false;
                est = leerSN("¿Confirma el registro del alquiler (S/N)? (V = volver): ", &confirmar);
                if (est == VOLVER) {
                    paso = 3;
                    break;
                }
                if (est != LEIDO || !confirmar) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }

                Alquiler nuevo;
                memset(&nuevo, 0, sizeof(nuevo));
                nuevo.numAlquiler = proximoNumeroAlquiler();
                nuevo.dni = dni;
                copiarCadena(nuevo.patente, sizeof(nuevo.patente), patente);
                nuevo.fechaInicio = fechaInicio;
                nuevo.fechaPrevista = fechaPrevista;
                nuevo.fechaEfectiva = 0;
                nuevo.cantDias = cantidadDias;
                nuevo.importeTotal = importeTotal;
                nuevo.importePagado = 0.0;
                copiarCadena(nuevo.estado, sizeof(nuevo.estado), EST_ALQ_ACTIVO);

                /* Guardar copias previas para rollback */
                Alquiler* alquileresPrevios = (Alquiler*)malloc((size_t)cantAlquileres * sizeof(Alquiler));
                if (cantAlquileres > 0 && alquileresPrevios) {
                    memcpy(alquileresPrevios, alquileres, (size_t)cantAlquileres * sizeof(Alquiler));
                }
                Vehiculo* vehiculosPrevios = (Vehiculo*)malloc((size_t)cantVehiculos * sizeof(Vehiculo));
                if (cantVehiculos > 0 && vehiculosPrevios) {
                    memcpy(vehiculosPrevios, vehiculos, (size_t)cantVehiculos * sizeof(Vehiculo));
                }

                Alquiler* tempAlq = (Alquiler*)realloc(alquileres, (size_t)(cantAlquileres + 1) * sizeof(Alquiler));
                if (!tempAlq) {
                    printf("\n[ERROR] Memoria insuficiente.\n");
                    free(alquileresPrevios);
                    free(vehiculosPrevios);
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }
                alquileres = tempAlq;
                alquileres[cantAlquileres++] = nuevo;

                copiarCadena(vehiculo->estado, sizeof(vehiculo->estado), EST_VEH_ALQUILADO);

                if (!guardarAlquilerYVehiculos(alquileres, cantAlquileres, vehiculos, cantVehiculos,
                                             alquileresPrevios, cantAlquileres - 1,
                                             vehiculosPrevios, cantVehiculos)) {
                    printf("\n[ERROR] No se pudo registrar el alquiler.\n");
                    free(alquileresPrevios);
                    free(vehiculosPrevios);
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }

                free(alquileresPrevios);
                free(vehiculosPrevios);

                printf("\n========================================\n");
                printf(">>> ALQUILER REGISTRADO EXITOSAMENTE\n");
                printf("Numero de Alquiler: %d\n", nuevo.numAlquiler);
                printf("Cliente:            %s (DNI: %d)\n", cliente->apellidoNombre, cliente->dni);
                printf("Vehiculo:           %s %s (Patente: %s)\n", vehiculo->marca, vehiculo->modelo, patente);
                printf("Fecha de Inicio:    %s\n", fechaALarga(nuevo.fechaInicio));
                printf("Fecha Prevista:     %s\n", fechaALarga(nuevo.fechaPrevista));
                printf("Dias Contratados:   %d\n", nuevo.cantDias);
                printf("Importe Total:      $%.2f\n", nuevo.importeTotal);
                printf("Estado Alquiler:    %s\n", EST_ALQ_ACTIVO);
                printf("Estado Vehiculo:    %s\n", EST_VEH_ALQUILADO);
                printf("========================================\n");

                liberarClientes(clientes);
                liberarVehiculos(vehiculos);
                liberarAlquileres(alquileres);
                pausarPantalla();
                return;
            }
        }
    }
}

void modificarAlquiler(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("          MODIFICAR ALQUILER\n");
    printf("========================================\n");

    int numero = 0;
    EstadoLectura est = leerEntero("Ingrese el numero de alquiler a modificar (V = volver): ", 1, 9999999, &numero);
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
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
        pausarPantalla();
        return;
    }

    int indiceAlquiler = buscarIndiceAlquiler(alquileres, cantAlquileres, numero);
    if (indiceAlquiler < 0) {
        printf("\n[ERROR] El alquiler Nro %d no existe.\n", numero);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }
    Alquiler* alquiler = &alquileres[indiceAlquiler];
    if (strcmp(alquiler->estado, EST_ALQ_ACTIVO) != 0) {
        printf("\n[ERROR] Solo se pueden modificar alquileres en estado ACTIVO. Estado actual: %s.\n", alquiler->estado);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    char patenteActual[LON_PATENTE];
    copiarCadena(patenteActual, sizeof(patenteActual), alquiler->patente);
    int indiceVehiculo = buscarIndiceVehiculo(vehiculos, cantVehiculos, patenteActual);
    if (indiceVehiculo < 0) {
        printf("\n[ERROR] No se encontro el vehiculo con patente %s asociado al alquiler.\n", patenteActual);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    char nomCli[LON_NOMBRE];
    obtenerNombreCliente(clientes, cantClientes, alquiler->dni, nomCli, sizeof(nomCli));
    Vehiculo* vehiculoActual = &vehiculos[indiceVehiculo];

    printf("\nAlquiler Nro: %d | Cliente: %d - %s\n", alquiler->numAlquiler, alquiler->dni, nomCli);
    printf("Vehiculo: %s | %s %s | Tarifa/dia: $%.2f\n",
           patenteActual, vehiculoActual->marca, vehiculoActual->modelo, vehiculoActual->precioDia);
    printf("Inicio: %s | Dias: %d | Prevista: %s | Total: $%.2f | Pagado: $%.2f\n",
           fechaALarga(alquiler->fechaInicio), alquiler->cantDias, fechaALarga(alquiler->fechaPrevista),
           alquiler->importeTotal, alquiler->importePagado);
    printf("--------------------------------------------------------------------\n");

    Alquiler* alquileresPrevios = (Alquiler*)malloc((size_t)cantAlquileres * sizeof(Alquiler));
    if (cantAlquileres > 0 && alquileresPrevios) {
        memcpy(alquileresPrevios, alquileres, (size_t)cantAlquileres * sizeof(Alquiler));
    }
    Vehiculo* vehiculosPrevios = (Vehiculo*)malloc((size_t)cantVehiculos * sizeof(Vehiculo));
    if (cantVehiculos > 0 && vehiculosPrevios) {
        memcpy(vehiculosPrevios, vehiculos, (size_t)cantVehiculos * sizeof(Vehiculo));
    }

    bool cambiarVehiculo = false;
    EstadoLectura estSN = leerSN("Desea cambiar el vehiculo asignado (S/N)? (V = volver): ", &cambiarVehiculo);
    if (estSN == VOLVER) {
        printf("\nModificacion cancelada.\n");
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }
    if (estSN != LEIDO) {
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        return;
    }
    if (cambiarVehiculo) {
        char nuevaPatente[LON_PATENTE];
        est = leerPatente("Ingrese la nueva patente (V = volver): ", nuevaPatente, sizeof(nuevaPatente));
        if (est == VOLVER) {
            printf("\nModificacion cancelada.\n");
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            pausarPantalla();
            return;
        }
        if (est != LEIDO) {
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            return;
        }
        int indiceNuevo = buscarIndiceVehiculo(vehiculos, cantVehiculos, nuevaPatente);
        if (indiceNuevo < 0) {
            printf("\n[ERROR] El vehiculo con patente %s no existe.\n", nuevaPatente);
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            pausarPantalla();
            return;
        }
        if (strcmp(nuevaPatente, patenteActual) == 0) {
            printf("\n[AVISO] La patente ingresada es la misma que la actual.\n");
        } else {
            Vehiculo* vehiculoNuevo = &vehiculos[indiceNuevo];
            if (strcmp(vehiculoNuevo->estado, EST_VEH_DISPONIBLE) != 0) {
                printf("\n[ERROR] El nuevo vehiculo no esta disponible. Estado actual: %s.\n", vehiculoNuevo->estado);
                free(alquileresPrevios);
                free(vehiculosPrevios);
                liberarClientes(clientes);
                liberarVehiculos(vehiculos);
                liberarAlquileres(alquileres);
                pausarPantalla();
                return;
            }
            copiarCadena(vehiculos[indiceVehiculo].estado, sizeof(vehiculos[indiceVehiculo].estado), EST_VEH_DISPONIBLE);
            copiarCadena(vehiculos[indiceNuevo].estado, sizeof(vehiculos[indiceNuevo].estado), EST_VEH_ALQUILADO);
            copiarCadena(alquiler->patente, sizeof(alquiler->patente), nuevaPatente);
            indiceVehiculo = indiceNuevo;
            copiarCadena(patenteActual, sizeof(patenteActual), nuevaPatente);
            printf("\nVehiculo cambiado a %s (%s %s).\n",
                   nuevaPatente, vehiculos[indiceNuevo].marca, vehiculos[indiceNuevo].modelo);
        }
    }

    bool cambiarFecha = false;
    estSN = leerSN("Desea cambiar la fecha de inicio (S/N)? (V = volver): ", &cambiarFecha);
    if (estSN == VOLVER) {
        printf("\nModificacion cancelada.\n");
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }
    if (estSN != LEIDO) {
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        return;
    }
    if (cambiarFecha) {
        int primeraPago = primeraFechaPago(alquiler->numAlquiler);
        if (primeraPago < 0) {
            printf("\n[ERROR] No se pudieron cargar los movimientos del alquiler.\n");
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            pausarPantalla();
            return;
        }
        int fechaMinima = fechaActual();
        int fechaMaxima = sumarDias(fechaMinima, DIAS_MAX_INICIO_FUTURO);
        if (primeraPago > 0 && primeraPago < fechaMaxima) {
            fechaMaxima = primeraPago;
        }
        if (fechaMaxima < fechaMinima) {
            printf("\n[ERROR] No se puede cambiar la fecha de inicio: ya existen pagos registrados con fecha anterior a hoy (%s).\n",
                   fechaALarga(primeraPago));
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            pausarPantalla();
            return;
        }
        int nuevaFecha = 0;
        est = leerFechaEnRango("Ingrese la nueva fecha de inicio (AAAAMMDD) (V = volver): ",
                                fechaMinima, fechaMaxima, &nuevaFecha);
        if (est == VOLVER) {
            printf("\nModificacion cancelada.\n");
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            pausarPantalla();
            return;
        }
        if (est != LEIDO) {
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            return;
        }
        if (primeraPago > 0 && nuevaFecha > primeraPago) {
            printf("\n[ERROR] La fecha de inicio no puede ser posterior al primer pago registrado (%s).\n",
                   fechaALarga(primeraPago));
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            pausarPantalla();
            return;
        }
        alquiler->fechaInicio = nuevaFecha;
    }

    bool cambiarDias = false;
    estSN = leerSN("Desea cambiar la cantidad de dias contratados (S/N)? (V = volver): ", &cambiarDias);
    if (estSN == VOLVER) {
        printf("\nModificacion cancelada.\n");
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }
    if (estSN != LEIDO) {
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        return;
    }
    if (cambiarDias) {
        int nuevosDias = 0;
        char prompt[128];
        snprintf(prompt, sizeof(prompt), "Ingrese la nueva cantidad de dias (1-%d) (V = volver): ", DIAS_MAX_ALQUILER);
        est = leerEntero(prompt, 1, DIAS_MAX_ALQUILER, &nuevosDias);
        if (est == VOLVER) {
            printf("\nModificacion cancelada.\n");
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            pausarPantalla();
            return;
        }
        if (est != LEIDO) {
            free(alquileresPrevios);
            free(vehiculosPrevios);
            liberarClientes(clientes);
            liberarVehiculos(vehiculos);
            liberarAlquileres(alquileres);
            return;
        }
        alquiler->cantDias = nuevosDias;
    }

    alquiler->fechaPrevista = sumarDias(alquiler->fechaInicio, alquiler->cantDias);
    const Vehiculo* vehiculoFinal = &vehiculos[indiceVehiculo];
    double nuevoTotal = alquiler->cantDias * vehiculoFinal->precioDia;
    if (nuevoTotal + TOLERANCIA_SALDO < alquiler->importePagado) {
        printf("\n[ERROR] El nuevo importe total ($%.2f) no puede ser menor a lo ya pagado ($%.2f).\n",
               nuevoTotal, alquiler->importePagado);
        printf("Modificacion cancelada.\n");
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }
    if (nuevoTotal < alquiler->importePagado) {
        nuevoTotal = alquiler->importePagado;
    }
    double diferencia = nuevoTotal - alquiler->importeTotal;
    alquiler->importeTotal = nuevoTotal;

    if (!guardarAlquilerYVehiculos(alquileres, cantAlquileres, vehiculos, cantVehiculos,
                                 alquileresPrevios, cantAlquileres,
                                 vehiculosPrevios, cantVehiculos)) {
        printf("\n[ERROR] No se pudo guardar la modificacion del alquiler.\n");
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    free(alquileresPrevios);
    free(vehiculosPrevios);

    if (fabs(diferencia) > TOLERANCIA_SALDO) {
        char errorMov[256];
        if (!registrarMovimiento(alquiler->numAlquiler, MOV_AJUSTE, diferencia,
                                 fechaActual(), MEDIO_NINGUNO, errorMov, sizeof(errorMov))) {
            printf("[AVISO] El alquiler se modifico, pero no se pudo registrar el movimiento de ajuste: %s\n", errorMov);
        }
    }

    printf("\n>>> Alquiler modificado exitosamente.\n");
    printf("Nueva Fecha Prevista: %s\n", fechaALarga(alquiler->fechaPrevista));
    printf("Nuevo Importe Total:  $%.2f\n", alquiler->importeTotal);

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarPantalla();
}

void listarAlquileres(void) {
    limpiarPantalla();
    printf("\n=============================================================================================================================================\n");
    printf("                                           LISTADO GENERAL DE ALQUILERES\n");
    printf("=============================================================================================================================================\n");

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
        pausarPantalla();
        return;
    }
    if (cantAlquileres == 0) {
        printf("No hay alquileres registrados.\n");
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    imprimirEncabezadoAlquileres();
    for (int i = 0; i < cantAlquileres; ++i) {
        imprimirFilaAlquiler(&alquileres[i], clientes, cantClientes, vehiculos, cantVehiculos);
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres registrados: %d\n", cantAlquileres);

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarPantalla();
}

void registrarDevolucion(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("        REGISTRAR DEVOLUCION\n");
    printf("========================================\n");

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
        pausarPantalla();
        return;
    }

    int metodo = 0;
    int indiceAlquiler = -1;
    int indiceVehiculo = -1;
    int fechaDevolucion = 0;
    char patente[LON_PATENTE];
    patente[0] = '\0';
    int numero = 0;
    int paso = 0;
    EstadoLectura est;

    while (1) {
        switch (paso) {
            case 0: {
                printf("Seleccione metodo de identificacion:\n");
                printf("1. Por patente de vehiculo\n");
                printf("2. Por numero de alquiler\n");
                est = leerEntero("Opcion (1-2) (V = volver): ", 1, 2, &metodo);
                if (est == VOLVER) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    return;
                }
                paso++;
                break;
            }
            case 1: {
                indiceAlquiler = -1;
                indiceVehiculo = -1;
                if (metodo == 1) {
                    est = leerPatente("Ingrese la patente del vehiculo devuelto (V = volver): ",
                                      patente, sizeof(patente));
                    if (est == VOLVER) {
                        paso--;
                        break;
                    }
                    if (est != LEIDO) {
                        liberarClientes(clientes);
                        liberarVehiculos(vehiculos);
                        liberarAlquileres(alquileres);
                        return;
                    }
                    char normPatente[LON_PATENTE];
                    normalizarPatente(patente, normPatente, sizeof(normPatente));
                    for (int i = 0; i < cantAlquileres; ++i) {
                        char normActual[LON_PATENTE];
                        normalizarPatente(alquileres[i].patente, normActual, sizeof(normActual));
                        if (strcmp(normActual, normPatente) == 0 && strcmp(alquileres[i].estado, EST_ALQ_ACTIVO) == 0) {
                            indiceAlquiler = i;
                            break;
                        }
                    }
                    if (indiceAlquiler < 0) {
                        printf("\n[ERROR] No existe ningun alquiler ACTIVO para el vehiculo con patente %s. Reintente.\n", patente);
                        continue;
                    }
                } else {
                    est = leerEntero("Ingrese el numero de alquiler (V = volver): ", 1, 9999999, &numero);
                    if (est == VOLVER) {
                        paso--;
                        break;
                    }
                    if (est != LEIDO) {
                        liberarClientes(clientes);
                        liberarVehiculos(vehiculos);
                        liberarAlquileres(alquileres);
                        return;
                    }
                    indiceAlquiler = buscarIndiceAlquiler(alquileres, cantAlquileres, numero);
                    if (indiceAlquiler < 0) {
                        printf("\n[ERROR] El alquiler Nro %d no existe. Reintente.\n", numero);
                        continue;
                    }
                    if (strcmp(alquileres[indiceAlquiler].estado, EST_ALQ_ACTIVO) != 0) {
                        printf("\n[ERROR] El alquiler Nro %d no se encuentra en estado ACTIVO. Estado actual: %s. Reintente.\n",
                               numero, alquileres[indiceAlquiler].estado);
                        continue;
                    }
                }

                Alquiler* alq = &alquileres[indiceAlquiler];
                copiarCadena(patente, sizeof(patente), alq->patente);
                indiceVehiculo = buscarIndiceVehiculo(vehiculos, cantVehiculos, patente);
                if (indiceVehiculo < 0) {
                    printf("\n[ERROR] No se encontro el vehiculo con patente %s asociado al alquiler.\n", patente);
                    continue;
                }

                if (alq->fechaInicio > fechaActual()) {
                    printf("\n[ERROR] El alquiler comienza el %s; no puede devolverse antes de esa fecha. Reintente.\n",
                           fechaALarga(alq->fechaInicio));
                    continue;
                }

                char nomCli[LON_NOMBRE];
                obtenerNombreCliente(clientes, cantClientes, alq->dni, nomCli, sizeof(nomCli));
                printf("\nAlquiler ACTIVO localizado: Nro %d | Cliente: %d - %s\n", alq->numAlquiler, alq->dni, nomCli);
                printf("Vehiculo: %s (%s %s) | Tarifa: $%.2f/dia\n",
                       patente, vehiculos[indiceVehiculo].marca, vehiculos[indiceVehiculo].modelo,
                       vehiculos[indiceVehiculo].precioDia);
                printf("Fecha Inicio: %s | Prevista: %s\n",
                       fechaALarga(alq->fechaInicio), fechaALarga(alq->fechaPrevista));

                paso++;
                break;
            }
            case 2: {
                Alquiler* alq = &alquileres[indiceAlquiler];
                est = leerFechaEnRango("Ingrese la fecha efectiva de devolucion (AAAAMMDD) (V = volver): ",
                                       alq->fechaInicio, fechaActual(), &fechaDevolucion);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    return;
                }
                paso++;
                break;
            }
            case 3: {
                Alquiler* alquiler = &alquileres[indiceAlquiler];
                Vehiculo* vehiculo = &vehiculos[indiceVehiculo];
                char nomCli[LON_NOMBRE];
                obtenerNombreCliente(clientes, cantClientes, alquiler->dni, nomCli, sizeof(nomCli));

                int diasAdicionales = diasEntre(alquiler->fechaPrevista, fechaDevolucion);
                double recargo = 0.0;
                if (diasAdicionales > 0) {
                    recargo = diasAdicionales * vehiculo->precioDia;
                }
                double nuevoTotal = alquiler->importeTotal + recargo;

                printf("\n---------------- RESUMEN DE DEVOLUCION ----------------\n");
                printf("Alquiler Nro:            %d\n", alquiler->numAlquiler);
                printf("Cliente:                 %d - %s\n", alquiler->dni, nomCli);
                printf("Vehiculo:                %s - %s %s\n", patente, vehiculo->marca, vehiculo->modelo);
                printf("Fecha Inicio:            %s\n", fechaALarga(alquiler->fechaInicio));
                printf("Fecha Prevista:          %s\n", fechaALarga(alquiler->fechaPrevista));
                printf("Fecha Efectiva:          %s\n", fechaALarga(fechaDevolucion));
                if (diasAdicionales > 0) {
                    printf("Dias de Demora:          %d\n", diasAdicionales);
                    printf("Recargo por Demora:      $%.2f\n", recargo);
                } else {
                    printf("Devolucion en plazo o anticipada (sin recargo).\n");
                }
                printf("Importe Total Final:     $%.2f\n", nuevoTotal);
                printf("Importe Pagado:          $%.2f\n", alquiler->importePagado);
                printf("Saldo Pendiente:         $%.2f\n", nuevoTotal - alquiler->importePagado);
                printf("-------------------------------------------------------\n");

                bool confirmar = false;
                est = leerSN("¿Confirma la devolucion (S/N)? (V = volver): ", &confirmar);
                if (est == VOLVER) {
                    paso = 2;
                    break;
                }
                if (est != LEIDO || !confirmar) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }

                Alquiler* alquileresPrevios = (Alquiler*)malloc((size_t)cantAlquileres * sizeof(Alquiler));
                if (cantAlquileres > 0 && alquileresPrevios) {
                    memcpy(alquileresPrevios, alquileres, (size_t)cantAlquileres * sizeof(Alquiler));
                }
                Vehiculo* vehiculosPrevios = (Vehiculo*)malloc((size_t)cantVehiculos * sizeof(Vehiculo));
                if (cantVehiculos > 0 && vehiculosPrevios) {
                    memcpy(vehiculosPrevios, vehiculos, (size_t)cantVehiculos * sizeof(Vehiculo));
                }

                alquiler->importeTotal = nuevoTotal;
                alquiler->fechaEfectiva = fechaDevolucion;
                copiarCadena(alquiler->estado, sizeof(alquiler->estado), EST_ALQ_FINALIZADO);
                copiarCadena(vehiculos[indiceVehiculo].estado, sizeof(vehiculos[indiceVehiculo].estado), EST_VEH_DISPONIBLE);

                if (!guardarAlquilerYVehiculos(alquileres, cantAlquileres, vehiculos, cantVehiculos,
                                             alquileresPrevios, cantAlquileres,
                                             vehiculosPrevios, cantVehiculos)) {
                    printf("\n[ERROR] No se pudo registrar la devolucion.\n");
                    free(alquileresPrevios);
                    free(vehiculosPrevios);
                    liberarClientes(clientes);
                    liberarVehiculos(vehiculos);
                    liberarAlquileres(alquileres);
                    pausarPantalla();
                    return;
                }

                free(alquileresPrevios);
                free(vehiculosPrevios);

                if (recargo > TOLERANCIA_SALDO) {
                    char errorMov[256];
                    if (!registrarMovimiento(alquiler->numAlquiler, MOV_AJUSTE, recargo,
                                             fechaDevolucion, MEDIO_NINGUNO, errorMov, sizeof(errorMov))) {
                        printf("[AVISO] La devolucion se registro, pero no se pudo guardar el movimiento de ajuste: %s\n", errorMov);
                    }
                }

                printf("\n========================================\n");
                printf(">>> DEVOLUCION REGISTRADA EXITOSAMENTE\n");
                printf("Alquiler Nro:            %d\n", alquiler->numAlquiler);
                printf("Fecha Efectiva:          %s\n", fechaALarga(alquiler->fechaEfectiva));
                printf("Importe Total Final:     $%.2f\n", alquiler->importeTotal);
                printf("Importe Pagado:          $%.2f\n", alquiler->importePagado);
                printf("Saldo Pendiente:         $%.2f\n", alquiler->importeTotal - alquiler->importePagado);
                printf("Estado del Vehiculo:     %s\n", EST_VEH_DISPONIBLE);
                printf("========================================\n");

                liberarClientes(clientes);
                liberarVehiculos(vehiculos);
                liberarAlquileres(alquileres);
                pausarPantalla();
                return;
            }
        }
    }
}

void cancelarAlquiler(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("          CANCELAR ALQUILER\n");
    printf("========================================\n");

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
        pausarPantalla();
        return;
    }

    int numero = 0;
    EstadoLectura est = leerEntero("Ingrese el numero de alquiler a cancelar (V = volver): ", 1, 9999999, &numero);
    if (est == VOLVER) {
        printf("\nOperacion cancelada.\n");
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        return;
    }
    int indiceAlquiler = buscarIndiceAlquiler(alquileres, cantAlquileres, numero);
    if (indiceAlquiler < 0) {
        printf("\n[ERROR] El alquiler Nro %d no existe.\n", numero);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }
    Alquiler* alquiler = &alquileres[indiceAlquiler];
    if (strcmp(alquiler->estado, EST_ALQ_ACTIVO) != 0) {
        printf("\n[ERROR] Solo se pueden cancelar alquileres en estado ACTIVO. Estado actual: %s.\n", alquiler->estado);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    char patente[LON_PATENTE];
    copiarCadena(patente, sizeof(patente), alquiler->patente);
    int indiceVehiculo = buscarIndiceVehiculo(vehiculos, cantVehiculos, patente);
    if (indiceVehiculo < 0) {
        printf("\n[ERROR] No se encontro el vehiculo con patente %s asociado al alquiler.\n", patente);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    char nomCli[LON_NOMBRE];
    obtenerNombreCliente(clientes, cantClientes, alquiler->dni, nomCli, sizeof(nomCli));

    printf("\nAlquiler Nro: %d | Cliente: %d - %s\n", alquiler->numAlquiler, alquiler->dni, nomCli);
    printf("Vehiculo: %s | %s %s\n", patente, vehiculos[indiceVehiculo].marca, vehiculos[indiceVehiculo].modelo);
    printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", alquiler->importeTotal, alquiler->importePagado);
    printf("--------------------------------------------------------------------\n");

    bool confirmar = false;
    est = leerSN("Desea confirmar la cancelacion del alquiler (S/N)? (V = volver): ", &confirmar);
    if (est == VOLVER || est != LEIDO || !confirmar) {
        printf("\nOperacion cancelada por el usuario.\n");
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    Alquiler* alquileresPrevios = (Alquiler*)malloc((size_t)cantAlquileres * sizeof(Alquiler));
    if (cantAlquileres > 0 && alquileresPrevios) {
        memcpy(alquileresPrevios, alquileres, (size_t)cantAlquileres * sizeof(Alquiler));
    }
    Vehiculo* vehiculosPrevios = (Vehiculo*)malloc((size_t)cantVehiculos * sizeof(Vehiculo));
    if (cantVehiculos > 0 && vehiculosPrevios) {
        memcpy(vehiculosPrevios, vehiculos, (size_t)cantVehiculos * sizeof(Vehiculo));
    }

    double totalAnterior = alquiler->importeTotal;
    alquiler->importeTotal = alquiler->importePagado;
    copiarCadena(alquiler->estado, sizeof(alquiler->estado), EST_ALQ_CANCELADO);
    copiarCadena(vehiculos[indiceVehiculo].estado, sizeof(vehiculos[indiceVehiculo].estado), EST_VEH_DISPONIBLE);

    if (!guardarAlquilerYVehiculos(alquileres, cantAlquileres, vehiculos, cantVehiculos,
                                 alquileresPrevios, cantAlquileres,
                                 vehiculosPrevios, cantVehiculos)) {
        printf("\n[ERROR] No se pudo cancelar el alquiler.\n");
        free(alquileresPrevios);
        free(vehiculosPrevios);
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        pausarPantalla();
        return;
    }

    free(alquileresPrevios);
    free(vehiculosPrevios);

    if (totalAnterior - alquiler->importePagado > TOLERANCIA_SALDO) {
        double ajuste = alquiler->importePagado - totalAnterior;
        char errorMov[256];
        if (!registrarMovimiento(alquiler->numAlquiler, MOV_AJUSTE, ajuste,
                                 fechaActual(), MEDIO_NINGUNO, errorMov, sizeof(errorMov))) {
            printf("[AVISO] El alquiler se cancelo, pero no se pudo registrar el movimiento de ajuste: %s\n", errorMov);
        }
    }

    printf("\n========================================\n");
    printf(">>> ALQUILER CANCELADO EXITOSAMENTE\n");
    printf("Estado del Alquiler: %s\n", EST_ALQ_CANCELADO);
    printf("Importe Retenido:    $%.2f\n", alquiler->importePagado);
    printf("Saldo Pendiente:     $0.00\n");
    printf("Estado del Vehiculo: %s\n", EST_VEH_DISPONIBLE);
    printf("========================================\n");

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    pausarPantalla();
}

void submenuAlquileres(void) {
    int opcion = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("           SUBMENU ALQUILERES\n");
        printf("========================================\n");
        printf("1. Realizar nuevo alquiler\n");
        printf("2. Modificar alquiler\n");
        printf("3. Listar alquileres\n");
        printf("4. Cancelar alquiler\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");

        if (leerEntero("Seleccione una opcion (0-4): ", 0, 4, &opcion) != LEIDO) {
            break;
        }

        switch (opcion) {
            case 1: realizarAlquiler(); break;
            case 2: modificarAlquiler(); break;
            case 3: listarAlquileres(); break;
            case 4: cancelarAlquiler(); break;
            case 0: break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

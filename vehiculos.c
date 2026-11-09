#include "vehiculos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alquileres.h"
#include "archivos.h"
#include "fechas.h"
#include "utils.h"

static EstadoLectura pedirMarca(const char* mensaje, bool permitirVacio, char* salida, size_t tamSalida) {
    char buffer[LON_MARCA];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, buffer, sizeof(buffer), permitirVacio, LON_MARCA - 1);
        if (est != LEIDO) {
            return est;
        }
        if (buffer[0] == '\0' && permitirVacio) {
            salida[0] = '\0';
            return LEIDO;
        }
        if (validarMarca(buffer)) {
            copiarCadena(salida, tamSalida, buffer);
            return LEIDO;
        }
        printf("[ERROR] Marca invalida. Minimo 2 caracteres, con al menos una letra; "
               "puede usar espacios, '-', '.' o '&'. Ej: Toyota. Reintente.\n");
    }
}

static EstadoLectura pedirModelo(const char* mensaje, bool permitirVacio, char* salida, size_t tamSalida) {
    char buffer[LON_MODELO];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, buffer, sizeof(buffer), permitirVacio, LON_MODELO - 1);
        if (est != LEIDO) {
            return est;
        }
        if (buffer[0] == '\0' && permitirVacio) {
            salida[0] = '\0';
            return LEIDO;
        }
        if (validarModelo(buffer)) {
            copiarCadena(salida, tamSalida, buffer);
            return LEIDO;
        }
        printf("[ERROR] Modelo invalido. Minimo 2 caracteres, con al menos una letra o digito; "
               "puede usar espacios, '-', '.', '/' o '&'. Ej: Corolla, 208, C4. Reintente.\n");
    }
}

static EstadoLectura pedirTipo(const char* mensaje, bool permitirVacio, char* salida, size_t tamSalida) {
    while (1) {
        printf("Tipos disponibles:\n");
        printf("1. Auto\n");
        printf("2. Camioneta\n");
        printf("3. Utilitario\n");
        printf("4. Moto\n");
        printf("5. 4x4\n");
        if (permitirVacio) {
            printf("0. Mantener tipo actual\n");
        }
        int opcion = 0;
        EstadoLectura est = leerEntero(mensaje, 0, 5, &opcion);
        if (est != LEIDO) {
            return est;
        }
        if (opcion == 0) {
            if (permitirVacio) {
                salida[0] = '\0';
                return LEIDO;
            }
            printf("[ERROR] Debe seleccionar un tipo de vehiculo. Reintente.\n");
            continue;
        }
        static const char* tipos[] = {"Auto", "Camioneta", "Utilitario", "Moto", "4x4"};
        copiarCadena(salida, tamSalida, tipos[opcion - 1]);
        return LEIDO;
    }
}

static int anioMaximo(void) {
    return fechaActual() / 10000 + 1;
}

bool cargarVehiculos(Vehiculo** vehiculos, int* cantidad) {
    char error[256];
    if (!cargarRegistrosGenerico(ARCH_VEHICULOS, sizeof(Vehiculo), (void**)vehiculos, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    if (*vehiculos && *cantidad > 0) {
        for (int i = 0; i < *cantidad; ++i) {
            (*vehiculos)[i].patente[LON_PATENTE - 1] = '\0';
            (*vehiculos)[i].marca[LON_MARCA - 1] = '\0';
            (*vehiculos)[i].modelo[LON_MODELO - 1] = '\0';
            (*vehiculos)[i].tipo[LON_TIPO_VEH - 1] = '\0';
            (*vehiculos)[i].estado[LON_ESTADO - 1] = '\0';
        }
    }
    return true;
}

bool guardarVehiculos(const Vehiculo* vehiculos, int cantidad) {
    char error[256];
    if (!guardarRegistrosGenerico(ARCH_VEHICULOS, sizeof(Vehiculo), vehiculos, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    return true;
}

void liberarVehiculos(Vehiculo* vehiculos) {
    if (vehiculos) {
        free(vehiculos);
    }
}

int buscarIndiceVehiculo(const Vehiculo* vehiculos, int cantidad, const char* patente) {
    if (!vehiculos || !patente) return -1;
    char buscada[LON_PATENTE];
    normalizarPatente(patente, buscada, sizeof(buscada));
    for (int i = 0; i < cantidad; ++i) {
        char actual[LON_PATENTE];
        normalizarPatente(vehiculos[i].patente, actual, sizeof(actual));
        if (strcmp(actual, buscada) == 0) {
            return i;
        }
    }
    return -1;
}

bool buscarVehiculoPorPatente(const char* patente, Vehiculo* vehiculo) {
    Vehiculo* vehiculos = NULL;
    int cantidad = 0;
    if (!cargarVehiculos(&vehiculos, &cantidad)) {
        return false;
    }
    int indice = buscarIndiceVehiculo(vehiculos, cantidad, patente);
    if (indice < 0) {
        liberarVehiculos(vehiculos);
        return false;
    }
    if (vehiculo) {
        *vehiculo = vehiculos[indice];
    }
    liberarVehiculos(vehiculos);
    return true;
}

bool existeVehiculo(const char* patente) {
    Vehiculo* vehiculos = NULL;
    int cantidad = 0;
    if (!cargarVehiculos(&vehiculos, &cantidad)) {
        return false;
    }
    int indice = buscarIndiceVehiculo(vehiculos, cantidad, patente);
    liberarVehiculos(vehiculos);
    return indice >= 0;
}

bool validarDatosVehiculo(const Vehiculo* vehiculo, char* error, size_t tamError) {
    if (!vehiculo) {
        if (error && tamError > 0) snprintf(error, tamError, "Registro nulo.");
        return false;
    }
    if (!esPatenteValida(vehiculo->patente)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "La patente no tiene un formato valido. Use AAA123, AA123AA, 123ABC o A123ABC.");
        }
        return false;
    }
    if (!validarMarca(vehiculo->marca)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "La marca no tiene un formato valido. Minimo 2 caracteres con letras.");
        }
        return false;
    }
    if (!validarModelo(vehiculo->modelo)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "El modelo no tiene un formato valido. Minimo 2 caracteres con letras o digitos. Ej: Corolla, 208, C4.");
        }
        return false;
    }
    if (!validarAnioVehiculo(vehiculo->anio)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "El anio debe estar entre %d y %d.", ANIO_MIN_VEHICULO, anioMaximo());
        }
        return false;
    }
    if (!validarTipoVehiculo(vehiculo->tipo)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "El tipo debe ser Auto, Camioneta, Utilitario, Moto o 4x4.");
        }
        return false;
    }
    if (!validarCapacidadVehiculo(vehiculo->capacidad)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "La capacidad debe estar entre %d y %d personas.",
                     CAPACIDAD_MIN_VEHICULO, CAPACIDAD_MAX_VEHICULO);
        }
        return false;
    }
    if (!validarPrecioDia(vehiculo->precioDia)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "El precio por dia debe ser mayor a cero y no superar $%.0f.", PRECIO_DIA_MAXIMO);
        }
        return false;
    }
    if (!validarEstadoVehiculo(vehiculo->estado)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "El estado del vehiculo no es valido.");
        }
        return false;
    }
    return true;
}

bool vehiculoSePuedeDarDeBaja(const char* patente, char* error, size_t tamError) {
    if (vehiculoTieneAlquilerActivo(patente)) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "El vehiculo con patente %s posee un alquiler ACTIVO. "
                     "Finalicelo o cancelalo antes de pasarlo a Baja o En mantenimiento.", patente);
        }
        return false;
    }
    return true;
}

bool cambiarEstadoVehiculo(const char* patente, const char* nuevoEstado, char* error, size_t tamError) {
    Vehiculo* vehiculos = NULL;
    int cantidad = 0;
    if (!cargarVehiculos(&vehiculos, &cantidad)) {
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo cargar %s.", ARCH_VEHICULOS);
        return false;
    }
    int indice = buscarIndiceVehiculo(vehiculos, cantidad, patente);
    if (indice < 0) {
        if (error && tamError > 0) snprintf(error, tamError, "El vehiculo con patente %s no existe.", patente);
        liberarVehiculos(vehiculos);
        return false;
    }
    if (!validarEstadoVehiculo(nuevoEstado)) {
        if (error && tamError > 0) snprintf(error, tamError, "El estado de vehiculo '%s' no es valido.", nuevoEstado);
        liberarVehiculos(vehiculos);
        return false;
    }
    if (vehiculoTieneAlquilerActivo(patente)) {
        if (strcmp(nuevoEstado, EST_VEH_BAJA) == 0 || strcmp(nuevoEstado, EST_VEH_MANT) == 0) {
            bool ok = vehiculoSePuedeDarDeBaja(patente, error, tamError);
            liberarVehiculos(vehiculos);
            return ok;
        }
        if (strcmp(nuevoEstado, EST_VEH_DISPONIBLE) == 0) {
            if (error && tamError > 0) snprintf(error, tamError, "El vehiculo posee un alquiler ACTIVO y no puede pasar a Disponible.");
            liberarVehiculos(vehiculos);
            return false;
        }
    }
    copiarCadena(vehiculos[indice].estado, sizeof(vehiculos[indice].estado), nuevoEstado);
    if (!guardarVehiculos(vehiculos, cantidad)) {
        if (error && tamError > 0) snprintf(error, tamError, "No se pudo guardar %s.", ARCH_VEHICULOS);
        liberarVehiculos(vehiculos);
        return false;
    }
    liberarVehiculos(vehiculos);
    return true;
}

void registrarVehiculo(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("           ALTA DE VEHICULO\n");
    printf("========================================\n");

    Vehiculo* vehiculos = NULL;
    int cantidad = 0;
    if (!cargarVehiculos(&vehiculos, &cantidad)) {
        pausarPantalla();
        return;
    }

    Vehiculo nuevo;
    memset(&nuevo, 0, sizeof(nuevo));

    char patente[LON_PATENTE];
    char prompt[128];
    int paso = 0;
    EstadoLectura est;

    while (1) {
        switch (paso) {
            case 0: {
                est = leerPatente("Ingrese la patente (AAA123, AA123AA, 123ABC o A123ABC) (V = volver): ",
                                  patente, sizeof(patente));
                if (est == VOLVER) {
                    printf("\nRegistro cancelado.\n");
                    liberarVehiculos(vehiculos);
                    pausarPantalla();
                    return;
                }
                if (est != LEIDO) {
                    liberarVehiculos(vehiculos);
                    return;
                }
                if (buscarIndiceVehiculo(vehiculos, cantidad, patente) >= 0) {
                    printf("\n[ERROR] La patente %s ya se encuentra registrada. Reintente.\n", patente);
                    continue;
                }
                copiarCadena(nuevo.patente, sizeof(nuevo.patente), patente);
                paso++;
                break;
            }
            case 1: {
                est = pedirMarca("Ingrese la marca (V = volver): ", false, nuevo.marca, sizeof(nuevo.marca));
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarVehiculos(vehiculos);
                    return;
                }
                paso++;
                break;
            }
            case 2: {
                est = pedirModelo("Ingrese el modelo (V = volver): ", false, nuevo.modelo, sizeof(nuevo.modelo));
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarVehiculos(vehiculos);
                    return;
                }
                paso++;
                break;
            }
            case 3: {
                snprintf(prompt, sizeof(prompt), "Ingrese el anio de fabricacion (%d-%d) (V = volver): ",
                         ANIO_MIN_VEHICULO, anioMaximo());
                est = leerEntero(prompt, ANIO_MIN_VEHICULO, anioMaximo(), &nuevo.anio);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarVehiculos(vehiculos);
                    return;
                }
                paso++;
                break;
            }
            case 4: {
                est = pedirTipo("Seleccione el tipo (1-5) (V = volver): ", false, nuevo.tipo, sizeof(nuevo.tipo));
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarVehiculos(vehiculos);
                    return;
                }
                paso++;
                break;
            }
            case 5: {
                snprintf(prompt, sizeof(prompt), "Ingrese la capacidad de personas (%d-%d) (V = volver): ",
                         CAPACIDAD_MIN_VEHICULO, CAPACIDAD_MAX_VEHICULO);
                est = leerEntero(prompt, CAPACIDAD_MIN_VEHICULO, CAPACIDAD_MAX_VEHICULO, &nuevo.capacidad);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarVehiculos(vehiculos);
                    return;
                }
                paso++;
                break;
            }
            case 6: {
                est = leerDoubleEnRango("Ingrese el precio de alquiler por dia (V = volver): ",
                                        0.0, PRECIO_DIA_MAXIMO, &nuevo.precioDia);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarVehiculos(vehiculos);
                    return;
                }
                copiarCadena(nuevo.estado, sizeof(nuevo.estado), EST_VEH_DISPONIBLE);
                paso++;
                break;
            }
            case 7: {
                printf("\n---------------- RESUMEN DE DATOS ----------------\n");
                printf("Patente:        %s\n", nuevo.patente);
                printf("Marca y Modelo: %s %s\n", nuevo.marca, nuevo.modelo);
                printf("Anio:           %d\n", nuevo.anio);
                printf("Tipo:           %s\n", nuevo.tipo);
                printf("Capacidad:      %d personas\n", nuevo.capacidad);
                printf("Precio por dia: $%.2f\n", nuevo.precioDia);
                printf("Estado:         %s\n", EST_VEH_DISPONIBLE);
                printf("--------------------------------------------------\n");

                bool confirmar = false;
                est = leerSN("¿Confirma el registro del vehiculo (S/N)? (V = volver): ", &confirmar);
                if (est == VOLVER) {
                    paso = 6;
                    break;
                }
                if (est != LEIDO || !confirmar) {
                    printf("\nRegistro cancelado.\n");
                    liberarVehiculos(vehiculos);
                    pausarPantalla();
                    return;
                }

                char error[256];
                if (!validarDatosVehiculo(&nuevo, error, sizeof(error))) {
                    printf("\n[ERROR] %s\n", error);
                    liberarVehiculos(vehiculos);
                    pausarPantalla();
                    return;
                }

                Vehiculo* temp = (Vehiculo*)realloc(vehiculos, (size_t)(cantidad + 1) * sizeof(Vehiculo));
                if (!temp) {
                    printf("\n[ERROR] Memoria insuficiente para registrar vehiculo.\n");
                    liberarVehiculos(vehiculos);
                    pausarPantalla();
                    return;
                }
                vehiculos = temp;
                vehiculos[cantidad++] = nuevo;

                if (!guardarVehiculos(vehiculos, cantidad)) {
                    printf("\n[ERROR] No se pudo guardar el vehiculo en %s.\n", ARCH_VEHICULOS);
                    liberarVehiculos(vehiculos);
                    pausarPantalla();
                    return;
                }
                liberarVehiculos(vehiculos);

                printf("\n>>> Vehiculo registrado exitosamente (Estado: DISPONIBLE).\n");
                printf("Patente: %s | %s %s\n", nuevo.patente, nuevo.marca, nuevo.modelo);
                pausarPantalla();
                return;
            }
        }
    }
}

void modificarVehiculo(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("         MODIFICAR VEHICULO\n");
    printf("========================================\n");

    char patente[LON_PATENTE];
    EstadoLectura est = leerPatente("Ingrese la patente del vehiculo a modificar (V = volver): ",
                                    patente, sizeof(patente));
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        return;
    }

    Vehiculo* vehiculos = NULL;
    int cantidad = 0;
    if (!cargarVehiculos(&vehiculos, &cantidad)) {
        pausarPantalla();
        return;
    }
    int indice = buscarIndiceVehiculo(vehiculos, cantidad, patente);
    if (indice < 0) {
        printf("\n[ERROR] Vehiculo con patente %s no encontrado.\n", patente);
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }

    Vehiculo* vehiculo = &vehiculos[indice];
    bool tieneAlquilerActivo = vehiculoTieneAlquilerActivo(patente);

    printf("\nVehiculo encontrado: %s %s (Anio: %d, Estado: %s)\n",
           vehiculo->marca, vehiculo->modelo, vehiculo->anio, vehiculo->estado);
    printf("Precio por dia actual: $%.2f\n", vehiculo->precioDia);
    printf("Complete los datos nuevos o presione ENTER para mantener el valor actual.\n");
    if (tieneAlquilerActivo) {
        printf("[AVISO] El vehiculo tiene un alquiler ACTIVO. No se puede cambiar su estado ni su precio por dia.\n");
    }
    printf("------------------------------------------------------------\n");

    char prompt[128];
    char valor[LON_MODELO];

    snprintf(prompt, sizeof(prompt), "Nueva marca (actual: %s) (V = volver): ", vehiculo->marca);
    est = pedirMarca(prompt, true, valor, sizeof(valor));
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarVehiculos(vehiculos);
        return;
    }
    if (valor[0] != '\0') {
        copiarCadena(vehiculo->marca, sizeof(vehiculo->marca), valor);
    }

    snprintf(prompt, sizeof(prompt), "Nuevo modelo (actual: %s) (V = volver): ", vehiculo->modelo);
    est = pedirModelo(prompt, true, valor, sizeof(valor));
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarVehiculos(vehiculos);
        return;
    }
    if (valor[0] != '\0') {
        copiarCadena(vehiculo->modelo, sizeof(vehiculo->modelo), valor);
    }

    bool cambiarAnio = false;
    EstadoLectura estSN = leerSN("Desea cambiar el anio de fabricacion (S/N)? (V = volver): ", &cambiarAnio);
    if (estSN == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }
    if (estSN != LEIDO) {
        liberarVehiculos(vehiculos);
        return;
    }
    if (cambiarAnio) {
        int nuevoAnio = vehiculo->anio;
        snprintf(prompt, sizeof(prompt), "Nuevo anio (%d-%d) (V = volver): ", ANIO_MIN_VEHICULO, anioMaximo());
        est = leerEntero(prompt, ANIO_MIN_VEHICULO, anioMaximo(), &nuevoAnio);
        if (est == VOLVER) {
            printf("\nModificacion cancelada.\n");
            liberarVehiculos(vehiculos);
            pausarPantalla();
            return;
        }
        if (est != LEIDO) {
            liberarVehiculos(vehiculos);
            return;
        }
        vehiculo->anio = nuevoAnio;
    }

    snprintf(prompt, sizeof(prompt), "Seleccione el nuevo tipo (actual: %s) (V = volver): ", vehiculo->tipo);
    est = pedirTipo(prompt, true, valor, sizeof(valor));
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarVehiculos(vehiculos);
        return;
    }
    if (valor[0] != '\0') {
        copiarCadena(vehiculo->tipo, sizeof(vehiculo->tipo), valor);
    }

    bool cambiarCapacidad = false;
    estSN = leerSN("Desea cambiar la capacidad (S/N)? (V = volver): ", &cambiarCapacidad);
    if (estSN == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }
    if (estSN != LEIDO) {
        liberarVehiculos(vehiculos);
        return;
    }
    if (cambiarCapacidad) {
        int nuevaCapacidad = vehiculo->capacidad;
        snprintf(prompt, sizeof(prompt), "Nueva capacidad (%d-%d) (V = volver): ", CAPACIDAD_MIN_VEHICULO, CAPACIDAD_MAX_VEHICULO);
        est = leerEntero(prompt, CAPACIDAD_MIN_VEHICULO, CAPACIDAD_MAX_VEHICULO, &nuevaCapacidad);
        if (est == VOLVER) {
            printf("\nModificacion cancelada.\n");
            liberarVehiculos(vehiculos);
            pausarPantalla();
            return;
        }
        if (est != LEIDO) {
            liberarVehiculos(vehiculos);
            return;
        }
        vehiculo->capacidad = nuevaCapacidad;
    }

    if (!tieneAlquilerActivo) {
        bool cambiarPrecio = false;
        estSN = leerSN("Desea cambiar el precio por dia (S/N)? (V = volver): ", &cambiarPrecio);
        if (estSN == VOLVER) {
            printf("\nModificacion cancelada.\n");
            liberarVehiculos(vehiculos);
            pausarPantalla();
            return;
        }
        if (estSN != LEIDO) {
            liberarVehiculos(vehiculos);
            return;
        }
        if (cambiarPrecio) {
            double nuevoPrecio = vehiculo->precioDia;
            est = leerDoubleEnRango("Nuevo precio por dia (V = volver): ", 0.0, PRECIO_DIA_MAXIMO, &nuevoPrecio);
            if (est == VOLVER) {
                printf("\nModificacion cancelada.\n");
                liberarVehiculos(vehiculos);
                pausarPantalla();
                return;
            }
            if (est != LEIDO) {
                liberarVehiculos(vehiculos);
                return;
            }
            vehiculo->precioDia = nuevoPrecio;
        }
    }

    printf("\nSeleccione el nuevo estado del vehiculo:\n");
    printf("1. %s\n", EST_VEH_DISPONIBLE);
    printf("2. %s\n", EST_VEH_MANT);
    printf("3. %s\n", EST_VEH_BAJA);
    printf("0. Mantener estado actual (%s)\n", vehiculo->estado);
    int opcionEstado = 0;
    est = leerEntero("Opcion (V = volver): ", 0, 3, &opcionEstado);
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarVehiculos(vehiculos);
        return;
    }
    if (opcionEstado == 1) {
        if (tieneAlquilerActivo) {
            printf("\n[ERROR] No se puede pasar el vehiculo a Disponible mientras tenga un alquiler ACTIVO.\n");
            liberarVehiculos(vehiculos);
            pausarPantalla();
            return;
        }
        copiarCadena(vehiculo->estado, sizeof(vehiculo->estado), EST_VEH_DISPONIBLE);
    } else if (opcionEstado == 2 || opcionEstado == 3) {
        char errorBaja[256];
        if (!vehiculoSePuedeDarDeBaja(patente, errorBaja, sizeof(errorBaja))) {
            printf("\n[ERROR] %s\n", errorBaja);
            liberarVehiculos(vehiculos);
            pausarPantalla();
            return;
        }
        if (opcionEstado == 2) {
            copiarCadena(vehiculo->estado, sizeof(vehiculo->estado), EST_VEH_MANT);
        } else {
            copiarCadena(vehiculo->estado, sizeof(vehiculo->estado), EST_VEH_BAJA);
        }
    }

    char error[256];
    if (!validarDatosVehiculo(vehiculo, error, sizeof(error))) {
        printf("\n[ERROR] %s\n", error);
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }

    if (!guardarVehiculos(vehiculos, cantidad)) {
        printf("\n[ERROR] No se pudieron guardar las modificaciones del vehiculo.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }
    liberarVehiculos(vehiculos);

    printf("\n>>> Vehiculo modificado exitosamente.\n");
    pausarPantalla();
}

void buscarVehiculo(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("            BUSCAR VEHICULO\n");
    printf("========================================\n");

    char patente[LON_PATENTE];
    if (leerPatente("Ingrese la patente a buscar (V = volver): ", patente, sizeof(patente)) != LEIDO) {
        return;
    }

    Vehiculo vehiculo;
    memset(&vehiculo, 0, sizeof(vehiculo));
    if (!buscarVehiculoPorPatente(patente, &vehiculo)) {
        printf("\n[ERROR] Vehiculo con patente %s no fue encontrado.\n", patente);
        pausarPantalla();
        return;
    }

    printf("\n========================================\n");
    printf("           DATOS DEL VEHICULO\n");
    printf("========================================\n");
    printf("Patente:        %s\n", vehiculo.patente);
    printf("Marca y Modelo: %s %s\n", vehiculo.marca, vehiculo.modelo);
    printf("Anio:           %d\n", vehiculo.anio);
    printf("Tipo:           %s\n", vehiculo.tipo);
    printf("Capacidad:      %d personas\n", vehiculo.capacidad);
    printf("Precio por dia: $%.2f\n", vehiculo.precioDia);
    printf("Estado:         %s\n", vehiculo.estado);
    printf("========================================\n");
    pausarPantalla();
}

void listarVehiculos(void) {
    limpiarPantalla();
    printf("\n=====================================================================================\n");
    printf("                         LISTADO GENERAL DE VEHICULOS\n");
    printf("=====================================================================================\n");

    Vehiculo* vehiculos = NULL;
    int cantidad = 0;
    if (!cargarVehiculos(&vehiculos, &cantidad)) {
        pausarPantalla();
        return;
    }
    if (cantidad == 0) {
        printf("No hay vehiculos registrados.\n");
        liberarVehiculos(vehiculos);
        pausarPantalla();
        return;
    }

    printf("%-9s | %-12s | %-14s | %-6s | %-12s | %-5s | %-12s | ESTADO\n",
           "PATENTE", "MARCA", "MODELO", "ANIO", "TIPO", "CAP.", "PRECIO/DIA");
    printf("-------------------------------------------------------------------------------------\n");
    for (int i = 0; i < cantidad; ++i) {
        const Vehiculo* v = &vehiculos[i];
        printf("%-9s | %-12.12s | %-14.14s | %-6d | %-12.12s | %-5d | $%11.2f | %s\n",
               v->patente, v->marca, v->modelo, v->anio, v->tipo, v->capacidad,
               v->precioDia, v->estado);
    }
    printf("-------------------------------------------------------------------------------------\n");
    printf("Total de vehiculos registrados: %d\n", cantidad);

    liberarVehiculos(vehiculos);
    pausarPantalla();
}

void submenuVehiculos(void) {
    int opcion = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("           SUBMENU VEHICULOS\n");
        printf("========================================\n");
        printf("1. Registrar vehiculo\n");
        printf("2. Modificar vehiculo\n");
        printf("3. Buscar vehiculo por patente\n");
        printf("4. Listar vehiculos\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");

        if (leerEntero("Seleccione una opcion (0-4): ", 0, 4, &opcion) != LEIDO) {
            break;
        }

        switch (opcion) {
            case 1: registrarVehiculo(); break;
            case 2: modificarVehiculo(); break;
            case 3: buscarVehiculo(); break;
            case 4: listarVehiculos(); break;
            case 0: break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

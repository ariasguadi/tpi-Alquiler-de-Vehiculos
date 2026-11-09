#include "clientes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alquileres.h"
#include "archivos.h"
#include "fechas.h"
#include "movimientos.h"
#include "utils.h"
#include "vehiculos.h"

static EstadoLectura pedirNombre(const char* mensaje, bool permitirVacio, char* salida, size_t tamSalida) {
    char buffer[LON_NOMBRE];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, buffer, sizeof(buffer), permitirVacio, LON_NOMBRE - 1);
        if (est != LEIDO) {
            return est;
        }
        if (buffer[0] == '\0' && permitirVacio) {
            salida[0] = '\0';
            return LEIDO;
        }
        if (validarNombre(buffer)) {
            copiarCadena(salida, tamSalida, buffer);
            return LEIDO;
        }
        printf("[ERROR] Debe ingresar Apellido Y Nombre separados por espacio "
               "(ej: Gomez Martin). Solo letras, espacios, punto, apostrofe o guion. Reintente.\n");
    }
}

static EstadoLectura pedirTelefono(const char* mensaje, bool permitirVacio, char* salida, size_t tamSalida,
                                   int dniExcluido) {
    char buffer[LON_TEL];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, buffer, sizeof(buffer), permitirVacio, LON_TEL - 1);
        if (est != LEIDO) {
            return est;
        }
        if (buffer[0] == '\0' && permitirVacio) {
            salida[0] = '\0';
            return LEIDO;
        }
        if (!validarTelefono(buffer)) {
            printf("[ERROR] Telefono invalido. Debe tener entre 8 y 15 digitos; "
                   "puede usar espacios, '-', '(' y ')'; '+' solo al inicio. "
                   "Ej: 3764-100001. Reintente.\n");
            continue;
        }
        if (existeTelefono(buffer, dniExcluido)) {
            printf("[ERROR] El telefono %s ya esta registrado para otro cliente. Reintente.\n", buffer);
            continue;
        }
        copiarCadena(salida, tamSalida, buffer);
        return LEIDO;
    }
}

static EstadoLectura pedirEmail(const char* mensaje, bool permitirVacio, char* salida, size_t tamSalida,
                                int dniExcluido) {
    char buffer[LON_EMAIL];
    char normalizado[LON_EMAIL];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, buffer, sizeof(buffer), permitirVacio, LON_EMAIL - 1);
        if (est != LEIDO) {
            return est;
        }
        if (buffer[0] == '\0' && permitirVacio) {
            salida[0] = '\0';
            return LEIDO;
        }
        normalizarEmail(buffer, normalizado, sizeof(normalizado));
        if (!validarEmail(normalizado)) {
            printf("[ERROR] E-mail invalido. Use usuario@dominio.com, sin espacios ni '..'. Reintente.\n");
            continue;
        }
        if (existeEmail(normalizado, dniExcluido)) {
            printf("[ERROR] El e-mail %s ya esta registrado para otro cliente. Reintente.\n", normalizado);
            continue;
        }
        copiarCadena(salida, tamSalida, normalizado);
        return LEIDO;
    }
}

static EstadoLectura pedirDireccion(const char* mensaje, bool permitirVacio, char* salida, size_t tamSalida) {
    char buffer[LON_DIR];
    while (1) {
        EstadoLectura est = leerTexto(mensaje, buffer, sizeof(buffer), permitirVacio, LON_DIR - 1);
        if (est != LEIDO) {
            return est;
        }
        if (buffer[0] == '\0' && permitirVacio) {
            salida[0] = '\0';
            return LEIDO;
        }
        if (validarDireccion(buffer)) {
            copiarCadena(salida, tamSalida, buffer);
            return LEIDO;
        }
        printf("[ERROR] Direccion invalida. Minimo 5 caracteres y al menos una letra. Ej: Av. Mitre 123. Reintente.\n");
    }
}

static EstadoLectura pedirFechaAlta(int* fecha) {
    return leerFechaEnRango("Ingrese fecha de alta (AAAAMMDD, hoy o anterior) (V = volver): ",
                            19000101, fechaActual(), fecha);
}

bool cargarClientes(Cliente** clientes, int* cantidad) {
    char error[256];
    if (!cargarRegistrosGenerico(ARCH_CLIENTES, sizeof(Cliente), (void**)clientes, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    if (*clientes && *cantidad > 0) {
        for (int i = 0; i < *cantidad; ++i) {
            (*clientes)[i].apellidoNombre[LON_NOMBRE - 1] = '\0';
            (*clientes)[i].telefono[LON_TEL - 1] = '\0';
            (*clientes)[i].email[LON_EMAIL - 1] = '\0';
            (*clientes)[i].direccion[LON_DIR - 1] = '\0';
        }
    }
    return true;
}

bool guardarClientes(const Cliente* clientes, int cantidad) {
    char error[256];
    if (!guardarRegistrosGenerico(ARCH_CLIENTES, sizeof(Cliente), clientes, cantidad, error, sizeof(error))) {
        printf("[ERROR] %s\n", error);
        return false;
    }
    return true;
}

void liberarClientes(Cliente* clientes) {
    if (clientes) {
        free(clientes);
    }
}

int buscarIndiceCliente(const Cliente* clientes, int cantidad, int dni) {
    if (!clientes) return -1;
    for (int i = 0; i < cantidad; ++i) {
        if (clientes[i].dni == dni) {
            return i;
        }
    }
    return -1;
}

bool buscarClientePorDNI(int dni, Cliente* cliente) {
    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        return false;
    }
    int indice = buscarIndiceCliente(clientes, cantidad, dni);
    if (indice < 0) {
        liberarClientes(clientes);
        return false;
    }
    if (cliente) {
        *cliente = clientes[indice];
    }
    liberarClientes(clientes);
    return true;
}

bool existeCliente(int dni) {
    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        return false;
    }
    int indice = buscarIndiceCliente(clientes, cantidad, dni);
    liberarClientes(clientes);
    return indice >= 0;
}

bool existeEmail(const char* email, int dniExcluido) {
    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        return false;
    }
    char buscado[LON_EMAIL];
    normalizarEmail(email, buscado, sizeof(buscado));
    for (int i = 0; i < cantidad; ++i) {
        if (clientes[i].dni == dniExcluido) {
            continue;
        }
        char actual[LON_EMAIL];
        normalizarEmail(clientes[i].email, actual, sizeof(actual));
        if (strcmp(buscado, actual) == 0) {
            liberarClientes(clientes);
            return true;
        }
    }
    liberarClientes(clientes);
    return false;
}

bool existeTelefono(const char* telefono, int dniExcluido) {
    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        return false;
    }
    for (int i = 0; i < cantidad; ++i) {
        if (clientes[i].dni == dniExcluido) {
            continue;
        }
        if (strcmp(telefono, clientes[i].telefono) == 0) {
            liberarClientes(clientes);
            return true;
        }
    }
    liberarClientes(clientes);
    return false;
}

bool clienteSePuedeDarDeBaja(int dni, char* error, size_t tamError) {
    int numAlquiler = clienteTieneAlquilerActivo(dni);
    if (numAlquiler > 0) {
        if (error && tamError > 0) {
            snprintf(error, tamError,
                     "El cliente tiene el alquiler #%d activo. Debe finalizarlo o cancelarlo antes de darlo de baja.",
                     numAlquiler);
        }
        return false;
    }
    return true;
}

bool validarDatosCliente(const Cliente* cliente, char* error, size_t tamError) {
    if (!cliente) {
        if (error && tamError > 0) snprintf(error, tamError, "Registro nulo.");
        return false;
    }
    if (cliente->dni < DNI_MIN || cliente->dni > DNI_MAX) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "El DNI debe estar entre 1.000.000 y 50.000.000.");
        }
        return false;
    }
    if (!validarNombre(cliente->apellidoNombre)) {
        if (error && tamError > 0) snprintf(error, tamError, "Nombre del cliente invalido.");
        return false;
    }
    if (!validarTelefono(cliente->telefono)) {
        if (error && tamError > 0) snprintf(error, tamError, "Telefono del cliente invalido.");
        return false;
    }
    char normEmail[LON_EMAIL];
    normalizarEmail(cliente->email, normEmail, sizeof(normEmail));
    if (!validarEmail(normEmail)) {
        if (error && tamError > 0) snprintf(error, tamError, "E-mail del cliente invalido.");
        return false;
    }
    if (!validarDireccion(cliente->direccion)) {
        if (error && tamError > 0) snprintf(error, tamError, "Direccion del cliente invalida.");
        return false;
    }
    if (!esFechaPasadaOHoy(cliente->fechaAlta)) {
        if (error && tamError > 0) snprintf(error, tamError, "La fecha de alta debe ser una fecha valida igual o anterior a hoy.");
        return false;
    }
    if (cliente->estado != CLI_ACTIVO && cliente->estado != CLI_INACTIVO) {
        if (error && tamError > 0) snprintf(error, tamError, "Estado de cliente invalido.");
        return false;
    }
    return true;
}

void registrarCliente(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("           ALTA DE CLIENTE\n");
    printf("========================================\n");

    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        pausarPantalla();
        return;
    }

    Cliente nuevo;
    memset(&nuevo, 0, sizeof(nuevo));

    int paso = 0;
    EstadoLectura est;

    while (1) {
        switch (paso) {
            case 0: {
                est = leerDNI("Ingrese el DNI (entre 1.000.000 y 50.000.000) (V = volver): ", &nuevo.dni);
                if (est == VOLVER) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    pausarPantalla();
                    return;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    return;
                }
                if (buscarIndiceCliente(clientes, cantidad, nuevo.dni) >= 0) {
                    printf("\n[ERROR] El DNI %d ya se encuentra registrado en el sistema. Reintente.\n", nuevo.dni);
                    continue;
                }
                paso++;
                break;
            }
            case 1: {
                est = pedirNombre("Ingrese Apellido y Nombre (V = volver): ", false,
                                  nuevo.apellidoNombre, sizeof(nuevo.apellidoNombre));
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    return;
                }
                paso++;
                break;
            }
            case 2: {
                est = pedirTelefono("Ingrese Telefono (V = volver): ", false,
                                    nuevo.telefono, sizeof(nuevo.telefono), 0);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    return;
                }
                paso++;
                break;
            }
            case 3: {
                est = pedirEmail("Ingrese E-mail (V = volver): ", false,
                                 nuevo.email, sizeof(nuevo.email), 0);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    return;
                }
                paso++;
                break;
            }
            case 4: {
                est = pedirDireccion("Ingrese Direccion (V = volver): ", false,
                                     nuevo.direccion, sizeof(nuevo.direccion));
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    return;
                }
                paso++;
                break;
            }
            case 5: {
                est = pedirFechaAlta(&nuevo.fechaAlta);
                if (est == VOLVER) {
                    paso--;
                    break;
                }
                if (est != LEIDO) {
                    liberarClientes(clientes);
                    return;
                }
                nuevo.estado = CLI_ACTIVO;
                paso++;
                break;
            }
            case 6: {
                printf("\n---------------- RESUMEN DE DATOS ----------------\n");
                printf("DNI:            %d\n", nuevo.dni);
                printf("Nombre:         %s\n", nuevo.apellidoNombre);
                printf("Telefono:       %s\n", nuevo.telefono);
                printf("E-mail:         %s\n", nuevo.email);
                printf("Direccion:      %s\n", nuevo.direccion);
                printf("Fecha de Alta:  %s\n", fechaALarga(nuevo.fechaAlta));
                printf("Estado:         ACTIVO\n");
                printf("--------------------------------------------------\n");

                bool confirmar = false;
                est = leerSN("¿Confirma el registro del cliente (S/N)? (V = volver): ", &confirmar);
                if (est == VOLVER) {
                    paso = 5;
                    break;
                }
                if (est != LEIDO || !confirmar) {
                    printf("\nRegistro cancelado.\n");
                    liberarClientes(clientes);
                    pausarPantalla();
                    return;
                }

                char error[256];
                if (!validarDatosCliente(&nuevo, error, sizeof(error))) {
                    printf("\n[ERROR] %s\n", error);
                    liberarClientes(clientes);
                    pausarPantalla();
                    return;
                }

                Cliente* temp = (Cliente*)realloc(clientes, (size_t)(cantidad + 1) * sizeof(Cliente));
                if (!temp) {
                    printf("\n[ERROR] Memoria insuficiente para registrar cliente.\n");
                    liberarClientes(clientes);
                    pausarPantalla();
                    return;
                }
                clientes = temp;
                clientes[cantidad++] = nuevo;

                if (!guardarClientes(clientes, cantidad)) {
                    printf("\n[ERROR] No se pudo guardar el cliente en %s.\n", ARCH_CLIENTES);
                    liberarClientes(clientes);
                    pausarPantalla();
                    return;
                }
                liberarClientes(clientes);

                char errorTexto[256];
                if (!regenerarArchivoMovimientosCliente(nuevo.dni, errorTexto, sizeof(errorTexto))) {
                    printf("[AVISO] El cliente se guardo, pero no se pudo crear su archivo de movimientos: %s\n", errorTexto);
                }

                printf("\n>>> Cliente registrado exitosamente (Estado: ACTIVO).\n");
                printf("DNI: %d | Nombre: %s\n", nuevo.dni, nuevo.apellidoNombre);
                pausarPantalla();
                return;
            }
        }
    }
}

void modificarCliente(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("         MODIFICAR CLIENTE\n");
    printf("========================================\n");

    int dni = 0;
    EstadoLectura est = leerDNI("Ingrese el DNI del cliente a modificar (V = volver): ", &dni);
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        return;
    }

    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        pausarPantalla();
        return;
    }
    int indice = buscarIndiceCliente(clientes, cantidad, dni);
    if (indice < 0) {
        printf("\n[ERROR] Cliente con DNI %d no fue encontrado.\n", dni);
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }

    Cliente* cliente = &clientes[indice];
    printf("\nCliente encontrado: %s\n", cliente->apellidoNombre);
    printf("Estado actual: %s\n", cliente->estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO");
    printf("Complete los datos nuevos o presione ENTER para mantener el valor actual.\n");
    printf("------------------------------------------------------------\n");

    char prompt[128];
    char valor[LON_DIR];

    snprintf(prompt, sizeof(prompt), "Nuevo Apellido y Nombre (actual: %s) (V = volver): ", cliente->apellidoNombre);
    est = pedirNombre(prompt, true, valor, sizeof(valor));
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarClientes(clientes);
        return;
    }
    if (valor[0] != '\0') {
        copiarCadena(cliente->apellidoNombre, sizeof(cliente->apellidoNombre), valor);
    }

    snprintf(prompt, sizeof(prompt), "Nuevo Telefono (actual: %s) (V = volver): ", cliente->telefono);
    est = pedirTelefono(prompt, true, valor, sizeof(valor), cliente->dni);
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarClientes(clientes);
        return;
    }
    if (valor[0] != '\0') {
        copiarCadena(cliente->telefono, sizeof(cliente->telefono), valor);
    }

    snprintf(prompt, sizeof(prompt), "Nuevo E-mail (actual: %s) (V = volver): ", cliente->email);
    est = pedirEmail(prompt, true, valor, sizeof(valor), cliente->dni);
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarClientes(clientes);
        return;
    }
    if (valor[0] != '\0') {
        copiarCadena(cliente->email, sizeof(cliente->email), valor);
    }

    snprintf(prompt, sizeof(prompt), "Nueva Direccion (actual: %s) (V = volver): ", cliente->direccion);
    est = pedirDireccion(prompt, true, valor, sizeof(valor));
    if (est == VOLVER) {
        printf("\nModificacion cancelada.\n");
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }
    if (est != LEIDO) {
        liberarClientes(clientes);
        return;
    }
    if (valor[0] != '\0') {
        copiarCadena(cliente->direccion, sizeof(cliente->direccion), valor);
    }

    char error[256];
    if (!validarDatosCliente(cliente, error, sizeof(error))) {
        printf("\n[ERROR] %s\n", error);
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }

    if (!guardarClientes(clientes, cantidad)) {
        printf("\n[ERROR] No se pudieron guardar los cambios en %s.\n", ARCH_CLIENTES);
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }
    liberarClientes(clientes);

    printf("\n>>> Datos del cliente actualizados correctamente.\n");
    pausarPantalla();
}

void consultarCliente(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("         CONSULTA DE CLIENTE\n");
    printf("========================================\n");

    int dni = 0;
    if (leerDNI("Ingrese el DNI a consultar (V = volver): ", &dni) != LEIDO) {
        return;
    }

    Cliente* clientes = NULL;
    int cantClientes = 0;
    if (!cargarClientes(&clientes, &cantClientes)) {
        pausarPantalla();
        return;
    }
    int indice = buscarIndiceCliente(clientes, cantClientes, dni);
    if (indice < 0) {
        printf("\n[ERROR] Cliente con DNI %d no existe.\n", dni);
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }

    const Cliente* cliente = &clientes[indice];
    printf("\n---------------- DATOS DEL CLIENTE ----------------\n");
    printf("DNI:            %d\n", cliente->dni);
    printf("Nombre:         %s\n", cliente->apellidoNombre);
    printf("Telefono:       %s\n", cliente->telefono);
    printf("E-mail:         %s\n", cliente->email);
    printf("Direccion:      %s\n", cliente->direccion);
    printf("Fecha de Alta:  %s\n", fechaALarga(cliente->fechaAlta));
    printf("Estado:         %s\n", cliente->estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO");

    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    cargarAlquileres(&alquileres, &cantAlquileres);
    cargarVehiculos(&vehiculos, &cantVehiculos);

    printf("\n--- HISTORIAL DE ALQUILERES ---\n");
    int countAlq = 0;
    for (int i = 0; i < cantAlquileres; ++i) {
        if (alquileres[i].dni == dni) {
            if (countAlq == 0) {
                imprimirEncabezadoAlquileres();
            }
            imprimirFilaAlquiler(&alquileres[i], clientes, cantClientes, vehiculos, cantVehiculos);
            countAlq++;
        }
    }
    if (countAlq == 0) {
        printf("Sin alquileres registrados para este cliente.\n");
    }

    liberarAlquileres(alquileres);
    liberarVehiculos(vehiculos);
    liberarClientes(clientes);

    const char* ruta = nombreArchivoMovimientos(dni);
    FILE* archivo = fopen(ruta, "r");
    printf("\n--- HISTORIAL DE MOVIMIENTOS (%s) ---\n", ruta);
    if (!archivo) {
        printf("[AVISO] No se encontro el archivo de movimientos del cliente.\n");
    } else {
        printf("%-6s | %-6s | %-8s | %-12s | %-10s | %-10s | MEDIO\n",
               "NRO", "ALQ", "TIPO", "MONTO", "FECHA", "PATENTE");
        printf("-----------------------------------------------------------------------------\n");
        char linea[256];
        int countMov = 0;
        while (fgets(linea, sizeof(linea), archivo)) {
            recortar(linea);
            if (linea[0] == '\0') continue;
            char* campos[10];
            int nCampos = dividir(linea, ';', campos, 10);
            if (nCampos < 8) continue;
            double monto = strtod(campos[3], NULL);
            printf("%-6s | %-6s | %-8s | $%11.2f | %-10s | %-10s | %s\n",
                   campos[0], campos[1], campos[2], monto, campos[4], campos[6], campos[7]);
            countMov++;
        }
        if (countMov == 0) {
            printf("Sin movimientos registrados.\n");
        }
        fclose(archivo);
    }

    pausarPantalla();
}

void listarClientes(void) {
    limpiarPantalla();
    printf("\n=================================================================================================\n");
    printf("                                      LISTADO DE CLIENTES\n");
    printf("=================================================================================================\n");

    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        pausarPantalla();
        return;
    }

    if (cantidad == 0) {
        printf("\nNo hay clientes registrados en el sistema.\n");
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }

    printf("%-10s | %-24s | %-15s | %-25s | %-10s | %s\n",
           "DNI", "APELLIDO Y NOMBRE", "TELEFONO", "E-MAIL", "FECHA ALTA", "ESTADO");
    printf("-------------------------------------------------------------------------------------------------\n");

    int activos = 0;
    for (int i = 0; i < cantidad; ++i) {
        const Cliente* c = &clientes[i];
        if (c->estado == CLI_ACTIVO) {
            activos++;
        }
        printf("%-10d | %-24.24s | %-15.15s | %-25.25s | %-10s | %s\n",
               c->dni, c->apellidoNombre, c->telefono, c->email,
               fechaALarga(c->fechaAlta),
               c->estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO");
    }

    printf("-------------------------------------------------------------------------------------------------\n");
    printf("Total de clientes: %d  (Activos: %d | Inactivos: %d)\n",
           cantidad, activos, cantidad - activos);

    liberarClientes(clientes);
    pausarPantalla();
}

void darDeBajaCliente(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("     BAJA / REACTIVACION DE CLIENTE\n");
    printf("========================================\n");

    int dni = 0;
    if (leerDNI("Ingrese el DNI del cliente (V = volver): ", &dni) != LEIDO) {
        return;
    }

    Cliente* clientes = NULL;
    int cantidad = 0;
    if (!cargarClientes(&clientes, &cantidad)) {
        pausarPantalla();
        return;
    }

    int indice = buscarIndiceCliente(clientes, cantidad, dni);
    if (indice < 0) {
        printf("\n[ERROR] Cliente con DNI %d no existe.\n", dni);
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }

    Cliente* cliente = &clientes[indice];
    bool estabaActivo = (cliente->estado == CLI_ACTIVO);

    printf("\nCliente: %s\n", cliente->apellidoNombre);
    printf("Estado actual: %s\n", estabaActivo ? "ACTIVO" : "INACTIVO");

    if (estabaActivo) {
        char error[256];
        if (!clienteSePuedeDarDeBaja(dni, error, sizeof(error))) {
            printf("\n[ERROR] %s\n", error);
            liberarClientes(clientes);
            pausarPantalla();
            return;
        }
        printf("\nADVERTENCIA: El cliente pasara a estado INACTIVO.\n");
    } else {
        printf("\nEl cliente pasara a estado ACTIVO.\n");
    }

    bool confirmar = false;
    if (leerSN("Confirma la operacion (S/N)? (V = volver): ", &confirmar) != LEIDO || !confirmar) {
        printf("\nOperacion cancelada por el usuario.\n");
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }

    cliente->estado = estabaActivo ? CLI_INACTIVO : CLI_ACTIVO;
    if (!guardarClientes(clientes, cantidad)) {
        printf("\n[ERROR] No se pudo actualizar el estado del cliente.\n");
        liberarClientes(clientes);
        pausarPantalla();
        return;
    }

    printf("\n>>> Cliente %s exitosamente.\n", estabaActivo ? "dado de baja" : "reactivado");
    liberarClientes(clientes);
    pausarPantalla();
}

void submenuClientes(void) {
    int opcion = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("           SUBMENU CLIENTES\n");
        printf("========================================\n");
        printf("1. Registrar cliente\n");
        printf("2. Modificar cliente\n");
        printf("3. Consultar cliente (con historial)\n");
        printf("4. Listar clientes\n");
        printf("5. Dar de baja / reactivar cliente\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");

        if (leerEntero("Seleccione una opcion (0-5): ", 0, 5, &opcion) != LEIDO) {
            break;
        }

        switch (opcion) {
            case 1: registrarCliente(); break;
            case 2: modificarCliente(); break;
            case 3: consultarCliente(); break;
            case 4: listarClientes(); break;
            case 5: darDeBajaCliente(); break;
            case 0: break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

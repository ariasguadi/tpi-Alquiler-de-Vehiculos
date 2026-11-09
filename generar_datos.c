#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "alquileres.h"
#include "archivos.h"
#include "clientes.h"
#include "comun.h"
#include "fechas.h"
#include "movimientos.h"
#include "utils.h"
#include "vehiculos.h"

static Cliente crearCliente(int dni, const char* nombre, const char* telefono,
                            const char* email, const char* direccion,
                            int fechaAlta, int estado) {
    Cliente c;
    memset(&c, 0, sizeof(c));
    c.dni = dni;
    copiarCadena(c.apellidoNombre, sizeof(c.apellidoNombre), nombre);
    copiarCadena(c.telefono, sizeof(c.telefono), telefono);
    copiarCadena(c.email, sizeof(c.email), email);
    copiarCadena(c.direccion, sizeof(c.direccion), direccion);
    c.fechaAlta = fechaAlta;
    c.estado = estado;
    return c;
}

static Vehiculo crearVehiculo(const char* patente, const char* marca,
                              const char* modelo, int anio, const char* tipo,
                              int capacidad, double precioDia, const char* estado) {
    Vehiculo v;
    memset(&v, 0, sizeof(v));
    copiarCadena(v.patente, sizeof(v.patente), patente);
    copiarCadena(v.marca, sizeof(v.marca), marca);
    copiarCadena(v.modelo, sizeof(v.modelo), modelo);
    v.anio = anio;
    copiarCadena(v.tipo, sizeof(v.tipo), tipo);
    v.capacidad = capacidad;
    v.precioDia = precioDia;
    copiarCadena(v.estado, sizeof(v.estado), estado);
    return v;
}

static Alquiler crearAlquiler(int numero, int dni, const char* patente, int inicio,
                             int prevista, int efectiva, int dias, double total, double pagado,
                             const char* estado) {
    Alquiler a;
    memset(&a, 0, sizeof(a));
    a.numAlquiler = numero;
    a.dni = dni;
    copiarCadena(a.patente, sizeof(a.patente), patente);
    a.fechaInicio = inicio;
    a.fechaPrevista = prevista;
    a.fechaEfectiva = efectiva;
    a.cantDias = dias;
    a.importeTotal = total;
    a.importePagado = pagado;
    copiarCadena(a.estado, sizeof(a.estado), estado);
    return a;
}

static Movimiento crearMovimiento(int numero, int alquiler, const char* tipo, double monto,
                                 int fecha, const char* medio) {
    Movimiento m;
    memset(&m, 0, sizeof(m));
    m.numMovimiento = numero;
    m.numAlquiler = alquiler;
    copiarCadena(m.tipoMov, sizeof(m.tipoMov), tipo);
    m.monto = monto;
    m.fecha = fecha;
    copiarCadena(m.medioPago, sizeof(m.medioPago), medio);
    return m;
}

static void completarMovimientos(Movimiento* movimientos, int cantMovimientos,
                                const Alquiler* alquileres, int cantAlquileres) {
    for (int i = 0; i < cantMovimientos; ++i) {
        for (int j = 0; j < cantAlquileres; ++j) {
            if (alquileres[j].numAlquiler == movimientos[i].numAlquiler) {
                movimientos[i].dni = alquileres[j].dni;
                copiarCadena(movimientos[i].patente, sizeof(movimientos[i].patente), alquileres[j].patente);
                break;
            }
        }
    }
}

static bool verificarDataset(void) {
    Cliente* clientes = NULL;
    int cantClientes = 0;
    Vehiculo* vehiculos = NULL;
    int cantVehiculos = 0;
    Alquiler* alquileres = NULL;
    int cantAlquileres = 0;
    Movimiento* movimientos = NULL;
    int cantMovimientos = 0;

    if (!cargarClientes(&clientes, &cantClientes) ||
        !cargarVehiculos(&vehiculos, &cantVehiculos) ||
        !cargarAlquileres(&alquileres, &cantAlquileres) ||
        !cargarMovimientos(&movimientos, &cantMovimientos)) {
        liberarClientes(clientes);
        liberarVehiculos(vehiculos);
        liberarAlquileres(alquileres);
        liberarMovimientos(movimientos);
        return false;
    }

    bool ok = true;
    if (cantClientes != 10) {
        printf("[FALLO] Se esperaban 10 clientes y hay %d.\n", cantClientes);
        ok = false;
    }
    if (cantVehiculos != 10) {
        printf("[FALLO] Se esperaban 10 vehiculos y hay %d.\n", cantVehiculos);
        ok = false;
    }
    if (cantAlquileres != 15) {
        printf("[FALLO] Se esperaban 15 alquileres y hay %d.\n", cantAlquileres);
        ok = false;
    }
    if (cantMovimientos != 23) {
        printf("[FALLO] Se esperaban 23 movimientos y hay %d.\n", cantMovimientos);
        ok = false;
    }

    for (int i = 0; i < cantClientes; ++i) {
        if (clientes[i].dni < DNI_MIN || clientes[i].dni > DNI_MAX) {
            printf("[FALLO] Cliente DNI %d fuera de rango legal [%d, %d].\n",
                   clientes[i].dni, DNI_MIN, DNI_MAX);
            ok = false;
        }
        char errorCliente[256];
        if (!validarDatosCliente(&clientes[i], errorCliente, sizeof(errorCliente))) {
            printf("[FALLO] Cliente DNI %d: %s\n", clientes[i].dni, errorCliente);
            ok = false;
        }
        for (int j = i + 1; j < cantClientes; ++j) {
            char emailI[LON_EMAIL], emailJ[LON_EMAIL];
            normalizarEmail(clientes[i].email, emailI, sizeof(emailI));
            normalizarEmail(clientes[j].email, emailJ, sizeof(emailJ));
            if (strcmp(emailI, emailJ) == 0) {
                printf("[FALLO] E-mail duplicado entre los DNI %d y %d (%s).\n",
                       clientes[i].dni, clientes[j].dni, emailI);
                ok = false;
            }
        }
    }

    for (int i = 0; i < cantVehiculos; ++i) {
        char errorVehiculo[256];
        if (!validarDatosVehiculo(&vehiculos[i], errorVehiculo, sizeof(errorVehiculo))) {
            printf("[FALLO] Vehiculo %s: %s\n", vehiculos[i].patente, errorVehiculo);
            ok = false;
        }
    }

    for (int i = 0; i < cantAlquileres; ++i) {
        char errorAlquiler[256];
        if (!validarDatosAlquiler(&alquileres[i], errorAlquiler, sizeof(errorAlquiler))) {
            printf("[FALLO] Alquiler Nro %d: %s\n", alquileres[i].numAlquiler, errorAlquiler);
            ok = false;
        }
        for (int j = i + 1; j < cantAlquileres; ++j) {
            if (alquileres[i].numAlquiler == alquileres[j].numAlquiler) {
                printf("[FALLO] Numero de alquiler duplicado: %d.\n", alquileres[i].numAlquiler);
                ok = false;
            }
        }
    }

    for (int i = 0; i < cantClientes; ++i) {
        int activosCliente = 0;
        for (int j = 0; j < cantAlquileres; ++j) {
            if (alquileres[j].dni == clientes[i].dni && strcmp(alquileres[j].estado, EST_ALQ_ACTIVO) == 0) {
                ++activosCliente;
            }
        }
        if (activosCliente > 1) {
            printf("[FALLO] El cliente DNI %d tiene %d alquileres ACTIVOS.\n",
                   clientes[i].dni, activosCliente);
            ok = false;
        }
        if (activosCliente > 0 && clientes[i].estado != CLI_ACTIVO) {
            printf("[FALLO] El cliente DNI %d esta INACTIVO pero tiene un alquiler ACTIVO.\n",
                   clientes[i].dni);
            ok = false;
        }
    }

    for (int i = 0; i < cantVehiculos; ++i) {
        char patente[LON_PATENTE];
        copiarCadena(patente, sizeof(patente), vehiculos[i].patente);
        bool tieneActivo = false;
        for (int j = 0; j < cantAlquileres; ++j) {
            char normA[LON_PATENTE], normV[LON_PATENTE];
            normalizarPatente(alquileres[j].patente, normA, sizeof(normA));
            normalizarPatente(patente, normV, sizeof(normV));
            if (strcmp(normA, normV) == 0 && strcmp(alquileres[j].estado, EST_ALQ_ACTIVO) == 0) {
                tieneActivo = true;
                break;
            }
        }
        if (tieneActivo && strcmp(vehiculos[i].estado, EST_VEH_ALQUILADO) != 0) {
            printf("[FALLO] El vehiculo %s tiene alquiler ACTIVO pero su estado es %s.\n",
                   patente, vehiculos[i].estado);
            ok = false;
        }
        if (!tieneActivo && strcmp(vehiculos[i].estado, EST_VEH_ALQUILADO) == 0) {
            printf("[FALLO] El vehiculo %s figura Alquilado pero no tiene alquiler ACTIVO.\n", patente);
            ok = false;
        }
    }

    for (int i = 0; i < cantMovimientos; ++i) {
        const Movimiento* m = &movimientos[i];
        int indiceAlquiler = buscarIndiceAlquiler(alquileres, cantAlquileres, m->numAlquiler);
        if (indiceAlquiler < 0) {
            printf("[FALLO] El movimiento Nro %d referencia el alquiler inexistente %d.\n",
                   m->numMovimiento, m->numAlquiler);
            ok = false;
            continue;
        }
        const Alquiler* a = &alquileres[indiceAlquiler];
        if (strcmp(m->tipoMov, MOV_PAGO) == 0) {
            if (m->fecha < a->fechaInicio) {
                printf("[FALLO] El pago Nro %d es anterior al inicio del alquiler %d.\n",
                       m->numMovimiento, a->numAlquiler);
                ok = false;
            }
            if (!esFechaPasadaOHoy(m->fecha)) {
                printf("[FALLO] El pago Nro %d tiene fecha futura.\n", m->numMovimiento);
                ok = false;
            }
        }
        if (m->dni != a->dni) {
            printf("[FALLO] El movimiento Nro %d tiene un DNI distinto al del alquiler.\n", m->numMovimiento);
            ok = false;
        }
        char normM[LON_PATENTE], normA[LON_PATENTE];
        normalizarPatente(m->patente, normM, sizeof(normM));
        normalizarPatente(a->patente, normA, sizeof(normA));
        if (strcmp(normM, normA) != 0) {
            printf("[FALLO] El movimiento Nro %d tiene una patente distinta a la del alquiler.\n", m->numMovimiento);
            ok = false;
        }
    }

    int disponibles = 0, alquilados = 0, mantenimiento = 0, baja = 0;
    for (int i = 0; i < cantVehiculos; ++i) {
        if (strcmp(vehiculos[i].estado, EST_VEH_DISPONIBLE) == 0) disponibles++;
        else if (strcmp(vehiculos[i].estado, EST_VEH_ALQUILADO) == 0) alquilados++;
        else if (strcmp(vehiculos[i].estado, EST_VEH_MANT) == 0) mantenimiento++;
        else if (strcmp(vehiculos[i].estado, EST_VEH_BAJA) == 0) baja++;
    }
    if (disponibles != 5 || alquilados != 3 || mantenimiento != 1 || baja != 1) {
        printf("[FALLO] Distribucion de estados de vehiculos incorrecta: %d disponibles, %d alquilados, %d mantenimiento, %d baja.\n",
               disponibles, alquilados, mantenimiento, baja);
        ok = false;
    }

    int activos = 0, finalizados = 0, cancelados = 0, pagos = 0, ajustes = 0;
    double deuda = 0.0, cobrado = 0.0;
    for (int i = 0; i < cantAlquileres; ++i) {
        const Alquiler* a = &alquileres[i];
        if (strcmp(a->estado, EST_ALQ_ACTIVO) == 0) activos++;
        else if (strcmp(a->estado, EST_ALQ_FINALIZADO) == 0) finalizados++;
        else if (strcmp(a->estado, EST_ALQ_CANCELADO) == 0) cancelados++;
        cobrado += a->importePagado;
        if (strcmp(a->estado, EST_ALQ_CANCELADO) != 0) {
            double saldo = a->importeTotal - a->importePagado;
            if (saldo > TOLERANCIA_SALDO) {
                deuda += saldo;
            }
        }
    }
    for (int i = 0; i < cantMovimientos; ++i) {
        if (strcmp(movimientos[i].tipoMov, MOV_PAGO) == 0) pagos++;
        else if (strcmp(movimientos[i].tipoMov, MOV_AJUSTE) == 0) ajustes++;
    }
    if (activos != 3 || finalizados != 9 || cancelados != 3) {
        printf("[FALLO] Distribucion de estados de alquileres incorrecta: %d activos, %d finalizados, %d cancelados.\n",
               activos, finalizados, cancelados);
        ok = false;
    }
    if (pagos != 18 || ajustes != 5) {
        printf("[FALLO] Se esperaban 18 pagos y 5 ajustes; hay %d y %d.\n", pagos, ajustes);
        ok = false;
    }
    if (fabs(deuda - 1000000.0) > 0.01) {
        printf("[FALLO] La deuda total esperada era $1000000.00 y es $%.2f.\n", deuda);
        ok = false;
    }
    if (fabs(cobrado - 2581000.0) > 0.01) {
        printf("[FALLO] El total cobrado esperado era $2581000.00 y es $%.2f.\n", cobrado);
        ok = false;
    }

    for (int i = 0; i < cantClientes; ++i) {
        const char* ruta = nombreArchivoMovimientos(clientes[i].dni);
        if (!existeArchivo(ruta)) {
            printf("[FALLO] No existe el archivo de texto %s.\n", ruta);
            ok = false;
        }
    }

    if (ok) {
        printf("[OK] Verificacion del dataset: 10 clientes, 10 vehiculos, 15 alquileres, 23 movimientos.\n");
        printf("[OK] Estados: 5 disponibles, 3 alquilados, 1 mantenimiento, 1 baja.\n");
        printf("[OK] Alquileres: 3 activos, 9 finalizados, 3 cancelados.\n");
        printf("[OK] Deuda total: $1000000.00 | Total cobrado: $2581000.00.\n");
        printf("[OK] Archivos de texto por cliente generados.\n");
    }

    liberarClientes(clientes);
    liberarVehiculos(vehiculos);
    liberarAlquileres(alquileres);
    liberarMovimientos(movimientos);
    return ok;
}

int main(void) {
    Cliente clientes[10];
    clientes[0] = crearCliente(30111222, "Gomez Martin", "3764-100001", "mgomez@mail.com", "Av. Mitre 123", 20240110, CLI_ACTIVO);
    clientes[1] = crearCliente(31222333, "Fernandez Lucia", "3764-100002", "lfernandez@mail.com", "Colon 456", 20240215, CLI_ACTIVO);
    clientes[2] = crearCliente(32333444, "Rodriguez Pablo", "3764-100003", "prodriguez@mail.com", "San Martin 789", 20240320, CLI_ACTIVO);
    clientes[3] = crearCliente(33444555, "Lopez Carolina", "3764-100004", "clopez@mail.com", "Sarmiento 321", 20240505, CLI_ACTIVO);
    clientes[4] = crearCliente(34555666, "Martinez Diego", "3764-100005", "dmartinez@mail.com", "Belgrano 654", 20240612, CLI_ACTIVO);
    clientes[5] = crearCliente(35666777, "Sosa Valeria", "3764-100006", "vsosa@mail.com", "Rivadavia 987", 20240801, CLI_ACTIVO);
    clientes[6] = crearCliente(36777888, "Benitez Javier", "3764-100007", "jbenitez@mail.com", "Junin 159", 20240910, CLI_ACTIVO);
    clientes[7] = crearCliente(37888999, "Acosta Romina", "3764-100008", "racosta@mail.com", "Cordoba 753", 20241120, CLI_ACTIVO);
    clientes[8] = crearCliente(38999000, "Duarte Nicolas", "3764-100009", "nduarte@mail.com", "Entre Rios 852", 20250118, CLI_ACTIVO);
    clientes[9] = crearCliente(39000111, "Romero Agustina", "3764-100010", "aromero@mail.com", "Salta 951", 20250302, CLI_INACTIVO);

    Vehiculo vehiculos[10];
    vehiculos[0] = crearVehiculo("ABC123", "Toyota", "Corolla", 2022, "Auto", 5, 45000.0, EST_VEH_DISPONIBLE);
    vehiculos[1] = crearVehiculo("AB123CD", "Ford", "Ranger", 2021, "Camioneta", 5, 70000.0, EST_VEH_ALQUILADO);
    vehiculos[2] = crearVehiculo("DEF456", "Volkswagen", "Gol", 2020, "Auto", 5, 30000.0, EST_VEH_DISPONIBLE);
    vehiculos[3] = crearVehiculo("GH789IJ", "Renault", "Kangoo", 2019, "Utilitario", 3, 40000.0, EST_VEH_ALQUILADO);
    vehiculos[4] = crearVehiculo("JKL012", "Chevrolet", "Onix", 2023, "Auto", 5, 38000.0, EST_VEH_DISPONIBLE);
    vehiculos[5] = crearVehiculo("MN345OP", "Fiat", "Cronos", 2022, "Auto", 5, 36000.0, EST_VEH_ALQUILADO);
    vehiculos[6] = crearVehiculo("PQR678", "Peugeot", "208", 2021, "Auto", 5, 34000.0, EST_VEH_DISPONIBLE);
    vehiculos[7] = crearVehiculo("ST901UV", "Toyota", "Hilux", 2023, "Camioneta", 5, 85000.0, EST_VEH_MANT);
    vehiculos[8] = crearVehiculo("WXY234", "Citroen", "Berlingo", 2020, "Utilitario", 3, 42000.0, EST_VEH_DISPONIBLE);
    vehiculos[9] = crearVehiculo("ZAB567", "Volkswagen", "Amarok", 2022, "Camioneta", 5, 90000.0, EST_VEH_BAJA);

    Alquiler alquileres[15];
    alquileres[0] = crearAlquiler(1, 30111222, "ABC123", 20260105, 20260110, 20260110, 5, 225000.0, 225000.0, EST_ALQ_FINALIZADO);
    alquileres[1] = crearAlquiler(2, 31222333, "AB123CD", 20260112, 20260115, 20260115, 3, 210000.0, 210000.0, EST_ALQ_FINALIZADO);
    alquileres[2] = crearAlquiler(3, 32333444, "DEF456", 20260120, 20260127, 20260130, 7, 300000.0, 210000.0, EST_ALQ_FINALIZADO);
    alquileres[3] = crearAlquiler(4, 33444555, "JKL012", 20260201, 20260205, 20260205, 4, 152000.0, 152000.0, EST_ALQ_FINALIZADO);
    alquileres[4] = crearAlquiler(5, 34555666, "WXY234", 20260210, 20260220, 20260220, 10, 420000.0, 200000.0, EST_ALQ_FINALIZADO);
    alquileres[5] = crearAlquiler(6, 35666777, "ZAB567", 20260301, 20260303, 20260303, 2, 180000.0, 180000.0, EST_ALQ_FINALIZADO);
    alquileres[6] = crearAlquiler(7, 30111222, "AB123CD", 20260315, 20260321, 20260322, 6, 490000.0, 490000.0, EST_ALQ_FINALIZADO);
    alquileres[7] = crearAlquiler(8, 36777888, "GH789IJ", 20260401, 20260406, 20260406, 5, 200000.0, 100000.0, EST_ALQ_FINALIZADO);
    alquileres[8] = crearAlquiler(9, 37888999, "ABC123", 20260415, 20260418, 0, 3, 0.0, 0.0, EST_ALQ_CANCELADO);
    alquileres[9] = crearAlquiler(10, 38999000, "DEF456", 20260502, 20260506, 0, 4, 50000.0, 50000.0, EST_ALQ_CANCELADO);
    alquileres[10] = crearAlquiler(11, 31222333, "JKL012", 20260510, 20260518, 20260518, 8, 304000.0, 304000.0, EST_ALQ_FINALIZADO);
    alquileres[11] = crearAlquiler(12, 33444555, "AB123CD", 20260601, 20260606, 0, 5, 0.0, 0.0, EST_ALQ_CANCELADO);
    alquileres[12] = crearAlquiler(13, 34555666, "AB123CD", 20261026, 20261102, 0, 7, 490000.0, 100000.0, EST_ALQ_ACTIVO);
    alquileres[13] = crearAlquiler(14, 32333444, "GH789IJ", 20261028, 20261102, 0, 5, 200000.0, 0.0, EST_ALQ_ACTIVO);
    alquileres[14] = crearAlquiler(15, 35666777, "MN345OP", 20261020, 20261030, 0, 10, 360000.0, 360000.0, EST_ALQ_ACTIVO);

    Movimiento movimientos[23];
    movimientos[0] = crearMovimiento(1, 1, MOV_PAGO, 100000.0, 20260105, MEDIO_EFECTIVO);
    movimientos[1] = crearMovimiento(2, 1, MOV_PAGO, 125000.0, 20260110, MEDIO_TARJETA);
    movimientos[2] = crearMovimiento(3, 2, MOV_PAGO, 210000.0, 20260112, MEDIO_TRANSFERENCIA);
    movimientos[3] = crearMovimiento(4, 3, MOV_PAGO, 100000.0, 20260120, MEDIO_EFECTIVO);
    movimientos[4] = crearMovimiento(5, 3, MOV_PAGO, 110000.0, 20260127, MEDIO_TARJETA);
    movimientos[5] = crearMovimiento(6, 3, MOV_AJUSTE, 90000.0, 20260130, MEDIO_NINGUNO);
    movimientos[6] = crearMovimiento(7, 4, MOV_PAGO, 152000.0, 20260201, MEDIO_TARJETA);
    movimientos[7] = crearMovimiento(8, 5, MOV_PAGO, 100000.0, 20260210, MEDIO_EFECTIVO);
    movimientos[8] = crearMovimiento(9, 5, MOV_PAGO, 100000.0, 20260220, MEDIO_TRANSFERENCIA);
    movimientos[9] = crearMovimiento(10, 6, MOV_PAGO, 180000.0, 20260301, MEDIO_TARJETA);
    movimientos[10] = crearMovimiento(11, 7, MOV_PAGO, 300000.0, 20260315, MEDIO_EFECTIVO);
    movimientos[11] = crearMovimiento(12, 7, MOV_AJUSTE, 70000.0, 20260322, MEDIO_NINGUNO);
    movimientos[12] = crearMovimiento(13, 7, MOV_PAGO, 190000.0, 20260322, MEDIO_TRANSFERENCIA);
    movimientos[13] = crearMovimiento(14, 8, MOV_PAGO, 100000.0, 20260401, MEDIO_EFECTIVO);
    movimientos[14] = crearMovimiento(15, 9, MOV_AJUSTE, -135000.0, 20260416, MEDIO_NINGUNO);
    movimientos[15] = crearMovimiento(16, 10, MOV_PAGO, 50000.0, 20260502, MEDIO_EFECTIVO);
    movimientos[16] = crearMovimiento(17, 10, MOV_AJUSTE, -70000.0, 20260504, MEDIO_NINGUNO);
    movimientos[17] = crearMovimiento(18, 11, MOV_PAGO, 150000.0, 20260510, MEDIO_TARJETA);
    movimientos[18] = crearMovimiento(19, 11, MOV_PAGO, 154000.0, 20260518, MEDIO_EFECTIVO);
    movimientos[19] = crearMovimiento(20, 12, MOV_AJUSTE, -350000.0, 20260602, MEDIO_NINGUNO);
    movimientos[20] = crearMovimiento(21, 15, MOV_PAGO, 180000.0, 20261020, MEDIO_TRANSFERENCIA);
    movimientos[21] = crearMovimiento(22, 15, MOV_PAGO, 180000.0, 20261025, MEDIO_TARJETA);
    movimientos[22] = crearMovimiento(23, 13, MOV_PAGO, 100000.0, 20261026, MEDIO_EFECTIVO);

    completarMovimientos(movimientos, 23, alquileres, 15);

    printf("Generando dataset de prueba del sistema de alquiler...\n");
    if (!guardarClientes(clientes, 10)) {
        return 1;
    }
    if (!guardarVehiculos(vehiculos, 10)) {
        return 1;
    }
    if (!guardarAlquileres(alquileres, 15)) {
        return 1;
    }
    if (!guardarMovimientos(movimientos, 23)) {
        return 1;
    }

    char error[256];
    if (!regenerarTodosLosArchivosTexto(error, sizeof(error))) {
        printf("[ERROR] No se pudieron generar los archivos de texto: %s\n", error);
        return 1;
    }

    printf("\nResumen de datos generados:\n");
    printf("  Clientes:    10\n");
    printf("  Vehiculos:   10\n");
    printf("  Alquileres:  15\n");
    printf("  Movimientos: 23\n\n");

    if (!verificarDataset()) {
        printf("\n[ERROR] La verificacion del dataset fallo.\n");
        return 1;
    }

    printf("\nDATASET GENERADO Y VERIFICADO EXITOSAMENTE.\n");
    return 0;
}

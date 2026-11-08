#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "alquileres.h"
#include "archivos.h"
#include "clientes.h"
#include "comun.h"
#include "fechas.h"
#include "movimientos.h"
#include "utils.h"
#include "vehiculos.h"

namespace {

Cliente crearCliente(int dni, const std::string& nombre, const std::string& telefono,
                     const std::string& email, const std::string& direccion,
                     int fechaAlta, int estado) {
    Cliente c = Cliente();
    c.dni = dni;
    copiarCadena(c.apellidoNombre, LON_NOMBRE, nombre);
    copiarCadena(c.telefono, LON_TEL, telefono);
    copiarCadena(c.email, LON_EMAIL, email);
    copiarCadena(c.direccion, LON_DIR, direccion);
    c.fechaAlta = fechaAlta;
    c.estado = estado;
    return c;
}

Vehiculo crearVehiculo(const std::string& patente, const std::string& marca,
                       const std::string& modelo, int anio, const std::string& tipo,
                       int capacidad, double precioDia, const std::string& estado) {
    Vehiculo v = Vehiculo();
    copiarCadena(v.patente, LON_PATENTE, patente);
    copiarCadena(v.marca, LON_MARCA, marca);
    copiarCadena(v.modelo, LON_MODELO, modelo);
    v.anio = anio;
    copiarCadena(v.tipo, LON_TIPO_VEH, tipo);
    v.capacidad = capacidad;
    v.precioDia = precioDia;
    copiarCadena(v.estado, LON_ESTADO, estado);
    return v;
}

Alquiler crearAlquiler(int numero, int dni, const std::string& patente, int inicio,
                       int prevista, int efectiva, int dias, double total, double pagado,
                       const std::string& estado) {
    Alquiler a = Alquiler();
    a.numAlquiler = numero;
    a.dni = dni;
    copiarCadena(a.patente, LON_PATENTE, patente);
    a.fechaInicio = inicio;
    a.fechaPrevista = prevista;
    a.fechaEfectiva = efectiva;
    a.cantDias = dias;
    a.importeTotal = total;
    a.importePagado = pagado;
    copiarCadena(a.estado, LON_ESTADO, estado);
    return a;
}

Movimiento crearMovimiento(int numero, int alquiler, const std::string& tipo, double monto,
                           int fecha, const std::string& medio) {
    Movimiento m = Movimiento();
    m.numMovimiento = numero;
    m.numAlquiler = alquiler;
    copiarCadena(m.tipoMov, LON_TIPO_MOV, tipo);
    m.monto = monto;
    m.fecha = fecha;
    copiarCadena(m.medioPago, LON_MEDIO_PAGO, medio);
    return m;
}

void completarMovimientos(std::vector<Movimiento>& movimientos,
                          const std::vector<Alquiler>& alquileres) {
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        for (std::size_t j = 0; j < alquileres.size(); ++j) {
            if (alquileres[j].numAlquiler == movimientos[i].numAlquiler) {
                movimientos[i].dni = alquileres[j].dni;
                copiarCadena(movimientos[i].patente, LON_PATENTE,
                             cadenaAString(alquileres[j].patente, LON_PATENTE));
                break;
            }
        }
    }
}

bool verificarDataset() {
    std::vector<Cliente> clientes;
    std::vector<Vehiculo> vehiculos;
    std::vector<Alquiler> alquileres;
    std::vector<Movimiento> movimientos;
    if (!cargarClientes(clientes) || !cargarVehiculos(vehiculos) ||
        !cargarAlquileres(alquileres) || !cargarMovimientos(movimientos)) {
        return false;
    }

    bool ok = true;
    if (clientes.size() != 10) {
        std::cout << "[FALLO] Se esperaban 10 clientes y hay " << clientes.size() << ".\n";
        ok = false;
    }
    if (vehiculos.size() != 10) {
        std::cout << "[FALLO] Se esperaban 10 vehiculos y hay " << vehiculos.size() << ".\n";
        ok = false;
    }
    if (alquileres.size() != 15) {
        std::cout << "[FALLO] Se esperaban 15 alquileres y hay " << alquileres.size() << ".\n";
        ok = false;
    }
    if (movimientos.size() != 23) {
        std::cout << "[FALLO] Se esperaban 23 movimientos y hay " << movimientos.size() << ".\n";
        ok = false;
    }

    for (std::size_t i = 0; i < clientes.size(); ++i) {
        std::string errorCliente;
        if (!validarDatosCliente(clientes[i], errorCliente)) {
            std::cout << "[FALLO] Cliente DNI " << clientes[i].dni << ": "
                      << errorCliente << "\n";
            ok = false;
        }
        for (std::size_t j = i + 1; j < clientes.size(); ++j) {
            std::string emailI = normalizarEmail(
                cadenaAString(clientes[i].email, LON_EMAIL));
            std::string emailJ = normalizarEmail(
                cadenaAString(clientes[j].email, LON_EMAIL));
            if (emailI == emailJ) {
                std::cout << "[FALLO] E-mail duplicado entre los DNI "
                          << clientes[i].dni << " y " << clientes[j].dni
                          << " (" << emailI << ").\n";
                ok = false;
            }
        }
    }

    for (std::size_t i = 0; i < vehiculos.size(); ++i) {
        std::string errorVehiculo;
        if (!validarDatosVehiculo(vehiculos[i], errorVehiculo)) {
            std::cout << "[FALLO] Vehiculo "
                      << cadenaAString(vehiculos[i].patente, LON_PATENTE) << ": "
                      << errorVehiculo << "\n";
            ok = false;
        }
    }

    for (std::size_t i = 0; i < alquileres.size(); ++i) {
        std::string errorAlquiler;
        if (!validarDatosAlquiler(alquileres[i], errorAlquiler)) {
            std::cout << "[FALLO] Alquiler Nro " << alquileres[i].numAlquiler << ": "
                      << errorAlquiler << "\n";
            ok = false;
        }
        for (std::size_t j = i + 1; j < alquileres.size(); ++j) {
            if (alquileres[i].numAlquiler == alquileres[j].numAlquiler) {
                std::cout << "[FALLO] Numero de alquiler duplicado: "
                          << alquileres[i].numAlquiler << ".\n";
                ok = false;
            }
        }
    }

    for (std::size_t i = 0; i < clientes.size(); ++i) {
        int activosCliente = 0;
        for (std::size_t j = 0; j < alquileres.size(); ++j) {
            if (alquileres[j].dni == clientes[i].dni &&
                cadenaAString(alquileres[j].estado, LON_ESTADO) == EST_ALQ_ACTIVO) {
                ++activosCliente;
            }
        }
        if (activosCliente > 1) {
            std::cout << "[FALLO] El cliente DNI " << clientes[i].dni
                      << " tiene " << activosCliente << " alquileres ACTIVOS.\n";
            ok = false;
        }
        if (activosCliente > 0 && clientes[i].estado != CLI_ACTIVO) {
            std::cout << "[FALLO] El cliente DNI " << clientes[i].dni
                      << " esta INACTIVO pero tiene un alquiler ACTIVO.\n";
            ok = false;
        }
    }

    for (std::size_t i = 0; i < vehiculos.size(); ++i) {
        std::string patente = cadenaAString(vehiculos[i].patente, LON_PATENTE);
        bool tieneActivo = false;
        for (std::size_t j = 0; j < alquileres.size(); ++j) {
            if (cadenaAString(alquileres[j].patente, LON_PATENTE) == patente &&
                cadenaAString(alquileres[j].estado, LON_ESTADO) == EST_ALQ_ACTIVO) {
                tieneActivo = true;
                break;
            }
        }
        std::string estadoVehiculo = cadenaAString(vehiculos[i].estado, LON_ESTADO);
        if (tieneActivo && estadoVehiculo != EST_VEH_ALQUILADO) {
            std::cout << "[FALLO] El vehiculo " << patente
                      << " tiene alquiler ACTIVO pero su estado es "
                      << estadoVehiculo << ".\n";
            ok = false;
        }
        if (!tieneActivo && estadoVehiculo == EST_VEH_ALQUILADO) {
            std::cout << "[FALLO] El vehiculo " << patente
                      << " figura Alquilado pero no tiene alquiler ACTIVO.\n";
            ok = false;
        }
    }

    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        const Movimiento& m = movimientos[i];
        int indiceAlquiler = buscarIndiceAlquiler(alquileres, m.numAlquiler);
        if (indiceAlquiler < 0) {
            std::cout << "[FALLO] El movimiento Nro " << m.numMovimiento
                      << " referencia el alquiler inexistente " << m.numAlquiler << ".\n";
            ok = false;
            continue;
        }
        const Alquiler& a = alquileres[static_cast<std::size_t>(indiceAlquiler)];
        if (cadenaAString(m.tipoMov, LON_TIPO_MOV) == MOV_PAGO) {
            if (m.fecha < a.fechaInicio) {
                std::cout << "[FALLO] El pago Nro " << m.numMovimiento
                          << " es anterior al inicio del alquiler " << a.numAlquiler << ".\n";
                ok = false;
            }
            if (!esFechaPasadaOHoy(m.fecha)) {
                std::cout << "[FALLO] El pago Nro " << m.numMovimiento
                          << " tiene fecha futura.\n";
                ok = false;
            }
        }
        if (m.dni != a.dni) {
            std::cout << "[FALLO] El movimiento Nro " << m.numMovimiento
                      << " tiene un DNI distinto al del alquiler.\n";
            ok = false;
        }
        if (cadenaAString(m.patente, LON_PATENTE) !=
            cadenaAString(a.patente, LON_PATENTE)) {
            std::cout << "[FALLO] El movimiento Nro " << m.numMovimiento
                      << " tiene una patente distinta a la del alquiler.\n";
            ok = false;
        }
    }

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
    if (disponibles != 5 || alquilados != 3 || mantenimiento != 1 || baja != 1) {
        std::cout << "[FALLO] Distribucion de estados de vehiculos incorrecta: "
                  << disponibles << " disponibles, " << alquilados << " alquilados, "
                  << mantenimiento << " mantenimiento, " << baja << " baja.\n";
        ok = false;
    }

    int activos = 0;
    int finalizados = 0;
    int cancelados = 0;
    int pagos = 0;
    int ajustes = 0;
    double deuda = 0.0;
    double cobrado = 0.0;
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
                deuda += saldo;
            }
        }
    }
    for (std::size_t i = 0; i < movimientos.size(); ++i) {
        std::string tipo = cadenaAString(movimientos[i].tipoMov, LON_TIPO_MOV);
        if (tipo == MOV_PAGO) {
            ++pagos;
        } else if (tipo == MOV_AJUSTE) {
            ++ajustes;
        }
    }
    if (activos != 3 || finalizados != 9 || cancelados != 3) {
        std::cout << "[FALLO] Distribucion de estados de alquileres incorrecta: "
                  << activos << " activos, " << finalizados << " finalizados, "
                  << cancelados << " cancelados.\n";
        ok = false;
    }
    if (pagos != 18 || ajustes != 5) {
        std::cout << "[FALLO] Se esperaban 18 pagos y 5 ajustes; hay "
                  << pagos << " y " << ajustes << ".\n";
        ok = false;
    }
    if (std::fabs(deuda - 1000000.0) > 0.01) {
        std::cout << "[FALLO] La deuda total esperada era $1000000.00 y es $"
                  << deuda << ".\n";
        ok = false;
    }
    if (std::fabs(cobrado - 2581000.0) > 0.01) {
        std::cout << "[FALLO] El total cobrado esperado era $2581000.00 y es $"
                  << cobrado << ".\n";
        ok = false;
    }

    for (std::size_t i = 0; i < clientes.size(); ++i) {
        std::string ruta = nombreArchivoMovimientos(clientes[i].dni);
        if (!existeArchivo(ruta)) {
            std::cout << "[FALLO] No existe el archivo de texto " << ruta << ".\n";
            ok = false;
        }
    }

    if (ok) {
        std::cout << "[OK] Verificacion del dataset: 10 clientes, 10 vehiculos, "
                     "15 alquileres, 23 movimientos.\n";
        std::cout << "[OK] Estados: 5 disponibles, 3 alquilados, 1 mantenimiento, 1 baja.\n";
        std::cout << "[OK] Alquileres: 3 activos, 9 finalizados, 3 cancelados.\n";
        std::cout << "[OK] Deuda total: $1000000.00 | Total cobrado: $2581000.00.\n";
        std::cout << "[OK] Archivos de texto por cliente generados.\n";
    }
    return ok;
}

} /* namespace */

int main() {
    std::vector<Cliente> clientes;
    clientes.push_back(crearCliente(30111222, "Gomez Martin", "3764-100001",
                                    "mgomez@mail.com", "Av. Mitre 123", 20240110, CLI_ACTIVO));
    clientes.push_back(crearCliente(31222333, "Fernandez Lucia", "3764-100002",
                                    "lfernandez@mail.com", "Colon 456", 20240215, CLI_ACTIVO));
    clientes.push_back(crearCliente(32333444, "Rodriguez Pablo", "3764-100003",
                                    "prodriguez@mail.com", "San Martin 789", 20240320, CLI_ACTIVO));
    clientes.push_back(crearCliente(33444555, "Lopez Carolina", "3764-100004",
                                    "clopez@mail.com", "Sarmiento 321", 20240505, CLI_ACTIVO));
    clientes.push_back(crearCliente(34555666, "Martinez Diego", "3764-100005",
                                    "dmartinez@mail.com", "Belgrano 654", 20240612, CLI_ACTIVO));
    clientes.push_back(crearCliente(35666777, "Sosa Valeria", "3764-100006",
                                    "vsosa@mail.com", "Rivadavia 987", 20240801, CLI_ACTIVO));
    clientes.push_back(crearCliente(36777888, "Benitez Javier", "3764-100007",
                                    "jbenitez@mail.com", "Junin 159", 20240910, CLI_ACTIVO));
    clientes.push_back(crearCliente(37888999, "Acosta Romina", "3764-100008",
                                    "racosta@mail.com", "Cordoba 753", 20241120, CLI_ACTIVO));
    clientes.push_back(crearCliente(38999000, "Duarte Nicolas", "3764-100009",
                                    "nduarte@mail.com", "Entre Rios 852", 20250118, CLI_ACTIVO));
    clientes.push_back(crearCliente(39000111, "Romero Agustina", "3764-100010",
                                    "aromero@mail.com", "Salta 951", 20250302, CLI_INACTIVO));

    std::vector<Vehiculo> vehiculos;
    vehiculos.push_back(crearVehiculo("ABC123", "Toyota", "Corolla", 2022, "Auto",
                                      5, 45000.0, EST_VEH_DISPONIBLE));
    vehiculos.push_back(crearVehiculo("AB123CD", "Ford", "Ranger", 2021, "Camioneta",
                                      5, 70000.0, EST_VEH_ALQUILADO));
    vehiculos.push_back(crearVehiculo("DEF456", "Volkswagen", "Gol", 2020, "Auto",
                                      5, 30000.0, EST_VEH_DISPONIBLE));
    vehiculos.push_back(crearVehiculo("GH789IJ", "Renault", "Kangoo", 2019, "Utilitario",
                                      3, 40000.0, EST_VEH_ALQUILADO));
    vehiculos.push_back(crearVehiculo("JKL012", "Chevrolet", "Onix", 2023, "Auto",
                                      5, 38000.0, EST_VEH_DISPONIBLE));
    vehiculos.push_back(crearVehiculo("MN345OP", "Fiat", "Cronos", 2022, "Auto",
                                      5, 36000.0, EST_VEH_ALQUILADO));
    vehiculos.push_back(crearVehiculo("PQR678", "Peugeot", "208", 2021, "Auto",
                                      5, 34000.0, EST_VEH_DISPONIBLE));
    vehiculos.push_back(crearVehiculo("ST901UV", "Toyota", "Hilux", 2023, "Camioneta",
                                      5, 85000.0, EST_VEH_MANT));
    vehiculos.push_back(crearVehiculo("WXY234", "Citroen", "Berlingo", 2020, "Utilitario",
                                      3, 42000.0, EST_VEH_DISPONIBLE));
    vehiculos.push_back(crearVehiculo("ZAB567", "Volkswagen", "Amarok", 2022, "Camioneta",
                                      5, 90000.0, EST_VEH_BAJA));

    std::vector<Alquiler> alquileres;
    alquileres.push_back(crearAlquiler(1, 30111222, "ABC123", 20260105, 20260110,
                                       20260110, 5, 225000.0, 225000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(2, 31222333, "AB123CD", 20260112, 20260115,
                                       20260115, 3, 210000.0, 210000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(3, 32333444, "DEF456", 20260120, 20260127,
                                       20260130, 7, 300000.0, 210000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(4, 33444555, "JKL012", 20260201, 20260205,
                                       20260205, 4, 152000.0, 152000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(5, 34555666, "WXY234", 20260210, 20260220,
                                       20260220, 10, 420000.0, 200000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(6, 35666777, "ZAB567", 20260301, 20260303,
                                       20260303, 2, 180000.0, 180000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(7, 30111222, "AB123CD", 20260315, 20260321,
                                       20260322, 6, 490000.0, 490000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(8, 36777888, "GH789IJ", 20260401, 20260406,
                                       20260406, 5, 200000.0, 100000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(9, 37888999, "ABC123", 20260415, 20260418,
                                       0, 3, 0.0, 0.0, EST_ALQ_CANCELADO));
    alquileres.push_back(crearAlquiler(10, 38999000, "DEF456", 20260502, 20260506,
                                       0, 4, 50000.0, 50000.0, EST_ALQ_CANCELADO));
    alquileres.push_back(crearAlquiler(11, 31222333, "JKL012", 20260510, 20260518,
                                       20260518, 8, 304000.0, 304000.0, EST_ALQ_FINALIZADO));
    alquileres.push_back(crearAlquiler(12, 33444555, "AB123CD", 20260601, 20260606,
                                       0, 5, 0.0, 0.0, EST_ALQ_CANCELADO));
    alquileres.push_back(crearAlquiler(13, 34555666, "AB123CD", 20261026, 20261102,
                                       0, 7, 490000.0, 100000.0, EST_ALQ_ACTIVO));
    alquileres.push_back(crearAlquiler(14, 32333444, "GH789IJ", 20261028, 20261102,
                                       0, 5, 200000.0, 0.0, EST_ALQ_ACTIVO));
    alquileres.push_back(crearAlquiler(15, 35666777, "MN345OP", 20261020, 20261030,
                                       0, 10, 360000.0, 360000.0, EST_ALQ_ACTIVO));

    std::vector<Movimiento> movimientos;
    movimientos.push_back(crearMovimiento(1, 1, MOV_PAGO, 100000.0, 20260105, MEDIO_EFECTIVO));
    movimientos.push_back(crearMovimiento(2, 1, MOV_PAGO, 125000.0, 20260110, MEDIO_TARJETA));
    movimientos.push_back(crearMovimiento(3, 2, MOV_PAGO, 210000.0, 20260112, MEDIO_TRANSFERENCIA));
    movimientos.push_back(crearMovimiento(4, 3, MOV_PAGO, 100000.0, 20260120, MEDIO_EFECTIVO));
    movimientos.push_back(crearMovimiento(5, 3, MOV_PAGO, 110000.0, 20260127, MEDIO_TARJETA));
    movimientos.push_back(crearMovimiento(6, 3, MOV_AJUSTE, 90000.0, 20260130, MEDIO_NINGUNO));
    movimientos.push_back(crearMovimiento(7, 4, MOV_PAGO, 152000.0, 20260201, MEDIO_TARJETA));
    movimientos.push_back(crearMovimiento(8, 5, MOV_PAGO, 100000.0, 20260210, MEDIO_EFECTIVO));
    movimientos.push_back(crearMovimiento(9, 5, MOV_PAGO, 100000.0, 20260220, MEDIO_TRANSFERENCIA));
    movimientos.push_back(crearMovimiento(10, 6, MOV_PAGO, 180000.0, 20260301, MEDIO_TARJETA));
    movimientos.push_back(crearMovimiento(11, 7, MOV_PAGO, 300000.0, 20260315, MEDIO_EFECTIVO));
    movimientos.push_back(crearMovimiento(12, 7, MOV_AJUSTE, 70000.0, 20260322, MEDIO_NINGUNO));
    movimientos.push_back(crearMovimiento(13, 7, MOV_PAGO, 190000.0, 20260322, MEDIO_TRANSFERENCIA));
    movimientos.push_back(crearMovimiento(14, 8, MOV_PAGO, 100000.0, 20260401, MEDIO_EFECTIVO));
    movimientos.push_back(crearMovimiento(15, 9, MOV_AJUSTE, -135000.0, 20260416, MEDIO_NINGUNO));
    movimientos.push_back(crearMovimiento(16, 10, MOV_PAGO, 50000.0, 20260502, MEDIO_EFECTIVO));
    movimientos.push_back(crearMovimiento(17, 10, MOV_AJUSTE, -70000.0, 20260504, MEDIO_NINGUNO));
    movimientos.push_back(crearMovimiento(18, 11, MOV_PAGO, 150000.0, 20260510, MEDIO_TARJETA));
    movimientos.push_back(crearMovimiento(19, 11, MOV_PAGO, 154000.0, 20260518, MEDIO_EFECTIVO));
    movimientos.push_back(crearMovimiento(20, 12, MOV_AJUSTE, -350000.0, 20260602, MEDIO_NINGUNO));
    movimientos.push_back(crearMovimiento(21, 15, MOV_PAGO, 180000.0, 20261020, MEDIO_TRANSFERENCIA));
    movimientos.push_back(crearMovimiento(22, 15, MOV_PAGO, 180000.0, 20261025, MEDIO_TARJETA));
    movimientos.push_back(crearMovimiento(23, 13, MOV_PAGO, 100000.0, 20261026, MEDIO_EFECTIVO));

    completarMovimientos(movimientos, alquileres);

    std::cout << "Generando dataset de prueba del sistema de alquiler...\n";
    if (!guardarClientes(clientes)) {
        return 1;
    }
    if (!guardarVehiculos(vehiculos)) {
        return 1;
    }
    if (!guardarAlquileres(alquileres)) {
        return 1;
    }
    if (!guardarMovimientos(movimientos)) {
        return 1;
    }

    std::string error;
    if (!regenerarTodosLosArchivosTexto(error)) {
        std::cout << "[ERROR] No se pudieron generar los archivos de texto: "
                  << error << "\n";
        return 1;
    }

    std::cout << "\nResumen de datos generados:\n";
    std::cout << "  Clientes:    " << clientes.size() << "\n";
    std::cout << "  Vehiculos:   " << vehiculos.size() << "\n";
    std::cout << "  Alquileres:  " << alquileres.size() << "\n";
    std::cout << "  Movimientos: " << movimientos.size() << "\n\n";

    if (!verificarDataset()) {
        std::cout << "\n[ERROR] La verificacion del dataset fallo.\n";
        return 1;
    }

    std::cout << "\nDATASET GENERADO Y VERIFICADO EXITOSAMENTE.\n";
    return 0;
}

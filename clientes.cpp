#include "clientes.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "alquileres.h"
#include "archivos.h"
#include "fechas.h"
#include "movimientos.h"
#include "utils.h"
#include "vehiculos.h"

namespace {

bool pedirNombre(const std::string& mensaje, bool permitirVacio, std::string& salida) {
    while (true) {
        if (!leerTexto(mensaje, salida, permitirVacio, LON_NOMBRE - 1)) {
            return false;
        }
        if (salida.empty() && permitirVacio) {
            return true;
        }
        if (validarNombre(salida)) {
            return true;
        }
        std::cout << "[ERROR] Nombre invalido. Use solo letras, espacios, punto, "
                     "apostrofe o guion; minimo 2 caracteres. Ej: Gomez Martin. Reintente.\n";
    }
}

bool pedirTelefono(const std::string& mensaje, bool permitirVacio, std::string& salida) {
    while (true) {
        if (!leerTexto(mensaje, salida, permitirVacio, LON_TEL - 1)) {
            return false;
        }
        if (salida.empty() && permitirVacio) {
            return true;
        }
        if (validarTelefono(salida)) {
            return true;
        }
        std::cout << "[ERROR] Telefono invalido. Debe tener entre 8 y 15 digitos; "
                     "puede usar espacios, '-', '(' y ')'; '+' solo al inicio. "
                     "Ej: 3764-100001. Reintente.\n";
    }
}

bool pedirEmail(const std::string& mensaje, bool permitirVacio, std::string& salida,
                int dniExcluido) {
    while (true) {
        if (!leerTexto(mensaje, salida, permitirVacio, LON_EMAIL - 1)) {
            return false;
        }
        if (salida.empty() && permitirVacio) {
            return true;
        }
        std::string normalizado = normalizarEmail(salida);
        if (!validarEmail(normalizado)) {
            std::cout << "[ERROR] E-mail invalido. Use usuario@dominio.com, sin espacios "
                         "ni '..'. Reintente.\n";
            continue;
        }
        if (existeEmail(normalizado, dniExcluido)) {
            std::cout << "[ERROR] El e-mail " << normalizado
                      << " ya esta registrado para otro cliente. Reintente.\n";
            continue;
        }
        salida = normalizado;
        return true;
    }
}

bool pedirDireccion(const std::string& mensaje, bool permitirVacio, std::string& salida) {
    while (true) {
        if (!leerTexto(mensaje, salida, permitirVacio, LON_DIR - 1)) {
            return false;
        }
        if (salida.empty() && permitirVacio) {
            return true;
        }
        if (validarDireccion(salida)) {
            return true;
        }
        std::cout << "[ERROR] Direccion invalida. Minimo 5 caracteres y al menos una "
                     "letra. Ej: Av. Mitre 123. Reintente.\n";
    }
}

bool pedirFechaAlta(int& fecha) {
    return leerFechaEnRango("Ingrese fecha de alta (AAAAMMDD, hoy o anterior): ",
                            19000101, fechaActual(), fecha);
}

} /* namespace */

bool cargarClientes(std::vector<Cliente>& clientes) {
    std::string error;
    if (!cargarRegistros(ARCH_CLIENTES, clientes, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    for (std::size_t i = 0; i < clientes.size(); ++i) {
        clientes[i].apellidoNombre[LON_NOMBRE - 1] = '\0';
        clientes[i].telefono[LON_TEL - 1] = '\0';
        clientes[i].email[LON_EMAIL - 1] = '\0';
        clientes[i].direccion[LON_DIR - 1] = '\0';
    }
    return true;
}

bool guardarClientes(const std::vector<Cliente>& clientes) {
    std::string error;
    if (!guardarRegistros(ARCH_CLIENTES, clientes, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    return true;
}

int buscarIndiceCliente(const std::vector<Cliente>& clientes, int dni) {
    for (std::size_t i = 0; i < clientes.size(); ++i) {
        if (clientes[i].dni == dni) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool buscarClientePorDNI(int dni, Cliente& cliente) {
    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        return false;
    }
    int indice = buscarIndiceCliente(clientes, dni);
    if (indice < 0) {
        return false;
    }
    cliente = clientes[static_cast<std::size_t>(indice)];
    return true;
}

bool existeCliente(int dni) {
    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        return false;
    }
    return buscarIndiceCliente(clientes, dni) >= 0;
}

bool existeEmail(const std::string& email, int dniExcluido) {
    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        return false;
    }
    std::string buscado = normalizarEmail(email);
    for (std::size_t i = 0; i < clientes.size(); ++i) {
        if (clientes[i].dni == dniExcluido) {
            continue;
        }
        std::string actual = normalizarEmail(cadenaAString(clientes[i].email, LON_EMAIL));
        if (actual == buscado) {
            return true;
        }
    }
    return false;
}

bool clienteSePuedeDarDeBaja(int dni, std::string& error) {
    int alquilerActivo = clienteTieneAlquilerActivo(dni);
    if (alquilerActivo > 0) {
        error = "El cliente posee el alquiler Nro " + enteroAString(alquilerActivo) +
                " en estado ACTIVO. Finalicelo o cancelalo antes de darlo de baja.";
        return false;
    }
    return true;
}

bool validarDatosCliente(const Cliente& cliente, std::string& error) {
    if (!validarDNI(cliente.dni)) {
        error = "El DNI debe tener 7 u 8 digitos (entre 1000000 y 99999999).";
        return false;
    }
    if (!validarNombre(cadenaAString(cliente.apellidoNombre, LON_NOMBRE))) {
        error = "El apellido y nombre no tiene un formato valido. Use solo letras, "
                "espacios, punto, apostrofe o guion; minimo 2 caracteres.";
        return false;
    }
    if (!validarTelefono(cadenaAString(cliente.telefono, LON_TEL))) {
        error = "El telefono no tiene un formato valido. Use entre 8 y 15 digitos, "
                "con espacios, '-', '(' o ')'. Ej: 3764-100001.";
        return false;
    }
    if (!validarEmail(cadenaAString(cliente.email, LON_EMAIL))) {
        error = "El e-mail no tiene un formato valido. Use usuario@dominio.com.";
        return false;
    }
    if (existeEmail(cadenaAString(cliente.email, LON_EMAIL), cliente.dni)) {
        error = "El e-mail ya esta registrado para otro cliente.";
        return false;
    }
    if (!validarDireccion(cadenaAString(cliente.direccion, LON_DIR))) {
        error = "La direccion no tiene un formato valido. Minimo 5 caracteres y "
                "al menos una letra.";
        return false;
    }
    if (!esFechaValida(cliente.fechaAlta)) {
        error = "La fecha de alta no es valida.";
        return false;
    }
    if (!esFechaPasadaOHoy(cliente.fechaAlta)) {
        error = "La fecha de alta no puede ser posterior a la fecha actual.";
        return false;
    }
    if (cliente.estado != CLI_ACTIVO && cliente.estado != CLI_INACTIVO) {
        error = "El estado del cliente debe ser Activo (1) o Inactivo (0).";
        return false;
    }
    return true;
}

void registrarCliente() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "           ALTA DE CLIENTE\n";
    std::cout << "========================================\n";

    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        pausarPantalla();
        return;
    }

    Cliente nuevo = Cliente();
    if (!leerDNI("Ingrese el DNI (7 u 8 digitos): ", nuevo.dni)) {
        return;
    }
    if (buscarIndiceCliente(clientes, nuevo.dni) >= 0) {
        std::cout << "\n[ERROR] El DNI " << nuevo.dni
                  << " ya se encuentra registrado en el sistema.\n";
        pausarPantalla();
        return;
    }

    std::string valor;
    if (!pedirNombre("Ingrese Apellido y Nombre: ", false, valor)) {
        return;
    }
    copiarCadena(nuevo.apellidoNombre, LON_NOMBRE, valor);

    if (!pedirTelefono("Ingrese Telefono: ", false, valor)) {
        return;
    }
    copiarCadena(nuevo.telefono, LON_TEL, valor);

    if (!pedirEmail("Ingrese E-mail: ", false, valor, 0)) {
        return;
    }
    copiarCadena(nuevo.email, LON_EMAIL, valor);

    if (!pedirDireccion("Ingrese Direccion: ", false, valor)) {
        return;
    }
    copiarCadena(nuevo.direccion, LON_DIR, valor);

    if (!pedirFechaAlta(nuevo.fechaAlta)) {
        return;
    }
    nuevo.estado = CLI_ACTIVO;

    std::string error;
    if (!validarDatosCliente(nuevo, error)) {
        std::cout << "\n[ERROR] " << error << "\n";
        pausarPantalla();
        return;
    }

    clientes.push_back(nuevo);
    if (!guardarClientes(clientes)) {
        std::cout << "\n[ERROR] No se pudo guardar el cliente en " << ARCH_CLIENTES << ".\n";
        pausarPantalla();
        return;
    }

    std::string errorTexto;
    if (!regenerarArchivoMovimientosCliente(nuevo.dni, errorTexto)) {
        std::cout << "[AVISO] El cliente se guardo, pero no se pudo crear su archivo "
                     "de movimientos: " << errorTexto << "\n";
    }

    std::cout << "\n>>> Cliente registrado exitosamente (Estado: ACTIVO).\n";
    std::cout << "DNI: " << nuevo.dni << " | Nombre: "
              << cadenaAString(nuevo.apellidoNombre, LON_NOMBRE) << "\n";
    pausarPantalla();
}

void modificarCliente() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "         MODIFICAR CLIENTE\n";
    std::cout << "========================================\n";

    int dni = 0;
    if (!leerDNI("Ingrese el DNI del cliente a modificar: ", dni)) {
        return;
    }

    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        pausarPantalla();
        return;
    }
    int indice = buscarIndiceCliente(clientes, dni);
    if (indice < 0) {
        std::cout << "\n[ERROR] Cliente con DNI " << dni << " no fue encontrado.\n";
        pausarPantalla();
        return;
    }

    Cliente& cliente = clientes[static_cast<std::size_t>(indice)];
    std::cout << "\nCliente encontrado: "
              << cadenaAString(cliente.apellidoNombre, LON_NOMBRE) << "\n";
    std::cout << "Estado actual: "
              << (cliente.estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO") << "\n";
    std::cout << "Complete los datos nuevos o presione ENTER para mantener el valor actual.\n";
    std::cout << "------------------------------------------------------------\n";

    std::string valor;
    if (!pedirNombre("Nuevo Apellido y Nombre (actual: " +
                     cadenaAString(cliente.apellidoNombre, LON_NOMBRE) + "): ",
                     true, valor)) {
        return;
    }
    if (!valor.empty()) {
        copiarCadena(cliente.apellidoNombre, LON_NOMBRE, valor);
    }

    if (!pedirTelefono("Nuevo Telefono (actual: " +
                       cadenaAString(cliente.telefono, LON_TEL) + "): ", true, valor)) {
        return;
    }
    if (!valor.empty()) {
        copiarCadena(cliente.telefono, LON_TEL, valor);
    }

    if (!pedirEmail("Nuevo E-mail (actual: " +
                    cadenaAString(cliente.email, LON_EMAIL) + "): ", true, valor,
                    cliente.dni)) {
        return;
    }
    if (!valor.empty()) {
        copiarCadena(cliente.email, LON_EMAIL, valor);
    }

    if (!pedirDireccion("Nueva Direccion (actual: " +
                        cadenaAString(cliente.direccion, LON_DIR) + "): ", true, valor)) {
        return;
    }
    if (!valor.empty()) {
        copiarCadena(cliente.direccion, LON_DIR, valor);
    }

    std::string error;
    if (!validarDatosCliente(cliente, error)) {
        std::cout << "\n[ERROR] " << error << "\n";
        pausarPantalla();
        return;
    }

    if (!guardarClientes(clientes)) {
        std::cout << "\n[ERROR] No se pudieron guardar los cambios del cliente.\n";
        pausarPantalla();
        return;
    }
    std::cout << "\n>>> Datos del cliente modificados exitosamente.\n";
    pausarPantalla();
}

void consultarCliente() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "         CONSULTAR CLIENTE\n";
    std::cout << "========================================\n";

    int dni = 0;
    if (!leerDNI("Ingrese el DNI del cliente a consultar: ", dni)) {
        return;
    }

    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        pausarPantalla();
        return;
    }
    int indice = buscarIndiceCliente(clientes, dni);
    if (indice < 0) {
        std::cout << "\n[ERROR] Cliente con DNI " << dni << " no fue encontrado.\n";
        pausarPantalla();
        return;
    }

    const Cliente& cliente = clientes[static_cast<std::size_t>(indice)];
    std::cout << "\n========================================\n";
    std::cout << "           DATOS DEL CLIENTE\n";
    std::cout << "========================================\n";
    std::cout << "DNI:            " << cliente.dni << "\n";
    std::cout << "Apellido y Nom: " << cadenaAString(cliente.apellidoNombre, LON_NOMBRE) << "\n";
    std::cout << "Telefono:       " << cadenaAString(cliente.telefono, LON_TEL) << "\n";
    std::cout << "E-mail:         " << cadenaAString(cliente.email, LON_EMAIL) << "\n";
    std::cout << "Direccion:      " << cadenaAString(cliente.direccion, LON_DIR) << "\n";
    std::cout << "Fecha de Alta:  " << fechaALarga(cliente.fechaAlta) << "\n";
    std::cout << "Estado:         "
              << (cliente.estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO") << "\n";
    std::cout << "========================================\n";

    std::vector<Alquiler> alquileres;
    std::vector<Vehiculo> vehiculos;
    bool alquileresOk = cargarAlquileres(alquileres);
    bool vehiculosOk = cargarVehiculos(vehiculos);

    std::cout << "\n--- HISTORIAL DE ALQUILERES ---\n";
    if (!alquileresOk) {
        std::cout << "[ERROR] No se pudo cargar " << ARCH_ALQUILERES << ".\n";
    } else if (!vehiculosOk) {
        std::cout << "[ERROR] No se pudo cargar " << ARCH_VEHICULOS << ".\n";
    } else {
        bool hay = false;
        for (std::size_t i = 0; i < alquileres.size(); ++i) {
            if (alquileres[i].dni == dni) {
                if (!hay) {
                    imprimirEncabezadoAlquileres();
                    hay = true;
                }
                imprimirFilaAlquiler(alquileres[i], clientes, vehiculos);
            }
        }
        if (!hay) {
            std::cout << "El cliente no posee alquileres registrados.\n";
        }
    }

    std::string ruta = nombreArchivoMovimientos(dni);
    std::ifstream archivo(ruta.c_str());
    std::cout << "\n--- HISTORIAL DE MOVIMIENTOS (" << ruta << ") ---\n";
    if (!archivo.is_open()) {
        std::cout << "[AVISO] No se encontro el archivo de movimientos del cliente.\n";
    } else {
        std::cout << std::left << std::setw(6) << "NRO" << " | "
                  << std::setw(6) << "ALQ" << " | "
                  << std::setw(8) << "TIPO" << " | "
                  << std::setw(12) << "MONTO" << " | "
                  << std::setw(10) << "FECHA" << " | "
                  << std::setw(10) << "PATENTE" << " | "
                  << "MEDIO\n";
        std::cout << "-----------------------------------------------------------------------------\n";
        std::string linea;
        int cantidad = 0;
        while (std::getline(archivo, linea)) {
            if (linea.empty()) {
                continue;
            }
            std::vector<std::string> campos = dividir(linea, ';');
            if (campos.size() < 8) {
                continue;
            }
            double monto = 0.0;
            std::stringstream ss(campos[3]);
            ss >> monto;
            std::cout << std::left << std::setw(6) << campos[0] << " | "
                      << std::setw(6) << campos[1] << " | "
                      << std::setw(8) << campos[2] << " | $"
                      << std::setw(11) << std::fixed << std::setprecision(2) << monto << " | "
                      << std::setw(10) << campos[4] << " | "
                      << std::setw(10) << campos[6] << " | "
                      << campos[7] << "\n";
            ++cantidad;
        }
        if (cantidad == 0) {
            std::cout << "Sin movimientos registrados.\n";
        }
    }
    pausarPantalla();
}

void listarClientes() {
    limpiarPantalla();
    std::cout << "\n===============================================================================\n";
    std::cout << "                         LISTADO GENERAL DE CLIENTES\n";
    std::cout << "===============================================================================\n";

    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        pausarPantalla();
        return;
    }
    if (clientes.empty()) {
        std::cout << "No hay clientes registrados.\n";
        pausarPantalla();
        return;
    }

    std::cout << std::left << std::setw(10) << "DNI" << " | "
              << std::setw(26) << "APELLIDO Y NOMBRE" << " | "
              << std::setw(15) << "TELEFONO" << " | "
              << std::setw(26) << "EMAIL" << " | "
              << std::setw(10) << "ALTA" << " | "
              << "ESTADO\n";
    std::cout << "-------------------------------------------------------------------------------\n";
    for (std::size_t i = 0; i < clientes.size(); ++i) {
        const Cliente& c = clientes[i];
        std::cout << std::left << std::setw(10) << c.dni << " | "
                  << std::setw(26) << limitar(cadenaAString(c.apellidoNombre, LON_NOMBRE), 26) << " | "
                  << std::setw(15) << limitar(cadenaAString(c.telefono, LON_TEL), 15) << " | "
                  << std::setw(26) << limitar(cadenaAString(c.email, LON_EMAIL), 26) << " | "
                  << std::setw(10) << fechaAString(c.fechaAlta) << " | "
                  << (c.estado == CLI_ACTIVO ? "ACTIVO" : "INACTIVO") << "\n";
    }
    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "Total de clientes registrados: " << clientes.size() << "\n";
    pausarPantalla();
}

void darDeBajaCliente() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "      BAJA / REACTIVACION DE CLIENTE\n";
    std::cout << "========================================\n";

    int dni = 0;
    if (!leerDNI("Ingrese el DNI del cliente: ", dni)) {
        return;
    }

    std::vector<Cliente> clientes;
    if (!cargarClientes(clientes)) {
        pausarPantalla();
        return;
    }
    int indice = buscarIndiceCliente(clientes, dni);
    if (indice < 0) {
        std::cout << "\n[ERROR] Cliente con DNI " << dni << " no fue encontrado.\n";
        pausarPantalla();
        return;
    }

    Cliente& cliente = clientes[static_cast<std::size_t>(indice)];
    std::string errorBaja;
    if (!clienteSePuedeDarDeBaja(dni, errorBaja)) {
        std::cout << "\n[ERROR] " << errorBaja << "\n";
        pausarPantalla();
        return;
    }

    bool estabaActivo = cliente.estado == CLI_ACTIVO;
    std::cout << "\nCliente: " << cadenaAString(cliente.apellidoNombre, LON_NOMBRE)
              << " (DNI " << cliente.dni << ")\n";
    std::cout << "Estado actual: " << (estabaActivo ? "ACTIVO" : "INACTIVO") << "\n";
    std::cout << "La operacion cambiara el estado a "
              << (estabaActivo ? "INACTIVO" : "ACTIVO") << ".\n";

    if (!leerSN("Confirma la operacion (S/N)?: ")) {
        if (finEntrada) {
            return;
        }
        std::cout << "\nOperacion cancelada.\n";
        pausarPantalla();
        return;
    }

    cliente.estado = estabaActivo ? CLI_INACTIVO : CLI_ACTIVO;
    if (!guardarClientes(clientes)) {
        std::cout << "\n[ERROR] No se pudo actualizar el estado del cliente.\n";
        pausarPantalla();
        return;
    }

    std::cout << "\n>>> Cliente " << (estabaActivo ? "dado de baja" : "reactivado")
              << " exitosamente.\n";
    pausarPantalla();
}

void submenuClientes() {
    int opcion = -1;
    do {
        limpiarPantalla();
        std::cout << "\n========================================\n";
        std::cout << "           SUBMENU CLIENTES\n";
        std::cout << "========================================\n";
        std::cout << "1. Registrar cliente\n";
        std::cout << "2. Modificar cliente\n";
        std::cout << "3. Consultar cliente (con historial)\n";
        std::cout << "4. Listar clientes\n";
        std::cout << "5. Dar de baja / reactivar cliente\n";
        std::cout << "0. Volver al Menu Principal\n";
        std::cout << "========================================\n";
        if (!leerEntero("Seleccione una opcion (0-5): ", 0, 5, opcion)) {
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
                std::cout << "\n[ERROR] Opcion no valida.\n";
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

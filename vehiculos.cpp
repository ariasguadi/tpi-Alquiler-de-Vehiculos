#include "vehiculos.h"

#include <cmath>
#include <iomanip>
#include <iostream>

#include "alquileres.h"
#include "archivos.h"
#include "fechas.h"
#include "utils.h"

namespace {

bool pedirMarca(const std::string& mensaje, bool permitirVacio, std::string& salida) {
    while (true) {
        if (!leerTexto(mensaje, salida, permitirVacio, LON_MARCA - 1)) {
            return false;
        }
        if (salida.empty() && permitirVacio) {
            return true;
        }
        if (validarMarca(salida)) {
            return true;
        }
        std::cout << "[ERROR] Marca invalida. Minimo 2 caracteres, con al menos una "
                     "letra; puede usar espacios, '-', '.' o '&'. Ej: Toyota. Reintente.\n";
    }
}

bool pedirModelo(const std::string& mensaje, bool permitirVacio, std::string& salida) {
    while (true) {
        if (!leerTexto(mensaje, salida, permitirVacio, LON_MODELO - 1)) {
            return false;
        }
        if (salida.empty() && permitirVacio) {
            return true;
        }
        if (validarModelo(salida)) {
            return true;
        }
        std::cout << "[ERROR] Modelo invalido. Minimo 2 caracteres, con al menos una "
                     "letra o digito; puede usar espacios, '-', '.', '/' o '&'. "
                     "Ej: Corolla, 208, C4. Reintente.\n";
    }
}

bool pedirTipo(const std::string& mensaje, bool permitirVacio, std::string& salida) {
    while (true) {
        std::cout << "Tipos disponibles:\n";
        std::cout << "1. Auto\n";
        std::cout << "2. Camioneta\n";
        std::cout << "3. Utilitario\n";
        std::cout << "4. Moto\n";
        std::cout << "5. 4x4\n";
        if (permitirVacio) {
            std::cout << "0. Mantener tipo actual\n";
        }
        int opcion = 0;
        if (!leerEntero(mensaje, 0, 5, opcion)) {
            return false;
        }
        if (opcion == 0) {
            if (permitirVacio) {
                salida.clear();
                return true;
            }
            std::cout << "[ERROR] Debe seleccionar un tipo de vehiculo. Reintente.\n";
            continue;
        }
        static const char* tipos[] = {"Auto", "Camioneta", "Utilitario", "Moto", "4x4"};
        salida = tipos[opcion - 1];
        return true;
    }
}

int anioMaximo() {
    return fechaActual() / 10000 + 1;
}

} /* namespace */

bool cargarVehiculos(std::vector<Vehiculo>& vehiculos) {
    std::string error;
    if (!cargarRegistros(ARCH_VEHICULOS, vehiculos, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    for (std::size_t i = 0; i < vehiculos.size(); ++i) {
        vehiculos[i].patente[LON_PATENTE - 1] = '\0';
        vehiculos[i].marca[LON_MARCA - 1] = '\0';
        vehiculos[i].modelo[LON_MODELO - 1] = '\0';
        vehiculos[i].tipo[LON_TIPO_VEH - 1] = '\0';
        vehiculos[i].estado[LON_ESTADO - 1] = '\0';
    }
    return true;
}

bool guardarVehiculos(const std::vector<Vehiculo>& vehiculos) {
    std::string error;
    if (!guardarRegistros(ARCH_VEHICULOS, vehiculos, error)) {
        std::cout << "[ERROR] " << error << "\n";
        return false;
    }
    return true;
}

int buscarIndiceVehiculo(const std::vector<Vehiculo>& vehiculos, const std::string& patente) {
    for (std::size_t i = 0; i < vehiculos.size(); ++i) {
        if (cadenaAString(vehiculos[i].patente, LON_PATENTE) == patente) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool buscarVehiculoPorPatente(const std::string& patente, Vehiculo& vehiculo) {
    std::vector<Vehiculo> vehiculos;
    if (!cargarVehiculos(vehiculos)) {
        return false;
    }
    int indice = buscarIndiceVehiculo(vehiculos, patente);
    if (indice < 0) {
        return false;
    }
    vehiculo = vehiculos[static_cast<std::size_t>(indice)];
    return true;
}

bool existeVehiculo(const std::string& patente) {
    std::vector<Vehiculo> vehiculos;
    if (!cargarVehiculos(vehiculos)) {
        return false;
    }
    return buscarIndiceVehiculo(vehiculos, patente) >= 0;
}

bool validarDatosVehiculo(const Vehiculo& vehiculo, std::string& error) {
    std::string patente = cadenaAString(vehiculo.patente, LON_PATENTE);
    if (!esPatenteValida(patente)) {
        error = "La patente no tiene un formato valido. Use AAA123, AA123AA, "
                "123ABC o A123ABC.";
        return false;
    }
    if (!validarMarca(cadenaAString(vehiculo.marca, LON_MARCA))) {
        error = "La marca no tiene un formato valido. Minimo 2 caracteres con letras.";
        return false;
    }
    if (!validarModelo(cadenaAString(vehiculo.modelo, LON_MODELO))) {
        error = "El modelo no tiene un formato valido. Minimo 2 caracteres con "
                "letras o digitos. Ej: Corolla, 208, C4.";
        return false;
    }
    if (!validarAnioVehiculo(vehiculo.anio)) {
        error = "El anio debe estar entre " + enteroAString(ANIO_MIN_VEHICULO) +
                " y " + enteroAString(anioMaximo()) + ".";
        return false;
    }
    if (!validarTipoVehiculo(cadenaAString(vehiculo.tipo, LON_TIPO_VEH))) {
        error = "El tipo debe ser Auto, Camioneta, Utilitario, Moto o 4x4.";
        return false;
    }
    if (!validarCapacidadVehiculo(vehiculo.capacidad)) {
        error = "La capacidad debe estar entre " +
                enteroAString(CAPACIDAD_MIN_VEHICULO) + " y " +
                enteroAString(CAPACIDAD_MAX_VEHICULO) + " personas.";
        return false;
    }
    if (!validarPrecioDia(vehiculo.precioDia)) {
        error = "El precio por dia debe ser mayor a cero y no superar $" +
                enteroAString(static_cast<int>(PRECIO_DIA_MAXIMO)) + ".";
        return false;
    }
    if (!validarEstadoVehiculo(cadenaAString(vehiculo.estado, LON_ESTADO))) {
        error = "El estado del vehiculo no es valido.";
        return false;
    }
    return true;
}

bool vehiculoSePuedeDarDeBaja(const std::string& patente, std::string& error) {
    if (vehiculoTieneAlquilerActivo(patente)) {
        error = "El vehiculo con patente " + patente + " posee un alquiler ACTIVO. "
                "Finalicelo o cancelalo antes de pasarlo a Baja o En mantenimiento.";
        return false;
    }
    return true;
}

bool cambiarEstadoVehiculo(const std::string& patente, const std::string& nuevoEstado,
                           std::string& error) {
    std::vector<Vehiculo> vehiculos;
    if (!cargarVehiculos(vehiculos)) {
        error = "No se pudo cargar " + std::string(ARCH_VEHICULOS) + ".";
        return false;
    }
    int indice = buscarIndiceVehiculo(vehiculos, patente);
    if (indice < 0) {
        error = "El vehiculo con patente " + patente + " no existe.";
        return false;
    }
    if (!validarEstadoVehiculo(nuevoEstado)) {
        error = "El estado de vehiculo '" + nuevoEstado + "' no es valido.";
        return false;
    }
    if (vehiculoTieneAlquilerActivo(patente)) {
        if (nuevoEstado == EST_VEH_BAJA || nuevoEstado == EST_VEH_MANT) {
            return vehiculoSePuedeDarDeBaja(patente, error);
        }
        if (nuevoEstado == EST_VEH_DISPONIBLE) {
            error = "El vehiculo posee un alquiler ACTIVO y no puede pasar a Disponible.";
            return false;
        }
    }
    copiarCadena(vehiculos[static_cast<std::size_t>(indice)].estado, LON_ESTADO, nuevoEstado);
    if (!guardarVehiculos(vehiculos)) {
        error = "No se pudo guardar " + std::string(ARCH_VEHICULOS) + ".";
        return false;
    }
    return true;
}

void registrarVehiculo() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "           ALTA DE VEHICULO\n";
    std::cout << "========================================\n";

    std::vector<Vehiculo> vehiculos;
    if (!cargarVehiculos(vehiculos)) {
        pausarPantalla();
        return;
    }

    Vehiculo nuevo = Vehiculo();
    std::string patente;
    if (!leerPatente("Ingrese la patente (AAA123, AA123AA, 123ABC o A123ABC): ", patente)) {
        return;
    }
    if (!esPatenteValida(patente)) {
        std::cout << "\n[ERROR] Patente invalida. Use AAA123, AA123AA, 123ABC o "
                     "A123ABC.\n";
        pausarPantalla();
        return;
    }
    if (buscarIndiceVehiculo(vehiculos, patente) >= 0) {
        std::cout << "\n[ERROR] La patente " << patente << " ya se encuentra registrada.\n";
        pausarPantalla();
        return;
    }
    copiarCadena(nuevo.patente, LON_PATENTE, patente);

    std::string valor;
    if (!pedirMarca("Ingrese la marca: ", false, valor)) {
        return;
    }
    copiarCadena(nuevo.marca, LON_MARCA, valor);

    if (!pedirModelo("Ingrese el modelo: ", false, valor)) {
        return;
    }
    copiarCadena(nuevo.modelo, LON_MODELO, valor);

    if (!leerEntero("Ingrese el anio de fabricacion (" +
                        enteroAString(ANIO_MIN_VEHICULO) + "-" +
                        enteroAString(anioMaximo()) + "): ",
                    ANIO_MIN_VEHICULO, anioMaximo(), nuevo.anio)) {
        return;
    }

    if (!pedirTipo("Seleccione el tipo (1-5): ", false, valor)) {
        return;
    }
    copiarCadena(nuevo.tipo, LON_TIPO_VEH, valor);

    if (!leerEntero("Ingrese la capacidad de personas (" +
                        enteroAString(CAPACIDAD_MIN_VEHICULO) + "-" +
                        enteroAString(CAPACIDAD_MAX_VEHICULO) + "): ",
                    CAPACIDAD_MIN_VEHICULO, CAPACIDAD_MAX_VEHICULO, nuevo.capacidad)) {
        return;
    }
    if (!leerDoubleEnRango("Ingrese el precio de alquiler por dia: ", 0.0,
                           PRECIO_DIA_MAXIMO, nuevo.precioDia)) {
        return;
    }
    copiarCadena(nuevo.estado, LON_ESTADO, EST_VEH_DISPONIBLE);

    std::string error;
    if (!validarDatosVehiculo(nuevo, error)) {
        std::cout << "\n[ERROR] " << error << "\n";
        pausarPantalla();
        return;
    }

    vehiculos.push_back(nuevo);
    if (!guardarVehiculos(vehiculos)) {
        std::cout << "\n[ERROR] No se pudo guardar el vehiculo en " << ARCH_VEHICULOS << ".\n";
        pausarPantalla();
        return;
    }

    std::cout << "\n>>> Vehiculo registrado exitosamente (Estado: DISPONIBLE).\n";
    std::cout << "Patente: " << patente << " | "
              << cadenaAString(nuevo.marca, LON_MARCA) << " "
              << cadenaAString(nuevo.modelo, LON_MODELO) << "\n";
    pausarPantalla();
}

void modificarVehiculo() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "         MODIFICAR VEHICULO\n";
    std::cout << "========================================\n";

    std::string patente;
    if (!leerPatente("Ingrese la patente del vehiculo a modificar: ", patente)) {
        return;
    }

    std::vector<Vehiculo> vehiculos;
    if (!cargarVehiculos(vehiculos)) {
        pausarPantalla();
        return;
    }
    int indice = buscarIndiceVehiculo(vehiculos, patente);
    if (indice < 0) {
        std::cout << "\n[ERROR] Vehiculo con patente " << patente << " no encontrado.\n";
        pausarPantalla();
        return;
    }

    Vehiculo& vehiculo = vehiculos[static_cast<std::size_t>(indice)];
    bool tieneAlquilerActivo = vehiculoTieneAlquilerActivo(patente);

    std::cout << "\nVehiculo encontrado: " << cadenaAString(vehiculo.marca, LON_MARCA)
              << " " << cadenaAString(vehiculo.modelo, LON_MODELO)
              << " (Anio: " << vehiculo.anio
              << ", Estado: " << cadenaAString(vehiculo.estado, LON_ESTADO) << ")\n";
    std::cout << "Precio por dia actual: $" << std::fixed << std::setprecision(2)
              << vehiculo.precioDia << "\n";
    std::cout << "Complete los datos nuevos o presione ENTER para mantener el valor actual.\n";
    if (tieneAlquilerActivo) {
        std::cout << "[AVISO] El vehiculo tiene un alquiler ACTIVO. No se puede cambiar "
                     "su estado ni su precio por dia.\n";
    }
    std::cout << "------------------------------------------------------------\n";

    std::string valor;
    if (!pedirMarca("Nueva marca (actual: " + cadenaAString(vehiculo.marca, LON_MARCA) + "): ",
                    true, valor)) {
        return;
    }
    if (!valor.empty()) {
        copiarCadena(vehiculo.marca, LON_MARCA, valor);
    }

    if (!pedirModelo("Nuevo modelo (actual: " + cadenaAString(vehiculo.modelo, LON_MODELO) + "): ",
                     true, valor)) {
        return;
    }
    if (!valor.empty()) {
        copiarCadena(vehiculo.modelo, LON_MODELO, valor);
    }

    bool cambiarAnio = leerSN("Desea cambiar el anio de fabricacion (S/N)?: ");
    if (finEntrada) {
        return;
    }
    if (cambiarAnio) {
        int nuevoAnio = vehiculo.anio;
        if (!leerEntero("Nuevo anio (" + enteroAString(ANIO_MIN_VEHICULO) + "-" +
                            enteroAString(anioMaximo()) + "): ",
                        ANIO_MIN_VEHICULO, anioMaximo(), nuevoAnio)) {
            return;
        }
        vehiculo.anio = nuevoAnio;
    }

    if (!pedirTipo("Seleccione el nuevo tipo (actual: " +
                       cadenaAString(vehiculo.tipo, LON_TIPO_VEH) + "): ",
                   true, valor)) {
        return;
    }
    if (!valor.empty()) {
        copiarCadena(vehiculo.tipo, LON_TIPO_VEH, valor);
    }

    bool cambiarCapacidad = leerSN("Desea cambiar la capacidad (S/N)?: ");
    if (finEntrada) {
        return;
    }
    if (cambiarCapacidad) {
        int nuevaCapacidad = vehiculo.capacidad;
        if (!leerEntero("Nueva capacidad (" + enteroAString(CAPACIDAD_MIN_VEHICULO) + "-" +
                            enteroAString(CAPACIDAD_MAX_VEHICULO) + "): ",
                        CAPACIDAD_MIN_VEHICULO, CAPACIDAD_MAX_VEHICULO, nuevaCapacidad)) {
            return;
        }
        vehiculo.capacidad = nuevaCapacidad;
    }

    if (!tieneAlquilerActivo) {
        bool cambiarPrecio = leerSN("Desea cambiar el precio por dia (S/N)?: ");
        if (finEntrada) {
            return;
        }
        if (cambiarPrecio) {
            double nuevoPrecio = vehiculo.precioDia;
            if (!leerDoubleEnRango("Nuevo precio por dia: ", 0.0, PRECIO_DIA_MAXIMO,
                                   nuevoPrecio)) {
                return;
            }
            vehiculo.precioDia = nuevoPrecio;
        }
    }

    std::cout << "\nSeleccione el nuevo estado del vehiculo:\n";
    std::cout << "1. " << EST_VEH_DISPONIBLE << "\n";
    std::cout << "2. " << EST_VEH_MANT << "\n";
    std::cout << "3. " << EST_VEH_BAJA << "\n";
    std::cout << "0. Mantener estado actual ("
              << cadenaAString(vehiculo.estado, LON_ESTADO) << ")\n";
    int opcionEstado = 0;
    if (!leerEntero("Opcion: ", 0, 3, opcionEstado)) {
        return;
    }
    if (opcionEstado == 1) {
        if (tieneAlquilerActivo) {
            std::cout << "\n[ERROR] No se puede pasar el vehiculo a Disponible mientras "
                         "tenga un alquiler ACTIVO.\n";
            pausarPantalla();
            return;
        }
        copiarCadena(vehiculo.estado, LON_ESTADO, EST_VEH_DISPONIBLE);
    } else if (opcionEstado == 2 || opcionEstado == 3) {
        std::string errorBaja;
        if (!vehiculoSePuedeDarDeBaja(patente, errorBaja)) {
            std::cout << "\n[ERROR] " << errorBaja << "\n";
            pausarPantalla();
            return;
        }
        if (opcionEstado == 2) {
            copiarCadena(vehiculo.estado, LON_ESTADO, EST_VEH_MANT);
        } else {
            copiarCadena(vehiculo.estado, LON_ESTADO, EST_VEH_BAJA);
        }
    }

    std::string error;
    if (!validarDatosVehiculo(vehiculo, error)) {
        std::cout << "\n[ERROR] " << error << "\n";
        pausarPantalla();
        return;
    }

    if (!guardarVehiculos(vehiculos)) {
        std::cout << "\n[ERROR] No se pudieron guardar las modificaciones del vehiculo.\n";
        pausarPantalla();
        return;
    }
    std::cout << "\n>>> Vehiculo modificado exitosamente.\n";
    pausarPantalla();
}

void buscarVehiculo() {
    limpiarPantalla();
    std::cout << "\n========================================\n";
    std::cout << "            BUSCAR VEHICULO\n";
    std::cout << "========================================\n";

    std::string patente;
    if (!leerPatente("Ingrese la patente a buscar: ", patente)) {
        return;
    }

    Vehiculo vehiculo = Vehiculo();
    if (!buscarVehiculoPorPatente(patente, vehiculo)) {
        std::cout << "\n[ERROR] Vehiculo con patente " << patente << " no fue encontrado.\n";
        pausarPantalla();
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "           DATOS DEL VEHICULO\n";
    std::cout << "========================================\n";
    std::cout << "Patente:        " << cadenaAString(vehiculo.patente, LON_PATENTE) << "\n";
    std::cout << "Marca y Modelo: " << cadenaAString(vehiculo.marca, LON_MARCA) << " "
              << cadenaAString(vehiculo.modelo, LON_MODELO) << "\n";
    std::cout << "Anio:           " << vehiculo.anio << "\n";
    std::cout << "Tipo:           " << cadenaAString(vehiculo.tipo, LON_TIPO_VEH) << "\n";
    std::cout << "Capacidad:      " << vehiculo.capacidad << " personas\n";
    std::cout << "Precio por dia: $" << std::fixed << std::setprecision(2)
              << vehiculo.precioDia << "\n";
    std::cout << "Estado:         " << cadenaAString(vehiculo.estado, LON_ESTADO) << "\n";
    std::cout << "========================================\n";
    pausarPantalla();
}

void listarVehiculos() {
    limpiarPantalla();
    std::cout << "\n=====================================================================================\n";
    std::cout << "                         LISTADO GENERAL DE VEHICULOS\n";
    std::cout << "=====================================================================================\n";

    std::vector<Vehiculo> vehiculos;
    if (!cargarVehiculos(vehiculos)) {
        pausarPantalla();
        return;
    }
    if (vehiculos.empty()) {
        std::cout << "No hay vehiculos registrados.\n";
        pausarPantalla();
        return;
    }

    std::cout << std::left << std::setw(9) << "PATENTE" << " | "
              << std::setw(12) << "MARCA" << " | "
              << std::setw(14) << "MODELO" << " | "
              << std::setw(6) << "ANIO" << " | "
              << std::setw(12) << "TIPO" << " | "
              << std::setw(5) << "CAP." << " | "
              << std::setw(12) << "PRECIO/DIA" << " | "
              << "ESTADO\n";
    std::cout << "-------------------------------------------------------------------------------------\n";
    for (std::size_t i = 0; i < vehiculos.size(); ++i) {
        const Vehiculo& v = vehiculos[i];
        std::cout << std::left << std::setw(9) << cadenaAString(v.patente, LON_PATENTE) << " | "
                  << std::setw(12) << limitar(cadenaAString(v.marca, LON_MARCA), 12) << " | "
                  << std::setw(14) << limitar(cadenaAString(v.modelo, LON_MODELO), 14) << " | "
                  << std::setw(6) << v.anio << " | "
                  << std::setw(12) << limitar(cadenaAString(v.tipo, LON_TIPO_VEH), 12) << " | "
                  << std::setw(5) << v.capacidad << " | $"
                  << std::setw(11) << std::fixed << std::setprecision(2) << v.precioDia << " | "
                  << cadenaAString(v.estado, LON_ESTADO) << "\n";
    }
    std::cout << "-------------------------------------------------------------------------------------\n";
    std::cout << "Total de vehiculos registrados: " << vehiculos.size() << "\n";
    pausarPantalla();
}

void submenuVehiculos() {
    int opcion = -1;
    do {
        limpiarPantalla();
        std::cout << "\n========================================\n";
        std::cout << "           SUBMENU VEHICULOS\n";
        std::cout << "========================================\n";
        std::cout << "1. Registrar vehiculo\n";
        std::cout << "2. Modificar vehiculo\n";
        std::cout << "3. Buscar vehiculo por patente\n";
        std::cout << "4. Listar vehiculos\n";
        std::cout << "0. Volver al Menu Principal\n";
        std::cout << "========================================\n";
        if (!leerEntero("Seleccione una opcion (0-4): ", 0, 4, opcion)) {
            break;
        }
        switch (opcion) {
            case 1: registrarVehiculo(); break;
            case 2: modificarVehiculo(); break;
            case 3: buscarVehiculo(); break;
            case 4: listarVehiculos(); break;
            case 0: break;
            default:
                std::cout << "\n[ERROR] Opcion no valida.\n";
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

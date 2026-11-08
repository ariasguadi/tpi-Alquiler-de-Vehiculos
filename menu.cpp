#include "menu.h"

#include <iostream>

#include "alquileres.h"
#include "clientes.h"
#include "movimientos.h"
#include "reportes.h"
#include "utils.h"
#include "vehiculos.h"

void menuPrincipal() {
    int opcion = -1;
    do {
        limpiarPantalla();
        std::cout << "\n========================================\n";
        std::cout << "     SISTEMA DE ALQUILER DE VEHICULOS\n";
        std::cout << "========================================\n";
        std::cout << "1. Clientes\n";
        std::cout << "2. Vehiculos\n";
        std::cout << "3. Alquileres\n";
        std::cout << "4. Devoluciones\n";
        std::cout << "5. Pagos\n";
        std::cout << "6. Consultas y Reportes\n";
        std::cout << "0. Salir\n";
        std::cout << "========================================\n";
        if (!leerEntero("Seleccione una opcion (0-6): ", 0, 6, opcion)) {
            break;
        }
        switch (opcion) {
            case 1: submenuClientes(); break;
            case 2: submenuVehiculos(); break;
            case 3: submenuAlquileres(); break;
            case 4: registrarDevolucion(); break;
            case 5: submenuMovimientos(); break;
            case 6: submenuReportes(); break;
            case 0:
                std::cout << "\nGracias por utilizar el sistema. Hasta luego.\n";
                break;
            default:
                std::cout << "\n[ERROR] Opcion no valida.\n";
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

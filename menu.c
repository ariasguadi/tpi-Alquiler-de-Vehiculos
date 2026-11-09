#include "menu.h"

#include <stdio.h>

#include "alquileres.h"
#include "clientes.h"
#include "movimientos.h"
#include "reportes.h"
#include "utils.h"
#include "vehiculos.h"

void menuPrincipal(void) {
    int opcion = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("     SISTEMA DE ALQUILER DE VEHICULOS\n");
        printf("========================================\n");
        printf("1. Clientes\n");
        printf("2. Vehiculos\n");
        printf("3. Alquileres\n");
        printf("4. Devoluciones\n");
        printf("5. Pagos\n");
        printf("6. Consultas y Reportes\n");
        printf("0. Salir\n");
        printf("========================================\n");

        if (leerEntero("Seleccione una opcion (0-6): ", 0, 6, &opcion) != LEIDO) {
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
                printf("\nGracias por utilizar el sistema. Hasta luego.\n");
                break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opcion != 0);
}

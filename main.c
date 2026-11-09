#include <stdio.h>
#include <string.h>

#include "menu.h"
#include "utils.h"

static void mostrarAyuda(const char* programa) {
    printf("Uso: %s [opciones]\n", programa);
    printf("Opciones:\n");
    printf("  --test    Modo de prueba: no limpia pantalla ni pausa.\n");
    printf("  --help    Muestra esta ayuda.\n");
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--test") == 0) {
            modoPrueba = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            mostrarAyuda(argv[0]);
            return 0;
        } else {
            printf("[ERROR] Opcion desconocida: %s\n", argv[i]);
            mostrarAyuda(argv[0]);
            return 1;
        }
    }

    /* if (!modoPrueba) {
        printf("Sistema de Gestion de Alquiler de Vehiculos\n");
        printf("Trabajo Practico Integrador - Programacion Estructurada\n");
        printf("Asegurese de haber ejecutado generar_datos.exe para cargar los datos de prueba.\n");
        printf("Presione ENTER para comenzar...");
        fflush(stdout);
        char linea[128];
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            return 0;
        }
    } */

    menuPrincipal();
    return 0;
}

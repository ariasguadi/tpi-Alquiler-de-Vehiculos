#include <iostream>
#include <string>

#include "menu.h"
#include "utils.h"

namespace {

void mostrarAyuda(const char* programa) {
    std::cout << "Uso: " << programa << " [opciones]\n";
    std::cout << "Opciones:\n";
    std::cout << "  --test    Modo de prueba: no limpia pantalla ni pausa.\n";
    std::cout << "  --help    Muestra esta ayuda.\n";
}

} /* namespace */

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string argumento = argv[i];
        if (argumento == "--test") {
            modoPrueba = true;
        } else if (argumento == "--help" || argumento == "-h") {
            mostrarAyuda(argv[0]);
            return 0;
        } else {
            std::cout << "[ERROR] Opcion desconocida: " << argumento << "\n";
            mostrarAyuda(argv[0]);
            return 1;
        }
    }

    if (!modoPrueba) {
        std::cout << "Sistema de Gestion de Alquiler de Vehiculos\n";
        std::cout << "Trabajo Practico Integrador - Programacion Estructurada\n";
        std::cout << "Asegurese de haber ejecutado generar_datos.exe para cargar "
                     "los datos de prueba.\n";
        std::cout << "Presione ENTER para comenzar...";
        std::cout.flush();
        std::string linea;
        if (!std::getline(std::cin, linea)) {
            return 0;
        }
    }

    menuPrincipal();
    return 0;
}

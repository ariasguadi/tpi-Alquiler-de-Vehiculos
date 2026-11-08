# ---------------------------------------------------------------------------
# Sistema de Gestion de Alquiler de Vehiculos
# Makefile generico para MinGW/GCC con g++ disponible en el PATH.
#
# Uso:
#   mingw32-make            Compila los tres ejecutables.
#   mingw32-make sistema    Compila solo sistema.exe.
#   mingw32-make datos      Compila solo generar_datos.exe.
#   mingw32-make tests      Compila solo tests_validaciones.exe.
#   mingw32-make ejecutar   Compila y ejecuta la aplicacion.
#   mingw32-make probar     Compila y ejecuta las pruebas.
#   mingw32-make limpiar    Elimina los ejecutables generados.
# ---------------------------------------------------------------------------

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -O2

BIN_SISTEMA = sistema.exe
BIN_DATOS   = generar_datos.exe
BIN_TESTS   = tests_validaciones.exe

FUENTES_SISTEMA = main.cpp menu.cpp fechas.cpp utils.cpp clientes.cpp \
                  vehiculos.cpp alquileres.cpp movimientos.cpp reportes.cpp
FUENTES_DATOS   = generar_datos.cpp fechas.cpp utils.cpp clientes.cpp \
                  vehiculos.cpp alquileres.cpp movimientos.cpp
FUENTES_TESTS   = tests_validaciones.cpp fechas.cpp utils.cpp

CABECERAS = $(wildcard *.h)

.PHONY: all sistema datos tests ejecutar probar limpiar

all: sistema datos tests

sistema: $(BIN_SISTEMA)

datos: $(BIN_DATOS)

tests: $(BIN_TESTS)

$(BIN_SISTEMA): $(FUENTES_SISTEMA) $(CABECERAS)
	$(CXX) $(CXXFLAGS) -o $@ $(FUENTES_SISTEMA)

$(BIN_DATOS): $(FUENTES_DATOS) $(CABECERAS)
	$(CXX) $(CXXFLAGS) -o $@ $(FUENTES_DATOS)

$(BIN_TESTS): $(FUENTES_TESTS) $(CABECERAS)
	$(CXX) $(CXXFLAGS) -o $@ $(FUENTES_TESTS)

ejecutar: $(BIN_SISTEMA)
	./$(BIN_SISTEMA)

probar: $(BIN_TESTS)
	./$(BIN_TESTS)

limpiar:
	-rm -f $(BIN_SISTEMA) $(BIN_DATOS) $(BIN_TESTS)

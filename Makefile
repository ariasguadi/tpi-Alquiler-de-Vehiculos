# ---------------------------------------------------------------------------
# Sistema de Gestion de Alquiler de Vehiculos
# Makefile generico para MinGW/GCC (C99/C11).
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

CC     ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -O2

BIN_SISTEMA = sistema.exe
BIN_DATOS   = generar_datos.exe
BIN_TESTS   = tests_validaciones.exe

FUENTES_SISTEMA = main.c menu.c fechas.c utils.c clientes.c \
                  vehiculos.c alquileres.c movimientos.c reportes.c
FUENTES_DATOS   = generar_datos.c fechas.c utils.c clientes.c \
                  vehiculos.c alquileres.c movimientos.c
FUENTES_TESTS   = tests_validaciones.c fechas.c utils.c

CABECERAS = $(wildcard *.h)

.PHONY: all sistema datos tests ejecutar probar limpiar

all: sistema datos tests

sistema: $(BIN_SISTEMA)

datos: $(BIN_DATOS)

tests: $(BIN_TESTS)

$(BIN_SISTEMA): $(FUENTES_SISTEMA) $(CABECERAS)
	$(CC) $(CFLAGS) -o $@ $(FUENTES_SISTEMA)

$(BIN_DATOS): $(FUENTES_DATOS) $(CABECERAS)
	$(CC) $(CFLAGS) -o $@ $(FUENTES_DATOS)

$(BIN_TESTS): $(FUENTES_TESTS) $(CABECERAS)
	$(CC) $(CFLAGS) -o $@ $(FUENTES_TESTS)

ejecutar: $(BIN_SISTEMA)
	./$(BIN_SISTEMA)

probar: $(BIN_TESTS)
	./$(BIN_TESTS)

limpiar:
	-del /f /q $(BIN_SISTEMA) $(BIN_DATOS) $(BIN_TESTS) 2>nul || rm -f $(BIN_SISTEMA) $(BIN_DATOS) $(BIN_TESTS)

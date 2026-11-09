#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include <stdbool.h>
#include <stddef.h>
#include "comun.h"

bool cargarMovimientos(Movimiento** movimientos, int* cantidad);
bool guardarMovimientos(const Movimiento* movimientos, int cantidad);
void liberarMovimientos(Movimiento* movimientos);
int proximoNumeroMovimiento(void);
const char* nombreArchivoMovimientos(int dni);
bool regenerarArchivoMovimientosCliente(int dni, char* error, size_t tamError);
bool regenerarTodosLosArchivosTexto(char* error, size_t tamError);
bool registrarMovimiento(int numAlquiler, const char* tipo, double monto,
                         int fecha, const char* medio, char* error, size_t tamError);

void registrarPago(void);
void listarMovimientos(void);
void submenuMovimientos(void);

#endif /* MOVIMIENTOS_H */

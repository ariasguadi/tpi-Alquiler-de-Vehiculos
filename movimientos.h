#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include <string>
#include <vector>

#include "comun.h"

bool cargarMovimientos(std::vector<Movimiento>& movimientos);
bool guardarMovimientos(const std::vector<Movimiento>& movimientos);
int proximoNumeroMovimiento();
std::string nombreArchivoMovimientos(int dni);
bool regenerarArchivoMovimientosCliente(int dni, std::string& error);
bool regenerarTodosLosArchivosTexto(std::string& error);
bool registrarMovimiento(int numAlquiler, const std::string& tipo, double monto,
                         int fecha, const std::string& medio, std::string& error);

void registrarPago();
void listarMovimientos();
void submenuMovimientos();

#endif /* MOVIMIENTOS_H */

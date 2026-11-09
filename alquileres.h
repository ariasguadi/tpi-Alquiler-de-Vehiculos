#ifndef ALQUILERES_H
#define ALQUILERES_H

#include <stdbool.h>
#include <stddef.h>
#include "comun.h"

bool cargarAlquileres(Alquiler** alquileres, int* cantidad);
bool guardarAlquileres(const Alquiler* alquileres, int cantidad);
void liberarAlquileres(Alquiler* alquileres);
int buscarIndiceAlquiler(const Alquiler* alquileres, int cantidad, int numero);
bool buscarAlquilerPorNumero(int numero, Alquiler* alquiler);
bool buscarAlquilerActivoPorPatente(const char* patente, Alquiler* alquiler);
int clienteTieneAlquilerActivo(int dni);
bool vehiculoTieneAlquilerActivo(const char* patente);
bool validarDatosAlquiler(const Alquiler* alquiler, char* error, size_t tamError);
int proximoNumeroAlquiler(void);

void imprimirEncabezadoAlquileres(void);
void imprimirFilaAlquiler(const Alquiler* alquiler, const Cliente* clientes, int cantClientes,
                          const Vehiculo* vehiculos, int cantVehiculos);

void realizarAlquiler(void);
void modificarAlquiler(void);
void listarAlquileres(void);
void registrarDevolucion(void);
void cancelarAlquiler(void);
void submenuAlquileres(void);

#endif /* ALQUILERES_H */

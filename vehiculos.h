#ifndef VEHICULOS_H
#define VEHICULOS_H

#include <stdbool.h>
#include <stddef.h>
#include "comun.h"

bool cargarVehiculos(Vehiculo** vehiculos, int* cantidad);
bool guardarVehiculos(const Vehiculo* vehiculos, int cantidad);
void liberarVehiculos(Vehiculo* vehiculos);
int buscarIndiceVehiculo(const Vehiculo* vehiculos, int cantidad, const char* patente);
bool buscarVehiculoPorPatente(const char* patente, Vehiculo* vehiculo);
bool existeVehiculo(const char* patente);
bool validarDatosVehiculo(const Vehiculo* vehiculo, char* error, size_t tamError);
bool vehiculoSePuedeDarDeBaja(const char* patente, char* error, size_t tamError);
bool cambiarEstadoVehiculo(const char* patente, const char* nuevoEstado, char* error, size_t tamError);

void registrarVehiculo(void);
void modificarVehiculo(void);
void buscarVehiculo(void);
void listarVehiculos(void);
void submenuVehiculos(void);

#endif /* VEHICULOS_H */

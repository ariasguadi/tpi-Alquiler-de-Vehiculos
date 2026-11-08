#ifndef VEHICULOS_H
#define VEHICULOS_H

#include <string>
#include <vector>

#include "comun.h"

bool cargarVehiculos(std::vector<Vehiculo>& vehiculos);
bool guardarVehiculos(const std::vector<Vehiculo>& vehiculos);
int buscarIndiceVehiculo(const std::vector<Vehiculo>& vehiculos, const std::string& patente);
bool buscarVehiculoPorPatente(const std::string& patente, Vehiculo& vehiculo);
bool existeVehiculo(const std::string& patente);
bool validarDatosVehiculo(const Vehiculo& vehiculo, std::string& error);
bool vehiculoSePuedeDarDeBaja(const std::string& patente, std::string& error);
bool cambiarEstadoVehiculo(const std::string& patente, const std::string& nuevoEstado,
                           std::string& error);

void registrarVehiculo();
void modificarVehiculo();
void buscarVehiculo();
void listarVehiculos();
void submenuVehiculos();

#endif /* VEHICULOS_H */

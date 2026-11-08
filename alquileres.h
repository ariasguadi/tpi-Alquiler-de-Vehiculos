#ifndef ALQUILERES_H
#define ALQUILERES_H

#include <string>
#include <vector>

#include "comun.h"

bool cargarAlquileres(std::vector<Alquiler>& alquileres);
bool guardarAlquileres(const std::vector<Alquiler>& alquileres);
int buscarIndiceAlquiler(const std::vector<Alquiler>& alquileres, int numero);
bool buscarAlquilerPorNumero(int numero, Alquiler& alquiler);
bool buscarAlquilerActivoPorPatente(const std::string& patente, Alquiler& alquiler);
int clienteTieneAlquilerActivo(int dni);
bool vehiculoTieneAlquilerActivo(const std::string& patente);
bool validarDatosAlquiler(const Alquiler& alquiler, std::string& error);
int proximoNumeroAlquiler();

void imprimirEncabezadoAlquileres();
void imprimirFilaAlquiler(const Alquiler& alquiler, const std::vector<Cliente>& clientes,
                          const std::vector<Vehiculo>& vehiculos);

void realizarAlquiler();
void modificarAlquiler();
void listarAlquileres();
void registrarDevolucion();
void cancelarAlquiler();
void submenuAlquileres();

#endif /* ALQUILERES_H */

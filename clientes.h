#ifndef CLIENTES_H
#define CLIENTES_H

#include <string>
#include <vector>

#include "comun.h"

bool cargarClientes(std::vector<Cliente>& clientes);
bool guardarClientes(const std::vector<Cliente>& clientes);
int buscarIndiceCliente(const std::vector<Cliente>& clientes, int dni);
bool buscarClientePorDNI(int dni, Cliente& cliente);
bool existeCliente(int dni);
bool existeEmail(const std::string& email, int dniExcluido);
bool clienteSePuedeDarDeBaja(int dni, std::string& error);
bool validarDatosCliente(const Cliente& cliente, std::string& error);

void registrarCliente();
void modificarCliente();
void consultarCliente();
void listarClientes();
void darDeBajaCliente();
void submenuClientes();

#endif /* CLIENTES_H */

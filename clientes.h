#ifndef CLIENTES_H
#define CLIENTES_H

#include <stdbool.h>
#include <stddef.h>
#include "comun.h"

bool cargarClientes(Cliente** clientes, int* cantidad);
bool guardarClientes(const Cliente* clientes, int cantidad);
void liberarClientes(Cliente* clientes);
int buscarIndiceCliente(const Cliente* clientes, int cantidad, int dni);
bool buscarClientePorDNI(int dni, Cliente* cliente);
bool existeCliente(int dni);
bool existeEmail(const char* email, int dniExcluido);
bool existeTelefono(const char* telefono, int dniExcluido);
bool clienteSePuedeDarDeBaja(int dni, char* error, size_t tamError);
bool validarDatosCliente(const Cliente* cliente, char* error, size_t tamError);

void registrarCliente(void);
void modificarCliente(void);
void consultarCliente(void);
void listarClientes(void);
void darDeBajaCliente(void);
void submenuClientes(void);

#endif /* CLIENTES_H */

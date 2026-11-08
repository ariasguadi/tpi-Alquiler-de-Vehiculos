#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <cerrno>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

/*
 * Plantillas genericas para persistencia binaria.
 * Cada registro se escribe tal cual ocupa en memoria (structs triviales).
 */

inline bool existeArchivo(const std::string& ruta) {
    std::FILE* archivo = std::fopen(ruta.c_str(), "rb");
    if (archivo != NULL) {
        std::fclose(archivo);
        return true;
    }
    return false;
}

template <typename T>
bool cargarRegistros(const std::string& ruta, std::vector<T>& registros, std::string& error) {
    registros.clear();
    std::ifstream archivo(ruta.c_str(), std::ios::binary);
    if (!archivo.is_open()) {
        errno = 0;
        std::FILE* prueba = std::fopen(ruta.c_str(), "rb");
        if (prueba == NULL && errno == ENOENT) {
            return true; /* Archivo inexistente: se considera una lista vacia. */
        }
        if (prueba != NULL) {
            std::fclose(prueba);
        }
        error = "No se pudo abrir el archivo " + ruta + " para lectura.";
        return false;
    }

    T registro;
    while (archivo.read(reinterpret_cast<char*>(&registro), sizeof(T))) {
        registros.push_back(registro);
    }

    if (archivo.bad()) {
        error = "Ocurrio un error de lectura en el archivo " + ruta + ".";
        return false;
    }
    if (archivo.eof() && archivo.gcount() > 0) {
        error = "El archivo " + ruta + " contiene un registro incompleto o corrupto.";
        return false;
    }
    return true;
}

template <typename T>
bool guardarRegistros(const std::string& ruta, const std::vector<T>& registros, std::string& error) {
    std::ofstream archivo(ruta.c_str(), std::ios::binary | std::ios::trunc);
    if (!archivo.is_open()) {
        error = "No se pudo abrir el archivo " + ruta + " para escritura.";
        return false;
    }

    if (!registros.empty()) {
        archivo.write(reinterpret_cast<const char*>(&registros[0]),
                      static_cast<std::streamsize>(registros.size() * sizeof(T)));
    }
    archivo.flush();
    if (!archivo.good()) {
        error = "No se pudo escribir correctamente en el archivo " + ruta + ".";
        return false;
    }
    archivo.close();
    return true;
}

#endif /* ARCHIVOS_H */

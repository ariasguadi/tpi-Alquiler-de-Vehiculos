#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/*
 * Persistencia binaria en C puro utilizando exclusivamente:
 * FILE*, fopen, fread, fwrite, fseek, ftell y fclose.
 */

static inline int existeArchivo(const char* ruta) {
    if (!ruta) return 0;
    FILE* archivo = fopen(ruta, "rb");
    if (archivo != NULL) {
        fclose(archivo);
        return 1;
    }
    return 0;
}

static inline long contarRegistrosArchivo(const char* ruta, size_t tamRegistro) {
    if (!ruta || tamRegistro == 0) return 0;
    FILE* archivo = fopen(ruta, "rb");
    if (archivo == NULL) return 0;
    fseek(archivo, 0, SEEK_END);
    long tamBytes = ftell(archivo);
    fclose(archivo);
    if (tamBytes <= 0) return 0;
    return tamBytes / (long)tamRegistro;
}

static inline bool cargarRegistrosGenerico(const char* ruta, size_t tamRegistro,
                                          void** registros, int* cantidad,
                                          char* error, size_t tamError) {
    if (!ruta || !registros || !cantidad || tamRegistro == 0) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "Parametros invalidos para cargar registros.");
        }
        return false;
    }
    *registros = NULL;
    *cantidad = 0;

    FILE* archivo = fopen(ruta, "rb");
    if (!archivo) {
        if (errno == ENOENT) {
            /* Archivo inexistente: se considera una lista vacia sin error. */
            return true;
        }
        if (error && tamError > 0) {
            snprintf(error, tamError, "No se pudo abrir el archivo %s para lectura.", ruta);
        }
        return false;
    }

    if (fseek(archivo, 0, SEEK_END) != 0) {
        fclose(archivo);
        if (error && tamError > 0) {
            snprintf(error, tamError, "Error al buscar el fin del archivo %s.", ruta);
        }
        return false;
    }

    long tamBytes = ftell(archivo);
    if (tamBytes < 0) {
        fclose(archivo);
        if (error && tamError > 0) {
            snprintf(error, tamError, "Error al determinar el tamano del archivo %s.", ruta);
        }
        return false;
    }

    if (tamBytes == 0) {
        fclose(archivo);
        return true;
    }

    if (tamBytes % (long)tamRegistro != 0) {
        fclose(archivo);
        if (error && tamError > 0) {
            snprintf(error, tamError, "El archivo %s contiene un registro incompleto o corrupto.", ruta);
        }
        return false;
    }

    rewind(archivo);

    int n = (int)(tamBytes / (long)tamRegistro);
    void* buffer = malloc((size_t)tamBytes);
    if (!buffer) {
        fclose(archivo);
        if (error && tamError > 0) {
            snprintf(error, tamError, "Memoria insuficiente para cargar el archivo %s.", ruta);
        }
        return false;
    }

    size_t leidos = fread(buffer, tamRegistro, (size_t)n, archivo);
    fclose(archivo);

    if (leidos != (size_t)n) {
        free(buffer);
        if (error && tamError > 0) {
            snprintf(error, tamError, "Ocurrio un error de lectura en el archivo %s.", ruta);
        }
        return false;
    }

    *registros = buffer;
    *cantidad = n;
    return true;
}

static inline bool guardarRegistrosGenerico(const char* ruta, size_t tamRegistro,
                                           const void* registros, int cantidad,
                                           char* error, size_t tamError) {
    if (!ruta || tamRegistro == 0) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "Parametros invalidos para guardar registros.");
        }
        return false;
    }

    FILE* archivo = fopen(ruta, "wb");
    if (!archivo) {
        if (error && tamError > 0) {
            snprintf(error, tamError, "No se pudo abrir el archivo %s para escritura.", ruta);
        }
        return false;
    }

    if (cantidad > 0 && registros != NULL) {
        size_t escritos = fwrite(registros, tamRegistro, (size_t)cantidad, archivo);
        if (escritos != (size_t)cantidad) {
            fclose(archivo);
            if (error && tamError > 0) {
                snprintf(error, tamError, "No se pudo escribir correctamente en el archivo %s.", ruta);
            }
            return false;
        }
    }

    fclose(archivo);
    return true;
}

#endif /* ARCHIVOS_H */

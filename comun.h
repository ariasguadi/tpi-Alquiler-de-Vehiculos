#ifndef COMUN_H
#define COMUN_H

/*
 * Sistema de Gestion de Alquiler de Vehiculos
 * Trabajo Practico Integrador - Programacion Estructurada
 *
 * Constantes, estados y estructuras compartidas por todos los modulos.
 * Estandar C99 / C11.
 */

#include <stddef.h>
#include <stdbool.h>

/* Estados de vehiculo */
#define EST_VEH_DISPONIBLE   "Disponible"
#define EST_VEH_ALQUILADO    "Alquilado"
#define EST_VEH_BAJA         "Baja"
#define EST_VEH_MANT         "En mantenimiento"

/* Estados de alquiler */
#define EST_ALQ_ACTIVO       "Activo"
#define EST_ALQ_FINALIZADO   "Finalizado"
#define EST_ALQ_CANCELADO    "Cancelado"

/* Tipos de movimiento */
#define MOV_PAGO             "Pago"
#define MOV_AJUSTE           "Ajuste"

/* Estados de cliente */
#define CLI_ACTIVO           1
#define CLI_INACTIVO         0

/* Medios de pago */
#define MEDIO_EFECTIVO       "Efectivo"
#define MEDIO_TARJETA        "Tarjeta"
#define MEDIO_TRANSFERENCIA  "Transferencia"
#define MEDIO_NINGUNO        "-"

/* Longitudes maximas de cadenas (incluyendo el terminador nulo) */
#define LON_PATENTE          15
#define LON_ESTADO           25
#define LON_TIPO_MOV         15
#define LON_MEDIO_PAGO       25
#define LON_NOMBRE           60
#define LON_TEL              25
#define LON_EMAIL            80
#define LON_DIR              100
#define LON_MARCA            40
#define LON_MODELO           40
#define LON_TIPO_VEH         30

/* Tolerancia para comparar importes en punto flotante */
#define TOLERANCIA_SALDO     0.005

/* Rangos globales de DNI */
#define DNI_MIN              1000000
#define DNI_MAX              50000000

/* Rangos de validacion de vehiculos */
#define ANIO_MIN_VEHICULO    1990
#define CAPACIDAD_MIN_VEHICULO 1
#define CAPACIDAD_MAX_VEHICULO 60
#define PRECIO_DIA_MAXIMO    10000000.0

/* Reglas de fechas para alquileres */
#define DIAS_MAX_ALQUILER    365
#define DIAS_MAX_INICIO_FUTURO 30

/* Nombres de archivos binarios */
#define ARCH_CLIENTES        "clientes.dat"
#define ARCH_VEHICULOS       "vehiculos.dat"
#define ARCH_ALQUILERES      "alquileres.dat"
#define ARCH_MOVIMIENTOS     "movimientos.dat"

/* -------------------------------------------------------------------------
 * Estructuras principales
 * ------------------------------------------------------------------------- */

typedef struct Cliente {
    int  dni;                         /* Clave primaria: 7 u 8 digitos */
    char apellidoNombre[LON_NOMBRE];
    char telefono[LON_TEL];
    char email[LON_EMAIL];
    char direccion[LON_DIR];
    int  fechaAlta;                   /* Formato AAAAMMDD */
    int  estado;                      /* CLI_ACTIVO o CLI_INACTIVO */
} Cliente;

typedef struct Vehiculo {
    char   patente[LON_PATENTE];      /* AAA123, AA123AA, 123ABC o A123ABC */
    char   marca[LON_MARCA];
    char   modelo[LON_MODELO];
    int    anio;                      /* 1990 .. anio actual + 1 */
    char   tipo[LON_TIPO_VEH];        /* Auto, Camioneta, Utilitario, Moto o 4x4 */
    int    capacidad;                 /* Personas: 1..60 */
    double precioDia;                 /* Tarifa diaria: > 0 y <= 10000000 */
    char   estado[LON_ESTADO];        /* Disponible, Alquilado, Baja, En mantenimiento */
} Vehiculo;

typedef struct Alquiler {
    int    numAlquiler;               /* Clave primaria autoincremental */
    int    dni;                       /* Clave foranea a Cliente */
    char   patente[LON_PATENTE];      /* Clave foranea a Vehiculo */
    int    fechaInicio;               /* AAAAMMDD */
    int    fechaPrevista;             /* AAAAMMDD = fechaInicio + cantDias */
    int    fechaEfectiva;             /* AAAAMMDD; 0 mientras este activo */
    int    cantDias;                  /* Dias contratados: > 0 */
    double importeTotal;              /* Incluye recargos por demora */
    double importePagado;             /* Total abonado hasta la fecha */
    char   estado[LON_ESTADO];        /* Activo, Finalizado, Cancelado */
} Alquiler;

typedef struct Movimiento {
    int    numMovimiento;             /* Clave primaria autoincremental */
    int    numAlquiler;               /* Clave foranea a Alquiler */
    char   tipoMov[LON_TIPO_MOV];     /* Pago o Ajuste */
    double monto;                     /* Positivo o negativo segun el ajuste */
    int    fecha;                     /* AAAAMMDD */
    int    dni;                       /* Clave foranea a Cliente */
    char   patente[LON_PATENTE];      /* Clave foranea a Vehiculo */
    char   medioPago[LON_MEDIO_PAGO]; /* Efectivo, Tarjeta, Transferencia, - */
} Movimiento;

#endif /* COMUN_H */

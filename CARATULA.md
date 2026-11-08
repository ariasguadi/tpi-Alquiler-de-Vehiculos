# Caratula

```text
+----------------------------------------------------------------------------------+
|                                                                                  |
|                              [UNIVERSIDAD GASTON DACHARY]                        |
|                                                                                  |
|                          [Ing. informática | Sede Campus]                        |
|                                                                                  |
|                                                                                  |
|                                                                                  |
|                                                                                  |
|                                                                                  |
|                     TRABAJO PRACTICO INTEGRADOR (TPI)                            |
|                                                                                  |
|              SISTEMA DE GESTION DE ALQUILER DE VEHICULOS                         |
|                                                                                  |
|                                                                                  |
|                                                                                  |
|                     Materia: [Programación Estructurada]                         |
|                     Docente: [NOMBRE DEL DOCENTE]                                |
|                     Comision: [A]                                                |
|                     Anio lectivo: [2026]                                         |
|                                                                                  |
|                                                                                  |
|                                                                                  |
|                     Integrantes:                                                 |
|                     - [ARIAS, GUADALUPE] - Legajo: [68227]                       |
|                     - [DUARTE, EMILY] - Legajo: [NUMERO]                         |
|                     - [VEDOYA CANTONI, ANA PAZ] - Legajo: [NUMERO]               |
|                                                                                  |
|                                                                                  |
|                     Fecha de entrega: [08/11/2026]                               |
|                                                                                  |
+----------------------------------------------------------------------------------+
```


## Datos tecnicos

| Campo | Valor |
|---|---|
| Lenguaje | C++ (C++11) |
| Paradigma | Programacion estructurada |
| Compilador | TDM-GCC 9.2.0 (Dev-C++) |
| Sistema operativo | Windows |
| Persistencia | Archivos binarios y archivos de texto |
| Ejecutable principal | `sistema.exe` |
| Generador de datos | `generar_datos.exe` |
| Pruebas | `tests_validaciones.exe` |

## Descripcion breve

El sistema administra clientes, vehiculos, alquileres, devoluciones y pagos de
una empresa de alquiler de vehiculos. Incluye validacion de patentes argentinas
en formato viejo y Mercosur, control de disponibilidad, calculo de recargos por
demora, control de saldos, generacion de archivos de texto por cliente y diez
reportes de consulta y estadisticas.

## Contenido de la entrega

- Codigo fuente completo del sistema.
- `sistema.exe`: aplicacion principal.
- `generar_datos.exe`: generador del dataset de prueba.
- `tests_validaciones.exe`: pruebas unitarias.
- `README.md`: guia de compilacion, ejecucion y uso.
- `DOCUMENTACION.md`: documentacion tecnica ampliada.
- `compilar.bat`, `Makefile` y `Makefile.win`: opciones de compilacion.
- Archivos de datos de prueba: `clientes.dat`, `vehiculos.dat`,
  `alquileres.dat`, `movimientos.dat` y `movimientos_DNI.txt`.

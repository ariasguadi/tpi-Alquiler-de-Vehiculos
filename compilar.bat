@echo off
setlocal

rem ---------------------------------------------------------------------------
rem Compilacion del Sistema de Gestion de Alquiler de Vehiculos en C (C99/C11).
rem Ajuste la variable GCC si el compilador esta instalado en otra ruta.
rem ---------------------------------------------------------------------------

set "GCC=C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin\gcc.exe"
set "CFLAGS=-std=c99 -Wall -Wextra -O2"

if not exist "%GCC%" goto sincompilador

echo ==========================================
echo  Compilando sistema.exe...
echo ==========================================
"%GCC%" %CFLAGS% -o sistema.exe main.c menu.c fechas.c utils.c clientes.c vehiculos.c alquileres.c movimientos.c reportes.c
if errorlevel 1 goto error

echo.
echo ==========================================
echo  Compilando generar_datos.exe...
echo ==========================================
"%GCC%" %CFLAGS% -o generar_datos.exe generar_datos.c fechas.c utils.c clientes.c vehiculos.c alquileres.c movimientos.c
if errorlevel 1 goto error

echo.
echo ==========================================
echo  Compilando tests_validaciones.exe...
echo ==========================================
"%GCC%" %CFLAGS% -o tests_validaciones.exe tests_validaciones.c fechas.c utils.c
if errorlevel 1 goto error

echo.
echo ==========================================
echo  Compilacion completada correctamente.
echo ==========================================
echo Ejecute generar_datos.exe para cargar el dataset de prueba
echo y luego sistema.exe para iniciar la aplicacion.
echo.
pause
exit /b 0

:sincompilador
echo [ERROR] No se encontro el compilador:
echo "%GCC%"
echo Edite compilar.bat y ajuste la variable GCC.
pause
exit /b 1

:error
echo.
echo [ERROR] Ocurrio un fallo durante la compilacion.
pause
exit /b 1

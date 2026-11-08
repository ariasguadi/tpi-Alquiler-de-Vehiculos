@echo off
setlocal

rem ---------------------------------------------------------------------------
rem Compilacion del Sistema de Gestion de Alquiler de Vehiculos.
rem Ajuste la variable GXX si el compilador esta instalado en otra ruta.
rem ---------------------------------------------------------------------------

set "GXX=C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin\g++.exe"
set "CXXFLAGS=-std=c++11 -Wall -Wextra -O2"

if not exist "%GXX%" goto sincompilador

echo ==========================================
echo  Compilando sistema.exe...
echo ==========================================
"%GXX%" %CXXFLAGS% -o sistema.exe main.cpp menu.cpp fechas.cpp utils.cpp clientes.cpp vehiculos.cpp alquileres.cpp movimientos.cpp reportes.cpp
if errorlevel 1 goto error

echo.
echo ==========================================
echo  Compilando generar_datos.exe...
echo ==========================================
"%GXX%" %CXXFLAGS% -o generar_datos.exe generar_datos.cpp fechas.cpp utils.cpp clientes.cpp vehiculos.cpp alquileres.cpp movimientos.cpp
if errorlevel 1 goto error

echo.
echo ==========================================
echo  Compilando tests_validaciones.exe...
echo ==========================================
"%GXX%" %CXXFLAGS% -o tests_validaciones.exe tests_validaciones.cpp fechas.cpp utils.cpp
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
echo "%GXX%"
echo Edite compilar.bat y ajuste la variable GXX.
pause
exit /b 1

:error
echo.
echo [ERROR] Fallo la compilacion. Revise los mensajes anteriores.
pause
exit /b 1

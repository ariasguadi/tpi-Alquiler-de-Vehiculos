# Guia para ejecutar el sistema en Embarcadero Dev-C++

Esta guia explica como abrir, compilar y ejecutar el proyecto en
**Embarcadero Dev-C++**, usando **C++11**.

El codigo tambien compila con C++98, por lo que no deberia dar errores por
`constexpr`, `nullptr`, `std::to_string` ni `std::isfinite`: esas
caracteristicas ya fueron reemplazadas por equivalentes compatibles.

---

## 1. Opcion rapida: no usar el IDE

La forma mas simple de compilar todo es ejecutar `compilar.bat`:

```bat
compilar.bat
```

Luego:

```bat
generar_datos.exe
sistema.exe
```

Y para las pruebas:

```bat
tests_validaciones.exe
```

`compilar.bat` compila los tres ejecutables con `-std=c++11`.

---

## 2. Opcion recomendada en Dev-C++: abrir el proyecto ya preparado

En la carpeta del proyecto se incluye:

```text
SistemaAlquiler.dev
```

Pasos:

1. Abrir **Embarcadero Dev-C++**.
2. Ir a **File** -> **Open Project or File**.
3. Seleccionar `SistemaAlquiler.dev`.
4. Verificar que en el panel del proyecto aparezcan los archivos.
5. Presionar **F11** (Compile & Run) o usar **Execute** -> **Compile & Run**.
6. Si todo esta bien, se genera `sistema.exe` y se ejecuta.

El proyecto ya tiene configurado:

- Compilador C++.
- Estandar `-std=c++11`.
- Makefile `Makefile.dev`.
- Salida `sistema.exe`.

Si tu version de Dev-C++ no abre el archivo `.dev`, usar la opcion manual de
la seccion 3.

---

## 3. Opcion manual: crear el proyecto en Dev-C++

### 3.1 Crear el proyecto principal

1. Abrir Dev-C++.
2. **File** -> **New** -> **Project**.
3. Elegir **Console Application**.
4. Elegir **C++ project** (no C project).
5. Nombre: `sistema`.
6. Ubicacion: la carpeta del proyecto.
7. Aceptar.

Si Dev-C++ crea un `main.cpp` de ejemplo:

1. En el panel del proyecto, hacer clic derecho sobre ese `main.cpp`.
2. Elegir **Remove file** (quitar del proyecto, no borrar del disco).
3. Agregar el `main.cpp` real del proyecto.

### 3.2 Archivos que debe llevar el proyecto principal

Agregar al proyecto **solamente** estos archivos:

| Archivo | Tipo | Funcion |
|---|---|---|
| `main.cpp` | Fuente | Punto de entrada del sistema. |
| `menu.cpp` | Fuente | Menu principal. |
| `fechas.cpp` | Fuente | Validacion y calculo de fechas. |
| `utils.cpp` | Fuente | Entrada de datos y validaciones. |
| `clientes.cpp` | Fuente | Gestion de clientes. |
| `vehiculos.cpp` | Fuente | Gestion de vehiculos. |
| `alquileres.cpp` | Fuente | Alquileres y devoluciones. |
| `movimientos.cpp` | Fuente | Pagos y movimientos. |
| `reportes.cpp` | Fuente | Reportes. |
| `comun.h` | Cabecera | Structs y constantes. |
| `fechas.h` | Cabecera | Funciones de fechas. |
| `utils.h` | Cabecera | Funciones de utilidades. |
| `archivos.h` | Cabecera | Lectura/escritura binaria. |
| `clientes.h` | Cabecera | Funciones de clientes. |
| `vehiculos.h` | Cabecera | Funciones de vehiculos. |
| `alquileres.h` | Cabecera | Funciones de alquileres. |
| `movimientos.h` | Cabecera | Funciones de movimientos. |
| `reportes.h` | Cabecera | Funciones de reportes. |
| `menu.h` | Cabecera | Funcion del menu. |

Para agregarlos:

1. Clic derecho sobre el nombre del proyecto en el panel izquierdo.
2. Elegir **Add to Project...**.
3. Seleccionar los archivos de la tabla.
4. Aceptar.

### 3.3 Archivos que NO deben ir en el proyecto principal

No agregar estos archivos al proyecto `sistema`:

```text
generar_datos.cpp
tests_validaciones.cpp
```

Motivo: cada uno tiene su propia funcion `main`. Si se agregan al proyecto
principal, el enlazador dara un error de **multiples definiciones de main**.

Esos dos programas se compilan aparte, como se explica en la seccion 5.

---

## 4. Configurar C++11 en Dev-C++

### Opcion A: configuracion global

1. **Tools** -> **Compiler Options**.
2. Pestania **Settings**.
3. Entrar a **Code Generation**.
4. En **Language standard (-std)**, elegir **ISO C++11**.
5. Aceptar.

### Opcion B: opciones del proyecto

1. **Project** -> **Project Options**.
2. Pestania **Parameters**.
3. En **C++ compiler**, agregar:

```text
-std=c++11
```

4. Aceptar.

### Opcion C: si no encontras la opcion

El codigo tambien compila con C++98, asi que no es obligatorio cambiar el
estandar. Igualmente, se recomienda usar C++11.

---

## 5. Compilar el generador de datos y las pruebas

Estos dos programas tienen su propio `main`, por lo que necesitan proyectos
separados o el script `compilar.bat`.

### 5.1 Generador de datos

Crear un proyecto nuevo llamado `generar_datos` y agregar:

```text
generar_datos.cpp
fechas.cpp
utils.cpp
clientes.cpp
vehiculos.cpp
alquileres.cpp
movimientos.cpp
comun.h
fechas.h
utils.h
archivos.h
clientes.h
vehiculos.h
alquileres.h
movimientos.h
```

Salida esperada: `generar_datos.exe`.

### 5.2 Pruebas de validacion

Crear un proyecto nuevo llamado `tests_validaciones` y agregar:

```text
tests_validaciones.cpp
fechas.cpp
utils.cpp
comun.h
fechas.h
utils.h
```

Salida esperada: `tests_validaciones.exe`.

### 5.3 Alternativa recomendada

Usar `compilar.bat`, que genera los tres ejecutables de una vez:

```bat
compilar.bat
```

---

## 6. Orden de ejecucion

```text
1. compilar.bat          (o F11 en Dev-C++ para el sistema principal)
2. generar_datos.exe     (carga el dataset de prueba)
3. sistema.exe           (abre el menu)
4. tests_validaciones.exe (verifica las validaciones)
```

Importante: `generar_datos.exe` debe ejecutarse desde la misma carpeta donde
estan los archivos `.dat` y los `.txt`. Si se ejecuta desde otra carpeta, el
programa no encontrara los datos.

---

## 7. Problemas frecuentes

### 7.1 Error en `constexpr int CLI_ACTIVO`

Ese error ya no deberia aparecer porque el codigo ahora usa `const int`. Si
aparece, estas compilando una copia vieja del archivo `comun.h`. Verifica que
Dev-C++ este usando el `comun.h` de esta carpeta.

### 7.2 Error de multiples definiciones de `main`

Estas agregando `generar_datos.cpp` o `tests_validaciones.cpp` al proyecto
principal. Quitalos del proyecto `sistema` y compilalos por separado.

### 7.3 Dev-C++ no encuentra el compilador

1. **Tools** -> **Compiler Options**.
2. Verificar que el compilador sea el de Dev-C++:
   `C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64`.
3. Si no aparece, reinstalar Dev-C++ o agregar la ruta manualmente.

### 7.4 El programa no encuentra los archivos `.dat`

Ejecutar el `.exe` desde la carpeta del proyecto. En Dev-C++, verificar que el
directorio de trabajo sea la carpeta donde estan `clientes.dat`,
`vehiculos.dat`, `alquileres.dat` y `movimientos.dat`.

### 7.5 La consola se cierra muy rapido

Ejecutar el programa desde una terminal:

```bat
sistema.exe
```

El modo normal del sistema ya incluye pausas. El modo `--test` no las incluye
porque esta pensado para pruebas automaticas.

### 7.6 Dev-C++ sobrescribe `Makefile.win`

Si el IDE genera su propio `Makefile.win`, no afecta a `compilar.bat`. Para
volver a compilar todo desde la terminal, usar:

```bat
compilar.bat
```

---

## 8. Resumen de archivos del proyecto principal

```text
main.cpp
menu.cpp
fechas.cpp
utils.cpp
clientes.cpp
vehiculos.cpp
alquileres.cpp
movimientos.cpp
reportes.cpp

comun.h
menu.h
fechas.h
utils.h
archivos.h
clientes.h
vehiculos.h
alquileres.h
movimientos.h
reportes.h
```

Con esos 19 archivos se genera `sistema.exe`.

Los archivos `generar_datos.cpp` y `tests_validaciones.cpp` se compilan aparte.

# Sistema de Gestion de Alquiler de Vehiculos

Trabajo Practico Integrador de **Programacion Estructurada** desarrollado en **C++**.
La aplicacion funciona por consola y permite administrar clientes, vehiculos,
alquileres, devoluciones, pagos y reportes, con persistencia en archivos
binarios y un archivo de texto de movimientos por cliente.

El codigo utiliza `struct` y funciones libres. No emplea clases, objetos,
herencia ni ningun otro mecanismo de programacion orientada a objetos.

---

## 1. Caracteristicas principales

- Alta, modificacion, consulta, listado y baja/reactivacion de clientes.
- Consulta de cliente con historial completo de alquileres.
- Alta, modificacion, busqueda por patente y listado de vehiculos.
- Alta, modificacion, listado y cancelacion de alquileres.
- Registro de devoluciones con calculo automatico de recargos por demora.
- Registro de pagos con control de saldo pendiente y medio de pago.
- Historial de movimientos (pagos y ajustes) por cliente.
- Diez reportes de consulta y estadisticas.
- Validacion de patentes argentinas en los formatos viejo (`AAA123`), Mercosur
  (`AA123AA`), moto vieja (`123ABC`) y moto Mercosur (`A123ABC`), con
  normalizacion automatica a mayusculas.
- Validacion de DNI, nombres, telefonos, e-mails, direcciones, fechas,
  importes, estados y demas datos de entrada.
- Persistencia binaria y regeneracion automatica de archivos de texto.
- Dataset de prueba precargado y verificacion automatica del mismo.
- Bateria de 169 pruebas unitarias de validaciones y fechas.

---

## 2. Requisitos

- Sistema operativo Windows (probado en Windows de 64 bits).
- Compilador de C++ con soporte para C++11.
- El codigo evita caracteristicas de C++14/17 y tambien compila con C++98,
  por lo que funciona incluso con configuraciones antiguas de Dev-C++.
- No requiere bibliotecas externas.

Configuracion utilizada durante el desarrollo:

| Elemento | Valor |
|---|---|
| Compilador | `g++.exe` (TDM-GCC 9.2.0, 64 bits) |
| Entorno | Embarcadero Dev-C++ |
| Ruta | `C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin\g++.exe` |
| Estandar | C++11 |
| Opciones | `-Wall -Wextra -O2` |

---

## 3. Compilacion

### Opcion A: script para Windows (recomendada)

Ejecutar desde la carpeta del proyecto:

```bat
compilar.bat
```

### Opcion A2: proyecto de Embarcadero Dev-C++

Abrir el archivo:

```text
SistemaAlquiler.dev
```

y presionar **F11** (Compile & Run). El proyecto ya usa `-std=c++11` y el
Makefile `Makefile.dev`. La guia paso a paso esta en `GUIA_DEVCPP.md`.

El script compila los tres ejecutables con el compilador de Dev-C++:

- `sistema.exe`: aplicacion principal.
- `generar_datos.exe`: generador del dataset de prueba.
- `tests_validaciones.exe`: pruebas unitarias.

Si el compilador esta instalado en otra ruta, editar la variable `GXX` al
comienzo de `compilar.bat`.

### Opcion B: Makefile para Windows

Con `mingw32-make` de Dev-C++:

```bat
"C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin\mingw32-make.exe" -f Makefile.win
```

Objetivos disponibles:

```bat
mingw32-make -f Makefile.win            REM compila todo
mingw32-make -f Makefile.win sistema    REM solo sistema.exe
mingw32-make -f Makefile.win datos      REM solo generar_datos.exe
mingw32-make -f Makefile.win tests      REM solo tests_validaciones.exe
mingw32-make -f Makefile.win probar     REM compila y ejecuta las pruebas
mingw32-make -f Makefile.win limpiar    REM elimina los .exe generados
```

### Opcion C: Makefile generico

Si `g++` esta en el `PATH`:

```bat
mingw32-make
```

### Opcion D: comandos manuales

```bat
g++ -std=c++11 -Wall -Wextra -O2 -o sistema.exe main.cpp menu.cpp fechas.cpp utils.cpp clientes.cpp vehiculos.cpp alquileres.cpp movimientos.cpp reportes.cpp

g++ -std=c++11 -Wall -Wextra -O2 -o generar_datos.exe generar_datos.cpp fechas.cpp utils.cpp clientes.cpp vehiculos.cpp alquileres.cpp movimientos.cpp

g++ -std=c++11 -Wall -Wextra -O2 -o tests_validaciones.exe tests_validaciones.cpp fechas.cpp utils.cpp
```

La compilacion fue verificada sin errores ni advertencias con las opciones
indicadas.

---

## 4. Ejecucion

### 4.1 Generar el dataset de prueba

Antes de usar la aplicacion por primera vez:

```bat
generar_datos.exe
```

Este programa:

1. Sobrescribe `clientes.dat`, `vehiculos.dat`, `alquileres.dat` y
   `movimientos.dat` con los datos de prueba.
2. Regenera los archivos `movimientos_DNI.txt` de todos los clientes.
3. Verifica cantidades, estados, deuda total y total cobrado.
4. Informa `DATASET GENERADO Y VERIFICADO EXITOSAMENTE.` si todo es correcto.

**Atencion:** `generar_datos.exe` reinicia la base de datos. No debe ejecutarse
si se desea conservar informacion cargada manualmente.

### 4.2 Iniciar la aplicacion

```bat
sistema.exe
```

La aplicacion muestra una pantalla de bienvenida y luego el menu principal.

### 4.3 Modo de prueba automatico

```bat
sistema.exe --test
```

En modo de prueba no se limpia la pantalla ni se realizan pausas. Esto permite
alimentar entradas por redireccionamiento y ejecutar pruebas de integracion.

### 4.4 Ayuda

```bat
sistema.exe --help
```

---

## 5. Pruebas

### 5.1 Pruebas unitarias

```bat
tests_validaciones.exe
```

Resultado esperado:

```text
Total de pruebas: 169
Fallos:           0
RESULTADO: TODAS LAS PRUEBAS PASARON.
```

Las pruebas cubren:

- Patentes viejas, Mercosur y de moto: casos validos, invalidos, minusculas,
  espacios, guiones y puntos.
- Normalizacion de patentes y de e-mails.
- Validacion de DNI.
- Nombres y apellidos, incluidos acentos y enie.
- Telefonos con 8 a 15 digitos y separadores.
- E-mails con un solo `@`, dominio con punto y sin `..`.
- Direcciones con al menos una letra.
- Marcas, modelos y tipos de vehiculo.
- Rangos de anio, capacidad y precio por dia.
- Estados de vehiculo, alquiler, movimiento y medios de pago.
- Fechas: bisiestos, meses, dias, suma, diferencia, rangos, pasado/futuro y
  formateo `DD/MM/AAAA`.
- Utilidades de texto: recorte, mayusculas, minusculas, division y copia segura.

### 5.2 Verificacion del dataset

El propio `generar_datos.exe` comprueba:

- 10 clientes, 10 vehiculos, 15 alquileres y 23 movimientos.
- 5 vehiculos disponibles, 3 alquilados, 1 en mantenimiento y 1 de baja.
- 3 alquileres activos, 9 finalizados y 3 cancelados.
- 18 pagos y 5 ajustes.
- Deuda total: `$1.000.000,00`.
- Total cobrado: `$2.581.000,00`.
- Existencia de los 10 archivos de texto por cliente.

---

## 6. Estructura del proyecto

| Archivo | Descripcion |
|---|---|
| `comun.h` | Constantes, estados, nombres de archivos y structs compartidos. |
| `fechas.h` / `fechas.cpp` | Validacion y aritmetica de fechas en formato `AAAAMMDD`. |
| `utils.h` / `utils.cpp` | Entrada de datos, normalizacion y validaciones. |
| `archivos.h` | Plantillas genericas de lectura/escritura binaria. |
| `clientes.h` / `clientes.cpp` | Gestion de clientes e historial. |
| `vehiculos.h` / `vehiculos.cpp` | Gestion de vehiculos. |
| `alquileres.h` / `alquileres.cpp` | Alquileres, devoluciones y cancelaciones. |
| `movimientos.h` / `movimientos.cpp` | Pagos, ajustes y archivos de texto. |
| `reportes.h` / `reportes.cpp` | Los diez reportes del sistema. |
| `menu.h` / `menu.cpp` | Menu principal y navegacion. |
| `main.cpp` | Punto de entrada y opciones de linea de comandos. |
| `generar_datos.cpp` | Generador y verificador del dataset de prueba. |
| `tests_validaciones.cpp` | Pruebas unitarias de validaciones y fechas. |
| `compilar.bat` | Compilacion automatica en Windows. |
| `Makefile` | Makefile generico para MinGW/GCC. |
| `Makefile.win` | Makefile para Dev-C++ / TDM-GCC. |
| `Makefile.dev` | Makefile del proyecto `SistemaAlquiler.dev`. |
| `SistemaAlquiler.dev` | Proyecto listo para abrir en Embarcadero Dev-C++. |
| `GUIA_DEVCPP.md` | Guia paso a paso para Dev-C++. |
| `README.md` | Este archivo. |
| `DOCUMENTACION.md` | Documentacion tecnica ampliada. |
| `CARATULA.md` | Caratula institucional con datos a completar. |

---

## 7. Archivos de datos

### 7.1 Archivos binarios

| Archivo | Contenido |
|---|---|
| `clientes.dat` | Registros `Cliente`. |
| `vehiculos.dat` | Registros `Vehiculo`. |
| `alquileres.dat` | Registros `Alquiler`. |
| `movimientos.dat` | Registros `Movimiento`. |

Cada archivo se escribe como una secuencia de registros de tamano fijo
(`sizeof(struct)`). Se cargan completos en memoria, se modifican y se vuelven
a escribir. Si un archivo no existe, se considera una lista vacia. Si existe
pero no puede abrirse o contiene un registro incompleto, se informa el error y
la operacion se cancela.

### 7.2 Archivos de texto por cliente

Para cada cliente se genera un archivo:

```text
movimientos_DNI.txt
```

Ejemplo: `movimientos_30111222.txt`.

Formato de cada linea:

```text
numMov;numAlquiler;Tipo;monto;fecha;DNI;patente;medioPago
```

Ejemplo:

```text
1;1;Pago;100000.00;20260105;30111222;ABC123;Efectivo
```

Los archivos se regeneran automaticamente cada vez que se registra un pago o
un ajuste, y tambien al ejecutar `generar_datos.exe`.

---

## 8. Validaciones destacadas

### 8.1 Patentes argentinas

Se aceptan exclusivamente cuatro formatos:

| Formato | Patron | Ejemplo |
|---|---|---|
| Auto viejo | 3 letras + 3 digitos | `ABC123` |
| Auto Mercosur | 2 letras + 3 digitos + 2 letras | `AB123CD` |
| Moto vieja | 3 digitos + 3 letras | `123ABC` |
| Moto Mercosur | 1 letra + 3 digitos + 3 letras | `A123BCD` |

Antes de validar, la entrada se normaliza:

- Se eliminan espacios, guiones y puntos.
- Todas las letras se convierten a mayusculas.

Por lo tanto, `ab 123 cd`, `AB-123-CD` y `ab.123.cd` se almacenan como
`AB123CD`. Una patente con formato incorrecto es rechazada con un mensaje
claro y no se registra.

### 8.2 Otras validaciones

- **DNI:** 7 u 8 digitos, entre `1000000` y `99999999`, unico en el sistema.
- **Apellido y nombre:** al menos 2 caracteres, solo letras (incluye acentos y
  enie), espacios, apostrofo, guion y punto; debe tener al menos una letra.
- **Telefono:** entre 8 y 15 digitos reales; admite espacios, `-`, `(` y `)`, y
  un `+` solo al inicio. Ejemplo valido: `3764-100001`.
- **E-mail:** un solo `@`, parte local y dominio no vacios, dominio con punto,
  sin espacios ni `..`, y caracteres permitidos. Se guarda en minusculas y no
  puede repetirse entre clientes.
- **Direccion:** entre 5 y 99 caracteres, con al menos una letra; no se aceptan
  cadenas solo numericas o solo de simbolos.
- **Marca:** al menos 2 caracteres con al menos una letra; admite espacios,
  guion, punto y `&`.
- **Modelo:** al menos 2 caracteres con al menos una letra o digito; admite
  espacios, guion, punto, barra y `&`. Se aceptan modelos como `208`, `C4` y
  `Cronos`.
- **Tipo:** solo `Auto`, `Camioneta`, `Utilitario`, `Moto` o `4x4`, elegido
  desde un menu.
- **Anio:** entre 1990 y el anio actual + 1.
- **Capacidad:** entre 1 y 60 personas.
- **Precio por dia:** mayor a cero y hasta `10000000`.
- **Fecha de alta:** valida, no posterior a hoy.
- **Fecha de inicio de alquiler:** entre hoy y hoy + 30 dias.
- **Fecha de devolucion y de pago:** entre el inicio del alquiler y hoy.
- **Estados:** solo los valores definidos por el sistema.
- **Importes:** mayores a cero y finitos.

---

## 9. Menu principal

```text
1. Clientes
2. Vehiculos
3. Alquileres
4. Devoluciones
5. Pagos
6. Consultas y Reportes
0. Salir
```

### 9.1 Clientes

```text
1. Registrar cliente
2. Modificar cliente
3. Consultar cliente (con historial)
4. Listar clientes
5. Dar de baja / reactivar cliente
```

### 9.2 Vehiculos

```text
1. Registrar vehiculo
2. Modificar vehiculo
3. Buscar vehiculo por patente
4. Listar vehiculos
```

### 9.3 Alquileres

```text
1. Realizar nuevo alquiler
2. Modificar alquiler
3. Listar alquileres
4. Cancelar alquiler
```

### 9.4 Pagos

```text
1. Registrar pago
2. Listar todos los movimientos
```

### 9.5 Consultas y Reportes

```text
 1. Vehiculos disponibles
 2. Vehiculos alquilados
 3. Alquileres por rango de fechas
 4. Alquileres por cliente
 5. Alquileres por vehiculo
 6. Alquileres por estado
 7. Alquileres por importe
 8. Pagos realizados por periodo
 9. Clientes con deuda
10. Estadisticas de la empresa
```

---

## 10. Reglas de negocio principales

- Un cliente puede tener como maximo un alquiler activo a la vez.
- Un vehiculo puede tener como maximo un alquiler activo a la vez.
- Solo se alquilan vehiculos en estado `Disponible`.
- Solo se alquila a clientes en estado `Activo`.
- La fecha prevista se calcula como `fechaInicio + cantDias`.
- El importe total inicial es `cantDias * precioDia`.
- La fecha de inicio de un alquiler nuevo debe estar entre hoy y hoy + 30 dias.
- La devolucion no puede ser anterior al inicio del alquiler ni posterior a hoy.
- Si la devolucion es posterior a la fecha prevista, se cobra un recargo por
  cada dia de demora a la tarifa diaria del vehiculo.
- No se puede registrar un pago superior al saldo pendiente.
- No se puede pagar un alquiler cancelado o ya saldado.
- La fecha de pago no puede ser anterior al inicio del alquiler ni posterior a
  hoy.
- Al modificar un alquiler, la nueva fecha de inicio no puede quedar posterior
  a un pago ya registrado.
- Solo se pueden modificar o cancelar alquileres activos.
- No se puede dar de baja a un cliente con alquiler activo.
- No se puede cambiar a `Baja` ni `En mantenimiento` un vehiculo con alquiler
  activo.
- Al finalizar o cancelar un alquiler, el vehiculo vuelve a `Disponible`.
- El reporte de clientes con deuda agrupa todos los alquileres de cada cliente
  en una sola fila, informando los numeros de alquiler, total, pagado y saldo.
  Un alquiler cancelado nunca genera deuda.

---

## 11. Dataset de prueba

El dataset incluye:

| Entidad | Cantidad |
|---|---|
| Clientes | 10 |
| Vehiculos | 10 |
| Alquileres | 15 |
| Movimientos | 23 (18 pagos + 5 ajustes) |
| Archivos de texto | 10 |

Distribucion de estados:

| Entidad | Estado | Cantidad |
|---|---|---|
| Vehiculos | Disponible | 5 |
| Vehiculos | Alquilado | 3 |
| Vehiculos | En mantenimiento | 1 |
| Vehiculos | Baja | 1 |
| Alquileres | Activo | 3 |
| Alquileres | Finalizado | 9 |
| Alquileres | Cancelado | 3 |

Totales economicos esperados:

- Deuda total pendiente: `$1.000.000,00`.
- Total cobrado: `$2.581.000,00`.

El detalle completo de clientes, vehiculos, alquileres y movimientos se
encuentra en `DOCUMENTACION.md`.

---

## 12. Notas tecnicas

- El codigo fue adaptado para compilar con C++11 y tambien con C++98. No usa
  `constexpr`, `nullptr`, `std::to_string` ni `std::isfinite`; esas funciones
  se reemplazaron por equivalentes compatibles.
- Todo el texto de la interfaz y del codigo fuente es ASCII, sin acentos ni
  caracteres especiales, para evitar problemas de codificacion en la consola
  de Windows.
- Los archivos binarios dependen del compilador y de la plataforma (tamano de
  `int`, `double`, alineacion y orden de bytes). Deben generarse y leerse con
  el mismo ejecutable o con uno compilado de la misma forma.
- La carga completa en memoria y la reescritura total son adecuadas para el
  volumen de datos de este trabajo practico.
- Las operaciones que modifican alquileres y vehiculos en conjunto utilizan
  una estrategia de respaldo y restauracion para evitar estados inconsistentes
  si falla el guardado.

---

## 13. Documentacion adicional

- `DOCUMENTACION.md`: diseno, modelo de datos, modulos, validaciones, formato
  de archivos, dataset completo y resultados de pruebas.
- `CARATULA.md`: caratula para la entrega, con espacios para completar los
  datos institucionales y de los integrantes.

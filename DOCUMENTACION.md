# Documentacion Tecnica

## Sistema de Gestion de Alquiler de Vehiculos

Trabajo Practico Integrador de Programacion Estructurada
Lenguaje: C++ (estandar C++11)
Interfaz: consola de Windows
Persistencia: archivos binarios y archivos de texto

---

## 1. Introduccion

El presente documento describe el diseno, la implementacion y el funcionamiento
del Sistema de Gestion de Alquiler de Vehiculos. El sistema permite administrar
el ciclo completo de una empresa de alquiler:

- Registro y mantenimiento de clientes.
- Registro y mantenimiento de vehiculos.
- Confeccion y seguimiento de alquileres.
- Devoluciones con recargo por demora.
- Cobros y registro de movimientos.
- Consultas y reportes estadisticos.

El programa fue desarrollado con un enfoque de **programacion estructurada**.
Se utilizan estructuras de datos (`struct`) y funciones libres organizadas por
modulos. No se utilizan clases, objetos, herencia, polimorfismo ni plantillas de
programacion orientada a objetos.

---

## 2. Objetivos

### 2.1 Objetivo general

Construir una aplicacion de consola que permita gestionar de forma persistente
los clientes, los vehiculos y los alquileres de una empresa de alquiler de
vehiculos, aplicando validaciones que garanticen la consistencia de los datos.

### 2.2 Objetivos especificos

- Implementar altas, bajas, modificaciones y consultas de clientes.
- Implementar altas, bajas logicas, modificaciones y consultas de vehiculos.
- Implementar alquileres, devoluciones, cancelaciones y pagos.
- Validar patentes argentinas en sus formatos viejo, Mercosur y de moto.
- Validar DNI, nombres, telefonos, e-mails, direcciones y fechas.
- Impedir operaciones inconsistentes, como alquilar un vehiculo no disponible
  o registrar un pago superior al saldo pendiente.
- Persistir la informacion en archivos binarios.
- Generar un archivo de texto de movimientos por cada cliente.
- Proveer un dataset de prueba representativo.
- Proveer pruebas automatizadas que verifiquen las validaciones y la logica.

---

## 3. Enfoque de programacion estructurada

El codigo se organizo en modulos con responsabilidades bien definidas. Cada
modulo expone sus operaciones mediante un encabezado (`.h`) y las implementa en
un archivo fuente (`.cpp`). Las estructuras de datos son publicas y se pasan a
las funciones por referencia o por valor segun corresponda.

Caracteristicas del diseno:

- **Structs**: `Cliente`, `Vehiculo`, `Alquiler` y `Movimiento`.
- **Funciones libres**: no hay metodos ni clases.
- **Variables globales minimas**: solo `modoPrueba` y `finEntrada`, usadas para
  controlar la interfaz durante las pruebas automaticas.
- **Constantes centralizadas** en `comun.h`.
- **Separacion de responsabilidades**: persistencia, validacion, logica de
  negocio, interfaz y reportes estan separados.
- **Reutilizacion**: las plantillas de `archivos.h` evitan duplicar el codigo de
  lectura y escritura binaria.
- **Compatibilidad**: el codigo evita caracteristicas de C++14/17 y compila
  tanto con C++11 como con C++98. Esto permite usarlo con configuraciones
  antiguas de Dev-C++ sin modificar el codigo.

---

## 4. Organizacion modular

| Modulo | Responsabilidad |
|---|---|
| `comun.h` | Constantes, estados, nombres de archivos y structs. |
| `fechas` | Validacion de fechas, bisiestos, conversion y aritmetica. |
| `utils` | Entrada de datos, normalizacion, validaciones y utilidades de texto. |
| `archivos.h` | Plantillas genericas de carga y guardado binario. |
| `clientes` | Persistencia y operaciones sobre clientes. |
| `vehiculos` | Persistencia y operaciones sobre vehiculos. |
| `alquileres` | Alquileres, devoluciones y cancelaciones. |
| `movimientos` | Pagos, ajustes y archivos de texto por cliente. |
| `reportes` | Los diez reportes del sistema. |
| `menu` | Menu principal y navegacion. |
| `main` | Punto de entrada y opciones `--test` y `--help`. |
| `generar_datos` | Generacion y verificacion del dataset de prueba. |
| `tests_validaciones` | Pruebas unitarias de validaciones, fechas y utilidades. |

Dependencias principales:

```text
main -> menu -> clientes, vehiculos, alquileres, movimientos, reportes
clientes -> archivos, utils, fechas, alquileres
vehiculos -> archivos, utils, alquileres
alquileres -> archivos, utils, fechas, clientes, vehiculos, movimientos
movimientos -> archivos, utils, fechas, alquileres, clientes
reportes -> clientes, vehiculos, alquileres, movimientos, fechas, utils
```

---

## 5. Modelo de datos

### 5.1 Cliente

```cpp
struct Cliente {
    int  dni;
    char apellidoNombre[60];
    char telefono[25];
    char email[80];
    char direccion[100];
    int  fechaAlta;   /* AAAAMMDD */
    int  estado;      /* 1 = Activo, 0 = Inactivo */
};
```

| Campo | Tipo | Descripcion |
|---|---|---|
| `dni` | `int` | Clave primaria. 7 u 8 digitos. Unico. |
| `apellidoNombre` | `char[60]` | Apellido y nombre del cliente. |
| `telefono` | `char[25]` | Telefono de contacto. |
| `email` | `char[80]` | Correo electronico. |
| `direccion` | `char[100]` | Domicilio. |
| `fechaAlta` | `int` | Fecha de alta en formato `AAAAMMDD`. |
| `estado` | `int` | `CLI_ACTIVO` (1) o `CLI_INACTIVO` (0). |

El DNI es la clave de busqueda y no se modifica una vez registrado. La baja de
un cliente es logica: cambia el estado a inactivo sin eliminar el registro.

### 5.2 Vehiculo

```cpp
struct Vehiculo {
    char   patente[15];
    char   marca[40];
    char   modelo[40];
    int    anio;
    char   tipo[30];
    int    capacidad;
    double precioDia;
    char   estado[25];
};
```

| Campo | Tipo | Descripcion |
|---|---|---|
| `patente` | `char[15]` | Clave primaria. Formato `AAA123`, `AA123AA`, `123ABC` o `A123ABC`. |
| `marca` | `char[40]` | Marca del vehiculo. |
| `modelo` | `char[40]` | Modelo del vehiculo. |
| `anio` | `int` | Anio de fabricacion. |
| `tipo` | `char[30]` | Auto, Camioneta, Utilitario, etc. |
| `capacidad` | `int` | Cantidad de personas (1 a 100). |
| `precioDia` | `double` | Tarifa diaria de alquiler. |
| `estado` | `char[25]` | Disponible, Alquilado, Baja o En mantenimiento. |

La patente es la clave de busqueda y no se modifica. La baja de un vehiculo es
logica: se utiliza el estado `Baja`.

### 5.3 Alquiler

```cpp
struct Alquiler {
    int    numAlquiler;
    int    dni;
    char   patente[15];
    int    fechaInicio;
    int    fechaPrevista;
    int    fechaEfectiva;
    int    cantDias;
    double importeTotal;
    double importePagado;
    char   estado[25];
};
```

| Campo | Tipo | Descripcion |
|---|---|---|
| `numAlquiler` | `int` | Clave primaria autoincremental. |
| `dni` | `int` | Clave foranea al cliente. |
| `patente` | `char[15]` | Clave foranea al vehiculo. |
| `fechaInicio` | `int` | Fecha de inicio (`AAAAMMDD`). |
| `fechaPrevista` | `int` | `fechaInicio + cantDias` (`AAAAMMDD`). |
| `fechaEfectiva` | `int` | Fecha de devolucion; `0` si esta activo. |
| `cantDias` | `int` | Dias contratados (1 a 365). |
| `importeTotal` | `double` | Importe total, incluidos recargos. |
| `importePagado` | `double` | Total abonado hasta el momento. |
| `estado` | `char[25]` | Activo, Finalizado o Cancelado. |

### 5.4 Movimiento

```cpp
struct Movimiento {
    int    numMovimiento;
    int    numAlquiler;
    char   tipoMov[15];
    double monto;
    int    fecha;
    int    dni;
    char   patente[15];
    char   medioPago[25];
};
```

| Campo | Tipo | Descripcion |
|---|---|---|
| `numMovimiento` | `int` | Clave primaria autoincremental. |
| `numAlquiler` | `int` | Clave foranea al alquiler. |
| `tipoMov` | `char[15]` | `Pago` o `Ajuste`. |
| `monto` | `double` | Importe del movimiento. Puede ser negativo en ajustes. |
| `fecha` | `int` | Fecha del movimiento (`AAAAMMDD`). |
| `dni` | `int` | DNI del cliente asociado al alquiler. |
| `patente` | `char[15]` | Patente del vehiculo asociado. |
| `medioPago` | `char[25]` | Efectivo, Tarjeta, Transferencia o `-`. |

### 5.5 Relaciones

```text
Cliente (1) -------- (N) Alquiler (1) -------- (N) Movimiento
Vehiculo (1) ------- (N) Alquiler
```

- Un cliente puede tener muchos alquileres historicos.
- Un cliente puede tener como maximo un alquiler activo.
- Un vehiculo puede tener muchos alquileres historicos.
- Un vehiculo puede tener como maximo un alquiler activo.
- Cada movimiento pertenece a un alquiler y hereda su DNI y su patente.

---

## 6. Persistencia

### 6.1 Archivos binarios

| Archivo | Struct | Contenido |
|---|---|---|
| `clientes.dat` | `Cliente` | Todos los clientes. |
| `vehiculos.dat` | `Vehiculo` | Todos los vehiculos. |
| `alquileres.dat` | `Alquiler` | Todos los alquileres. |
| `movimientos.dat` | `Movimiento` | Todos los movimientos. |

Cada archivo es una secuencia de registros de tamano fijo. No tiene cabecera ni
separadores. El tamano de cada registro esta dado por `sizeof(struct)` e incluye
el relleno (padding) que agregue el compilador.

La lectura y escritura se implementa con plantillas en `archivos.h`:

- `cargarRegistros<T>(ruta, vector, error)`: abre el archivo en modo binario,
  lee registros completos y los agrega al vector. Si el archivo no existe, se
  considera una lista vacia. Si hay un error de apertura o un registro
  incompleto, devuelve `false` y completa un mensaje de error.
- `guardarRegistros<T>(ruta, vector, error)`: abre el archivo en modo binario
  con truncamiento, escribe todos los registros y verifica el estado del flujo.

### 6.2 Estrategia de carga y guardado

Cada operacion sigue el mismo esquema:

1. Cargar el archivo completo en un `std::vector`.
2. Buscar el registro por su clave.
3. Validar la operacion.
4. Modificar el vector en memoria.
5. Reescribir el archivo completo.

Esta estrategia es simple, segura y adecuada para el volumen de datos de un
trabajo practico. Evita el manejo de posiciones fisicas y permite mantener la
logica de negocio separada de la persistencia.

### 6.3 Archivos de texto por cliente

Por cada cliente se genera un archivo:

```text
movimientos_<DNI>.txt
```

Ejemplo: `movimientos_30111222.txt`.

Cada linea representa un movimiento con el siguiente formato:

```text
numMov;numAlquiler;Tipo;monto;fecha;DNI;patente;medioPago
```

Ejemplo:

```text
1;1;Pago;100000.00;20260105;30111222;ABC123;Efectivo
```

Caracteristicas:

- Separador: punto y coma (`;`).
- `monto`: dos decimales fijos, con punto como separador decimal.
- `fecha`: entero en formato `AAAAMMDD`.
- Los ajustes pueden tener monto negativo.
- Los archivos se regeneran completos cada vez que se registra un pago o un
  ajuste, y tambien al ejecutar `generar_datos.exe`.
- Un cliente sin movimientos tiene un archivo vacio.

Funciones principales:

- `nombreArchivoMovimientos(dni)`: construye la ruta del archivo.
- `regenerarArchivoMovimientosCliente(dni, error)`: regenera un archivo.
- `regenerarTodosLosArchivosTexto(error)`: regenera los archivos de todos los
  clientes y de todos los DNI presentes en movimientos.

### 6.4 Integridad y recuperacion

Las operaciones que afectan simultaneamente a alquileres y vehiculos utilizan
una estrategia de respaldo:

1. Se copian los vectores originales de alquileres y vehiculos.
2. Se aplican los cambios en memoria.
3. Se guardan ambos archivos mediante `guardarAlquilerYVehiculos`.
4. Si falla cualquiera de los dos guardados, se restauran los vectores
   originales y se intenta dejar ambos archivos en su estado anterior.

La funcion `guardarAlquilerYVehiculos` se utiliza en:

- Alta de alquiler.
- Modificacion de alquiler.
- Devolucion.
- Cancelacion.

En el registro de pagos se aplica una recuperacion equivalente: si falla el
guardado del movimiento, se revierte el `importePagado` del alquiler.

---

## 7. Funcionalidades

### 7.1 Gestion de clientes

#### Registrar cliente

1. Solicita DNI.
2. Verifica que el DNI no exista.
3. Solicita apellido y nombre, telefono, e-mail, direccion y fecha de alta.
4. Valida cada dato con los validadores de formato y verifica que el e-mail no
   este repetido.
5. Guarda el cliente con estado `Activo`.

La fecha de alta se ingresa manualmente en formato `AAAAMMDD`. Debe ser una
fecha real y no puede ser posterior a la fecha actual.

#### Modificar cliente

1. Solicita DNI y verifica que exista.
2. Permite modificar apellido y nombre, telefono, e-mail y direccion.
3. Si el usuario presiona ENTER, conserva el valor anterior.
4. Cada dato nuevo pasa por los mismos validadores que en el alta; el e-mail se
   compara contra los demas clientes excluyendo el DNI actual.
5. Valida los datos resultantes y guarda.

El DNI, la fecha de alta y el estado no se modifican desde esta opcion. El
estado se cambia con la opcion de baja/reactivacion.

#### Consultar cliente

Muestra los datos personales y el historial completo de alquileres del cliente,
incluyendo numero, patente, fechas, dias, importes y estado. Si el cliente no
tiene alquileres, informa esa situacion.

#### Listar clientes

Muestra todos los clientes con DNI, apellido y nombre, telefono, e-mail,
direccion, fecha de alta y estado.

#### Dar de baja / reactivar cliente

1. Solicita DNI y verifica que exista.
2. Rechaza la operacion si el cliente tiene un alquiler activo.
3. Muestra el estado actual y el estado propuesto.
4. Pide confirmacion.
5. Cambia el estado entre `Activo` e `Inactivo`.

La baja es logica: el cliente y su historial se conservan.

### 7.2 Gestion de vehiculos

#### Registrar vehiculo

1. Solicita patente y la normaliza.
2. Verifica que la patente tenga uno de los cuatro formatos validos y que no
   exista.
3. Solicita marca, modelo, anio, tipo, capacidad y precio por dia.
4. El tipo se elige de un menu: `Auto`, `Camioneta`, `Utilitario`, `Moto` o
   `4x4`.
5. Valida los datos y los rangos de anio, capacidad y precio.
6. Guarda el vehiculo con estado `Disponible`.

#### Modificar vehiculo

1. Solicita patente y verifica que exista.
2. Permite modificar marca, modelo, anio, tipo y capacidad.
3. Si el vehiculo no tiene alquiler activo, permite modificar el precio por dia
   y el estado (Disponible, En mantenimiento o Baja).
4. Si el vehiculo tiene alquiler activo, conserva el precio y no permite
   pasarlo a `Disponible`, `Baja` ni `En mantenimiento`.
5. Antes de aceptar `Baja` o `En mantenimiento`, `vehiculoSePuedeDarDeBaja`
   verifica que no exista un alquiler activo.
6. Valida y guarda.

La patente no se modifica porque es la clave primaria.

#### Buscar vehiculo por patente

Muestra todos los datos del vehiculo. Si no existe, informa el error.

#### Listar vehiculos

Muestra todos los vehiculos con patente, marca, modelo, anio, tipo, capacidad,
precio por dia y estado.

### 7.3 Gestion de alquileres

#### Realizar nuevo alquiler

Validaciones previas:

- El DNI debe tener formato valido y corresponder a un cliente existente.
- El cliente debe estar activo.
- El cliente no debe tener otro alquiler activo.
- La patente debe tener formato valido y corresponder a un vehiculo existente.
- El vehiculo debe estar en estado `Disponible`.
- El vehiculo no debe tener otro alquiler activo.
- La fecha de inicio debe estar entre hoy y hoy + 30 dias.
- La cantidad de dias debe estar entre 1 y 365.

Datos calculados:

- `fechaPrevista = sumarDias(fechaInicio, cantDias)`.
- `importeTotal = cantDias * precioDia`.
- `importePagado = 0`.
- `estado = Activo`.
- El vehiculo pasa a estado `Alquilado`.

#### Modificar alquiler

Solo se pueden modificar alquileres activos. Permite:

- Cambiar el vehiculo asignado.
- Cambiar la fecha de inicio.
- Cambiar la cantidad de dias.

Reglas:

- El nuevo vehiculo debe existir y estar disponible.
- Al cambiar de vehiculo, el anterior vuelve a `Disponible` y el nuevo pasa a
  `Alquilado`.
- La nueva fecha de inicio debe estar entre hoy y hoy + 30 dias.
- Si el alquiler ya tiene pagos registrados, la nueva fecha de inicio no puede
  ser posterior a la fecha del primer pago. Si esa restriccion deja el rango
  vacio, la modificacion se rechaza.
- La fecha prevista se recalcula.
- El nuevo importe total se calcula con la tarifa del vehiculo final.
- El nuevo importe total no puede ser menor que el importe ya pagado.
- Si el nuevo total es menor que lo pagado pero dentro de la tolerancia, se
  ajusta al importe pagado.
- Si cambia el importe total, se registra un movimiento de ajuste con la fecha
  actual del sistema.
- Antes de guardar, `validarDatosAlquiler` verifica fechas, importes y
  coherencia entre el estado y la fecha efectiva de devolucion.

#### Listar alquileres

Muestra todos los alquileres con numero, cliente, patente, fechas, dias,
importes y estado.

#### Cancelar alquiler

1. Solicita numero de alquiler y verifica que exista.
2. Solo permite cancelar alquileres activos.
3. Muestra los datos y pide confirmacion.
4. Ajusta el importe total al importe ya pagado, de modo que el alquiler
   cancelado no genere deuda.
5. Cambia el estado a `Cancelado`.
6. Devuelve el vehiculo al estado `Disponible`.

Los pagos ya realizados se conservan registrados. No se permiten nuevos pagos
sobre un alquiler cancelado.

### 7.4 Devoluciones

La devolucion se puede identificar por patente del vehiculo o por numero de
alquiler.

Validaciones:

- Debe existir un alquiler activo para la patente o el numero indicado.
- La fecha de devolucion debe ser una fecha real.
- La fecha de devolucion debe estar entre la fecha de inicio del alquiler y la
  fecha actual, inclusive. Si el alquiler comienza en el futuro, la devolucion
  se rechaza porque todavia no puede haberse producido.

Calculo del recargo:

```text
diasAdicionales = diasEntre(fechaPrevista, fechaDevolucion)
si diasAdicionales > 0:
    recargo = diasAdicionales * precioDia
```

El importe total final es:

```text
importeTotal = importeTotal + recargo
```

Luego:

- Se registra `fechaEfectiva`.
- El alquiler pasa a `Finalizado`.
- El vehiculo vuelve a `Disponible`.
- Si el recargo es mayor que cero, se registra un movimiento de tipo `Ajuste`.

Una devolucion anticipada no genera descuento ni devolucion de dinero: el
importe total no se reduce.

### 7.5 Pagos y movimientos

#### Registrar pago

1. Solicita numero de alquiler y verifica que exista.
2. Rechaza alquileres cancelados.
3. Calcula el saldo pendiente: `importeTotal - importePagado`.
4. Si el saldo es cero (dentro de la tolerancia), informa que ya esta saldado.
5. Solicita el monto.
6. Rechaza montos mayores al saldo pendiente (con tolerancia) y ajusta montos
   que difieran solo por redondeo.
7. Solicita el medio de pago: Efectivo, Tarjeta o Transferencia.
8. Solicita la fecha de pago y exige que este entre el inicio del alquiler y
   hoy, inclusive. Si el alquiler comienza en el futuro, no permite registrar
   el pago.
9. Actualiza `importePagado` y guarda el alquiler.
10. Registra el movimiento de tipo `Pago`.
11. Si falla el registro del movimiento, revierte el importe pagado.

Se admiten pagos sobre alquileres activos o finalizados que tengan saldo
pendiente.

#### Listar movimientos

Muestra todos los movimientos con numero, alquiler, tipo, monto, fecha, DNI,
patente y medio de pago.

#### Movimientos de ajuste

Los ajustes se generan automaticamente en las siguientes situaciones:

- Recargo por devolucion tardia (monto positivo).
- Cambio de importe por modificacion de alquiler (positivo o negativo).
- Ajuste de trazabilidad al cancelar un alquiler (monto negativo, igual al
  importe estimado que se deja sin efecto).

Los ajustes no tienen medio de pago; se almacena `-`.

### 7.6 Reportes

| N. | Reporte | Descripcion |
|---|---|---|
| 1 | Vehiculos disponibles | Lista los vehiculos en estado `Disponible` con patente, marca, modelo, anio, tipo, capacidad y precio por dia. |
| 2 | Vehiculos alquilados | Lista los alquileres activos con patente, marca y modelo, cliente, fecha de inicio y fecha prevista. |
| 3 | Alquileres por rango de fechas | Solicita fecha inicial y final; lista los alquileres cuya `fechaInicio` cae dentro del rango, inclusive. |
| 4 | Alquileres por cliente | Solicita DNI; verifica que el cliente exista y lista todos sus alquileres. |
| 5 | Alquileres por vehiculo | Solicita patente; verifica que el vehiculo exista y lista todos sus alquileres historicos. |
| 6 | Alquileres por estado | Permite elegir Activos, Finalizados o Cancelados y lista los alquileres correspondientes. |
| 7 | Alquileres por importe | Solicita un importe de referencia y un criterio (mayor o menor); lista los alquileres que cumplen la condicion. |
| 8 | Pagos por periodo | Solicita un rango de fechas; lista solo los movimientos de tipo `Pago` del periodo y acumula el total. |
| 9 | Clientes con deuda | Agrupa por cliente todos los alquileres no cancelados con saldo pendiente. Muestra una sola fila por cliente con los numeros de alquiler, total, pagado y saldo acumulado, y la deuda total general. |
| 10 | Estadisticas | Resume cantidades de vehiculos por estado, alquileres por estado, total cobrado y total pendiente. |

Observacion sobre el reporte 9: la deuda se acumula por cliente. Si un cliente
tiene varios alquileres con saldo pendiente, aparece una sola vez y la columna
`ALQUILERES` lista todos los numeros involucrados. Los alquileres cancelados se
excluyen porque su importe total se ajusta al importe pagado. La deuda total
general se calcula sumando los saldos acumulados.

---

## 8. Validaciones

### 8.1 Patentes argentinas

Se aceptan cuatro formatos:

| Formato | Expresion | Largo | Ejemplo |
|---|---|---|---|
| Auto viejo | 3 letras + 3 digitos | 6 | `ABC123` |
| Auto Mercosur | 2 letras + 3 digitos + 2 letras | 7 | `AB123CD` |
| Moto vieja | 3 digitos + 3 letras | 6 | `123ABC` |
| Moto Mercosur | 1 letra + 3 digitos + 3 letras | 7 | `A123BCD` |

La funcion `esPatenteValida` exige mayusculas y no admite separadores;
`validarPatente` se conserva como envoltorio de compatibilidad. Para el ingreso
por consola, `leerPatente` aplica primero `normalizarPatente`:

1. Elimina espacios, tabulaciones, guiones y puntos.
2. Convierte todas las letras a mayusculas.
3. Valida el resultado contra los cuatro formatos.

Ejemplos de normalizacion:

| Entrada | Resultado | Valida |
|---|---|---|
| `abc123` | `ABC123` | Si |
| `ab 123 cd` | `AB123CD` | Si |
| `AB-123-CD` | `AB123CD` | Si |
| `ab.123.cd` | `AB123CD` | Si |
| `123abc` | `123ABC` | Si |
| `a 123 bcd` | `A123BCD` | Si |
| `ABC-12` | `ABC12` | No |
| `AB123C` | `AB123C` | No |
| `ABC1234` | `ABC1234` | No |
| `AB123CDE` | `AB123CDE` | No |

Una patente invalida se rechaza con el mensaje:

```text
[ERROR] Patente invalida. Formatos: AAA123 (auto viejo), AA123AA (auto Mercosur), 123ABC (moto vieja) o A123ABC (moto Mercosur). Reintente.
```

La patente se almacena normalizada y se usa como clave primaria del vehiculo.
No se permite registrar dos vehiculos con la misma patente.

### 8.2 Validaciones de cliente

| Dato | Regla |
|---|---|
| DNI | Entero entre `1000000` y `99999999`; 7 u 8 digitos; unico. |
| Apellido y nombre | Entre 2 y 59 caracteres; solo letras (incluye acentos y enie), espacios, apostrofo, guion y punto; debe contener al menos una letra. |
| Telefono | Entre 8 y 24 caracteres; entre 8 y 15 digitos reales; admite espacios, `-`, `(` y `)`, y un `+` solo al inicio. Ejemplo: `3764-100001`. |
| E-mail | Entre 5 y 79 caracteres; exactamente un `@`; local y dominio no vacios; sin espacios; sin puntos consecutivos; sin punto al inicio o al final del local o del dominio; el dominio debe contener un punto; solo letras, digitos, `@`, `.`, `_`, `%`, `+` y `-`; se guarda en minusculas y no puede repetirse entre clientes. |
| Direccion | Entre 5 y 99 caracteres ASCII imprimibles (32 a 126); al menos una letra; no se aceptan cadenas solo numericas o solo de simbolos. |
| Fecha de alta | Fecha real en formato `AAAAMMDD`, no posterior a la fecha actual. |
| Estado | `1` (Activo) o `0` (Inactivo). |

### 8.3 Validaciones de vehiculo

| Dato | Regla |
|---|---|
| Patente | Formato `AAA123`, `AA123AA`, `123ABC` o `A123ABC`; unica. |
| Marca | Entre 2 y 39 caracteres; al menos una letra; admite espacios, guion, punto y `&`. |
| Modelo | Entre 2 y 39 caracteres; al menos una letra o digito; admite espacios, guion, punto, barra y `&`. Se aceptan modelos como `208`, `C4` y `Cronos`. |
| Anio | Entre 1990 y el anio actual + 1. |
| Tipo | Solo `Auto`, `Camioneta`, `Utilitario`, `Moto` o `4x4`, elegido desde un menu. |
| Capacidad | Entero entre 1 y 60 personas. |
| Precio por dia | Numero finito mayor a cero y hasta `10000000`. |
| Estado | Disponible, Alquilado, Baja o En mantenimiento. |

### 8.4 Validaciones de fechas

- Formato de ingreso: `AAAAMMDD`.
- Rango general de ingreso: `19000101` a `20991231`.
- La fecha debe existir en el calendario.
- Se controlan los anios bisiestos.
- Se controla la cantidad de dias de cada mes.
- La fecha de alta de un cliente debe ser hoy o anterior.
- La fecha de inicio de un alquiler nuevo debe estar entre hoy y hoy + 30 dias.
- En las devoluciones, la fecha debe estar entre el inicio del alquiler y hoy.
- En los pagos, la fecha debe estar entre el inicio del alquiler y hoy.
- Al modificar un alquiler, la nueva fecha de inicio no puede ser posterior al
  primer pago registrado.
- En los reportes por rango, la fecha final no puede ser anterior a la inicial;
  el reporte 8 no admite fechas futuras.

### 8.5 Reglas de negocio

| Operacion | Regla |
|---|---|
| Alta de alquiler | Cliente existente y activo. |
| Alta de alquiler | Vehiculo existente y disponible. |
| Alta de alquiler | Sin otro alquiler activo del mismo cliente. |
| Alta de alquiler | Sin otro alquiler activo del mismo vehiculo. |
| Alta de alquiler | Fecha de inicio entre hoy y hoy + 30 dias. |
| Alta de alquiler | 1 a 365 dias. |
| Modificacion | Solo alquileres activos. |
| Modificacion | Nuevo vehiculo disponible. |
| Modificacion | Nueva fecha de inicio entre hoy y hoy + 30 dias. |
| Modificacion | Nueva fecha de inicio no posterior al primer pago. |
| Modificacion | Nuevo total no menor a lo pagado. |
| Devolucion | Solo alquileres activos. |
| Devolucion | Fecha entre el inicio del alquiler y hoy. |
| Cancelacion | Solo alquileres activos. |
| Pago | Alquiler existente y no cancelado. |
| Pago | Monto mayor a cero y no superior al saldo. |
| Pago | Fecha entre el inicio del alquiler y hoy. |
| Baja de cliente | Sin alquiler activo. |
| Modificacion de vehiculo | Sin cambio de precio/estado si tiene alquiler activo. |
| Baja de vehiculo | Solo si no tiene alquiler activo (el estado se cambia desde la modificacion). |
| Integridad | Un alquiler `Finalizado` debe tener fecha efectiva; uno `Activo` o `Cancelado` no puede tenerla. |

### 8.6 Mensajes de error

Los errores se informan con el prefijo `[ERROR]` y un texto descriptivo que
incluye el formato esperado o un ejemplo. Los avisos informativos usan
`[AVISO]`. Ejemplos:

```text
[ERROR] El DNI 30111222 ya se encuentra registrado en el sistema.
[ERROR] Patente invalida. Formatos: AAA123 (auto viejo), AA123AA (auto Mercosur), 123ABC (moto vieja) o A123ABC (moto Mercosur). Reintente.
[ERROR] El cliente con DNI 99999999 no existe en el sistema.
[ERROR] El vehiculo no esta disponible para alquiler. Estado actual: Alquilado.
[ERROR] El cliente posee el alquiler Nro 13 en estado ACTIVO. Finalicelo o cancelalo antes de darlo de baja.
[ERROR] La fecha debe estar entre 20261108 y 20261208. Reintente.
[ERROR] La fecha de inicio no puede ser posterior al primer pago registrado (20261108).
[ERROR] El monto no puede superar el saldo pendiente ($390000.00). Reintente.
[ERROR] No se pueden registrar pagos para alquileres CANCELADOS.
[ERROR] El vehiculo con patente AB123CD posee un alquiler ACTIVO. Finalicelo o cancelalo antes de pasarlo a Baja o En mantenimiento.
```

---

## 9. Manejo de fechas

Las fechas se representan como enteros con el formato `AAAAMMDD`. Por ejemplo,
el 7 de noviembre de 2026 se representa como `20261107`.

Funciones implementadas en `fechas.cpp`:

| Funcion | Descripcion |
|---|---|
| `esBisiesto(anio)` | Determina si un anio es bisiesto. |
| `diasEnMes(anio, mes)` | Devuelve la cantidad de dias del mes. |
| `esFechaValida(fecha)` | Verifica rango, mes, dia y bisiesto. |
| `esFechaEnRango(fecha, min, max)` | Verifica que la fecha sea valida y este dentro del rango inclusivo. |
| `esFechaPasadaOHoy(fecha)` | Verifica que la fecha sea valida y no posterior a hoy. |
| `esFechaFuturaOHoy(fecha)` | Verifica que la fecha sea valida y no anterior a hoy. |
| `fechaADias(fecha)` | Convierte una fecha a numero de dias. |
| `diasAFecha(numeroDias)` | Convierte un numero de dias a fecha. |
| `diasEntre(fecha1, fecha2)` | Calcula la diferencia en dias. |
| `sumarDias(fecha, dias)` | Suma dias a una fecha. |
| `fechaActual()` | Obtiene la fecha actual del sistema. |
| `fechaAString(fecha)` | Devuelve `AAAAMMDD`. |
| `fechaALarga(fecha)` | Devuelve una representacion legible. |
| `formatearFecha(fecha, buffer)` | Escribe `DD/MM/AAAA` en un buffer de al menos 11 bytes. |

El calculo de diferencias y sumas se realiza mediante una conversion a numero
de dias, lo que evita errores en los cambios de mes y de anio.

---

## 10. Interfaz y modo de prueba

La interfaz es de consola y utiliza menus numericos. Las entradas se leen con
`std::getline` y se validan una por una. Ante un dato invalido, se muestra un
mensaje y se vuelve a solicitar el mismo dato.

Variables de control:

- `modoPrueba`: se activa con `--test`. Evita `cls` y las pausas.
- `finEntrada`: se activa cuando se alcanza el fin de la entrada estandar.
  Evita bucles infinitos y pausas innecesarias en pruebas redirigidas.

Opciones de linea de comandos:

```text
sistema.exe --test    Modo de prueba.
sistema.exe --help    Ayuda.
sistema.exe -h        Ayuda.
```

---

## 11. Dataset de prueba

El dataset se genera con `generar_datos.exe`. Las fechas de los alquileres
activos estan ubicadas en octubre y noviembre de 2026, de modo que resultan
utiles para probar devoluciones y recargos por demora.

### 11.1 Clientes

| DNI | Apellido y nombre | Telefono | E-mail | Direccion | Alta | Estado |
|---|---|---|---|---|---|---|
| 30111222 | Gomez Martin | 3764-100001 | mgomez@mail.com | Av. Mitre 123 | 20240110 | Activo |
| 31222333 | Fernandez Lucia | 3764-100002 | lfernandez@mail.com | Colon 456 | 20240215 | Activo |
| 32333444 | Rodriguez Pablo | 3764-100003 | prodriguez@mail.com | San Martin 789 | 20240320 | Activo |
| 33444555 | Lopez Carolina | 3764-100004 | clopez@mail.com | Sarmiento 321 | 20240505 | Activo |
| 34555666 | Martinez Diego | 3764-100005 | dmartinez@mail.com | Belgrano 654 | 20240612 | Activo |
| 35666777 | Sosa Valeria | 3764-100006 | vsosa@mail.com | Rivadavia 987 | 20240801 | Activo |
| 36777888 | Benitez Javier | 3764-100007 | jbenitez@mail.com | Junin 159 | 20240910 | Activo |
| 37888999 | Acosta Romina | 3764-100008 | racosta@mail.com | Cordoba 753 | 20241120 | Activo |
| 38999000 | Duarte Nicolas | 3764-100009 | nduarte@mail.com | Entre Rios 852 | 20250118 | Activo |
| 39000111 | Romero Agustina | 3764-100010 | aromero@mail.com | Salta 951 | 20250302 | Inactivo |

### 11.2 Vehiculos

| Patente | Marca | Modelo | Anio | Tipo | Cap. | Precio/dia | Estado |
|---|---|---|---|---|---|---|---|
| ABC123 | Toyota | Corolla | 2022 | Auto | 5 | 45000.00 | Disponible |
| AB123CD | Ford | Ranger | 2021 | Camioneta | 5 | 70000.00 | Alquilado |
| DEF456 | Volkswagen | Gol | 2020 | Auto | 5 | 30000.00 | Disponible |
| GH789IJ | Renault | Kangoo | 2019 | Utilitario | 3 | 40000.00 | Alquilado |
| JKL012 | Chevrolet | Onix | 2023 | Auto | 5 | 38000.00 | Disponible |
| MN345OP | Fiat | Cronos | 2022 | Auto | 5 | 36000.00 | Alquilado |
| PQR678 | Peugeot | 208 | 2021 | Auto | 5 | 34000.00 | Disponible |
| ST901UV | Toyota | Hilux | 2023 | Camioneta | 5 | 85000.00 | En mantenimiento |
| WXY234 | Citroen | Berlingo | 2020 | Utilitario | 3 | 42000.00 | Disponible |
| ZAB567 | Volkswagen | Amarok | 2022 | Camioneta | 5 | 90000.00 | Baja |

### 11.3 Alquileres

| N. | DNI | Patente | Inicio | Prevista | Efectiva | Dias | Total | Pagado | Estado |
|---|---|---|---|---|---|---|---|---|---|
| 1 | 30111222 | ABC123 | 20260105 | 20260110 | 20260110 | 5 | 225000.00 | 225000.00 | Finalizado |
| 2 | 31222333 | AB123CD | 20260112 | 20260115 | 20260115 | 3 | 210000.00 | 210000.00 | Finalizado |
| 3 | 32333444 | DEF456 | 20260120 | 20260127 | 20260130 | 7 | 300000.00 | 210000.00 | Finalizado |
| 4 | 33444555 | JKL012 | 20260201 | 20260205 | 20260205 | 4 | 152000.00 | 152000.00 | Finalizado |
| 5 | 34555666 | WXY234 | 20260210 | 20260220 | 20260220 | 10 | 420000.00 | 200000.00 | Finalizado |
| 6 | 35666777 | ZAB567 | 20260301 | 20260303 | 20260303 | 2 | 180000.00 | 180000.00 | Finalizado |
| 7 | 30111222 | AB123CD | 20260315 | 20260321 | 20260322 | 6 | 490000.00 | 490000.00 | Finalizado |
| 8 | 36777888 | GH789IJ | 20260401 | 20260406 | 20260406 | 5 | 200000.00 | 100000.00 | Finalizado |
| 9 | 37888999 | ABC123 | 20260415 | 20260418 | 0 | 3 | 0.00 | 0.00 | Cancelado |
| 10 | 38999000 | DEF456 | 20260502 | 20260506 | 0 | 4 | 50000.00 | 50000.00 | Cancelado |
| 11 | 31222333 | JKL012 | 20260510 | 20260518 | 20260518 | 8 | 304000.00 | 304000.00 | Finalizado |
| 12 | 33444555 | AB123CD | 20260601 | 20260606 | 0 | 5 | 0.00 | 0.00 | Cancelado |
| 13 | 34555666 | AB123CD | 20261026 | 20261102 | 0 | 7 | 490000.00 | 100000.00 | Activo |
| 14 | 32333444 | GH789IJ | 20261028 | 20261102 | 0 | 5 | 200000.00 | 0.00 | Activo |
| 15 | 35666777 | MN345OP | 20261020 | 20261030 | 0 | 10 | 360000.00 | 360000.00 | Activo |

Notas sobre importes:

- Alquiler 3: 7 dias x 30000 = 210000; 3 dias de demora x 30000 = 90000 de
  recargo; total 300000; pagado 210000; deuda 90000.
- Alquiler 5: 10 dias x 42000 = 420000; pagado 200000; deuda 220000.
- Alquiler 7: 6 dias x 70000 = 420000; 1 dia de demora x 70000 = 70000 de
  recargo; total 490000; pagado 490000.
- Alquiler 8: 5 dias x 40000 = 200000; pagado 100000; deuda 100000.
- Alquiler 9: cancelado antes del inicio; total 0; sin deuda.
- Alquiler 10: cancelado despues de un pago de 50000; el total se ajusto a
  50000; sin deuda.
- Alquiler 12: cancelado; total 0; sin deuda.
- Alquiler 13: 7 dias x 70000 = 490000; pagado 100000; deuda 390000.
- Alquiler 14: 5 dias x 40000 = 200000; pagado 0; deuda 200000.
- Alquiler 15: 10 dias x 36000 = 360000; pagado 360000; sin deuda.

Deuda total:

```text
90000 + 220000 + 100000 + 390000 + 200000 = 1000000
```

Total cobrado (suma de `importePagado`):

```text
225000 + 210000 + 210000 + 152000 + 200000 + 180000
+ 490000 + 100000 + 0 + 50000 + 304000 + 0
+ 100000 + 0 + 360000 = 2581000
```

### 11.4 Movimientos

| N. | Alquiler | Tipo | Monto | Fecha | DNI | Patente | Medio |
|---|---|---|---|---|---|---|---|
| 1 | 1 | Pago | 100000.00 | 20260105 | 30111222 | ABC123 | Efectivo |
| 2 | 1 | Pago | 125000.00 | 20260110 | 30111222 | ABC123 | Tarjeta |
| 3 | 2 | Pago | 210000.00 | 20260112 | 31222333 | AB123CD | Transferencia |
| 4 | 3 | Pago | 100000.00 | 20260120 | 32333444 | DEF456 | Efectivo |
| 5 | 3 | Pago | 110000.00 | 20260127 | 32333444 | DEF456 | Tarjeta |
| 6 | 3 | Ajuste | 90000.00 | 20260130 | 32333444 | DEF456 | - |
| 7 | 4 | Pago | 152000.00 | 20260201 | 33444555 | JKL012 | Tarjeta |
| 8 | 5 | Pago | 100000.00 | 20260210 | 34555666 | WXY234 | Efectivo |
| 9 | 5 | Pago | 100000.00 | 20260220 | 34555666 | WXY234 | Transferencia |
| 10 | 6 | Pago | 180000.00 | 20260301 | 35666777 | ZAB567 | Tarjeta |
| 11 | 7 | Pago | 300000.00 | 20260315 | 30111222 | AB123CD | Efectivo |
| 12 | 7 | Ajuste | 70000.00 | 20260322 | 30111222 | AB123CD | - |
| 13 | 7 | Pago | 190000.00 | 20260322 | 30111222 | AB123CD | Transferencia |
| 14 | 8 | Pago | 100000.00 | 20260401 | 36777888 | GH789IJ | Efectivo |
| 15 | 9 | Ajuste | -135000.00 | 20260416 | 37888999 | ABC123 | - |
| 16 | 10 | Pago | 50000.00 | 20260502 | 38999000 | DEF456 | Efectivo |
| 17 | 10 | Ajuste | -70000.00 | 20260504 | 38999000 | DEF456 | - |
| 18 | 11 | Pago | 150000.00 | 20260510 | 31222333 | JKL012 | Tarjeta |
| 19 | 11 | Pago | 154000.00 | 20260518 | 31222333 | JKL012 | Efectivo |
| 20 | 12 | Ajuste | -350000.00 | 20260602 | 33444555 | AB123CD | - |
| 21 | 15 | Pago | 180000.00 | 20261020 | 35666777 | MN345OP | Transferencia |
| 22 | 15 | Pago | 180000.00 | 20261025 | 35666777 | MN345OP | Tarjeta |
| 23 | 13 | Pago | 100000.00 | 20261026 | 34555666 | AB123CD | Efectivo |

Resumen: 18 pagos y 5 ajustes. Los ajustes negativos corresponden a la
cancelacion de alquileres y dejan constancia del importe estimado que se deja
sin efecto.

### 11.5 Archivos de texto generados

Se generan diez archivos:

```text
movimientos_30111222.txt
movimientos_31222333.txt
movimientos_32333444.txt
movimientos_33444555.txt
movimientos_34555666.txt
movimientos_35666777.txt
movimientos_36777888.txt
movimientos_37888999.txt
movimientos_38999000.txt
movimientos_39000111.txt
```

El archivo del cliente 39000111 queda vacio porque no tiene movimientos.

---

## 12. Pruebas y verificacion

### 12.1 Pruebas unitarias

Archivo: `tests_validaciones.cpp`.
Ejecutable: `tests_validaciones.exe`.

Cobertura:

- Patentes: cuatro formatos validos e invalidos, minusculas, espacios, guiones
  y puntos.
- Normalizacion de patentes y de e-mails.
- DNI: limites y valores invalidos.
- Nombres: longitud minima, letras con acentos y enie, y caracteres permitidos.
- Telefonos: entre 8 y 15 digitos, separadores y `+` inicial.
- E-mails: estructura, dominio, espacios, puntos consecutivos y normalizacion.
- Direcciones: longitud minima y presencia de al menos una letra.
- Marcas, modelos y tipos de vehiculo.
- Rangos de anio, capacidad y precio por dia.
- Estados de vehiculo, alquiler, movimiento y medios de pago.
- Fechas: bisiestos, meses, sumas, restas, diferencias, rangos, comparaciones
  con hoy y formateo `DD/MM/AAAA`.
- Utilidades de texto: recorte, mayusculas, minusculas, division, copia y
  conversion.

Resultado obtenido:

```text
==============================================
   PRUEBAS DE VALIDACION DEL SISTEMA
==============================================
...
==============================================
Total de pruebas: 169
Fallos:           0
RESULTADO: TODAS LAS PRUEBAS PASARON.
```

### 12.2 Pruebas de integracion

Se ejecutaron pruebas de integracion con `sistema.exe --test` y entradas
redirigidas. Casos verificados:

- Alta de cliente valido.
- Rechazo de DNI duplicado.
- Rechazo de datos de cliente invalidos con reintento.
- Rechazo de fecha de alta futura.
- Rechazo de e-mail duplicado (sin distinguir mayusculas/minusculas).
- Modificacion de cliente conservando su propio e-mail.
- Alta de vehiculo valido.
- Rechazo de patente duplicada.
- Rechazo de patentes con formato invalido.
- Aceptacion de patentes normalizadas (minusculas, espacios, guiones).
- Rechazo de marca, modelo, anio, capacidad y precio fuera de rango.
- Alta de alquiler valido.
- Rechazo de cliente inexistente.
- Rechazo de vehiculo inexistente.
- Rechazo de vehiculo no disponible.
- Rechazo de cliente con alquiler activo.
- Rechazo de vehiculo con alquiler activo.
- Rechazo de fecha de inicio anterior a hoy.
- Registro de pago parcial.
- Rechazo de pago superior al saldo.
- Rechazo de pago sobre alquiler saldado.
- Rechazo de pago con fecha futura.
- Devolucion en plazo.
- Devolucion con recargo por demora.
- Rechazo de devolucion sin alquiler activo.
- Rechazo de devolucion con fecha anterior al inicio o posterior a hoy.
- Cancelacion de alquiler.
- Modificacion de alquiler.
- Cambio de vehiculo en un alquiler.
- Rechazo de modificacion que deja el total por debajo de lo pagado.
- Rechazo de fecha de inicio posterior al primer pago registrado.
- Baja de cliente.
- Rechazo de baja de cliente con alquiler activo.
- Reactivacion de cliente.
- Restricciones de modificacion de vehiculo alquilado.
- Rechazo de paso a `Baja` o `En mantenimiento` con alquiler activo.
- Ejecucion de los diez reportes.
- Reporte 9 con una sola fila por cliente y sin estados contradictorios.

### 12.3 Verificacion del dataset

`generar_datos.exe` verifica automaticamente:

```text
[OK] Verificacion del dataset: 10 clientes, 10 vehiculos, 15 alquileres, 23 movimientos.
[OK] Estados: 5 disponibles, 3 alquilados, 1 mantenimiento, 1 baja.
[OK] Alquileres: 3 activos, 9 finalizados, 3 cancelados.
[OK] Deuda total: $1000000.00 | Total cobrado: $2581000.00.
[OK] Archivos de texto por cliente generados.
```

Si alguna verificacion falla, el programa muestra `[FALLO]` y devuelve un
codigo de salida distinto de cero.

---

## 13. Compilacion y ejecucion

### 13.1 Compilacion

Opcion recomendada en Windows:

```bat
compilar.bat
```

Opcion con Makefile:

```bat
mingw32-make -f Makefile.win
```

Opcion manual:

```bat
g++ -std=c++11 -Wall -Wextra -O2 -o sistema.exe main.cpp menu.cpp fechas.cpp utils.cpp clientes.cpp vehiculos.cpp alquileres.cpp movimientos.cpp reportes.cpp
g++ -std=c++11 -Wall -Wextra -O2 -o generar_datos.exe generar_datos.cpp fechas.cpp utils.cpp clientes.cpp vehiculos.cpp alquileres.cpp movimientos.cpp
g++ -std=c++11 -Wall -Wextra -O2 -o tests_validaciones.exe tests_validaciones.cpp fechas.cpp utils.cpp
```

La compilacion fue verificada con TDM-GCC 9.2.0, sin errores ni advertencias.

### 13.2 Ejecucion

```bat
generar_datos.exe
tests_validaciones.exe
sistema.exe
```

Para pruebas automaticas:

```bat
sistema.exe --test
```

### 13.3 Archivos generados en tiempo de ejecucion

- `clientes.dat`
- `vehiculos.dat`
- `alquileres.dat`
- `movimientos.dat`
- `movimientos_<DNI>.txt` (uno por cliente)

---

## 14. Mapa de menus

```text
MENU PRINCIPAL
|
+-- 1. Clientes
|   +-- 1. Registrar cliente
|   +-- 2. Modificar cliente
|   +-- 3. Consultar cliente (con historial)
|   +-- 4. Listar clientes
|   +-- 5. Dar de baja / reactivar cliente
|
+-- 2. Vehiculos
|   +-- 1. Registrar vehiculo
|   +-- 2. Modificar vehiculo
|   +-- 3. Buscar vehiculo por patente
|   +-- 4. Listar vehiculos
|
+-- 3. Alquileres
|   +-- 1. Realizar nuevo alquiler
|   +-- 2. Modificar alquiler
|   +-- 3. Listar alquileres
|   +-- 4. Cancelar alquiler
|
+-- 4. Devoluciones
|
+-- 5. Pagos
|   +-- 1. Registrar pago
|   +-- 2. Listar todos los movimientos
|
+-- 6. Consultas y Reportes
|   +-- 1. Vehiculos disponibles
|   +-- 2. Vehiculos alquilados
|   +-- 3. Alquileres por rango de fechas
|   +-- 4. Alquileres por cliente
|   +-- 5. Alquileres por vehiculo
|   +-- 6. Alquileres por estado
|   +-- 7. Alquileres por importe
|   +-- 8. Pagos realizados por periodo
|   +-- 9. Clientes con deuda
|   +-- 10. Estadisticas de la empresa
|
+-- 0. Salir
```

---

## 15. Limitaciones y mejoras posibles

### 15.1 Limitaciones

- La base se carga completa en memoria; es adecuada para el volumen de un
  trabajo practico, pero no para grandes volumenes de datos.
- Los archivos binarios dependen de la plataforma y del compilador.
- No hay control de acceso concurrente: dos instancias del programa podrian
  sobrescribirse mutuamente.
- El DNI y la patente no se pueden modificar una vez registrados.
- No se gestionan backups automaticos.
- Las devoluciones anticipadas no generan reintegros.

### 15.2 Mejoras posibles

- Agregar indexacion o archivos de acceso directo para busquedas rapidas.
- Incorporar un modulo de backups y restauracion.
- Incorporar autenticacion de usuarios.
- Migrar la persistencia a una base de datos.
- Agregar pruebas de integracion automatizadas dentro del repositorio.
- Incorporar una interfaz grafica.
- Internacionalizar los mensajes.

---

## 16. Anexo: formato de archivos

### 16.1 Formato binario

Cada archivo binario contiene una secuencia consecutiva de registros:

```text
+------------------+------------------+------------------+-----+
| Registro 1       | Registro 2       | Registro 3       | ... |
+------------------+------------------+------------------+-----+
```

- Sin cabecera.
- Sin separadores.
- Sin marca de fin de archivo.
- Tamano de registro: `sizeof(struct)`.
- Orden de campos: el definido en `comun.h`.
- Codificacion de cadenas: ASCII, con terminador nulo y relleno no inicializado
  en los bytes restantes (se controla la lectura con el terminador).

### 16.2 Formato de texto de movimientos

```text
numMov;numAlquiler;Tipo;monto;fecha;DNI;patente;medioPago
```

Ejemplo completo:

```text
1;1;Pago;100000.00;20260105;30111222;ABC123;Efectivo
2;1;Pago;125000.00;20260110;30111222;ABC123;Tarjeta
12;7;Ajuste;70000.00;20260322;30111222;AB123CD;-
```

Reglas:

- Un movimiento por linea.
- Sin linea de cabecera.
- Sin linea de cierre.
- `monto` con dos decimales.
- `fecha` como entero `AAAAMMDD`.
- `medioPago` es `-` para los ajustes.
- El archivo se reescribe completo en cada regeneracion.

### 16.3 Ejemplo de contenido de un archivo de texto

Archivo `movimientos_30111222.txt`:

```text
1;1;Pago;100000.00;20260105;30111222;ABC123;Efectivo
2;1;Pago;125000.00;20260110;30111222;ABC123;Tarjeta
11;7;Pago;300000.00;20260315;30111222;AB123CD;Efectivo
12;7;Ajuste;70000.00;20260322;30111222;AB123CD;-
13;7;Pago;190000.00;20260322;30111222;AB123CD;Transferencia
```

---

## 17. Conclusion

El sistema cubre el ciclo completo de gestion de alquiler de vehiculos con una
arquitectura modular, persistencia binaria, validaciones estrictas y un conjunto
de reportes que permiten analizar la operacion. La separacion entre estructuras
de datos, funciones de validacion, logica de negocio y presentacion facilita el
mantenimiento y la extension del programa.

Las pruebas unitarias, las pruebas de integracion y la verificacion automatica
del dataset permiten afirmar que la implementacion cumple con los requisitos
funcionales del trabajo practico y que las validaciones, en particular la
validacion de patentes argentinas, funcionan correctamente.

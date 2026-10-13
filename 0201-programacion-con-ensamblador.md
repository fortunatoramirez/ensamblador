# Práctica 1. Primeros programas en ensamblador con RP2040-Zero

**Asignatura:** Lenguaje ensamblador  
**Tarjeta:** Waveshare RP2040-Zero  
**Entorno:** Visual Studio Code, extensión oficial Raspberry Pi Pico y Pico SDK  
**Punto de partida:** ya creamos proyectos, compilamos, cargamos archivos UF2 y realizamos pruebas en C.

## 1. Objetivo

Comprender cómo el procesador utiliza registros, instrucciones, comparaciones y saltos, y escribir las primeras funciones en ensamblador para el **ARM Cortex-M0+ del RP2040**.

Al finalizar podrás:

- Distinguir C, ensamblador y código máquina.
- Interpretar registros, valores inmediatos, directivas y etiquetas.
- Ejecutar sumas, restas, una comparación y un ciclo en ensamblador.
- Pasar dos números desde C a una función ensamblador y recuperar su resultado.
- Integrar archivos `.c` y `.S` en el mismo proyecto.
- Comprobar resultados en el monitor USB y mediante tablas de seguimiento.

**Dinámica:** en cada etapa primero predice el resultado, después ejecuta y finalmente explica lo observado. No es necesario memorizar todas las instrucciones del procesador.

## 2. Material y conocimientos previos

- RP2040-Zero y cable USB-C **de datos**.
- Computadora con el entorno de la práctica 0 funcionando.
- Un proyecto en C que ya compile y envíe mensajes por USB.
- Repaso de bit, byte, binario, hexadecimal y operaciones básicas de C.

No se necesitan protoboard, componentes adicionales ni depurador externo. No reinstales las herramientas si la práctica anterior ya funcionó.

### Nuestra tarjeta

| Elemento | En esta práctica |
|---|---|
| Tarjeta | Waveshare RP2040-Zero |
| Microcontrolador | RP2040 |
| Núcleos | Dos ARM Cortex-M0+; utilizaremos el núcleo 0 |
| Arquitectura | ARMv6-M |
| Instrucciones | Conjunto Thumb disponible en ARMv6-M |
| Registros de propósito general | 32 bits |
| Configuración de tarjeta del SDK | `waveshare_rp2040_zero` |
| Comunicación con la computadora | USB CDC, presentada como puerto serial |

**No seleccionar `pico2`:** corresponde a hardware basado en RP2350. La RP2040-Zero tampoco utiliza el LED convencional de una Raspberry Pi Pico como indicador de esta práctica.

Su LED integrado es un **RGB WS2812 conectado a GPIO16**. Necesita una secuencia de datos con tiempos específicos; dejar GPIO16 en alto o bajo no permite seleccionar sus colores. Aunque ya hayamos probado el LED desde C, aquí observaremos los resultados por USB para concentrarnos en las instrucciones de la CPU. Las instrucciones de PIO que puedan aparecer en un ejemplo de WS2812 pertenecen a otro procesador interno y **no son ensamblador ARM**.

## 3. De C a las instrucciones del procesador

En C podemos expresar:

```c
resultado = a + b;
```

El compilador transforma el programa en código máquina. En ensamblador escribimos nombres legibles de operaciones del procesador, llamados **mnemónicos**:

```asm
ADDS R0, R0, R1
```

Esta instrucción suma los contenidos de `R0` y `R1` y deja el resultado en `R0`.

| Nivel | Ejemplo o función |
|---|---|
| C | Expresa algoritmos mediante variables, funciones y estructuras de control |
| Ensamblador | Expresa operaciones mediante instrucciones, registros y etiquetas |
| Código máquina | Codificación binaria que ejecuta el procesador |

El programa que traduce ensamblador a código máquina se llama **ensamblador**. El **enlazador** une el código de nuestros archivos y las bibliotecas necesarias.

> Tanto el código escrito en C como el escrito en ensamblador termina ejecutándose como código máquina. La diferencia aquí es que nosotros escribiremos directamente las instrucciones que realizan los cálculos.

## 4. Registros: espacios de trabajo dentro de la CPU

Un registro es un pequeño espacio de almacenamiento dentro del procesador. Los registros de propósito general del Cortex-M0+ tienen **32 bits**, equivalentes a cuatro bytes.

No son variables de C con un nombre permanente. Distintas funciones pueden reutilizar el mismo registro.

| Registro | Uso básico |
|---|---|
| `R0–R3` | Argumentos y valores temporales; `R0` también devuelve nuestro resultado entero |
| `R4–R7` | Propósito general; deben conservarse si una función llamada desde C los modifica |
| `R8–R12` | También son de propósito general, pero muchas instrucciones Thumb tienen restricciones para utilizarlos |
| `R13` o `SP` | Puntero a la pila, una zona de RAM para información temporal |
| `R14` o `LR` | Información de retorno de una llamada a función |
| `R15` o `PC` | Contador de programa, relacionado con la dirección de ejecución |

Para empezar utilizaremos principalmente `R0`, `R1` y `R2`. Muchas formas sencillas de instrucciones admiten únicamente los registros bajos, `R0–R7`.

### Tres registros especiales

- **SP:** permite localizar la parte activa de la pila. El código de arranque del SDK prepara la pila antes de ejecutar `main`.
- **LR:** una llamada con `BL` guarda aquí la dirección de retorno. Nuestras funciones regresan con `BX LR`.
- **PC:** participa en el avance por las instrucciones y en los saltos. No es una variable que debamos modificar en esta práctica. Su lectura tiene reglas de la arquitectura; no debe interpretarse simplemente como «la dirección exacta de la instrucción que estoy viendo».

Las funciones de esta práctica no llaman a otras funciones, por lo que pueden regresar directamente con `BX LR`. Si más adelante una función utiliza `BL` para llamar a otra, deberá conservar su retorno original.

## 5. Cómo leer una línea de ensamblador

```asm
MOVS R0, #5      @ Colocar el numero 5 en R0
```

| Parte | Significado |
|---|---|
| `MOVS` | Mnemónico de la instrucción |
| `R0` | Registro destino |
| `#5` | Valor inmediato: el número literal 5 |
| `@ ...` | Comentario para las personas; no se ejecuta |

El destino aparece primero en las operaciones que utilizaremos:

```asm
MOVS R0, #5          @ R0 = 5
MOVS R1, #3          @ R1 = 3
ADDS R2, R0, R1      @ R2 = R0 + R1
```

Al terminar: `R0 = 5`, `R1 = 3`, `R2 = 8`.

### Copiar no significa borrar

```asm
MOVS R0, R1
```

copia el contenido de `R1` a `R0`; **no borra R1**.

### No todos los números caben en todas las instrucciones

En la forma `MOVS R0, #numero` usada aquí, el inmediato puede ser de **0 a 255**. Los registros sí tienen 32 bits; el límite corresponde a la codificación de esa instrucción, no al tamaño del registro.

Para otros valores se puede usar una pseudoinstrucción como:

```asm
LDR R0, =1000
```

El ensamblador elige una forma de cargar esa constante. No confundas `LDR R0, =1000` con `LDR R0, [R1]`: esta última lee memoria en la dirección guardada en `R1`.

## 6. Primera etapa: preparar el proyecto

### 6.1 Crear un proyecto nuevo

1. Abre la extensión **Raspberry Pi Pico** en VS Code.
2. Usa la opción de crear un proyecto C/C++ como en la práctica 0.
3. Nómbralo **`P01_Ensamblador`** y elige **C** para el archivo principal.
4. Conserva la versión del SDK que ya funcionó en tu computadora.
5. Activa **Console over USB**. No se requieren periféricos adicionales.
6. Si aparece la tarjeta `waveshare_rp2040_zero`, selecciónala. Si no aparece, crea el proyecto como en la práctica 0 y corrige `PICO_BOARD` antes de compilar.

### 6.2 Preparar los archivos

Renombra el archivo principal generado, por ejemplo `P01_Ensamblador.c`, a **`main.c`**. No conserves dos archivos con una función `main` dentro del ejecutable.

Crea **`operaciones.S`**, con `S` mayúscula, en la misma carpeta que `main.c`.

| Archivo | Responsabilidad |
|---|---|
| `main.c` | Inicializar USB, llamar funciones y mostrar resultados |
| `operaciones.S` | Realizar cálculos, comparaciones y ciclos en ensamblador |
| `CMakeLists.txt` | Indicar tarjeta, lenguajes, fuentes y bibliotecas |
| `pico_sdk_import.cmake` | Incorporar el SDK; conservar el archivo generado |

La extensión `.S` permite pasar por el preprocesador antes de ensamblar. Verifica que el archivo no termine en `.S.txt`.

### 6.3 Ajustar CMake sin perder la configuración del entorno

Abre `CMakeLists.txt` y conserva su parte superior generada por la extensión: `cmake_minimum_required`, configuración de estándares, versiones de herramientas y el bloque de integración con `pico-vscode.cmake`.

**No reemplaces ese encabezado por un CMake mínimo de Internet:** contiene la integración con las herramientas instaladas en tu computadora.

Busca la asignación de `PICO_BOARD` y sustitúyela por:

```cmake
set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Board type")
```

Debe estar **antes** de:

```cmake
include(pico_sdk_import.cmake)
```

Si existe una asignación explícita a `PICO_PLATFORM` con `rp2350` u otra plataforma incorrecta, elimínala para que el SDK derive la plataforma de la tarjeta. No agregues una segunda asignación de `PICO_BOARD`.

Después de `include(pico_sdk_import.cmake)`, sustituye el contenido restante del archivo por este bloque completo:

```cmake
project(P01_Ensamblador C CXX ASM)

pico_sdk_init()

add_executable(P01_Ensamblador
    main.c
    operaciones.S
)

pico_set_program_name(P01_Ensamblador "P01_Ensamblador")
pico_set_program_version(P01_Ensamblador "1.0")

target_link_libraries(P01_Ensamblador
    pico_stdlib
)

target_include_directories(P01_Ensamblador PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_enable_stdio_usb(P01_Ensamblador 1)
pico_enable_stdio_uart(P01_Ensamblador 0)

pico_add_extra_outputs(P01_Ensamblador)
```

**Puntos que debes identificar:**

- `ASM` habilita el lenguaje ensamblador.
- `add_executable` incluye los dos archivos fuente, una sola vez.
- `pico_stdlib` proporciona las funciones de apoyo usadas desde C.
- `pico_enable_stdio_usb(... 1)` dirige la salida estándar a USB.
- `pico_add_extra_outputs` solicita, entre otros archivos, el `.uf2`.

Todos los nombres de destino deben ser `P01_Ensamblador`, respetando mayúsculas y minúsculas.

Al terminar los archivos de la siguiente sección, ejecuta las acciones de **limpiar y reconfigurar CMake** de la extensión, como en la práctica anterior. Esto es especialmente importante si antes estaba seleccionado `pico2`: modificar una línea con `CACHE` no necesariamente elimina el valor previamente guardado.

## 7. Segunda etapa: primera función en ensamblador

### 7.1 Archivo `operaciones.S`

Copia este programa completo:

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.section .text
.balign 2

.global sumar_5_y_3
.type sumar_5_y_3, %function
.thumb_func
sumar_5_y_3:
    MOVS R0, #5
    MOVS R1, #3
    ADDS R0, R0, R1
    BX LR
.size sumar_5_y_3, .-sumar_5_y_3
```

### 7.2 Directivas e instrucciones

Las líneas que empiezan con punto son **directivas para las herramientas**; no son operaciones ejecutadas una por una por la CPU.

| Directiva | Propósito |
|---|---|
| `.syntax unified` | Selecciona la sintaxis unificada de ARM |
| `.cpu cortex-m0plus` | Define el procesador de destino |
| `.thumb` | Selecciona código Thumb |
| `.section .text` | Coloca el código en la sección de instrucciones |
| `.balign 2` | Alinea el comienzo a un múltiplo de dos bytes |
| `.global sumar_5_y_3` | Hace visible el símbolo para el enlazador |
| `.type ..., %function` | Identifica el símbolo como una función |
| `.thumb_func` | Marca la siguiente función como Thumb |
| `.size ...` | Registra su tamaño; `.` representa la posición actual |

`sumar_5_y_3:` es una **etiqueta**: da nombre a una posición del código. No realiza un cálculo. No toda etiqueta es una función: también tendremos etiquetas internas para saltar dentro de una función.

Las cuatro instrucciones ejecutables de esta función son `MOVS`, `MOVS`, `ADDS` y `BX`.

### 7.3 Archivo `main.c`

Copia este programa completo:

```c
#include <stdio.h>
#include "pico/stdlib.h"

extern int sumar_5_y_3(void);

int main(void)
{
    stdio_init_all();
    sleep_ms(2000);

    while (true)
    {
        int resultado = sumar_5_y_3();
        printf("Resultado producido por ensamblador: %d\n", resultado);
        sleep_ms(1000);
    }
}
```

`extern int sumar_5_y_3(void);` declara una función que devuelve un entero y no recibe argumentos. Su definición está en `operaciones.S`. La palabra `extern` no realiza la llamada; esta ocurre en `sumar_5_y_3()`.

**C prepara la comunicación y muestra el valor; la suma está escrita en ensamblador.**

### 7.4 Compilar y cargar

1. Guarda todos los archivos.
2. Limpia y reconfigura CMake si cambiaste la tarjeta o la configuración del proyecto.
3. Compila con la extensión. Comprueba que termina sin errores.
4. Localiza el archivo **`P01_Ensamblador.uf2`** recién generado, normalmente dentro de `build`.
5. En la RP2040-Zero, mantén presionado **BOOT**, presiona y suelta **RESET** y después suelta **BOOT**. También puedes conectarla mientras mantienes BOOT presionado.
6. Copia el UF2 a la unidad **`RPI-RP2`**.
7. Espera a que desaparezca la unidad y reaparezca el puerto serial USB.
8. Abre el monitor serial y selecciona el puerto de la tarjeta. Si solicita velocidad, puedes usar **115200**; en esta práctica el transporte es USB CDC, no una UART física.

Resultado esperado, aproximadamente una vez por segundo:

```text
Resultado producido por ensamblador: 8
Resultado producido por ensamblador: 8
Resultado producido por ensamblador: 8
```

La pausa inicial da tiempo a la conexión, pero no garantiza que el monitor ya esté abierto. Por eso el mensaje se repite.

### 7.5 Seguimiento manual

| Instrucción ejecutada | R0 | R1 | Explicación |
|---|---:|---:|---|
| Antes de empezar | ? | ? | No suponemos valores iniciales |
| `MOVS R0, #5` | 5 | ? | Carga del primer número |
| `MOVS R1, #3` | 5 | 3 | Carga del segundo número |
| `ADDS R0, R0, R1` | 8 | 3 | El resultado reemplaza el valor anterior de R0 |
| `BX LR` | 8 | 3 | Regresa al código que llamó a la función |

El signo `?` significa **valor no conocido**, no cero.

**Prueba:** cambia los inmediatos por 9 y 6. Predice, compila y carga de nuevo. Comprueba 15 y después restaura 5 y 3.

## 8. Cómo se comunican C y ensamblador

La comunicación sigue una convención de llamadas. Para las funciones con enteros de 32 bits que utilizaremos:

| Elemento de la llamada | Ubicación |
|---|---|
| Primer argumento | `R0` |
| Segundo argumento | `R1` |
| Tercer y cuarto argumento, si los hubiera | `R2` y `R3` |
| Resultado entero de 32 bits | `R0` |

Ejemplo:

```c
int resultado = sumar(20, 7);
```

Al entrar a `sumar`, `R0` contiene 20 y `R1` contiene 7. Si la función deja 27 en `R0` y retorna, C recibe 27.

Estas reglas no significan que todos los tipos de datos o cualquier cantidad de parámetros se manejen igual. Aquí trabajaremos con `int` de 32 bits y valores pequeños, evitando desbordamientos.

`R0–R3` y `R12` pueden cambiar durante una llamada. Una función debe preservar los registros que la convención exige conservar, como `R4–R11` en este entorno, si decide utilizarlos. Nuestra solución solo modifica `R0` y `R1` y no necesita guardar registros en la pila.

## 9. Banderas, comparaciones y saltos

La CPU dispone de banderas que describen resultados de operaciones. La `S` en `MOVS`, `ADDS` o `SUBS` indica que se actualizan determinadas banderas; **no todas esas instrucciones modifican las cuatro de la misma manera**.

| Bandera | Idea principal |
|---|---|
| `Z` | Vale 1 si el resultado considerado es cero |
| `N` | Copia el bit más significativo del resultado; indica signo negativo si lo interpretamos con signo |
| `C` | Acarreo en suma; en resta, 1 indica que no hubo préstamo |
| `V` | Desbordamiento en aritmética con signo |

### Comparar con CMP

```asm
CMP R0, R1
```

actualiza las banderas como si restara `R0 - R1`, pero **no guarda la resta ni modifica los operandos**.

Si ambos valores son iguales, `Z = 1`.

### Elegir hacia dónde continuar

| Instrucción | Cuándo salta |
|---|---|
| `B etiqueta` | Siempre |
| `BEQ etiqueta` | Si son iguales: `Z = 1` |
| `BNE etiqueta` | Si son diferentes: `Z = 0` |
| `BGT` / `BGE` | Mayor / mayor o igual, comparación **con signo** |
| `BLT` / `BLE` | Menor / menor o igual, comparación **con signo** |
| `BHI` / `BHS` | Mayor / mayor o igual, comparación **sin signo** |
| `BLO` / `BLS` | Menor / menor o igual, comparación **sin signo** |

`BHS` y `BLO` también se conocen como `BCS` y `BCC`.

Usaremos `int`, incluyendo pruebas negativas, por lo que `BGE` y `BLT` son apropiados. No se debe cambiar a `unsigned int` y conservar automáticamente los mismos saltos de orden.

> Coloca el salto condicional inmediatamente después de la comparación en estos ejemplos. Una instrucción intermedia que modifique banderas puede cambiar la decisión.

### Un if sencillo

```asm
CMP R0, R1
BNE .L_diferentes
MOVS R0, #1
BX LR
.L_diferentes:
MOVS R0, #0
BX LR
```

Si son diferentes, salta a `.L_diferentes`. Si son iguales, continúa con la siguiente instrucción. Esta función devuelve 1 o 0. Las etiquetas que empiezan con `.L` son símbolos locales usados internamente; dales nombres distintos dentro del archivo.

## 10. Tercera etapa: programa integrado completo

Ahora ejecutaremos cinco funciones. **Sustituye** el contenido de los dos archivos por las versiones siguientes; no las pegues debajo de las versiones anteriores.

`CMakeLists.txt` conserva la configuración de la sección 6.

### 10.1 Archivo completo `operaciones.S`

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.section .text
.balign 2

@ 1. Suma de constantes: devuelve 8.
.global sumar_5_y_3
.type sumar_5_y_3, %function
.thumb_func
sumar_5_y_3:
    MOVS R0, #5
    MOVS R1, #3
    ADDS R0, R0, R1
    BX LR
.size sumar_5_y_3, .-sumar_5_y_3

@ 2. Resta de constantes: devuelve 13.
.global resta
.type resta, %function
.thumb_func
resta:
    MOVS R0, #20
    MOVS R1, #7
    SUBS R0, R0, R1
    BX LR
.size resta, .-resta

@ 3. Entradas: R0 = a, R1 = b. Salida: R0 = a + b.
.global sumar
.type sumar, %function
.thumb_func
sumar:
    ADDS R0, R0, R1
    BX LR
.size sumar, .-sumar

@ 4. Devuelve el mayor de dos enteros CON SIGNO.
.global mayor
.type mayor, %function
.thumb_func
mayor:
    CMP R0, R1
    BGE .L_mayor_fin
    MOVS R0, R1
.L_mayor_fin:
    BX LR
.size mayor, .-mayor

@ 5. Contador con comprobacion ANTES del cuerpo: while.
.global contar
.type contar, %function
.thumb_func
contar:
    MOVS R0, #0
.L_contar_condicion:
    CMP R0, #10
    BGE .L_contar_fin
    ADDS R0, #1
    B .L_contar_condicion
.L_contar_fin:
    BX LR
.size contar, .-contar
```

### 10.2 Archivo completo `main.c`

```c
#include <stdio.h>
#include "pico/stdlib.h"

extern int sumar_5_y_3(void);
extern int resta(void);
extern int sumar(int a, int b);
extern int mayor(int a, int b);
extern int contar(void);

int main(void)
{
    stdio_init_all();
    sleep_ms(2000);

    while (true)
    {
        printf("\n--- Practica 1: ensamblador en RP2040-Zero ---\n");
        printf("sumar_5_y_3() = %d\n", sumar_5_y_3());
        printf("resta() = %d\n", resta());
        printf("sumar(20, 7) = %d\n", sumar(20, 7));
        printf("sumar(-5, 3) = %d\n", sumar(-5, 3));
        printf("mayor(8, 3) = %d\n", mayor(8, 3));
        printf("mayor(5, 15) = %d\n", mayor(5, 15));
        printf("mayor(7, 7) = %d\n", mayor(7, 7));
        printf("mayor(-8, -3) = %d\n", mayor(-8, -3));
        printf("contar() = %d\n", contar());
        sleep_ms(2000);
    }
}
```

Guarda, compila y carga el nuevo UF2. El monitor repetirá aproximadamente cada dos segundos:

```text
--- Practica 1: ensamblador en RP2040-Zero ---
sumar_5_y_3() = 8
resta() = 13
sumar(20, 7) = 27
sumar(-5, 3) = -2
mayor(8, 3) = 8
mayor(5, 15) = 15
mayor(7, 7) = 7
mayor(-8, -3) = -3
contar() = 10
```

## 11. Analizar las funciones

### 11.1 Resta

```asm
SUBS R0, R0, R1
```

significa `R0 = R0 - R1`. El orden de los operandos importa: `20 - 7` no es igual a `7 - 20`.

### 11.2 Suma con argumentos

En `sumar` no cargamos constantes con `MOVS`: los valores **ya llegan desde C**. Escribir `MOVS R0, #5` al entrar reemplazaría el primer argumento.

### 11.3 Elegir el mayor

Para `mayor(5, 15)`:

| Paso | Acción | R0 | R1 |
|---|---|---:|---:|
| Entrada | C entrega los argumentos | 5 | 15 |
| `CMP R0, R1` | Compara sin modificar R0 ni R1 | 5 | 15 |
| `BGE .L_mayor_fin` | No salta, porque 5 no es mayor o igual que 15 | 5 | 15 |
| `MOVS R0, R1` | Copia el mayor a R0 | 15 | 15 |
| `BX LR` | Devuelve 15 | 15 | 15 |

Si el primer número ya es mayor o igual, el salto evita la copia y devuelve el valor que estaba en `R0`.

### 11.4 Ciclo con condición al principio

Nuestra función `contar` equivale a:

```c
int contador = 0;
while (contador < 10)
{
    contador++;
}
return contador;
```

| R0 antes de comparar | ¿R0 >= 10? | Acción |
|---:|---|---|
| 0 | No | Incrementa a 1 y regresa a la condición |
| 1 | No | Incrementa a 2 y regresa a la condición |
| … | … | … |
| 9 | No | Incrementa a 10 y regresa a la condición |
| 10 | Sí | Sale del ciclo y devuelve 10 |

El monitor muestra **solo el valor final**. Los valores intermedios existen durante la ejecución, pero no hemos programado su impresión. Tampoco es un temporizador de diez segundos: este ciclo se ejecuta rápidamente.

### Diferencia con comprobar al final

Este fragmento coloca la comparación después del incremento:

```asm
MOVS R0, #0
.L_repetir:
    ADDS R0, #1
    CMP R0, #10
    BLT .L_repetir
```

Su estructura corresponde a un `do…while`: el cuerpo se ejecuta al menos una vez. Con inicio 0 y límite 10 ambos ejemplos terminan en 10, pero **no son equivalentes para cualquier valor inicial**.

**Experimento:** cambia el valor inicial a 12 en el programa integrado. El `while` devuelve 12 sin incrementar. En el fragmento con comprobación al final, incrementaría primero a 13. Explica por qué y restaura el valor inicial a 0.

## 12. Instrucciones que conoceremos después

Esta sección es de consulta; no necesitas agregar estas operaciones al programa integrado.

| Grupo | Instrucciones | Función |
|---|---|---|
| Aritmética | `ADDS`, `SUBS`, `MULS` | Suma, resta y multiplicación |
| Lógica | `ANDS`, `ORRS`, `EORS` | AND, OR y XOR bit a bit |
| Desplazamientos | `LSLS`, `LSRS`, `ASRS` | Desplazar bits; ASRS conserva el signo |
| Memoria | `LDR`, `STR` | Leer y escribir palabras en memoria |
| Memoria de menor tamaño | `LDRB`, `STRB`, `LDRH`, `STRH` | Leer y escribir bytes o medias palabras |
| Llamadas y retorno | `BL`, `BX` | Llamar una función y transferir el control |
| Pila | `PUSH`, `POP` | Guardar y recuperar registros |

Ejemplo de multiplicación en la sintaxis unificada utilizada:

```asm
MOVS R0, #5
MOVS R1, #4
MULS R0, R1, R0      @ R0 = 20; destino coincide con el ultimo operando
```

Ejemplos lógicos sobre los bits bajos, representados con cuatro bits por claridad; los registros siguen teniendo 32 bits:

| Operación | A = 1010, B = 1100 | Resultado |
|---|---|---|
| AND | `A AND B` | `1000` |
| OR | `A OR B` | `1110` |
| XOR | `A XOR B` | `0110` |

Un desplazamiento lógico a la izquierda multiplica por dos si no se pierden bits significativos. Uno a la derecha divide un entero sin signo entre dos descartando el residuo. No asumas el mismo comportamiento para enteros negativos.

Los periféricos también pueden tener registros accesibles mediante direcciones de memoria. **Los registros de la CPU y los registros de un periférico no son lo mismo**. Estudiaremos su acceso cuando trabajemos con GPIO.

## 13. Ejercicios para entregar

Agrega las funciones nuevas al final de `operaciones.S`, declara sus prototipos en `main.c` y agrega un `printf` por prueba dentro del `while`. No cambies CMake porque los nombres de los dos archivos siguen siendo los mismos.

Para cada función usa su propia `.global`, `.type`, `.thumb_func`, etiqueta y `.size`, siguiendo los ejemplos completos. Los nombres de función deben coincidir exactamente entre C y ensamblador.

### Ejercicio 1. Suma de constantes

Crea `int operacion1(void)` para calcular **12 + 8** usando dos registros y `ADDS`. No devuelvas directamente una constante con el resultado precalculado.

### Ejercicio 2. Resta de constantes

Crea `int operacion2(void)` para calcular **25 - 7** mediante `SUBS`.

### Ejercicio 3. Más pruebas de parámetros

Usa la función `sumar` existente con `(3, 4)`, `(15, 8)` y `(100, 20)`. Predice cada resultado antes de cargar el programa.

### Ejercicio 4. Resta con parámetros

Crea `int restar(int a, int b)`. Debe devolver `a - b`. Prueba `(20, 7)`, `(7, 20)` y `(5, 5)`.

Ejemplo de integración en C:

```c
/* Junto a los otros prototipos, fuera de main: */
extern int restar(int a, int b);

/* Dentro del while de main: */
printf("restar(7, 20) = %d\n", restar(7, 20));
```

Estas dos partes se insertan en lugares distintos; no sustituyen el archivo completo.

### Ejercicio 5. Igualdad

Crea `int es_igual(int a, int b)`. Devuelve 1 si son iguales y 0 si son diferentes. Prueba `(6, 6)`, `(6, 2)` y `(-3, -3)`.

Pista: utiliza `CMP`, un salto condicional y un valor de retorno para cada caso. El salto debe evaluar las banderas de la comparación, antes de que otra operación las cambie.

### Ejercicio 6. Comparación con signo

Usa `mayor` con `(8, 3)`, `(4, 12)`, `(-9, -2)` y `(5, 5)`. Explica qué sucede en el caso de igualdad y por qué no puedes usar el signo menos como criterio aislado para elegir el mayor.

### Ejercicio 7. Contar hasta 20

Crea `int contar_20(void)` con condición al principio. Inicializa en 0, incrementa de uno en uno y devuelve 20. No reemplaces el ciclo por una sola carga de constante.

### Ejercicio 8. Seguimiento sin tarjeta

Determina el valor final de `R0` y completa una tabla por instrucción:

```asm
MOVS R0, #10
MOVS R1, #5
ADDS R0, R0, R1
SUBS R0, #3
```

### Ejercicio 9. Seguimiento de un ciclo

Escribe los valores que toma `R0`, incluyendo el inicial, y cuántas veces se ejecuta `ADDS`:

```asm
MOVS R0, #0
.L_ejercicio9:
    ADDS R0, #2
    CMP R0, #10
    BLT .L_ejercicio9
```

¿Qué cambia si el límite es 9? ¿Es necesario que el contador tome exactamente el valor del límite para salir?

### Ejercicio 10. Explicación

Explica la diferencia entre `MOVS R0, #5` y `MOVS R0, R1`, e indica qué valor debe tener `R1` antes de ejecutar la segunda para poder predecir su resultado.

## 14. Preguntas de comprensión

1. ¿Cuál es el modelo de nuestra tarjeta y cuál es su microcontrolador?
2. ¿Por qué `pico2` no es la configuración adecuada?
3. ¿Qué es un registro y cuántos bits tiene?
4. ¿Qué diferencia existe entre un registro de CPU y una variable almacenada en RAM?
5. ¿Qué representan `SP`, `LR` y `PC`?
6. ¿Qué indican el símbolo `#` y el símbolo `@` en los ejemplos?
7. ¿Qué diferencia existe entre una directiva, una etiqueta y una instrucción?
8. ¿Por qué usamos `.S`, `ASM` y `.thumb_func`?
9. ¿En qué registros llegan los dos argumentos enteros de `sumar`?
10. ¿Dónde se coloca el resultado antes de regresar a C?
11. ¿`CMP` modifica R0 o R1? ¿Qué modifica?
12. ¿Qué relación tiene `BEQ` con la bandera Z?
13. ¿Qué diferencia existe entre `B`, `BL` y `BX LR`?
14. ¿Por qué `BGE` es apropiado para los argumentos `int` de `mayor`?
15. ¿Qué diferencia hay entre comprobar un ciclo antes y después de su cuerpo?
16. ¿Por qué `contar()` imprime 10 y no todos los valores de 0 a 10?
17. ¿Por qué el LED WS2812 de la RP2040-Zero no se controla simplemente como un LED convencional?
18. ¿Qué parte de esta práctica está escrita en C y qué parte en ensamblador?

## 15. Problemas frecuentes

| Síntoma | Qué revisar |
|---|---|
| Configura para RP2350 o Pico 2 | Corregir `PICO_BOARD`, retirar una plataforma explícita incorrecta y limpiar/reconfigurar CMake |
| `undefined reference to sumar...` | Nombre exacto en C y ensamblador, `.global` y presencia de `operaciones.S` en `add_executable` |
| `multiple definition of main` | Solo incluir un archivo con `main`; retirar el archivo C anterior del ejecutable |
| Símbolo o función definida dos veces | Sustituir las versiones de cada etapa; no pegar dos copias de una función |
| No encuentra `main.c` | Renombrar el archivo real y actualizar el nombre en CMake |
| No encuentra Pico SDK o las herramientas | Recuperar el encabezado generado por la extensión y abrir el proyecto desde su entorno |
| Error de instrucción o inmediato | Revisar Cortex-M0+, forma de operandos y límites; no usar instrucciones de otros ARM sin verificar |
| Compila, pero no aparece texto | Confirmar USB stdio, UF2 reciente, cable de datos y puerto serial correcto |
| El puerto cambió después de cargar | Volver a seleccionar el puerto y abrir de nuevo el monitor |
| Muestra el programa anterior | Copiar el nuevo UF2 después de cada compilación correcta |
| `RPI-RP2` desaparece después de copiar | Es normal: terminó la carga y la tarjeta se reinició |
| No aparece `RPI-RP2` | Repetir BOOT + RESET o conectar con BOOT presionado; comprobar cable y puerto USB |
| LED integrado apagado | No es un indicador de fallo en esta práctica; comprobar la salida USB |

Si una compilación falla, **no cargues un UF2 antiguo** pensando que contiene los cambios. Lee primero el primer error de compilación, corrígelo y compila otra vez.

## 16. Evidencias y entrega

Entrega el proyecto y un reporte breve con:

1. Los archivos finales `main.c`, `operaciones.S` y `CMakeLists.txt`; conserva también `pico_sdk_import.cmake` dentro del proyecto.
2. Captura de la selección `waveshare_rp2040_zero`, del bloque con `ASM` y de las fuentes incluidas en CMake.
3. Captura de compilación exitosa.
4. Captura del monitor con los resultados del programa integrado y de los ejercicios implementados.
5. Tabla de seguimiento de la suma, una llamada a `mayor` y un ciclo.
6. Respuestas a los ejercicios de razonamiento y las preguntas de comprensión.
7. Una conclusión breve: explica cómo entra un dato a una función ensamblador, cómo se transforma y cómo vuelve a C.

Para las pruebas utiliza esta tabla:

| Función y argumentos | Resultado esperado antes de ejecutar | Resultado obtenido | ¿Coincide? |
|---|---|---|---|
| `sumar(3, 4)` | | | |
| `restar(7, 20)` | | | |
| `es_igual(6, 6)` | | | |
| `mayor(-9, -2)` | | | |
| `contar_20()` | | | |

**Criterio de logro:** el programa compila y se ejecuta en la RP2040-Zero, los resultados coinciden con el seguimiento manual y el alumno puede explicar las instrucciones que escribió.

## 17. Fuentes de consulta

- [Práctica original del curso](https://github.com/fortunatoramirez/ensamblador/blob/main/01-programacion-con-ensamblador.md).
- [Waveshare: RP2040-Zero, características y documentación](https://www.waveshare.com/wiki/RP2040-Zero).
- [Raspberry Pi: configuración oficial waveshare_rp2040_zero en Pico SDK](https://github.com/raspberrypi/pico-sdk/blob/master/src/boards/include/boards/waveshare_rp2040_zero.h).
- [Raspberry Pi: documentación del SDK de C/C++](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).
- [Arm: recursos y manuales de la familia Cortex-M](https://developer.arm.com/community/arm-community-blogs/b/architectures-and-processors-blog/posts/cortex-m-resources).

**Nota sobre la revisión:** el material se ajustó para la RP2040-Zero y se revisó la coherencia entre funciones, prototipos y resultados. La compilación con el Pico SDK instalado y la ejecución física se comprueban en el laboratorio; no se presentan como pruebas realizadas en una tarjeta durante esta revisión.

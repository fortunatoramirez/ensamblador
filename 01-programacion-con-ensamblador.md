# Práctica 1. Introducción al lenguaje ensamblador con RP2040

## 1. Objetivo

El objetivo de esta práctica es comprender los fundamentos necesarios para comenzar a programar un microcontrolador **RP2040 utilizando lenguaje ensamblador ARM**.

Al finalizar la práctica, el estudiante será capaz de:

- Identificar los principales elementos internos del RP2040.
- Comprender qué es una arquitectura de procesador.
- Identificar la arquitectura ARM Cortex-M0+.
- Comprender qué es un registro.
- Reconocer los registros `R0` a `R15`.
- Comprender el funcionamiento especial de `SP`, `LR` y `PC`.
- Conocer el concepto de banderas del procesador.
- Interpretar instrucciones básicas de ensamblador.
- Comprender operandos, registros, constantes y etiquetas.
- Realizar operaciones aritméticas utilizando registros.
- Comprender cómo funcionan las comparaciones y los saltos.
- Crear, compilar y cargar un programa que contenga código ensamblador en el RP2040.

---

# 2. Antes de programar: ¿qué es un microcontrolador?

Un microcontrolador es una computadora integrada dentro de un circuito.

Dentro del RP2040 existen, entre otras cosas:

- procesadores;
- memoria RAM;
- buses de comunicación;
- temporizadores;
- GPIO;
- convertidor analógico-digital;
- UART;
- SPI;
- I²C;
- PWM;
- USB;
- controladores DMA;
- bloques PIO.

Durante esta práctica nos concentraremos solamente en una parte:

**el procesador.**

Posteriormente utilizaremos ensamblador para comunicarnos directamente con periféricos como:

- GPIO;
- OLED;
- joystick;
- botones;
- buzzer.

---

# 3. El RP2040

El RP2040 es el microcontrolador utilizado en tarjetas como Raspberry Pi Pico y muchas otras tarjetas compatibles.

Su procesador está formado por:

```text
RP2040
│
├── Cortex-M0+  Core 0
│
└── Cortex-M0+  Core 1
```

Tiene **dos núcleos ARM Cortex-M0+**.

Por ahora solamente utilizaremos:

```text
Core 0
```

El segundo núcleo se estudiará posteriormente.

---

# 4. ¿Qué significa ARM Cortex-M0+?

Esta descripción tiene varias partes.

## ARM

ARM es una arquitectura de procesadores.

Una arquitectura determina, entre otras cosas:

- qué registros existen;
- qué instrucciones entiende el procesador;
- cómo se representan los datos;
- cómo se accede a la memoria;
- cómo se realizan saltos;
- cómo funcionan las interrupciones.

---

## Cortex-M

La familia Cortex-M está diseñada principalmente para:

- microcontroladores;
- sistemas embebidos;
- dispositivos electrónicos;
- control digital;
- sensores;
- automatización.

Nuestro procesador es:

```text
Cortex-M0+
```

Es uno de los Cortex-M más sencillos.

Esto resulta conveniente para aprender ensamblador porque su conjunto de instrucciones es relativamente pequeño.

---

# 5. Arquitectura utilizada

El Cortex-M0+ del RP2040 implementa la arquitectura:

```text
ARMv6-M
```

y ejecuta instrucciones del conjunto **Thumb** correspondiente a esta arquitectura.

No necesitamos memorizar esto todavía.

Por ahora debemos comprender:

```text
Programa
   ↓
Instrucciones ARM
   ↓
Cortex-M0+
   ↓
Hardware
```

---

# 6. ¿Qué es lenguaje ensamblador?

Supongamos que queremos realizar:

```text
5 + 3
```

En un lenguaje de alto nivel podríamos escribir:

```c
resultado = 5 + 3;
```

El procesador no entiende directamente esa expresión.

En ensamblador podemos hacer algo similar a:

```asm
MOVS R0, #5
MOVS R1, #3
ADDS R0, R0, R1
```

Estas instrucciones están mucho más cerca de las operaciones reales del procesador.

---

# 7. Máquina, ensamblador y C

Podemos visualizar los niveles de programación de esta manera:

```text
Lenguaje C
resultado = a + b;

        ↓ compilador

Ensamblador
ADDS R0, R0, R1

        ↓ ensamblador

Código máquina
bits que entiende el procesador

        ↓

Cortex-M0+
```

El programa que convierte instrucciones ensamblador a código máquina se denomina:

```text
assembler
```

De ahí proviene el término:

```text
Assembly language
```

o

```text
lenguaje ensamblador.
```

---

# 8. ¿Qué es un registro?

Este es uno de los conceptos más importantes de toda la materia.

Un **registro** es una pequeña memoria que se encuentra directamente dentro del procesador.

El procesador puede operar con los registros de manera extremadamente rápida.

Podemos imaginar un registro como una pequeña caja:

```text
R0
┌────────────────────────────────┐
│             valor              │
└────────────────────────────────┘
              32 bits
```

En Cortex-M0+ los registros principales tienen **32 bits**.

Por ejemplo:

```text
R0 = 5
R1 = 3
```

Entonces podemos solicitar:

```text
R0 = R0 + R1
```

Después:

```text
R0 = 8
R1 = 3
```

---

# 9. ¿Por qué utilizar registros?

La CPU realiza gran parte de sus operaciones utilizando registros.

Por ejemplo:

```asm
MOVS R0, #10
MOVS R1, #20
ADDS R2, R0, R1
```

Después de ejecutar estas instrucciones:

```text
R0 = 10
R1 = 20
R2 = 30
```

Podemos visualizarlo como:

```text
       ┌───────┐
10 ───►│  R0   │
       └───┬───┘
           │
           │
           ├──────┐
           │      │
           ▼      ▼
         ┌──────────┐
20 ──►R1 │   SUMA   │
         └────┬─────┘
              │
              ▼
           R2 = 30
```

---

# 10. Registros del Cortex-M0+

El procesador dispone de registros:

```text
R0
R1
R2
R3
R4
R5
R6
R7
R8
R9
R10
R11
R12
R13
R14
R15
```

Los primeros registros se utilizan normalmente para almacenar:

- números;
- variables temporales;
- direcciones;
- parámetros;
- resultados.

---

# 11. Registros R0 a R12

Los registros:

```text
R0 - R12
```

son registros de propósito general.

Por ejemplo:

```asm
MOVS R0, #8
MOVS R1, #2
ADDS R2, R0, R1
```

produce:

```text
R0 = 8
R1 = 2
R2 = 10
```

---

# 12. Tres registros especiales

Los últimos tres registros tienen funciones especiales:

```text
R13 = SP
R14 = LR
R15 = PC
```

Estudiaremos cada uno.

---

# 13. R13 — SP — Stack Pointer

`SP` significa:

```text
Stack Pointer
```

o:

```text
puntero de pila.
```

La pila es una zona de memoria que se utiliza para almacenar temporalmente información.

Por ejemplo:

```text
RAM

Direcciones altas
┌──────────────────┐
│                  │
│      Stack       │ ← SP
│                  │
├──────────────────┤
│                  │
│    variables     │
│                  │
└──────────────────┘
Direcciones bajas
```

Más adelante utilizaremos instrucciones como:

```asm
PUSH
POP
```

para trabajar con ella.

---

# 14. R14 — LR — Link Register

`LR` significa:

```text
Link Register
```

Este registro se utiliza principalmente cuando se llama una función.

Por ejemplo:

```asm
BL sumar
```

El procesador necesita recordar:

> ¿a dónde debo regresar cuando termine `sumar`?

Esa dirección se almacena en:

```text
LR
```

Después podemos regresar con:

```asm
BX LR
```

---

# 15. R15 — PC — Program Counter

`PC` significa:

```text
Program Counter
```

Es uno de los registros más importantes.

Contiene información relacionada con la dirección de la instrucción que está ejecutando el procesador.

Podemos imaginar el programa:

```asm
MOVS R0, #5
MOVS R1, #3
ADDS R0, R0, R1
SUBS R0, #1
```

El procesador normalmente avanza instrucción por instrucción:

```text
           PC
            ↓
MOVS R0,#5

MOVS R1,#3

ADDS R0,R0,R1

SUBS R0,#1
```

Posteriormente:

```text
MOVS R0,#5

           PC
            ↓
MOVS R1,#3

ADDS R0,R0,R1

SUBS R0,#1
```

Los saltos modifican el flujo normal del programa.

---

# 16. Algo muy importante: el símbolo #

En ensamblador encontraremos frecuentemente:

```asm
#5
#10
#25
```

El símbolo:

```text
#
```

indica un **valor inmediato**.

Por ejemplo:

```asm
MOVS R0, #5
```

significa:

```text
coloca el número 5 directamente en R0.
```

En cambio:

```asm
MOVS R0, R1
```

significa:

```text
copia a R0 el contenido de R1.
```

Observe la diferencia.

---

# 17. Primera instrucción: MOVS

Escribamos:

```asm
MOVS R0, #5
```

Podemos leerla como:

> mover el valor 5 al registro R0.

Después:

```text
R0 = 5
```

Otro ejemplo:

```asm
MOVS R1, #25
```

produce:

```text
R1 = 25
```

---

# 18. ¿Qué significa la S?

Encontraremos instrucciones como:

```asm
MOVS
ADDS
SUBS
```

La letra `S` indica que la operación también actualiza ciertas **banderas de estado** del procesador.

Estas banderas permiten responder preguntas como:

```text
¿el resultado fue cero?
¿el resultado fue negativo?
¿hubo acarreo?
¿existió desbordamiento?
```

Más adelante utilizaremos estas banderas para tomar decisiones.

---

# 19. Las banderas

Entre las banderas importantes tenemos:

```text
N = Negative
Z = Zero
C = Carry
V = Overflow
```

Podemos recordarlas como:

```text
N Z C V
```

---

## Bandera Z

`Z` significa:

```text
Zero
```

Si una operación produce cero:

```text
Z = 1
```

Ejemplo conceptual:

```text
5 - 5 = 0
```

por lo tanto:

```text
Z = 1
```

---

## Bandera N

`N` significa:

```text
Negative
```

Indica que el resultado se interpreta como negativo.

---

## Bandera C

`C` significa:

```text
Carry
```

Está relacionada con acarreo en operaciones aritméticas sin signo.

---

## Bandera V

`V` significa:

```text
Overflow
```

Está relacionada con desbordamiento en operaciones con signo.

No profundizaremos todavía en `C` y `V`.

---

# 20. Instrucción ADDS

La instrucción:

```asm
ADDS
```

realiza una suma.

Ejemplo:

```asm
MOVS R0, #5
MOVS R1, #3
ADDS R2, R0, R1
```

Significa:

```text
R2 = R0 + R1
```

Entonces:

```text
R2 = 5 + 3
R2 = 8
```

---

# 21. Instrucción SUBS

La instrucción:

```asm
SUBS
```

realiza una resta.

Ejemplo:

```asm
MOVS R0, #10
MOVS R1, #4
SUBS R2, R0, R1
```

Resultado:

```text
R2 = 6
```

---

# 22. Instrucción MULS

Permite realizar multiplicaciones.

Ejemplo:

```asm
MOVS R0, #5
MOVS R1, #4
MULS R0, R1
```

Conceptualmente:

```text
R0 = R0 × R1
```

Resultado:

```text
R0 = 20
```

---

# 23. Operaciones lógicas

Ensamblador permite trabajar directamente con bits.

Esto será extremadamente importante cuando controlemos hardware.

Algunas instrucciones son:

```text
ANDS
ORRS
EORS
```

---

## ANDS

Realiza una operación AND bit a bit.

Ejemplo:

```text
1010
1100
----
1000
```

---

## ORRS

Realiza OR bit a bit.

```text
1010
1100
----
1110
```

---

## EORS

Realiza XOR bit a bit.

```text
1010
1100
----
0110
```

---

# 24. Desplazamiento de bits

También existen instrucciones como:

```text
LSLS
LSRS
ASRS
```

---

## LSLS

Logical Shift Left.

Ejemplo:

```text
00000101
```

desplazado una posición:

```text
00001010
```

En muchos casos equivale a multiplicar un número sin signo por 2.

---

## LSRS

Logical Shift Right.

Ejemplo:

```text
00001010
```

se convierte en:

```text
00000101
```

En muchos casos equivale a dividir un número sin signo entre 2.

---

# 25. Comparaciones: CMP

Una instrucción extremadamente importante es:

```asm
CMP
```

`CMP` significa:

```text
compare
```

Ejemplo:

```asm
CMP R0, R1
```

Internamente se realiza una comparación equivalente a:

```text
R0 - R1
```

pero el resultado **no necesita almacenarse**.

Lo importante es que se actualizan las banderas.

---

# 26. Ejemplo de comparación

Supongamos:

```text
R0 = 10
R1 = 10
```

Ejecutamos:

```asm
CMP R0, R1
```

Como:

```text
10 - 10 = 0
```

se activa:

```text
Z = 1
```

Ahora podemos utilizar una instrucción condicional.

---

# 27. Saltos

Normalmente el procesador ejecuta:

```text
instrucción 1
instrucción 2
instrucción 3
instrucción 4
```

pero podemos cambiar la dirección de ejecución.

---

# 28. B — Branch

La instrucción:

```asm
B etiqueta
```

significa:

> saltar a la etiqueta indicada.

Ejemplo:

```asm
inicio:

    MOVS R0, #5

    B inicio
```

El programa nunca termina.

Su flujo es:

```text
inicio
  ↓
MOVS
  ↓
B inicio
  │
  └─────────────┐
                │
                ▼
              inicio
```

Esto crea un ciclo infinito.

---

# 29. Etiquetas

Una etiqueta es simplemente un nombre que identifica una dirección del programa.

Ejemplo:

```asm
repetir:

    ADDS R0, #1

    B repetir
```

`repetir` no es una instrucción.

Es un nombre colocado sobre una determinada posición del programa.

---

# 30. Saltos condicionales

Existen saltos que dependen de las banderas.

Algunos importantes son:

| Instrucción | Significado |
|---|---|
| `BEQ` | Branch if Equal |
| `BNE` | Branch if Not Equal |
| `BGT` | Branch if Greater Than |
| `BLT` | Branch if Less Than |
| `BGE` | Branch if Greater or Equal |
| `BLE` | Branch if Less or Equal |

Ejemplo:

```asm
CMP R0, R1
BEQ iguales
```

Se interpreta como:

```text
comparar R0 con R1

si son iguales:
    saltar a "iguales"
```

---

# 31. Ejemplo de un if en ensamblador

En C:

```c
if (a == b)
{
    resultado = 1;
}
```

Una representación semejante en ensamblador sería:

```asm
CMP R0, R1
BNE fin

MOVS R2, #1

fin:
```

Observe algo muy importante:

En ensamblador **no existe `if`**.

Nosotros construimos el `if` utilizando:

```text
CMP
+
salto condicional
```

---

# 32. Ejemplo de un while

Supongamos:

```c
while (contador < 10)
{
    contador++;
}
```

Conceptualmente podemos realizar:

```asm
MOVS R0, #0

ciclo:

    ADDS R0, #1

    CMP R0, #10

    BLT ciclo
```

Veamos qué sucede.

Inicialmente:

```text
R0 = 0
```

Después:

```text
R0 = 1
R0 = 2
R0 = 3
...
R0 = 10
```

Cuando llega a 10 ya no se realiza el salto.

---

# 33. Algunas instrucciones importantes del Cortex-M0+

No es necesario memorizar todas en esta práctica.

### Movimiento de datos

```text
MOVS
LDR
STR
LDRB
STRB
LDRH
STRH
```

### Aritmética

```text
ADDS
SUBS
MULS
CMP
CMN
```

### Operaciones lógicas

```text
ANDS
ORRS
EORS
BICS
MVNS
TST
```

### Desplazamientos

```text
LSLS
LSRS
ASRS
RORS
```

### Saltos

```text
B
BEQ
BNE
BCS
BCC
BMI
BPL
BVS
BVC
BHI
BLS
BGE
BLT
BGT
BLE
BL
BX
```

### Pila

```text
PUSH
POP
```

### Otras

```text
NOP
SVC
BKPT
CPSID
CPSIE
```

Durante el curso aprenderemos estas instrucciones conforme sean necesarias.

No se busca memorizar una tabla completa.

Lo importante es comprender qué problema resuelve cada grupo de instrucciones.

---

# 34. Memoria: LDR y STR

Hasta ahora nuestros datos viven dentro de registros.

Pero solamente tenemos unos pocos registros.

Los programas reales necesitan utilizar memoria RAM.

Aquí aparecen dos instrucciones fundamentales:

```text
LDR
STR
```

---

## LDR

`LDR` significa aproximadamente:

```text
Load Register
```

Carga información desde memoria hacia un registro.

```text
MEMORIA ─────► REGISTRO
```

---

## STR

`STR` significa aproximadamente:

```text
Store Register
```

Guarda el contenido de un registro en memoria.

```text
REGISTRO ─────► MEMORIA
```

Esto será importantísimo posteriormente para controlar los periféricos.

¿Por qué?

Porque en un microcontrolador muchos periféricos se controlan mediante **direcciones de memoria**.

Más adelante llegaremos a instrucciones semejantes a:

```asm
LDR R0, =direccion_gpio
LDR R1, [R0]
STR R1, [R0]
```

Pero todavía no utilizaremos GPIO directamente.

---

# 35. Primer programa real

Ahora crearemos nuestro primer programa.

Queremos que el RP2040 calcule:

```text
5 + 3
```

pero queremos que **la operación sea realizada en ensamblador**.

Utilizaremos un pequeño programa en C únicamente para:

1. iniciar el sistema;
2. llamar nuestra función ensamblador;
3. mostrar el resultado por USB.

El cálculo será realizado por el Cortex-M0+ ejecutando nuestras instrucciones ensamblador.

---

# 36. Preparación del entorno

Utilizaremos:

- Visual Studio Code;
- extensión oficial **Raspberry Pi Pico**;
- Pico SDK;
- compilador ARM;
- CMake.

En Windows 10/11 la extensión oficial de Raspberry Pi puede encargarse de preparar las herramientas necesarias.

---

# 37. Instalar Visual Studio Code

Instalar:

```text
Visual Studio Code
```

Abrir VS Code.

---

# 38. Instalar la extensión de Raspberry Pi Pico

Abrir:

```text
Extensions
```

Buscar:

```text
Raspberry Pi Pico
```

Seleccionar la extensión oficial publicada por:

```text
Raspberry Pi
```

e instalarla.

La extensión permite crear proyectos utilizando el SDK oficial del RP2040.

---

# 39. Crear el proyecto

Abrir la paleta de comandos:

```text
Ctrl + Shift + P
```

Buscar una opción semejante a:

```text
Raspberry Pi Pico: New Project
```

Crear un proyecto para:

```text
Raspberry Pi Pico / RP2040
```

Nombre sugerido:

```text
P01_Ensamblador
```

Guardar el proyecto en una carpeta fácil de localizar.

---

# 40. Estructura de nuestro proyecto

Tendremos principalmente:

```text
P01_Ensamblador/
│
├── CMakeLists.txt
├── main.c
└── operaciones.S
```

Observe la extensión:

```text
.S
```

Ese archivo contendrá nuestro programa ensamblador.

---

# 41. ¿Por qué utilizar .S?

Una extensión común para código ensamblador es:

```text
.s
```

También se utiliza:

```text
.S
```

La `S` mayúscula permite que el archivo pase por el preprocesador antes de ser ensamblado.

Para nuestras prácticas utilizaremos:

```text
.S
```

---

# 42. Crear operaciones.S

Crear el archivo:

```text
operaciones.S
```

y escribir:

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.global sumar_5_y_3
.type sumar_5_y_3, %function
.thumb_func

sumar_5_y_3:

    MOVS R0, #5
    MOVS R1, #3

    ADDS R0, R0, R1

    BX LR
```

No copie el programa sin entenderlo.

Analizaremos **cada línea**.

---

# 43. Primera línea

```asm
.syntax unified
```

Esta línea indica al ensamblador que utilizaremos la sintaxis unificada de ARM.

No es una instrucción que ejecute el procesador.

Es una **directiva para el ensamblador**.

Esto es importante:

```text
Instrucción
```

y

```text
Directiva
```

no son lo mismo.

---

# 44. Directiva .cpu

```asm
.cpu cortex-m0plus
```

Le indicamos al ensamblador el tipo de procesador para el cual estamos escribiendo código.

En nuestro caso:

```text
ARM Cortex-M0+
```

---

# 45. Directiva .thumb

```asm
.thumb
```

Indica que el código utiliza instrucciones Thumb.

---

# 46. Directiva .global

```asm
.global sumar_5_y_3
```

Indica que el símbolo:

```text
sumar_5_y_3
```

será visible fuera de este archivo.

Eso permitirá llamar nuestra función desde `main.c`.

---

# 47. Tipo de símbolo

```asm
.type sumar_5_y_3, %function
```

Indica que:

```text
sumar_5_y_3
```

representa una función.

---

# 48. .thumb_func

```asm
.thumb_func
```

Indica que la siguiente función contiene código Thumb.

---

# 49. Nuestra etiqueta

```asm
sumar_5_y_3:
```

Esto crea una etiqueta.

Podemos pensar que estamos diciendo:

```text
aquí comienza la función sumar_5_y_3
```

---

# 50. Primera instrucción ejecutada

```asm
MOVS R0, #5
```

Antes:

```text
R0 = ?
```

Después:

```text
R0 = 5
```

---

# 51. Segunda instrucción

```asm
MOVS R1, #3
```

Después:

```text
R0 = 5
R1 = 3
```

---

# 52. La suma

```asm
ADDS R0, R0, R1
```

Se interpreta:

```text
R0 = R0 + R1
```

Como:

```text
R0 = 5
R1 = 3
```

entonces:

```text
R0 = 8
```

---

# 53. ¿Por qué dejamos el resultado en R0?

Existe una convención utilizada por ARM para comunicar funciones.

Entre otras cosas:

```text
R0-R3
```

se utilizan frecuentemente para argumentos de funciones.

Y:

```text
R0
```

se utiliza normalmente para devolver un resultado sencillo.

Por lo tanto nuestra función devolverá:

```text
R0 = 8
```

---

# 54. Regresar de la función

La última instrucción es:

```asm
BX LR
```

Recordemos:

```text
LR = Link Register
```

Al llamar la función, `LR` contiene la información necesaria para regresar.

Por eso:

```asm
BX LR
```

significa esencialmente:

> regresar al código que llamó esta función.

---

# 55. El archivo main.c

Crear:

```text
main.c
```

con:

```c
#include <stdio.h>
#include "pico/stdlib.h"

extern int sumar_5_y_3(void);

int main()
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

---

# 56. ¿Qué hace la línea extern?

Observe:

```c
extern int sumar_5_y_3(void);
```

Estamos diciendo:

> existe una función llamada `sumar_5_y_3`, pero se encuentra definida en otro archivo.

Ese otro archivo es:

```text
operaciones.S
```

---

# 57. Cuando C llama ensamblador

Cuando se ejecuta:

```c
resultado = sumar_5_y_3();
```

ocurre conceptualmente:

```text
main.c
   │
   │ llamada
   ▼
sumar_5_y_3
   │
   ├─ MOVS R0,#5
   ├─ MOVS R1,#3
   ├─ ADDS R0,R0,R1
   │
   └─ BX LR
        │
        ▼
      main.c
```

Y `R0` contiene:

```text
8
```

C recibe ese 8 como resultado de la función.

---

# 58. Configuración de CMake

En `CMakeLists.txt` debemos asegurarnos de que el proyecto permita ensamblador.

Una parte importante debe incluir:

```cmake
project(P01_Ensamblador C CXX ASM)
```

Observe:

```text
ASM
```

Eso habilita archivos ensamblador.

También debemos agregar ambos archivos al ejecutable:

```cmake
add_executable(P01_Ensamblador
    main.c
    operaciones.S
)
```

Y utilizar:

```cmake
target_link_libraries(P01_Ensamblador
    pico_stdlib
)
```

Para utilizar comunicación USB:

```cmake
pico_enable_stdio_usb(P01_Ensamblador 1)
pico_enable_stdio_uart(P01_Ensamblador 0)
```

Finalmente:

```cmake
pico_add_extra_outputs(P01_Ensamblador)
```

---

# 59. Ejemplo completo de CMakeLists.txt

Dependiendo de cómo se haya generado el proyecto, algunas líneas ya estarán presentes.

Una configuración simplificada puede verse así:

```cmake
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(P01_Ensamblador C CXX ASM)

pico_sdk_init()

add_executable(P01_Ensamblador
    main.c
    operaciones.S
)

target_link_libraries(P01_Ensamblador
    pico_stdlib
)

pico_enable_stdio_usb(P01_Ensamblador 1)
pico_enable_stdio_uart(P01_Ensamblador 0)

pico_add_extra_outputs(P01_Ensamblador)
```

---

# 60. Compilar

Utilizar la opción de compilación de la extensión de Raspberry Pi Pico.

Al finalizar correctamente se generarán varios archivos.

Uno muy importante es:

```text
P01_Ensamblador.uf2
```

---

# 61. ¿Qué es el archivo UF2?

Es un formato que permite cargar fácilmente programas a tarjetas Raspberry Pi Pico y compatibles.

Contiene el programa que terminará almacenado en la memoria Flash de la tarjeta.

---

# 62. Cargar el programa

Para cargar mediante BOOTSEL:

1. Desconectar la tarjeta.
2. Mantener presionado el botón:

```text
BOOTSEL
```

3. Conectar el cable USB.
4. Soltar `BOOTSEL`.

La computadora mostrará una unidad USB.

Normalmente aparece como:

```text
RPI-RP2
```

Copiar el archivo:

```text
P01_Ensamblador.uf2
```

a esa unidad.

La tarjeta se reiniciará y comenzará a ejecutar el programa.

---

# 63. Monitor serial

Abrir el monitor serial correspondiente al puerto USB.

Después del inicio deberá observarse repetidamente:

```text
Resultado producido por ensamblador: 8
Resultado producido por ensamblador: 8
Resultado producido por ensamblador: 8
```

Lo importante es comprender que el cálculo:

```text
5 + 3
```

**NO lo realizó C.**

Fue realizado mediante:

```asm
MOVS R0, #5
MOVS R1, #3
ADDS R0, R0, R1
```

directamente por el Cortex-M0+.

---

# 64. Seguimiento manual del programa

Antes:

```text
R0 = ?
R1 = ?
```

Primera instrucción:

```asm
MOVS R0,#5
```

Estado:

```text
R0 = 5
R1 = ?
```

Segunda:

```asm
MOVS R1,#3
```

Estado:

```text
R0 = 5
R1 = 3
```

Tercera:

```asm
ADDS R0,R0,R1
```

Estado:

```text
R0 = 8
R1 = 3
```

Cuarta:

```asm
BX LR
```

Regresa a:

```text
main()
```

y devuelve:

```text
8
```

---

# 65. Segunda función: resta

Agregar al archivo ensamblador:

```asm
.global resta
.type resta, %function
.thumb_func

resta:

    MOVS R0, #20
    MOVS R1, #7

    SUBS R0, R0, R1

    BX LR
```

Responder antes de ejecutarla:

```text
¿Qué valor debe regresar R0?
```

Resultado esperado:

```text
13
```

---

# 66. Tercera función: parámetros

Ahora haremos algo todavía más interesante.

En lugar de colocar números fijos en ensamblador, C enviará los números.

Agregar:

```asm
.global sumar
.type sumar, %function
.thumb_func

sumar:

    ADDS R0, R0, R1

    BX LR
```

En C declarar:

```c
extern int sumar(int a, int b);
```

Y utilizar:

```c
int resultado = sumar(20, 7);
```

Cuando C llama:

```text
sumar(20,7)
```

los primeros parámetros llegan normalmente como:

```text
R0 = 20
R1 = 7
```

Nuestra función ejecuta:

```asm
ADDS R0,R0,R1
```

por lo tanto:

```text
R0 = 27
```

Después:

```asm
BX LR
```

devuelve:

```text
27
```

---

# 67. Comprendiendo parámetros

Observe lo interesante:

```c
sumar(20, 7)
```

se convierte conceptualmente en:

```text
R0 = 20
R1 = 7
```

Después nuestra CPU ejecuta:

```asm
ADDS R0,R0,R1
```

y produce:

```text
R0 = 27
```

---

# 68. Cuarta función: elegir el mayor

Ahora utilizaremos una comparación.

```asm
.global mayor
.type mayor, %function
.thumb_func

mayor:

    CMP R0, R1

    BGE r0_es_mayor

    MOVS R0, R1

r0_es_mayor:

    BX LR
```

En C:

```c
extern int mayor(int a, int b);
```

Probar:

```c
mayor(8, 3);
```

Resultado:

```text
8
```

Probar:

```c
mayor(5, 15);
```

Resultado:

```text
15
```

---

# 69. Analicemos el programa

Supongamos:

```text
R0 = 5
R1 = 15
```

Primera instrucción:

```asm
CMP R0,R1
```

compara:

```text
5 contra 15
```

Después:

```asm
BGE r0_es_mayor
```

pregunta:

```text
¿R0 es mayor o igual que R1?
```

No.

Por lo tanto continúa:

```asm
MOVS R0,R1
```

Ahora:

```text
R0 = 15
```

Y finalmente:

```asm
BX LR
```

El resultado es:

```text
15
```

---

# 70. Quinta función: un ciclo

Crear:

```asm
.global contar
.type contar, %function
.thumb_func

contar:

    MOVS R0, #0

ciclo:

    ADDS R0, #1

    CMP R0, #10

    BLT ciclo

    BX LR
```

Analizar qué valor contendrá:

```text
R0
```

cuando termine.

---

# 71. Seguimiento del ciclo

Inicialmente:

```text
R0 = 0
```

Después:

```text
R0 = 1
```

Compara:

```text
1 < 10
```

Sí.

Regresa.

Después:

```text
R0 = 2
```

Luego:

```text
3
4
5
6
7
8
9
10
```

Cuando:

```text
R0 = 10
```

la condición:

```text
R0 < 10
```

ya no es verdadera.

Por lo tanto no se ejecuta:

```asm
BLT ciclo
```

y continúa:

```asm
BX LR
```

---

# 72. Lo que acabamos de construir

Aunque parece muy sencillo, acabamos de construir en ensamblador conceptos equivalentes a:

```text
variables
operaciones
funciones
parámetros
if
while
return
```

pero trabajando casi directamente con el procesador.

---

# 73. Relación con programación de alto nivel

| Programación tradicional | Ensamblador |
|---|---|
| variable | registro o memoria |
| `a = 5` | `MOVS R0,#5` |
| `a+b` | `ADDS` |
| `a-b` | `SUBS` |
| `if` | `CMP` + salto |
| `while` | etiqueta + `CMP` + salto |
| función | etiqueta |
| `return` | valor en R0 + `BX LR` |
| parámetros | R0-R3 principalmente |

---

# 74. Ejercicio 1

Crear una función ensamblador llamada:

```text
operacion1
```

que calcule:

```text
12 + 8
```

y regrese el resultado.

No realizar la suma previamente.

Debe utilizar registros.

---

# 75. Ejercicio 2

Crear:

```text
operacion2
```

para calcular:

```text
25 - 7
```

---

# 76. Ejercicio 3

Crear una función:

```text
sumar
```

que reciba dos números desde C y devuelva su suma.

Probar:

```text
3 + 4
15 + 8
100 + 20
```

---

# 77. Ejercicio 4

Crear una función:

```text
restar
```

que reciba:

```text
R0 = número A
R1 = número B
```

y devuelva:

```text
A - B
```

---

# 78. Ejercicio 5

Crear una función:

```text
es_igual
```

que reciba dos números.

Debe devolver:

```text
1
```

si son iguales y:

```text
0
```

si son diferentes.

Pista:

```asm
CMP
BEQ
B
```

---

# 79. Ejercicio 6

Crear una función:

```text
mayor
```

que reciba dos números y regrese el mayor.

Ejemplo:

```text
mayor(8,3)  → 8
mayor(4,12) → 12
```

---

# 80. Ejercicio 7

Crear una función que cuente desde:

```text
0
```

hasta:

```text
20
```

utilizando:

```text
ADDS
CMP
BLT
```

Debe devolver:

```text
20
```

---

# 81. Ejercicio 8 — razonamiento

Sin utilizar la tarjeta, determine el resultado:

```asm
MOVS R0,#10
MOVS R1,#5

ADDS R0,R0,R1
SUBS R0,#3
```

Responder:

```text
R0 = ______
```

---

# 82. Ejercicio 9

Analice:

```asm
MOVS R0,#0

ciclo:

    ADDS R0,#2

    CMP R0,#10

    BLT ciclo
```

Escriba todos los valores que toma `R0`.

---

# 83. Ejercicio 10

Explique con sus propias palabras qué diferencia existe entre:

```asm
MOVS R0,#5
```

y:

```asm
MOVS R0,R1
```

---

# 84. Preguntas de comprensión

Responder individualmente:

1. ¿Qué es un registro?

2. ¿Cuántos bits tiene un registro de propósito general del Cortex-M0+?

3. ¿Qué función tiene R0?

4. ¿Qué significa SP?

5. ¿Qué significa LR?

6. ¿Qué significa PC?

7. ¿Qué significa el símbolo `#`?

8. ¿Qué hace `MOVS`?

9. ¿Qué hace `ADDS`?

10. ¿Qué hace `SUBS`?

11. ¿Qué hace `CMP`?

12. ¿Qué es una etiqueta?

13. ¿Qué hace `B`?

14. ¿Cuál es la diferencia entre `B` y `BEQ`?

15. ¿Qué significa la bandera Z?

16. ¿Qué instrucción se utiliza normalmente para regresar de nuestra función?

17. ¿En qué registro se devuelve normalmente un resultado entero sencillo?

18. ¿Por qué los registros son importantes?

19. ¿Qué diferencia existe entre un registro y la memoria RAM?

20. ¿Por qué ensamblador nos permite entender mejor el funcionamiento interno de un microcontrolador?

---

# 85. Evidencias de la práctica

Entregar:

### 1. Código fuente

```text
main.c
operaciones.S
CMakeLists.txt
```

### 2. Captura de compilación correcta

Debe observarse que el proyecto se compiló sin errores.

### 3. Captura del monitor serial

Mostrando los resultados de las funciones realizadas.

### 4. Tabla de seguimiento

Realizar manualmente una tabla como:

| Instrucción | R0 | R1 | R2 |
|---|---:|---:|---:|
| Inicio | ? | ? | ? |
| `MOVS R0,#5` | 5 | ? | ? |
| `MOVS R1,#3` | 5 | 3 | ? |
| `ADDS R2,R0,R1` | 5 | 3 | 8 |

### 5. Preguntas de comprensión

Responder las preguntas anteriores.

---

# 86. Conclusión

En esta práctica no se pretendió controlar todavía sensores, pantallas o videojuegos.

El objetivo fue comprender la parte fundamental que existe debajo de todo ello:

```text
CPU
 ↓
registros
 ↓
instrucciones
 ↓
comparaciones
 ↓
saltos
 ↓
memoria
```

Por primera vez utilizamos directamente instrucciones que puede ejecutar el núcleo Cortex-M0+:

```asm
MOVS
ADDS
SUBS
CMP
B
BEQ
BLT
BX
```

# Clase 1 — Introducción al Lenguaje Ensamblador desde Cero

> **Objetivo de la clase:** comprender qué es el lenguaje ensamblador, por qué existe, cómo se relaciona con el procesador y el código máquina, y llegar de manera gradual al concepto de **mnemónicos**.

---

## 1. Antes de hablar de ensamblador: ¿qué hace una computadora?

Una computadora ejecuta instrucciones.

Aunque normalmente programamos usando lenguajes como:

- C
- C++
- Python
- Java
- JavaScript

el procesador **no entiende directamente esos lenguajes**.

Un procesador trabaja internamente con instrucciones representadas mediante valores binarios.

Por ejemplo:

```text
10110000 01100001
```

Para nosotros esta secuencia no dice mucho, pero para cierto procesador puede representar una instrucción específica.

La idea principal que debemos entender es:

```text
Programa
   ↓
Instrucciones
   ↓
Procesador
   ↓
Operaciones
```

El procesador ejecuta instrucciones extremadamente simples, por ejemplo:

```text
Mover un número
Sumar dos números
Comparar dos números
Leer una posición de memoria
Guardar un dato
Saltar a otra instrucción
```

Un programa complejo está formado por **millones de operaciones simples** como estas.

---

# 2. El procesador

El **CPU** (*Central Processing Unit*) es el componente encargado de ejecutar las instrucciones de un programa.

De manera simplificada podemos imaginarlo así:

```text
              ┌───────────────────────┐
              │          CPU          │
              │                       │
Memoria ─────►│   Registros           │
              │                       │
              │   Unidad Aritmética   │
              │   y Lógica (ALU)      │
              │                       │
              │   Unidad de Control   │
              └───────────────────────┘
```

Algunos componentes importantes del procesador son:

- **Registros**
- **ALU**
- **Unidad de control**
- **Contador de programa**
- **Buses internos**

Durante este curso iremos entendiendo cada uno.

---

# 3. ¿Qué es una instrucción?

Una **instrucción** es una orden que el procesador sabe ejecutar.

Algunos ejemplos conceptuales serían:

```text
guardar el número 5
sumar dos números
restar dos números
comparar dos valores
leer memoria
escribir memoria
saltar a otra parte del programa
```

El procesador tiene un conjunto limitado de instrucciones que puede ejecutar.

Ese conjunto depende de la arquitectura del procesador.

---

# 4. Código máquina

El lenguaje que realmente ejecuta el procesador se conoce como:

> **Código máquina**

El código máquina está formado por patrones de bits.

Por ejemplo:

```text
11010010 10000000
```

o:

```text
00100000 00000101
```

Cada combinación puede representar cosas diferentes dependiendo del procesador.

Una parte de los bits puede indicar:

```text
qué operación realizar
```

y otra parte:

```text
sobre qué datos realizarla
```

Por ejemplo, conceptualmente una instrucción podría dividirse así:

```text
┌────────────┬────────────┐
│ Operación  │ Operando   │
├────────────┼────────────┤
│ 0010       │ 00000101   │
└────────────┴────────────┘
```

Esto podría significar algo parecido a:

```text
GUARDAR 5
```

El significado real depende completamente de la arquitectura del procesador.

---

# 5. ¿Por qué no programamos directamente en binario?

Imaginemos un programa escrito así:

```text
00100001 00000101
00100010 00000011
01000001 00000010
10010010 11110100
```

Es posible hacerlo, pero rápidamente aparecen problemas.

Es difícil:

- recordar qué significa cada combinación;
- detectar errores;
- modificar un programa;
- leer programas escritos por otra persona;
- trabajar con programas grandes.

Por esta razón apareció una forma más humana de representar las instrucciones del procesador.

Esa representación es el:

# Lenguaje Ensamblador

---

# 6. ¿Qué es el lenguaje ensamblador?

El **lenguaje ensamblador** (*Assembly Language*) es una representación textual de las instrucciones de un procesador.

En lugar de escribir:

```text
00100001 00000101
```

podemos escribir algo similar a:

```asm
MOV R0, #5
```

Esto es muchísimo más fácil de leer.

Conceptualmente:

```text
Código ensamblador
       ↓
Ensamblador
       ↓
Código máquina
       ↓
Procesador
```

El programa llamado **assembler** o **ensamblador** convierte las instrucciones escritas por el programador en código máquina.

---

# 7. Una diferencia importante

Cuando programamos en Python podemos escribir:

```python
resultado = 5 + 3
```

Cuando programamos en C:

```c
int resultado = 5 + 3;
```

Pero en ensamblador normalmente debemos trabajar más cerca del funcionamiento real del procesador.

Por ejemplo, conceptualmente:

```asm
MOV R0, #5
MOV R1, #3
ADD R2, R0, R1
```

Esto podría interpretarse como:

```text
Coloca 5 en R0
Coloca 3 en R1
Suma R0 + R1 y guarda el resultado en R2
```

Ahora aparece una pregunta importante:

> ¿Qué son R0, R1 y R2?

Para responderla necesitamos hablar de los **registros**.

---

# 8. Registros

Los **registros** son pequeñas posiciones de almacenamiento que se encuentran dentro del procesador.

Son extremadamente rápidos.

Podemos imaginarlos como pequeñas cajas:

```text
CPU

┌───────────────┐
│ R0 │    5     │
├───────────────┤
│ R1 │    3     │
├───────────────┤
│ R2 │    8     │
├───────────────┤
│ R3 │          │
└───────────────┘
```

En un programa ensamblador utilizaremos constantemente registros.

Por ejemplo:

```asm
MOV R0, #10
```

podría significar:

```text
R0 = 10
```

Después:

```asm
MOV R1, #20
```

significaría:

```text
R1 = 20
```

Y finalmente:

```asm
ADD R2, R0, R1
```

significaría:

```text
R2 = R0 + R1
```

Por lo tanto:

```text
R2 = 30
```

---

# 9. ¿Por qué usar registros y no solamente memoria RAM?

La memoria RAM tiene mucho más espacio que los registros.

Sin embargo, los registros están directamente dentro del procesador y pueden utilizarse de manera extremadamente rápida.

De manera muy simplificada:

```text
Registros → muy rápidos, poca capacidad
RAM       → más lenta, mucha más capacidad
```

Podemos imaginar el proceso así:

```text
RAM
 ↓
Registro
 ↓
Operación
 ↓
Registro
 ↓
RAM
```

Muchas instrucciones del procesador trabajan directamente con registros.

---

# 10. Memoria

La memoria puede imaginarse como una enorme colección de posiciones numeradas.

Por ejemplo:

```text
Dirección        Contenido

0x20000000       25
0x20000004       18
0x20000008       90
0x2000000C       42
```

Cada posición tiene una **dirección**.

La dirección permite localizar el dato.

Podemos pensar en ella como si fuera la dirección de una casa.

```text
Dirección → indica dónde está el dato
Dato      → información almacenada
```

En ensamblador trabajaremos frecuentemente con direcciones de memoria.

---

# 11. ¿Qué significa 0x?

En programación de bajo nivel es muy común utilizar números en **hexadecimal**.

Por ejemplo:

```text
0x20000000
```

El prefijo:

```text
0x
```

indica que el número está escrito en hexadecimal.

Los sistemas numéricos más importantes para ensamblador son:

| Sistema | Base | Ejemplo |
|---|---:|---|
| Binario | 2 | `10110110` |
| Decimal | 10 | `182` |
| Hexadecimal | 16 | `0xB6` |

El hexadecimal es muy utilizado porque representa de forma compacta grupos de bits.

Por ejemplo:

```text
Binario:      1111 1010
Hexadecimal:  FA
```

Por lo tanto:

```text
11111010₂ = 0xFA
```

---

# 12. Bits y bytes

Un **bit** solamente puede tener dos valores:

```text
0
```

o:

```text
1
```

Un grupo de 8 bits forma un:

> **byte**

Ejemplo:

```text
10110110
```

Un byte puede representar valores desde:

```text
0
```

hasta:

```text
255
```

si se interpreta como entero sin signo.

---

# 13. ¿Qué significa que un procesador sea de 32 bits?

En términos generales, indica que el procesador está diseñado para trabajar naturalmente con datos y registros de cierto tamaño.

En un procesador ARM Cortex-M de 32 bits, por ejemplo, muchos registros tienen:

```text
32 bits
```

Visualmente:

```text
R0

┌───────────────────────────────────────────────┐
│                32 bits                       │
└───────────────────────────────────────────────┘
```

Esos 32 bits pueden interpretarse de diferentes maneras:

```text
un número entero
una dirección de memoria
un conjunto de bits de control
parte de una operación
```

---

# 14. Arquitectura del procesador

No todos los procesadores entienden las mismas instrucciones.

Por ejemplo:

- ARM
- x86
- x86-64
- AVR
- RISC-V
- MIPS

son arquitecturas diferentes.

Cada arquitectura define, entre otras cosas:

- qué registros existen;
- qué instrucciones existen;
- cómo se codifican;
- cómo se accede a memoria;
- cómo se realizan saltos;
- cómo se pasan parámetros.

---

# 15. ISA — Instruction Set Architecture

El término **ISA** significa:

> **Instruction Set Architecture**

o:

> **Arquitectura del Conjunto de Instrucciones**

La ISA funciona como un contrato entre el software y el procesador.

Define las instrucciones que el procesador puede entender.

Por ejemplo, una ISA puede definir instrucciones equivalentes a:

```text
Mover
Sumar
Restar
Multiplicar
Comparar
Saltar
Leer memoria
Escribir memoria
```

En ensamblador veremos estas instrucciones utilizando palabras cortas.

Estas palabras reciben un nombre muy importante:

# Mnemónicos

---

# 16. ¿Qué es un mnemónico?

Un **mnemónico** es una palabra corta utilizada para representar una instrucción del procesador.

La palabra viene de la idea de algo que ayuda a recordar.

En lugar de memorizar una secuencia de bits como:

```text
1110000010000010...
```

utilizamos palabras como:

```asm
MOV
ADD
SUB
CMP
LDR
STR
B
```

Estas palabras representan operaciones.

Por ejemplo:

| Mnemónico | Significado aproximado |
|---|---|
| `MOV` | Move — mover/copiar un dato |
| `ADD` | Add — sumar |
| `SUB` | Subtract — restar |
| `CMP` | Compare — comparar |
| `LDR` | Load Register — cargar desde memoria |
| `STR` | Store Register — guardar en memoria |
| `B` | Branch — saltar |

> Los mnemónicos disponibles y su comportamiento exacto dependen de la arquitectura.

---

# 17. Primer ejemplo de mnemónico: MOV

Observemos:

```asm
MOV R0, #5
```

Podemos separar la instrucción:

```text
MOV     R0      #5
│       │       │
│       │       └── dato
│       └────────── destino
└────────────────── operación
```

El mnemónico es:

```asm
MOV
```

y normalmente significa algo parecido a:

```text
mover/copiar un valor
```

En este ejemplo:

```asm
MOV R0, #5
```

podemos interpretarlo como:

```text
R0 = 5
```

---

# 18. Operandos

Los valores sobre los que trabaja una instrucción reciben el nombre de:

> **operandos**

Por ejemplo:

```asm
ADD R2, R0, R1
```

tenemos:

```text
ADD → mnemónico

R2
R0  → operandos
R1
```

Conceptualmente:

```text
R2 = R0 + R1
```

---

# 19. Instrucciones con diferentes cantidades de operandos

Dependiendo de la arquitectura, una instrucción puede tener diferentes cantidades de operandos.

Por ejemplo:

```asm
MOV R0, #5
```

tiene dos operandos:

```text
R0
#5
```

Mientras:

```asm
ADD R2, R0, R1
```

tiene tres:

```text
R2
R0
R1
```

Podemos leerla como:

```text
Suma R0 y R1 y guarda el resultado en R2.
```

---

# 20. Valores inmediatos

En ejemplos de ARM es común encontrar:

```asm
#5
```

El símbolo `#` indica un **valor inmediato**.

Es decir, el número está escrito directamente dentro de la instrucción.

Ejemplo:

```asm
MOV R0, #10
```

significa:

```text
colocar directamente el valor 10 en R0
```

En cambio:

```asm
MOV R0, R1
```

significaría:

```text
copiar el contenido de R1 hacia R0
```

Comparemos:

```asm
MOV R0, #10
```

```text
R0 = 10
```

contra:

```asm
MOV R0, R1
```

```text
R0 = R1
```

---

# 21. Ejemplo completo

Consideremos:

```asm
MOV R0, #5
MOV R1, #3
ADD R2, R0, R1
```

Vamos instrucción por instrucción.

### Primera instrucción

```asm
MOV R0, #5
```

Estado:

```text
R0 = 5
```

---

### Segunda instrucción

```asm
MOV R1, #3
```

Estado:

```text
R0 = 5
R1 = 3
```

---

### Tercera instrucción

```asm
ADD R2, R0, R1
```

Se realiza:

```text
R2 = R0 + R1
```

Por lo tanto:

```text
R2 = 5 + 3
```

Resultado:

```text
R2 = 8
```

---

# 22. El ensamblador no es un lenguaje universal

Es muy importante entender esto desde el principio:

> No existe un único lenguaje ensamblador que funcione en todos los procesadores.

Cada familia de procesadores posee sus propias instrucciones.

Por ejemplo:

```text
ARM Assembly
x86 Assembly
AVR Assembly
RISC-V Assembly
MIPS Assembly
```

pueden utilizar mnemónicos y formatos diferentes.

Incluso cuando utilizan un mnemónico parecido, la forma exacta de escribir la instrucción puede cambiar.

---

# 23. ¿Qué ensamblador utilizaremos?

En este curso utilizaremos como referencia principalmente:

> **ARM Assembly**

especialmente pensando en microcontroladores basados en:

```text
ARM Cortex-M
```

como los presentes en muchas familias STM32.

Esto tiene una ventaja importante:

podremos observar directamente cómo las instrucciones del procesador interactúan posteriormente con:

- GPIO;
- LEDs;
- botones;
- temporizadores;
- memoria;
- interrupciones;
- periféricos.

Pero antes de controlar hardware necesitamos dominar primero los conceptos básicos.

---

# 24. Ensamblador y microcontroladores

En un microcontrolador tenemos algo parecido a:

```text
┌──────────────────────────────────────┐
│          MICROCONTROLADOR            │
│                                      │
│   ┌───────────┐                      │
│   │    CPU    │                      │
│   └───────────┘                      │
│                                      │
│   Memoria Flash                      │
│   Memoria RAM                        │
│                                      │
│   GPIO                               │
│   Timers                             │
│   ADC                                │
│   UART                               │
│   SPI                                │
│   I2C                                │
│   etc.                               │
│                                      │
└──────────────────────────────────────┘
```

El procesador ejecuta instrucciones.

Muchas de esas instrucciones permiten leer o modificar posiciones de memoria.

En un microcontrolador, determinadas posiciones de memoria están asociadas directamente a periféricos.

Por esta razón, más adelante podremos hacer cosas como:

```text
Escribir un registro
        ↓
Cambiar un bit
        ↓
Cambiar el estado de un GPIO
        ↓
Encender un LED
```

Ahí comienza una de las partes más interesantes del lenguaje ensamblador.

---

# 25. De un programa en C al procesador

Supongamos este código:

```c
int a = 5;
int b = 3;
int c = a + b;
```

El procesador no ejecuta directamente:

```c
c = a + b;
```

Un compilador transforma ese código en instrucciones de máquina.

Una representación simplificada podría parecerse a:

```asm
MOV R0, #5
MOV R1, #3
ADD R2, R0, R1
```

y finalmente estas instrucciones se convierten en código máquina.

El proceso es aproximadamente:

```text
Código C
   ↓
Compilador
   ↓
Assembly
   ↓
Assembler
   ↓
Código máquina
   ↓
CPU
```

En compiladores modernos algunos pasos pueden combinarse internamente, pero conceptualmente este modelo nos ayuda a comprender qué ocurre.

---

# 26. Compilador vs ensamblador

No debemos confundirlos.

## Compilador

Convierte un lenguaje de más alto nivel, por ejemplo:

```c
resultado = a + b;
```

a instrucciones de nivel más bajo.

---

## Ensamblador

Convierte código ensamblador como:

```asm
ADD R2, R0, R1
```

en código máquina.

---

# 27. ¿Qué significa realmente programar en ensamblador?

Programar en ensamblador significa trabajar muy cerca de la arquitectura del procesador.

El programador debe pensar en elementos como:

```text
registros
memoria
direcciones
bits
instrucciones
saltos
banderas
pila
periféricos
```

Por eso ensamblador ayuda enormemente a comprender cómo funciona una computadora por dentro.

---

# 28. Anatomía de una instrucción ensamblador

Una instrucción típica puede verse así:

```asm
ADD R2, R0, R1
```

Podemos dividirla en:

```text
┌───────────┬─────────────────┐
│ Mnemónico │ Operand(os)     │
├───────────┼─────────────────┤
│ ADD       │ R2, R0, R1      │
└───────────┴─────────────────┘
```

El **mnemónico** indica:

```text
qué operación se realizará
```

Los **operandos** indican:

```text
con qué datos se realizará
```

---

# 29. Algunos mnemónicos que encontraremos

Todavía no es necesario memorizarlos.

Por ahora solamente debemos reconocer qué representan.

```asm
MOV
ADD
SUB
MUL
CMP
LDR
STR
B
BL
AND
ORR
EOR
LSL
LSR
```

Algunos significados aproximados:

| Mnemónico | Idea |
|---|---|
| `MOV` | mover/copiar |
| `ADD` | sumar |
| `SUB` | restar |
| `MUL` | multiplicar |
| `CMP` | comparar |
| `LDR` | cargar un dato desde memoria |
| `STR` | guardar un dato en memoria |
| `B` | salto |
| `BL` | salto a subrutina |
| `AND` | operación AND bit a bit |
| `ORR` | operación OR bit a bit |
| `EOR` | XOR bit a bit |
| `LSL` | desplazamiento a la izquierda |
| `LSR` | desplazamiento a la derecha |

En las siguientes clases iremos estudiándolos con detalle.

---

# 30. Ejemplo visual

Código:

```asm
MOV R0, #7
MOV R1, #2
SUB R2, R0, R1
```

Después de la primera instrucción:

```text
R0 = 7
R1 = ?
R2 = ?
```

Después de la segunda:

```text
R0 = 7
R1 = 2
R2 = ?
```

Después de la tercera:

```text
R0 = 7
R1 = 2
R2 = 5
```

Porque:

```text
7 - 2 = 5
```

---

# 31. Algo importante: MOV no necesariamente significa mover físicamente

El nombre:

```asm
MOV
```

viene de:

```text
MOVE
```

Pero muchas veces es más útil imaginarlo como:

```text
copiar
```

Por ejemplo:

```asm
MOV R0, R1
```

normalmente no destruye automáticamente el contenido de `R1`.

Conceptualmente ocurre:

```text
R0 ← R1
```

Es decir:

```text
se copia el valor de R1 hacia R0
```

---

# 32. ¿Qué significa la flecha ← ?

Durante el curso utilizaremos esta notación:

```text
R0 ← 5
```

que significa:

```text
guardar 5 en R0
```

También:

```text
R2 ← R0 + R1
```

significa:

```text
sumar R0 y R1 y guardar el resultado en R2
```

Esta notación es útil porque permite explicar una instrucción sin depender todavía de la sintaxis exacta del procesador.

---

# 33. Programa conceptual

Supongamos que queremos calcular:

```text
(10 + 5) - 3
```

Podemos hacerlo así:

```asm
MOV R0, #10
MOV R1, #5
ADD R2, R0, R1

MOV R3, #3
SUB R4, R2, R3
```

Paso a paso:

```text
R0 = 10
R1 = 5

R2 = R0 + R1
R2 = 15

R3 = 3

R4 = R2 - R3
R4 = 12
```

Resultado:

```text
12
```

---

# 34. De instrucciones humanas a instrucciones del procesador

Podemos observar diferentes niveles de abstracción.

## Nivel alto

```python
resultado = (10 + 5) - 3
```

---

## Nivel intermedio

```c
int resultado = (10 + 5) - 3;
```

---

## Ensamblador

```asm
MOV R0, #10
MOV R1, #5
ADD R2, R0, R1

MOV R3, #3
SUB R4, R2, R3
```

---

## Código máquina

```text
bits
bits
bits
bits
bits
```

Finalmente, el procesador solamente ejecuta la última representación.

---

# 35. ¿Por qué aprender ensamblador?

Aprender ensamblador permite comprender mejor:

- cómo funciona un procesador;
- qué son los registros;
- cómo funciona la memoria;
- qué ocurre realmente al ejecutar un programa;
- cómo funcionan las llamadas a funciones;
- qué ocurre con las variables;
- cómo funciona una pila;
- cómo funcionan las interrupciones;
- cómo se controla hardware;
- cómo trabaja un compilador;
- cómo se representan datos mediante bits.

También ayuda mucho para comprender posteriormente:

```text
microcontroladores
sistemas embebidos
sistemas operativos
arquitectura de computadoras
drivers
depuración
optimización
seguridad informática
```

---

# 36. Conceptos que debemos dominar antes de continuar

Después de esta clase debes poder explicar con tus propias palabras:

1. ¿Qué es un procesador?
2. ¿Qué es una instrucción?
3. ¿Qué es código máquina?
4. ¿Por qué existe el lenguaje ensamblador?
5. ¿Qué es un registro?
6. ¿Qué es una dirección de memoria?
7. ¿Qué significa ISA?
8. ¿Qué es un mnemónico?
9. ¿Qué es un operando?
10. ¿Qué representa un valor inmediato?

---

# 37. Ejercicio 1

Analiza el siguiente código:

```asm
MOV R0, #4
MOV R1, #6
ADD R2, R0, R1
```

Contesta:

```text
¿Qué valor termina almacenado en R0?
¿Qué valor termina almacenado en R1?
¿Qué valor termina almacenado en R2?
```

---

# 38. Ejercicio 2

Analiza:

```asm
MOV R0, #20
MOV R1, #8
SUB R2, R0, R1
```

¿Con qué valor termina `R2`?

---

# 39. Ejercicio 3

Identifica el **mnemónico** y los **operandos** de:

```asm
ADD R3, R1, R2
```

---

# 40. Ejercicio 4

Explica la diferencia conceptual entre:

```asm
MOV R0, #5
```

y:

```asm
MOV R0, R1
```

---

# 41. Ejercicio 5

Sin escribir todavía código ensamblador, describe qué operaciones básicas tendría que realizar un procesador para calcular:

```text
resultado = (8 + 2) - 4
```

Piensa en:

```text
cargar valores
sumar
guardar resultados
restar
```

---

# 42. Resumen

En esta clase aprendimos la cadena fundamental:

```text
Problema
   ↓
Programa
   ↓
Instrucciones
   ↓
Código máquina
   ↓
Procesador
```

Y también:

```text
Código ensamblador
        ↓
Mnemónicos
        ↓
Instrucciones
        ↓
Código máquina
        ↓
CPU
```

Los puntos más importantes son:

- El procesador ejecuta **instrucciones**.
- Las instrucciones reales están codificadas como **bits**.
- Esa representación se llama **código máquina**.
- Escribir directamente código máquina es difícil.
- El lenguaje ensamblador utiliza nombres legibles para representar instrucciones.
- Esos nombres se llaman **mnemónicos**.
- Los datos utilizados por las instrucciones se llaman **operandos**.
- Los **registros** son pequeñas zonas de almacenamiento extremadamente rápidas dentro del procesador.
- Cada arquitectura posee su propio conjunto de instrucciones o **ISA**.

---

# 43. Lo que sigue

En la siguiente clase podemos comenzar a estudiar con más detalle la arquitectura **ARM Cortex-M**.

El siguiente paso natural será conocer:

```text
R0
R1
R2
...
R12
SP
LR
PC
```

y comprender para qué sirve cada registro.

Después podremos comenzar con instrucciones como:

```asm
MOV
ADD
SUB
CMP
B
```

antes de pasar a:

```asm
LDR
STR
```

y posteriormente controlar directamente memoria y periféricos de un microcontrolador STM32.

---

## Idea central de la clase

> **El lenguaje ensamblador no es una colección de comandos que hay que memorizar. Es una forma legible de expresar las instrucciones que el procesador realmente puede ejecutar.**


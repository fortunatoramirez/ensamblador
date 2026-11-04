# Práctica: lectura de una entrada analógica en ensamblador

**Asignatura:** Lenguaje ensamblador  
**Tarjeta:** Waveshare RP2040-Zero  
**Entorno:** VS Code, extensión Raspberry Pi Pico y Pico SDK  
**Arquitectura:** ARM Cortex-M0+, instrucciones Thumb (ARMv6-M)

## 1. Objetivo

Leer el voltaje de un potenciómetro conectado a la RP2040-Zero y observar su valor numérico en la computadora mediante USB. El ciclo de lectura se programará en ensamblador, con funciones pequeñas en C para acceder al ADC, imprimir el resultado y realizar una pausa.

Al terminar podrás explicar cómo se solicita una lectura, en qué registro se recibe y cómo se entrega a otra función para mostrarla.

## 2. Material

- RP2040-Zero y cable USB de datos.
- Potenciómetro de 10 kΩ.
- Protoboard y cables.

## 3. ¿Qué es una lectura analógica?

Una entrada analógica permite medir un voltaje que puede variar dentro de un intervalo. El potenciómetro nos permitirá cambiar ese voltaje girando su perilla.

El **ADC**, convertidor analógico a digital, transforma el voltaje en un número. En el RP2040 produce códigos de **12 bits**:

**2¹² = 4096 valores posibles: de 0 a 4095.**

Con una referencia nominal de 3.3 V, esperamos aproximadamente:

| Voltaje de entrada | Lectura ADC |
|---:|---:|
| 0 V | 0 |
| 0.825 V | 1024 |
| 1.65 V | 2048 |
| 2.475 V | 3072 |
| Cerca de 3.3 V | Cerca de 4095 |

El número que veremos en pantalla es un **código ADC**, no un voltaje expresado en volts. Para estimar el voltaje manualmente:

**Voltaje ≈ lectura × 3.3 / 4096**

Por ejemplo, una lectura de 2048 corresponde aproximadamente a **1.65 V**. Los valores reales pueden variar por ruido, tolerancias y diferencias en la referencia. Tener códigos de 12 bits no significa tener 12 bits de precisión perfecta.

## 4. Conectar el potenciómetro

1. Desconecta el USB antes de hacer las conexiones.
2. Identifica los dos extremos y el cursor del potenciómetro; en uno convencional de tres terminales, el cursor es la terminal central.
3. Conecta según la tabla.
4. Revisa las conexiones antes de alimentar la tarjeta.

| Terminal del potenciómetro | Conexión en la RP2040-Zero |
|---|---|
| Un extremo | 3V3 / 3.3 V |
| El otro extremo | GND |
| Cursor, terminal central | GPIO26 |

**No conectes 5 V a GPIO26.** Usa el pin 3V3 para alimentar el potenciómetro. Los números GPIO son nombres de señales, no posiciones contadas desde una esquina del conector.

Al girar la perilla, el cursor entrega una fracción del voltaje de alimentación. Si intercambias únicamente los dos extremos, se invierte el sentido del recorrido.

**GPIO26 y ADC0 no significan lo mismo:** GPIO26 es el pin físico de señal; ADC0 es el canal del convertidor asociado a ese pin. Para medir GPIO26 seleccionaremos el canal **0**, no el 26.

## 5. Crear el proyecto

1. Abre la extensión **Raspberry Pi Pico** en VS Code.
2. Selecciona **New C/C++ Project**.
3. Usa el nombre **`lectura_adc_asm`**.
4. Selecciona **`waveshare_rp2040_zero`**. Si no aparece, utiliza **`pico`** como alternativa para este ejercicio.
5. **No selecciones Pico 2**, porque corresponde a otro microcontrolador.
6. Selecciona lenguaje **C** y habilita la salida estándar por **USB**.
7. Crea el proyecto con las herramientas que ya utilizaste en las prácticas anteriores.
8. El archivo C principal deberá llamarse **`lectura_adc_asm.c`**. Si tiene otro nombre, renómbralo.
9. Crea en la misma carpeta el archivo **`programa.S`**, con **S mayúscula**.

### Configurar CMakeLists.txt

Conserva las líneas de configuración del SDK generadas por la extensión. Ajusta las secciones siguientes, sin duplicarlas.

Habilita ensamblador en la declaración del proyecto:

```cmake
project(lectura_adc_asm C CXX ASM)
```

Incluye ambos archivos en el ejecutable:

```cmake
add_executable(lectura_adc_asm
    lectura_adc_asm.c
    programa.S
)
```

Incluye las bibliotecas:

```cmake
target_link_libraries(lectura_adc_asm
    pico_stdlib
    hardware_adc
)
```

Comprueba estas opciones:

```cmake
pico_enable_stdio_usb(lectura_adc_asm 1)
pico_enable_stdio_uart(lectura_adc_asm 0)
pico_add_extra_outputs(lectura_adc_asm)
```

Utiliza el mismo nombre de ejecutable, `lectura_adc_asm`, en las demás opciones que haya generado la extensión. Si pide volver a configurar CMake, acepta.

## Código CMakeLists.txt Completo:
```c
# Version minima de CMake.
cmake_minimum_required(VERSION 3.13)

# Si la extension genero un bloque de configuracion
# de Pico para VS Code, conservarlo aqui.

# Tarjeta utilizada en la practica.
set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Board")

# Importar el SDK antes de declarar el proyecto.
include(pico_sdk_import.cmake)

# Nombre del proyecto y lenguajes habilitados.
# ASM permite compilar el archivo programa.S.
project(lectura_adc_asm C CXX ASM)

# Estandares de los lenguajes.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Inicializar el SDK de Raspberry Pi Pico.
pico_sdk_init()

# Archivos que forman el programa.
add_executable(lectura_adc_asm
    lectura_adc_asm.c
    programa.S
)

# Nombre y version del programa.
pico_set_program_name(lectura_adc_asm "lectura_adc_asm")
pico_set_program_version(lectura_adc_asm "1.0")

# Permitir encabezados ubicados en la carpeta del proyecto.
target_include_directories(lectura_adc_asm PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Bibliotecas necesarias:
# pico_stdlib: funciones generales, USB y temporizacion.
# hardware_adc: convertidor analogico a digital.
target_link_libraries(lectura_adc_asm
    pico_stdlib
    hardware_adc
)

# Mostrar printf por USB.
pico_enable_stdio_usb(lectura_adc_asm 1)

# Deshabilitar la salida estandar por UART.
pico_enable_stdio_uart(lectura_adc_asm 0)

# Generar el archivo UF2 para cargarlo en la tarjeta.
pico_add_extra_outputs(lectura_adc_asm)

```

## 6. Escribir el archivo C de apoyo

Pega este código completo en **`lectura_adc_asm.c`**:

```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Funcion escrita en programa.S.
extern void programa_asm(void);

void preparar_adc(void)
{
    adc_init();           // Inicializar el convertidor.
    adc_gpio_init(26);    // Preparar GPIO26 como entrada analogica.
    adc_select_input(0);  // Seleccionar ADC0, asociado a GPIO26.
}

uint32_t leer_adc(void)
{
    return adc_read();    // Hacer una conversion y devolver su resultado.
}

void mostrar_adc(uint32_t lectura)
{
    printf("ADC = %lu\n", (unsigned long)lectura);
}

void esperar_ms(uint32_t tiempo)
{
    sleep_ms(tiempo);
}

int main(void)
{
    stdio_init_all();     // Preparar la salida por USB.
    programa_asm();       // Entregar el control al ensamblador.
    return 0;
}
```

### ¿Para qué sirve cada función?

| Función | Qué hace |
|---|---|
| `preparar_adc()` | Inicializa el ADC y selecciona el canal que mediremos. |
| `leer_adc()` | Solicita una conversión, espera su resultado y devuelve el número. |
| `mostrar_adc(lectura)` | Escribe el número en la computadora y agrega un salto de línea. |
| `esperar_ms(tiempo)` | Hace una pausa indicada en milisegundos. |
| `main()` | Prepara USB y llama a nuestra rutina de ensamblador. |

Estas funciones son puentes hacia el SDK. Varias funciones del SDK son `static inline`; los puentes nos permiten llamarlas desde ensamblador mediante nombres de funciones definidos en nuestro archivo C.

La práctica usa **ensamblador con apoyo del SDK en C**. La conversión la realiza el periférico ADC; el ensamblador organiza cuándo prepararlo, leerlo, mostrar su resultado y repetir.

## 7. Escribir el código ensamblador

Pega este código completo en **`programa.S`**:

```asm
.syntax unified
.cpu cortex-m0plus
.thumb

.section .text
.balign 4
.global programa_asm
.type programa_asm, %function
.thumb_func

programa_asm:
    push {r4, lr}          @ Guardar dos registros: 8 bytes.
    bl preparar_adc       @ Configurar el ADC una sola vez.

ciclo_lectura:
    bl leer_adc           @ La lectura obtenida regresa en r0.
    bl mostrar_adc        @ Usar ese mismo r0 como argumento.

    ldr r0, =500          @ Preparar una pausa de 500 ms.
    bl esperar_ms         @ Realizar la pausa.

    b ciclo_lectura       @ Repetir desde una nueva lectura.

.size programa_asm, .-programa_asm
.ltorg
```

## 8. Explicación paso a paso del ensamblador

### 8.1. Los registros

Un registro es un espacio de almacenamiento dentro del procesador. En esta práctica nos interesan:

| Registro | Uso |
|---|---|
| `r0` | Recibe el resultado de `leer_adc` y sirve para pasar el primer argumento a otras funciones. |
| `lr` | Guarda la dirección de regreso de una llamada. |
| `sp` | Indica la posición actual de la pila. |
| `r4` | Se guarda junto con `lr` para mantener la pila alineada; aquí no almacena la lectura. |

### 8.2. `programa_asm:`: punto de entrada

Es una **etiqueta**, un nombre que identifica una posición del programa. Cuando el archivo C ejecuta `programa_asm()`, comienza a ejecutar las instrucciones de esta etiqueta.

### 8.3. `push {r4, lr}`: guardar en la pila

La pila es una zona de memoria utilizada para guardar datos temporalmente. `push` almacena los registros indicados y actualiza `sp`.

Cada registro ocupa cuatro bytes; guardar dos ocupa **ocho bytes**. Esto mantiene la alineación de ocho bytes requerida en las llamadas a funciones C, suponiendo la alineación correcta al entrar desde C.

Aquí se guarda `r4` junto con `lr` para mantener esa alineación. No necesitamos usar `r4` para la medición.

El `push` se ejecuta **una sola vez**, antes del ciclo. Si lo pusieras dentro del ciclo, reservarías memoria en cada vuelta hasta agotar la pila.

Nuestra rutina repite indefinidamente y no regresa a `main()`. Por eso no hay un `pop`. En una versión que terminara y regresara a C, habría que restaurar la pila y regresar, por ejemplo mediante `pop {r4, pc}`.

### 8.4. `bl preparar_adc`: llamar a una función

`bl` significa **Branch with Link**. Salta a una función y guarda en `lr` la dirección donde debe continuar al regresar.

`preparar_adc` configura el convertidor y selecciona GPIO26. Cuando termina, la ejecución continúa después de esta llamada.

La configuración está antes de `ciclo_lectura`, de modo que se hace una sola vez.

### 8.5. `ciclo_lectura:`: inicio de las acciones repetidas

Esta etiqueta indica el punto al que volveremos después de mostrar cada lectura. Una etiqueta no realiza una acción por sí misma: únicamente identifica una posición.

### 8.6. `bl leer_adc`: obtener el número

La función realiza una conversión y devuelve el resultado en **`r0`**, según la convención de llamadas utilizada por C y ensamblador.

Por ejemplo, si el resultado fue 1300, al regresar tenemos:

**r0 = 1300**

La lectura ya está disponible para la siguiente instrucción.

### 8.7. `bl mostrar_adc`: mostrar el resultado

El primer argumento de una función se pasa en **`r0`**. Como `leer_adc` acaba de dejar su resultado ahí, podemos llamar inmediatamente a `mostrar_adc`.

Estas dos instrucciones trabajan juntas:

```asm
bl leer_adc
bl mostrar_adc
```

La primera deja la lectura en `r0`; la segunda recibe ese valor y lo imprime. Si la lectura fue 1300, veremos:

```text
ADC = 1300
```

**No necesitamos copiar la lectura a otro registro**, porque la mostramos inmediatamente y no la utilizamos después. Los registros `r0` a `r3` pueden cambiar durante una llamada: al regresar de `mostrar_adc`, ya no debemos suponer que `r0` conserva la lectura.

Si más adelante necesitáramos usarla después de imprimirla, tendríamos que conservarla antes, por ejemplo en un registro preservado como `r4`.

### 8.8. `ldr r0, =500`: preparar el tiempo de espera

La forma con `=` es una **pseudoinstrucción**: solicita al ensamblador que coloque el valor 500 en `r0` mediante una secuencia adecuada.

Ahora `r0` tiene otra función: ya no transporta una lectura; transporta el tiempo de espera.

**r0 = 500 milisegundos**

`ldr r0, =500` carga la constante 500; no significa leer el contenido de la dirección de memoria 500. Usamos esta forma porque la instrucción inmediata sencilla `movs r0, #valor` del Cortex-M0+ admite valores de 0 a 255, y 500 queda fuera de ese intervalo.

### 8.9. `bl esperar_ms`: esperar

La función recibe 500 en `r0` y hace una pausa de medio segundo. Así la pantalla no se llena demasiado rápido y podemos observar los cambios.

### 8.10. `b ciclo_lectura`: volver a medir

`b` es un salto que continúa la ejecución en la etiqueta indicada. A diferencia de `bl`, no guarda una dirección de regreso en `lr`.

La secuencia queda así:

1. Leer el ADC.
2. Mostrar el resultado.
3. Esperar 500 ms.
4. Volver a leer.

El intervalo entre lecturas será algo mayor de 500 ms porque también se ejecutan la conversión, la impresión y las demás instrucciones. Esta práctica no implementa un muestreo de tiempo exacto.

### 8.11. Directivas y comentarios

Las líneas que empiezan con punto son instrucciones para el **ensamblador**, no operaciones que el procesador repite en el ciclo.

| Directiva | Función |
|---|---|
| `.syntax unified` | Selecciona la sintaxis del archivo. |
| `.cpu cortex-m0plus` | Indica el procesador del RP2040. |
| `.thumb` | Selecciona instrucciones Thumb. |
| `.section .text` | Coloca el contenido en la sección de código. |
| `.balign 4` | Alinea el inicio a un múltiplo de cuatro bytes. |
| `.global programa_asm` | Permite encontrar la función desde el archivo C. |
| `.type programa_asm, %function` | Identifica el símbolo como una función. |
| `.thumb_func` | Marca la siguiente función como una función Thumb. |
| `.size programa_asm, .-programa_asm` | Registra el tamaño del código de la función. |
| `.ltorg` | Coloca las constantes pendientes que necesite el ensamblador. |

`.ltorg` está después del salto incondicional para que la ejecución normal no continúe sobre esas constantes. El símbolo `@` inicia un comentario que no se ejecuta.

## 9. Compilar y cargar

1. Guarda `lectura_adc_asm.c`, `programa.S` y `CMakeLists.txt`.
2. Ejecuta **Build/Compile** desde la extensión Pico.
3. Corrige cualquier error antes de continuar.
4. Desconecta la tarjeta.
5. Mantén presionado **BOOT** y conecta el cable USB.
6. Suelta BOOT cuando aparezca la unidad **RPI-RP2**.
7. Copia a esa unidad el archivo **`lectura_adc_asm.uf2`**, normalmente dentro de `build`.
8. La unidad desaparecerá y el programa comenzará a ejecutarse.

## 10. Observar las lecturas

1. Abre un monitor serial y selecciona el puerto USB de la tarjeta. En Windows suele aparecer como un puerto COM.
2. Si solicita velocidad, selecciona **115200**. Usamos USB CDC: ese ajuste no determina una velocidad física UART.
3. Observa las líneas `ADC = ...`.
4. Gira lentamente el potenciómetro de un extremo al otro.
5. Detén la perilla en distintas posiciones y observa el valor.

Ejemplo ilustrativo de salida, no mediciones reales:

```text
ADC = 18
ADC = 720
ADC = 1580
ADC = 2052
ADC = 3120
ADC = 4080
```

Se imprime continuamente, por lo que puedes abrir el monitor después de que el programa haya iniciado. Cerca de los extremos esperamos valores próximos a 0 y 4095; cerca de la mitad, alrededor de 2048. No es necesario que coincidan exactamente. Pequeñas variaciones con la perilla quieta son normales.

## 11. Actividades

1. Registra la lectura en cinco posiciones aproximadas del potenciómetro.
2. Estima el voltaje de cada lectura con la fórmula indicada.
3. Cambia la pausa de 500 a **1000 ms**, compila y carga de nuevo. Describe qué cambia en la pantalla.
4. Después prueba una pausa de **200 ms**. Explica por qué esto cambia la frecuencia con la que ves nuevas lecturas, pero no la resolución de 12 bits del ADC.

| Posición aproximada | Lectura observada | Voltaje estimado: lectura × 3.3 / 4096 |
|---|---:|---:|
| Mínimo | | |
| Un cuarto del recorrido | | |
| Mitad | | |
| Tres cuartos del recorrido | | |
| Máximo | | |

## 12. Preguntas de comprensión

1. ¿Qué hace el ADC?
2. ¿Por qué una lectura de 2048 no significa 2048 volts?
3. ¿Cuál es la diferencia entre GPIO26 y ADC0?
4. ¿En qué registro recibimos el resultado de `leer_adc`?
5. ¿Por qué podemos llamar a `mostrar_adc` inmediatamente después de `leer_adc`?
6. ¿Qué contiene `r0` después de ejecutar `ldr r0, =500`?
7. ¿Qué diferencia hay entre `b` y `bl`?
8. ¿Por qué `preparar_adc` y `push` están fuera del ciclo?

## 13. Entrega

- Archivos `lectura_adc_asm.c`, `programa.S` y `CMakeLists.txt`.
- Fotografía de las conexiones del potenciómetro.
- Captura o video donde se observe el cambio de lectura al girar la perilla.
- Tabla de mediciones completa y respuestas a las preguntas.

Debes poder explicar el recorrido del dato: **potenciómetro → ADC → r0 → función que imprime → monitor serial**.

## 14. Solución de problemas

| Problema | Qué revisar |
|---|---|
| No aparece RPI-RP2 | Cable de datos y BOOT presionado antes de conectar. |
| `undefined reference to programa_asm` | `programa.S` incluido en `add_executable`, nombre correcto y directiva `.global`. |
| Error al enlazar ADC | Biblioteca `hardware_adc` incluida en CMake. |
| Error de instrucciones | Proyecto para RP2040, Cortex-M0+ y lenguaje ASM habilitado. |
| No aparece texto | Salida USB habilitada, COM correcto y ningún otro programa ocupando el puerto. |
| Lectura siempre cerca de cero o del máximo | Cursor central, GPIO26, 3V3 y GND conectados correctamente. |
| Cambios grandes con el potenciómetro quieto | Cables flojos o entrada desconectada. |
| Los cambios de código no se reflejan | Compilar de nuevo y cargar el UF2 recién generado. |

## Referencias técnicas

- [RP2040-Zero: documentación y pinout de Waveshare](https://www.waveshare.com/wiki/RP2040-Zero).
- [Pico SDK: ADC y GPIO](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html).
- [Código oficial de hardware_adc](https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/hardware_adc/include/hardware/adc.h).
- [Hoja de datos del RP2040](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf).

**Nota de validación:** código revisado estáticamente; pendiente de compilación con Pico SDK y prueba física en la tarjeta.

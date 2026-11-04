# Práctica: dos LED alternados y lectura analógica en ensamblador

**Asignatura:** Lenguaje ensamblador  
**Tarjeta:** Waveshare RP2040-Zero  
**Entorno:** VS Code, extensión Raspberry Pi Pico y Pico SDK  
**Arquitectura:** ARM Cortex-M0+, instrucciones Thumb (ARMv6-M)

## 1. ¿Qué vamos a aprender?

En esta práctica programaremos dos comportamientos:

1. **Parte 1:** encender un LED mientras el otro está apagado. Cada 500 ms intercambiarán su estado.
2. **Parte 2:** leer un potenciómetro. En la mitad inferior de su recorrido encenderá el LED 1; en la mitad superior encenderá el LED 2. También veremos la lectura en la computadora.

La secuencia, los ciclos y la decisión se escribirán en **ensamblador**. Usaremos un archivo C de apoyo para inicializar el hardware y llamar a funciones del SDK. No es un programa completamente escrito en ensamblador ni una práctica de acceso directo a registros de periféricos: el objetivo es aprender primero las instrucciones y las llamadas a funciones sin configurar a mano todo el microcontrolador.

## 2. Material

- RP2040-Zero y cable USB **de datos**.
- Protoboard y cables.
- Dos LED, preferentemente de diferente color.
- Dos resistencias de **330 Ω**, una por LED.
- Un potenciómetro de **10 kΩ**.

**Trabajaremos con 3.3 V. No conectes 5 V al GPIO26 ni a ningún GPIO. Desconecta el USB antes de modificar el circuito.** Los números GPIO identifican señales; no son posiciones contadas desde una esquina del conector.

## 3. Conectar los dos LED

Usaremos los pines marcados **0**, **1** y **GND** en la RP2040-Zero. Consulta la serigrafía y el pinout de tu tarjeta.

| Elemento | Conexión |
|---|---|
| LED 1 | GPIO0 → resistencia de 330 Ω → ánodo del LED 1 |
| LED 2 | GPIO1 → resistencia de 330 Ω → ánodo del LED 2 |
| Cátodos de ambos LED | GND de la tarjeta |

El ánodo suele ser la terminal larga; el cátodo suele ser la corta y coincide con el lado plano de la cápsula. Si las terminales están cortadas, revisa el lado plano o comprueba la polaridad.

Con esta conexión, un nivel **1** en la salida enciende el LED y un nivel **0** lo apaga. Cada LED necesita su propia resistencia. Usamos LED externos; el LED RGB integrado de la tarjeta se maneja de otra manera.

## 4. Crear el proyecto

1. Abre VS Code y la extensión **Raspberry Pi Pico**.
2. Selecciona **New C/C++ Project**.
3. Escribe el nombre **`leds_adc_asm`**.
4. Selecciona la tarjeta **`waveshare_rp2040_zero`**. Si no aparece, utiliza **`pico`** como alternativa para este ejercicio con periféricos externos.
5. **No selecciones Pico 2:** utiliza un microcontrolador diferente.
6. Selecciona lenguaje **C** y habilita la salida estándar por **USB**.
7. Crea el proyecto usando el SDK y las herramientas que ya utilizaste en las prácticas anteriores.
8. En la carpeta principal identifica el archivo C generado y `CMakeLists.txt`.
9. Utilizaremos solamente un archivo C con `main()`: **`leds_adc_asm.c`**. Si el generado tiene otro nombre, renómbralo.
10. Crea, en la misma carpeta, **`programa.S`**. La extensión es una **S mayúscula**.

### Ajustar CMakeLists.txt

Conserva la configuración del SDK y las herramientas generada por la extensión. Modifica únicamente las secciones indicadas; no pegues un segundo `project()` o un segundo `add_executable()`.

Comprueba que `project` incluya ASM:

```cmake
project(leds_adc_asm C CXX ASM)
```

Modifica el bloque del ejecutable para que incluya los dos archivos:

```cmake
add_executable(leds_adc_asm
    leds_adc_asm.c
    programa.S
)
```

En el bloque de bibliotecas incluye:

```cmake
target_link_libraries(leds_adc_asm
    pico_stdlib
    hardware_adc
)
```

Comprueba que también estén estas líneas, sin duplicarlas:

```cmake
pico_enable_stdio_usb(leds_adc_asm 1)
pico_enable_stdio_uart(leds_adc_asm 0)
pico_add_extra_outputs(leds_adc_asm)
```

Desactivamos la salida estándar UART porque usaremos GPIO0 y GPIO1 para los LED. La comunicación con la computadora será por USB. Si la extensión pregunta si deseas volver a configurar CMake, acepta.

## 5. Archivo C de apoyo: se utiliza en ambas partes

Reemplaza el contenido de **`leds_adc_asm.c`** por lo siguiente:

```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_1 0
#define LED_2 1
#define PIN_ADC 26

// Esta funcion esta definida en programa.S.
extern void programa_asm(void);

// Recibe un patron: bit 0 = LED 1, bit 1 = LED 2.
void escribir_leds(uint32_t patron)
{
    gpio_put_masked(0x03u, patron);
}

void esperar_ms(uint32_t tiempo)
{
    sleep_ms(tiempo);
}

void preparar_adc(void)
{
    adc_init();
    adc_gpio_init(PIN_ADC);
    adc_select_input(0);  // Canal ADC0 corresponde a GPIO26.
}

uint32_t leer_adc(void)
{
    return adc_read();
}

void mostrar_adc(uint32_t lectura)
{
    printf("ADC = %lu\n", (unsigned long)lectura);
}

int main(void)
{
    stdio_init_all();

    gpio_init(LED_1);
    gpio_init(LED_2);
    escribir_leds(0);  // Cargar un cero antes de habilitar las salidas.
    gpio_set_dir(LED_1, GPIO_OUT);
    gpio_set_dir(LED_2, GPIO_OUT);

    programa_asm();
    return 0;
}
```

### ¿Qué hace este archivo?

`main()` prepara los dos GPIO y entrega el control a `programa_asm()`. Desde ese momento, la rutina de ensamblador decide qué hacer y cuándo hacerlo.

Las otras funciones son pequeños **puentes** hacia el SDK:

| Función | Para qué sirve |
|---|---|
| `escribir_leds(patron)` | Actualiza los dos LED usando los bits del patrón. |
| `esperar_ms(tiempo)` | Espera la cantidad indicada de milisegundos. |
| `preparar_adc()` | Prepara el convertidor y selecciona GPIO26. |
| `leer_adc()` | Hace una conversión y devuelve su resultado. |
| `mostrar_adc(lectura)` | Imprime el número por USB. |

Varias funciones del SDK están definidas como `static inline`. Estos puentes ofrecen nombres de funciones que el archivo ensamblador puede llamar de forma explícita y sencilla.

### ¿Qué significa la máscara 0x03?

`0x03` equivale a `00000011` en binario. Sus bits 0 y 1 indican que `gpio_put_masked` modificará **GPIO0 y GPIO1**. El resto de los GPIO no se modifica con esta llamada.

El patrón indica el estado deseado:

| Patrón decimal | Bits 1 y 0 | GPIO1 / LED 2 | GPIO0 / LED 1 |
|---:|:---:|:---:|:---:|
| 0 | `00` | Apagado | Apagado |
| 1 | `01` | Apagado | Encendido |
| 2 | `10` | Encendido | Apagado |
| 3 | `11` | Encendido | Encendido |

En la práctica alternaremos entre **1 y 2**. El patrón no es el número de un pin: es un conjunto de bits. La llamada actualiza ambas salidas juntas; no hacemos una llamada para encender y otra para apagar.

## 6. Parte 1: alternar los dos LED

### Paso 1. Escribir el código ensamblador

Pega el siguiente código completo en **`programa.S`**:

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
    push {r4, lr}           @ Guardar registros; reservar 8 bytes.

ciclo:
    movs r0, #1            @ Patron 01: LED 1 encendido, LED 2 apagado.
    bl escribir_leds       @ Enviar el patron al hardware.

    ldr r0, =500           @ Preparar el argumento: 500 milisegundos.
    bl esperar_ms          @ Mantener este estado durante 500 ms.

    movs r0, #2            @ Patron 10: LED 1 apagado, LED 2 encendido.
    bl escribir_leds       @ Aplicar el nuevo estado.

    ldr r0, =500           @ Preparar otra espera de 500 ms.
    bl esperar_ms

    b ciclo                @ Regresar al principio y repetir.

.size programa_asm, .-programa_asm
.ltorg
```

### Paso 2. Compilar y cargar

1. Guarda ambos archivos y `CMakeLists.txt`.
2. Ejecuta **Build/Compile** desde la extensión Pico.
3. Si aparece un error, corrígelo antes de continuar.
4. Con la placa desconectada, mantén presionado **BOOT** y conecta el USB.
5. Suelta BOOT cuando aparezca la unidad **RPI-RP2**.
6. Copia a esa unidad el archivo **`leds_adc_asm.uf2`** generado, normalmente dentro de `build`.
7. La unidad desaparecerá y el programa comenzará a ejecutarse.

**Resultado esperado:** el LED 1 permanece encendido medio segundo y luego el LED 2 permanece encendido medio segundo. La secuencia se repite. Durante la inicialización ambos están apagados; durante la alternancia siempre hay uno encendido y el otro apagado.

### Paso 3. Entender las instrucciones

#### Registros: espacios de trabajo dentro del procesador

Un registro almacena temporalmente un dato. En este ejercicio usamos:

| Registro | Uso |
|---|---|
| `r0` | Primer argumento de una función y lugar donde recibimos un resultado entero. |
| `r1` | Un dato temporal; en la parte 2 almacenará el umbral. |
| `r4` | Conserva una lectura mientras llamamos a otras funciones. |
| `lr` | Dirección a la que debe regresar una llamada a función. |
| `sp` | Indica la posición actual de la pila. |

Los registros `r0` a `r3` pueden cambiar cuando llamamos a una función. No debemos suponer que conservan su valor después de `bl`.

#### `movs r0, #1`: colocar un número

Esta instrucción coloca el número **1** en `r0`. El signo `#` significa que se trata de un valor inmediato escrito en el código. No significa una dirección de memoria.

La `s` indica que también se actualizan ciertas banderas del procesador, como la de cero. Aquí todavía no las utilizamos.

#### `bl escribir_leds`: llamar a una función

`bl` significa **Branch with Link**: salta a una función y guarda en `lr` la dirección de regreso. La función recibe su primer argumento en `r0`.

Por eso estas instrucciones deben leerse juntas:

```asm
movs r0, #1
bl escribir_leds
```

Significan: «coloca 1 en el registro del primer argumento y llama a `escribir_leds`». Equivalen a pedir `escribir_leds(1)`.

Cuando la función termina, la ejecución continúa en la instrucción que sigue a `bl`.

#### `ldr r0, =500`: cargar una constante

Esta forma con `=` es una **pseudoinstrucción**. Le pedimos al ensamblador que coloque la constante 500 en `r0`; él genera la forma apropiada para hacerlo.

En Cortex-M0+, la forma inmediata de `movs` usada aquí admite números de 0 a 255. Por eso no escribimos `movs r0, #500`.

La forma `ldr r0, =500` carga el **valor 500**. No significa leer el contenido de la dirección de memoria 500. La directiva `.ltorg` permite colocar las constantes que el ensamblador necesite; está después de un salto incondicional, fuera del camino normal de ejecución.

#### `b ciclo`: repetir sin llamar a una función

`ciclo:` es una **etiqueta**: un nombre para una posición del programa. `b ciclo` salta a esa posición sin guardar una dirección de regreso.

- Usa `bl` para llamar a una función que regresará.
- Usa `b` para continuar la ejecución en otra parte del mismo flujo.

Como siempre regresamos a `ciclo`, tenemos un ciclo infinito.

#### `push {r4, lr}`: guardar en la pila

La pila es una zona de memoria usada para guardar datos temporalmente. `push` guarda los registros indicados y actualiza `sp`.

Aquí se guardan dos registros de 32 bits: **8 bytes**. Así mantenemos la alineación de la pila exigida para las llamadas a funciones C. En esta primera parte `r4` no tiene otro uso; en la segunda conservará la lectura.

El `push` está **antes** de `ciclo`: se ejecuta una sola vez y no llena la pila en cada vuelta. La rutina nunca termina, por eso no aparece un `pop`. Si diseñáramos una versión que regresara a C, tendría que restaurar lo guardado, por ejemplo con `pop {r4, pc}` al finalizar; no debe regresar sin recuperar la pila.

#### Las líneas que empiezan con punto

Son **directivas para el ensamblador**, no acciones que se repiten en el microcontrolador:

| Directiva | Función |
|---|---|
| `.syntax unified` | Selecciona la sintaxis de ensamblador. |
| `.cpu cortex-m0plus` | Indica para qué procesador escribimos. |
| `.thumb` | Selecciona el conjunto de instrucciones Thumb. |
| `.section .text` | Coloca las instrucciones en la sección de código. |
| `.balign 4` | Alinea el inicio a un múltiplo de cuatro bytes. |
| `.global programa_asm` | Permite que otro archivo encuentre esta función. |
| `.type ...` | Identifica el símbolo como función. |
| `.thumb_func` | Marca la siguiente función como función Thumb. |
| `.size ...` | Registra el tamaño del código de la función. |
| `.ltorg` | Emite las constantes pendientes cuando se necesitan. |

El símbolo `@` inicia un comentario: lo que está a su derecha no se ejecuta.

### Paso 4. Seguir una vuelta a mano

| Paso | Acción | LED 1 | LED 2 |
|---:|---|:---:|:---:|
| 1 | Cargar patrón 1 y llamar a `escribir_leds` | Encendido | Apagado |
| 2 | Esperar 500 ms | Encendido | Apagado |
| 3 | Cargar patrón 2 y llamar a `escribir_leds` | Apagado | Encendido |
| 4 | Esperar 500 ms | Apagado | Encendido |
| 5 | Saltar a `ciclo` | Vuelve al paso 1 | Vuelve al paso 1 |

**Prueba breve:** cambia las dos constantes 500 por 1000, vuelve a compilar y carga el UF2 nuevo. Cada estado debe durar aproximadamente un segundo. Después restaura 500.

## 7. Parte 2: leer una entrada analógica

### Paso 1. Comprender qué vamos a medir

Una salida digital como la del LED tiene dos estados. Una entrada analógica permite medir un voltaje que varía dentro de un intervalo.

El **ADC**, convertidor analógico a digital, transforma ese voltaje en un número. En el RP2040 produce códigos de **12 bits**, es decir, 4096 valores posibles: **0 a 4095**. Eso no implica que una medición real tenga 12 bits de precisión perfecta.

Con una referencia nominal de 3.3 V:

| Voltaje aproximado | Lectura aproximada |
|---:|---:|
| 0 V | 0 |
| 0.825 V | 1024 |
| 1.65 V | 2048 |
| 2.475 V | 3072 |
| Cerca de 3.3 V | Cerca de 4095 |

Para estimar el voltaje a partir del código: **V ≈ lectura × 3.3 / 4096**. La referencia real, el ruido y la tolerancia del circuito afectan la medición; no esperes números idénticos en todas las tarjetas.

### Paso 2. Conectar el potenciómetro

Desconecta el USB. Conserva los LED y agrega estas conexiones:

| Terminal del potenciómetro | Conectar a |
|---|---|
| Un extremo | 3V3 / 3.3 V de la RP2040-Zero |
| El otro extremo | GND |
| Terminal central, cursor | GPIO26 |

El cursor entrega una fracción del voltaje de 3.3 V. Al girar la perilla cambia ese voltaje. Si intercambias únicamente los dos extremos, se invierte el sentido del recorrido.

**Usa 3V3, no el pin 5V.** Todos los elementos deben compartir GND. No dejes la entrada desconectada: una entrada flotante puede mostrar valores erráticos.

### Paso 3. Distinguir pin y canal

En esta práctica:

- **GPIO26** es el pin donde conectamos el cable.
- **ADC0** es el canal que seleccionamos en el convertidor.

Por eso el archivo C utiliza `adc_gpio_init(26)` y `adc_select_input(0)`. El canal 0 del ADC **no es GPIO0**; GPIO0 sigue conectado al LED 1.

### Paso 4. Cambiar únicamente programa.S

Conserva el archivo C y CMake. **Reemplaza todo** el contenido de `programa.S` por esta segunda versión. No pegues ambas versiones juntas, porque repetirías los nombres de las funciones y etiquetas.

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
    push {r4, lr}           @ Guardar registros una sola vez.
    bl preparar_adc        @ Configurar ADC0 / GPIO26 una sola vez.

ciclo_adc:
    bl leer_adc            @ La lectura regresa en r0.
    movs r4, r0            @ Conservarla antes de llamar otra funcion.

    movs r0, r4            @ Primer argumento: lectura que queremos ver.
    bl mostrar_adc         @ Mostrar la lectura por USB.

    ldr r1, =2048          @ Umbral: aproximadamente la mitad de escala.
    cmp r4, r1             @ Comparar lectura con umbral.
    blo valor_bajo         @ Saltar si lectura < 2048 (sin signo).

valor_alto:
    movs r0, #2            @ Lectura >= 2048: LED 2 encendido.
    bl escribir_leds
    b pausa                @ Evitar ejecutar tambien el caso bajo.

valor_bajo:
    movs r0, #1            @ Lectura < 2048: LED 1 encendido.
    bl escribir_leds

pausa:
    ldr r0, =200           @ Espera de 200 ms entre iteraciones.
    bl esperar_ms
    b ciclo_adc            @ Tomar una nueva lectura.

.size programa_asm, .-programa_asm
.ltorg
```

### Paso 5. Entender el recorrido de la lectura

#### Recibir un resultado: `bl leer_adc`

La función C realiza la conversión. Cuando regresa, el número está en `r0`. Si el potenciómetro produce una lectura de 1300, entonces `r0` contiene 1300 después de esta llamada.

#### Conservarlo: `movs r4, r0`

Esta instrucción copia el contenido de `r0` en `r4`. No borra `r0` y no lee una dirección de memoria.

Necesitamos la copia porque enseguida llamamos a `mostrar_adc`. Una llamada puede modificar `r0` a `r3`. En cambio, las funciones C respetan el valor de `r4` según la convención de llamadas: si lo usan internamente, deben restaurarlo antes de regresar.

**Idea clave:** usamos `r0` para intercambiar datos con las funciones y `r4` para conservar nuestra lectura entre llamadas.

#### Mostrarlo: `movs r0, r4` y `bl mostrar_adc`

Colocamos la lectura en el registro del primer argumento y llamamos a la función que imprime. Aunque la copia parezca repetida inmediatamente después de guardar la lectura, deja explícito qué dato recibe la función.

#### Comparar: `cmp r4, r1`

Antes cargamos **2048** en `r1`. `cmp` compara los registros como si calculara `r4 - r1`, pero **no guarda la resta en ninguno de ellos**. Actualiza las banderas del procesador para permitir una decisión.

#### Decidir: `blo valor_bajo`

`blo` significa **Branch if Lower**, salto si es menor en una comparación **sin signo**. Es apropiado porque la lectura ADC va de 0 a 4095 y no es negativa. Después de esta comparación, `blo` salta cuando la bandera de acarreo C está en cero, indicando préstamo en la resta sin signo.

- Si la lectura es menor que 2048, salta a `valor_bajo`.
- Si es igual o mayor que 2048, no salta: continúa en `valor_alto`.

`cmp` y `blo` deben permanecer juntos aquí: no insertes entre ellos una llamada u otra instrucción que cambie las banderas.

#### Evitar ejecutar los dos casos: `b pausa`

Después del caso alto debemos saltar a `pausa`. Si eliminamos ese salto, el procesador continuará por las instrucciones del caso bajo y volverá a cambiar los LED.

Las etiquetas **no detienen** al procesador ni delimitan automáticamente un bloque como las llaves de C. La ejecución continúa en orden salvo que una instrucción de salto cambie el recorrido.

#### Repetir la medición

`b ciclo_adc` vuelve a leer el potenciómetro. La preparación del ADC y el `push` quedan fuera del ciclo: no se repiten.

La pausa de 200 ms permite ver los cambios con calma. Cada iteración tarda algo más de 200 ms porque también incluye la conversión, la impresión y las demás instrucciones; no es un muestreo de tiempo exacto.

### Paso 6. Compilar, cargar y observar

1. Compila la segunda versión.
2. Carga el nuevo UF2 mediante BOOT, como en la primera parte.
3. Abre un monitor serial y selecciona el puerto USB de la RP2040-Zero. En Windows suele aparecer como un puerto COM.
4. Si el monitor pide velocidad, selecciona **115200**. Aquí usamos USB CDC: ese ajuste no determina una velocidad física UART.
5. Observa las líneas `ADC = ...`. Se imprimen continuamente, así que puedes abrir el monitor después de iniciar el programa.
6. Gira lentamente el potenciómetro y verifica la tabla siguiente.

| Lectura | LED 1 | LED 2 |
|---:|:---:|:---:|
| 0 a 2047 | Encendido | Apagado |
| 2048 a 4095 | Apagado | Encendido |

**En esta segunda parte ya no alternan por tiempo:** el potenciómetro decide cuál permanece encendido. Cerca del punto medio pueden cambiar rápidamente debido al ruido. En este ejercicio usamos un solo umbral; todavía no aplicamos filtrado ni histéresis.

### Paso 7. Seguir dos ejemplos

**Lectura de 1300:** `leer_adc` devuelve 1300 en `r0`; lo copiamos a `r4`; lo mostramos; comparamos con 2048; `blo` sí salta; enviamos patrón 1 y enciende el LED 1.

**Lectura de 3000:** guardamos y mostramos 3000; la comparación indica que no es menor que 2048; `blo` no salta; enviamos patrón 2 y enciende el LED 2; `b pausa` evita ejecutar el caso bajo.

**Lectura exactamente igual a 2048:** no es menor; se ejecuta el caso alto y enciende el LED 2.

## 8. Actividades para demostrar que entendiste

1. En la parte 1, configura **200 ms** para el primer estado y **800 ms** para el segundo. Explica qué constantes cambiaste.
2. En la parte 2, cambia el umbral de **2048 a 1024**. Predice en qué zona del giro cambiarán los LED y luego compruébalo.
3. Regresa el umbral a 2048 e invierte los LED de los casos bajo y alto **modificando solamente el ensamblador**.
4. Registra tres mediciones reales: potenciómetro cerca del mínimo, cerca de la mitad y cerca del máximo.

| Posición | Lectura ADC observada | Voltaje estimado: lectura × 3.3 / 4096 | LED encendido |
|---|---:|---:|---|
| Mínimo | | | |
| Mitad | | | |
| Máximo | | | |

## 9. Preguntas de comprensión

1. ¿Por qué el patrón 1 enciende el LED conectado a GPIO0?
2. ¿Qué diferencia hay entre `b` y `bl`?
3. ¿Qué representa `#` en `movs r0, #2`?
4. ¿Por qué usamos `ldr r0, =500` en lugar de `movs r0, #500`?
5. ¿En qué registro recibimos el resultado de `leer_adc`?
6. ¿Por qué copiamos la lectura a `r4` antes de mostrarla?
7. ¿`cmp` cambia la lectura almacenada en `r4`?
8. ¿Qué sucede si quitamos `b pausa` del caso alto?
9. ¿Cuál es la diferencia entre GPIO26 y ADC0?
10. ¿Por qué sería incorrecto poner `push {r4, lr}` dentro del ciclo?

## 10. Entrega

- Archivo `leds_adc_asm.c`.
- Las dos versiones de ensamblador guardadas como evidencia: `parte1.S` y `parte2.S`. Para compilar, copia **solo una** a `programa.S`; no agregues simultáneamente ambas al ejecutable.
- Fotografía del circuito donde se distingan las resistencias y las conexiones.
- Video corto de cada parte funcionando.
- Tabla de mediciones y respuestas a las preguntas.

**Criterio principal:** demostrar que el circuito funciona y explicar el recorrido de las instrucciones, no únicamente copiar el código.

## 11. Si algo no funciona

| Problema | Revisión |
|---|---|
| Un LED nunca enciende | Polaridad, resistencia, continuidad y conexión al GPIO correcto. |
| Los dos LED tienen el mismo comportamiento | Comprueba que no estén conectados al mismo nodo de la protoboard. |
| No aparece RPI-RP2 | Usa un cable de datos y mantén BOOT presionado antes de conectar. |
| `undefined reference to programa_asm` | Incluye `programa.S` en `add_executable`; revisa el nombre y `.global`. |
| Error con instrucciones o procesador | Verifica RP2040, Cortex-M0+, ASM habilitado y que no seleccionaste Pico 2. |
| Error al enlazar ADC | Agrega `hardware_adc` a `target_link_libraries`. |
| Símbolos duplicados | Compila una sola versión de `programa_asm` y conserva un solo `main`. |
| No aparece texto | Habilita stdio USB, elige el COM correcto y cierra otros programas que ocupen el puerto. |
| Lectura siempre cercana a cero o al máximo | Revisa el cursor central, GPIO26, 3V3 y GND. |
| La lectura cambia sin tocar nada | Verifica conexiones; pequeñas variaciones son normales. |
| Cambios de código sin efecto | Compila de nuevo y carga el UF2 recién generado. |

## Referencias técnicas

- [RP2040-Zero: documentación y pinout de Waveshare](https://www.waveshare.com/wiki/RP2040-Zero).
- [Pico SDK: ADC y GPIO](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html).
- [Código oficial de hardware_adc](https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/hardware_adc/include/hardware/adc.h).
- [Hoja de datos oficial del RP2040](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf).

**Nota de validación:** el material incluye una revisión estática del flujo y de la interfaz entre C y ensamblador. No se ha probado físicamente en una RP2040-Zero ni compilado con el Pico SDK en este entorno.

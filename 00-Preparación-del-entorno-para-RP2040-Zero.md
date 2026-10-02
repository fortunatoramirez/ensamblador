# Práctica 0. Preparación del entorno para RP2040-Zero

## Objetivo

Preparar la computadora y comprobar que la tarjeta **Waveshare RP2040-Zero** puede ser programada correctamente desde Visual Studio Code.

Al finalizar, el alumno deberá poder:

- Crear un proyecto para RP2040.
- Compilarlo.
- Generar un archivo `.uf2`.
- Cargarlo a la tarjeta.
- Ver un mensaje enviado por USB.

---

## Material

- Waveshare RP2040-Zero.
- Cable USB-C de datos.
- Computadora con Windows.
- Visual Studio Code.

---

## 1. Instalar Visual Studio Code

Descargar e instalar **Visual Studio Code**.

Abrirlo y entrar a:

```text
Extensions
```

Buscar:

```text
Raspberry Pi Pico
```

Instalar la extensión oficial publicada por **Raspberry Pi**.

---

## 2. Crear el proyecto

En la barra lateral de Raspberry Pi Pico seleccionar:

```text
New C/C++ Project
```

Configurar:

```text
Name:
P00_Entorno
```

En:

```text
Board type
```

seleccionar:

```text
Other
```

Dejar:

```text
Pico SDK:
v2.3.1
```

Activar:

```text
Console over USB
```

No activar por ahora:

```text
SPI
I2C
UART
PIO
DMA
etc.
```

Crear el proyecto.

---

## 3. Corregir el modelo de tarjeta

Abrir el archivo:

```text
CMakeLists.txt
```

Buscar esta línea:

```cmake
set(PICO_BOARD pico2 CACHE STRING "Board type")
```

y cambiarla por:

```cmake
set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Board type")
```

Esto es importante porque nuestra tarjeta utiliza un **RP2040**, no un RP2350.

Guardar el archivo.

---

## 4. Limpiar y configurar CMake

En la barra lateral de Raspberry Pi Pico ejecutar:

```text
Clean CMake
```

Después:

```text
Configure CMake
```

Esto obliga al proyecto a reconstruirse usando la tarjeta correcta.

---

## 5. Primer programa

Abrir el archivo `.c` principal del proyecto.

Por ejemplo:

```text
P00_Entorno.c
```

Escribir:

```c
#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();

    sleep_ms(2000);

    while (true)
    {
        printf("Hola desde la RP2040-Zero\n");

        sleep_ms(1000);
    }
}
```

---

## 6. ¿Qué hace el programa?

La instrucción:

```c
stdio_init_all();
```

inicializa la comunicación.

La instrucción:

```c
printf("Hola desde la RP2040-Zero\n");
```

envía un mensaje a la computadora.

La instrucción:

```c
sleep_ms(1000);
```

espera:

```text
1000 ms = 1 segundo
```

Y:

```c
while (true)
```

hace que el programa se repita indefinidamente.

---

## 7. Compilar

Guardar el programa.

Seleccionar:

```text
Compile Project
```

Si todo está correcto, el proyecto deberá compilar sin errores.

Se generará un archivo:

```text
.uf2
```

Ese archivo contiene el programa que cargaremos al microcontrolador.

---

## 8. Entrar en modo BOOTSEL

La RP2040-Zero tiene botones:

```text
BOOT
RESET
```

Para entrar al modo de programación:

1. Mantener presionado **BOOT**.
2. Presionar y soltar **RESET**.
3. Soltar **BOOT**.

También puede realizarse conectando la tarjeta mientras se mantiene presionado BOOT.

Windows deberá detectar una unidad USB llamada aproximadamente:

```text
RPI-RP2
```

---

## 9. Cargar el programa

Localizar el archivo:

```text
P00_Entorno.uf2
```

Copiarlo a:

```text
RPI-RP2
```

La unidad desaparecerá automáticamente.

Esto es normal.

La tarjeta se reiniciará y comenzará a ejecutar el programa.

---

## 10. Ver el resultado

Abrir el monitor serial desde Visual Studio Code.

Seleccionar el puerto correspondiente a la RP2040-Zero.

Debe aparecer:

```text
Hola desde la RP2040-Zero
Hola desde la RP2040-Zero
Hola desde la RP2040-Zero
...
```

aproximadamente una vez por segundo.

---

## 11. Prueba

Modificar:

```c
sleep_ms(1000);
```

por:

```c
sleep_ms(500);
```

Volver a:

```text
Guardar
↓
Compile Project
↓
Cargar .uf2
```

Observar qué sucede.

Después probar:

```c
sleep_ms(2000);
```

---

## 12. Evidencias

Entregar:

1. Captura de la extensión **Raspberry Pi Pico** instalada.
2. Captura del proyecto creado.
3. Captura de `CMakeLists.txt` mostrando:

```cmake
set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Board type")
```

4. Captura de compilación correcta.
5. Captura del monitor mostrando:

```text
Hola desde la RP2040-Zero
```

6. Explicar brevemente qué sucede al cambiar:

```c
sleep_ms(1000);
```

por:

```c
sleep_ms(500);
```

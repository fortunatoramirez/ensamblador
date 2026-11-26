# Agregando sonido al Sistema Retro

Continuación de [Aventura retro con IA](../aventura_retro_ia). Trabaja sobre **tu juego personalizado de la práctica anterior**: conservaremos personajes, animación y reglas, y agregaremos sonido a sus eventos.

## Objetivo

Incorporar un buzzer al videojuego y utilizar IA para diseñar efectos que correspondan a su temática. El módulo de audio ya está preparado; tu trabajo será integrarlo, comprender la selección en ensamblador y personalizar las notas.

## 1. Material y conexión

Además del sistema que ya funciona, necesitas:

- Un **transductor piezoeléctrico pasivo de dos terminales**, apto para señal de 3.3 V.
- Resistencia de **1 kΩ**, cables y protoboard.

La conexión siguiente es para un piezo pasivo de baja carga. Un buzzer magnético, una bocina o un módulo de tres terminales requiere revisar su circuito; no lo conectes como si fuera este piezo. Un buzzer activo no permitirá obtener los efectos de distintas notas previstos aquí.

Con USB desconectado, conecta:

| Desde | Hacia |
|---|---|
| GPIO14 | Resistencia de 1 kΩ. |
| Otro extremo de la resistencia | Terminal + del piezo. |
| Terminal − del piezo | GND de la tarjeta. |

Conserva el cableado de pantalla y joystick. **GPIO14 debe estar libre** en tu proyecto. El módulo reserva su generador PWM; no configures otro PWM en GPIO12–15 durante esta práctica.

## 2. Agregar los archivos preparados

Haz una copia de tu proyecto funcional. Conserva su nombre en CMake y elimina de la copia la carpeta `build` si contiene rutas anteriores.

Descarga estos tres archivos y colócalos junto a tu archivo C principal:

- [audio.h](audio.h): nombres de eventos y declaraciones.
- [audio.c](audio.c): generación y reproducción de sonidos.
- [eventos_audio.S](eventos_audio.S): selección del efecto en ensamblador.

No sustituyas tu `programa.S` ni tu juego por estos archivos.

En el `add_executable(...)` existente, agrega:

```cmake
    audio.c
    eventos_audio.S
```

Conserva el archivo C principal y `programa.S` que ya aparecen en ese bloque.

En `target_link_libraries(...)`, agrega:

```cmake
    hardware_pwm
    hardware_clocks
```

Conserva `pico_stdlib`, `hardware_adc` y `hardware_i2c`. La declaración `project(... C CXX ASM)` debe seguir incluyendo ASM.

## 3. Hacer sonar el primer efecto

En el archivo C de tu juego agrega:

```c
#include "audio.h"
```

Al final de `preparar_hardware()`, agrega:

```c
audio_iniciar();
audio_evento(AUDIO_CAPTURA); // Prueba al encender; quitar despues.
```

En el ciclo de `programa.S`, agrega **una sola llamada** entre actualizar el juego y dibujar:

```asm
ciclo:
    bl actualizar_joystick
    bl actualizar_juego
    bl audio_actualizar
    bl dibujar_pantalla
    movs r0, #20
    bl esperar_ms
    b ciclo
```

Si tu ciclo tiene otras llamadas de animación, consérvalas. No dupliques la etiqueta `ciclo`: modifica el bloque existente.

Guarda, ejecuta Clean CMake y Configure CMake, compila y carga el UF2 como antes. Debe escucharse un sonido corto al arrancar.

Cuando funcione, elimina únicamente la llamada de prueba `audio_evento(AUDIO_CAPTURA)`; conserva `audio_iniciar()`.

### ¿Cómo funciona?

PWM genera una señal que alterna entre nivel alto y bajo. Su frecuencia determina la altura del tono: más frecuencia se escucha más aguda.

El módulo reproduce una nota, regresa al juego y revisa en cada vuelta si ya corresponde pasar a la siguiente. No incorpora esperas para la duración de las notas.

La actualización ocurre una vez por vuelta: las duraciones se aproximan al ritmo del ciclo y pueden alargarse por el dibujo o la comunicación. El juego conserva su espera de 20 ms anterior; **no agregues más esperas para los sonidos**.

## 4. Conectar el sonido con los eventos

Usa estas llamadas **donde ocurre el evento**, dentro de `actualizar_juego()` o de la función que gestione tus reglas:

| Evento | Llamada |
|---|---|
| Captura normal sin terminar partida | `audio_evento(AUDIO_CAPTURA);` |
| Captura especial sin terminar partida | `audio_evento(AUDIO_ESPECIAL);` |
| Mina que quita una vida y permite continuar | `audio_evento(AUDIO_GOLPE);` |
| Cambio a GANASTE | `audio_evento(AUDIO_VICTORIA);` |
| Cambio a PERDISTE por vidas o tiempo | `audio_evento(AUDIO_DERROTA);` |

**La llamada debe ejecutarse una vez al ocurrir el evento**, no repetirse mientras se dibuja una pantalla. De lo contrario, la secuencia vuelve a empezar continuamente.

### A. Capturar objetos

Después de actualizar los puntos, revisa primero si ya ganaste:

```c
// Aqui conservas tu actualizacion de puntos de la practica anterior.
if (puntos >= META) {
    estado = GANASTE;
    audio_evento(AUDIO_VICTORIA);
    return;
}
// Aqui reproduce captura normal o especial, segun el objeto capturado.
// Despues conserva la dificultad y generacion del siguiente objeto.
```

Usa el tipo del objeto **que acabas de capturar**, antes de generar el siguiente. Los nombres de tu variable de objeto especial pueden ser distintos entre equipos.

### B. Recibir daño

Inmediatamente después de descontar una vida, la estructura será:

```c
if (!vidas) {
    estado = PERDISTE;
    audio_evento(AUDIO_DERROTA);
    return;
}
audio_evento(AUDIO_GOLPE);
// Conserva el regreso al centro y la recolocacion segura del tablero.
```

Agrega también el efecto de derrota al bloque que detecta que se agotó el tiempo.

### C. Pausar y reiniciar

- Al cambiar a PAUSA, llama a `audio_detener();`.
- Al comenzar o reiniciar partida, llama a `audio_detener();`.
- Al continuar desde pausa no reinicies el efecto anterior.
- Después de ganar o perder, sigue llamando `audio_actualizar()` en el ciclo: así termina la melodía.

Un sonido nuevo reemplaza al anterior. No hay cola ni sistema adicional de prioridades; si el evento termina la partida, reproduce únicamente el efecto final.

### Usa IA para adaptar las llamadas a TU versión

Comparte tus archivos C, `programa.S` y `audio.h` con el asistente que ya utilizaste. Puedes usar ChatGPT o Copilot. Si no puede abrir enlaces, adjunta los archivos o pega el código.

> Continúo mi aventura retro en RP2040-Zero, Cortex-M0+, GNU Thumb y Pico SDK. Integra el módulo audio.h/audio.c y eventos_audio.S que adjunto. Agrega las llamadas de captura normal, especial, daño, victoria y derrota solamente donde ocurre cada evento. Detén el audio al pausar y al iniciar una partida. Usa el tipo del objeto capturado antes de regenerarlo. Conserva mis sprites, reglas, funciones ASM y CMake. Muéstrame los bloques exactos que debo modificar, sin reescribir todo el juego, y explica por qué no se repiten los efectos cada cuadro.

Compara los cambios propuestos con las indicaciones anteriores, compila y juega.

## 5. Personalizar los efectos con IA

Abre `audio.c`. Al principio están las tablas que puedes cambiar:

```c
static const Nota captura[] = {{1047,80}};
```

Cada par significa **{frecuencia en Hz, duración en ms}**. Por ejemplo:

| Par | Qué representa |
|---|---|
| `{1047,80}` | Tono de 1047 Hz durante unos 80 ms. |
| `{0,40}` | Silencio de unos 40 ms. |

La frecuencia cambia la altura del tono, no es un control de volumen. Para esta práctica conserva frecuencias de **150 a 4000 Hz**, silencios de 0 Hz y duraciones de **40 a 300 ms** por nota. El módulo limita automáticamente frecuencias fuera del rango.

Usa este prompt y sustituye el tema:

> Mi videojuego trata de [tema y personaje]. Uso un piezo pasivo con PWM y una sola nota a la vez. Diseña tres familias de efectos: recolección (normal y especial), daño y fin de partida (victoria y derrota). Devuelve SOLO las cinco tablas C de tipo Nota con los nombres captura, especial, golpe, victoria y derrota. Cada elemento es {Hz,ms}; 0 Hz es silencio. Usa 150–4000 Hz, 40–300 ms por nota, máximo seis elementos por tabla. Captura normal debe durar menos de 200 ms, especial menos de 400 ms y cada final menos de un segundo. Explica despues brevemente cómo se relacionan con mi temática. No cambies el reproductor ni agregues delays.

Reemplaza **solo las cinco tablas**, escucha y solicita una corrección concreta, por ejemplo:

> El daño se parece demasiado a la recolección. Conserva el resto y propón un daño más grave y descendente.

Comprueba la suma de las duraciones: la IA puede equivocarse. No necesitas generar un MP3; aquí componemos secuencias de notas que puede ejecutar el buzzer.

## 6. Leer la parte nueva de ensamblador

Abre `eventos_audio.S`. La función `seleccionar_efecto_asm` recibe el evento en `r0` y devuelve en ese mismo registro el identificador del efecto.

En esta versión evento y efecto usan los mismos números: 1..5. Un valor fuera del rango devuelve 0 y no interrumpe el sonido actual.

| Instrucción | Función |
|---|---|
| `cmp r0, #1` | Compara el evento con el mínimo válido. |
| `blo` | Salta si es menor, interpretando el dato sin signo. |
| `cmp r0, #5` y `bhi` | Rechazan eventos mayores que cinco. |
| `movs r0, #0` | Selecciona ningún efecto ante un dato inválido. |
| `bx lr` | Regresa al módulo C. |

Pide:

> Explica seleccionar_efecto_asm siguiendo r0 con entradas 0, 2 y 6. Indica qué salto se toma y qué valor regresa. ¿Por qué aquí usamos comparaciones sin signo? No modifiques el código.

Sigue las instrucciones tú mismo y contrasta la explicación. La rutina solo usa `r0` y no llama otras funciones; no necesita guardar registros en la pila.

## 7. Jugar y preparar la base del proyecto final

Comprueba una partida completa:

- Se distinguen captura normal, especial y daño.
- La última captura reproduce victoria; el último golpe o el tiempo agotado reproduce derrota.
- Puedes mover al personaje mientras suena un efecto.
- La pausa silencia el buzzer; al continuar no se reinicia la secuencia.
- Reiniciar detiene la melodía anterior.
- Tu personalización gráfica y las reglas anteriores siguen funcionando.

Si no suena, revisa GND, GPIO14, el tipo de piezo y la llamada a `audio_iniciar()`. Si suena continuamente, revisa `audio_actualizar()` y que no estés disparando el evento en cada cuadro. Si aparece un error de enlace, comprueba los archivos del ejecutable y las bibliotecas PWM/clocks.

**Tu mismo juego queda ahora con entrada, pantalla y sonido**, una base reutilizable para desarrollar la consola del proyecto final.

### Referencia y alcance de la comprobación

El reproductor utiliza las [API de hardware del Pico SDK](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html). Se comprobó la lógica temporal en computadora con funciones de hardware simuladas. Esa prueba no sustituye compilar con Pico SDK ni escuchar el piezo en la tarjeta; ambas verificaciones se realizan al integrar la práctica.

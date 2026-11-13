# Cazador de objetos — RP2040-Zero, OLED y ensamblador

Continuación de `../oled_joystick_asm`, en un proyecto independiente. Usa el mismo cableado y la misma pantalla **SSD1306 I2C de 128×64**. No requiere bocina ni componentes adicionales.

## El reto

Controla el cuadro hueco con el joystick y **come 10 rombos en 60 segundos**. Al tocar un rombo, desaparece y aparece otro en una posición diferente. Tienes tres vidas.

- Al llegar a 3 objetos aparece una mina; con 6 hay dos y con 9 hay tres.
- Las minas se dibujan como una **X**. Tocarlas cuesta una vida y devuelve el jugador al centro.
- Cada recolección o golpe reorganiza comida y minas, dejando espacio alrededor del jugador y entre objetos.
- El marcador muestra `P` (objetos recogidos/meta), `V` (vidas) y `T` (segundos restantes).
- Ganas al recoger el décimo objeto antes de terminar el tiempo. Pierdes al quedarte sin vidas o sin tiempo.
- El botón del joystick inicia, pausa, continúa o reinicia, según la pantalla. **Suéltalo entre pulsaciones**; mantenerlo presionado no repite la acción.
- El tiempo se detiene en pausa. La posición se conserva al soltar la palanca y se permiten diagonales.

## 1. Conexiones

Desconecta el USB antes de cablear. Usa los nombres GPIO impresos; no cuentes posiciones del conector.

| Módulo | Terminal | RP2040-Zero |
|---|---|---|
| OLED | VCC | 3V3 |
| OLED | GND | GND |
| OLED | SDA | GPIO0 |
| OLED | SCL | GPIO1 |
| Joystick | VCC o +5V | **3V3**, aunque el módulo diga +5V |
| Joystick | GND | GND |
| Joystick | VRx | GPIO26 / ADC0 |
| Joystick | VRy | GPIO27 / ADC1 |
| Joystick | SW | GPIO15 |

Este ejemplo supone un módulo joystick pasivo convencional que funciona a 3.3 V y un módulo OLED compatible con alimentación de 3.3 V. Los GPIO y el ADC no deben recibir 5 V. Ambos módulos comparten GND. Para SW se activa el pull-up interno; al pulsar, debe conectar la señal a GND.

Los módulos OLED suelen incluir resistencias pull-up I2C. Si el tuyo no las incluye, agrega una resistencia de 4.7 kΩ de SDA a 3V3 y otra de SCL a 3V3. Mantén los cables cortos.

## 2. Crear el proyecto en VS Code

1. En Raspberry Pi Pico, selecciona New C/C++ Project.
2. Nombre: `oled_cazador_asm`.
3. Board: `waveshare_rp2040_zero`; si no aparece, usa `pico` para este ejemplo. **No Pico 2**.
4. Lenguaje C y salida estándar por USB.
5. Crea el proyecto.
6. Reemplaza el contenido de `oled_cazador_asm.c` con el archivo incluido.
7. Copia `programa.S` a la misma carpeta. La S es mayúscula.
8. Ajusta CMake como se indica abajo.

## 3. Ajustar CMakeLists.txt

**Conserva el archivo generado por la extensión**, incluyendo las rutas, configuración de herramientas, `include(...)` del SDK y `pico_sdk_init()`. El archivo `CMake_fragmento.txt` es una referencia, no un CMake completo.

En la declaración existente del proyecto incluye ASM:

```cmake
project(oled_cazador_asm C CXX ASM)
```

Reemplaza el bloque `add_executable` existente:

```cmake
add_executable(oled_cazador_asm
    oled_cazador_asm.c
    programa.S
)
```

En el bloque existente de bibliotecas incluye las tres:

```cmake
target_link_libraries(oled_cazador_asm
    pico_stdlib
    hardware_adc
    hardware_i2c
)
```

Comprueba estas opciones, sin duplicarlas:

```cmake
pico_enable_stdio_usb(oled_cazador_asm 1)
pico_enable_stdio_uart(oled_cazador_asm 0)
pico_add_extra_outputs(oled_cazador_asm)
```

## 4. Compilar, cargar y jugar

1. Compila con Compile/Build de la extensión Raspberry Pi Pico.
2. Desconecta la tarjeta. Mantén BOOT presionado mientras conectas USB; suelta cuando aparezca RPI-RP2.
3. Copia `build/oled_cazador_asm.uf2` a RPI-RP2.
4. Deja el joystick suelto los primeros dos segundos para calibrar el centro.
5. En la portada, pulsa y suelta el joystick. Busca el rombo relleno y mueve el cuadro hasta tocarlo.
6. Comprueba que aumente P y que el rombo cambie de posición. Desde el tercer objeto, evita las X.
7. Pulsa para pausar; pulsa de nuevo para continuar. Al terminar, pulsa para una partida nueva.

Esta carpeta contiene fuentes, no un UF2 precompilado. En la extensión usa el CMake generado y el fragmento incluido. También se proporciona un `CMakeLists.txt` completo para una instalación manual del SDK:

```bash
cmake -S . -B build -DPICO_SDK_PATH=/ruta/pico-sdk -DPICO_BOARD=waveshare_rp2040_zero
cmake --build build
```

Requiere Pico SDK instalado con sus submódulos y toolchain ARM. No uses `pico2`: este proyecto es para RP2040/Cortex-M0+.

## 5. Entender el programa paso a paso

### Paso 1: observar el ciclo principal

En `programa.S`, `programa_asm` prepara el hardware una sola vez. Después repite:

1. `actualizar_joystick`: lee los ejes y obtiene un evento del botón con antirrebote.
2. `actualizar_juego`: decide pausa, colisiones, puntos, vidas y fin de partida.
3. `dibujar_pantalla`: construye y envía la imagen.
4. `esperar_ms`: pausa 20 ms antes de repetir. La vuelta también incluye el tiempo del ADC e I2C; no son 50 cuadros exactos por segundo.

### Paso 2: mover el jugador con ensamblador

`mover_eje_asm` es la misma rutina de la práctica anterior: recibe posición en r0, diferencia ADC-centro en r1, mínimo en r2 y máximo en r3. La zona muerta es ±350. Devuelve la nueva posición en r0. Se llama una vez por eje, solo durante la partida.

El centro del cuadro se limita a X=3..124 y Y=19..60. Como mide 5×5, sus bordes quedan dentro del área de juego, sin tocar el marcador.

### Paso 3: detectar que el cuadro comió un objeto

La nueva función `colision_asm` recibe cuatro argumentos:

| Registro | Contenido |
|---|---|
| r0 | X del jugador; al regresar, 1 si hubo contacto o 0 si no |
| r1 | Y del jugador |
| r2 | X del objeto |
| r3 | Y del objeto |

Cada figura cabe en una caja de 5×5, con dos píxeles a cada lado del centro. Hay contacto entre las cajas cuando **ambas** distancias entre centros son menores o iguales a cuatro:

```text
abs(x - objeto_x) <= 4  y  abs(y - objeto_y) <= 4
```

`subs` calcula la diferencia; `bpl` evita negarla si ya es positiva; `rsbs` obtiene el valor absoluto de una diferencia negativa; `cmp` y `bgt` descartan objetos alejados. `bx lr` devuelve el resultado a C. Esta función usa solamente r0–r3 y no llama otras funciones, por eso no necesita guardar LR en la pila.

Se comparan cajas, no píxeles de la figura: una esquina vacía del rombo o de la X también puede contar como contacto. Es una colisión sencilla y deliberada para aprender.

### Paso 4: cambiar el estado del juego

El `enum Estado` distingue PORTADA, JUGANDO, PAUSA, GANASTE y PERDISTE. Solo JUGANDO mueve el cuadro y revisa colisiones. El botón cambia de estado con un evento por pulsación, después de 30 ms de estabilidad.

El tiempo se calcula con el reloj del microcontrolador, no contando vueltas. Al continuar, se desplaza el inicio por la duración de la pausa. Las restas se hacen sin signo para tolerar el desbordamiento del contador de milisegundos.

### Paso 5: generar objetos y aumentar dificultad

`aleatorio` usa xorshift32, sembrado con el instante de inicio y las lecturas del joystick. Es pseudoaleatorio: no garantiza que nunca se repita una posición. `colocar` evita objetos cercanos al jugador, a la comida y a las minas ya colocadas, con intentos limitados y un recorrido de respaldo.

`puntos/3` determina cuántas minas aparecen, con máximo tres. En cada captura se reorganiza todo el tablero. Después de un golpe se recoloca al jugador y se genera un tablero seguro para evitar perder varias vidas por el mismo contacto.

**La práctica combina C y ensamblador**: ASM controla el ciclo, movimiento y colisión; C gestiona SDK, dibujo, estados, objetos, tiempo, puntos y vidas. No es un juego escrito completamente en ensamblador.

## 6. Ajustes y ejercicios

- En C, cambia `INVERTIR_X` o `INVERTIR_Y` a 1 si un eje está invertido.
- Cambia `META` para modificar el número de objetos, o `DURACION_MS` para el tiempo total.
- En ASM, modifica 350 para experimentar con la zona muerta.
- Reto: mover el incremento de puntos a una nueva función ASM y explicar qué registro recibe y devuelve el valor.
- Reto: agregar una comida especial que valga dos puntos; distinguirla visualmente y limitar la puntuación a la meta.

## 7. Comprobación en la tarjeta

| Prueba | Resultado esperado |
|---|---|
| Encender sin mover joystick | Portada estable |
| Pulsar y mantener | Inicia una sola vez |
| Soltar palanca | Cuadro quieto |
| Tocar rombo | Sube P y cambia el tablero |
| Recoger 3, 6 y 9 | Hay 1, 2 y 3 minas |
| Tocar mina | Baja V; cuadro vuelve al centro |
| Pausar varios segundos | El tiempo restante se conserva al continuar |
| Recoger 10 antes del límite | GANASTE |
| Agotar tiempo o vidas | FIN DEL JUEGO |
| Pulsar al finalizar | P=0, V=3, T=60 y sin minas |

Si no aparece imagen, verifica SSD1306 I2C 128×64, alimentación, SDA y SCL. Se prueban 0x3C y 0x3D. El monitor USB imprime ADC y OLED=OK/ERROR; un ACK no identifica el controlador. Ante un fallo I2C, corrige conexiones y reinicia. Para errores de enlace, comprueba ASM en `project()` y `programa.S` en `add_executable`.

## Validación realizada

Se verificó el código C con GCC y cabeceras simuladas del SDK, y se probaron en el equipo de desarrollo las reglas de puntuación, dificultad, vidas, pausa, tiempo, reinicio y separación de objetos. Estas pruebas sustituyen las funciones ASM por equivalentes C y **no validan el binario ARM ni el hardware**. Quedan pendientes la compilación completa con Pico SDK y la prueba física de pantalla, joystick y rutinas ASM.

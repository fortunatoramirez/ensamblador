# OLED y joystick completo — RP2040-Zero

Ejemplo para continuar la práctica de ADC en ensamblador. Requiere una pantalla **SSD1306, I2C, 128×64, de cuatro conexiones** y joystick analógico de cinco conexiones (dos potenciómetros y botón). No es un controlador para SH1106, pantallas SPI ni LCD 16×2.

## Qué hace

- Enciende la OLED y muestra las lecturas X/Y de 0 a 4095.
- Mueve un cuadro a izquierda, derecha, arriba, abajo y en diagonal.
- Deja el cuadro quieto cuando el joystick está al centro; no lo regresa automáticamente.
- Al presionar el joystick, centra el cuadro y lo dibuja relleno mientras el botón permanece presionado. B:1 indica pulsado; B:0, suelto.
- Imprime lecturas y estado de la OLED por USB cada 250 ms aproximadamente.

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
2. Nombre: `oled_joystick_asm`.
3. Board: `waveshare_rp2040_zero`; si no aparece, usa `pico` para este ejemplo. **No Pico 2**.
4. Lenguaje C y salida estándar por USB.
5. Crea el proyecto.
6. Reemplaza el contenido de `oled_joystick_asm.c` con el archivo incluido.
7. Copia `programa.S` a la misma carpeta. La S es mayúscula.
8. Ajusta CMake como se indica abajo.

## 3. Ajustar CMakeLists.txt

**Conserva el archivo generado por la extensión**, incluyendo las rutas, configuración de herramientas, `include(...)` del SDK y `pico_sdk_init()`. El archivo `CMake_fragmento.txt` es una referencia, no un CMake completo.

En la declaración existente del proyecto incluye ASM:

```cmake
project(oled_joystick_asm C CXX ASM)
```

Reemplaza el bloque `add_executable` existente:

```cmake
add_executable(oled_joystick_asm
    oled_joystick_asm.c
    programa.S
)
```

En el bloque existente de bibliotecas incluye las tres:

```cmake
target_link_libraries(oled_joystick_asm
    pico_stdlib
    hardware_adc
    hardware_i2c
)
```

Comprueba estas opciones, sin duplicarlas:

```cmake
pico_enable_stdio_usb(oled_joystick_asm 1)
pico_enable_stdio_uart(oled_joystick_asm 0)
pico_add_extra_outputs(oled_joystick_asm)
```

## 4. Compilar y cargar

1. Guarda y ejecuta Compile/Build en la extensión.
2. Con la tarjeta desconectada, mantén BOOT y conecta USB.
3. Cuando aparezca RPI-RP2, suelta BOOT.
4. Copia `build/oled_joystick_asm.uf2` a RPI-RP2.
5. **Deja el joystick suelto durante los primeros dos segundos**: se mide su centro automáticamente.
6. Mueve cada eje y luego ambos a la vez.
7. Presiona el joystick: el cuadro vuelve al centro. Si mantienes pulsado y mueves la palanca, puede desplazarse otra vez; el centrado ocurre una vez por pulsación.
8. Opcional: abre el monitor USB serial (115200 si lo solicita) para ver lecturas y diagnóstico.

## 5. Qué está en ensamblador

`programa.S` contiene el ciclo principal: preparar, leer, dibujar, esperar y repetir. Además, `mover_eje_asm` decide el movimiento y limita la posición a la zona visible.

| Registro de mover_eje_asm | Dato |
|---|---|
| r0 | Posición actual; al regresar contiene la nueva posición |
| r1 | Lectura ADC menos centro calibrado |
| r2 | Posición mínima permitida |
| r3 | Posición máxima permitida |
| r4 | Constante 350; se conserva mediante push/pop |

- Diferencia mayor que 350: avanzar un píxel.
- Diferencia menor que −350: retroceder un píxel.
- Entre −350 y 350: no moverse (zona muerta).
- `cmp`, `bgt`, `blt`, `bge` y `ble` comparan valores con signo.
- `adds` y `subs` modifican la posición.
- `push {r4, lr}` conserva registros y mantiene la pila alineada a ocho bytes.
- C llama a esta función una vez por eje: así admite diagonales.

C se encarga de acceder al SDK, promediar ocho conversiones por eje, calibrar el centro, eliminar rebotes del botón y construir/enviar la imagen por I2C. **Es ensamblador con apoyo en C, igual que la práctica anterior; no es ensamblador puro.** El dibujo no requiere instalar una biblioteca OLED externa.

La velocidad es constante fuera de la zona muerta: no aumenta al inclinar más la palanca. El periodo real incluye la transferencia de imagen, además de los 20 ms de pausa.

## 6. Ajustes y problemas

| Situación | Acción |
|---|---|
| Eje horizontal invertido | Cambia `INVERTIR_X` de 0 a 1 en C |
| Eje vertical invertido | Cambia `INVERTIR_Y` de 0 a 1 en C |
| Ejes intercambiados | Con USB desconectado, intercambia VRx y VRy |
| Se mueve solo | Reinicia sin tocar la palanca; si persiste, revisa cableado y aumenta 350 en ASM |
| OLED en blanco | Revisa controlador SSD1306, resolución, alimentación, GND, SDA y SCL |
| USB informa OLED=ERROR | No hubo respuesta I2C o falló una transferencia; revisa cables y reinicia |
| OLED=OK pero imagen incorrecta | Un ACK no identifica el controlador; verifica que sea SSD1306 128×64 |
| Botón siempre en 0 | Revisa SW a GPIO15 y GND común |
| ADC siempre en un extremo | Revisa alimentación de 3.3 V y conexiones VRx/VRy |
| Error de enlace a funciones ASM | Incluye programa.S y habilita ASM en project() |

El controlador intenta 0x3C y después 0x3D. Una falla durante el funcionamiento detiene los envíos a la pantalla, pero conserva lecturas y diagnóstico USB. Tras corregir las conexiones, reinicia.

## Validación y referencias

Se revisaron estáticamente el mapeo de canales, las llamadas C/ASM, la alineación de pila, los límites del cuadro y la lógica de antirrebote. **Pendiente de compilación con Pico SDK y prueba física**: este paquete contiene fuentes, no un UF2 compilado.

- [RP2040-Zero, Waveshare](https://www.waveshare.com/wiki/RP2040-Zero)
- [API de hardware del Pico SDK](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html)
- [Ejemplos oficiales Pico](https://github.com/raspberrypi/pico-examples)

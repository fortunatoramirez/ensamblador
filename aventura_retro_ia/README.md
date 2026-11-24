# Estudio retro: videojuego en ensamblador asistido por IA

## Propósito

Transformar [Cazador de objetos](../oled_cazador_asm) en una aventura propia con personaje animado, objetos especiales y una nueva función en ensamblador. Utilizarás inteligencia artificial para diseñar, comprender, programar y corregir tu juego.

La IA será tu asistente durante el desarrollo. El juego terminado se ejecutará en la RP2040-Zero sin Internet ni servicios de IA.

## Material y punto de partida

- RP2040-Zero, OLED SSD1306 I2C de 128×64 y joystick de la práctica anterior.
- Visual Studio Code con el entorno Pico que ya funciona.
- Archivos de `oled_cazador_asm`: programa C, `programa.S` y configuración del proyecto.
- Acceso a un asistente de IA de texto y código.
- Editor de píxeles para adaptar los gráficos.

Conserva las conexiones anteriores: OLED SDA=GPIO0 y SCL=GPIO1; joystick VRx=GPIO26, VRy=GPIO27 y SW=GPIO15. Alimenta los módulos a 3.3 V y comparte GND.

## 1. Elegir tus herramientas

Puedes trabajar con un asistente de conversación que permita compartir código, como ChatGPT, o con [GitHub Copilot](https://github.com/features/copilot) dentro de VS Code. Utiliza el que tengas disponible; no necesitas varias suscripciones.

Para los gráficos puedes generar una referencia con [Adobe Firefly](https://www.adobe.com/products/firefly/features/text-to-image.html) o con una herramienta de generación de imágenes disponible para ti. Después simplifica el dibujo en [Pixilart](https://www.pixilart.com/draw), que es un editor de píxeles.

También puedes pedir al asistente una cuadrícula de ceros y unos y dibujarla directamente. Esto permite completar la práctica sin una herramienta de generación de imágenes.

**Regla de trabajo:** pide una modificación pequeña, entiende qué cambia, compila y comprueba el resultado antes de continuar.

## 2. Preparar una copia que funcione

1. Conserva intacto el proyecto anterior.
2. Crea una carpeta local llamada `mi_aventura_ia` y copia los archivos fuente de tu proyecto funcional.
3. Si copiaste también la carpeta `build`, elimínala de la copia: contiene rutas de la ubicación anterior.
4. Abre la nueva carpeta en VS Code.
5. Conserva por ahora los nombres del ejecutable y del archivo C dentro de CMake. El nombre de la carpeta puede ser diferente.
6. Ejecuta Clean CMake, Configure CMake y compila.
7. Carga el nuevo UF2 y juega una partida.

El UF2 conservará el nombre del ejecutable configurado, por ejemplo `oled_cazador_asm.uf2`.

**Punto de comprobación:** antes de modificar nada, deben funcionar movimiento, comida, minas, pausa y reinicio.

## 3. Entender el programa con ayuda de IA

Comparte con el asistente los archivos C y `programa.S`. No supongas que puede leer un enlace: si no tiene acceso, adjunta o pega el contenido.

Usa este contexto al iniciar:

> Trabajo con una RP2040-Zero, Arm Cortex-M0+, instrucciones ARMv6-M Thumb, sintaxis GNU y Pico SDK. Tengo una OLED SSD1306 I2C de 128×64 y un joystick. Mi proyecto combina C y ensamblador. Conserva el controlador OLED, el cableado y la configuración que ya funcionan. Propón cambios pequeños compatibles con esta arquitectura.

Después pide:

> Explica el recorrido desde main hasta el ciclo del juego. Identifica qué hace C y qué hace ensamblador. Luego explica mover_eje_asm y colision_asm, indicando entradas, salida y registros que cambian. Todavía no modifiques código.

Busca las funciones en tus archivos mientras lees la respuesta. En la base actual:

| Función | Responsabilidad |
|---|---|
| `programa_asm` | Coordina el ciclo principal. |
| `mover_eje_asm` | Mueve y limita una coordenada. |
| `colision_asm` | Comprueba contacto entre cajas de 5×5. |
| `actualizar_juego` | Administra estados, puntos, vidas y tiempo en C. |
| `dibujar_pantalla` | Dibuja la escena y actualiza la OLED. |

Comprueba una explicación con números: jugador en (30,30), objeto en (34,30). La distancia X es 4 y la distancia Y es 0; la rutina original debe indicar colisión. Si el objeto está en (35,30), ya no hay contacto según esa regla.

## 4. Diseñar una aventura propia

Define un personaje, algo que deba recolectar y un peligro. Puedes imaginar rescate espacial, exploración submarina, una misión de un robot u otra temática.

Pide:

> Propón tres aventuras breves basadas en mi juego actual. Cada una debe conservar joystick, vidas, tiempo y pausa, y funcionar en una pantalla monocromática de 128×64. Incluye personaje, objeto normal, objeto especial y peligro. Evita hardware adicional. Recomienda la más sencilla de implementar por etapas.

Elige una y escribe sus reglas antes de programar:

- ¿Qué recolecta el personaje?
- ¿Qué representa una mina?
- ¿Cómo se distingue el objeto especial?
- ¿Cómo se gana y se pierde?

La mejora común será un **objeto especial que vale dos puntos**, con una función nueva en ASM para sumar sin superar la meta. Después podrás elegir otra habilidad como ampliación.

## 5. Dar identidad al personaje

Empieza con dos cuadros de animación de **8×8 píxeles**, blanco y negro. Serán dos versiones del mismo personaje: por ejemplo, una nave con dos posiciones de su propulsor.

Para una referencia visual puedes pedir:

> Diseña un personaje para una aventura de [tema]. Debe poder simplificarse a un sprite de 8×8 píxeles, monocromático, con silueta reconocible y pocos detalles. Propón dos poses para una animación sencilla.

Una imagen generada puede parecer pixel art sin respetar una cuadrícula exacta. Redibújala en una cuadrícula real de 8×8 o pide:

> Representa el personaje mediante ocho filas de ocho valores 0 o 1. 1 significa píxel encendido. Genera dos cuadros del mismo tamaño y explica qué cambia entre ellos.

### De píxeles a bytes

Cada fila de ocho píxeles cabe en un byte. Por ejemplo:

```text
00011000 = 0x18
00111100 = 0x3C
```

Solicita la conversión de tus ocho filas a ocho bytes y comprueba al menos una fila manualmente.

**Nuestro formato será por filas, con el bit 7 a la izquierda.** El buffer interno de la OLED utiliza otra organización; no copies estos bytes directamente al buffer.

Pide al asistente una función C que recorra las ocho filas y columnas, consulte cada bit con una máscara y llame a la función existente `pixel(x,y)` cuando corresponda. La firma puede ser:

```c
static void dibujar_sprite(int izquierda, int arriba,
                           const uint8_t filas[8]);
```

Mantén inicialmente el dibujo en C: la nueva lógica de puntuación se trabajará en ASM.

## 6. Integrar el sprite sin romper el movimiento

Las coordenadas actuales `x,y` representan el centro del personaje. Define que el sprite de 8×8 se dibuje desde:

```text
izquierda = x - 3
arriba    = y - 3
```

Así ocupa desde x−3 hasta x+4, y desde y−3 hasta y+4.

Para conservarlo dentro del área actual de juego, cambia los límites enviados a `mover_eje_asm`:

| Eje | Mínimo | Máximo |
|---|---:|---:|
| X | 4 | 122 |
| Y | 20 | 58 |

El área interior está entre X=1..126 y Y=17..62. Comprueba los cuatro bordes.

Pide:

> Sustituye solamente el cuadro del jugador por mi sprite de 8×8, dibujado desde x−3,y−3. Mantén comida y minas. Indica el bloque exacto que reemplazaré y ajusta los límites de movimiento. No cambies todavía la lógica de estados.

### Ajustar las colisiones

La rutina original supone dos figuras de 5×5. El nuevo personaje es mayor: cambiar el dibujo no cambia automáticamente su colisión.

Para esta etapa usa una caja de colisión **conservadora de 9×9 centrada en el jugador**; comida y minas permanecen en 5×5. La condición será:

```text
abs(x - objeto_x) <= 6
y
abs(y - objeto_y) <= 6
```

Esta caja incluye un pequeño margen alrededor del sprite; puede contar contacto en algún píxel vacío. Es una simplificación deliberada.

Pide al asistente que identifique y ajuste las dos comparaciones de distancia en `colision_asm`. Prueba diferencias de 5, 6 y 7 en ambos ejes. Debe haber contacto hasta 6 cuando la otra distancia también cumple.

**Punto de comprobación:** el personaje se reconoce, no atraviesa los bordes y conserva colisiones comprensibles.

## 7. Animar al personaje

Alterna los dos cuadros aproximadamente cada 150 ms mientras se juega. Usa tiempo transcurrido, no una espera adicional que congele el juego.

Pide:

> Integra dos cuadros de animación de 8×8 alternados cada 150 ms. Usa el reloj del SDK y resta de contadores uint32_t. No agregues sleep_ms dentro del dibujo. La pausa debe congelar la animación y al reiniciar debe comenzar en el primer cuadro. Explícame las variables nuevas y dónde se actualizan.

Prueba que mover la palanca y pulsar pausa siga respondiendo.

## 8. Crear el objeto especial

Mantén un único objeto recolectable activo. Algunos serán normales y otros especiales; esto evita agregar más objetos y colisiones simultáneas.

Una regla sencilla:

- Los primeros tres objetos generados son normales.
- El cuarto es especial.
- El patrón se repite y comienza de nuevo al reiniciar.
- El normal vale 1 punto; el especial, 2.
- El especial debe distinguirse con un dibujo de hasta 5×5, para conservar su colisión.
- Define el tipo al generar un nuevo objeto tras una captura. Un golpe que reorganiza el tablero conserva el tipo actual.

Pide:

> Agrega este patrón usando un contador de objetos generados y un indicador de objeto especial. Todavía conserva el incremento de puntos actual. Muéstrame únicamente las variables nuevas y las modificaciones de inicio, captura y dibujo. La recolocación por una mina no debe adelantar el patrón.

Prueba el patrón visual antes de modificar la puntuación.

## 9. Construir una función nueva en ensamblador

Ahora sustituirás el incremento de puntos por una función que permita sumar 1 o 2 sin rebasar la meta:

```c
extern uint32_t sumar_puntos_asm(uint32_t actuales,
                                uint32_t premio,
                                uint32_t meta);
```

| Registro | Contenido al entrar |
|---|---|
| r0 | Puntos actuales. |
| r1 | Premio: 1 o 2. |
| r2 | Meta. |
| r0 al regresar | Nuevo puntaje, limitado a la meta. |

Usaremos cantidades pequeñas no negativas: puntos entre 0 y la meta, premio 1 o 2 y meta de hasta 100. No es una rutina general para sumas arbitrarias de 32 bits.

Primero pide el algoritmo:

> Diseña el pseudocódigo de sumar_puntos_asm: sumar actuales y premio; si supera la meta, devolver la meta. Explica cómo recorrerlo con 9 puntos, premio 2 y meta 10. Todavía no escribas ASM.

Después pide la implementación:

> Convierte el algoritmo a ensamblador GNU Thumb para Cortex-M0+. Usa r0,r1,r2 como argumentos y r0 como resultado. Usa comparación sin signo, conserva los registros que exige la convención de llamadas y explica cada instrucción. Incluye las directivas de función compatibles con mi programa.S. No uses instrucciones de Cortex-M3/M4 ni división de hardware.

Busca la suma, la comparación y el salto. Explica por qué el resultado de 9+2 debe ser 10 en este juego.

Agrega la función a `programa.S` **antes de la sección final `.note.GNU-stack`**, dentro de `.text`. No dupliques etiquetas existentes.

En C, declara la función y reemplaza `++puntos;` por una llamada que pase el premio correspondiente. Conserva después la comprobación de victoria.

### Revisar la dificultad

Al ganar dos puntos puedes saltar un umbral: de 2 pasarás a 4. Conserva la regla actual de minas basada en `puntos/3`, con máximo tres. Debe haber una mina al alcanzar 3 o más, dos al alcanzar 6 o más y tres al alcanzar 9 o más.

## 10. Comprobar que la IA acertó

Antes de jugar, prueba la función de puntos con valores conocidos. Puedes pedir ayuda para imprimir resultados por USB desde C y retirar ese código de prueba después.

| Actuales | Premio | Meta | Resultado esperado |
|---:|---:|---:|---:|
| 0 | 1 | 10 | 1 |
| 3 | 2 | 10 | 5 |
| 8 | 2 | 10 | 10 |
| 9 | 2 | 10 | 10 |
| 10 | 1 | 10 | 10 |

Luego revisa en la tarjeta:

- Una captura normal suma 1 y una especial suma 2.
- Llegar a la meta, incluso con premio doble, activa la victoria.
- El contador del patrón y los puntos se reinician.
- Las minas siguen quitando una sola vida por contacto.
- La pausa conserva el tiempo de juego y congela la animación.
- El personaje permanece dentro de los bordes.

Si falla algo, pide:

> Esperaba [resultado], pero observé [comportamiento]. Este es el fragmento relevante y este es el mensaje completo del compilador, si lo hay. Identifica la causa probable y propón el cambio mínimo. Explica cómo comprobarlo antes de sugerir otra modificación.

Una respuesta convincente de la IA es una hipótesis: la compilación, el seguimiento de registros y la prueba física permiten comprobarla.

## 11. Personalizar la presentación

Cambia portada, instrucciones y mensajes para que correspondan a tu aventura. Usa frases cortas.

La fuente incluida reconoce mayúsculas A–Z, dígitos, dos puntos y diagonal; usa texto sin acentos ni ñ, salvo que agregues sus patrones a la fuente.

Pide:

> Propón textos breves en MAYÚSCULAS, sin acentos ni ñ, para portada, pausa, victoria y derrota. Mi pantalla es de 128×64 y la fuente avanza cuatro píxeles por carácter. Deja márgenes y evita que los textos se salgan.

Compila y juega una partida completa con la presentación final.

## 12. Ampliación creativa

Cuando la versión anterior funcione, elige **una** ampliación:

| Idea | Qué cambia | Posible participación del ASM |
|---|---|---|
| Escudo | Un objeto permite resistir un golpe. | Decidir si consume escudo o resta vida. |
| Perseguidor | Un enemigo se acerca al jugador a intervalos. | Calcular un paso hacia una coordenada objetivo. |
| Niveles | Cambian tiempo, meta o dificultad. | Elegir un nivel según el puntaje. |

Pide primero reglas y casos de prueba. Para efectos temporales usa el tiempo de partida que excluye las pausas. Para enemigos móviles evita daño repetido inmediato con un periodo de protección o recolocación segura.

Agrega la ampliación en una copia funcional y prueba cada modificación. El resultado será una aventura diseñada por ti, con gráficos propios y lógica que puedes explicar desde los registros hasta la pantalla.

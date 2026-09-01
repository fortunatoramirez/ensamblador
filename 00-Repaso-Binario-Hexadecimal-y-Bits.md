# Clase 0 — Repaso de Binario, Hexadecimal y Representación de Datos

> **Objetivo:** repasar los sistemas numéricos y conceptos básicos de representación de datos necesarios antes de comenzar formalmente con lenguaje ensamblador.

---

## 1. ¿Por qué necesitamos este repaso?

Antes de trabajar con ensamblador debemos sentirnos cómodos con:

- bits y bytes;
- binario;
- decimal;
- hexadecimal;
- conversiones entre sistemas;
- posiciones de bits;
- operaciones lógicas;
- máscaras;
- desplazamientos;
- números con y sin signo.

En lenguajes de alto nivel muchas de estas cosas quedan ocultas. En ensamblador aparecerán constantemente expresiones como:

```asm
MOV R0, #10
LDR R1, =0x40020000
ORR R0, R0, #0x04
```

Para entenderlas bien necesitamos dominar primero cómo se representan los números.

---

# 2. Sistemas numéricos

| Sistema | Base | Símbolos |
|---|---:|---|
| Binario | 2 | `0`, `1` |
| Decimal | 10 | `0` a `9` |
| Hexadecimal | 16 | `0` a `9`, `A` a `F` |

---

# 3. Sistema decimal

El sistema decimal tiene base 10.

Por ejemplo:

```text
538
```

equivale a:

```text
5 × 10² + 3 × 10¹ + 8 × 10⁰
```

es decir:

```text
500 + 30 + 8
```

La posición de cada dígito determina su valor.

---

# 4. Sistema binario

El sistema binario utiliza solamente:

```text
0
1
```

y tiene base 2.

Cada posición representa una potencia de 2.

Ejemplo:

```text
1011₂
```

equivale a:

```text
1×2³ + 0×2² + 1×2¹ + 1×2⁰
```

por lo tanto:

```text
8 + 0 + 2 + 1 = 11
```

Entonces:

```text
1011₂ = 11₁₀
```

---

# 5. Potencias de 2 importantes

| Potencia | Valor |
|---:|---:|
| `2⁰` | 1 |
| `2¹` | 2 |
| `2²` | 4 |
| `2³` | 8 |
| `2⁴` | 16 |
| `2⁵` | 32 |
| `2⁶` | 64 |
| `2⁷` | 128 |
| `2⁸` | 256 |
| `2⁹` | 512 |
| `2¹⁰` | 1024 |

Conviene memorizar al menos:

```text
1, 2, 4, 8, 16, 32, 64, 128, 256
```

---

# 6. Posiciones de bits

Consideremos:

```text
10110110
```

Las posiciones son:

```text
Bit:      7   6   5   4   3   2   1   0
Valor:  128  64  32  16   8   4   2   1
Dato:     1   0   1   1   0   1   1   0
```

Sumamos las posiciones activas:

```text
128 + 32 + 16 + 4 + 2 = 182
```

Entonces:

```text
10110110₂ = 182₁₀
```

---

# 7. ¿Qué es un bit?

Un bit puede tomar dos valores:

```text
0
1
```

Dependiendo del contexto, puede representar:

```text
apagado / encendido
falso / verdadero
bajo / alto
```

---

# 8. ¿Qué es un byte?

Un byte contiene:

```text
8 bits
```

Ejemplo:

```text
10101100
```

Con 8 bits existen:

```text
2⁸ = 256
```

combinaciones.

Si se interpreta como número sin signo:

```text
00000000 = 0
11111111 = 255
```

---

# 9. Tamaños comunes

| Nombre | Tamaño |
|---|---:|
| Bit | 1 bit |
| Nibble | 4 bits |
| Byte | 8 bits |
| Halfword | 16 bits |
| Word | 32 bits |
| Doubleword | 64 bits |

En ARM Cortex-M trabajaremos mucho con 8, 16 y 32 bits.

---

# 10. Nibble

Un **nibble** contiene 4 bits.

Ejemplo:

```text
1010
```

Esto es importante porque:

> **Un dígito hexadecimal representa exactamente 4 bits.**

---

# 11. LSB y MSB

## LSB

**Least Significant Bit**

Es el bit de menor peso.

Normalmente:

```text
bit 0
```

## MSB

**Most Significant Bit**

Es el bit de mayor peso.

En un byte:

```text
bit 7
```

Ejemplo:

```text
10110010
↑      ↑
MSB    LSB
```

---

# 12. Sistema hexadecimal

Hexadecimal tiene base 16.

Utiliza:

```text
0 1 2 3 4 5 6 7 8 9 A B C D E F
```

| Hex | Decimal |
|---|---:|
| `0` | 0 |
| `1` | 1 |
| `2` | 2 |
| `3` | 3 |
| `4` | 4 |
| `5` | 5 |
| `6` | 6 |
| `7` | 7 |
| `8` | 8 |
| `9` | 9 |
| `A` | 10 |
| `B` | 11 |
| `C` | 12 |
| `D` | 13 |
| `E` | 14 |
| `F` | 15 |

---

# 13. ¿Por qué usamos hexadecimal?

Un número binario puede ser largo:

```text
11111111101000100100110110001100
```

En hexadecimal puede escribirse de forma más compacta:

```text
0xFFA24D8C
```

La razón es:

```text
1 dígito hexadecimal = 4 bits
```

---

# 14. Tabla binario ↔ hexadecimal

| Binario | Hex |
|---|---|
| `0000` | `0` |
| `0001` | `1` |
| `0010` | `2` |
| `0011` | `3` |
| `0100` | `4` |
| `0101` | `5` |
| `0110` | `6` |
| `0111` | `7` |
| `1000` | `8` |
| `1001` | `9` |
| `1010` | `A` |
| `1011` | `B` |
| `1100` | `C` |
| `1101` | `D` |
| `1110` | `E` |
| `1111` | `F` |

---

# 15. Binario a hexadecimal

Convertir:

```text
11010110
```

Agrupamos de 4 en 4:

```text
1101 0110
```

Convertimos:

```text
1101 = D
0110 = 6
```

Resultado:

```text
0xD6
```

---

# 16. Hexadecimal a binario

Convertir:

```text
0x3A
```

Tomamos cada dígito:

```text
3 = 0011
A = 1010
```

Resultado:

```text
00111010₂
```

---

# 17. ¿Qué significa `0x`?

El prefijo:

```text
0x
```

indica que el número está en hexadecimal.

Ejemplo:

```text
0x10
```

significa:

```text
1×16 + 0
```

por lo tanto:

```text
0x10 = 16 decimal
```

---

# 18. Valores hexadecimal muy importantes

```text
0x01 = 00000001 = 1
0x02 = 00000010 = 2
0x04 = 00000100 = 4
0x08 = 00001000 = 8
0x10 = 00010000 = 16
0x20 = 00100000 = 32
0x40 = 01000000 = 64
0x80 = 10000000 = 128
```

Cada uno representa un solo bit activo dentro de un byte.

---

# 19. Hexadecimal a decimal

Convertir:

```text
0x2F
```

Como es base 16:

```text
2×16¹ + F×16⁰
```

Sabemos que:

```text
F = 15
```

Entonces:

```text
2×16 + 15 = 47
```

Por lo tanto:

```text
0x2F = 47 decimal
```

---

# 20. Decimal a binario

Ejemplo:

```text
13
```

Podemos construirlo con potencias de 2:

```text
8 + 4 + 1
```

Por lo tanto:

```text
1101₂
```

---

# 21. Decimal a binario mediante divisiones

Para convertir 13:

```text
13 / 2 = 6  residuo 1
 6 / 2 = 3  residuo 0
 3 / 2 = 1  residuo 1
 1 / 2 = 0  residuo 1
```

Leemos los residuos de abajo hacia arriba:

```text
1101
```

---

# 22. Decimal a hexadecimal

Convertir:

```text
254
```

Dividimos entre 16:

```text
254 / 16 = 15
```

Residuo:

```text
14
```

En hexadecimal:

```text
15 = F
14 = E
```

Resultado:

```text
0xFE
```

---

# 23. Los bits no tienen significado por sí solos

El patrón:

```text
01000001
```

puede representar:

- `65` decimal;
- la letra `A` en ASCII;
- parte de una instrucción;
- una máscara;
- un dato de sensor;
- una parte de una dirección.

> Los bits son solamente bits. El significado depende de cómo se interpreten.

---

# 24. Números sin signo

Para `n` bits, el rango sin signo es:

```text
0 a 2ⁿ - 1
```

Ejemplos:

```text
8 bits  → 0 a 255
16 bits → 0 a 65535
```

---

# 25. Números con signo

Para representar números negativos suele utilizarse:

> **Complemento a dos**

Con 8 bits podemos representar:

```text
-128 a 127
```

---

# 26. Complemento a dos

Para representar `-5` en 8 bits:

Primero:

```text
5 = 00000101
```

Invertimos:

```text
11111010
```

Sumamos 1:

```text
11111011
```

Entonces:

```text
-5 = 11111011
```

en complemento a dos de 8 bits.

---

# 27. ¿Por qué importa esto?

El mismo patrón de bits puede interpretarse de distintas maneras.

Por ejemplo:

```text
11111111
```

puede representar:

```text
255
```

si se interpreta como unsigned,

o:

```text
-1
```

si se interpreta como complemento a dos de 8 bits.

---

# 28. Operaciones lógicas

En ensamblador utilizaremos:

```text
AND
OR
XOR
NOT
```

Estas operaciones trabajan bit por bit.

---

# 29. AND

| A | B | Resultado |
|---|---|---|
| 0 | 0 | 0 |
| 0 | 1 | 0 |
| 1 | 0 | 0 |
| 1 | 1 | 1 |

Ejemplo:

```text
1010
AND
1100
----
1000
```

---

# 30. OR

| A | B | Resultado |
|---|---|---|
| 0 | 0 | 0 |
| 0 | 1 | 1 |
| 1 | 0 | 1 |
| 1 | 1 | 1 |

Ejemplo:

```text
1010
OR
1100
----
1110
```

---

# 31. XOR

| A | B | Resultado |
|---|---|---|
| 0 | 0 | 0 |
| 0 | 1 | 1 |
| 1 | 0 | 1 |
| 1 | 1 | 0 |

Ejemplo:

```text
1010
XOR
1100
----
0110
```

---

# 32. NOT

Invierte cada bit:

```text
0 → 1
1 → 0
```

Ejemplo:

```text
10110010
```

se convierte en:

```text
01001101
```

---

# 33. Máscaras de bits

Una máscara permite trabajar con bits específicos.

Supongamos:

```text
registro = 10100000
```

Queremos activar el bit 2.

Máscara:

```text
00000100
```

Aplicamos OR:

```text
10100000
OR
00000100
--------
10100100
```

---

# 34. Activar un bit

Para activar un bit utilizamos normalmente:

```text
OR
```

Ejemplo:

```text
00010000
OR
00000100
--------
00010100
```

---

# 35. Apagar un bit

Para apagar un bit utilizamos normalmente:

```text
AND
```

con una máscara invertida.

Ejemplo, apagar bit 2:

```text
11111111
AND
11111011
--------
11111011
```

---

# 36. Alternar un bit

Para cambiar un bit de 0 a 1 o de 1 a 0 podemos usar:

```text
XOR
```

Ejemplo:

```text
00000100
XOR
00000100
--------
00000000
```

---

# 37. Leer un bit

Supongamos:

```text
registro = 10110100
```

Queremos revisar el bit 2:

```text
10110100
AND
00000100
--------
00000100
```

Como el resultado no es cero:

```text
el bit 2 está activo
```

---

# 38. Desplazamientos de bits

Podemos desplazar bits:

```text
hacia la izquierda
```

o:

```text
hacia la derecha
```

---

# 39. Desplazamiento a la izquierda

Partimos de:

```text
00000001
```

Una posición:

```text
00000010
```

Otra:

```text
00000100
```

Otra:

```text
00001000
```

Observa los valores:

```text
1
2
4
8
```

En números positivos, desplazar a la izquierda normalmente equivale a multiplicar por 2.

---

# 40. Operador `<<`

En muchos lenguajes:

```text
<<
```

representa desplazamiento a la izquierda.

Ejemplos:

```text
1 << 0 = 1
1 << 1 = 2
1 << 2 = 4
1 << 3 = 8
1 << 4 = 16
1 << 5 = 32
```

---

# 41. Crear una máscara mediante desplazamiento

Para seleccionar el bit 5:

```text
1 << 5
```

Partimos de:

```text
00000001
```

desplazamos cinco posiciones:

```text
00100000
```

Resultado:

```text
32 decimal
0x20 hexadecimal
```

---

# 42. Tabla de máscaras

| Bit | Binario | Hex |
|---:|---|---|
| 0 | `00000001` | `0x01` |
| 1 | `00000010` | `0x02` |
| 2 | `00000100` | `0x04` |
| 3 | `00001000` | `0x08` |
| 4 | `00010000` | `0x10` |
| 5 | `00100000` | `0x20` |
| 6 | `01000000` | `0x40` |
| 7 | `10000000` | `0x80` |

---

# 43. Desplazamiento a la derecha

Ejemplo:

```text
10000000
```

una posición:

```text
01000000
```

otra:

```text
00100000
```

otra:

```text
00010000
```

En números positivos, desplazar a la derecha suele equivaler a dividir entre 2.

---

# 44. ¿Por qué esto importa en microcontroladores?

Los periféricos se controlan mediante registros.

Un registro podría verse así:

```text
Bit:       7 6 5 4 3 2 1 0
Contenido: 0 0 1 0 0 1 0 1
```

Cada bit puede controlar una función diferente.

Por ejemplo:

```text
bit 0 → activar periférico
bit 1 → habilitar interrupción
bit 2 → seleccionar modo
```

Por eso necesitaremos saber:

```text
activar bits
apagar bits
leer bits
crear máscaras
desplazar bits
```

---

# 45. Ejemplo conceptual de un registro

Imaginemos un registro llamado:

```text
CONTROL
```

con:

```text
Bit:      7 6 5 4 3 2 1 0
Función:  - - - - - M I E
```

donde:

```text
E = Enable
I = Interrupt
M = Mode
```

Para activar solamente `Enable`:

```text
00000001
```

es decir:

```text
0x01
```

---

# 46. Activar varios bits

Queremos activar:

```text
bit 0
bit 2
```

Máscaras:

```text
00000001
00000100
```

Combinamos con OR:

```text
00000101
```

En hexadecimal:

```text
0x05
```

---

# 47. Ejemplo con bits 3 y 5

```text
bit 3 = 00001000 = 0x08
bit 5 = 00100000 = 0x20
```

Combinamos:

```text
00101000
```

Resultado:

```text
0x28
```

También puede expresarse conceptualmente como:

```text
(1 << 3) OR (1 << 5)
```

---

# 48. Hexadecimal y memoria

En sistemas embebidos veremos direcciones como:

```text
0x20000000
0x40020000
```

Son direcciones de memoria escritas en hexadecimal.

Hexadecimal es muy conveniente porque un número de 32 bits puede representarse con solamente 8 dígitos hexadecimales.

---

# 49. ¿Cuántos bits representa un número hexadecimal?

Como:

```text
1 dígito hexadecimal = 4 bits
```

entonces:

```text
0xA        → 4 bits
0xA3       → 8 bits
0xA3F2     → 16 bits
0xA3F24510 → 32 bits
```

---

# 50. Ejemplo de 32 bits

```text
0x1234ABCD
```

Cada dígito son 4 bits:

```text
1 = 0001
2 = 0010
3 = 0011
4 = 0100
A = 1010
B = 1011
C = 1100
D = 1101
```

Entonces:

```text
0001 0010 0011 0100 1010 1011 1100 1101
```

---

# 51. Conexión con ARM

Más adelante encontraremos instrucciones como:

```asm
AND
ORR
EOR
LSL
LSR
```

que corresponden a conceptos que acabamos de repasar:

| Ensamblador ARM | Concepto |
|---|---|
| `AND` | AND bit a bit |
| `ORR` | OR bit a bit |
| `EOR` | XOR bit a bit |
| `LSL` | desplazamiento izquierda |
| `LSR` | desplazamiento derecha |

---

# 52. Ejercicio 1 — Binario a decimal

Convierte:

```text
1010
1101
11110000
10000001
```

---

# 53. Ejercicio 2 — Decimal a binario

Convierte:

```text
5
12
25
100
```

---

# 54. Ejercicio 3 — Binario a hexadecimal

Convierte:

```text
10101010
11110000
00111101
1101111010101101
```

---

# 55. Ejercicio 4 — Hexadecimal a binario

Convierte:

```text
0x0F
0x32
0xA5
0xF0C3
```

---

# 56. Ejercicio 5 — Hexadecimal a decimal

Convierte:

```text
0x10
0x20
0xFF
0x100
```

---

# 57. Ejercicio 6 — Identificar bits

Para:

```text
10110100
```

indica qué bits están activos.

Recuerda:

```text
bit 7 ... bit 0
```

---

# 58. Ejercicio 7 — Máscaras

Escribe en binario y hexadecimal la máscara para activar:

```text
bit 0
bit 3
bit 6
bit 7
```

---

# 59. Ejercicio 8 — Máscara de varios bits

Crea una máscara con activos:

```text
bit 1
bit 5
```

Expresa el resultado en:

```text
binario
hexadecimal
decimal
```

---

# 60. Ejercicio 9 — Operaciones lógicas

Calcula:

```text
10101010
AND
11110000
```

Después:

```text
10101010
OR
00001111
```

Y:

```text
10101010
XOR
11111111
```

---

# 61. Ejercicio 10 — Desplazamientos

Calcula:

```text
00000001 << 1
00000001 << 4
00000001 << 7
```

---

# 62. Ejercicio 11 — Razonamiento

Sin calculadora:

```text
1 << 0
1 << 1
1 << 2
1 << 3
1 << 4
1 << 5
```

Expresa cada resultado también en hexadecimal.

---

# 63. Ejercicio 12 — Aplicación a registros

Supongamos un registro inicialmente en:

```text
00000000
```

Queremos activar:

```text
bit 2
bit 4
bit 7
```

Contesta:

1. ¿Cuál es la máscara binaria?
2. ¿Cuál es la máscara hexadecimal?
3. ¿Cuál es su valor decimal?

---

# 64. Preguntas de comprensión

1. ¿Por qué una computadora utiliza binario?
2. ¿Qué diferencia existe entre bit y byte?
3. ¿Cuántos bits tiene un nibble?
4. ¿Por qué hexadecimal es útil?
5. ¿Cuántos bits representa un dígito hexadecimal?
6. ¿Qué significa `0x`?
7. ¿Qué es el bit 0?
8. ¿Qué significa LSB?
9. ¿Qué significa MSB?
10. ¿Para qué sirve una máscara de bits?
11. ¿Qué operación utilizarías para activar un bit?
12. ¿Qué operación utilizarías para comprobar un bit?
13. ¿Qué ocurre al desplazar un número una posición a la izquierda?
14. ¿Qué significa `1 << 5`?
15. ¿Por qué todo esto será importante con microcontroladores?

---

# 65. Tabla de referencia rápida

| Decimal | Binario | Hex |
|---:|---|---|
| 0 | `0000` | `0` |
| 1 | `0001` | `1` |
| 2 | `0010` | `2` |
| 3 | `0011` | `3` |
| 4 | `0100` | `4` |
| 5 | `0101` | `5` |
| 6 | `0110` | `6` |
| 7 | `0111` | `7` |
| 8 | `1000` | `8` |
| 9 | `1001` | `9` |
| 10 | `1010` | `A` |
| 11 | `1011` | `B` |
| 12 | `1100` | `C` |
| 13 | `1101` | `D` |
| 14 | `1110` | `E` |
| 15 | `1111` | `F` |

---

# 66. Tabla de posiciones de un byte

```text
Bit       7    6    5    4    3    2    1    0
Valor   128   64   32   16    8    4    2    1
Hex     0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01
```

---

# 67. Resumen

Debemos recordar:

```text
1 bit = 0 o 1
8 bits = 1 byte
4 bits = 1 nibble
1 dígito hexadecimal = 4 bits
```

Los sistemas más importantes serán:

```text
binario      base 2
decimal      base 10
hexadecimal  base 16
```

Para manipular bits utilizaremos:

```text
AND
OR
XOR
NOT
```

y desplazamientos:

```text
<<
>>
```

Estos conceptos serán fundamentales cuando trabajemos con:

```text
registros
memoria
GPIO
periféricos
máscaras
banderas
instrucciones ARM
```

---

# 68. Conexión con la siguiente clase

Después de este repaso será mucho más natural encontrar instrucciones como:

```asm
MOV R0, #5
```

o:

```asm
LDR R1, =0x40020000
```

y posteriormente:

```asm
ORR R0, R0, #0x04
```

porque ya sabemos que:

```text
0x04 = 00000100
```

es decir:

```text
bit 2 activo
```

---

## Idea central

> **Binario es la representación natural de los circuitos digitales; hexadecimal es una forma compacta y cómoda para que las personas podamos leer y manipular esos bits.**

.syntax unified
.cpu cortex-m0plus
.thumb
.section .text
.balign 4
.global programa_asm
.type programa_asm, %function
.thumb_func
programa_asm:
    push {r4, lr}          @ 8 bytes: pila alineada al llamar C.
    bl preparar_hardware
ciclo:
    bl actualizar_joystick
    bl actualizar_juego
    bl dibujar_pantalla
    movs r0, #20
    bl esperar_ms
    b ciclo
.size programa_asm, .-programa_asm

@ int32_t mover_eje_asm(int32_t posicion, int32_t diferencia,
@                      int32_t minimo, int32_t maximo)
@ r0=posicion, r1=lectura-centro, r2=minimo, r3=maximo.
@ Zona muerta de +/-350; mover un pixel fuera de ella.
.global mover_eje_asm
.type mover_eje_asm, %function
.thumb_func
mover_eje_asm:
    push {r4, lr}
    ldr r4, =350
    cmp r1, r4
    bgt aumentar
    rsbs r4, r4, #0        @ r4 = -350
    cmp r1, r4
    blt disminuir
    b limitar
 aumentar:
    adds r0, #1
    b limitar
 disminuir:
    subs r0, #1
 limitar:
    cmp r0, r2
    bge revisar_maximo
    mov r0, r2
 revisar_maximo:
    cmp r0, r3
    ble terminar
    mov r0, r3
 terminar:
    pop {r4, pc}
.size mover_eje_asm, .-mover_eje_asm
.ltorg
@ int32_t colision_asm(x, y, objeto_x, objeto_y)
@ Dos cajas de 5x5 se tocan si |x-ox| <= 4 Y |y-oy| <= 4.
@ r0..r3 son argumentos; r0 regresa 1 o 0. No llama otras funciones.
.global colision_asm
.type colision_asm, %function
.thumb_func
colision_asm:
    subs r0, r0, r2
    bpl distancia_x
    rsbs r0, r0, #0
 distancia_x:
    cmp r0, #4
    bgt sin_colision
    subs r1, r1, r3
    bpl distancia_y
    rsbs r1, r1, #0
 distancia_y:
    cmp r1, #4
    bgt sin_colision
    movs r0, #1
    bx lr
 sin_colision:
    movs r0, #0
    bx lr
.size colision_asm, .-colision_asm
.section .note.GNU-stack,"",%progbits


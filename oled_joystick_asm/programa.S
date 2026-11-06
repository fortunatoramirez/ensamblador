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
.section .note.GNU-stack,"",%progbits

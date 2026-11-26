.syntax unified
.cpu cortex-m0plus
.thumb
.text
.balign 2
.global seleccionar_efecto_asm
.type seleccionar_efecto_asm, %function
.thumb_func
@ r0 recibe el evento. Devuelve el mismo ID si es 1..5, o 0.
@ Los IDs de eventos y efectos coinciden en esta practica.
seleccionar_efecto_asm:
    cmp r0, #1
    blo .Laudio_invalido
    cmp r0, #5
    bhi .Laudio_invalido
    bx lr
.Laudio_invalido:
    movs r0, #0
    bx lr
.size seleccionar_efecto_asm, .-seleccionar_efecto_asm
.section .note.GNU-stack,"",%progbits

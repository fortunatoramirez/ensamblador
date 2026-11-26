#ifndef AVENTURA_AUDIO_H
#define AVENTURA_AUDIO_H
#include <stdint.h>
enum {
    AUDIO_NINGUNO = 0,
    AUDIO_CAPTURA = 1,
    AUDIO_ESPECIAL = 2,
    AUDIO_GOLPE = 3,
    AUDIO_VICTORIA = 4,
    AUDIO_DERROTA = 5
};
void audio_iniciar(void);
void audio_evento(uint32_t evento);
void audio_actualizar(void);
void audio_detener(void);
uint32_t seleccionar_efecto_asm(uint32_t evento);
#endif

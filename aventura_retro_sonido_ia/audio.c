#include "audio.h"
#include <stdbool.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_PIN 14
typedef struct { uint16_t hz, ms; } Nota;

// Personaliza SOLO estas cinco tablas con ayuda de IA.
// Cada par es {frecuencia_Hz, duracion_ms}; 0 Hz es silencio.
static const Nota captura[] = {{1047,80}};
static const Nota especial[] = {{1047,80},{0,40},{1568,120}};
static const Nota golpe[] = {{392,120},{262,160}};
static const Nota victoria[] = {{784,100},{988,100},{1175,100},{1568,220}};
static const Nota derrota[] = {{523,140},{392,140},{262,220}};

static const Nota *secuencia;
static size_t cantidad, indice;
static uint32_t comienzo;
static unsigned slice;
static bool listo;

static void tono(uint16_t hz) {
    // Este modulo reserva el slice PWM del GPIO14.
    pwm_set_enabled(slice, false);
    pwm_set_gpio_level(BUZZER_PIN, 0);
    pwm_set_counter(slice, 0);
    // TOP y duty se cargan con PWM detenido, evitando un ciclo transitorio.
    if (hz == 0) {
        pwm_set_enabled(slice, true);
        return;
    }
    if (hz < 150) hz = 150;
    if (hz > 4000) hz = 4000;
    uint32_t cuentas = clock_get_hz(clk_sys) / (64u * hz);
    if (cuentas < 2) cuentas = 2;
    if (cuentas > 65535) cuentas = 65535;
    pwm_set_wrap(slice, (uint16_t)(cuentas - 1));
    pwm_set_gpio_level(BUZZER_PIN, (uint16_t)(cuentas / 2));
    pwm_set_enabled(slice, true);
}

void audio_detener(void) {
    secuencia = NULL;
    cantidad = indice = 0;
    if (listo) tono(0);
}

void audio_iniciar(void) {
    slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 64.0f);
    pwm_init(slice, &config, false);
    pwm_set_gpio_level(BUZZER_PIN, 0);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    listo = true;
    audio_detener();
}

#define SELECCIONAR(tabla) do { \
    secuencia = tabla; cantidad = sizeof(tabla) / sizeof((tabla)[0]); \
} while (0)

void audio_evento(uint32_t evento) {
    if (!listo) return;
    uint32_t efecto = seleccionar_efecto_asm(evento);
    if (efecto == AUDIO_NINGUNO) return;
    // Un evento valido nuevo reemplaza al sonido anterior; no hay cola.
    audio_detener();
    switch (efecto) {
        case AUDIO_CAPTURA: SELECCIONAR(captura); break;
        case AUDIO_ESPECIAL: SELECCIONAR(especial); break;
        case AUDIO_GOLPE: SELECCIONAR(golpe); break;
        case AUDIO_VICTORIA: SELECCIONAR(victoria); break;
        case AUDIO_DERROTA: SELECCIONAR(derrota); break;
        default: return;
    }
    indice = 0;
    comienzo = to_ms_since_boot(get_absolute_time());
    tono(secuencia[0].hz);
}

void audio_actualizar(void) {
    if (!listo || !secuencia) return;
    uint32_t ahora = to_ms_since_boot(get_absolute_time());
    if ((uint32_t)(ahora - comienzo) < secuencia[indice].ms) return;
    ++indice;
    if (indice == cantidad) {
        audio_detener();
        return;
    }
    comienzo = ahora;
    tono(secuencia[indice].hz);
}

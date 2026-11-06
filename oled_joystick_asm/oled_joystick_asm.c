#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

#define SDA_PIN 0
#define SCL_PIN 1
#define SW_PIN  15
#define INVERTIR_X 0
#define INVERTIR_Y 0  // Cambiar a 1 si arriba mueve el cuadro hacia abajo.

extern void programa_asm(void);
extern int32_t mover_eje_asm(int32_t pos, int32_t diferencia,
                            int32_t minimo, int32_t maximo);
static uint8_t direccion;
static uint8_t buffer[1025] = {0x40}; // Control I2C + 1024 bytes de imagen.
static bool oled_ok;
static uint16_t lectura_x, lectura_y, centro_x, centro_y;
static int32_t x = 64, y = 40;
static bool sw_estable, sw_anterior;
static uint32_t sw_cambio;

static bool enviar(const uint8_t *datos, size_t n) {
    return i2c_write_timeout_us(i2c0, direccion, datos, n, false, 100000)
           == (int)n;
}
static bool comando(uint8_t c) {
    uint8_t datos[2] = {0x00, c};
    return enviar(datos, 2);
}
static bool iniciar_oled(void) {
    // Buscar las dos direcciones habituales. ACK no identifica el controlador.
    direccion = 0x3C;
    if (!comando(0xAE)) {
        direccion = 0x3D;
        if (!comando(0xAE)) return false;
    }
    const uint8_t inicio[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6
    };
    for (size_t i = 0; i < sizeof inicio; ++i)
        if (!comando(inicio[i])) return false;
    return true; // Se enciende despues de enviar la primera imagen.
}
static bool refrescar(void) {
    const uint8_t ventana[] = {0x00, 0x21, 0, 127, 0x22, 0, 7};
    return enviar(ventana, sizeof ventana) && enviar(buffer, sizeof buffer);
}
static void pixel(int px, int py) {
    if (px >= 0 && px < 128 && py >= 0 && py < 64)
        buffer[1 + px + (py / 8) * 128] |= (uint8_t)(1u << (py % 8));
}
// Fuente propia 3x5: cinco filas, tres bits por fila.
static void caracter(int px, int py, char c) {
    static const uint8_t digitos[10][5] = {
        {7,5,5,5,7}, {2,6,2,2,7}, {7,1,7,4,7}, {7,1,7,1,7},
        {5,5,7,1,1}, {7,4,7,1,7}, {7,4,7,5,7}, {7,1,1,1,1},
        {7,5,7,5,7}, {7,5,7,1,7}
    };
    uint8_t filas[5] = {0};
    if (c >= '0' && c <= '9') memcpy(filas, digitos[c-'0'], 5);
    else if (c == 'X') { uint8_t f[]={5,5,2,5,5}; memcpy(filas,f,5); }
    else if (c == 'Y') { uint8_t f[]={5,5,2,2,2}; memcpy(filas,f,5); }
    else if (c == 'B') { uint8_t f[]={6,5,6,5,6}; memcpy(filas,f,5); }
    else if (c == ':') { filas[1]=2; filas[3]=2; }
    for (int r=0; r<5; ++r)
        for (int col=0; col<3; ++col)
            if (filas[r] & (1u << (2-col))) pixel(px+col,py+r);
}
static void texto(int px, int py, const char *s) {
    while (*s) { caracter(px,py,*s++); px+=4; }
}
static uint16_t leer_canal(uint canal) {
    adc_select_input(canal);
    (void)adc_read(); // Descartar primera conversion tras cambiar de canal.
    uint32_t suma=0;
    for (int i=0; i<8; ++i) suma+=adc_read();
    return (uint16_t)(suma/8);
}
void preparar_hardware(void) {
    adc_init();
    adc_gpio_init(26); // VRx -> ADC0.
    adc_gpio_init(27); // VRy -> ADC1.
    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN); // SW conecta a GND al presionar.
    i2c_init(i2c0, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    sleep_ms(1000); // Dejar joystick suelto al arrancar.
    uint32_t sx=0, sy=0;
    for (int i=0; i<32; ++i) {
        sx+=leer_canal(0); sy+=leer_canal(1); sleep_ms(2);
    }
    centro_x=(uint16_t)(sx/32); centro_y=(uint16_t)(sy/32);
    oled_ok=iniciar_oled();
}
void actualizar_joystick(void) {
    lectura_x=leer_canal(0);
    lectura_y=leer_canal(1);
    int32_t dx=(int32_t)lectura_x-centro_x;
    int32_t dy=(int32_t)lectura_y-centro_y;
    if (INVERTIR_X) dx=-dx;
    if (INVERTIR_Y) dy=-dy;
    // Ambas llamadas por vuelta permiten las diagonales.
    x=mover_eje_asm(x, dx, 3, 124);
    y=mover_eje_asm(y, dy, 19, 60);
    bool pulsado=!gpio_get(SW_PIN);
    uint32_t ahora=to_ms_since_boot(get_absolute_time());
    if (pulsado != sw_anterior) { sw_anterior=pulsado; sw_cambio=ahora; }
    if ((uint32_t)(ahora-sw_cambio)>=30 && pulsado!=sw_estable) {
        sw_estable=pulsado;
        if (sw_estable) { x=64; y=40; } // Una accion por pulsacion.
    }
    static uint32_t ultimo;
    if ((uint32_t)(ahora-ultimo)>=250) {
        ultimo=ahora;
        printf("X=%u Y=%u B=%u OLED=%s\n", (unsigned)lectura_x,
               (unsigned)lectura_y, (unsigned)sw_estable,
               oled_ok ? "OK" : "ERROR");
    }
}
void dibujar_pantalla(void) {
    if (!oled_ok) return;
    memset(buffer+1, 0, 1024);
    char linea[32];
    snprintf(linea,sizeof linea,"X:%04u Y:%04u B:%u",
             (unsigned)lectura_x,(unsigned)lectura_y,(unsigned)sw_estable);
    texto(2,4,linea);
    for (int i=0; i<128; ++i) { pixel(i,16); pixel(i,63); }
    for (int j=16; j<64; ++j) { pixel(0,j); pixel(127,j); }
    for (int j=-2; j<=2; ++j)
        for (int i=-2; i<=2; ++i)
            if (sw_estable || i==-2 || i==2 || j==-2 || j==2)
                pixel((int)x+i,(int)y+j);
    if (!refrescar() || !comando(0xAF)) oled_ok=false;
}
void esperar_ms(uint32_t ms) { sleep_ms(ms); }
int main(void) {
    stdio_init_all();
    programa_asm();
    return 0;
}

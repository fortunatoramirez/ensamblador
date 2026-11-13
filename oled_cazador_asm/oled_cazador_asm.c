#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

#define META 10u
#define DURACION_MS 60000u

#define SDA_PIN 0
#define SCL_PIN 1
#define SW_PIN  15
#define INVERTIR_X 0
#define INVERTIR_Y 0  // Cambiar a 1 si arriba mueve el cuadro hacia abajo.

extern void programa_asm(void);
extern int32_t mover_eje_asm(int32_t pos, int32_t diferencia,
                            int32_t minimo, int32_t maximo);
extern int32_t colision_asm(int32_t x, int32_t y, int32_t ox, int32_t oy);
typedef struct { int32_t x, y; } Objeto;
typedef enum { PORTADA, JUGANDO, PAUSA, GANASTE, PERDISTE } Estado;
static Estado estado = PORTADA;
static Objeto comida, minas[3];
static unsigned puntos, vidas, cantidad_minas;
static uint32_t inicio, pausa_inicio, semilla = 1;
static bool evento_boton;
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
    else if (c >= 'A' && c <= 'Z') {
        static const uint8_t letras[26][5] = {
            {2,5,7,5,5},{6,5,6,5,6},{3,4,4,4,3},{6,5,5,5,6},
            {7,4,6,4,7},{7,4,6,4,4},{3,4,5,5,3},{5,5,7,5,5},
            {7,2,2,2,7},{1,1,1,5,2},{5,5,6,5,5},{4,4,4,4,7},
            {5,7,7,5,5},{5,7,7,7,5},{2,5,5,5,2},{6,5,6,4,4},
            {2,5,5,7,3},{6,5,6,5,5},{3,4,2,1,6},{7,2,2,2,2},
            {5,5,5,5,7},{5,5,5,5,2},{5,5,7,7,5},{5,5,2,5,5},
            {5,5,2,2,2},{7,1,2,4,7}
        };
        memcpy(filas, letras[c-'A'], 5);
    }
    else if (c == ':') { filas[1]=2; filas[3]=2; }
    else if (c == '/') { filas[0]=1; filas[1]=1; filas[2]=2; filas[3]=4; filas[4]=4; }
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
    if (estado == JUGANDO) {
        x=mover_eje_asm(x, dx, 3, 124);
        y=mover_eje_asm(y, dy, 19, 60);
    }
    evento_boton=false;
    bool pulsado=!gpio_get(SW_PIN);
    uint32_t ahora=to_ms_since_boot(get_absolute_time());
    if (pulsado != sw_anterior) { sw_anterior=pulsado; sw_cambio=ahora; }
    if ((uint32_t)(ahora-sw_cambio)>=30 && pulsado!=sw_estable) {
        sw_estable=pulsado;
        if (sw_estable) evento_boton=true; // Un evento por pulsacion.
    }
    static uint32_t ultimo;
    if ((uint32_t)(ahora-ultimo)>=250) {
        ultimo=ahora;
        printf("X=%u Y=%u B=%u OLED=%s\n", (unsigned)lectura_x,
               (unsigned)lectura_y, (unsigned)sw_estable,
               oled_ok ? "OK" : "ERROR");
    }
}
// Xorshift32: posiciones variables, no se usa para seguridad.
static uint32_t aleatorio(void) {
    semilla ^= semilla << 13;
    semilla ^= semilla >> 17;
    semilla ^= semilla << 5;
    return semilla;
}
static bool cerca(Objeto a, Objeto b) {
    // Margen mayor que la colision para dejar espacio libre alrededor.
    return a.x-b.x >= -10 && a.x-b.x <= 10 &&
           a.y-b.y >= -10 && a.y-b.y <= 10;
}
static bool libre(Objeto o, unsigned anteriores, bool evitar_comida) {
    if (cerca(o, (Objeto){x,y})) return false;
    if (evitar_comida && cerca(o, comida)) return false;
    for (unsigned i=0; i<anteriores; ++i)
        if (cerca(o, minas[i])) return false;
    return true;
}
static Objeto colocar(unsigned anteriores, bool evitar_comida) {
    for (unsigned intento=0; intento<128; ++intento) {
        Objeto o={(int32_t)(5+aleatorio()%118), (int32_t)(21+aleatorio()%38)};
        if (libre(o, anteriores, evitar_comida)) return o;
    }
    // Recorrido finito de respaldo: siempre hay espacio para estos 4 objetos.
    for (int py=21; py<=58; ++py)
        for (int px=5; px<=122; ++px) {
            Objeto o={px,py};
            if (libre(o, anteriores, evitar_comida)) return o;
        }
    return (Objeto){5,21};
}
static void nuevo_tablero(void) {
    comida=colocar(0, false);
    for (unsigned i=0; i<cantidad_minas; ++i) minas[i]=colocar(i, true);
}
static void iniciar_partida(uint32_t ahora) {
    puntos=0; vidas=3; cantidad_minas=0;
    x=64; y=40; inicio=ahora;
    semilla=ahora ^ ((uint32_t)lectura_x<<16) ^ lectura_y;
    if (!semilla) semilla=1;
    estado=JUGANDO;
    nuevo_tablero();
}
void actualizar_juego(void) {
    uint32_t ahora=to_ms_since_boot(get_absolute_time());
    if (evento_boton) {
        if (estado==PORTADA || estado==GANASTE || estado==PERDISTE) {
            iniciar_partida(ahora);
        } else if (estado==JUGANDO) {
            pausa_inicio=ahora; estado=PAUSA;
        } else {
            inicio += (uint32_t)(ahora-pausa_inicio);
            estado=JUGANDO;
        }
        return;
    }
    if (estado!=JUGANDO) return;
    if ((uint32_t)(ahora-inicio)>=DURACION_MS) { estado=PERDISTE; return; }
    for (unsigned i=0; i<cantidad_minas; ++i) {
        if (colision_asm(x,y,minas[i].x,minas[i].y)) {
            --vidas;
            if (!vidas) { estado=PERDISTE; return; }
            x=64; y=40;
            nuevo_tablero(); // Todo reaparece lejos del jugador: no pierde vidas seguidas.
            return;
        }
    }
    if (colision_asm(x,y,comida.x,comida.y)) {
        ++puntos;
        if (puntos>=META) { estado=GANASTE; return; }
        cantidad_minas=puntos/3;
        if (cantidad_minas>3) cantidad_minas=3;
        nuevo_tablero();
    }
}
void dibujar_pantalla(void) {
    if (!oled_ok) return;
    memset(buffer+1, 0, 1024);
    char linea[32];
    uint32_t ahora=to_ms_since_boot(get_absolute_time());
    uint32_t transcurrido=(uint32_t)((estado==PAUSA ? pausa_inicio : ahora)-inicio);
    unsigned segundos=transcurrido>=DURACION_MS ? 0 : (DURACION_MS-transcurrido+999)/1000;
    if (estado==PORTADA) {
        texto(16,8,"CAZADOR DE OBJETOS");
        texto(12,23,"COME 10 EN 60 SEGUNDOS");
        texto(12,34,"EVITA X   VIDAS:3");
        texto(12,49,"PULSA PARA EMPEZAR");
    } else if (estado==GANASTE || estado==PERDISTE) {
        texto(30,12,estado==GANASTE ? "GANASTE" : "FIN DEL JUEGO");
        snprintf(linea,sizeof linea,"OBJETOS:%u/%u",puntos,META);
        texto(30,27,linea);
        texto(12,45,"PULSA PARA REINICIAR");
    } else if (estado==PAUSA) {
        texto(50,20,"PAUSA");
        texto(12,38,"PULSA PARA CONTINUAR");
    } else {
        snprintf(linea,sizeof linea,"P:%02u/%u V:%u T:%02u",puntos,META,vidas,segundos);
        texto(2,4,linea);
        for (int i=0; i<128; ++i) { pixel(i,16); pixel(i,63); }
        for (int j=16; j<64; ++j) { pixel(0,j); pixel(127,j); }
        // Jugador: cuadro hueco de 5x5. Comida: rombo relleno de 5x5.
        for (int j=-2; j<=2; ++j)
            for (int i=-2; i<=2; ++i) {
                if (i==-2 || i==2 || j==-2 || j==2) pixel(x+i,y+j);
                int ai=i<0 ? -i:i, aj=j<0 ? -j:j;
                if (ai+aj<=2) pixel(comida.x+i,comida.y+j);
            }
        for (unsigned n=0; n<cantidad_minas; ++n)
            for (int i=-2; i<=2; ++i) {
                pixel(minas[n].x+i,minas[n].y+i);
                pixel(minas[n].x+i,minas[n].y-i);
            }
    }
    if (!refrescar() || !comando(0xAF)) oled_ok=false;
}
void esperar_ms(uint32_t ms) { sleep_ms(ms); }
int main(void) {
    stdio_init_all();
    programa_asm();
    return 0;
}


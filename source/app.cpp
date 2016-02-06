#include "mbed-drivers/mbed.h"
#include "ws2812.h"
#include "pixel-array.h"

#define WS2812_BUF 8
#define NUM_COLORS 4
#define NUM_LEDS_PER_COLOR 2

PixelArray px(WS2812_BUF);

// See the program page for information on the timing numbers
// The given numbers are for the K64F
WS2812 ws(P0_2, WS2812_BUF);

void app_start(int, char**)
{

    ws.useII(WS2812::PER_PIXEL); // use per-pixel intensity scaling

    // set up the colours we want to draw with
    int colorbuf[NUM_COLORS] = {0x2f0000,0x2f2f00,0x002f00,0x002f2f};

    // for each of the colours (j) write out 10 of them
    // the pixels are written at the colour*10, plus the colour position
    // all modulus 60 so it wraps around
    for (int i = 0; i < WS2812_BUF; i++) {
        px.Set(i, colorbuf[(i / NUM_LEDS_PER_COLOR) % NUM_COLORS]);
    }

    // now all the colours are computed, add a fade effect using intensity scaling
    // compute and write the II value for each pixel
    for (int j=0; j<WS2812_BUF; j++) {
        // px.SetI(pixel position, II value)
        px.SetI(j%WS2812_BUF, 0xf+(0xf*(j%NUM_LEDS_PER_COLOR)));
    }

    // Now the buffer is written, rotate it
    // by writing it out with an increasing offset
    while (1) {
        for (int z=WS2812_BUF; z >= 0 ; z--) {
            ws.write_offsets(px.getBuf(),z,z,z);
            wait(0.075);
        }
    }

}
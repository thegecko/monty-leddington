/* Copyright (c) 2012 cstyles, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef WS2812_H
#define WS2812_H

#include "mbed-drivers/mbed.h"

#define FRAME_SIZE 24

//!Library for the WS2812 RGB LED with integrated controller

class WS2812
{
public:
    enum BrightnessControl { OFF, GLOBAL, PER_PIXEL };

    /**
    *   Constructor
    *
    * @param pin Output pin. Connect to "Din" on the first WS2812 in the strip
    * @param size Number of LEDs in your strip
    */
    WS2812(PinName pin, int size);

    /*!
    Destroys instance.
    */
    ~WS2812();

    /**
    *   Writes the given buffer to the LED strip with the given offsets.
    *   NOTE: This function is timing critical, therefore interrupts are disabled during the transmission section.
    *
    * @param buf Pointer to the PixelArray buffer
    * @param r_offset The offset where each each pixel pulls its red component. Wraps to beginning if end is reached.
    * @param g_offset The offset where each each pixel pulls its green component. Wraps to beginning if end is reached.
    * @param b_offset The offset where each each pixel pulls its blue component. Wraps to beginning if end is reached.
    *
    */
    void write_offsets(int buf[], int r_offset = 0, int g_offset = 0, int b_offset = 0);


    /**
    *   Writes the given buffer to the LED strip
    *   NOTE: This function is timing critical, therefore interrupts are disabled during the transmission section.
    *
    * @param buf Pointer to the PixelArray buffer
    *
    */
    void write(int buf[]);

    /**
    *   Sets the brightness mode
    *
    * @param bc The brightness control. Defaults to OFF. Possible values include OFF, GLOBAL, and PER_PIXEL
    *
    */
    void useII(BrightnessControl bc);

    /**
    *   Sets the global brightness level.
    *
    * @param II The brightness level. Possible values include 0 - 255 (0x00 - 0xFF).
    *
    */
    void setII(unsigned char II);



private:

    int __size;
    unsigned char __II;
    BrightnessControl __use_II;
    bool *__transmitBuf;
    void __loadBuf(int buf[],int r_offset=0, int g_offset=0, int b_offset=0);

    volatile uint32_t *__setPtr;
    volatile uint32_t *__clrPtr;

    DigitalOut __gpo;
};

#endif
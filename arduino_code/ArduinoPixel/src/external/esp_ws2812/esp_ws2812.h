/*! \file esp_ws2812.h
 *  \brief Class declaration of driver for the WS2812 RGB LEDs using the RMT
 * peripheral on ESP32.
 *  \note Based on public domain code created on 19 Nov 2016 by Chris Osborn
 * <fozztexx@fozztexx.com> http://insentricity.com
 *  \author Martin F. Falatic
 *  \date 2017
 *  \copyright The MIT License (MIT)
 *  \par
 *  Copyright (c) 2017 Martin F. Falatic
 *  \par
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  \par
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *  \par
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#ifndef ARDUINO_PIXEL_EXTERNAL_ESP_WS2812_ESP_WS2812_H
#ifdef ESP32
#define ARDUINO_PIXEL_EXTERNAL_ESP_WS2812_ESP_WS2812_H

#include "external/esp_ws2812/esp_ws2812_rmt.h"

class Ws2812 {
 public:
  enum class LedType : uint8_t { NONE, WS2812, WS2812B, SK6812, WS2813 };

  Ws2812(int num_leds, int pin, LedType type);

  ~Ws2812();
  
  int init();
  
  void setPixelColor(uint16_t i, uint8_t red, uint8_t green, uint8_t blue);

  void show();

  int numPixels() const { return num_leds_; }

 private:
  int num_leds_;
  int pin_;
  LedType type_;
};

#endif  // ESP32
#endif  // ARDUINO_PIXEL_EXTERNAL_ESP_WS2812_ESP_WS2812_H

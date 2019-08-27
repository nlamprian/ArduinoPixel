/*! \file led_strip_esp_ws2812.h
 *  \brief Interface for the WS2812 LEDs with the ESP32 board.
 *  \details All LEDs lights up with the same color.
 *  \author Nick Lamprianidis
 *  \version 2.0.0
 *  \date 2017
 *  \copyright The MIT License (MIT)
 *  \par
 *  Copyright (c) 2017 Nick Lamprianidis
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

#ifndef ARDUINO_PIXEL_LED_STRIP_LED_STRIP_ESP_WS2812_H
#define ARDUINO_PIXEL_LED_STRIP_LED_STRIP_ESP_WS2812_H

#include "external/esp_ws2812/esp_ws2812.h"
#include "led_strip/led_strip_base.h"

namespace arduino_pixel {
namespace led_strip {

class LedStripEspWs2812 : public LedStripBase {
 public:
  LedStripEspWs2812(const int &num_leds, const int &pin, Ws2812::LedType type)
      : strip_(num_leds, pin, type) {}

  virtual void init() override { strip_.init(); }

  virtual void colorize(bool force = false) override {
    if (not mode_->update() and not force) return;
    for (uint16_t idx = 0; idx < strip_.numPixels(); ++idx) {
      const Color &color = mode_->getPixel(idx);
      strip_.setPixelColor(idx, color.red, color.green, color.blue);
    }
    strip_.show();
  }

  virtual int getNumLeds() const { return strip_.numPixels(); }

 protected:
  Ws2812 strip_;
};

}  // namespace led_strip
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_LED_STRIP_LED_STRIP_ESP_WS2812_H

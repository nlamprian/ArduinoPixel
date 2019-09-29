/*! \file led_strip_neopixel_esp32.h
 *  \brief Interface for NeoPixel strips driven by an ESP32 board.
 *  \author Nick Lamprianidis
 *  \version 2.1.0
 *  \date 2019
 *  \copyright The MIT License (MIT)
 *  \par
 *  Copyright (c) 2019 Nick Lamprianidis
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

#ifndef ARDUINO_PIXEL_LED_STRIP_LED_STRIP_NEOPIXEL_ESP32_H
#define ARDUINO_PIXEL_LED_STRIP_LED_STRIP_NEOPIXEL_ESP32_H

#include <Arduino.h>
#include <esp32-hal.h>

#include "led_strip/led_strip_base.h"

namespace arduino_pixel {
namespace led_strip {

class LedStripNeoPixelEsp32 : public LedStripBase {
 public:
  LedStripNeoPixelEsp32(const int &num_pixels, const int &pin)
      : num_pixels_(num_pixels), pin_(pin) {
    num_bits_per_pixel_ = 3 * 8;
    num_bits_ = num_pixels_ * num_bits_per_pixel_;

    buffer_ = new rmt_data_t[num_bits_];
    for (uint32_t index = 0; index < num_bits_; ++index) {
      buffer_[index].level0 = 1;
      buffer_[index].level1 = 0;
    }
  }

  virtual void init() override {
    rmt_sender_ = rmtInit(pin_, true, RMT_MEM_64);
    if (rmt_sender_ == NULL) {
      Serial.println("Failed to initialize RMT sender!");
      return;
    }
    float tick = rmtSetTick(rmt_sender_, 100);  // 0.1us tick
    Serial.printf("RMT tick set to %fus\n", tick / 1000);
  }

  virtual void colorize(bool force = false) override {
    if (rmt_sender_ == NULL) return;
    if (not mode_->update() and not force) return;

    for (uint16_t pixel = 0; pixel < num_pixels_; ++pixel) {
      const Color &color = mode_->getPixel(pixel);
      setPixelColor(pixel, color);
    }

    rmtWrite(rmt_sender_, buffer_, num_bits_);
  }

  virtual int getNumLeds() const { return num_pixels_; }

 protected:
  inline void setPixelColor(uint16_t pixel, const Color &color) {
    uint32_t pixel_index = pixel * num_bits_per_pixel_;
    setColorComponent(pixel_index + 0 * 8, color.green);
    setColorComponent(pixel_index + 1 * 8, color.red);
    setColorComponent(pixel_index + 2 * 8, color.blue);
  }

  inline void setColorComponent(uint32_t index, uint8_t value) {
    for (int16_t bit = 7; bit >= 0; --bit, ++index) {
      if (value & (1<<bit)) {
        buffer_[index].duration0 = 8;
        buffer_[index].duration1 = 4;
      } else {
        buffer_[index].duration0 = 4;
        buffer_[index].duration1 = 8;
      }
    }
  }

  int num_pixels_;
  int num_bits_per_pixel_;
  uint32_t num_bits_;
  int pin_;
  rmt_data_t *buffer_;
  rmt_obj_t *rmt_sender_;
};

}  // namespace led_strip
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_LED_STRIP_LED_STRIP_NEOPIXEL_ESP_H

/*! \file rainbow_cycle.h
 *  \brief Defines the rainbow cycle mode.
 *  \details The rainbow cycles through the LED strip.
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

#ifndef ARDUINO_PIXEL_MODE_RAINBOW_CYCLE_H
#define ARDUINO_PIXEL_MODE_RAINBOW_CYCLE_H

#include "mode/rainbow_base.h"

namespace arduino_pixel {
namespace mode {

class RainbowCycle : public RainbowBase {
 public:
  RainbowCycle(const int& num_leds, const unsigned long& period)
      : RainbowBase(num_leds, period) {}

  virtual ~RainbowCycle() {}

  virtual Mode getModeType() const override { return Mode::RAINBOW_CYCLE; }

  virtual String getMode() const override {
    return toString(Mode::RAINBOW_CYCLE);
  }

 private:
  void setPixels() {
    for (uint16_t idx = 0; idx < num_leds_; ++idx)
      pixels_[idx] = wheel((offset_ + (idx * 256 / num_leds_)) & 255);
    offset_ = (offset_ + 1) % (5 * 256);
  }
};

}  // namespace mode
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_MODE_RAINBOW_CYCLE_H

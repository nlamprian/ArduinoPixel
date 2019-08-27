/*! \file single_color.h
 *  \brief Defines the single color mode.
 *  \details All LEDs light up with the same color.
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

#ifndef ARDUINO_PIXEL_MODE_SINGLE_COLOR_H
#define ARDUINO_PIXEL_MODE_SINGLE_COLOR_H

#include "mode/mode_base.h"

namespace arduino_pixel {
namespace mode {

class SingleColor : public ModeBase {
 public:
  SingleColor(const int& num_leds) : ModeBase(num_leds) {
    setColor(Color{0, 0, 0});
  }

  virtual ~SingleColor() {}

  virtual void init() override {}

  virtual bool update() override { return false; }

  virtual const Color& getPixel(int idx = 0) const override { return color_; }

  virtual const Color& getColor(int idx = 0) const override { return color_; }

  virtual void setColor(const Color& color, int idx = 0) override {
    color_ = color;
  }

  virtual Mode getModeType() const override { return Mode::SINGLE_COLOR; }

  virtual String getMode() const override {
    return toString(Mode::SINGLE_COLOR);
  }

 private:
  Color color_;  // Holds the active color of the LED strip
};

}  // namespace mode
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_MODE_SINGLE_COLOR_H

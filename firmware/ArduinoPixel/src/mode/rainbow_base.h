/*! \file rainbow_base.h
 *  \brief Defines the base of the rainbow modes.
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

#ifndef ARDUINO_PIXEL_MODE_RAINBOW_BASE_H
#define ARDUINO_PIXEL_MODE_RAINBOW_BASE_H

#include "mode/mode_base.h"

namespace arduino_pixel {
namespace mode {

class RainbowBase : public ModeBase {
 public:
  RainbowBase(const int& num_leds, const unsigned long& period)
      : ModeBase(num_leds),
        color_(0, 0, 0),
        alpha_(0.5f),
        period_(period),
        offset_(0) {
    pixels_ = new Color[num_leds_];
    last_update_time_ = millis();
  }

  virtual ~RainbowBase() { delete[] pixels_; }

  virtual void init() override { setPixels(); }

  virtual bool update() override {
    unsigned long current_time = millis();
    if ((unsigned long)(current_time - last_update_time_) < period_) return false;
    setPixels();
    last_update_time_ = current_time;
    return true;
  }

  virtual const Color& getPixel(int idx) const override { return pixels_[idx]; }

  virtual const Color& getColor(int idx = 0) const override { return color_; }

  virtual void setColor(const Color& color, int idx = 0) override {
    color_ = color;
    alpha_ = 0.2989f * color.red + 0.5870f * color.green + 0.1140f * color.blue;
    alpha_ = alpha_ / 255.f;
  }

  virtual Mode getModeType() const override = 0;

  virtual String getMode() const override = 0;

 protected:
  virtual void setPixels() = 0;

  /**
   * \brief Turns the requested value into a color.
   * \details The colors are the cycle r - g - b.
   * \note The code is borrowsed from the strandtest example of the
   * Adafruit_NeoPixel library.
   * \param[in] pos a value.
   * \return A color.
   */
  Color wheel(byte pos) {
    pos = 255 - pos;
    if (pos < 85) {
      return Color(255 - pos * 3, 0, pos * 3, alpha_);
    } else if (pos < 170) {
      pos -= 85;
      return Color(0, pos * 3, 255 - pos * 3, alpha_);
    } else {
      pos -= 170;
      return Color(pos * 3, 255 - pos * 3, 0, alpha_);
    }
  }

  Color color_;
  float alpha_;           // Defines the brightness ([0.0,1.0]) of the rainbow
  unsigned long period_;  // The period at which the rainbow moves

  Color* pixels_;

  unsigned long last_update_time_;
  uint16_t offset_;
};

}  // namespace mode
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_MODE_RAINBOW_BASE_H

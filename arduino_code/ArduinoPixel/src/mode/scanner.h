/*! \file scanner.h
 *  \brief Defines the scanner mode.
 *  \details A light-bullet cycles through the entire strip.
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

#ifndef ARDUINO_PIXEL_MODE_SCANNER_H
#define ARDUINO_PIXEL_MODE_SCANNER_H

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#include "mode/mode_base.h"

namespace arduino_pixel {
namespace mode {

class Scanner : public ModeBase {
 public:
  Scanner(const int& num_leds, const unsigned long& period)
      : ModeBase(num_leds), color_(0, 0, 0), period_(period) {
    length_ = min(16u, (unsigned int)(num_leds_ / 2));
    pixels_ = new Color[num_leds_];
  }

  virtual ~Scanner() { delete[] pixels_; }

  virtual void init() override {
    start_idx_ = 0;
    end_idx_ = length_ - 1;

    for (int i = 0; i <= end_idx_; ++i) turnPixelOn(i);
    for (int i = end_idx_ + 1; i < num_leds_; ++i) turnPixelOff(i);

    last_update_time_ = millis();
  }

  virtual bool update() override {
    unsigned long current_time = millis();
    if ((unsigned long)(current_time - last_update_time_) < period_) return false;

    turnPixelOff(start_idx_);
    start_idx_ = (start_idx_ + 1) % num_leds_;
    end_idx_ = (end_idx_ + 1) % num_leds_;
    turnPixelOn(end_idx_);

    last_update_time_ = current_time;
    return true;
  }

  virtual const Color& getPixel(int idx) const override { return pixels_[idx]; }

  virtual const Color& getColor(int idx = 0) const override { return color_; }

  virtual void setColor(const Color& color, int idx = 0) override {
    color_ = color;
  }

  virtual Mode getModeType() const override { return Mode::SCANNER; }

  virtual String getMode() const override { return toString(Mode::SCANNER); }

 private:
  void turnPixelOff(int idx) {
    static Color color(0, 0, 0);
    pixels_[idx] = color;
  }

  void turnPixelOn(int idx) { pixels_[idx] = color_; }

  Color color_;           // Holds the active color of the scanner
  byte length_;           // The length of the scanner
  unsigned long period_;  // The period at which the scanner moves

  Color* pixels_;

  boolean inited_;
  unsigned long last_update_time_;
  int start_idx_, end_idx_;
};

}  // namespace mode
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_MODE_SCANNER_H

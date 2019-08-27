/*! \file common_types.h
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

#ifndef ARDUINO_PIXEL_COMMON_TYPES_H
#define ARDUINO_PIXEL_COMMON_TYPES_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#elif defined(ARDUINO)  // Pre 1.0
#include <Arduino.h>
#elif defined(ESP_PLATFORM)
#include <arduinoish.hpp>
#endif

namespace arduino_pixel {

enum class Mode : byte {
  INVALID,
  SINGLE_COLOR,
  SCANNER,
  RAINBOW,
  RAINBOW_CYCLE
};

inline String toString(Mode mode) {
  switch (mode) {
    case Mode::SINGLE_COLOR:
      return String("SINGLE_COLOR");
    case Mode::SCANNER:
      return String("SCANNER");
    case Mode::RAINBOW:
      return String("RAINBOW");
    case Mode::RAINBOW_CYCLE:
      return String("RAINBOW_CYCLE");
    default:
      return String("INVALID");
  }
}

struct Color {
  Color() {}

  Color(byte red, byte green, byte blue) : red(red), green(green), blue(blue) {}

  Color(byte red, byte green, byte blue, float alpha) {
    this->red = (byte)((float)red * alpha);
    this->green = (byte)((float)green * alpha);
    this->blue = (byte)((float)blue * alpha);
  }

  byte red;
  byte green;
  byte blue;
};

}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_COMMON_TYPES_H

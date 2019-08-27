/*! \file led_strip_base.h
 *  \brief Defines the LED strip base.
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

#ifndef ARDUINO_PIXEL_LED_STRIP_LED_STRIP_BASE_H
#define ARDUINO_PIXEL_LED_STRIP_LED_STRIP_BASE_H

#include "mode/mode_base.h"
#include "common_types.h"

namespace arduino_pixel {
namespace led_strip {

class LedStripBase {
 public:
  virtual ~LedStripBase() {}
  /**
   * \brief Initializes the LED strip.
   * \note Use to initialize any member variables when appropriate.
   */
  virtual void init() {}
  /**
   * \brief Sets the mode.
   * \note The mode will be used to retrieve the colors of the pixels.
   * \param[in] mode a mode instance.
   */
  void setMode(mode::ModeBase *mode) { mode_ = mode; }

  /**
   * \brief Updates the LED strip.
   * \details Gets the colors from the mode and updates the LED strip.
   * \param[in] force Force updating the strip.
   */
  virtual void colorize(bool force = false) = 0;
  /**
   * \brief Gets the number of LEDs on the strip.
   * \return The number of LEDs.
   */
  virtual int getNumLeds() const = 0;

 protected:
  LedStripBase() {}
  LedStripBase(mode::ModeBase *mode) : mode_(mode) {}

  mode::ModeBase *mode_;
};

}  // namespace mode
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_LED_STRIP_LED_STRIP_BASE_H

/*! \file mode_base.h
 *  \brief Defines the mode base.
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

#ifndef ARDUINO_PIXEL_MODE_BASE_H
#define ARDUINO_PIXEL_MODE_BASE_H

#include "common_types.h"

namespace arduino_pixel {
namespace mode {

class ModeBase {
 public:
  virtual ~ModeBase() {}
  /**
   * \brief Initializes the mode.
   * \note Use to initialize any member variables when appropriate.
   * E.g. Initialize the LED array after the base color has been set.
   */
  virtual void init() = 0;
  /**
   * \brief Updates the colors on the LED array.
   * \note If you think the mode as a video (sequence of images), 
   * this is where you fill the LED array with the next image.
   * \return Flag to indicate whether the LED array has been updated.
   */
  virtual bool update() = 0;
  /**
   * \brief Gets the color of the request LED (aka pixel).
   * \param[in] idx the index of the pixel in the array.
   * \return The color of the pixel.
   */
  virtual const Color& getPixel(int idx = 0) const = 0;
  /**
   * \brief Gets the requested color.
   * \param[in] idx the index of the color in the color array.
   * \return The requested color.
   */
  virtual const Color& getColor(int idx = 0) const = 0;
  /**
   * \brief Sets the color #idx.
   * \note If the mode consists of some base colors for its operation, 
   * this is where you set those colors.
   * \param[in] color the color.
   * \param[in] idx the index of the color.
   */
  virtual void setColor(const Color& color, int idx = 0) = 0;
  /**
   * \brief Gets the name of the mode.
   * \return The mode as a C++ type.
   */
  virtual Mode getModeType() const = 0;
  /**
   * \brief The name of the mode.
   * \return The mode as a string.
   */
  virtual String getMode() const = 0;

 protected:
  ModeBase(const int& num_leds) : num_leds_(num_leds) {}

  const int num_leds_;
};

}  // namespace mode
}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_MODE_BASE_H

/*! \file server_types.h
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

#ifndef ARDUINO_PIXEL_SERVER_TYPES_H
#define ARDUINO_PIXEL_SERVER_TYPES_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#elif defined(ARDUINO)  // Pre 1.0
#include <Arduino.h>
#elif defined(ESP_PLATFORM)
#include <arduinoish.hpp>
#endif

namespace arduino_pixel {

enum class HttpMethod : byte { INVALID, GET, PUT };

inline String toString(HttpMethod method) {
  switch (method) {
    case HttpMethod::GET:
      return String("GET");
    case HttpMethod::PUT:
      return String("PUT");
    default:
      return String("INVALID");
  }
}

enum class Uri : byte {
  INVALID,
  ROOT,        // "/"
  STATUS,      // "/strip/status"
  STATUS_ON,   // "/strip/status/on"
  STATUS_OFF,  // "/strip/status/off"
  MODES,       // "/strip/modes"
  MODE_GET,    // "/strip/mode"
  MODE_PUT,    // "/strip/mode"
  COLOR_GET,   // "/strip/color"
  COLOR_PUT    // "/strip/color"
};

inline String toString(Uri uri) {
  switch (uri) {
    case Uri::ROOT:
      return String("ROOT");
    case Uri::STATUS:
      return String("STATUS");
    case Uri::STATUS_ON:
      return String("STATUS_ON");
    case Uri::STATUS_OFF:
      return String("STATUS_OFF");
    case Uri::MODES:
      return String("MODES");
    case Uri::MODE_GET:
      return String("MODE_GET");
    case Uri::MODE_PUT:
      return String("MODE_PUT");
    case Uri::COLOR_GET:
      return String("COLOR_GET");
    case Uri::COLOR_PUT:
      return String("COLOR_PUT");
    default:
      return String("INVALID");
  }
}

struct RequestData {
  HttpMethod http_method;
  Uri uri;
  String data;
};

struct ResponseData {
  ResponseData() {}
  ResponseData(int status_code, String status_msg, boolean keep_alive,
               String data)
      : status_code(status_code),
        status_msg(status_msg),
        keep_alive(keep_alive),
        data(data) {}
  int status_code;
  String status_msg;
  boolean keep_alive;
  String data;
};

}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_SERVER_TYPES_H

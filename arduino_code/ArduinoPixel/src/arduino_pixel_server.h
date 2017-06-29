/*! \file arduino_pixel_server.h
 *  \brief Declares the server side.
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

#ifndef ARDUINO_PIXEL_ARDUINO_PIXEL_SERVER_H
#define ARDUINO_PIXEL_ARDUINO_PIXEL_SERVER_H

// #define DEBUG

#include <Client.h>

#include "common_types.h"
#include "server_types.h"
#include "led_strip/led_strip_base.h"
#include "modes.h"

namespace arduino_pixel {

class ArduinoPixelServer {
 public:
  ArduinoPixelServer();
  virtual ~ArduinoPixelServer();
  /**
   * \brief Handles an http request.
   * \details Retrieves the http request, parses it, updates the LED strip as
   * necessary, and responds to the client.
   * \param[in] client client that has the http request.
   */
  virtual void processRequest(Client &client);
  /**
   * \brief Updates the colors on the LED strip.
   */
  virtual void colorize();

 protected:
  /**
   * \brief Initializes the pointer to the controlled LED strip.
   * \param[in] strip LED strip instance.
   */
  void init(led_strip::LedStripBase *strip);
  /**
   * \brief Powers the LED strip on.
   */
  void powerOn();
  /**
   * \brief Powers the LED strip off.
   */
  void powerOff();

  /**
   * \brief Parses an http request.
   * \details Retrieves the http request and extracts the http method, the uri,
   * and the request data.
   * \param[in] client client that has the http request.
   */
  RequestData parseRequest(Client &client) const;
  /**
   * \brief Extracts the request lines.
   * \param[in] client client that has the http request.
   * \return The request line.
   */
  String getRequestLine(Client &client) const;
  /**
   * \brief Parses a http method.
   * \param[in] request_line a request line.
   * \return The http method.
   */
  HttpMethod parseHttpMethod(const String &request_line) const;
  /**
   * \brief Parses a uri.
   * \param[in] method a http method.
   * \param[in] request_line a request line.
   * \return The uri.
   */
  Uri parseUri(HttpMethod method, const String &request_line) const;
  /**
   * \brief Extracts the request data.
   * \param[in] client client that has the http request.
   * \return The request data.
   */
  String getRequestData(Client &client) const;

  /**
   * \brief Constructs the response based on a request.
   * \param[in] request a http request.
   * \return The http response.
   */
  ResponseData getResponse(RequestData &request) const;
  /**
   * \brief Constructs the response based on a get request.
   * \param[in] request a http request.
   * \return The http response.
   */
  ResponseData getGetResponse(RequestData &request) const;
  /**
   * \brief Constructs the response based on a put request.
   * \param[in] request a http request.
   * \return The http response.
   */
  ResponseData getPutResponse(RequestData &request) const;
  /**
   * \brief Sends a response to a client.
   * \param[in] client client that made the http request.
   * \param[in] response the response.
   */
  void sendResponse(Client &client, ResponseData &response) const;

  /**
   * \brief Gets a sequence of the available modes.
   * \return A comma separated list of the available modes.
   */
  String getModes() const;
  /**
   * \brief Retrieves the base color of the active mode.
   * \return A json representation of the active color.
   */
  String getColor() const;
  /**
   * \brief Updates the LED strip based on a request.
   * \param[in] request a http request.
   */
  void updateStrip(const RequestData &request);
  /**
   * \brief Updates the mode and the LED strip.
   * \param[in]
   */
  void updateMode(const String &mode);
  /**
   * \brief Parses the requested color and updates the LED strip.
   * \param[in] json the color in json format.
   */
  void updateColor(const String &json);

  boolean power_;  // Flag that indicates whether the LED strip is on or off

  led_strip::LedStripBase *strip_;

  mode::ModeBase *mode_;  // Active mode
  mode::SingleColor *mode_off_;  // Mode that turns off the LED strip
};

}  // namespace arduino_pixel

#endif  // ARDUINO_PIXEL_ARDUINO_PIXEL_SERVER_H

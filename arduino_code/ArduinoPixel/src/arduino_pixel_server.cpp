/*! \file arduino_pixel_server.cpp
 *  \brief Implements the server side and interfaces with the LED strip.
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

#include "arduino_pixel_server.h"

namespace arduino_pixel {

ArduinoPixelServer::ArduinoPixelServer()
    : power_(false), strip_(nullptr), mode_(nullptr), mode_off_(nullptr) {}

ArduinoPixelServer::~ArduinoPixelServer() {
  if (mode_) delete mode_;
  if (mode_off_) delete mode_off_;
}

void ArduinoPixelServer::processRequest(Client &client) {
  // Print the entire request and send default response
  // while(client.available()) Serial.print((char)client.read());
  // ResponseData res(200, "OK", false, "");
  // sendResponse(client,res);
  // return;

  RequestData request = parseRequest(client);
  updateStrip(request);
  ResponseData response = getResponse(request);
  sendResponse(client, response);
}

void ArduinoPixelServer::colorize() { strip_->colorize(); }

void ArduinoPixelServer::init(led_strip::LedStripBase *strip) {
  strip_ = strip;
  mode_ = new mode::SingleColor(strip_->getNumLeds());
  mode_->setColor(Color(128, 128, 128));
  mode_off_ = new mode::SingleColor(strip_->getNumLeds());
  mode_off_->setColor(Color(0, 0, 0));
  powerOff();
}

void ArduinoPixelServer::powerOn() {
  power_ = true;
  strip_->setMode(mode_);
  strip_->colorize();
}

void ArduinoPixelServer::powerOff() {
  power_ = false;
  strip_->setMode(mode_off_);
  strip_->colorize();
}

RequestData ArduinoPixelServer::parseRequest(Client &client) const {
  RequestData request;
  String request_line = getRequestLine(client);
  request.http_method = parseHttpMethod(request_line);
  request.uri = parseUri(request.http_method, request_line);
  request.data = getRequestData(client);

#ifdef DEBUG
  Serial.print("HTTP Method: ");
  Serial.println(toString(request.http_method));
  Serial.print("URI: ");
  Serial.println(toString(request.uri));
  Serial.print("Data: ");
  Serial.println(request.data);
  Serial.println();
#endif
  return request;
}

String ArduinoPixelServer::getRequestLine(Client &client) const {
  String request_line;
  char req_char;
  while (client.available()) {
    if ((req_char = client.read()) == '\r') {
      client.read();  // Get rid of '\n'
      break;
    } else {
      request_line += req_char;
    }
  }

#ifdef DEBUG
  Serial.print("Request Line: ");
  Serial.println(request_line);
#endif
  return request_line;
}

HttpMethod ArduinoPixelServer::parseHttpMethod(
    const String &request_line) const {
  String method = request_line.substring(0, request_line.indexOf(' '));
  if (method.equals("GET"))
    return HttpMethod::GET;
  else if (method.equals("PUT"))
    return HttpMethod::PUT;
  else
    return HttpMethod::INVALID;
}

Uri ArduinoPixelServer::parseUri(HttpMethod method,
                                 const String &request_line) const {
  String uri = request_line.substring(request_line.indexOf(' ') + 1,
                                      request_line.lastIndexOf(' '));
  if (uri.equals("/"))
    return Uri::ROOT;
  else if (uri.startsWith("/strip/status/on"))
    return Uri::STATUS_ON;
  else if (uri.startsWith("/strip/status/off"))
    return Uri::STATUS_OFF;
  else if (uri.startsWith("/strip/status"))
    return Uri::STATUS;
  else if (uri.startsWith("/strip/modes"))
    return Uri::MODES;
  else if (uri.startsWith("/strip/mode"))
    return (method == HttpMethod::GET) ? Uri::MODE_GET : Uri::MODE_PUT;
  else if (uri.startsWith("/strip/color"))
    return (method == HttpMethod::GET) ? Uri::COLOR_GET : Uri::COLOR_PUT;
  else
    return Uri::INVALID;
}

String ArduinoPixelServer::getRequestData(Client &client) const {
  String tmp;
  while (client.available()) tmp += (char)client.read();
  int idx = tmp.indexOf("\r\n\r\n");  // The data begin after an empty line
  return (idx > 0) ? tmp.substring(idx + 4) : String();
}

ResponseData ArduinoPixelServer::getResponse(RequestData &request) const {
  switch (request.http_method) {
    case HttpMethod::GET:
      return getGetResponse(request);
    case HttpMethod::PUT:
      return getPutResponse(request);
    default:
      return ResponseData(404, "Not Found", false, "");
  }
}

ResponseData ArduinoPixelServer::getGetResponse(RequestData &request) const {
  switch (request.uri) {
    case Uri::ROOT:
      return ResponseData(200, "OK", false, "Hello from Arduino Server");
    case Uri::STATUS:
      return ResponseData(200, "OK", false, power_ ? "ON" : "OFF");
    case Uri::MODES:
      return ResponseData(200, "OK", false, getModes());
    case Uri::MODE_GET:
      return ResponseData(200, "OK", false, mode_->getMode());
    case Uri::COLOR_GET:
      return ResponseData(200, "OK", false, getColor());
    default:
      return ResponseData(404, "Not Found", false, "");
  }
}

ResponseData ArduinoPixelServer::getPutResponse(RequestData &request) const {
  switch (request.uri) {
    case Uri::STATUS_ON:
      return ResponseData(200, "OK", false, "");
    case Uri::STATUS_OFF:
      return ResponseData(200, "OK", false, "");
    case Uri::MODE_PUT:
      return ResponseData(200, "OK", false, "");
    case Uri::COLOR_PUT:
      return ResponseData(200, "OK", true, "");
    default:
      return ResponseData(404, "Not Found", false, "");
  }
}

void ArduinoPixelServer::sendResponse(Client &client,
                                      ResponseData &response) const {
#ifdef DEBUG
  Serial.print("HTTP/1.1 ");
  Serial.print(response.status_code);
  Serial.print(" ");
  Serial.println(response.status_msg);
  if (!response.keep_alive) Serial.println("Connection: close");
  Serial.println();
  if (response.data.length()) {
    Serial.println(response.data);
    Serial.println();
  }
  Serial.println("==========");
  Serial.println();
#endif

  client.print("HTTP/1.1 ");
  client.print(response.status_code);
  client.print(" ");
  client.println(response.status_msg);
  if (!response.keep_alive) client.println("Connection: close");
  if (response.data.length()) {
    client.println();
    client.print(response.data);
  }
  delay(1);
  client.stop();
}

String ArduinoPixelServer::getModes() const {
  String modes;
  switch (Mode::INVALID) {
    case Mode::INVALID:
    case Mode::SINGLE_COLOR:
      modes += toString(Mode::SINGLE_COLOR);
    case Mode::SCANNER:
      modes += ",";
      modes += toString(Mode::SCANNER);
    case Mode::RAINBOW:
      modes += ",";
      modes += toString(Mode::RAINBOW);
    case Mode::RAINBOW_CYCLE:
      modes += ",";
      modes += toString(Mode::RAINBOW_CYCLE);
  }
  return modes;
}

String ArduinoPixelServer::getColor() const {
  const Color &color = mode_->getColor();
  return "{\"r\":" + String(color.red) + ",\"g\":" + String(color.green) +
         ",\"b\":" + String(color.blue) + "}";
}

void ArduinoPixelServer::updateStrip(const RequestData &request) {
  if (request.http_method == HttpMethod::GET) return;
  switch (request.uri) {
    case Uri::STATUS_ON:  // Turn the LED strip on
      powerOn();
      break;
    case Uri::STATUS_OFF:  // Turn the LED strip off
      powerOff();
      break;
    case Uri::MODE_PUT:  // Update mode
      updateMode(request.data);
      strip_->colorize();
      break;
    case Uri::COLOR_PUT:  // Update the LED strip color
      updateColor(request.data);
      strip_->colorize();
      break;
  }
}

void ArduinoPixelServer::updateMode(const String &data) {
  mode::ModeBase *mode;
  if (data.indexOf("SINGLE_COLOR") > 0) {
    mode = new mode::SingleColor(strip_->getNumLeds());
  } else if (data.indexOf("SCANNER") > 0) {
    unsigned long period = data.substring(data.lastIndexOf(' ') + 1).toInt();
    if (period == 0) period = 100ul;
    mode = new mode::Scanner(strip_->getNumLeds(), period);
  } else if (data.indexOf("RAINBOW_CYCLE") > 0) {
    unsigned long period = data.substring(data.lastIndexOf(' ') + 1).toInt();
    if (period == 0) period = 10ul;
    mode = new mode::RainbowCycle(strip_->getNumLeds(), period);
  } else if (data.indexOf("RAINBOW") > 0) {
    unsigned long period = data.substring(data.lastIndexOf(' ') + 1).toInt();
    if (period == 0) period = 10ul;
    mode = new mode::Rainbow(strip_->getNumLeds(), period);
  } else {
    return;
  }

  mode->setColor(mode_->getColor());
  mode->init();
  if (mode_) delete mode_;
  mode_ = mode;
  if (power_) strip_->setMode(mode_);
}

void ArduinoPixelServer::updateColor(const String &json) {
  byte idx = 0;
  byte color[3];
  for (byte i = 0; i < 3; ++i) {
    while (json[idx++] != ':')
      ;
    byte start_idx = idx;
    while (json[idx] != ',' && json[idx] != '}') idx++;
    byte end_idx = idx;
    String color_str = json.substring(start_idx, end_idx);
    color[i] = (byte)color_str.toInt();
  }
  mode_->setColor(Color(color[0], color[1], color[2]));
}

}  // namespace arduino_pixel

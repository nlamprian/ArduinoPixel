/*! \file esp_ws2812_rmt.h
 *  \brief Declarations of functions for interfacing with the RMT peripheral on
 * ESP32.
 *  \note Based on public domain code created on 19 Nov 2016 by Chris Osborn
 * <fozztexx@fozztexx.com> http://insentricity.com
 *  \author Martin F. Falatic
 *  \date 2017
 *  \copyright The MIT License (MIT)
 *  \par
 *  Copyright (c) 2017 Martin F. Falatic
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

#ifndef ARDUINO_PIXEL_EXTERNAL_ESP_WS2812_ESP_WS2812_RMT_H
#ifdef ESP32
#define ARDUINO_PIXEL_EXTERNAL_ESP_WS2812_ESP_WS2812_RMT_H

// #define DEBUG_WS2812_DRIVER

#include <stdint.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#elif defined(ARDUINO)  // Pre 1.0
#include <Arduino.h>
#elif defined(ESP_PLATFORM)
#include <arduinoish.hpp>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARDUINO)
#include <esp32-hal.h>
#include <esp_intr.h>
#include <driver/gpio.h>
#include <driver/periph_ctrl.h>
#include <freertos/semphr.h>
#include <soc/rmt_struct.h>
#elif defined(ESP_PLATFORM)
#include <esp_intr.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <soc/dport_reg.h>
#include <soc/gpio_sig_map.h>
#include <soc/rmt_struct.h>
#include <stdio.h>
#endif

#ifdef __cplusplus
}
#endif

#define RMTCHANNEL 0   // There are 8 possible channels
#define DIVIDER 4      // 8 stil seems to work, but timings become marginal
#define MAX_PULSES 32  // A channel has a 64 pulse buffer - we use half per pass
#define RMT_DURATION_NS \
  12.5  // Minimum time of a single RMT duration based on clock ns

#ifdef DEBUG_WS2812_DRIVER
extern char *debug_buffer;
extern const int debug_buffer_size;
#endif

typedef struct {
  uint32_t T0H;
  uint32_t T1H;
  uint32_t T0L;
  uint32_t T1L;
  uint32_t TRS;
} TimingParams;

typedef union {
  struct {
    uint32_t duration0 : 15;
    uint32_t level0 : 1;
    uint32_t duration1 : 15;
    uint32_t level1 : 1;
  };
  uint32_t val;
} RmtPulsePair;

void initRMTChannel(int rmtChannel);
void copyToRmtBlock_half();
void handleInterrupt(void *arg);
void dumpDebugBuffer(int id);

#endif  // ESP32
#endif  // ARDUINO_PIXEL_EXTERNAL_ESP_WS2812_ESP_WS2812_RMT_H

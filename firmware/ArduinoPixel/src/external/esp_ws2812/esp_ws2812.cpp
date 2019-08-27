/*! \file esp_ws2812.cpp
 *  \brief Class definition of driver for the WS2812 RGB LEDs using the RMT
 * peripheral on ESP32.
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

#ifdef ESP32

#include "external/esp_ws2812/esp_ws2812.h"

TimingParams params;
xSemaphoreHandle ws2812_sem = NULL;
uint8_t *ws2812_buffer = NULL;

extern RmtPulsePair ws2812_bitval_to_rmt_map[2];
extern uint16_t ws2812_pos, ws2812_len, ws2812_half;

static intr_handle_t rmt_intr_handle = NULL;

Ws2812::Ws2812(int num_leds, int pin, LedType type)
    : num_leds_(num_leds), pin_(pin), type_(type) {}

Ws2812::~Ws2812() { delete[] ws2812_buffer; }

int Ws2812::init() {
  ws2812_len = (3 * num_leds_) * sizeof(uint8_t);
  ws2812_buffer = new uint8_t[ws2812_len];
#ifdef DEBUG_WS2812_DRIVER
  debug_buffer = (char *)calloc(debug_buffer_size, sizeof(char));
#endif

  switch (type_) {
    case LedType::WS2812:
      params = TimingParams{
          .T0H = 350, .T1H = 700, .T0L = 800, .T1L = 600, .TRS = 50000};
      break;
    case LedType::WS2812B:
      params = TimingParams{
          .T0H = 350, .T1H = 900, .T0L = 900, .T1L = 350, .TRS = 50000};
      break;
    case LedType::SK6812:
      params = TimingParams{
          .T0H = 300, .T1H = 600, .T0L = 900, .T1L = 600, .TRS = 80000};
      break;
    case LedType::WS2813:
      params = TimingParams{
          .T0H = 350, .T1H = 800, .T0L = 350, .T1L = 350, .TRS = 300000};
      break;
    default:
      return 1;
  }

  DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_RMT_CLK_EN);
  DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST);

  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin_], 2);
  gpio_matrix_out((gpio_num_t)pin_, RMT_SIG_OUT0_IDX + RMTCHANNEL, 0, 0);
  gpio_set_direction((gpio_num_t)pin_, GPIO_MODE_OUTPUT);

  initRMTChannel(RMTCHANNEL);

  RMT.tx_lim_ch[RMTCHANNEL].limit = MAX_PULSES;
  RMT.int_ena.ch0_tx_thr_event = 1;
  RMT.int_ena.ch0_tx_end = 1;

  // RMT config for WS2812 bit val 0
  ws2812_bitval_to_rmt_map[0].level0 = 1;
  ws2812_bitval_to_rmt_map[0].level1 = 0;
  ws2812_bitval_to_rmt_map[0].duration0 =
      params.T0H / (RMT_DURATION_NS * DIVIDER);
  ws2812_bitval_to_rmt_map[0].duration1 =
      params.T0L / (RMT_DURATION_NS * DIVIDER);

  // RMT config for WS2812 bit val 1
  ws2812_bitval_to_rmt_map[1].level0 = 1;
  ws2812_bitval_to_rmt_map[1].level1 = 0;
  ws2812_bitval_to_rmt_map[1].duration0 =
      params.T1H / (RMT_DURATION_NS * DIVIDER);
  ws2812_bitval_to_rmt_map[1].duration1 =
      params.T1L / (RMT_DURATION_NS * DIVIDER);

  esp_intr_alloc(ETS_RMT_INTR_SOURCE, 0, handleInterrupt, NULL,
                 &rmt_intr_handle);

  return 0;
}

void Ws2812::setPixelColor(uint16_t i, uint8_t red, uint8_t green,
                           uint8_t blue) {
  // Color order is mapped from RGB to GRB for WS2812
  uint16_t offset = 3 * i;
  ws2812_buffer[offset + 0] = green;
  ws2812_buffer[offset + 1] = red;
  ws2812_buffer[offset + 2] = blue;
}

void Ws2812::show() {
  ws2812_pos = 0;
  ws2812_half = 0;

  copyToRmtBlock_half();

  if (ws2812_pos < ws2812_len) {
#ifdef DEBUG_WS2812_DRIVER
    snprintf(debug_buffer, debug_buffer_size, "%s# ", debug_buffer);
#endif
    copyToRmtBlock_half();  // Fill the other half of the buffer block
  }

  ws2812_sem = xSemaphoreCreateBinary();

  RMT.conf_ch[RMTCHANNEL].conf1.mem_rd_rst = 1;
  RMT.conf_ch[RMTCHANNEL].conf1.tx_start = 1;

  xSemaphoreTake(ws2812_sem, portMAX_DELAY);
  vSemaphoreDelete(ws2812_sem);
  ws2812_sem = NULL;
}

#endif  // ESP32

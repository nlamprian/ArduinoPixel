/*! \file esp_ws2812_rmt.cpp
 *  \brief Definitions of functions for interfacing with the RMT peripheral on
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

#ifdef ESP32

#include "external/esp_ws2812/esp_ws2812_rmt.h"

#ifdef DEBUG_WS2812_DRIVER
char *debug_buffer = NULL;
const int debug_buffer_size = 1024;
#endif

#define RMTCHANNEL 0   // There are 8 possible channels
#define DIVIDER 4      // 8 stil seems to work, but timings become marginal
#define MAX_PULSES 32  // A channel has a 64 pulse buffer - we use half per pass
#define RMT_DURATION_NS \
  12.5  // Minimum time of a single RMT duration based on clock ns

RmtPulsePair ws2812_bitval_to_rmt_map[2];
uint16_t ws2812_pos, ws2812_len, ws2812_half;

extern TimingParams params;
extern xSemaphoreHandle ws2812_sem;
extern uint8_t *ws2812_buffer;

static uint16_t ws2812_buf_is_dirty;

void initRMTChannel(int rmtChannel) {
  RMT.apb_conf.fifo_mask = 1;  // Enable memory access, instead of FIFO mode
  RMT.apb_conf.mem_tx_wrap_en = 1;  // Wrap around when hitting end of buffer
  RMT.conf_ch[rmtChannel].conf0.div_cnt = DIVIDER;
  RMT.conf_ch[rmtChannel].conf0.mem_size = 1;
  RMT.conf_ch[rmtChannel].conf0.carrier_en = 0;
  RMT.conf_ch[rmtChannel].conf0.carrier_out_lv = 1;
  RMT.conf_ch[rmtChannel].conf0.mem_pd = 0;

  RMT.conf_ch[rmtChannel].conf1.rx_en = 0;
  RMT.conf_ch[rmtChannel].conf1.mem_owner = 0;
  RMT.conf_ch[rmtChannel].conf1.tx_conti_mode = 0;  // Loop back mode
  RMT.conf_ch[rmtChannel].conf1.ref_always_on = 1;  // Use apb clock ( 80M)
  RMT.conf_ch[rmtChannel].conf1.idle_out_en = 1;
  RMT.conf_ch[rmtChannel].conf1.idle_out_lv = 0;
}

void copyToRmtBlock_half() {
  // This fills half an RMT block
  // When wraparound happens, we want to keep
  // the inactive half of the RMT block filled

  uint16_t offset = ws2812_half * MAX_PULSES;
  ws2812_half = !ws2812_half;

  uint16_t len = ws2812_len - ws2812_pos;
  if (len > (MAX_PULSES / 8)) len = (MAX_PULSES / 8);

  if (!len) {
    if (!ws2812_buf_is_dirty) return;
    // Clear the channel's data block and return
    for (uint16_t i = 0; i < MAX_PULSES; ++i)
      RMTMEM.chan[RMTCHANNEL].data32[i + offset].val = 0;
    ws2812_buf_is_dirty = 0;
    return;
  }
  ws2812_buf_is_dirty = 1;

  uint16_t i;
  for (i = 0; i < len; ++i) {
    uint16_t byteval = ws2812_buffer[i + ws2812_pos];

#ifdef DEBUG_WS2812_DRIVER
    snprintf(debug_buffer, debug_buffer_size, "%s%d(", debug_buffer, byteval);
#endif

    // Shift bits out, MSB first, setting RMTMEM.chan[n].data32[x] to the
    // RmtPulsePair value corresponding to the buffered bit value
    for (uint16_t j = 0; j < 8; ++j, byteval <<= 1) {
      int bitval = (byteval >> 7) & 0x01;
      int data32_idx = i * 8 + offset + j;
      RMTMEM.chan[RMTCHANNEL].data32[data32_idx].val =
          ws2812_bitval_to_rmt_map[bitval].val;
#ifdef DEBUG_WS2812_DRIVER
      snprintf(debug_buffer, debug_buffer_size, "%s%d", debug_buffer, bitval);
#endif
    }

#ifdef DEBUG_WS2812_DRIVER
    snprintf(debug_buffer, debug_buffer_size, "%s) ", debug_buffer);
#endif

    // Handle the reset bit by stretching duration1 for the final bit in the
    // stream
    if (i + ws2812_pos == ws2812_len - 1) {
      RMTMEM.chan[RMTCHANNEL].data32[i * 8 + offset + 7].duration1 =
          params.TRS / (RMT_DURATION_NS * DIVIDER);

#ifdef DEBUG_WS2812_DRIVER
      snprintf(debug_buffer, debug_buffer_size, "%sRESET ", debug_buffer);
#endif
    }
  }

  // Clear the remainder of the channel's data not set above
  for (i *= 8; i < MAX_PULSES; ++i)
    RMTMEM.chan[RMTCHANNEL].data32[i + offset].val = 0;

  ws2812_pos += len;

#ifdef DEBUG_WS2812_DRIVER
  snprintf(debug_buffer, debug_buffer_size, "%s ", debug_buffer);
#endif
}

void handleInterrupt(void *arg) {
  portBASE_TYPE task_awoken = 0;
  if (RMT.int_st.ch0_tx_thr_event) {
    copyToRmtBlock_half();
    RMT.int_clr.ch0_tx_thr_event = 1;
  } else if (RMT.int_st.ch0_tx_end && ws2812_sem) {
    xSemaphoreGiveFromISR(ws2812_sem, &task_awoken);
    RMT.int_clr.ch0_tx_end = 1;
  }
}

void dumpDebugBuffer(int id) {
#ifdef DEBUG_WS2812_DRIVER
  Serial.print("DEBUG: (");
  Serial.print(id);
  Serial.print(") ");
  Serial.println(debug_buffer);
  debug_buffer[0] = 0;
#endif
}

#endif  // ESP32

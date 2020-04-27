/*******************************************************************************
 *
 * crc.h - Header file for STM8 CRC library functions
 *
 * Copyright (c) 2020 Basil Hussain
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/

#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

// Initial values for the various CRC implementations.
#define crc8_1wire_init() ((uint8_t)0x0)
#define crc16_ansi_init() ((uint16_t)0xFFFF)
#define crc16_ccitt_init() ((uint16_t)0xFFFF)
#define crc16_xmodem_init() ((uint16_t)0x0)
#define crc32_init() ((uint32_t)0x0)

// XMODEM has same implementation as CCITT, just with different initial value,
// so just alias it to the latter function.
#define crc16_xmodem_update crc16_ccitt_update

extern uint8_t crc8_1wire_update(uint8_t crc, uint8_t data);
extern uint16_t crc16_ansi_update(uint16_t crc, uint8_t data);
extern uint16_t crc16_ccitt_update(uint16_t crc, uint8_t data);
extern uint32_t crc32_update(uint32_t crc, uint8_t data);

#endif // CRC_H_

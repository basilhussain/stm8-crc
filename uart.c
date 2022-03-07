/*******************************************************************************
 *
 * uart.c - Basic UART implementation functions to facilitate stdio
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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "uart_regs.h"

/******************************************************************************/

typedef struct {
	uint8_t brr1;
	uint8_t brr2;
} uart_baud_cfg_t;

#ifndef F_CPU
#error "F_CPU is not defined"
#endif

const static uart_baud_cfg_t uart_baud_cfg[] = {
#if defined(F_CPU) && F_CPU == 16000000UL
	{ .brr1 = 0xA0, .brr2 = 0x1B }, // 2400
	{ .brr1 = 0x68, .brr2 = 0x03 }, // 9600
	{ .brr1 = 0x34, .brr2 = 0x01 }, // 19200
	{ .brr1 = 0x11, .brr2 = 0x06 }, // 57600
	{ .brr1 = 0x08, .brr2 = 0x0B }, // 115200
	{ .brr1 = 0x04, .brr2 = 0x05 }, // 230400
	{ .brr1 = 0x02, .brr2 = 0x03 }, // 460800
	{ .brr1 = 0x01, .brr2 = 0x01 }  // 921600
#else
#error "No UART BRR register values defined for current F_CPU value"
#endif
};

static uart_putchar_func_t uart_putchar_func = NULL;
static uart_getchar_func_t uart_getchar_func = NULL;

/******************************************************************************/

int putchar(int c) {
	if(uart_putchar_func != NULL) {
		return uart_putchar_func(c);
	} else {
		return c;
	}
}

int getchar(void) {
	if(uart_getchar_func != NULL) {
		return uart_getchar_func();
	} else {
		return EOF;
	}
}

void uart_init(const uart_baud_enum_t baud, uart_putchar_func_t put_func, uart_getchar_func_t get_func) {
	// Configure BRR for specified baud rate. BRR2 must be set first.
	UART1_BRR2 = uart_baud_cfg[baud].brr2;
	UART1_BRR1 = uart_baud_cfg[baud].brr1;

	UART1_CR1 = 0; // Default of UART enabled, 8 data bits, no parity
	UART1_CR3 = 0; // Default of 1 stop bit
	UART1_CR4 = 0;
	UART1_CR5 = 0;
	UART1_CR2 = (1U << UART1_CR2_REN) | (1U << UART1_CR2_TEN); // Enable RX & TX.

	uart_putchar_func = put_func;
	uart_getchar_func = get_func;
}

int uart_putchar(int c) {
	// When binary mode is not set and character to transmit is LF, send a CR
	// preceding it.
	// if(c == '\n') putchar('\r');

	// Wait until transmit data register is empty, then write the byte to be
	// transmitted to it.
	while(!(UART1_SR & (1U << UART1_SR_TXE)));
	UART1_DR = c;

	return c;
}

int uart_getchar(void) {
	// Wait until a character has been received.
	while(!(UART1_SR & (1U << UART1_SR_RXNE)));

	// If there was a framing or overrun error, ignore what was received and
	// return error code; otherwise return the byte received.
	if(UART1_SR & ((1U << UART1_SR_OR) | (1U << UART1_SR_FE))) {
		(void)UART1_DR; // Resets the error flags.
		return EOF;
	} else {
		return UART1_DR;
	}
}

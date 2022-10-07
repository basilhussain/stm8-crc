/*******************************************************************************
 *
 * main.c - Test and benchmarking code for STM8 CRC library functions
 *
 * Copyright (c) 2022 Basil Hussain
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
#include "ucsim.h"
#include "crc.h"
#include "crc_ref.h"

#define CLK_CKDIVR (*(volatile uint8_t *)(0x50C6))

// PC5 is connected to the built-in LED on the STM8S208 Nucleo-64 board.
// Toggling this pin is used for benchmarking.
#define PC_ODR (*(volatile uint8_t *)(0x500A))
#define PC_ODR_ODR5 5
#define PC_DDR (*(volatile uint8_t *)(0x500C))
#define PC_DDR_DDR5 5
#define PC_CR1 (*(volatile uint8_t *)(0x500D))
#define PC_CR1_C15 5

typedef uint8_t (*crc8_update_func_t)(uint8_t crc, uint8_t data) __stack_args;
typedef uint16_t (*crc16_update_func_t)(uint16_t crc, uint8_t data) __stack_args;
typedef uint32_t (*crc32_update_func_t)(uint32_t crc, uint8_t data) __stack_args;

typedef struct {
	uint8_t init_val;
	crc8_update_func_t update_func;
	uint8_t xorout_val;
} crc8_type_t;

typedef struct {
	uint16_t init_val;
	crc16_update_func_t update_func;
	uint16_t xorout_val;
} crc16_type_t;

typedef struct {
	uint32_t init_val;
	crc32_update_func_t update_func;
	uint32_t xorout_val;
} crc32_type_t;

typedef struct {
	char *name;
	uint8_t *data;
	size_t data_len;
	crc8_type_t c_func;
	crc8_type_t asm_func;
	uint8_t expected;
} crc8_test_t;

typedef struct {
	char *name;
	uint8_t *data;
	size_t data_len;
	crc16_type_t c_func;
	crc16_type_t asm_func;
	uint16_t expected;
} crc16_test_t;

typedef struct {
	char *name;
	uint8_t *data;
	size_t data_len;
	crc32_type_t c_func;
	crc32_type_t asm_func;
	uint32_t expected;
} crc32_test_t;

#define benchmark_marker_start() do { PC_ODR |= (1 << PC_ODR_ODR5); } while(0)
#define benchmark_marker_end() do { PC_ODR &= ~(1 << PC_ODR_ODR5); } while(0)

static const crc8_type_t crc8_functions[] = {
	{ CRC8_1WIRE_INIT, crc8_1wire_update_ref, CRC8_1WIRE_XOROUT },
	{ CRC8_1WIRE_INIT, crc8_1wire_update, CRC8_1WIRE_XOROUT },
	{ CRC8_J1850_INIT, crc8_j1850_update_ref, CRC8_J1850_XOROUT },
	{ CRC8_J1850_INIT, crc8_j1850_update, CRC8_J1850_XOROUT },
	{ CRC8_AUTOSAR_INIT, crc8_autosar_update_ref, CRC8_AUTOSAR_XOROUT },
	{ CRC8_AUTOSAR_INIT, crc8_autosar_update, CRC8_AUTOSAR_XOROUT }
};

static const crc16_type_t crc16_functions[] = {
	{ CRC16_ANSI_INIT, crc16_ansi_update_ref, CRC16_ANSI_XOROUT },
	{ CRC16_ANSI_INIT, crc16_ansi_update, CRC16_ANSI_XOROUT },
	{ CRC16_CCITT_INIT, crc16_ccitt_update_ref, CRC16_CCITT_XOROUT },
	{ CRC16_CCITT_INIT, crc16_ccitt_update, CRC16_CCITT_XOROUT }
};

static const crc32_type_t crc32_functions[] = {
	{ CRC32_INIT, crc32_update_ref, CRC32_XOROUT },
	{ CRC32_INIT, crc32_update, CRC32_XOROUT },
	{ CRC32_POSIX_INIT, crc32_posix_update_ref, CRC32_POSIX_XOROUT },
	{ CRC32_POSIX_INIT, crc32_posix_update, CRC32_POSIX_XOROUT }
};

static const uint8_t test_data_a[] = {
	0x01, 0x32, 0xF0, 0x21, 0x97, 0x68, 0x22, 0x3E
};

static const uint8_t test_data_b[] = {
	0x03, 0x58, 0x24, 0x45, 0x19, 0x6B, 0x82, 0x62, 0x10, 0xCC, 0xD2, 0x47, 0x71, 0xAA, 0xB6, 0xF9,
	0x3B, 0x68, 0x2D, 0x10, 0x80, 0x7B, 0x26, 0xC8, 0xDD, 0x15, 0x58, 0xD6, 0x22, 0x06, 0x0B, 0xA1,
	0xC4, 0xF0, 0xCE, 0xCF, 0xEF, 0x69, 0x54, 0x47, 0x53, 0x40, 0xEF, 0x7C, 0xFD, 0x71, 0x58, 0xE0,
	0xDF, 0x64, 0xCE, 0xBB, 0xD5, 0x22, 0xB8, 0xA9, 0x46, 0x15, 0x3B, 0x6C, 0xB4, 0x1E, 0x93, 0xDE,
	0xD2, 0x88, 0x0D, 0xB1, 0x04, 0x1C, 0xEF, 0xF7, 0x4E, 0x69, 0xB3, 0x1A, 0x1B, 0xEA, 0x64, 0xB4,
	0x68, 0xA9, 0xFB, 0x46, 0x34, 0xC0, 0x0B, 0x93, 0x47, 0x9B, 0x22, 0xC2, 0xAB, 0xD2, 0xA7, 0xAD,
	0x6D, 0xE3, 0x1E, 0x35, 0x7E, 0x6A, 0x14, 0x1E, 0xD0, 0x23, 0x29, 0x2D, 0x02, 0xFB, 0xEE, 0x5A,
	0x35, 0x7B, 0x8F, 0x08, 0xE3, 0xF1, 0x83, 0x37, 0xCE, 0x34, 0xB9, 0xD4, 0x60, 0x3D, 0xFB, 0x72,
	0x15, 0x44, 0x7B, 0x29, 0xC5, 0xC4, 0xC7, 0x02, 0xE8, 0x68, 0x9B, 0x4D, 0x29, 0x4F, 0x48, 0x84,
	0xE6, 0x17, 0xA4, 0x30, 0x6B, 0x94, 0xC2, 0x81, 0x0A, 0x83, 0xEA, 0x05, 0x68, 0x74, 0x81, 0x76,
	0x87, 0x5E, 0xDF, 0x8C, 0x7F, 0x90, 0x4B, 0xBE, 0xE4, 0x3D, 0x95, 0x52, 0x48, 0xC3, 0x06, 0xDC,
	0x5A, 0xA5, 0x95, 0x78, 0x91, 0x32, 0xAB, 0xC3, 0x6B, 0x2B, 0xE1, 0xCC, 0x9B, 0xF7, 0x6C, 0x13,
	0xC4, 0x46, 0x44, 0x3F, 0x93, 0x99, 0x22, 0x68, 0x55, 0xA4, 0xE3, 0xFF, 0x55, 0xD6, 0xFB, 0x38,
	0xAF, 0x21, 0xFE, 0xD2, 0x5D, 0x75, 0x62, 0xE6, 0x9E, 0xC7, 0x07, 0x66, 0x0F, 0x21, 0x30, 0xE8,
	0x09, 0x61, 0xE7, 0xB1, 0x29, 0x82, 0x11, 0x47, 0x08, 0x8A, 0x8E, 0xB6, 0x86, 0x19, 0x3A, 0xCD,
	0x44, 0x53, 0xBA, 0x1B, 0x16, 0x94, 0x4A, 0x9A, 0x95, 0xD5, 0x09, 0x83, 0x1C, 0x95, 0x7E, 0x06
};

// From SAE J1850 standard, section 5.4.1, table 1.
// Also AUTOSAR standard document Specification of CRC Routines AUTOSAR CP R21-11, section 7.2.1.2.
// https://www.autosar.org/fileadmin/user_upload/standards/classic/21-11/AUTOSAR_SWS_CRCLibrary.pdf
static const uint8_t test_data_c[] = {
	0x33, 0x22, 0x55, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};

// From Maxim Application Note 27
// https://www.maximintegrated.com/en/design/technical-documents/app-notes/2/27.html
static const uint8_t test_data_d[] = {
	0x02, 0x1C, 0xB8, 0x01, 0x00, 0x00, 0x00
};

// All expected CRC values obtained from (and matching between) the following
// calculators:
// https://crccalc.com/
// https://www.tahapaksu.com/crc/
// https://www.scadacore.com/tools/programming-calculators/online-checksum-calculator/
// http://www.sunshine2k.de/coding/javascript/crc/crc_js.html

static const crc8_test_t crc8_tests[] = {
	{
		.name = "crc8-1wire",
		.data = test_data_a,
		.data_len = sizeof(test_data_a),
		.c_func = { CRC8_1WIRE_INIT, crc8_1wire_update_ref, CRC8_1WIRE_XOROUT },
		.asm_func = { CRC8_1WIRE_INIT, crc8_1wire_update, CRC8_1WIRE_XOROUT },
		.expected = 0x7C
	},
	{
		.name = "crc8-1wire",
		.data = test_data_d,
		.data_len = sizeof(test_data_d),
		.c_func = { CRC8_1WIRE_INIT, crc8_1wire_update_ref, CRC8_1WIRE_XOROUT },
		.asm_func = { CRC8_1WIRE_INIT, crc8_1wire_update, CRC8_1WIRE_XOROUT },
		.expected = 0xA2
	},
	{
		.name = "crc8-j1850",
		.data = test_data_a,
		.data_len = sizeof(test_data_a),
		.c_func = { CRC8_J1850_INIT, crc8_j1850_update_ref, CRC8_J1850_XOROUT },
		.asm_func = { CRC8_J1850_INIT, crc8_j1850_update, CRC8_J1850_XOROUT },
		.expected = 0x04
	},
	{
		.name = "crc8-j1850",
		.data = test_data_c,
		.data_len = sizeof(test_data_c),
		.c_func = { CRC8_J1850_INIT, crc8_j1850_update_ref, CRC8_J1850_XOROUT },
		.asm_func = { CRC8_J1850_INIT, crc8_j1850_update, CRC8_J1850_XOROUT },
		.expected = 0xCB
	},
	{
		.name = "crc8-autosar",
		.data = test_data_c,
		.data_len = sizeof(test_data_c),
		.c_func = { CRC8_AUTOSAR_INIT, crc8_autosar_update_ref, CRC8_AUTOSAR_XOROUT },
		.asm_func = { CRC8_AUTOSAR_INIT, crc8_autosar_update, CRC8_AUTOSAR_XOROUT },
		.expected = 0x11
	}
};

static const crc16_test_t crc16_tests[] = {
	{
		.name = "crc16-ansi",
		.data = test_data_a,
		.data_len = sizeof(test_data_a),
		.c_func = { CRC16_ANSI_INIT, crc16_ansi_update_ref, CRC16_ANSI_XOROUT },
		.asm_func = { CRC16_ANSI_INIT, crc16_ansi_update, CRC16_ANSI_XOROUT },
		.expected = 0x2B0E
	},
	{
		.name = "crc16-ansi",
		.data = test_data_b,
		.data_len = sizeof(test_data_b),
		.c_func = { CRC16_ANSI_INIT, crc16_ansi_update_ref, CRC16_ANSI_XOROUT },
		.asm_func = { CRC16_ANSI_INIT, crc16_ansi_update, CRC16_ANSI_XOROUT },
		.expected = 0x4173
	},
	{
		.name = "crc16-ccitt",
		.data = test_data_a,
		.data_len = sizeof(test_data_a),
		.c_func = { CRC16_CCITT_INIT, crc16_ccitt_update_ref, CRC16_CCITT_XOROUT },
		.asm_func = { CRC16_CCITT_INIT, crc16_ccitt_update, CRC16_CCITT_XOROUT },
		.expected = 0x6EBB
	},
	{
		.name = "crc16-ccitt",
		.data = test_data_b,
		.data_len = sizeof(test_data_b),
		.c_func = { CRC16_CCITT_INIT, crc16_ccitt_update_ref, CRC16_CCITT_XOROUT },
		.asm_func = { CRC16_CCITT_INIT, crc16_ccitt_update, CRC16_CCITT_XOROUT },
		.expected = 0x61DE
	},
	{
		.name = "crc16-xmodem",
		.data = test_data_a,
		.data_len = sizeof(test_data_a),
		.c_func = { CRC16_XMODEM_INIT, crc16_xmodem_update_ref, CRC16_XMODEM_XOROUT },
		.asm_func = { CRC16_XMODEM_INIT, crc16_xmodem_update, CRC16_XMODEM_XOROUT },
		.expected = 0x5F85
	},
	{
		.name = "crc16-xmodem",
		.data = test_data_b,
		.data_len = sizeof(test_data_b),
		.c_func = { CRC16_XMODEM_INIT, crc16_xmodem_update_ref, CRC16_XMODEM_XOROUT },
		.asm_func = { CRC16_XMODEM_INIT, crc16_xmodem_update, CRC16_XMODEM_XOROUT },
		.expected = 0x2036
	}
};

static const crc32_test_t crc32_tests[] = {
	{
		.name = "crc32",
		.data = test_data_a,
		.data_len = sizeof(test_data_a),
		.c_func = { CRC32_INIT, crc32_update_ref, CRC32_XOROUT },
		.asm_func = { CRC32_INIT, crc32_update, CRC32_XOROUT },
		.expected = 0x7FC76C2F
	},
	{
		.name = "crc32",
		.data = test_data_b,
		.data_len = sizeof(test_data_b),
		.c_func = { CRC32_INIT, crc32_update_ref, CRC32_XOROUT },
		.asm_func = { CRC32_INIT, crc32_update, CRC32_XOROUT },
		.expected = 0x791FF31F
	},
	{
		.name = "crc32-posix",
		.data = test_data_a,
		.data_len = sizeof(test_data_a),
		.c_func = { CRC32_POSIX_INIT, crc32_posix_update_ref, CRC32_POSIX_XOROUT },
		.asm_func = { CRC32_POSIX_INIT, crc32_posix_update, CRC32_POSIX_XOROUT },
		.expected = 0x93E5A427
	},
	{
		.name = "crc32-posix",
		.data = test_data_b,
		.data_len = sizeof(test_data_b),
		.c_func = { CRC32_POSIX_INIT, crc32_posix_update_ref, CRC32_POSIX_XOROUT },
		.asm_func = { CRC32_POSIX_INIT, crc32_posix_update, CRC32_POSIX_XOROUT },
		.expected = 0x4D84D9B6
	}
};

/******************************************************************************/

void print_hex(const void *data, const size_t data_len) {
	for(size_t i = 0; i < data_len; i++) {
		if(i > 0) putchar(' ');
		printf("%02X", ((const uint8_t *)data)[i]);
	}
}

void verify(void) {
	// Use ANSI terminal escape codes for highlighting pass/fail text.
	static const char pass_str[] = "\x1B[1m\x1B[32mPASS\x1B[0m"; // Bold green
	static const char fail_str[] = "\x1B[1m\x1B[31mFAIL\x1B[0m"; // Bold red
	uint8_t crc_8_c, crc_8_asm;
	uint16_t crc_16_c, crc_16_asm;
	uint32_t crc_32_c, crc_32_asm;

	printf("verify()\n");

	for(size_t i = 0; i < (sizeof(crc8_tests) / sizeof(crc8_tests[0])); i++) {
		printf("  %s:\n", crc8_tests[i].name);
		printf("    data = ");
		print_hex(crc8_tests[i].data, crc8_tests[i].data_len);
		printf(" (%u bytes)\n", crc8_tests[i].data_len);
		printf("    expected = 0x%02X\n", crc8_tests[i].expected);

		crc_8_c = crc8_tests[i].c_func.init_val;
		crc_8_asm = crc8_tests[i].asm_func.init_val;

		for(size_t n = 0; n < crc8_tests[i].data_len; n++) {
			crc_8_c = (*crc8_tests[i].c_func.update_func)(crc_8_c, crc8_tests[i].data[n]);
			crc_8_asm = (*crc8_tests[i].asm_func.update_func)(crc_8_asm, crc8_tests[i].data[n]);
		}

		crc_8_c ^= crc8_tests[i].c_func.xorout_val;
		crc_8_asm ^= crc8_tests[i].asm_func.xorout_val;

		printf("    c = 0x%02X - %s\n", crc_8_c, (crc_8_c == crc8_tests[i].expected ? pass_str : fail_str));
		printf("    asm = 0x%02X - %s\n", crc_8_asm, (crc_8_asm == crc8_tests[i].expected ? pass_str : fail_str));
	}

	for(size_t i = 0; i < (sizeof(crc16_tests) / sizeof(crc16_tests[0])); i++) {
		printf("  %s:\n", crc16_tests[i].name);
		printf("    data = ");
		print_hex(crc16_tests[i].data, crc16_tests[i].data_len);
		printf(" (%u bytes)\n", crc16_tests[i].data_len);
		printf("    expected = 0x%04X\n", crc16_tests[i].expected);

		crc_16_c = crc16_tests[i].c_func.init_val;
		crc_16_asm = crc16_tests[i].asm_func.init_val;

		for(size_t n = 0; n < crc16_tests[i].data_len; n++) {
			crc_16_c = (*crc16_tests[i].c_func.update_func)(crc_16_c, crc16_tests[i].data[n]);
			crc_16_asm = (*crc16_tests[i].asm_func.update_func)(crc_16_asm, crc16_tests[i].data[n]);
		}

		crc_16_c ^= crc16_tests[i].c_func.xorout_val;
		crc_16_asm ^= crc16_tests[i].asm_func.xorout_val;

		printf("    c = 0x%04X - %s\n", crc_16_c, (crc_16_c == crc16_tests[i].expected ? pass_str : fail_str));
		printf("    asm = 0x%04X - %s\n", crc_16_asm, (crc_16_asm == crc16_tests[i].expected ? pass_str : fail_str));
	}

	for(size_t i = 0; i < (sizeof(crc32_tests) / sizeof(crc32_tests[0])); i++) {
		printf("  %s:\n", crc32_tests[i].name);
		printf("    data = ");
		print_hex(crc32_tests[i].data, crc32_tests[i].data_len);
		printf(" (%u bytes)\n", crc32_tests[i].data_len);
		printf("    expected = 0x%08lX\n", crc32_tests[i].expected);

		crc_32_c = crc32_tests[i].c_func.init_val;
		crc_32_asm = crc32_tests[i].asm_func.init_val;

		for(size_t n = 0; n < crc32_tests[i].data_len; n++) {
			crc_32_c = (*crc32_tests[i].c_func.update_func)(crc_32_c, crc32_tests[i].data[n]);
			crc_32_asm = (*crc32_tests[i].asm_func.update_func)(crc_32_asm, crc32_tests[i].data[n]);
		}

		crc_32_c ^= crc32_tests[i].c_func.xorout_val;
		crc_32_asm ^= crc32_tests[i].asm_func.xorout_val;

		printf("    c = 0x%08lX - %s\n", crc_32_c, (crc_32_c == crc32_tests[i].expected ? pass_str : fail_str));
		printf("    asm = 0x%08lX - %s\n", crc_32_asm, (crc_32_asm == crc32_tests[i].expected ? pass_str : fail_str));
	}
}

void benchmark(const uint16_t iters) {
	uint8_t crc_8;
	uint16_t crc_16, n;
	uint32_t crc_32;

	printf("benchmark()\n");

	for(size_t i = 0; i < (sizeof(crc8_functions) / sizeof(crc8_functions[0])); i++) {
		n = iters;
		crc_8 = crc8_functions[i].init_val;
		benchmark_marker_start();
		while(n--) {
			crc_8 = (*crc8_functions[i].update_func)(crc_8, 0x55);
		}
		benchmark_marker_end();
		crc_8 ^= crc8_functions[i].xorout_val;
	}

	for(size_t i = 0; i < (sizeof(crc16_functions) / sizeof(crc16_functions[0])); i++) {
		n = iters;
		crc_16 = crc16_functions[i].init_val;
		benchmark_marker_start();
		while(n--) {
			crc_16 = (*crc16_functions[i].update_func)(crc_16, 0x55);
		}
		benchmark_marker_end();
		crc_16 ^= crc16_functions[i].xorout_val;
	}

	for(size_t i = 0; i < (sizeof(crc32_functions) / sizeof(crc32_functions[0])); i++) {
		n = iters;
		crc_32 = crc32_functions[i].init_val;
		benchmark_marker_start();
		while(n--) {
			crc_32 = (*crc32_functions[i].update_func)(crc_32, 0x55);
		}
		benchmark_marker_end();
		crc_32 ^= crc32_functions[i].xorout_val;
	}
}

void main(void) {
	CLK_CKDIVR = 0;
	PC_DDR = (1 << PC_DDR_DDR5);
	PC_CR1 = (1 << PC_CR1_C15);

	if(ucsim_if_detect()) {
		uart_init(UART_BAUD_115200, ucsim_if_putchar, NULL);
	} else {
		uart_init(UART_BAUD_115200, uart_putchar, uart_getchar);
	}

	verify();
	benchmark(10000);

	if(ucsim_if_detect()) {
		ucsim_if_stop();
	} else {
		while(1);
	}
}

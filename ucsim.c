/*******************************************************************************
 *
 * ucsim.c - ucSim 'simif' interface
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "ucsim.h"

bool ucsim_if_detect(void) {
	UCSIM_IF = UCSIM_IF_CMD_DETECT;
	return (UCSIM_IF == UCSIM_IF_DETECT_RESP);
}

uint8_t ucsim_if_version(void) {
	UCSIM_IF = UCSIM_IF_CMD_IFVER;
	return UCSIM_IF;
}

void ucsim_if_reset(void) {
	UCSIM_IF = UCSIM_IF_CMD_IFRESET;
}

void ucsim_if_stop(void) {
	UCSIM_IF = UCSIM_IF_CMD_STOP;
}

int ucsim_if_putchar(int c) {
	UCSIM_IF = UCSIM_IF_CMD_PRINT;
	UCSIM_IF = (uint8_t)c;
	return c;
}

bool ucsim_if_fin_avail(void) {
	UCSIM_IF = UCSIM_IF_CMD_FIN_CHECK;
	return UCSIM_IF;
}

int ucsim_if_fin_getc(void) {
	UCSIM_IF = UCSIM_IF_CMD_READ;
	return UCSIM_IF;
}

int ucsim_if_fout_putc(int c) {
	UCSIM_IF = UCSIM_IF_CMD_WRITE;
	UCSIM_IF = c;
	return c;
}

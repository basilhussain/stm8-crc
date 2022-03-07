/*******************************************************************************
 *
 * ucsim.h - Header for ucSim 'simif' interface
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

#ifndef UCSIM_H_
#define UCSIM_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Address of 'register' used to interface with ucSim. Chosen to be just after
// the memory range used by GPIO/peripheral registers (0x5000 to 0x57FF).
#define UCSIM_IF_ADDR 0x5800
#define UCSIM_IF (*(volatile uint8_t *)(UCSIM_IF_ADDR))

// Answer to detect command.
#define UCSIM_IF_DETECT_RESP '!'

typedef enum {
	UCSIM_IF_CMD_DETECT		= '_',	// Command used to detect the interface
	UCSIM_IF_CMD_COMMANDS	= 'i',	// Get info about commands
	UCSIM_IF_CMD_IFVER		= 'v',	// Interface version
	UCSIM_IF_CMD_SIMVER		= 'V',	// Simulator version
	UCSIM_IF_CMD_IFRESET	= '@',	// Reset the interface
	UCSIM_IF_CMD_CMDINFO	= 'I',	// Info about a command
	UCSIM_IF_CMD_CMDHELP	= 'h',	// Help about a command
	UCSIM_IF_CMD_STOP		= 's',	// Stop simulation
	UCSIM_IF_CMD_PRINT		= 'p',	// Print character
	UCSIM_IF_CMD_FIN_CHECK	= 'f',	// Check input file for input
	UCSIM_IF_CMD_READ		= 'r',	// Read from input file
	UCSIM_IF_CMD_WRITE		= 'w',	// Write to output file
} ucsim_if_cmd_t;

/******************************************************************************/

extern bool ucsim_if_detect(void);
extern uint8_t ucsim_if_version(void);
extern void ucsim_if_reset(void);
extern void ucsim_if_stop(void);
extern int ucsim_if_putchar(int c);
extern bool ucsim_if_fin_avail(void);
extern int ucsim_if_fin_getc(void);
extern int ucsim_if_fout_putc(int c);

#endif // UCSIM_H_

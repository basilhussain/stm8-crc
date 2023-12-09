/*******************************************************************************
 *
 * crc32_posix.c - CRC32-POSIX implementation
 *
 * Copyright (c) 2023 Basil Hussain
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
#include "../crc.h"
#include "common.h"

// CRC32-POSIX (aka cksum)
// Polynomial: x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1 (0x04C11DB7, normal)
// Initial value: 0x00000000
// XOR out: 0xFFFFFFFF

#ifdef ALGORITHM_LUT

static const uint32_t crc32_posix_lut[16] = {
	0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9,
	0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
	0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61,
	0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD
};

uint32_t crc32_posix_update(uint32_t crc, uint8_t data) __naked __stack_args {
	// Avoid compiler warnings for unreferenced args.
	(void)crc;
	(void)data;

	// For return value/arg: 0xAABBCCDD
	// x = 0xCCDD (xh = 0xCC, xl = 0xDD)
	// y = 0xAABB (yh = 0xAA, yl = 0xBB)

	__asm
		; XOR the MSB of the CRC with data byte and store back to CRC.
		ld a, (ASM_ARGS_SP_OFFSET+4, sp)
		xor a, (ASM_ARGS_SP_OFFSET+0, sp)
		ld (ASM_ARGS_SP_OFFSET+0, sp), a

		.rept 2
			; Take most-significant nibble of CRC and use as index into LUT.
			; But, we want it as a byte offset into the LUT. Instead of swapping
			; high nibble into low position and multiplying by 4, we can instead
			; mask it and divide by 4.
			ld a, (ASM_ARGS_SP_OFFSET+0, sp)
			and a, #0xF0
			srl a
			srl a

			; LUT offset is in A, but we need it in X to perform the lookup.
			clrw x
			ld xl, a

			; Shift the CRC value on the stack left by 4 bits.
			; Oh, my kingdom for a barrel shifter!
			.rept 4
				sll (ASM_ARGS_SP_OFFSET+3, sp)
				rlc (ASM_ARGS_SP_OFFSET+2, sp)
				rlc (ASM_ARGS_SP_OFFSET+1, sp)
				rlc (ASM_ARGS_SP_OFFSET+0, sp)
			.endm

			; For each byte of the CRC on stack, XOR it with the corresponding
			; byte of the value in LUT (loaded from offset given in X), and
			; store it back on the stack.
			ld a, (_crc32_posix_lut+0, x)
			xor a, (ASM_ARGS_SP_OFFSET+0, sp)
			ld (ASM_ARGS_SP_OFFSET+0, sp), a
			ld a, (_crc32_posix_lut+1, x)
			xor a, (ASM_ARGS_SP_OFFSET+1, sp)
			ld (ASM_ARGS_SP_OFFSET+1, sp), a
			ld a, (_crc32_posix_lut+2, x)
			xor a, (ASM_ARGS_SP_OFFSET+2, sp)
			ld (ASM_ARGS_SP_OFFSET+2, sp), a
			ld a, (_crc32_posix_lut+3, x)
			xor a, (ASM_ARGS_SP_OFFSET+3, sp)
			ld (ASM_ARGS_SP_OFFSET+3, sp), a
		.endm

		; Load CRC value from stack into X and Y regs for function return value.
		ldw y, (ASM_ARGS_SP_OFFSET+0, sp)
		ldw x, (ASM_ARGS_SP_OFFSET+2, sp)

		ASM_RETURN
	__endasm;
}

#else

uint32_t crc32_posix_update(uint32_t crc, uint8_t data) __naked __stack_args {
	// Avoid compiler warnings for unreferenced args.
	(void)crc;
	(void)data;

	// For return value/arg: 0xAABBCCDD
	// x = 0xCCDD (xh = 0xCC, xl = 0xDD)
	// y = 0xAABB (yh = 0xAA, yl = 0xBB)

	__asm
		; XOR the MSB of the CRC with data byte, and put it back in the CRC.
		ld a, (ASM_ARGS_SP_OFFSET+4, sp)
		xor a, (ASM_ARGS_SP_OFFSET+0, sp)
		ld (ASM_ARGS_SP_OFFSET+0, sp), a

		; Load CRC variable from stack into X & Y regs for further work.
		ldw x, (ASM_ARGS_SP_OFFSET+2, sp)
		ldw y, (ASM_ARGS_SP_OFFSET+0, sp)

	.macro crc32_posix_update_shift_xor skip_lbl
			; Shift CRC value left by one bit.
			sllw x
			rlcw y

			; Jump if most-significant bit of CRC is now zero.
			jrnc skip_lbl

			; XOR the CRC value with the polynomial value.
			rrwa x
			xor a, #0xB7
			rrwa x
			xor a, #0x1D
			rrwa x
			rrwa y
			xor a, #0xC1
			rrwa y
			xor a, #0x04
			rrwa y

		skip_lbl:
	.endm

#ifdef ALGORITHM_BITWISE_UNROLLED

		crc32_posix_update_shift_xor 0001$
		crc32_posix_update_shift_xor 0002$
		crc32_posix_update_shift_xor 0003$
		crc32_posix_update_shift_xor 0004$
		crc32_posix_update_shift_xor 0005$
		crc32_posix_update_shift_xor 0006$
		crc32_posix_update_shift_xor 0007$
		crc32_posix_update_shift_xor 0008$

#else

		; Initialise counter to loop 8 times, once for each bit of data byte.
		ld a, #8

	0001$:

		crc32_posix_update_shift_xor 0002$

		; Decrement counter and loop around if it is not zero.
		dec a
		jrne 0001$

#endif

		; The X and Y registers now contain updated CRC value, so leave them
		; there as function return value.
		ASM_RETURN
	__endasm;
}

#endif

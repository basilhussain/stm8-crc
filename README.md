# Overview

This is a library providing CRC8, CRC16 and CRC32 computation functions for the STM8 microcontroller platform and the [SDCC](http://sdcc.sourceforge.net/) compiler. It aims to optimise for fast execution speed by implementing the CRC calculation functions with assembly code.

Implementations are included for the following CRC types:

* CRC8-1WIRE (aka Dallas, Maxim, iButton)
* CRC8-SAE-J1850 (aka OBD)
* CRC8-AUTOSAR (aka CRC8H2F)
* CRC16-ANSI (aka IBM, Modbus, USB)
* CRC16-CCITT
* CRC16-XMODEM
* CRC32 (aka GZIP, PKZIP, PNG, ZMODEM)
* CRC32-POSIX (aka cksum)

Because this library targets the STM8 embedded microcontroller family, in order to keep the compiled code size fairly compact, the bitwise computation technique is used. Various table-lookup techniques exist that provide faster computation, but these are generally not suitable for memory-constrained embedded environments, so this library does not use them.

In addition to the library functions, code is also included for plain C reference implementations of each CRC function, as well as a test and benchmarking program.

# Setup

You may either use a pre-compiled version of the library, or build the library code yourself. See below for further details.

This library has been written to accomodate and provide for both 'medium' (16-bit address space) and 'large' (24-bit address space) STM8 memory models.

* If you are building your project with either no specific SDCC memory model option, or the `--model-medium` option, then use the non-suffixed `crc.lib` library file.
* If you are building with `--model-large`, then use the `crc-large.lib` library file.

Unsure? If your target STM8 microcontroller model has less than 32KB of flash memory, then choose the former version; if larger flash, then you probably want the latter.

## Pre-compiled Library

1. Extract the relevant `.lib` file (see above) and `crc.h` file from the release archive.
2. Copy the two files to your project.

## Building

This library is developed and built with the [Code::Blocks](http://codeblocks.org/) IDE and [SDCC](http://sdcc.sourceforge.net/) compiler.

1. Load the `.cbp` project file in Code::Blocks.
2. Select the appropriate 'Library' build target for your STM8 memory model (see above) from the drop-down list on the compiler toolbar (or the *Build > Select Target* menu).
3. Build the library by pressing the 'Build' icon on the compiler toolbar (or Ctrl-F9 keyboard shortcut, or *Build > Build* menu entry).
4. Upon successful compilation, the resultant `.lib` file will be in the main base folder.
5. Copy the `.lib` file and the `crc.h` file to your project.

# Usage

1. Include the `crc.h` file in your C code wherever you want to use the CRC functions.
2. When compiling, provide the path to the `.lib` file with the `-l` SDCC command-line option.

For each CRC type, there are two functions provided: one that gives the appropriate initial value for that CRC variant, and one for incrementally computing the CRC on a byte-by-byte basis.

To calculate a CRC:

1. Declare a variable of appropriate type (`uint8_t` for CRC8, `uint16_t` for CRC16, `uint32_t` for CRC32) to hold the CRC value, and assign its initial value using the relevant 'init' function. **Always assign the initial value this way, or you may end up computing incorrect CRC values!**
2. For each byte of data that you wish to compute the CRC for, call the relevant 'update' function, passing the existing CRC variable value, and the data byte. The function will return a new CRC value, which should be re-assigned to the CRC variable.
3. Lastly, call the relevant 'final' function, passing the current CRC value. It will return the finalised CRC value. **Again, do not miss this step, or the CRC value may be incorrect!** It should also be noted that you should not continue to update the CRC value (with 'update' function calls) once this function has been used.

Please note that no functions are provided for computing a CRC across a buffer of data, because such code is not only trivial to write but also bloats the library with potentially unneeded code.

## Example

Calculating the 16-bit CCITT CRC of a data buffer:

```c
#include "crc.h"

static const uint8_t data[] = { 0x01, 0x32, 0xF0, 0x21, 0x97, 0x68, 0x22, 0x3E };
uint16_t crc = crc16_ccitt_init();

for(size_t i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
	crc = crc16_ccitt_update(crc, data[i]);
}

crc = crc16_ccitt_final(crc);
```

## Function Reference

```
uint8_t crc8_1wire_init()
uint8_t crc8_j1850_init()
uint8_t crc8_autosar_init()
uint16_t crc16_ansi_init()
uint16_t crc16_ccitt_init()
uint16_t crc16_xmodem_init()
uint32_t crc32_init()
uint32_t crc32_posix_init()

uint8_t crc8_1wire_update(uint8_t crc, uint8_t data)
uint8_t crc8_j1850_update(uint8_t crc, uint8_t data)
uint8_t crc8_autosar_update(uint8_t crc, uint8_t data)
uint16_t crc16_ansi_update(uint16_t crc, uint8_t data)
uint16_t crc16_ccitt_update(uint16_t crc, uint8_t data)
uint16_t crc16_xmodem_update(uint16_t crc, uint8_t data)
uint32_t crc32_update(uint32_t crc, uint8_t data)
uint32_t crc32_posix_update(uint32_t crc, uint8_t data)

uint8_t crc8_1wire_final(crc)
uint8_t crc8_j1850_final(crc)
uint8_t crc8_autosar_final(crc)
uint16_t crc16_ansi_final(crc)
uint16_t crc16_ccitt_final(crc)
uint16_t crc16_xmodem_final(crc)
uint32_t crc32_final(crc)
uint32_t crc32_posix_final(crc)
```

Note: the 'init' functions are actually macro definitions, so you may use them anywhere that a literal constant value is valid (e.g. initialisation of an array). The 'final' functions are also macros, but are not suitable for use in this way - rather, they are macros for the purposes of compiler optimisation.

# Benchmarks

To benchmark the optimised assembly implementations, they were compared with the execution speed of equivalent plain C implementations. Each function was run for 10,000 iterations, on each iteration updating the CRC value with a fixed data byte of `0x55`. Code was compiled using SDCC's default 'balanced' optimisation level. The benchmark was ran using the [μCsim](http://mazsola.iit.uni-miskolc.hu/~drdani/embedded/ucsim/) microcontroller simulator included with SDCC. The number of clock cycles consumed by all iterations of the loop (but not including initial value assignment or final XOR-out) was measured using the timer commands of μCsim.

| Implementation |  C Cycles | ASM Cycles | Ratio |
| -------------- | --------: | ---------: | ----: |
| CRC8-1WIRE     | 2,030,015 |    770,023 | 37.9% |
| CRC8-SAE-J1850 | 1,890,005 |    770,023 | 40.7% |
| CRC8-AUTOSAR   | 1,880,011 |    770,023 | 41.0% |
| CRC16-ANSI     | 2,318,727 |  1,059,375 | 45.7% |
| CRC16-CCITT    | 2,371,031 |  1,060,599 | 44.7% |
| CRC16-XMODEM†  |           |            |       |
| CRC32          | 3,851,349 |  1,481,349 | 38.5% |
| CRC32-POSIX    | 3,700,362 |  1,480,404 | 40.0% |

*(† See CCITT - algorithm is the same; only differs by initial value)*

Overall, the optimised assembly implementations execute in roughly 40% the time of the reference C implementations.

To confirm these results, the benchmark code was also ran on physical STM8 hardware (an STM8S208RBT6 Nucleo-64 board), with timing (in milliseconds) measured by capturing with a logic analyser the toggling of an IO pin before and after each iteration loop. A roughly equal relationship between the speed of reference C code and optimised assembly implementations was observed, confirming that the simulator results are accurate.

For the code used in the reference C implementations, see the `crc_ref.c` file. The benchmark code is in the `benchmark()` function of the `main.c` test program.

# Code Size

To attain the fastest execution, generally some trade-offs often have to be made, and in the case of this library, it is at the expense of compiled code size. Primarily due to the use of loop-unrolling, the size of the assembly CRC functions are larger than their reference C counterparts, but not by an egregious amount - typically only roughly twice the size. Some selected examples:

* `crc8_1wire_update` (ASM): 45 bytes
* `crc8_1wire_update_ref` (C): 41 bytes
* `crc16_ansi_update` (ASM): 89 bytes
* `crc16_ansi_update_ref` (C): 45 bytes
* `crc32_update` (ASM): 187 bytes
* `crc32_update_ref` (C): 95 bytes

If you want to use these library functions but minimise the code size, it is possible to disable the loop unrolling by removing the `ASM_UNROLL_LOOP` macro definition from the Code::Blocks project build options and re-compiling. While this will compromise the execution speed, it should still be faster than the reference C implementations.

# Licence

This library is licenced under the MIT Licence. See source code headers for full licence text.

# Contributing

Bug fixes, further optimisations, or additional CRC implementations are welcome. Please create a new GitHub issue or pull request.

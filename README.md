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

In addition to the library functions, code is also included for plain C reference implementations of each CRC function, as well as a test and benchmarking program.

Three variants of the library are available:

* **Fastest**: uses lookup table calculation (where implemented, otherwise bitwise), trading larger code size for the fastest performance.
* **Faster**: uses bitwise calculation with unrolled loops, making less of a trade-off of code size for speed.
* **Small**: uses bitwise calculation, but without unrolled loops, to minimise code size at the expense of speed.

You may choose to use the variant which best suits your performance and/or code size needs. See also [Code Size](#code-size) section below for details.

# Setup

You may either use a pre-compiled version of the library, or build the library code yourself. See below for further details.

This library has been written to accommodate and provide for both 'medium' (16-bit address space) and 'large' (24-bit address space) STM8 memory models.

* If you are building your project with either no specific SDCC memory model option, or the `--model-medium` option, then use one of the `.lib` library files *without* `large` suffix.
* If you are building with `--model-large`, then use a `.lib` library file *with* `large` suffix.

Unsure? If your target STM8 microcontroller model has less than 32KB of flash memory, then choose the former version; if larger flash, then you probably want the latter.

## Pre-compiled Library

Library files for each combination of performance variant and memory model are provided. The selection of provided pre-compiled `.lib` files is as follows:

| Variant | Memory Model | Library Filename        |
| ------- | ------------ | ----------------------- |
| Fastest | Medium       | `crc-fastest.lib`       |
| Fastest | Large        | `crc-large-fastest.lib` |
| Faster  | Medium       | `crc-faster.lib`        |
| Faster  | Large        | `crc-large-faster.lib`  |
| Small   | Medium       | `crc.lib`               |
| Small   | Large        | `crc-large.lib`         |

Once you have chosen which `.lib` file you will use:

1. Extract the relevant `.lib` file and `crc.h` file from the release archive.
2. Copy the two files to your project.

## Building

This library is developed and built with the [Code::Blocks](http://codeblocks.org/) IDE and [SDCC](http://sdcc.sourceforge.net/) compiler.

1. Load the `.cbp` project file in Code::Blocks.
2. Select the appropriate 'Library' build target for your chosen variant and STM8 memory model (see above) from the drop-down list on the compiler toolbar (or the *Build > Select Target* menu).
3. Build the library by pressing the 'Build' icon on the compiler toolbar (or Ctrl-F9 keyboard shortcut, or *Build > Build* menu entry).
4. Upon successful compilation, the resultant `.lib` file will be in the main base folder.
5. Copy the `.lib` file and the `crc.h` file to your project.

# Usage

1. Include the `crc.h` file in your C code wherever you want to use the CRC functions.
2. When compiling, provide the path to the `.lib` file with the `-l` SDCC command-line option.

For each CRC type, there are three functions provided: one that gives the appropriate initial value for that CRC variant, one for incrementally computing the CRC on a byte-by-byte basis, and one for finalising the CRC value.

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

To benchmark the fastest optimised assembly implementations, they were compared with the execution speed of equivalent plain C implementations. Each function was run for 10,000 iterations, on each iteration updating the CRC value with a fixed data byte of `0x55`. Code was compiled using SDCC's default 'balanced' optimisation level. The benchmark was ran using the [μCsim](http://mazsola.iit.uni-miskolc.hu/~drdani/embedded/ucsim/) microcontroller simulator included with SDCC. The number of clock cycles consumed by all iterations of the loop (but not including initial value assignment or final XOR-out) was measured using the timer commands of μCsim.

| CRC Type       |  C Cycles | ASM Cycles | Ratio |
| -------------- | --------: | ---------: | ----: |
| CRC8-1WIRE     | 1,750,009 |    700,017 | 40.0% |
| CRC8-SAE-J1850 | 1,649,999 |    700,017 | 42.4% |
| CRC8-AUTOSAR   | 1,650,005 |    700,017 | 42.4% |
| CRC16-ANSI     | 2,188,720 |    979,368 | 44.7% |
| CRC16-CCITT    | 2,121,168 |    980,592 | 46.2% |
| CRC16-XMODEM†  |           |            |       |
| CRC32          | 3,101,488 |  1,250,018 | 40.3% |
| CRC32-POSIX    | 3,010,438 |  1,250,018 | 41.5% |

*(† See CCITT - algorithm is the same; only differs by initial value)*

Overall, the optimised assembly implementations execute in roughly 40% the time of the reference C implementations.

To confirm these results, the benchmark code was also ran on physical STM8 hardware (an STM8S208RBT6 Nucleo-64 board), with timing (in milliseconds) measured by capturing with a logic analyser the toggling of an IO pin before and after each iteration loop. A roughly equal relationship between the speed of reference C code and optimised assembly implementations was observed, confirming that the simulator results are accurate.

For the code used in the reference C implementations, see the `crc_ref.c` file. The benchmark code is in the `benchmark()` function of the `main.c` test program.

# Code Size

For the faster and fastest performing variants of this library, generally some trade-offs often have to be made, and in the case of these variants, it is at the expense of compiled code size. Primarily due to the use of lookup tables (LUTs) and/or loop-unrolling, the size of the assembly CRC functions are much larger than their reference C counterparts, but not by an egregious amount - typically only around twice the size.

Some selected comparisons of size of 'update' functions (in bytes):

| CRC Type    | Reference C | Fastest | Faster | Small |
| ----------- | ----------: | ------: | -----: | ----: |
| CRC8-1WIRE  |          39 |      45 |     45 |    16 |
| CRC16-ANSI  |          45 |      89 |     89 |    24 |
| CRC16-CCITT |          46 |      89 |     89 |    24 |
| CRC32       |          95 |    †211 |    187 |    38 |
| CRC32-POSIX |          93 |    †211 |    187 |    38 |

*(† Includes size of associated lookup table)*

The 'Fastest' variant is built with both `ALGORITHM_LUT` and `ALGORITHM_BITWISE_UNROLLED` defined. These definitions enable a lookup-table (LUT) based algorithm, or where not available (not all CRC types have it) a bitwise algorithm with unrolled loops.

The 'Faster' variant is built only with `ALGORITHM_BITWISE_UNROLLED` defined, so only bitwise algorithms with unrolled loops are enabled - no LUT algorithms.

The 'Small' variant is built with neither of the above definitions, so only looped bitwise algorithms are used. While this will compromise the execution speed, it may still be faster than the reference C implementations.

# Licence

This library is licenced under the MIT Licence. See source code headers for full licence text.

# Contributing

Bug fixes, further optimisations, or additional CRC implementations are welcome. Please create a new GitHub issue or pull request.

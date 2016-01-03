/*
 * Emrom Loader
 * Copyright (c) 2016 Andrey Skrypka
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "options.h"
#include "serial.h"
#include "parser.h"

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM "WINDOWS"
#elif defined(__linux__)
#define PLATFORM "LINUX"
#else
#define PLATFORM "UNKNOWN"
#endif

#define MEMORY_SIZE 0x10000

enum
{
    EMROM_ERASE_ERROR = -34,
    EMROM_WRITE_ERROR = -35,
};

static const option_t options[];
static const result_t results[];
static uint8_t memory[MEMORY_SIZE];

static int read_handler(uint32_t address, uint8_t value)
{
    if (address > MEMORY_SIZE)
        return PARSER_MEMORY_ERROR;
    memory[address] = value;
    return 0;
}

static int load(char *argv[])
{
    int result;
    printf("Loading from %s...", argv[0]);
    if (result = parser_read(argv[0], read_handler))
        return result;

    /*while (address <= stm32_end_address)
    {
        size = stm32_end_address - address + 1;
        if (size > 256)
            size = 256;
        if (result = stm32_write_memory(memory, address, size))
            return result;
        memory += size;
        address += size;
    }*/
    return 0;
}

static int erase(char *argv[])
{
    printf("Erasing...");
    return EMROM_ERASE_ERROR;
}

static int help(char* argv[])
{
    options_usage(options, results, 48);
    printf("Printing help...");
    return 0;
}

static const option_t options[] =
{
    {"-l", "--load", "Load data from file to device memory", 1, load},
    {"-e", "--erase", "Erase device memory with 0xFF", 0, erase},
    {"-h", "--help", "Print this help", 0, help},
    {0, 0, 0, 0, 0}
};

static const result_t results[] =
{
    {EMROM_WRITE_ERROR, "Failed to write data to device memory"},
    {EMROM_ERASE_ERROR, "Failed to erase device memory"},
    {PARSER_MEMORY_ERROR, "Invalid device memory location in file"},
    {PARSER_RECORD_ERROR, "Invalid record in file"},
    {PARSER_CHECKSUM_ERROR, "Invalid checksum of file"},
    {PARSER_IO_ERROR, "Bad format of file"},
    {PARSER_FILE_ERROR, "Can't open or create file"},
    {SERIAL_IO_ERROR, "Serial port IO error"},
    {SERIAL_FILE_ERROR, "Can't open serial port"},
    {OPTIONS_ARG_ERROR, "Invalid actual parameters"},
    {OPTIONS_ERROR, "Invalid option"},
    {0, "No errors, all done"},
};

int main(int argc, char* argv[])
{
    printf("\nEmrom Loader Version 1.00 %s\n", PLATFORM);
    printf("Copyright (c) 2016 Andrey Skrypka\n");
    return options_execute(options, argc, argv);
}


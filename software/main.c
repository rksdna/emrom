/*
 * Emrom - ROM emulator software
 * Copyright (c) 2016 rksdna
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
#include "buffer.h"
#include "error.h"

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM "WINDOWS"
#elif defined(__linux__)
#define PLATFORM "LINUX"
#else
#define PLATFORM "UNKNOWN"
#endif

#define MEMORY_SIZE 0x10000
#define PAGE_SIZE 0x100

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

static int emrom_checksum(int index)
{
    int result = index;
    int count = PAGE_SIZE;
    const uint8_t *p = memory + index * PAGE_SIZE;
    while (count--)
        result += *p++;
    return result;
}

static int emrom_page(int index)
{
    int result = 0;
    uint8_t ack;
    if (result = serial_write(&index, 1))
        return result;
    if (result = serial_write(memory + index * PAGE_SIZE, PAGE_SIZE))
        return result;
    if (result = serial_read(&ack, 1))
        return result;
    if ((uint8_t)emrom_checksum(index) != ack)
        return EMROM_WRITE_ERROR;
    return 0;
}

static int dump_memory(void)
{
    serial_open("/dev/ttyUSB0");
    serial_wait(10);
    serial_clear();
    printf(" DUMP %d", emrom_page(0));
    serial_close();
    return 0;

}

static int load(char *argv[])
{
    int result;
    printf("Loading from %s...", argv[0]);
    if (result = parser_read(argv[0], read_handler))
        return result;

    dump_memory();

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
    static const struct option options[] =
        {
            {JOINT_OPTION, 0, "rts", "Select RTS mode: reset - for device RESET, nreset - for inverted device RESET, boot - for device BOOT0 (default), nboot - for inverted device BOOT0, set - stay at high level, clear - stay at low level", select_rts_mode},
            {JOINT_OPTION, 0, "dtr", "Select DTR mode: reset - for device RESET (default), nreset - for inverted device RESET, boot - for device BOOT0, nboot - for inverted device BOOT0, set - stay at high level, clear - stay at low level", select_dtr_mode},
            {JOINT_OPTION, "c", "connect", "Open serial port and connect to device bootloader", connect_device},
            {PLAIN_OPTION, "u", "unprotect", "Erase and read-out unprotect device memory", unprotect_device},
            {JOINT_OPTION, "r", "read", "Read data from device memory to file", read_device},
            {PLAIN_OPTION, "e", "erase", "Erase device memory", erase_device},
            {JOINT_OPTION, "w", "write", "Write data from file to device memory", write_device},
            {PLAIN_OPTION, "p", "protect", "Read-out protect device memory", protect_device},
            {JOINT_OPTION, 0, "trace-time", "Set trace intercharacter interval in seconds (5 default)", set_trace_time},
            {JOINT_OPTION, 0, "trace-size", "Set maximum trace log size (4096 default)", set_trace_size},
            {PLAIN_OPTION, "t", "trace", "Restart device in user mode, with redirecting device output to stdout", trace_device},
            {PLAIN_OPTION, "d", "disconnect", "Disconnect device and close serial port", disconnect_device},
            {USAGE_OPTION, "h", "help", "Print this help", usage_options},
            {OTHER_OPTION}
        };

        static const struct error errors[] =
        {
            {INVALID_FILE_CHECKSUM, "Invalid checksum of file"},
            {INVALID_FILE_CONTENT, "Invalid device memory location or invalid record in file"},
            {UNSUPPORTED_DEVICE, "Unsupported device"},
            {INVALID_DEVICE_REPLY, "Invalid reply from device bootloader"},
            {NO_DEVICE_REPLY, "No reply from device bootloader"},
            {SERIAL_PORT_ALREADY_OPEN, "Serial port already open"},
            {INTERNAL_ERROR, "Internal error"},
            {INVALID_OPTIONS_ARGUMENT, "Invalid actual parameter"},
            {INVALID_OPTION, "Invalid option"},
            {DONE, "No errors, all done"},
        };

        static char stdout_buffer[256];
        setvbuf(stdout, stdout_buffer, _IOLBF, sizeof(stdout_buffer));
        fprintf(stdout, TTY_NONE "Swamp-boot, version 0.%d\n", VERSION);

        return invoke_options(TTY_BOLD "swamp-boot" TTY_NONE " [" TTY_UNLN "OPTIONS" TTY_NONE "] ", options, errors, argc, argv);
}


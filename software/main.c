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
#include "errors.h"

#define VERSION 0
#define MEMORY_SIZE 0x10000
#define PAGE_SIZE 0x40

#define FRAME_HEAD_SIZE (1 + 2 * 2)
#define FRAME_DATA_SIZE (2 * PAGE_SIZE)
#define FRAME_TAIL_SIZE (1)

static uint8_t memory[MEMORY_SIZE];

static int connect_device(const char *file)
{
    int result;

    fprintf(stdout, TTY_NONE "Connect \"%s\"...", file);

    if ((result = open_serial_port(file)))
        return result;

    return DONE;
}

static int read_device_memory(const struct buffer *buffer)
{
    uint32_t address = buffer->origin;
    uint8_t *data = buffer->data;
    size_t size = buffer->size;

    while (size)
    {
        int i;
        int result;
        char frame[FRAME_HEAD_SIZE + FRAME_DATA_SIZE + FRAME_TAIL_SIZE + 1];
        char *ptr = frame;

        ptr += sprintf(ptr, ":%.2X%.2X", address & 0xFF, (address >> 8) & 0xFF);
        ptr += sprintf(ptr, "\n");
        *ptr = 0;

        printf("%s", frame);

       /* if ((result = write_serial_port(frame, FRAME_HEAD_SIZE + FRAME_TAIL_SIZE)))
            return result;

        if ((result = read_serial_port(frame, FRAME_HEAD_SIZE + FRAME_DATA_SIZE + FRAME_TAIL_SIZE)))
            return result;*/

        ptr = frame + 1 + 2 + 2;
        for (i = 0; i < PAGE_SIZE; i++)
            ptr += sscanf(ptr, "%.2X", data[i]);

        size -= PAGE_SIZE;
        data += PAGE_SIZE;
        address += PAGE_SIZE;
    }

    return DONE;
}

static int read_device(const char *file)
{
    int result;
    struct buffer buffer =
    {
        0, 0, MEMORY_SIZE, memory
    };

    fprintf(stdout, TTY_NONE "Reading to \"%s\"...", file);

    if ((result = read_device_memory(&buffer)))
        return result;

    if ((result = save_file_buffer(&buffer, file)))
        return result;

    return DONE;
}

static int write_device_memory(const struct buffer *buffer)
{
    uint32_t address = buffer->origin;
    uint8_t *data = buffer->data;
    size_t size = buffer->size;

    while (size)
    {
        int i;
        int result;
        char frame[FRAME_HEAD_SIZE + FRAME_DATA_SIZE + FRAME_TAIL_SIZE + 1];
        char *ptr = frame;

        ptr += sprintf(ptr, ":%.2X%.2X", address & 0xFF, (address >> 8) & 0xFF);
        for (i = 0; i < PAGE_SIZE; i++)
            ptr += sprintf(ptr, "%.2X", data[i]);
        ptr += sprintf(ptr, "\n");

        *ptr = 0;

        printf("%s", frame);

       /* if ((result = write_serial_port(frame, FRAME_HEAD_SIZE + FRAME_DATA_SIZE + FRAME_TAIL_SIZE)))
            return result;

        if ((result = read_serial_port(frame, FRAME_HEAD_SIZE + FRAME_TAIL_SIZE)))
            return result;*/

        size -= PAGE_SIZE;
        data += PAGE_SIZE;
        address += PAGE_SIZE;
    }

    return DONE;
}

static int write_device(const char *file)
{
    int result;
    struct buffer buffer =
    {
        0, 0, MEMORY_SIZE, memory
    };

    fprintf(stdout, TTY_NONE "Writing from \"%s\"...", file);

    if ((result = load_file_buffer(&buffer, file)))
        return result;

    buffer.origin = 0;
    buffer.size = MEMORY_SIZE;

    if ((result = write_device_memory(&buffer)))
        return result;

    return result;
}

static int disconnect_device(void)
{
    int result;

    fprintf(stdout, TTY_NONE "Disconnecting...");

    if ((result = close_serial_port()))
        return result;

    return DONE;
}

int main(int argc, char* argv[])
{
    static const struct option options[] =
    {
        {JOINT_OPTION, "c", "connect", "Open serial port and connect to device", connect_device},
        {JOINT_OPTION, "r", "read", "Read data from device memory to file", read_device},
        {JOINT_OPTION, "w", "write", "Write data from file to device memory", write_device},
        {PLAIN_OPTION, "d", "disconnect", "Disconnect device and close serial port", disconnect_device},
        {USAGE_OPTION, "h", "help", "Print this help", usage_options},
        {OTHER_OPTION}
    };

    static const struct error errors[] =
    {
        {INVALID_FILE_CHECKSUM, "Invalid checksum of file"},
        {INVALID_FILE_CONTENT, "Invalid device memory location or invalid record in file"},
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
    fprintf(stdout, TTY_NONE "Emrom, version 0.%d\n", VERSION);

    return invoke_options(TTY_BOLD "emrom" TTY_NONE " [" TTY_UNLN "OPTIONS" TTY_NONE "] ", options, errors, argc, argv);
}


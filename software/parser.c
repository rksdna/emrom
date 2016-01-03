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
#include "parser.h"

static int read_record(FILE* file, read_handler_t handler, uint32_t *base)
{
    uint32_t size, address, type, value, checksum;
    if (fscanf(file, ":%2X%4X%2X", &size, &address, &type) != 3)
        return PARSER_IO_ERROR;
    checksum = size + address + (address >> 8) + type;
    switch (type)
    {
    case 0x00:
        while (size--)
        {
            if (fscanf(file, "%2X", &value) != 1)
                return PARSER_IO_ERROR;
            if (handler(*base + address++, value))
                return PARSER_MEMORY_ERROR;
            checksum += value;
        }
        break;
    case 0x01:
        if (size || address)
            return PARSER_RECORD_ERROR;
        break;
    case 0x04:
        if ((size != 0x02) || address)
            return PARSER_RECORD_ERROR;
        if (fscanf(file, "%4X", &value) != 1)
            return PARSER_IO_ERROR;
        *base = value << 16;
        checksum += value + (value >> 8);
        break;
    case 0x05:
        if ((size != 0x04) || address)
            return PARSER_RECORD_ERROR;
        if (fscanf(file, "%8X", &value) != 1)
            return PARSER_IO_ERROR;
        checksum += value + (value >> 8) + (value >> 16) + (value >> 24);
        break;
    default:
        return PARSER_RECORD_ERROR;
    }
    if (fscanf(file, "%2X\n", &value) != 1)
        return PARSER_IO_ERROR;
    if ((checksum + value) & 0xFF)
        return PARSER_CHECKSUM_ERROR;
    return 0;
}

int parser_read(const char *name, read_handler_t handler)
{
    int result;
    uint32_t base = 0;
    FILE *file = fopen(name, "rt");
    if (!file)
        return PARSER_FILE_ERROR;
    while (!feof(file))
        if (result = read_record(file, handler, &base))
            break;
    fclose(file);
    return result;
}

static int write_record(FILE* file, write_handler_t handler, uint32_t address, uint32_t size, uint32_t type, uint32_t base)
{
    uint8_t value;
    uint32_t checksum;
    if (!size && !type)
        return 0;
    if (fprintf(file, ":%02X%04X%02X", size, (uint16_t)address, type) != 9)
        return PARSER_IO_ERROR;
    checksum = size + address + (address >> 8) + type;
    switch (type)
    {
    case 0x00:
        while (size--)
        {
            if (handler(address, &value))
                return PARSER_MEMORY_ERROR;
            if (fprintf(file, "%02X", value) != 2)
                return PARSER_IO_ERROR;
            checksum += value;
            address++;
        }
        break;
    case 0x01:
        if (size || address)
            return PARSER_RECORD_ERROR;
        break;
    case 0x04:
        if ((size != 0x02) || address)
            return PARSER_RECORD_ERROR;
        if (fprintf(file, "%04X", base >> 16) != 4)
            return PARSER_IO_ERROR;
        checksum += (base >> 16) + (base >> 24);
        break;
    default:
        return PARSER_RECORD_ERROR;
    }
    if (fprintf(file, "%02X\n", (-checksum) & 0xFF) != 3)
        return PARSER_IO_ERROR;
    return 0;
}

int parser_write(const char* name, write_handler_t handler, uint32_t address, uint32_t size, int width)
{
    int result;
    uint32_t a = address, b = 0;
    FILE* file = fopen(name, "wt");
    if (!file)
        return PARSER_FILE_ERROR;
    while (size--)
    {
        if ((address ^ b) & 0xFFFF0000)
        {
            if (result = write_record(file, handler, a, address - a, 0x00, 0))
                break;
            a = address;
            b = address & 0xFFFF0000;
            if (result = write_record(file, handler, 0, 2, 0x04, b))
                break;
        }
        if (address >= a + width)
        {
            if (result = write_record(file, handler, a, address - a, 0x00, 0))
                break;
            a = address;
        }
        address++;
    }
    if (!result)
        result = write_record(file, handler, a, address - a, 0x00, b);
    if (!result)
        result = write_record(file, handler, 0, 0, 0x01, b);
    fclose(file);
    return result;
}

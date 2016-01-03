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

#ifndef PARSER_HEADER
#define PARSER_HEADER

#include <stdint.h>

typedef int (*read_handler_t)(uint32_t address, uint8_t value);
typedef int (*write_handler_t)(uint32_t address, uint8_t *value);

enum
{
    PARSER_FILE_ERROR = -20,
    PARSER_IO_ERROR = -21,
    PARSER_CHECKSUM_ERROR = -22,
    PARSER_RECORD_ERROR = -23,
    PARSER_MEMORY_ERROR = -24
};

int parser_read(const char *name, read_handler_t handler);
int parser_write(const char* name, write_handler_t handler, uint32_t address, uint32_t size, int width);

#endif

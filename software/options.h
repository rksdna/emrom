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

#ifndef OPTIONS_HEADER
#define OPTIONS_HEADER

typedef int (*handler_t)(char *argv[]);

typedef struct _option_t
{
    char *name;
    char *alias;
    char *usage;
    int argc;
    handler_t handler;
} option_t;

typedef struct _result_t
{
    int value;
    char *usage;
} result_t;

enum
{
    OPTIONS_ERROR = -1,
    OPTIONS_ARG_ERROR = -2
};

int options_execute(const option_t options[], int argc, char *argv[]);
void options_usage(const option_t options[], const result_t results[], int screen);

#endif

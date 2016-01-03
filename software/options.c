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

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "options.h"

int options_execute(const option_t options[], int argc, char *argv[])
{
    int result;
    const option_t *option;
    argv++; argc--;
    while (argc)
    {
        option = options;
        while (option->name)
        {
            if (!strcmp(option->name, *argv) || !strcmp(option->alias, *argv))
                break;
            option++;
        }
        if (!option->handler)
        {
            printf("INVALID OPTION [%s]\n",  *argv);
            return OPTIONS_ERROR;
        }
        if (--argc < option->argc)
        {
            printf("NOT ENOUGH ACTUAL PARAMETERS [%s]\n", *argv);
            return OPTIONS_ARG_ERROR;
        }
        argv++;
        result = option->handler(argv);
        argv += option->argc;
        argc -= option->argc;
        if (result)
        {
            printf(" FAILED [%d]\n", result);
            return result;
        }
        else
        {
            printf(" done\n");
            fflush(stdout);
        }
    }
    return 0;
}

static int word_wrap(const char *src, int width)
{
    int pos = 0;
    while (src[pos])
    {
        if (!isgraph(src[pos]) && (pos > width))
            break;
        pos++;
    }
    return pos;
}

static void print_option(int width, int screen, const char *name, const char *alias, const char *usage)
{
    int pos = word_wrap(usage, screen - width);
    printf(" %s, %-*s %.*s\n", name, width - (int)strlen(name) - 3, alias, pos, usage);
    usage += pos;
    while (strlen(usage))
    {
        pos = word_wrap(usage, screen - width);
        printf("%-*s%.*s\n", width, "", pos, usage);
        usage += pos;
    }
}

static void print_result(int width, int screen, int value, const char *usage)
{
    int pos = word_wrap(usage, screen - width);
    printf(" %*d %.*s\n", width - 1, value, pos, usage);
    usage += pos;
    while (strlen(usage))
    {
        pos = word_wrap(usage, screen - width);
        printf("%*s%.*s\n", width, "", pos, usage);
        usage += pos;
    }
}

void options_usage(const option_t options[], const result_t results[], int screen)
{
    int width = 0, w;
    const option_t *option = options;
    const result_t *result = results;
    while (option->name)
    {
        w = strlen(option->name) + strlen(option->alias) + 3;
        if (width < w)
            width = w;
        option++;
    }
    printf("Options:\n");
    option = options;
    while (option->name)
    {
        print_option(width, screen, option->name, option->alias, option->usage);
        option++;
    }
    printf("Return values:\n");
    while (result->value)
    {
        print_result(8, screen, result->value, result->usage);
        result++;
    }
    print_result(8, screen, result->value, result->usage);
}


/*

This file (it has no associated documentation) is under the MIT license:

Copyright (c) 2015 Lukas Mai

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __GNUC__
    #define ATTR_UNUSED __attribute__((__unused__))
#else
    #define ATTR_UNUSED
#endif

static unsigned test_counter;

static void plan(unsigned n) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    test_counter = 1;
    printf("1..%u\n", n);
}

ATTR_UNUSED
static int ok(int b, const char *fmt, ...) {
    va_list ap;
    printf("%sok %u - ", b ? "" : "not ", test_counter++);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    return b;
}

ATTR_UNUSED
static void bail_out(const char *s) {
    printf("Bail out! %s\n", s);
    exit(EXIT_FAILURE);
}

ATTR_UNUSED
static void diag(const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, "# ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

ATTR_UNUSED
static void note(const char *fmt, ...) {
    va_list ap;
    printf("# ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

/* vi: set ft=c: */

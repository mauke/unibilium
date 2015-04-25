/* Sample program to demonstrate basic unibilium features. */

/*

This file (it has no associated documentation) is under the MIT license:

Copyright (c) 2012 Lukas Mai

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

#include "unibilium.h"

#include <stdio.h>
#include <ctype.h>

static void print_str_esc(const char *s) {
    if (!s) {
        printf("(null)");
        return;
    }

    for (unsigned char c; (c = *s); s++) {
        switch (c) {
            case '\a': printf("\\a"); break;
            case '\b': printf("\\b"); break;
            case '\f': printf("\\f"); break;
            case '\n': printf("\\n"); break;
            case '\r': printf("\\r"); break;
            case '\t': printf("\\t"); break;
            case '\v': printf("\\v"); break;
            case '\033': printf("\\e"); break;
            default:
                if (isprint(c)) {
                    printf("%c", c);
                } else {
                    printf("\\%03o", c);
                }
                break;
        }
    }
}

int main(void) {
    unibi_term *const ut = unibi_from_env();
    if (!ut) {
        perror("unibi_from_env");
        return 1;
    }

    printf("Terminal name: %s\n", unibi_get_name(ut));
    {
        const char **a = unibi_get_aliases(ut);
        if (*a) {
            printf("Aliases: ");
            do {
                printf("%s%s", *a, a[1] ? " | " : "");
                a++;
            } while (*a);
            printf("\n");
        }
    }
    printf("\n");

    printf("Boolean capabilities:\n");
    for (enum unibi_boolean i = unibi_boolean_begin_ + 1; i < unibi_boolean_end_; i++) {
        if (unibi_get_bool(ut, i)) {
            printf("  %-25s / %s\n", unibi_name_bool(i), unibi_short_name_bool(i));
        }
    }
    printf("\n");

    printf("Numeric capabilities:\n");
    for (enum unibi_numeric i = unibi_numeric_begin_ + 1; i < unibi_numeric_end_; i++) {
        short n = unibi_get_num(ut, i);
        if (n != -1) {
            printf("  %-25s / %-10s = %hd\n", unibi_name_num(i), unibi_short_name_num(i), n);
        }
    }
    printf("\n");

    printf("String capabilities:\n");
    for (enum unibi_string i = unibi_string_begin_ + 1; i < unibi_string_end_; i++) {
        const char *s = unibi_get_str(ut, i);
        if (s) {
            /* Most of these strings will contain escape sequences */
            printf("  %-25s / %-10s = ", unibi_name_str(i), unibi_short_name_str(i));
            print_str_esc(s);
            printf("\n");
        }
    }
    printf("\n");

    if (unibi_count_ext_bool(ut)) {
        printf("Extended boolean capabilities:\n");
        for (size_t i = 0; i < unibi_count_ext_bool(ut); i++) {
            printf("  %-25s = %s\n", unibi_get_ext_bool_name(ut, i), unibi_get_ext_bool(ut, i) ? "true" : "false");
        }
        printf("\n");
    }

    if (unibi_count_ext_num(ut)) {
        printf("Extended numeric capabilities:\n");
        for (size_t i = 0; i < unibi_count_ext_num(ut); i++) {
            printf("  %-25s = %hd\n", unibi_get_ext_num_name(ut, i), unibi_get_ext_num(ut, i));
        }
        printf("\n");
    }

    if (unibi_count_ext_str(ut)) {
        printf("Extended string capabilities:\n");
        for (size_t i = 0; i < unibi_count_ext_str(ut); i++) {
            printf("  %-25s = ", unibi_get_ext_str_name(ut, i));
            print_str_esc(unibi_get_ext_str(ut, i));
            printf("\n");
        }
        printf("\n");
    }

    unibi_destroy(ut);

    return 0;
}

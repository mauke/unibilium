/*

This file (it has no associated documentation) is under the MIT license:

Copyright (c) 2011 Lukas Mai

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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s CAPNAME [PARAM...]\n", argv[0]);
        return 2;
    }

    unibi_term *t = unibi_from_env();
    if (!t) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], getenv("TERM"), strerror(errno));
        return 3;
    }

    if (strcmp(argv[1], "longname") == 0) {
        printf("%s\n", unibi_get_name(t));
        return 0;
    }

    for (enum unibi_boolean i = unibi_boolean_begin_ + 1; i < unibi_boolean_end_; ++i) {
        if (
            strcmp(argv[1], unibi_name_bool(i)) == 0 ||
            strcmp(argv[1], unibi_short_name_bool(i)) == 0
        ) {
            if (unibi_get_bool(t, i)) {
                return 0;
            } else {
                return 1;
            }
        }
    }

    for (enum unibi_numeric i = unibi_numeric_begin_ + 1; i < unibi_numeric_end_; ++i) {
        if (
            strcmp(argv[1], unibi_name_num(i)) == 0 ||
            strcmp(argv[1], unibi_short_name_num(i)) == 0
        ) {
            printf("%hd\n", unibi_get_num(t, i));
            return 0;
        }
    }

    for (enum unibi_string i = unibi_string_begin_ + 1; i < unibi_string_end_; ++i) {
        if (
            strcmp(argv[1], unibi_name_str(i)) == 0 ||
            strcmp(argv[1], unibi_short_name_str(i)) == 0
        ) {
            const char *fmt = unibi_get_str(t, i);
            if (!fmt) {
                return 1;
            }
            if (argc < 3) {
                fputs(fmt, stdout);
                return 0;
            }
            unibi_var_t vars[9] = {{0}};
            switch (i) {
#define REQ(N) do { if (argc - 2 < (N)) { fprintf(stderr, "%s: %s: missing argument\n", argv[0], argv[1]); return 4; } } while (0)

                case unibi_dial_phone:
                case unibi_quick_dial:
                    REQ(1);
                    vars[0].p = argv[2];
                    break;

                case unibi_pkey_key:
                case unibi_pkey_local:
                case unibi_pkey_xmit:
                case unibi_plab_norm:
                    REQ(2);
                    vars[0].i = atoi(argv[2]);
                    vars[1].p = argv[3];
                    break;

                case unibi_pkey_plab:
                    REQ(3);
                    vars[0].i = atoi(argv[2]);
                    vars[1].p = argv[3];
                    vars[2].p = argv[4];
                    break;

                default: {
                    for (int k = 0; k + 2 < argc; ++k) {
                        vars[k].i = atoi(argv[k + 2]);
                    }
                    break;

#undef REQ
                }
            }
            char buf[1024];
            size_t r = unibi_run(fmt, vars, buf, sizeof buf);
            fwrite(buf, 1, r, stdout);
            return 0;
        }
    }

    fprintf(stderr, "%s: %s: unknown terminfo capability\n", argv[0], argv[1]);
    return 5;
}

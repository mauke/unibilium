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

#include <unibilium.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static void say(const char *s) {
    puts(s);
}

static void say_(const char *s) {
    fputs(s, stdout);
}

static void print_str_esc(const char *s, int twice) {
    for (unsigned char c; (c = *s); s++) {
        if (isprint(c)) {
            putchar(c);
        } else {
            printf("%s\\%03o", twice ? "\\" : "", (unsigned)c);
        }
    }
}

int main(void) {
    unibi_term *ut = unibi_from_fp(stdin);
    if (!ut) {
        perror("unibi_from_fp(stdin)");
        return EXIT_FAILURE;
    }

    char buf[4096];
    size_t r = unibi_dump(ut, buf, sizeof buf);
    if (r > sizeof buf) {
        perror("unibi_dump()");
        return EXIT_FAILURE;
    }

    unsigned test_counter = 0;

    say("#include <unibilium.h>");
    say("#include <stdio.h>");
    say("#include <errno.h>");
    say("#include <string.h>");
    say("#include <stdlib.h>");
    say("");
    say("static unsigned test_counter;");
    say("");
    say("static void plan(unsigned n) {");
    say("    printf(\"1..%u\\n\", n);");
    say("}");
    say("");
    say("static void ok(int b, const char *s) {");
    say("    printf(\"%sok %u - %s\\n\", b ? \"\" : \"not \", test_counter++, s);");
    say("}");
    say("");
    say("static void bail_out(const char *s) {");
    say("    printf(\"Bail out! %s\\n\", s);");
    say("    exit(EXIT_FAILURE);");
    say("}");
    say("");
    say("static void diag(const char *s) {");
    say("    printf(\"# %s\\n\", s);");
    say("}");
    say("");
    say_("const char terminfo[] = {");
    for (size_t i = 0; i < r; i++) {
        if (i) {
            printf(",");
        }
        printf("%s", i % 20 ? " " : "\n    ");
        printf("%d", (int)buf[i]);
    }
    say("\n};");
    say("");
    say("static void setup(void);");
    say("");
    say("int main(void) {");
    say("    setup();");
    say("");
    say("    int e;");
    say("    unibi_term *dt = unibi_dummy();");
    say("    e = errno;");
    say("    ok(dt != NULL, \"dummy constructed\");");
    test_counter++;
    say("    if (!dt) {");
    say("        bail_out(strerror(e));");
    say("    }");
    say("    unibi_term *ut = unibi_from_mem(terminfo, sizeof terminfo);");
    say("    e = errno;");
    say("    ok(ut != NULL, \"terminfo loaded\");");
    test_counter++;
    say("    if (!ut) {");
    say("        bail_out(strerror(e));");
    say("    }");
    say("");
    say("    diag(\"terminal name\");");
    {
        const char *name = unibi_get_name(ut);
        printf("    ok(strcmp(unibi_get_name(ut), \"");
        print_str_esc(name, 0);
        printf("\") == 0, \"terminal name = \\\"");
        print_str_esc(name, 1);
        printf("\\\"\");\n");
        test_counter++;
        printf("    unibi_set_name(dt, \"");
        print_str_esc(name, 0);
        printf("\");\n");
    }
    {
        size_t i;
        const char **aliases = unibi_get_aliases(ut);
        say("    {");
        say_("        static const char *def_aliases[] = {");
        for (i = 0; aliases[i]; i++) {
            if (i) {
                say_(", ");
            }
            say_("\"");
            print_str_esc(aliases[i], 0);
            say_("\"");
        }
        if (i) {
            say_(", ");
        }
        say("NULL};");
        say("        const char **aliases = unibi_get_aliases(ut);");
        for (i = 0; aliases[i]; i++) {
            printf("        ok(strcmp(aliases[%zu], def_aliases[%zu]) == 0, \"terminal alias #%zu = \\\"", i, i, i);
            print_str_esc(aliases[i], 0);
            printf("\\\"\");\n");
            test_counter++;
        }
        printf("        ok(aliases[%zu] == NULL, \"terminal alias #%zu = null\");\n", i, i);
        test_counter++;
        say("        unibi_set_aliases(dt, def_aliases);");
        say("    }");
    }
    say("");
    say("    diag(\"boolean capabilities\");");
    for (enum unibi_boolean i = unibi_boolean_begin_ + 1; i < unibi_boolean_end_; i++) {
        int b = unibi_get_bool(ut, i);
        const char *c = unibi_name_bool(i);
        printf("    ok(unibi_get_bool(ut, unibi_%s) == %d, \"%s = %s\");\n", c, b, c, b ? "true" : "false");
        test_counter++;
        if (b) {
            printf("    unibi_set_bool(dt, unibi_%s, 1);\n", c);
        }
    }
    say("");
    say("    diag(\"numeric capabilities\");");
    for (enum unibi_numeric i = unibi_numeric_begin_ + 1; i < unibi_numeric_end_; i++) {
        short v = unibi_get_num(ut, i);
        const char *c = unibi_name_num(i);
        printf("    ok(unibi_get_num(ut, unibi_%s) == %hd, \"%s = %hd\");\n", c, v, c, v);
        test_counter++;
        if (v >= 0) {
            printf("    unibi_set_num(dt, unibi_%s, %hd);\n", c, v);
        }
    }
    say("");
    say("    diag(\"string capabilities\");");
    for (enum unibi_string i = unibi_string_begin_ + 1; i < unibi_string_end_; i++) {
        const char *s = unibi_get_str(ut, i);
        const char *c = unibi_name_str(i);
        if (s) {
            printf("    ok(strcmp(unibi_get_str(ut, unibi_%s), \"", c);
            print_str_esc(s, 0);
            printf("\") == 0, \"%s = \\\"", c);
            print_str_esc(s, 1);
            printf("\\\"\");\n");
            printf("    unibi_set_str(dt, unibi_%s, \"", c);
            print_str_esc(s, 0);
            printf("\");\n");
        } else {
            printf("    ok(unibi_get_str(ut, unibi_%s) == NULL, \"%s = null\");\n", c, c);
        }
        test_counter++;
    }
    say("");
    say("    diag(\"extended boolean capabilities\");");
    {
        const size_t n_ext = unibi_count_ext_bool(ut);

        say("    {");
        say("        const size_t n_ext = unibi_count_ext_bool(ut);");
        printf("        ok(n_ext == %zu, \"#ext_bool = %zu\");\n", n_ext, n_ext);
        test_counter++;

        for (size_t i = 0; i < n_ext; i++) {
            int b = unibi_get_ext_bool(ut, i);
            const char *c = unibi_get_ext_bool_name(ut, i);
            printf("        ok(%zu < n_ext && unibi_get_ext_bool(ut, %zu) == %d, \"ext_bool[%zu].value = %d\");\n", i, i, b, i, b);
            test_counter++;
            printf("        ok(%zu < n_ext && strcmp(unibi_get_ext_bool_name(ut, %zu), \"", i, i);
            test_counter++;
            print_str_esc(c, 0);
            printf("\") == 0, \"ext_bool[%zu].name = \\\"", i);
            print_str_esc(c, 1);
            printf("\\\"\");\n");
            printf("        unibi_add_ext_bool(dt, \"");
            print_str_esc(c, 0);
            printf("\", %d);\n", b);
        }
        say("    }");
    }
    say("");
    say("    diag(\"extended numeric capabilities\");");
    {
        const size_t n_ext = unibi_count_ext_num(ut);

        say("    {");
        say("        const size_t n_ext = unibi_count_ext_num(ut);");
        printf("        ok(n_ext == %zu, \"#ext_num = %zu\");\n", n_ext, n_ext);
        test_counter++;

        for (size_t i = 0; i < n_ext; i++) {
            short v = unibi_get_ext_num(ut, i);
            const char *c = unibi_get_ext_num_name(ut, i);
            printf("        ok(%zu < n_ext && unibi_get_ext_num(ut, %zu) == %hd, \"ext_num[%zu].value = %hd\");\n", i, i, v, i, v);
            test_counter++;
            printf("        ok(%zu < n_ext && strcmp(unibi_get_ext_num_name(ut, %zu), \"", i, i);
            test_counter++;
            print_str_esc(c, 0);
            printf("\") == 0, \"ext_num[%zu].name = \\\"", i);
            print_str_esc(c, 1);
            printf("\\\"\");\n");
            printf("        unibi_add_ext_num(dt, \"");
            print_str_esc(c, 0);
            printf("\", %hd);\n", v);
        }
        say("    }");
    }
    say("");
    say("    diag(\"extended string capabilities\");");
    {
        const size_t n_ext = unibi_count_ext_str(ut);

        say("    {");
        say("        const size_t n_ext = unibi_count_ext_str(ut);");
        printf("        ok(n_ext == %zu, \"#ext_str = %zu\");\n", n_ext, n_ext);
        test_counter++;

        for (size_t i = 0; i < n_ext; i++) {
            const char *s = unibi_get_ext_str(ut, i);
            const char *c = unibi_get_ext_str_name(ut, i);
            printf("        ok(%zu < n_ext && ", i);
            if (s) {
                printf("strcmp(unibi_get_ext_str(ut, %zu), \"", i);
                print_str_esc(s, 0);
                printf("\") == 0, \"ext_str[%zu].value = \\\"", i);
                print_str_esc(s, 1);
                printf("\\\"\");\n");
                printf("        unibi_add_ext_str(dt, \"");
                print_str_esc(c, 0);
                printf("\", \"");
                print_str_esc(s, 0);
                printf("\");\n");
            } else {
                printf("unibi_get_ext_str(ut, %zu) == NULL, \"ext_str[%zu].value = null\");\n", i, i);
                printf("        unibi_add_ext_str(dt, \"");
                print_str_esc(c, 0);
                printf("\", NULL);\n");
            }
            test_counter++;
            printf("        ok(%zu < n_ext && strcmp(unibi_get_ext_str_name(ut, %zu), \"", i, i);
            test_counter++;
            print_str_esc(c, 0);
            printf("\") == 0, \"ext_str[%zu].name = \\\"", i);
            print_str_esc(c, 1);
            printf("\\\"\");\n");
        }
        say("    }");
    }
    say("");
    say("    {");
    say("        char buf[sizeof terminfo];");
    say("        size_t r = unibi_dump(ut, buf, sizeof buf);");
    say("        ok(r == sizeof terminfo, \"redump size == orig size\");");
    test_counter++;
    say("        ok(memcmp(terminfo, buf, sizeof buf) == 0, \"redump == orig\");");
    test_counter++;
    say("    }");
    say("");
    say("    {");
    say("        char buf[sizeof terminfo];");
    say("        size_t r = unibi_dump(dt, buf, sizeof buf);");
    say("        ok(r == sizeof terminfo, \"dummy redump size == orig size\");");
    test_counter++;
    say("        ok(memcmp(terminfo, buf, sizeof buf) == 0, \"dummy redump == orig\");");
    test_counter++;
    say("    }");
    say("");
    say("    unibi_destroy(ut);");
    say("    ok(1, \"object destroyed\");");
    test_counter++;
    say("");
    say("    unibi_destroy(dt);");
    say("    ok(1, \"dummy object destroyed\");");
    test_counter++;
    say("");
    say("    return 0;");
    say("}");

    unibi_destroy(ut);

    say("");
    say("static void setup(void) {");
    say("    setvbuf(stdout, NULL, _IOLBF, 0);");
    say("");
    printf("    plan(%u);\n", test_counter);
    say("    test_counter = 1;");
    say("}");

    return 0;
}

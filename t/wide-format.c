#include <unibilium.h>
#include <string.h>
#include "test-simple.c.inc"

static size_t smin(size_t a, size_t b) {
    return a < b ? a : b;
}

static void is_zu(size_t a, size_t b, const char *d, const char *file, unsigned line) {
    if (ok(a == b, "%s", d)) {
        return;
    }
    fputc('\n', stderr);
    diag("  Failed test '%s'", d);
    diag("  at %s line %u.", file, line);
    diag("         got: %zu", a);
    diag("    expected: %zu", b);
}
#define is_zu(A, B, D) is_zu(A, B, D, __FILE__, __LINE__)

static unibi_term *test_term(void) {
    static const char *aliases[] = { "test", NULL };
    unibi_term *const t = unibi_dummy();
    unibi_set_name(t, "not a real terminal");
    unibi_set_aliases(t, aliases);
    return t;
}

#define SIZE_ERR ((size_t)-1)

int main(void) {
    unibi_term *t;
    char buf[4096];
    size_t n;

    plan(8);

    t = test_term();

    ok(unibi_add_ext_num(t, "Ext1", -1) != SIZE_ERR, "add Ext1");
    ok(unibi_add_ext_num(t, "Ext2", 256) != SIZE_ERR, "add Ext2");

    unibi_set_num(t, unibi_columns, 123);
    unibi_set_num(t, unibi_lines, 4444);

    unibi_set_str(t, unibi_bell, "BONG!");
    unibi_set_str(t, unibi_cursor_up, "yo");

    ok(unibi_add_ext_str(t, "wow", "such string") != SIZE_ERR, "add ext string");

    n = unibi_dump(t, buf, sizeof buf);
    is_zu(n, 142, "simple dump length");
    ok(memcmp(buf, "\032\001\031\000\000\000\003\000\024\000\t\000test|not a real terminal\000\000{\000\377\377\\\021\377\377\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\006\000BONG!\000yo\000\000\000\000\002\000\001\000\004\000\032\000\377\377\000\001\000\000\000\000\005\000\n\000such string\000Ext1\000Ext2\000wow\000", smin(n, 142)) == 0, "simple dump contents");

    is_zu(unibi_add_ext_num(t, "Ext3", 0x10000), 2, "add Ext3");

    n = unibi_dump(t, buf, sizeof buf);
    is_zu(n, 163, "wide dump length");
    ok(memcmp(buf, "\036\002\031\000\000\000\003\000\024\000\t\000test|not a real terminal\000\000{\000\000\000\377\377\377\377\\\021\000\000\377\377\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\006\000BONG!\000yo\000\000\000\000\003\000\001\000\005\000\037\000\377\377\377\377\000\001\000\000\000\000\001\000\000\000\000\000\005\000\n\000\017\000such string\000Ext1\000Ext2\000Ext3\000wow\000", smin(n, 163)) == 0, "wide dump contents");

    unibi_destroy(t);

    return 0;
}

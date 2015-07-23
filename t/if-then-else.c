#include <unibilium.h>
#include <string.h>
#include "test-simple.c.inc"

struct wlog {
    size_t used;
    char buf[1024];
};

static size_t smin(size_t a, size_t b) {
    return a < b ? a : b;
}

static void is_zu(size_t a, size_t b, const char *d, const char *file, unsigned line) {
    if (ok(a == b, "%s length", d)) {
        return;
    }
    fputc('\n', stderr);
    diag("  Failed test '%s length'", d);
    diag("  at %s line %u.", file, line);
    diag("         got: %zu", a);
    diag("    expected: %zu", b);
}
#define is_zu(A, B, D) is_zu(A, B, D, __FILE__, __LINE__)

static void is_ms(const char *pa, size_t na, const char *pb, const char *d, const char *file, unsigned line) {
    const size_t nb = strlen(pb), n = smin(na, nb);
    (is_zu)(na, nb, d, file, line);
    if (ok(memcmp(pa, pb, n) == 0, "%s contents", d)) {
        return;
    }
    fputc('\n', stderr);
    diag("  Failed test '%s contents'", d);
    diag("  at %s line %u.", file, line);
    diag("         got: \"%.*s\"", (int)na, pa);
    diag("    expected: \"%s\"", pb);
}
#define is_ms(AP, AN, BP, D) is_ms(AP, AN, BP, D, __FILE__, __LINE__)

static void out(void *ctx, const char *p, size_t n) {
    struct wlog *wlog = ctx;
    size_t m = smin(n, sizeof wlog->buf - wlog->used);
    memcpy(wlog->buf + wlog->used, p, m);
    wlog->used += m;
}

int main(void) {
    unibi_var_t var_dyn   [26] = {0};
    unibi_var_t var_static[26] = {0};
    unibi_var_t param[9] = {0};

    struct wlog wlog;

    plan(8);

    wlog.used = 0;
    unibi_format(
        var_dyn, var_static,
        "A"
        "%?%{0}%t"
            "B"
            "%?%{0}%t"
                "C"
            "%e"
                "D"
            "%;"
            "E"
        "%;"
        "F"
        ,
        param,
        out, &wlog,
        NULL, NULL
    );
    is_ms(wlog.buf, wlog.used, "AF", "nested then/if");

    wlog.used = 0;
    unibi_format(
        var_dyn, var_static,
        "A"
        "%?%{0}%t"
            "B"
            "%?%{0}%t"
                "C"
            "%e"
                "D"
            "%;"
            "E"
        "%e"
            "F"
            "%?%{1}%t"
                "G"
            "%e"
                "H"
            "%;"
            "I"
        "%;"
        "J"
        ,
        param,
        out, &wlog,
        NULL, NULL
    );
    is_ms(wlog.buf, wlog.used, "AFGIJ", "nested then/if/else");

    wlog.used = 0;
    unibi_format(
        var_dyn, var_static,
        "A"
        "%?%{0}%t"
            "B"
        "%e%{0}%t"
            "%?%{0}%t"
                "C"
            "%e"
                "D"
            "%;"
            "E"
        "%e%{42}%t"
            "F"
            "%?%{0}%t"
                "G"
            "%e"
                "H"
            "%;"
            "I"
        "%e"
            "J"
        "%;"
        "K"
        ,
        param,
        out, &wlog,
        NULL, NULL
    );
    is_ms(wlog.buf, wlog.used, "AFHIK", "else-if");

    param[0] = unibi_var_from_num(26);
    param[1] = unibi_var_from_str((char *)"ducks");
    wlog.used = 0;
    unibi_format(
        var_dyn, var_static,
        "<ESC>[0;"
        "%?%p1%{11}%<%t"
            "%{58}"
        "%e"
            "%?%p1%{13}%<%t"
                "%{122}"
            "%e"
                "%?%p1%{23}%<%t"
                    "%{71}"
                "%e"
                    "%?%p1%{25}%<%t"
                        "%{112}"
                    "%e"
                        "%?%p1%{35}%<%t"
                            "%{69}"
                        "%e"
                            "%?%p1%{37}%<%t"
                                "%{102}"
                            "%e"
                                "%?%p1%{47}%<%t"
                                    "%{67}"
                                "%e"
                                    "%{92}"
                                "%;"
                            "%;"
                        "%;"
                    "%;"
                "%;"
            "%;"
        "%;"
        "%p1%+%d;"
        "%p2\"%s\"p"
        ,
        param,
        out, &wlog,
        NULL, NULL
    );
    is_ms(wlog.buf, wlog.used, "<ESC>[0;95;\"ducks\"p", "ansi-pfkey");

    return 0;
}

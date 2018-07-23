/*

Copyright 2008, 2010, 2012, 2013, 2015 Lukas Mai.

This file is part of unibilium.

Unibilium is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Unibilium is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with unibilium.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "unibilium.h"

#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define ASSERT_RETURN(COND, VAL) do { \
    assert(COND); \
    if (!(COND)) return VAL; \
} while (0)

#define ASSERT_RETURN_(COND) ASSERT_RETURN(COND, )

#define COUNTOF(a) (sizeof (a) / sizeof *(a))

#define NCONTAINERS(n, csize) (((n) - 1) / (csize) + 1u)

#define SIZE_ERR ((size_t)-1)

#define MAX15BITS 0x7fff
#define MAX31BITS 0x7fffffff

#if INT_MAX < MAX31BITS
#error "int must be at least 32 bits wide"
#endif

#define DYNARR(W, X) DynArr_ ## W ## _ ## X
#define DYNARR_T(W) DYNARR(W, t)
#define DEFDYNARRAY(T, W) \
    typedef struct { T (*data); size_t used, size; } DYNARR_T(W); \
    static void DYNARR(W, init)(DYNARR_T(W) *const d) { \
        d->data = NULL; \
        d->used = d->size = 0; \
    } \
    static void DYNARR(W, free)(DYNARR_T(W) *const d) { \
        free(d->data); \
        DYNARR(W, init)(d); \
    } \
    static int DYNARR(W, ensure_slots)(DYNARR_T(W) *const d, const size_t n) { \
        size_t k = d->size; \
        while (d->used + n > k) { \
            k = next_alloc(k); \
        } \
        if (k > d->size) { \
            T (*const p) = realloc(d->data, k * sizeof *p); \
            if (!p) { \
                return 0; \
            } \
            d->data = p; \
            d->size = k; \
        } \
        return 1; \
    } \
    static int DYNARR(W, ensure_slot)(DYNARR_T(W) *const d) { \
        return DYNARR(W, ensure_slots)(d, 1); \
    } \
    static void DYNARR(W, init)(DYNARR_T(W) *)

static size_t next_alloc(size_t n) {
    return n * 3 / 2 + 5;
}

DEFDYNARRAY(unsigned char, bool);
DEFDYNARRAY(int, num);
DEFDYNARRAY(const char *, str);


enum {
    MAGIC_16BIT = 00432,
    MAGIC_32BIT = 01036
};

struct unibi_term {
    const char *name;
    const char **aliases;

    unsigned char bools[NCONTAINERS(unibi_boolean_end_ - unibi_boolean_begin_ - 1, CHAR_BIT)];
    int nums[unibi_numeric_end_ - unibi_numeric_begin_ - 1];
    const char *strs[unibi_string_end_ - unibi_string_begin_ - 1];
    char *alloc;

    DYNARR_T(bool) ext_bools;
    DYNARR_T(num) ext_nums;
    DYNARR_T(str) ext_strs;
    DYNARR_T(str) ext_names;
    char *ext_alloc;
};

#define ASSERT_EXT_NAMES(X) assert((X)->ext_names.used == (X)->ext_bools.used + (X)->ext_nums.used + (X)->ext_strs.used)


static unsigned short get_ushort16(const char *p) {
    const unsigned char *q = (const unsigned char *)p;
    return q[0] + q[1] * 256;
}

static short get_short16(const char *p) {
    unsigned short n = get_ushort16(p);
    return n <= MAX15BITS ? n : -1;
}

static unsigned int get_uint32(const char *p) {
    const unsigned char *q = (const unsigned char *)p;
    return q[0] + q[1] * 256u + q[2] * 256u * 256u + q[3] * 256u * 256u * 256u;
}

static int get_int32(const char *p) {
    unsigned int n = get_uint32(p);
    return n <= MAX31BITS ? (int)n : -1;
}

static void fill_1(int *p, size_t n) {
    while (n--) {
        *p++ = -1;
    }
}

static void fill_null(const char **p, size_t n) {
    while (n--) {
        *p++ = NULL;
    }
}

static const char *off_of(const char *p, size_t n, short i) {
    return i < 0 || (size_t)i >= n ? NULL : p + i;
}

unibi_term *unibi_dummy(void) {
    unibi_term *t;
    void *mem;

    if (!(t = malloc(sizeof *t))) {
        return NULL;
    }
    if (!(mem = malloc(2 * sizeof *t->aliases))) {
        free(t);
        return NULL;
    }
    t->alloc = mem;
    t->aliases = mem;
    t->name = "unibilium dummy terminal";
    t->aliases[0] = "null";
    t->aliases[1] = NULL;
    memset(t->bools, '\0', sizeof t->bools);
    fill_1(t->nums, COUNTOF(t->nums));
    fill_null(t->strs, COUNTOF(t->strs));

    DYNARR(bool, init)(&t->ext_bools);
    DYNARR(num, init)(&t->ext_nums);
    DYNARR(str, init)(&t->ext_strs);
    DYNARR(str, init)(&t->ext_names);
    t->ext_alloc = NULL;

    ASSERT_EXT_NAMES(t);

    return t;
}

static size_t mcount(const char *p, size_t n, char c) {
    size_t r = 0;
    while (n--) {
        if (*p++ == c) {
            r++;
        }
    }
    return r;
}

static size_t size_max(size_t a, size_t b) {
    return a >= b ? a : b;
}

#define FAIL_IF_(c, e, f) do { if (c) { f; errno = (e); return NULL; } } while (0)
#define FAIL_IF(c, e) FAIL_IF_(c, e, (void)0)
#define DEL_FAIL_IF(c, e, x) FAIL_IF_(c, e, unibi_destroy(x))

unibi_term *unibi_from_mem(const char *p, size_t n) {
    unibi_term *t = NULL;
    size_t numsize;
    unsigned short magic, namlen, boollen, numlen, strslen, tablsz;
    char *strp, *namp;
    size_t namco;
    size_t i;

    FAIL_IF(n < 12, EFAULT);

    magic   = get_ushort16(p + 0);
    FAIL_IF(magic != MAGIC_16BIT && magic != MAGIC_32BIT, EINVAL);
    numsize = magic == MAGIC_16BIT ? 2 : 4;

    namlen  = get_ushort16(p + 2);
    boollen = get_ushort16(p + 4);
    numlen  = get_ushort16(p + 6);
    strslen = get_ushort16(p + 8);
    tablsz  = get_ushort16(p + 10);
    p += 12;
    n -= 12;

    FAIL_IF(n < namlen, EFAULT);

    namco = mcount(p, namlen, '|') + 1;

    if (!(t = malloc(sizeof *t))) {
        return NULL;
    }
    {
        void *mem;
        if (!(mem = malloc(namco * sizeof *t->aliases + tablsz + namlen + 1))) {
            free(t);
            return NULL;
        }
        t->alloc = mem;
        t->aliases = mem;
    }
    strp = t->alloc + namco * sizeof *t->aliases;
    namp = strp + tablsz;
    memcpy(namp, p, namlen);
    namp[namlen] = '\0';
    p += namlen;
    n -= namlen;

    {
        size_t k = 0;
        char *a, *z;
        a = namp;

        while ((z = strchr(a, '|'))) {
            *z = '\0';
            t->aliases[k++] = a;
            a = z + 1;
        }
        assert(k < namco);
        t->aliases[k] = NULL;

        t->name = a;
    }

    DYNARR(bool, init)(&t->ext_bools);
    DYNARR(num, init)(&t->ext_nums);
    DYNARR(str, init)(&t->ext_strs);
    DYNARR(str, init)(&t->ext_names);
    t->ext_alloc = NULL;

    DEL_FAIL_IF(n < boollen, EFAULT, t);
    memset(t->bools, '\0', sizeof t->bools);
    for (i = 0; i < boollen && i / CHAR_BIT < COUNTOF(t->bools); i++) {
        if (p[i]) {
            t->bools[i / CHAR_BIT] |= 1 << i % CHAR_BIT;
        }
    }
    p += boollen;
    n -= boollen;

    if ((namlen + boollen) % 2 && n > 0) {
        p += 1;
        n -= 1;
    }

    DEL_FAIL_IF(n < numlen * numsize, EFAULT, t);
    for (i = 0; i < numlen && i < COUNTOF(t->nums); i++) {
        if (numsize == 2) {
            t->nums[i] = get_short16(p + i * 2);
        } else {
            t->nums[i] = get_int32(p + i * 4);
        }
    }
    fill_1(t->nums + i, COUNTOF(t->nums) - i);
    p += numlen * numsize;
    n -= numlen * numsize;

    DEL_FAIL_IF(n < strslen * 2u, EFAULT, t);
    for (i = 0; i < strslen && i < COUNTOF(t->strs); i++) {
        t->strs[i] = off_of(strp, tablsz, get_short16(p + i * 2));
    }
    fill_null(t->strs + i, COUNTOF(t->strs) - i);
    p += strslen * 2;
    n -= strslen * 2;

    DEL_FAIL_IF(n < tablsz, EFAULT, t);
    memcpy(strp, p, tablsz);
    if (tablsz) {
        strp[tablsz - 1] = '\0';
    }
    p += tablsz;
    n -= tablsz;

    if (tablsz % 2 && n > 0) {
        p += 1;
        n -= 1;
    }

    if (n >= 10) {
        unsigned short extboollen, extnumlen, extstrslen, extofflen, exttablsz;
        size_t extalllen;

        extboollen = get_ushort16(p + 0);
        extnumlen  = get_ushort16(p + 2);
        extstrslen = get_ushort16(p + 4);
        extofflen  = get_ushort16(p + 6);
        exttablsz  = get_ushort16(p + 8);

        if (
            extboollen <= MAX15BITS &&
            extnumlen <= MAX15BITS &&
            extstrslen <= MAX15BITS &&
            extofflen <= MAX15BITS &&
            exttablsz <= MAX15BITS
        ) {
            p += 10;
            n -= 10;

            extalllen = 0;
            extalllen += extboollen;
            extalllen += extnumlen;
            extalllen += extstrslen;

            DEL_FAIL_IF(
                n <
                extboollen +
                extboollen % 2 +
                extnumlen * numsize +
                extstrslen * 2 +
                extalllen * 2 +
                exttablsz,
                EFAULT,
                t
            );

            DEL_FAIL_IF(
                !DYNARR(bool, ensure_slots)(&t->ext_bools, extboollen) ||
                !DYNARR(num, ensure_slots)(&t->ext_nums, extnumlen) ||
                !DYNARR(str, ensure_slots)(&t->ext_strs, extstrslen) ||
                !DYNARR(str, ensure_slots)(&t->ext_names, extalllen) ||
                (exttablsz && !(t->ext_alloc = malloc(exttablsz))),
                ENOMEM,
                t
            );

            for (i = 0; i < extboollen; i++) {
                t->ext_bools.data[i] = !!p[i];
            }
            t->ext_bools.used = extboollen;
            p += extboollen;
            n -= extboollen;

            if (extboollen % 2) {
                p += 1;
                n -= 1;
            }

            for (i = 0; i < extnumlen; i++) {
                if (numsize == 2) {
                    t->ext_nums.data[i] = get_short16(p + i * 2);
                } else {
                    t->ext_nums.data[i] = get_int32(p + i * 4);
                }
            }
            t->ext_nums.used = extnumlen;
            p += extnumlen * numsize;
            n -= extnumlen * numsize;

            {
                char *ext_alloc2;
                size_t tblsz2;
                const char *const tbl1 = p + extstrslen * 2 + extalllen * 2;
                size_t s_max = 0, s_sum = 0;

                for (i = 0; i < extstrslen; i++) {
                    const short v = get_short16(p + i * 2);
                    if (v < 0 || (unsigned short)v >= exttablsz) {
                        t->ext_strs.data[i] = NULL;
                    } else {
                        const char *start = tbl1 + v;
                        const char *end = memchr(start, '\0', exttablsz - v);
                        if (end) {
                            end++;
                        } else {
                            end = tbl1 + exttablsz;
                        }
                        s_sum += end - start;
                        s_max = size_max(s_max, end - tbl1);
                        t->ext_strs.data[i] = t->ext_alloc + v;
                    }
                }
                t->ext_strs.used = extstrslen;
                p += extstrslen * 2;
                n -= extstrslen * 2;

                DEL_FAIL_IF(s_max != s_sum, EINVAL, t);

                ext_alloc2 = t->ext_alloc + s_sum;
                tblsz2 = exttablsz - s_sum;

                for (i = 0; i < extalllen; i++) {
                    const short v = get_short16(p + i * 2);
                    DEL_FAIL_IF(v < 0 || (unsigned short)v >= tblsz2, EINVAL, t);
                    t->ext_names.data[i] = ext_alloc2 + v;
                }
                t->ext_names.used = extalllen;
                p += extalllen * 2;
                n -= extalllen * 2;

                assert(p == tbl1);

                if (exttablsz) {
                    memcpy(t->ext_alloc, p, exttablsz);
                    t->ext_alloc[exttablsz - 1] = '\0';

                    p += exttablsz;
                    n -= exttablsz;
                }
            }
        }
    }

    ASSERT_EXT_NAMES(t);

    return t;
}

#undef FAIL_IF
#undef FAIL_IF_
#undef DEL_FAIL_IF

void unibi_destroy(unibi_term *t) {
    DYNARR(bool, free)(&t->ext_bools);
    DYNARR(num, free)(&t->ext_nums);
    DYNARR(str, free)(&t->ext_strs);
    DYNARR(str, free)(&t->ext_names);
    free(t->ext_alloc);
    t->ext_alloc = (char *)">_>";

    t->aliases = NULL;
    free(t->alloc);
    t->alloc = (char *)":-O";
    free(t);
}

static void put_ushort16(char *p, unsigned short n) {
    unsigned char *q = (unsigned char *)p;
    q[0] = n % 256;
    q[1] = n / 256 % 256;
}

static void put_short16(char *p, short n) {
#if MAX15BITS < SHRT_MAX
    assert(n <= MAX15BITS);
#endif
    put_ushort16(
        p,
        n < 0
#if MAX15BITS < SHRT_MAX
        || n > MAX15BITS
#endif
        ? 0xffffU
        : (unsigned short)n
    );
}

static void put_uint32(char *p, unsigned int n) {
    unsigned char *q = (unsigned char *)p;
    q[0] = n % 256u;
    q[1] = n / 256u % 256u;
    q[2] = n / (256u * 256u) % 256u;
    q[3] = n / (256u * 256u * 256u) % 256u;
}

static void put_int32(char *p, int n) {
#if MAX31BITS < INT_MAX
    assert(n <= MAX31BITS);
#endif
    put_uint32(
        p,
        n < 0
#if MAX31BITS < INT_MAX
        || n > MAX31BITS
#endif
        ? 0xffffffffU
        : (unsigned int)n
    );
}

#define FAIL_INVAL_IF(c) if (c) { errno = EINVAL; return SIZE_ERR; } else (void)0

size_t unibi_dump(const unibi_term *t, char *ptr, size_t n) {
    size_t req, i;
    size_t namlen, boollen, numlen, strslen, tablsz;
    size_t ext_count, ext_tablsz1, ext_tablsz2;
    char *p;

    ASSERT_EXT_NAMES(t);

    p = ptr;

    req = 2 + 5 * 2;

    namlen = strlen(t->name) + 1;
    for (i = 0; t->aliases[i]; i++) {
        namlen += strlen(t->aliases[i]) + 1;
    }
    req += namlen;

    for (i = unibi_boolean_end_ - unibi_boolean_begin_ - 1; i--; ) {
        if (t->bools[i / CHAR_BIT] >> i % CHAR_BIT & 1) {
            break;
        }
    }
    i++;
    boollen = i;
    req += boollen;

    if (req % 2) {
        req += 1;
    }

    size_t numsize = 2;
    for (i = COUNTOF(t->nums); i--; ) {
        if (t->nums[i] >= 0) {
            break;
        }
    }
    i++;
    numlen = i;
    while (i--) {
        if (t->nums[i] > MAX15BITS) {
            FAIL_INVAL_IF(t->nums[i] > MAX31BITS);
            numsize = 4;
        }
    }
    req += numlen * numsize;

    for (i = COUNTOF(t->strs); i--; ) {
        if (t->strs[i]) {
            break;
        }
    }
    i++;
    strslen = i;
    req += strslen * 2;

    tablsz = 0;
    while (i--) {
        if (t->strs[i]) {
            tablsz += strlen(t->strs[i]) + 1;
        }
    }
    req += tablsz;

    FAIL_INVAL_IF(tablsz > MAX15BITS);

    FAIL_INVAL_IF(t->ext_bools.used > MAX15BITS);
    FAIL_INVAL_IF(t->ext_nums.used > MAX15BITS);
    FAIL_INVAL_IF(t->ext_strs.used > MAX15BITS);

    ext_tablsz1 = ext_tablsz2 = 0;

    ext_count = t->ext_bools.used + t->ext_nums.used + t->ext_strs.used;
    assert(ext_count == t->ext_names.used);

    if (ext_count) {
        if (req % 2) {
            req += 1;
        }

        req += 5 * 2;

        req += t->ext_bools.used;
        if (req % 2) {
            req += 1;
        }

        if (numsize == 2) {
            for (i = 0; i < t->ext_nums.used; i++) {
                if (t->ext_nums.data[i] > MAX15BITS) {
                    FAIL_INVAL_IF(t->ext_nums.data[i] > MAX31BITS);
                    numsize = 4;
                }
            }
            if (numsize == 4) {
                req += numlen * 2;
            }
        }
        req += t->ext_nums.used * numsize;

        req += t->ext_strs.used * 2;

        req += ext_count * 2;

        for (i = 0; i < t->ext_strs.used; i++) {
            if (t->ext_strs.data[i]) {
                ext_tablsz1 += strlen(t->ext_strs.data[i]) + 1;
            }
        }
        FAIL_INVAL_IF(ext_tablsz1 > MAX15BITS);
        req += ext_tablsz1;

        for (i = 0; i < t->ext_names.used; i++) {
            ext_tablsz2 += strlen(t->ext_names.data[i]) + 1;
        }
        FAIL_INVAL_IF(ext_tablsz2 > MAX15BITS);
        req += ext_tablsz2;

        FAIL_INVAL_IF(ext_tablsz1 + ext_tablsz2 > MAX15BITS);
    }

    if (req > n) {
        errno = EFAULT;
        return req;
    }

    put_ushort16(p + 0, numsize == 2 ? MAGIC_16BIT : MAGIC_32BIT);
    put_ushort16(p + 2, namlen);
    put_ushort16(p + 4, boollen);
    put_ushort16(p + 6, numlen);
    put_ushort16(p + 8, strslen);
    put_ushort16(p + 10, tablsz);
    p += 12;

    for (i = 0; t->aliases[i]; i++) {
        size_t k = strlen(t->aliases[i]);
        memcpy(p, t->aliases[i], k);
        p += k;
        *p++ = '|';
    }
    {
        size_t k = strlen(t->name) + 1;
        memcpy(p, t->name, k);
        p += k;
    }

    for (i = 0; i < boollen; i++) {
        *p++ = t->bools[i / CHAR_BIT] >> i % CHAR_BIT & 1;
    }

    if ((namlen + boollen) % 2) {
        *p++ = '\0';
    }

    for (i = 0; i < numlen; i++) {
        if (numsize == 2) {
            put_short16(p, t->nums[i]);
            p += 2;
        } else {
            put_int32(p, t->nums[i]);
            p += 4;
        }
    }

    {
        char *const tbl = p + strslen * 2;
        size_t off = 0;

        for (i = 0; i < strslen; i++) {
            if (!t->strs[i]) {
                put_short16(p, -1);
                p += 2;
            } else {
                size_t k = strlen(t->strs[i]) + 1;
                assert(off < MAX15BITS);
                put_short16(p, (short)off);
                p += 2;
                memcpy(tbl + off, t->strs[i], k);
                off += k;
            }
        }

        assert(off == tablsz);
        assert(p == tbl);

        p += off;
    }

    if (ext_count) {
        if ((p - ptr) % 2) {
            *p++ = '\0';
        }

        put_ushort16(p + 0, t->ext_bools.used);
        put_ushort16(p + 2, t->ext_nums.used);
        put_ushort16(p + 4, t->ext_strs.used);
        put_ushort16(p + 6, t->ext_strs.used + ext_count);
        put_ushort16(p + 8, ext_tablsz1 + ext_tablsz2);
        p += 10;

        if (t->ext_bools.used) {
            memcpy(p, t->ext_bools.data, t->ext_bools.used);
            p += t->ext_bools.used;
        }

        if (t->ext_bools.used % 2) {
            *p++ = '\0';
        }

        for (i = 0; i < t->ext_nums.used; i++) {
            if (numsize == 2) {
                put_short16(p, t->ext_nums.data[i]);
                p += 2;
            } else {
                put_int32(p, t->ext_nums.data[i]);
                p += 4;
            }
        }

        {
            char *const tbl1 = p + (t->ext_strs.used + ext_count) * 2;
            char *const tbl2 = tbl1 + ext_tablsz1;
            size_t off = 0;

            for (i = 0; i < t->ext_strs.used; i++) {
                const char *const s = t->ext_strs.data[i];
                if (!s) {
                    put_short16(p, -1);
                } else {
                    const size_t k = strlen(s) + 1;
                    assert(off < MAX15BITS);
                    put_ushort16(p, off);
                    memcpy(tbl1 + off, s, k);
                    off += k;
                }
                p += 2;
            }

            assert(off == ext_tablsz1);
            assert(p + ext_count * 2 == tbl1);

            off = 0;
            for (i = 0; i < t->ext_names.used; i++) {
                const char *const s = t->ext_names.data[i];
                const size_t k = strlen(s) + 1;
                assert(off < MAX15BITS);
                put_ushort16(p, off);
                p += 2;
                memcpy(tbl2 + off, s, k);
                off += k;
            }

            assert(off == ext_tablsz2);
            assert(p == tbl1);
            p += ext_tablsz1 + ext_tablsz2;
        }
    }

    assert((size_t)(p - ptr) == req);

    return req;
}

const char *unibi_get_name(const unibi_term *t) {
    return t->name;
}

void unibi_set_name(unibi_term *t, const char *s) {
    t->name = s;
}

const char **unibi_get_aliases(const unibi_term *t) {
    return t->aliases;
}

void unibi_set_aliases(unibi_term *t, const char **a) {
    t->aliases = a;
}

int unibi_get_bool(const unibi_term *t, enum unibi_boolean v) {
    size_t i;
    ASSERT_RETURN(v > unibi_boolean_begin_ && v < unibi_boolean_end_, -1);
    i = v - unibi_boolean_begin_ - 1;
    return t->bools[i / CHAR_BIT] >> i % CHAR_BIT & 1;
}

void unibi_set_bool(unibi_term *t, enum unibi_boolean v, int x) {
    size_t i;
    ASSERT_RETURN_(v > unibi_boolean_begin_ && v < unibi_boolean_end_);
    i = v - unibi_boolean_begin_ - 1;
    if (x) {
        t->bools[i / CHAR_BIT] |= 1 << i % CHAR_BIT;
    } else {
        t->bools[i / CHAR_BIT] &= ~(1 << i % CHAR_BIT);
    }
}

int unibi_get_num(const unibi_term *t, enum unibi_numeric v) {
    size_t i;
    ASSERT_RETURN(v > unibi_numeric_begin_ && v < unibi_numeric_end_, -2);
    i = v - unibi_numeric_begin_ - 1;
    return t->nums[i];
}

void unibi_set_num(unibi_term *t, enum unibi_numeric v, int x) {
    size_t i;
    ASSERT_RETURN_(v > unibi_numeric_begin_ && v < unibi_numeric_end_);
    i = v - unibi_numeric_begin_ - 1;
    t->nums[i] = x;
}

const char *unibi_get_str(const unibi_term *t, enum unibi_string v) {
    size_t i;
    ASSERT_RETURN(v > unibi_string_begin_ && v < unibi_string_end_, NULL);
    i = v - unibi_string_begin_ - 1;
    return t->strs[i];
}

void unibi_set_str(unibi_term *t, enum unibi_string v, const char *x) {
    size_t i;
    ASSERT_RETURN_(v > unibi_string_begin_ && v < unibi_string_end_);
    i = v - unibi_string_begin_ - 1;
    t->strs[i] = x;
}


size_t unibi_count_ext_bool(const unibi_term *t) {
    return t->ext_bools.used;
}

size_t unibi_count_ext_num(const unibi_term *t) {
    return t->ext_nums.used;
}

size_t unibi_count_ext_str(const unibi_term *t) {
    return t->ext_strs.used;
}

int unibi_get_ext_bool(const unibi_term *t, size_t i) {
    ASSERT_RETURN(i < t->ext_bools.used, -1);
    return t->ext_bools.data[i] ? 1 : 0;
}

const char *unibi_get_ext_bool_name(const unibi_term *t, size_t i) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN(i < t->ext_bools.used, NULL);
    return t->ext_names.data[i];
}

int unibi_get_ext_num(const unibi_term *t, size_t i) {
    ASSERT_RETURN(i < t->ext_nums.used, -2);
    return t->ext_nums.data[i];
}

const char *unibi_get_ext_num_name(const unibi_term *t, size_t i) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN(i < t->ext_nums.used, NULL);
    return t->ext_names.data[t->ext_bools.used + i];
}

const char *unibi_get_ext_str(const unibi_term *t, size_t i) {
    ASSERT_RETURN(i < t->ext_strs.used, NULL);
    return t->ext_strs.data[i];
}

const char *unibi_get_ext_str_name(const unibi_term *t, size_t i) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN(i < t->ext_strs.used, NULL);
    return t->ext_names.data[t->ext_bools.used + t->ext_nums.used + i];
}

void unibi_set_ext_bool(unibi_term *t, size_t i, int v) {
    ASSERT_RETURN_(i < t->ext_bools.used);
    t->ext_bools.data[i] = !!v;
}

void unibi_set_ext_bool_name(unibi_term *t, size_t i, const char *c) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN_(i < t->ext_bools.used);
    t->ext_names.data[i] = c;
}

void unibi_set_ext_num(unibi_term *t, size_t i, int v) {
    ASSERT_RETURN_(i < t->ext_nums.used);
    t->ext_nums.data[i] = v;
}

void unibi_set_ext_num_name(unibi_term *t, size_t i, const char *c) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN_(i < t->ext_nums.used);
    t->ext_names.data[t->ext_bools.used + i] = c;
}

void unibi_set_ext_str(unibi_term *t, size_t i, const char *v) {
    ASSERT_RETURN_(i < t->ext_strs.used);
    t->ext_strs.data[i] = v;
}

void unibi_set_ext_str_name(unibi_term *t, size_t i, const char *c) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN_(i < t->ext_strs.used);
    t->ext_names.data[t->ext_bools.used + t->ext_nums.used + i] = c;
}

size_t unibi_add_ext_bool(unibi_term *t, const char *c, int v) {
    size_t r;
    ASSERT_EXT_NAMES(t);
    if (
        !DYNARR(bool, ensure_slot)(&t->ext_bools) ||
        !DYNARR(str, ensure_slot)(&t->ext_names)
    ) {
        return SIZE_ERR;
    }
    {
        const char **const p = t->ext_names.data + t->ext_bools.used;
        memmove(p + 1, p, (t->ext_names.used - t->ext_bools.used) * sizeof *t->ext_names.data);
        *p = c;
        t->ext_names.used++;
    }
    r = t->ext_bools.used++;
    t->ext_bools.data[r] = !!v;
    return r;
}

size_t unibi_add_ext_num(unibi_term *t, const char *c, int v) {
    size_t r;
    ASSERT_EXT_NAMES(t);
    if (
        !DYNARR(num, ensure_slot)(&t->ext_nums) ||
        !DYNARR(str, ensure_slot)(&t->ext_names)
    ) {
        return SIZE_ERR;
    }
    {
        const char **const p = t->ext_names.data + t->ext_bools.used + t->ext_nums.used;
        memmove(p + 1, p, (t->ext_names.used - t->ext_bools.used - t->ext_nums.used) * sizeof *t->ext_names.data);
        *p = c;
        t->ext_names.used++;
    }
    r = t->ext_nums.used++;
    t->ext_nums.data[r] = v;
    return r;
}

size_t unibi_add_ext_str(unibi_term *t, const char *c, const char *v) {
    size_t r;
    ASSERT_EXT_NAMES(t);
    if (
        !DYNARR(str, ensure_slot)(&t->ext_strs) ||
        !DYNARR(str, ensure_slot)(&t->ext_names)
    ) {
        return SIZE_ERR;
    }
    t->ext_names.data[t->ext_names.used++] = c;
    r = t->ext_strs.used++;
    t->ext_strs.data[r] = v;
    return r;
}

void unibi_del_ext_bool(unibi_term *t, size_t i) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN_(i < t->ext_bools.used);
    {
        unsigned char *const p = t->ext_bools.data + i;
        memmove(p, p + 1, (t->ext_bools.used - i - 1) * sizeof *t->ext_bools.data);
        t->ext_bools.used--;
    }
    {
        const char **const p = t->ext_names.data + i;
        memmove(p, p + 1, (t->ext_names.used - i - 1) * sizeof *t->ext_names.data);
        t->ext_names.used--;
    }
}

void unibi_del_ext_num(unibi_term *t, size_t i) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN_(i < t->ext_nums.used);
    {
        int *const p = t->ext_nums.data + i;
        memmove(p, p + 1, (t->ext_nums.used - i - 1) * sizeof *t->ext_nums.data);
        t->ext_nums.used--;
    }
    {
        const char **const p = t->ext_names.data + t->ext_bools.used + i;
        memmove(p, p + 1, (t->ext_names.used - i - 1) * sizeof *t->ext_names.data);
        t->ext_names.used--;
    }
}

void unibi_del_ext_str(unibi_term *t, size_t i) {
    ASSERT_EXT_NAMES(t);
    ASSERT_RETURN_(i < t->ext_strs.used);
    {
        const char **const p = t->ext_strs.data + i;
        memmove(p, p + 1, (t->ext_strs.used - i - 1) * sizeof *t->ext_strs.data);
        t->ext_strs.used--;
    }
    {
        const char **const p = t->ext_names.data + t->ext_bools.used + t->ext_nums.used + i;
        memmove(p, p + 1, (t->ext_names.used - i - 1) * sizeof *t->ext_names.data);
        t->ext_names.used--;
    }
}


unibi_var_t unibi_var_from_num(int i) {
    unibi_var_t v;
    v.p_ = NULL;
    v.i_ = i;
    return v;
}

unibi_var_t unibi_var_from_str(char *p) {
    unibi_var_t v;
    assert(p != NULL);
    v.i_ = INT_MIN;
    v.p_ = p;
    return v;
}

int unibi_num_from_var(unibi_var_t v) {
    return v.p_ ? INT_MIN : v.i_;
}

const char *unibi_str_from_var(unibi_var_t v) {
    return v.p_ ? v.p_ : "";
}

static void dput(
    char t,
    const char *fmt,
    int w,
    int p,
    unibi_var_t x,
    void (*out)(void *, const char *, size_t),
    void *ctx
) {
    char buf[512];
    buf[0] = '\0';

#define BITTY(A, B, C) (!!(A) << 0 | !!(B) << 1 | !!(C) << 2)

    switch (BITTY(t == 's', w != -1, p != -1)) {
        case BITTY(0, 0, 0): snprintf(buf, sizeof buf, fmt,       unibi_num_from_var(x)); break;
        case BITTY(0, 0, 1): snprintf(buf, sizeof buf, fmt,    p, unibi_num_from_var(x)); break;
        case BITTY(0, 1, 0): snprintf(buf, sizeof buf, fmt, w,    unibi_num_from_var(x)); break;
        case BITTY(0, 1, 1): snprintf(buf, sizeof buf, fmt, w, p, unibi_num_from_var(x)); break;
        case BITTY(1, 0, 0): snprintf(buf, sizeof buf, fmt,       unibi_str_from_var(x)); break;
        case BITTY(1, 0, 1): snprintf(buf, sizeof buf, fmt,    p, unibi_str_from_var(x)); break;
        case BITTY(1, 1, 0): snprintf(buf, sizeof buf, fmt, w,    unibi_str_from_var(x)); break;
        case BITTY(1, 1, 1): snprintf(buf, sizeof buf, fmt, w, p, unibi_str_from_var(x)); break;
    }

#undef BITTY

    out(ctx, buf, strlen(buf));
}

static long cstrtol(const char *s, const char **pp) {
    long r;
    char *tmp;
    r = strtol(s, &tmp, 10);
    *pp = tmp;
    return r;
}

void unibi_format(
    unibi_var_t var_dyn[26],
    unibi_var_t var_static[26],
    const char *fmt,
    unibi_var_t param[9],
    void (*out)(void *, const char *, size_t),
    void *ctx1,
    void (*pad)(void *, size_t, int, int),
    void *ctx2
) {
    const unibi_var_t zero = {0};
    unibi_var_t stack[123] = {{0}};
    size_t sp = 0;

#define UC(F, C) (F((unsigned char)(C)))

#define POP() (sp ? stack[--sp] : zero)
#define PUSH(X) do { if (sp < COUNTOF(stack)) { stack[sp++] = (X); } } while (0)
#define PUSHi(N) do { unibi_var_t tmp_ = unibi_var_from_num(N); PUSH(tmp_); } while (0)

    while (*fmt) {
        {
            size_t r = strcspn(fmt, "%$");
            if (r) {
                out(ctx1, fmt, r);
                fmt += r;
                if (!*fmt) {
                    break;
                }
            }
        }

        if (*fmt == '$') {
            ++fmt;
            if (*fmt == '<' && UC(isdigit, fmt[1])) {
                int scale = 0, force = 0;
                const char *v = fmt + 1;
                size_t n = cstrtol(v, &v);
                n *= 10;
                if (*v == '.') {
                    ++v;
                }
                if (UC(isdigit, *v)) {
                    n += *v++ - '0';
                }
                if (*v == '/') {
                    ++v;
                    force = 1;
                    if (*v == '*') {
                        ++v;
                        scale = 1;
                    }
                } else if (*v == '*') {
                    ++v;
                    scale = 1;
                    if (*v == '/') {
                        ++v;
                        force = 1;
                    }
                }
                if (*v == '>') {
                    fmt = v + 1;
                    if (pad) {
                        pad(ctx2, n, scale, force);
                    }
                } else {
                    out(ctx1, fmt - 1, 1);
                }
            } else {
                out(ctx1, fmt - 1, 1);
            }
            continue;
        }

        assert(*fmt == '%');
        ++fmt;

        if (UC(isdigit, *fmt) || (*fmt && strchr(":# .doxX", *fmt))) {
            enum {
                FlagAlt = 1,
                FlagSpc = 2,
                FlagSgn = 4,
                FlagLft = 8,
                FlagZro = 16
            };
            int flags = 0, width = -1, prec = -1;
            const char *v = fmt;
            if (*v == ':') {
                ++v;
            }
            while (1) {
                switch (*v++) {
                    case '#': flags |= FlagAlt; continue;
                    case ' ': flags |= FlagSpc; continue;
                    case '0': flags |= FlagZro; continue;
                    case '+': flags |= FlagSgn; continue;
                    case '-': flags |= FlagLft; continue;
                }
                --v;
                break;
            }
            if (UC(isdigit, *v)) {
                width = cstrtol(v, &v);
            }
            if (*v == '.' && UC(isdigit, v[1])) {
                ++v;
                prec = cstrtol(v, &v);
            }
            if (*v && strchr("doxXs", *v)) {
                char gen[sizeof "%# +-0*.*d"], *g = gen;
                *g++ = '%';
                if (flags & FlagAlt) { *g++ = '#'; }
                if (flags & FlagSpc) { *g++ = ' '; }
                if (flags & FlagSgn) { *g++ = '+'; }
                if (flags & FlagLft) { *g++ = '-'; }
                if (flags & FlagZro) { *g++ = '0'; }
                if (width != -1) { *g++ = '*'; }
                if (prec  != -1) { *g++ = '.'; *g++ = '*'; }
                *g++ = *v;
                *g = '\0';
                dput(*v, gen, width, prec, POP(), out, ctx1);
                fmt = v;
            } else {
                out(ctx1, fmt - 1, 2);
            }
            ++fmt;
            continue;
        }

        switch (*fmt++) {
            default:
                out(ctx1, fmt - 2, 2);
                break;

            case '\0':
                --fmt;
                out(ctx1, "%", 1);
                break;

            case '%':
                out(ctx1, "%", 1);
                break;

            case 'c': {
                unsigned char c;
                c = unibi_num_from_var(POP());
                out(ctx1, (const char *)&c, 1);
                break;
            }

            case 's': {
                const char *s;
                s = unibi_str_from_var(POP());
                out(ctx1, s, strlen(s));
                break;
            }

            case 'p':
                if (*fmt >= '1' && *fmt <= '9') {
                    size_t n = *fmt++ - '1';
                    PUSH(param[n]);
                } else {
                    out(ctx1, fmt - 2, 2);
                }
                break;

            case 'P':
                if (*fmt >= 'a' && *fmt <= 'z') {
                    var_dyn[*fmt - 'a'] = POP();
                    fmt++;
                } else if (*fmt >= 'A' && *fmt <= 'Z') {
                    var_static[*fmt - 'A'] = POP();
                    fmt++;
                } else {
                    out(ctx1, fmt - 2, 2);
                }
                break;

            case 'g':
                if (*fmt >= 'a' && *fmt <= 'z') {
                    PUSH(var_dyn[*fmt - 'a']);
                    fmt++;
                } else if (*fmt >= 'A' && *fmt <= 'Z') {
                    PUSH(var_static[*fmt - 'A']);
                    fmt++;
                } else {
                    out(ctx1, fmt - 2, 2);
                }
                break;

            case '\'':
                if (*fmt && fmt[1] == '\'') {
                    PUSHi((unsigned char)*fmt);
                    fmt += 2;
                } else {
                    out(ctx1, fmt - 2, 2);
                }
                break;

            case '{': {
                size_t r = strspn(fmt, "0123456789");
                if (r && fmt[r] == '}') {
                    PUSHi(atoi(fmt));
                    fmt += r + 1;
                } else {
                    out(ctx1, fmt - 2, 2);
                }
                break;
            }

            case 'l':
                PUSHi(strlen(unibi_str_from_var(POP())));
                break;

            case 'i':
                param[0] = unibi_var_from_num(unibi_num_from_var(param[0]) + 1);
                param[1] = unibi_var_from_num(unibi_num_from_var(param[1]) + 1);
                break;

            case '?':
                break;

            case 't': {
                int c = unibi_num_from_var(POP());
                if (!c) {
                    size_t nesting = 0;
                    for (; *fmt; ++fmt) {
                        if (*fmt == '%') {
                            ++fmt;
                            if (*fmt == '?') {
                                ++nesting;
                            } else if (*fmt == ';') {
                                if (!nesting) {
                                    ++fmt;
                                    break;
                                }
                                --nesting;
                            } else if (*fmt == 'e' && !nesting) {
                                ++fmt;
                                break;
                            } else if (!*fmt) {
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case 'e': {
                size_t nesting = 0;
                for (; *fmt; ++fmt) {
                    if (*fmt == '%') {
                        ++fmt;
                        if (*fmt == '?') {
                            ++nesting;
                        } else if (*fmt == ';') {
                            if (!nesting) {
                                ++fmt;
                                break;
                            }
                            --nesting;
                        } else if (!*fmt) {
                            break;
                        }
                    }
                }
                break;
            }

            case ';':
                break;

#define ARITH2(C, O) \
    case (C): { \
        unibi_var_t x, y; \
        y = POP(); \
        x = POP(); \
        PUSHi(unibi_num_from_var(x) O unibi_num_from_var(y)); \
    } break

            ARITH2('+', +);
            ARITH2('-', -);
            ARITH2('*', *);
            ARITH2('/', /);
            ARITH2('m', %);
            ARITH2('&', &);
            ARITH2('|', |);
            ARITH2('^', ^);
            ARITH2('=', ==);
            ARITH2('<', <);
            ARITH2('>', >);
            ARITH2('A', &&);
            ARITH2('O', ||);

#undef ARITH2

#define ARITH1(C, O) \
    case (C): \
        PUSHi(O unibi_num_from_var(POP())); \
        break

            ARITH1('!', !);
            ARITH1('~', ~);

#undef ARITH1
        }
    }

#undef PUSHi
#undef PUSH
#undef POP

#undef UC
}

typedef struct {
    char *p;
    size_t n, w;
} run_ctx_t;

static size_t xmin(size_t a, size_t b) {
    return a < b ? a : b;
}

static void out(void *vctx, const char *p, size_t n) {
    run_ctx_t *ctx = vctx;
    size_t k = xmin(n, ctx->n);
    ctx->w += n;
    memcpy(ctx->p, p, k);
    ctx->p += k;
    ctx->n -= k;
}

size_t unibi_run(const char *fmt, unibi_var_t param[9], char *p, size_t n) {
    unibi_var_t vars[26 + 26] = {{0}};
    run_ctx_t ctx;

    ctx.p = p;
    ctx.n = n;
    ctx.w = 0;

    unibi_format(vars, vars + 26, fmt, param, out, &ctx, NULL, NULL);
    return ctx.w;
}

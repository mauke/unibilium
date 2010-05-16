/* Copyright 2010, Lukas Mai.
 *
 * This library is under the GNU Lesser General Public License;
 * see the file LGPLv3 for details.
 */

#include "unibilium.h"

#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define COUNTOF(a) (sizeof (a) / sizeof *(a))

#define NCONTAINERS(n, csize) (((n) - 1) / (csize) + 1u)

#define MAX15BITS 0x7fff

enum {MAGIC = 0432};

struct unibi_term {
	const char *name;
	const char **aliases;

	unsigned char bools[NCONTAINERS(unibi_boolean_end_ - unibi_boolean_begin_ - 1, CHAR_BIT)];
	short nums[unibi_numeric_end_ - unibi_numeric_begin_ - 1];
	const char *strs[unibi_string_end_ - unibi_string_begin_ - 1];
	char *alloc;

#if 0
	size_t ext_bsize;
	struct { const char *name; } *ext_bools;
	size_t ext_nsize;
	struct { const char *name; short value; } *ext_nums;
	size_t ext_ssize;
	struct { const char *name; const char *value; } *ext_strs;
	char *ext_alloc;
#endif
};

/* No extended terminfo attributes for me. term(5) says the extended header
 * consists of:
 * |  (1)  count of extended boolean capabilities
 * |  (2)  count of extended numeric capabilities
 * |  (3)  count of extended string capabilities
 * |  (4)  size of the extended string table in bytes.
 * |  (5)  last offset of the extended string table in bytes.
 *
 * I have no idea why (5) is necessary. From this description it seems like (5)
 * should always be (4) - 1. Looking at the ncurses source code I find that it
 * completely ignores (4). However, it treats (5) as the size of the string
 * table. Later on, (5) is used as the size of both an array A and the part of
 * A starting at some offset > 0. This looks obviously broken. In addition, the
 * function responsible for checking these bounds has an off-by-one error.
 * Finally, I don't actually have any terminfo files that use extended
 * capabilities on my system.
 *
 * In conclusion: The docs are vague, the source contradicts the docs (but is
 * buggy), and I have no data files to reverse engineer the format. Until this
 * changes, I have no idea what to do in this library; therefore I simply
 * ignore extended capabilities.
 */

static unsigned short get_ushort(const char *p) {
	const unsigned char *q = (const unsigned char *)p;
	return q[0] + q[1] * 256;
}

static short get_short(const char *p) {
	unsigned short n = get_ushort(p);
	return n <= MAX15BITS ? n : -1;
}

static void fill_1(short *p, size_t n) {
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
	if (!(t = malloc(sizeof *t))) {
		return NULL;
	}
	if (!(t->alloc = malloc(2 * sizeof *t->aliases))) {
		free(t);
		return NULL;
	}
	t->aliases = (const char **)t->alloc;
	t->name = "unibilium dummy terminal";
	t->aliases[0] = "null";
	t->aliases[1] = NULL;
	memset(t->bools, '\0', sizeof t->bools);
	fill_1(t->nums, COUNTOF(t->nums));
	fill_null(t->strs, COUNTOF(t->strs));

#if 0
	t->ext_bsize = 0;
	t->ext_bools = NULL;
	t->ext_nsize = 0;
	t->ext_nums = NULL;
	t->ext_ssize = 0;
	t->ext_strs = NULL;
	t->ext_alloc = NULL;
#endif

	return t;
}

static size_t mcount(const char *p, size_t n, char c) {
	size_t r = 0;
	while (n--) {
		if (*p++ == c) {
			++r;
		}
	}
	return r;
}

#define FAIL_IF_(c, e, f) do { if (c) { f; errno = (e); return NULL; } } while (0)
#define FAIL_IF(c, e) FAIL_IF_(c, e, (void)0)
#define DEL_FAIL_IF(c, e, x) FAIL_IF_(c, e, unibi_destroy(x))
#define SOFT_FAIL_IF(c) if (!(c)) ; else break

unibi_term *unibi_init(const char *p, size_t n) {
	unibi_term *t = NULL;
	unsigned short magic, namlen, boollen, numlen, strslen, tablsz;
	char *strp, *namp;
	size_t namco;
	size_t i;

	FAIL_IF(n < 12, EFAULT);

	magic   = get_ushort(p + 0);
	FAIL_IF(magic != MAGIC, EINVAL);

	namlen  = get_ushort(p + 2);
	boollen = get_ushort(p + 4);
	numlen  = get_ushort(p + 6);
	strslen = get_ushort(p + 8);
	tablsz  = get_ushort(p + 10);
	p += 12;
	n -= 12;

	FAIL_IF(n < namlen, EFAULT);

	namco = mcount(p, namlen, '|') + 1;

	if (!(t = malloc(sizeof *t))) {
		return NULL;
	}
	if (!(t->alloc = malloc(namco * sizeof *t->aliases + tablsz + namlen + 1))) {
		free(t);
		return NULL;
	}
	t->aliases = (const char **)t->alloc;
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

#if 0
	t->ext_bsize = 0;
	t->ext_bools = NULL;
	t->ext_nsize = 0;
	t->ext_nums = NULL;
	t->ext_ssize = 0;
	t->ext_strs = NULL;
	t->ext_alloc = NULL;
#endif

	DEL_FAIL_IF(n < boollen, EFAULT, t);
	memset(t->bools, '\0', sizeof t->bools);
	for (i = 0; i < boollen && i / CHAR_BIT < COUNTOF(t->bools); ++i) {
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

	DEL_FAIL_IF(n < numlen * 2u, EFAULT, t);
	for (i = 0; i < numlen && i < COUNTOF(t->nums); ++i) {
		t->nums[i] = get_short(p + i * 2);
	}
	fill_1(t->nums + i, COUNTOF(t->nums) - i);
	p += numlen * 2;
	n -= numlen * 2;

	DEL_FAIL_IF(n < strslen * 2u, EFAULT, t);
	for (i = 0; i < strslen && i < COUNTOF(t->strs); ++i) {
		t->strs[i] = off_of(strp, tablsz, get_short(p + i * 2));
	}
	fill_null(t->strs + i, COUNTOF(t->strs) - i);
	p += strslen * 2;
	n -= strslen * 2;

	DEL_FAIL_IF(n < tablsz, EFAULT, t);
	memcpy(strp, p, tablsz);
	p += tablsz;
	n -= tablsz;

#if 0
	do {
		unsigned short extboollen, extnumlen, extstrslen, exttablsz, exttablend;

		if (tablsz % 2 && n > 0) {
			p += 1;
			n -= 1;
		}

		SOFT_FAIL_IF(n < 10);
		extboollen = get_ushort(p + 0);
		extnumlen  = get_ushort(p + 2);
		extstrslen = get_ushort(p + 4);
		exttablsz  = get_ushort(p + 6);
		exttablend = get_ushort(p + 8);
		p += 10;
		n -= 10;

		SOFT_FAIL_IF(n < extboollen + extboollen % 2 + extnumlen * 2 + extstrslen * 2);
	} while (0);
#endif

	return t;
}

void unibi_destroy(unibi_term *t) {
#if 0
	free(t->ext_alloc);
	t->ext_alloc = (char *)":-S";
#endif
	t->aliases = NULL;
	free(t->alloc);
	t->alloc = (char *)":-O";
	free(t);
}

static void put_ushort(char *p, unsigned short n) {
	unsigned char *q = (unsigned char *)p;
	q[0] = n % 256;
	q[1] = n / 256 % 256;
}

static void put_short(char *p, short n) {
	put_ushort(
		p,
		n < 0
#if MAX15BITS < SHRT_MAX
		|| n > MAX15BITS
#endif
		? 0xffffU
		: (unsigned short)n
	);
}

size_t unibi_dump(const unibi_term *t, char *ptr, size_t n) {
	size_t req, i;
	size_t namlen, boollen, numlen, strslen, tablsz;
	char *p = ptr;

	req = 12;

	namlen = strlen(t->name) + 1;
	for (i = 0; t->aliases[i]; ++i) {
		namlen += strlen(t->aliases[i]) + 1;
	}
	req += namlen;

	for (i = unibi_boolean_end_ - unibi_boolean_begin_ - 1; i--; ) {
		if (t->bools[i / CHAR_BIT] >> i % CHAR_BIT & 1) {
			break;
		}
	}
	++i;
	boollen = i;
	req += boollen;

	if (req % 2) {
		req += 1;
	}

	for (i = COUNTOF(t->nums); i--; ) {
		if (t->nums[i] >= 0) {
			break;
		}
	}
	++i;
	numlen = i;
	req += numlen * 2;

	for (i = COUNTOF(t->strs); i--; ) {
		if (t->strs[i]) {
			break;
		}
	}
	++i;
	strslen = i;
	req += strslen * 2;

	tablsz = 0;
	while (i--) {
		if (t->strs[i]) {
			tablsz += strlen(t->strs[i]) + 1;
		}
	}
	req += tablsz;

	if (tablsz > MAX15BITS) {
		errno = EINVAL;
		return ~(size_t)0;
	}

	if (req > n) {
		errno = EFAULT;
		return req;
	}

	put_ushort(p + 0, MAGIC);
	put_ushort(p + 2, namlen);
	put_ushort(p + 4, boollen);
	put_ushort(p + 6, numlen);
	put_ushort(p + 8, strslen);
	put_ushort(p + 10, tablsz);
	p += 12;

	for (i = 0; t->aliases[i]; ++i) {
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

	for (i = 0; i < boollen; ++i) {
		*p++ = t->bools[i / UCHAR_MAX] >> i % UCHAR_MAX & 1;
	}

	if ((namlen + boollen) % 2) {
		*p++ = '\0';
	}

	for (i = 0; i < numlen; ++i) {
		put_short(p, t->nums[i]);
		p += 2;
	}

	{
		char *const tbl = p + strslen * 2;
		size_t off = 0;

		for (i = 0; i < strslen; ++i) {
			if (!t->strs[i]) {
				put_short(p, -1);
				p += 2;
			} else {
				size_t k = strlen(t->strs[i]) + 1;
				assert(off < MAX15BITS);
				put_short(p, (short)off);
				p += 2;
				memcpy(tbl + off, t->strs[i], k);
				off += k;
			}
		}

		assert(off == tablsz);
		assert(p == tbl);

		p += off;
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
	assert(v > unibi_boolean_begin_ && v < unibi_boolean_end_);
	if (v <= unibi_boolean_begin_ || v >= unibi_boolean_end_) {
		return -1;
	}
	i = v - unibi_boolean_begin_ - 1;
	return t->bools[i / CHAR_BIT] >> i % CHAR_BIT & 1;
}

void unibi_set_bool(unibi_term *t, enum unibi_boolean v, int x) {
	size_t i;
	assert(v > unibi_boolean_begin_ && v < unibi_boolean_end_);
	if (v <= unibi_boolean_begin_ || v >= unibi_boolean_end_) {
		return;
	}
	i = v - unibi_boolean_begin_ - 1;
	if (x) {
		t->bools[i / CHAR_BIT] |= 1 << i % CHAR_BIT;
	} else {
		t->bools[i / CHAR_BIT] &= ~(1 << i % CHAR_BIT);
	}
}

short unibi_get_num(const unibi_term *t, enum unibi_numeric v) {
	size_t i;
	assert(v > unibi_numeric_begin_ && v < unibi_numeric_end_);
	if (v <= unibi_numeric_begin_ || v >= unibi_numeric_end_) {
		return -2;
	}
	i = v - unibi_numeric_begin_ - 1;
	return t->nums[i];
}

void unibi_set_num(unibi_term *t, enum unibi_numeric v, short x) {
	size_t i;
	assert(v > unibi_numeric_begin_ && v < unibi_numeric_end_);
	if (v <= unibi_numeric_begin_ || v >= unibi_numeric_end_) {
		return;
	}
	i = v - unibi_numeric_begin_ - 1;
	t->nums[i] = x;
}

const char *unibi_get_str(const unibi_term *t, enum unibi_string v) {
	size_t i;
	assert(v > unibi_string_begin_ && v < unibi_string_end_);
	if (v <= unibi_string_begin_ || v >= unibi_string_end_) {
		return NULL;
	}
	i = v - unibi_string_begin_ - 1;
	return t->strs[i];
}

void unibi_set_str(unibi_term *t, enum unibi_string v, const char *x) {
	size_t i;
	assert(v > unibi_string_begin_ && v < unibi_string_end_);
	if (v <= unibi_string_begin_ || v >= unibi_string_end_) {
		return;
	}
	i = v - unibi_string_begin_ - 1;
	t->strs[i] = x;
}


#if 0
int unibi_get_ext_bool(const unibi_term *t, const char *k) {
	size_t i;

	for (i = 0; i < t->ext_bsize && t->ext_bools[i].name; ++i) {
		if (strcmp(t->ext_bools[i].name, k) == 0) {
			return 1;
		}
	}

	return 0;
}

int unibi_set_ext_bool(unibi_term *t, const char *k, int v) {
	size_t i;

	for (i = 0; i < t->ext_bsize && t->ext_bools[i].name; ++i) {
		if (strcmp(t->ext_bools[i].name, k) == 0) {
			if (!v) {
				size_t j;
				for (j = i + 1; j < t->ext_bsize && t->ext_bools[j].name; ++j)
					;
				memmove(t->ext_bools + i, t->ext_bools + i + 1, j - (i + 1));
				t->ext_bools[j - 1].name = NULL;
			}
			return 0;
		}
	}

	if (!v) {
		return 0;
	}

	if (i >= t->ext_bsize) {
		const size_t size = (t->ext_bsize + 1) * 3 / 2;
		void *p = realloc(t->ext_bools, sizeof *t->ext_bools * size);
		if (!p) {
			return -1;
		}
		t->ext_bools = p;
		t->ext_bools[t->ext_bsize].name = NULL;
		t->ext_bsize = size;
	}

	t->ext_bools[i].name = k;
	if (i + 1 < t->ext_bsize) {
		t->ext_bools[i + 1].name = NULL;
	}
	return 0;
}

short unibi_get_ext_num(const unibi_term *t, const char *k) {
	size_t i;

	for (i = 0; i < t->ext_nsize && t->ext_nums[i].name; ++i) {
		if (strcmp(t->ext_nums[i].name, k) == 0) {
			return t->ext_nums[i].value;
		}
	}

	return -1;
}

int unibi_set_ext_num(unibi_term *t, const char *k, short v) {
	size_t i;

	for (i = 0; i < t->ext_nsize && t->ext_nums[i].name; ++i) {
		if (strcmp(t->ext_nums[i].name, k) == 0) {
			t->ext_nums[i].value = v;
			return 0;
		}
	}

	if ((unsigned short)v > MAX15BITS) {
		return 0;
	}

	{
		const size_t size = (t->ext_nsize + 1) * 3 / 2;
		void *p = realloc(t->ext_nums, sizeof *t->ext_nums * size);
		if (!p) {
			return -1;
		}

		t->ext_nums = p;
		t->ext_nums[t->ext_nsize].name = NULL;
		t->ext_nsize = size;
	}

	t->ext_nums[i].name = k;
	t->ext_nums[i].value = v;
	if (i + 1 < t->ext_nsize) {
		t->ext_nums[i + 1].name = NULL;
	}
	return 0;
}

const char *unibi_get_ext_str(const unibi_term *t, const char *k) {
	size_t i;

	for (i = 0; i < t->ext_ssize && t->ext_strs[i].name; ++i) {
		if (strcmp(t->ext_strs[i].name, k) == 0) {
			return t->ext_strs[i].value;
		}
	}

	return NULL;
}

int unibi_set_ext_str(unibi_term *t, const char *k, const char *v) {
	size_t i;

	for (i = 0; i < t->ext_ssize && t->ext_strs[i].name; ++i) {
		if (strcmp(t->ext_strs[i].name, k) == 0) {
			t->ext_strs[i].value = v;
			return 0;
		}
	}

	if (!v) {
		return 0;
	}

	{
		const size_t size = (t->ext_ssize + 1) * 3 / 2;
		void *p = realloc(t->ext_strs, sizeof *t->ext_strs * size);
		if (!p) {
			return -1;
		}

		t->ext_strs = p;
		t->ext_strs[t->ext_ssize].name = NULL;
		t->ext_ssize = size;
	}

	t->ext_strs[i].name = k;
	t->ext_strs[i].value = v;
	if (i + 1 < t->ext_ssize) {
		t->ext_strs[i + 1].name = NULL;
	}
	return 0;
}
#endif

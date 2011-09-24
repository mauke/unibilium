/* Copyright 2011 Lukas Mai.
 *
 * This library is under the GNU Lesser General Public License;
 * see the file LGPLv3 for details.
 */

#include "sparchways.h"

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define UC(F, C) (F((unsigned char)(C)))

#define COUNTOF(A) (sizeof (A) / sizeof *(A))

#define BITTY(A, B, C) (!!(A) << 0 | !!(B)<< 1 | !!(C) << 2)

static void dput(
	char t,
	const char *fmt,
	int w,
	int p,
	sparch_var_t x,
	void (*out)(void *, const char *, size_t),
	void *ctx
) {
	char buf[512];
	buf[0] = '\0';
	switch (BITTY(t == 's', w != -1, p != -1)) {
		case BITTY(0, 0, 0): snprintf(buf, sizeof buf, fmt, x.i); break;
		case BITTY(0, 0, 1): snprintf(buf, sizeof buf, fmt, p, x.i); break;
		case BITTY(0, 1, 0): snprintf(buf, sizeof buf, fmt, w, x.i); break;
		case BITTY(0, 1, 1): snprintf(buf, sizeof buf, fmt, w, p, x.i); break;
		case BITTY(1, 0, 0): snprintf(buf, sizeof buf, fmt, x.p); break;
		case BITTY(1, 0, 1): snprintf(buf, sizeof buf, fmt, p, x.p); break;
		case BITTY(1, 1, 0): snprintf(buf, sizeof buf, fmt, w, x.p); break;
		case BITTY(1, 1, 1): snprintf(buf, sizeof buf, fmt, w, p, x.p); break;
	}
	out(ctx, buf, strlen(buf));
}

static long cstrtol(const char *s, const char **pp) {
	long r;
	char *tmp;
	r = strtol(s, &tmp, 10);
	*pp = tmp;
	return r;
}

void sparch_format(
	sparch_var_t var_dyn[26],
	sparch_var_t var_static[26],
	const char *fmt,
	sparch_var_t param[9],
	void (*out)(void *, const char *, size_t),
	void *ctx1,
	void (*pad)(void *, size_t, int, int),
	void *ctx2
) {
	const sparch_var_t zero = {0};
	sparch_var_t stack[123] = {{0}};
	size_t sp = 0;

#define POP() (sp ? stack[--sp] : zero)
#define PUSH(X) do { if (sp < COUNTOF(stack)) { stack[sp++] = (X); } } while (0)
#define PUSHi(N) do { sparch_var_t tmp_; tmp_.i = (N); PUSH(tmp_); } while (0)

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
				if (prec != -1) { *g++ = '.'; *g++ = '*'; }
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
				c = POP().i;
				out(ctx1, (char *)&c, 1);
				break;
			}

			case 's': {
				char *s;
				s = POP().p;
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
					++fmt;
					var_dyn[*fmt - 'a'] = POP();
				} else if (*fmt >= 'A' && *fmt <= 'Z') {
					++fmt;
					var_static[*fmt - 'A'] = POP();
				} else {
					out(ctx1, fmt - 2, 2);
				}
				break;

			case 'g':
				if (*fmt >= 'a' && *fmt <= 'z') {
					++fmt;
					PUSH(var_dyn[*fmt - 'a']);
				} else if (*fmt >= 'A' && *fmt <= 'Z') {
					++fmt;
					PUSH(var_static[*fmt - 'A']);
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
				PUSHi(strlen(POP().p));
				break;

			case 'i':
				++param[0].i;
				++param[1].i;
				break;

			case '?':
				break;

			case 't': {
				int c = POP().i;
				if (!c) {
					size_t nesting = 0;
					for (; *fmt; ++fmt) {
						if (*fmt == '%') {
							++fmt;
							if (*fmt == '?') {
								++nesting;
							} else if (*fmt == ';' || *fmt == 'e') {
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
		sparch_var_t x, y; \
		y = POP(); \
		x = POP(); \
		PUSHi(x.i O y.i); \
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
		PUSHi(O POP().i); \
		break

			ARITH1('!', !);
			ARITH1('~', ~);

#undef ARITH1
		}
	}

#undef PUSHi
#undef PUSH
#undef POP
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

size_t sparch_run(const char *fmt, sparch_var_t param[9], char *p, size_t n) {
	sparch_var_t vars[26 + 26] = {{0}};
	run_ctx_t ctx;

	ctx.p = p;
	ctx.n = n;
	ctx.w = 0;

	sparch_format(vars, vars + 26, fmt, param, out, &ctx, NULL, NULL);
	return ctx.w;
}

#ifndef GUARD_SPARCHWAYS_H_
#define GUARD_SPARCHWAYS_H_

#include <stddef.h>

typedef union {
	int i;
	char *p;
} sparch_var_t;

void sparch_format(
	sparch_var_t [26],
	sparch_var_t [26],
	const char *,
	sparch_var_t [9],
	void (*)(void *, const char *, size_t),
	void *,
	void (*)(void *, size_t, int, int),
	void *
);

size_t sparch_run(const char *, sparch_var_t [9], char *, size_t);

#endif /* GUARD_SPARCHWAYS_H_ */

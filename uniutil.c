/*

Copyright 2008, 2010 Lukas Mai.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef TERMINFO_DIRS
#error "internal error: TERMINFO_DIRS is not defined"
#endif

enum {MAX_BUF = 4096};

const char *const unibi_terminfo_dirs = TERMINFO_DIRS;

unibi_term *unibi_from_fp(FILE *fp) {
	char buf[MAX_BUF];
	size_t n, r;

	for (n = 0; n < sizeof buf && (r = fread(buf + n, 1, sizeof buf - n, fp)) > 0; ) {
		n += r;

		if (feof(fp)) {
			break;
		}
	}

	if (ferror(fp)) {
		return NULL;
	}

	return unibi_from_mem(buf, n);
}

unibi_term *unibi_from_fd(int fd) {
	char buf[MAX_BUF];
	size_t n;
	ssize_t r;

	for (n = 0; n < sizeof buf && (r = read(fd, buf + n, sizeof buf - n)) > 0; ) {
		n += r;
	}

	if (r < 0) {
		return NULL;
	}

	return unibi_from_mem(buf, n);
}

unibi_term *unibi_from_file(const char *file) {
	int fd;
	unibi_term *ut;

	if ((fd = open(file, O_RDONLY)) < 0) {
		return NULL;
	}

	ut = unibi_from_fd(fd);
	close(fd);
	return ut;
}

static unibi_term *from_dir(const char *base, const char *mid, const char *term) {
	char *path;
	unibi_term *ret;

	path = malloc(strlen(base) + 1 + strlen(mid) + 1 + 1 + 1 + strlen(term) + 1);
	if(!path)
		return NULL;
	sprintf(path,        "%s"   "/"         "%s"  "/" "%c""/"         "%s",
	                     base,              mid,      term[0],        term);
	ret = unibi_from_file(path);
	free(path);
	return ret;
}

static unibi_term *from_dirs(const char *list, const char *term) {
	char *copy, *a, *z;

	if (list[0] == '\0') {
		errno = ENOENT;
		return NULL;
	}

	copy = malloc(strlen(list) + 1);
	if(!copy)
		return NULL;

	strcpy(copy, list);

	a = copy;

	for (;;) {
		unibi_term *ut;

		while (*a == ':') {
			++a;
		}
		if (*a == '\0') {
			break;
		}

		z = strchr(a, ':');
		if (z) {
			*z = '\0';
		}

		ut = from_dir(a, ".", term);
		if (ut) {
			free(copy);
			return ut;
		}

		if (!z) {
			break;
		}
		a = z + 1;
	}

	free(copy);
	errno = ENOENT;
	return NULL;
}

unibi_term *unibi_from_term(const char *term) {
	unibi_term *ut;
	const char *env;

	if (term[0] == '\0' || term[0] == '.' || strchr(term, '/')) {
		errno = EINVAL;
		return NULL;
	}

	if ((env = getenv("TERMINFO"))) {
		return from_dir(env, ".", term);
	}

	if ((env = getenv("HOME"))) {
		ut = from_dir(env, ".terminfo", term);
		if (ut) {
			return ut;
		}
	}

	if ((env = getenv("TERMINFO_DIRS"))) {
		return from_dirs(env, term);
	}

	return from_dirs(unibi_terminfo_dirs, term);
}

unibi_term *unibi_from_env(void) {
	const char *term = getenv("TERM");
	if (!term) {
		errno = ENOENT;
		return NULL;
	}

	return unibi_from_term(term);
}

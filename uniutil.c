/*

Copyright 2008, 2010, 2012 Lukas Mai.

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

static int add_overflowed(size_t *dst, size_t src) {
    *dst += src;
    return *dst < src;
}

static unibi_term *from_dir(const char *dir_begin, const char *dir_end, const char *mid, const char *term) {
    char *path;
    unibi_term *ut;
    size_t dir_len, mid_len, term_len, path_size;

    dir_len = dir_end ? (size_t)(dir_end - dir_begin) : strlen(dir_begin);
    mid_len = mid ? strlen(mid) + 1 : 0;
    term_len = strlen(term);

    path_size = 0;
    if (
        add_overflowed(&path_size, dir_len) ||
        add_overflowed(&path_size, mid_len) ||
        add_overflowed(&path_size, term_len) ||
        add_overflowed(&path_size, 1 + 2           + 1 + 1)
                                /* /   (%c | %02x)   /   \0 */
    ) {
        /* overflow */
        errno = ENOMEM;
        return NULL;
    }
    if (!(path = malloc(path_size))) {
        return NULL;
    }

    memcpy(path, dir_begin, dir_len);
    sprintf(path + dir_len, "/" "%s"            "%s"             "%c" "/" "%s",
                                 mid ? mid : "", mid ? "/" : "",  term[0], term);

    errno = 0;
    ut = unibi_from_file(path);
    if (!ut && errno == ENOENT) {
        /* OS X likes to use /usr/share/terminfo/<hexcode>/name instead of the first letter */
        sprintf(path + dir_len + 1 + mid_len, "%02x/%s",
                                               (unsigned int)((unsigned char)term[0] & 0xff),
                                               term);
        ut = unibi_from_file(path);
    }
    free(path);
    return ut;
}

static unibi_term *from_dirs(const char *list, const char *term) {
    const char *a, *z;

    if (list[0] == '\0') {
        errno = ENOENT;
        return NULL;
    }

    a = list;

    for (;;) {
        unibi_term *ut;

        while (*a == ':') {
            ++a;
        }
        if (*a == '\0') {
            break;
        }

        z = strchr(a, ':');

        ut = from_dir(a, z, NULL, term);
        if (ut) {
            return ut;
        }

        if (!z) {
            break;
        }
        a = z + 1;
    }

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
        return from_dir(env, NULL, NULL, term);
    }

    if ((env = getenv("HOME"))) {
        ut = from_dir(env, NULL, ".terminfo", term);
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

Overview
========

Unibilium is a very basic terminfo library. It can read and write
ncurses-style terminfo files, and it can interpret terminfo format strings.
It doesn't depend on curses or any other library. It also doesn't use global
variables, so it should be thread-safe.


Building and installing
=======================

Prerequisites
-------------

- a C compiler (technically this is all you need, but if you want to use the
  included `Makefile`, read on)
- GNU make (the included `Makefile` was written for GNU make; if it works in
  anything else, it's by accident)
- GNU libtool (for building shared libraries)
- a unix-ish shell (sh, that is)
- Perl (specifically, `pod2man` for creating the man pages and `prove` for
  running the test suite)
- gzip (for compressing the man pages)

Building
--------

There is no configure step. Compile `unibilium.c`, `uninames.c`, and
`uniutil.c` into a library.

The included `Makefile` does this for you:

    make PREFIX=/usr/local

or

    make all PREFIX=/usr/local

creates the library files, generates the man pages, and compiles the test
suite.

There is a number of variables you can specify on the `make` command line to
override the default behavior:

- `TERMINFO_DIRS`: A C string literal containing a colon-separated list of
  directories where unibilium should look for compiled terminfo files at
  runtime (e.g. `make TERMINFO_DIRS='"/etc/terminfo:/usr/share/terminfo"'`).
  This depends on how your ncurses library was configured.
  Defaults to `""` on Windows; everywhere else the local ncurses installation
  is queried (by trying `ncursesw6-config`, `ncurses6-config`,
  `ncursesw5-config`, `ncurses5-config` in turn) and if that fails, a
  hardcoded list of likely directory names is used.
- `PREFIX`: The target directory where files should be installed. This
  variable is only used to set the defaults for `LIBDIR`, `INCDIR`, and
  `MANDIR`, i.e. you don't need to set `PREFIX` if you set the other three
  directly. Defaults to `/usr/local`.
- `LIBDIR`, `INCDIR`, `MANDIR`: Where the resulting library files, header
  files, and man pages should be installed. They default to `${PREFIX}/lib`,
  `${PREFIX}/include`, and `${PREFIX}/share/man`, respectively.
- `LIBTOOL`: The name of the GNU libtool script on your platform. Defaults to
  `glibtool` on Mac OS and `libtool` everywhere else.
- `CC`: The name of the C compiler that should be used (e.g. use
  `make CC=clang` to build with `clang`). Defaults to whatever `make` sets
  `CC` to by default (probably `cc`).
- `CFLAGS`: C compiler (optimization) flags. Defaults to `-O2`.
- `DEBUG`: Specify `make DEBUG=1` to build with debugging information.
  Defaults to unset.
- `CFLAGS_DEBUG`: C compiler debugging flags. Empty by default unless
  `DEBUG=1` is specified, in which case `-ggdb -DDEBUG` is used.

Testing
-------

Run

    make test

to run the included test suite. If that fails, please report it at
https://github.com/mauke/unibilium/issues.

Installing
----------

Run

    make install PREFIX=...

to install the library, header files, man pages, and pkg-config file. Take
care to specify the same `PREFIX`, `LIBDIR`, `INCDIR`, and `MANDIR` settings
you used for building.

`make install` supports one additional variable:

- `DESTDIR`: Acts as an additional prefix for the final installation step. For
  example, if you do
  `make PREFIX=/usr && make install PREFIX=/usr DESTDIR=/tmp`, then the
  library will be configured for installation under `/usr`, but the actual
  files will be copied to `/tmp/usr`. Defaults to empty.


License
=======

See the `LICENSE` file for licensing information.

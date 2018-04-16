ifneq ($(wildcard .maint),)
  include maint.mk
endif

ifeq ($(shell uname),Darwin)
  LIBTOOL?=glibtool
else
  LIBTOOL?=libtool
endif

CFLAGS?=-O2

CFLAGS_DEBUG=

PACKAGE=unibilium

PKG_MAJOR=2
PKG_MINOR=0
PKG_REVISION=0

PKG_VERSION=$(PKG_MAJOR).$(PKG_MINOR).$(PKG_REVISION)

# I am implementation $LT_REVISION of binary interface $LT_CURRENT, which is
# a superset of all interfaces back to $LT_CURRENT - $LT_AGE.
LT_REVISION=0
LT_CURRENT=4
LT_AGE=0

PREFIX?=/usr/local
LIBDIR?=$(PREFIX)/lib
INCDIR?=$(PREFIX)/include
SHAREDIR?=$(PREFIX)/share
MANDIR?=$(SHAREDIR)/man
MAN3DIR?=$(MANDIR)/man3

ifneq ($(OS),Windows_NT)
  TERMINFO_DIRS="$(shell ncursesw6-config --terminfo-dirs 2>/dev/null || \
                         ncurses6-config  --terminfo-dirs 2>/dev/null || \
                         ncursesw5-config --terminfo-dirs 2>/dev/null || \
                         ncurses5-config  --terminfo-dirs 2>/dev/null || \
                         echo "/etc/terminfo:/lib/terminfo:/usr/share/terminfo:/usr/lib/terminfo:/usr/local/share/terminfo:/usr/local/lib/terminfo")"
else
  TERMINFO_DIRS=""
endif

POD2MAN=pod2man
POD2MAN_OPTS=-c "$(PACKAGE)" -s3 -r "$(PACKAGE)-$(PKG_VERSION)"

PROVE=prove
PROVEFLAGS=`perl -we 'print $$ENV{MAKEFLAGS} =~ /-j *(\d+)?/ ? "-j" . ($$1 || 2) : ""'`

ifeq ($(DEBUG),1)
  CFLAGS_DEBUG=-ggdb -DDEBUG
endif

OBJECTS=unibilium.lo uninames.lo uniutil.lo
LIBRARY=libunibilium.la

PODS=$(wildcard doc/*.pod)
MANPAGES=$(addprefix man/,$(notdir $(PODS:.pod=.3.gz)))

TOOLS=$(wildcard tools/*.c)

TESTS=$(wildcard t/*.c)

.PHONY: all
all: $(LIBRARY) build-man build-tools build-test

%.lo: %.c unibilium.h
	$(LIBTOOL) --mode=compile --tag=CC $(CC) -I. -Wall -std=c99 $(CFLAGS) $(CFLAGS_DEBUG) -o $@ -c $<

uniutil.lo: uniutil.c unibilium.h
	$(LIBTOOL) --mode=compile --tag=CC $(CC) -I. -DTERMINFO_DIRS='$(TERMINFO_DIRS)' -Wall -std=c99 $(CFLAGS) $(CFLAGS_DEBUG) -o $@ -c $<

$(LIBRARY): $(OBJECTS)
	$(LIBTOOL) --mode=link --tag=CC $(CC) $(LDFLAGS) -rpath '$(LIBDIR)' -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -o $@ $^

tools/%: $(LIBRARY) tools/%.lo
	$(LIBTOOL) --mode=link --tag=CC $(CC) $(LDFLAGS) -o $@ $^

%.t: $(LIBRARY) %.lo
	$(LIBTOOL) --mode=link --tag=CC $(CC) $(LDFLAGS) -o $@ $^

.PHONY: build-tools
build-tools: $(TOOLS:.c=)

.PHONY: build-test
build-test: $(TESTS:.c=.t)

.PHONY: test
test: build-test
	@echo $(PROVE) $(PROVEFLAGS)
	@$(PROVE) $(PROVEFLAGS)

.PHONY: clean
clean:
	$(LIBTOOL) --mode=clean rm -f $(OBJECTS) $(LIBRARY) $(MANPAGES)
	$(LIBTOOL) --mode=clean rm -f tools/.libs/* t/.libs/*  # XXX there has to be a better way
	$(LIBTOOL) --mode=clean rm -f $(TOOLS:.c=) $(TOOLS:.c=.o) $(TESTS:.c=.t) $(TESTS:.c=.o)

.PHONY: install
install: install-inc install-lib install-man
	$(LIBTOOL) --mode=finish '$(DESTDIR)$(LIBDIR)'

.PHONY: install-inc
install-inc:
	mkdir -p '$(DESTDIR)$(INCDIR)'
	install -m644 unibilium.h '$(DESTDIR)$(INCDIR)'
	mkdir -p '$(DESTDIR)$(LIBDIR)/pkgconfig'
	perl -wpe 'BEGIN { @V{"VERSION", "LIBDIR", "INCDIR"} = splice @ARGV } s/\@(VERSION|LIBDIR|INCDIR)\@/$$V{$$1}/g' '$(PKG_VERSION)' '$(LIBDIR)' '$(INCDIR)' <unibilium.pc.in >'$(DESTDIR)$(LIBDIR)/pkgconfig/unibilium.pc'

.PHONY: install-lib
install-lib:
	mkdir -p '$(DESTDIR)$(LIBDIR)'
	$(LIBTOOL) --mode=install cp $(LIBRARY) '$(DESTDIR)$(LIBDIR)/$(LIBRARY)'

.PHONY: install-man
install-man: build-man
	mkdir -p '$(DESTDIR)$(MAN3DIR)'
	install -m644 $(MANPAGES) '$(DESTDIR)$(MAN3DIR)'

.PHONY: build-man
build-man: $(MANPAGES)

man/%.3.gz: doc/%.pod
	$(POD2MAN) $(POD2MAN_OPTS) $< | gzip > $@


.PHONY: regenerate-tests
regenerate-tests: tools/gen-static-test
	for t in screen-256color screen tmux xterm-256color xterm; do \
	    $< $$t > t/static_$$t.c; \
	done

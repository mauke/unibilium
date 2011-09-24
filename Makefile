LIBTOOL=libtool

CFLAGS?=

CFLAGS_DEBUG=

PACKAGE=unibilium

PKG_MAJOR=0
PKG_MINOR=1
PKG_REVISION=0

PKG_VERSION=$(PKG_MAJOR).$(PKG_MINOR).$(PKG_REVISION)

# I am implementation $LT_REVISION of binary interface $LT_CURRENT, which is
# a superset of all interfaces back to $LT_CURRENT - $LT_AGE.
LT_REVISION=0
LT_CURRENT=0
LT_AGE=0

PREFIX=/usr/local
LIBDIR=$(PREFIX)/lib
INCDIR=$(PREFIX)/include
MANDIR=$(PREFIX)/share/man
MAN3DIR=$(MANDIR)/man3

TERMINFO_DIRS="/etc/terminfo:/lib/terminfo:/usr/share/terminfo:/usr/lib/terminfo:/usr/local/share/terminfo:/usr/local/lib/terminfo"

POD2MAN=pod2man
POD2MAN_OPTS=-c "$(PACKAGE)" -s3 -r "$(PACKAGE)-$(PKG_VERSION)"

ifeq ($(DEBUG),1)
  CFLAGS_DEBUG=-ggdb -DDEBUG
endif

OBJECTS=unibilium.lo uninames.lo uniutil.lo
LIBRARY=libunibilium.la

PODS=$(wildcard doc/*.pod)
MANPAGES=$(addprefix man/,$(notdir $(PODS:.pod=.3.gz)))

.PHONY: all
all: $(LIBRARY) build-man unibi-dump

%.lo: %.c unibilium.h
	$(LIBTOOL) --mode=compile --tag=CC gcc $(CFLAGS) $(CFLAGS_DEBUG) -Wall -std=c99 -o $@ -c $<

uniutil.lo: uniutil.c unibilium.h
	$(LIBTOOL) --mode=compile --tag=CC gcc -DTERMINFO_DIRS='$(TERMINFO_DIRS)' $(CFLAGS) $(CFLAGS_DEBUG) -Wall -std=c99 -o $@ -c $<

$(LIBRARY): $(OBJECTS)
	$(LIBTOOL) --mode=link --tag=CC gcc -rpath '$(LIBDIR)' -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -o $@ $^

unibi-dump: $(LIBRARY) unibi-dump.lo
	$(LIBTOOL) --mode=link --tag=CC gcc -o $@ $^

.PHONY: clean
clean:
	$(LIBTOOL) --mode=clean rm -f $(OBJECTS) $(LIBRARY) $(MANPAGES)
	$(LIBTOOL) --mode=clean rm -f unibi-dump unibi-dump.lo

.PHONY: install
install: install-inc install-lib install-man
	$(LIBTOOL) --mode=finish '$(DESTDIR)$(LIBDIR)'

.PHONY: install-inc
install-inc:
	install -d '$(DESTDIR)$(INCDIR)'
	install -m644 unibilium.h '$(DESTDIR)$(INCDIR)'
	install -d '$(DESTDIR)$(LIBDIR)/pkgconfig'
	sed 's,@VERSION@,$(PKG_VERSION),;s,@LIBDIR@,$(LIBDIR),;s,@INCDIR@,$(INCDIR),' <unibilium.pc.in >'$(DESTDIR)$(LIBDIR)/pkgconfig/unibilium.pc'

.PHONY: install-lib
install-lib:
	install -d '$(DESTDIR)$(LIBDIR)'
	$(LIBTOOL) --mode=install cp $(LIBRARY) '$(DESTDIR)$(LIBDIR)/$(LIBRARY)'

.PHONY: install-man
install-man: build-man
	install -d '$(DESTDIR)$(MAN3DIR)'
	install -m644 $(MANPAGES) '$(DESTDIR)$(MAN3DIR)'

.PHONY: build-man
build-man: $(MANPAGES)

man/%.3.gz: doc/%.pod
	$(POD2MAN) $(POD2MAN_OPTS) $< | gzip > $@

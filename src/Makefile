CC ?= gcc
AR ?= ar
CFLAGS ?= -O2 -fPIC
CPPFLAGS ?=
LDFLAGS ?=
PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include
BINDIR ?= $(PREFIX)/bin

LIB_NAME = libzhello
STATIC_LIB = $(LIB_NAME).a
SHARED_LIB = $(LIB_NAME).so
CLI = zhello

all: $(STATIC_LIB) $(SHARED_LIB) $(CLI)

libzhello.o: libzhello.c libzhello.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c libzhello.c -o $@

zhello.o: zhello.c libzhello.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c zhello.c -o $@

$(STATIC_LIB): libzhello.o
	$(AR) rcs $@ $^

$(SHARED_LIB): libzhello.o
	$(CC) -shared $(LDFLAGS) -o $@ $^ -lz

$(CLI): zhello.o $(STATIC_LIB)
	$(CC) $(LDFLAGS) -o $@ zhello.o $(STATIC_LIB) -lz

install: all
	mkdir -p $(BINDIR) $(LIBDIR) $(INCLUDEDIR)
	install -m 0755 $(CLI) $(BINDIR)/$(CLI)
	install -m 0644 $(STATIC_LIB) $(LIBDIR)/$(STATIC_LIB)
	install -m 0755 $(SHARED_LIB) $(LIBDIR)/$(SHARED_LIB)
	install -m 0644 libzhello.h $(INCLUDEDIR)/libzhello.h

clean:
	rm -f *.o $(STATIC_LIB) $(SHARED_LIB) $(CLI)

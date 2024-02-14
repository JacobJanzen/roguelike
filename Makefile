CC=clang
SRCDIR=src
BUILDDIR=build
TARGET=$(BUILDDIR)/main

INCLUDE_PATHS=-Iinclude
LDFLAGS=-lcurses

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

all: $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) $(INCLUDE_PATHS) $^ -o $@

$(BUILDDIR)/main.o: main.c
	mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) $(INCLUDE_PATHS) $^ -o $@

$(TARGET): $(OBJS) $(BUILDDIR)/main.o
	mkdir -p $(BUILDDIR)
	$(CC) -o $@ $(LDFLAGS) $^

clean:
	rm -rf $(BUILDDIR)

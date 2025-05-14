CC = cc
CFLAGS = -g -Iinclude -Wall -std=c99 -pedantic

libswftag_objs = \
	lib/intermediate/cf_implementations.o \
	lib/intermediate/check_functions.o \
	lib/intermediate/decompression.o \
	lib/intermediate/primary.o \
	lib/intermediate/swfmath.o \

all: lib/libswftag.a lib/optional/error.o

lib/libswftag.a: $(libswftag_objs)
	ar rcs $@ $^

lib/intermediate/default/error.o: | lib/intermediate/default

lib/intermediate/%.o: src/%.c | lib/intermediate
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -c $< -o $@

lib/optional/error.o: lib/intermediate/default/error.o | lib/optional
	cp $< $@

clean:
	-rm -r lib

lib/intermediate/default:
	mkdir -p $@

lib/intermediate:
	mkdir -p $@

lib/optional:
	mkdir -p $@

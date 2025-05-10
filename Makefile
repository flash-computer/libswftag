CC = cc
CFLAGS = -O2 -g -Iinclude -Wall -std=c99 -pedantic

libswftag_objs = \
	lib/intermediate/cf_implementations.o \
	lib/intermediate/check_functions.o \
	lib/intermediate/decompression.o \
	lib/intermediate/primary.o \
	lib/intermediate/swfmath.o \

all: lib/libswftag.a lib/optional/error.o

lib/libswftag.a: $(libswftag_objs)
	ar rcs $@ $^

lib/optional/error.o: src/default/error.o lib/optional
	cp $< $@

lib/intermediate/%.o: src/%.c lib/intermediate
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f libswftag.a error.o $(libswftag_objs)

lib/intermediate:
	mkdir -p $@

lib/optional:
	mkdir -p $@

lib:
	mkdir $@

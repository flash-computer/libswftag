CC = cc
CFLAGS = -O2 -g -Iinclude -Wall -std=c99 -pedantic

libswftag_objs = \
	src/cf_implementations.o \
	src/check_functions.o \
	src/decompression.o \
	src/primary.o \
	src/swfmath.o \

all: libswftag.a error.o

libswftag.a: $(libswftag_objs)
	ar rcs $@ $^

error.o: src/default/error.o
	cp $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f libswftag.a error.o $(libswftag_objs)

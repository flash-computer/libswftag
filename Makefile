options := -Wall -std=c99
header_files := ./Headers/swftag.h ./Headers/tag_structs.h ./Headers/error.h ./Headers/decompression.h ./Headers/swfmath.h ./Headers/check_functions.h

all: ./build/lib/optional/def_error.o ./build/lib/libswftag.a
	echo ALL DONE!

./build/lib/optional/def_error.o: ./Implementations/Default/error.c ./build/include ./build/lib/optional
	gcc $(options) -c $< -o $@
./build/lib/optional: ./build/lib
	mkdir -p $@

./build/lib/libswftag.a: ./build/obj/check_functions.o ./build/obj/implementations.o ./build/obj/primary.o ./build/obj/decompression.o ./build/obj/swfmath.o ./build/lib
	if [ -f '$@' ]; then rm -f '$@'; fi
	ar rcs libswftag.a ./build/obj/check_functions.o ./build/obj/implementations.o ./build/obj/primary.o ./build/obj/decompression.o ./build/obj/swfmath.o
	mv libswftag.a ./build/lib/
	rm -r ./build/obj
./build/lib:
	mkdir -p '$@'

./build/obj/things:
	touch '$@'

./build/obj/implementations.o: ./Implementations/cf_implementations.c ./build/obj ./build/include
	gcc $(options) -c $< -o $@
./build/obj/decompression.o: ./Implementations/decompression.c ./build/obj ./build/include
	gcc $(options) -c $< -o $@
./build/obj/swfmath.o: ./Implementations/swfmath.c ./build/obj ./build/include
	gcc $(options) -c $< -o $@
./build/obj/check_functions.o: ./Implementations/check_functions.c ./build/obj ./build/include
	gcc $(options) -c $< -o $@
./build/obj/primary.o: ./Implementations/primary.c ./build/obj ./build/include
	gcc $(options) -c $< -o $@


./build/obj: ./build
	mkdir -p $@


./build/include: $(header_files)
	mkdir -p $@
	cp $^ '$@'
./build:
	mkdir $@



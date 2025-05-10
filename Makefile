options := -Wall -std=c99 -pedantic
header_files := ./Headers/swftag.h ./Headers/tag_structs.h ./Headers/error.h ./Headers/decompression.h ./Headers/swfmath.h ./Headers/check_functions.h

all: ../libswftag-build/lib/optional/def_error.o ../libswftag-build/lib/libswftag.a
	echo ALL DONE!

../libswftag-build/lib/optional/def_error.o: ./Implementations/Default/error.c ../libswftag-build/include/libswftag ../libswftag-build/lib/optional
	cc $(options) -c $< -o $@
../libswftag-build/lib/optional: ../libswftag-build/lib
	mkdir -p $@

../libswftag-build/lib/libswftag.a: ../libswftag-build/obj/check_functions.o ../libswftag-build/obj/implementations.o ../libswftag-build/obj/primary.o ../libswftag-build/obj/decompression.o ../libswftag-build/obj/swfmath.o ../libswftag-build/lib
	if [ -f '$@' ]; then rm -f '$@'; fi
	ar rcs libswftag.a ../libswftag-build/obj/check_functions.o ../libswftag-build/obj/implementations.o ../libswftag-build/obj/primary.o ../libswftag-build/obj/decompression.o ../libswftag-build/obj/swfmath.o
	mv libswftag.a ../libswftag-build/lib/
	rm -r ../libswftag-build/obj
../libswftag-build/lib:
	mkdir -p '$@'

../libswftag-build/obj/things:
	touch '$@'

../libswftag-build/obj/implementations.o: ./Implementations/cf_implementations.c ../libswftag-build/obj ../libswftag-build/include/libswftag
	cc $(options) -c $< -o $@
../libswftag-build/obj/decompression.o: ./Implementations/decompression.c ../libswftag-build/obj ../libswftag-build/include/libswftag
	cc $(options) -c $< -o $@
../libswftag-build/obj/swfmath.o: ./Implementations/swfmath.c ../libswftag-build/obj ../libswftag-build/include/libswftag
	cc $(options) -c $< -o $@
../libswftag-build/obj/check_functions.o: ./Implementations/check_functions.c ../libswftag-build/obj ../libswftag-build/include/libswftag
	cc $(options) -c $< -o $@
../libswftag-build/obj/primary.o: ./Implementations/primary.c ../libswftag-build/obj ../libswftag-build/include/libswftag
	cc $(options) -c $< -o $@


../libswftag-build/obj: ../libswftag-build
	mkdir -p $@


../libswftag-build/include/libswftag: $(header_files) ../libswftag-build/include
	mkdir -p $@
	cp $^ '$@'
../libswftag-build/include: ../libswftag-build
	mkdir -p $@
../libswftag-build:
	mkdir $@

CC=gcc

libsort.a: libsort.o
	ar rcs libsort.a libsort.o

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $^


.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a

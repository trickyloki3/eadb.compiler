CC=gcc

librbt.a: rbt.o
	ar rcs librbt.a rbt.o

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $^


.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a

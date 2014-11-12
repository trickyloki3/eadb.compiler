# c compiler and flags
CCompiler = gcc
CFlags = -std=c99 -pedantic -Wall -g -O

# eadb library variables
HDRDIR = -I hdr/
OBJDIR = obj/
SRCDIR = src/
_OBJ = db.o util.o load.o api.o
OBJ = $(patsubst %,$(OBJDIR)%,$(_OBJ))

# compile all applications or projects
all: main

memchk: main
	valgrind --leak-check=full --track-origins=yes --log-file=itemc_mem_check.log -v ./main

main: src/main.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o $^ $(HDRDIR)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CCompiler) -c -o $@ $(CFlags) $^ $(HDRDIR)

# linux clean
.PHONY: clean
lclean:
	rm -rf obj
	mkdir obj

wclean:
	rmdir obj /S /Q
	mkdir obj

# c compiler and flags
CCompiler = gcc
CFlags = -std=c99 -pedantic -Wall -g -O

# eadb library variables
HDRDIR = -I hdr/
OBJDIR = obj/
SRCDIR = src/
_OBJ = db.o util.o load.o api.o script.o range.o name_range.o table.o
OBJ = $(patsubst %,$(OBJDIR)%,$(_OBJ))

# compile all applications or projects
all: conv item

memchk: item
	valgrind --leak-check=full --track-origins=yes --log-file=itemc_mem_check.log -v ./item

test: src/test.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o $^ $(HDRDIR)

item: src/item.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o -lm -ldl -lpthread $^ $(HDRDIR)

conv: src/conv.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o -lm -ldl -lpthread $^ $(HDRDIR)

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

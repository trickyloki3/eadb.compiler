# c compiler and flags
CCompiler = gcc
CFlags = -std=c99 -pedantic -Wall -g -O

# eadb library variables
HDRDIR = -Ihdr/
OBJDIR = obj/
SRCDIR = src/
_OBJ = db.o util.o load.o api.o script.o range.o name_range.o table.o
OBJ = $(patsubst %,$(OBJDIR)%,$(_OBJ))

HE_HDRDIR = -I../lib
HE_LIBDIR = ../lib/hconfig.a

# compile all applications or projects
all: 
	make lclean
	make conv
	make item
	make hitem
	./conv all
	./hitem
memchk: item
	valgrind --leak-check=full --track-origins=yes --log-file=itemc_mem_check.log -v ./item rathena

hitem: src/he_item.c $(OBJ)
	$(CCompiler) -c -o he_item.o $(CFlags) src/he_item.c $(HDRDIR) $(HE_HDRDIR)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o -lm -ldl -lpthread $^ $(HDRDIR) $(HE_HDRDIR) $(HE_LIBDIR)

item: src/item.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o -lm -ldl -lpthread $^ $(HDRDIR)

conv: src/conv.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o -lm -ldl -lpthread $^ $(HDRDIR)

test: src/test.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) lib/sqlite3.o -lm -ldl -lpthread $^ $(HDRDIR)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CCompiler) -c -o $@ $(CFlags) $^ $(HDRDIR)

# linux clean
.PHONY: clean
lclean:
	rm -rf obj
	mkdir obj
	rm -f conv
	rm -f item
	rm -f hitem
	rm -f he_item.o
	rm -f dump.txt
	rm -f item.txt
	rm -f itemc_mem_check.log

wclean:
	rmdir obj /S /Q
	mkdir obj

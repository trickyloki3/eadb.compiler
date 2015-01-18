# c compiler and flags
CCompiler = gcc
CFlags = -std=c99 -pedantic -Wall -g -O

# db library variables
HDRDIR = -Ihdr/
OBJDIR = obj/
SRCDIR = src/
LIB = -lsqlite3 -ldl -lpthread -lm
_OBJ = db.o util.o load.o api.o script.o range.o name_range.o table.o
OBJ = $(patsubst %,$(OBJDIR)%,$(_OBJ))

# hercule item database require libconfig
HE_HDRDIR = -I../lib
HE_LIBDIR = ../lib/hconfig.a

# compile all applications or projects
all: 
	make lclean
	make conv
	make item
	make itemr

ball:
	rm -f athena.db
	./conv rathena
	make bmin

bmin:
	rm -f itemr
	make itemr
	./itemr rathena script.txt

memchk: item
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=itemc_mem_check.log -v ./item rathena

hitem: src/he_item.c $(OBJ)
	$(CCompiler) -c -o he_item.o $(CFlags) src/he_item.c $(HDRDIR) $(HE_HDRDIR)
	$(CCompiler) -o $@ $(CFlags) $^ $(HDRDIR) $(HE_HDRDIR) $(HE_LIBDIR) $(LIB)

item: src/item.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) $^ $(HDRDIR) $(LIB)

itemr: src/itemr.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) $^ $(HDRDIR) $(LIB)

conv: src/conv.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) $^ $(HDRDIR) $(LIB)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CCompiler) -c -o $@ $(CFlags) $^ $(HDRDIR)

# linux clean
.PHONY: clean
lclean:
	rm -rf obj
	mkdir obj
	rm -f conv
	rm -f item
	rm -f itemr
	rm -f hitem
	rm -f he_item.o
	rm -f dump.txt
	rm -f item.txt
	rm -f itemc_mem_check.log
	rm -rf *.dSYM
	
wclean:
	rmdir obj /S /Q
	mkdir obj

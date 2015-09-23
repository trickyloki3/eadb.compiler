# c compiler and flags
CCompiler = gcc
CFlags = -std=c99 -pedantic -Wall -g

# db library variables
HDRDIR = -Isrc/
OBJDIR = obj/
SRCDIR = src/
LIB = -lsqlite3 -ldl -lpthread -lm -llua
_OBJ = util.o range.o name_range.o db_search.o script.o
OBJ = $(patsubst %,$(OBJDIR)%,$(_OBJ))

# compile all applications or projects
all:
	make lclean
	make ic

mem: ic
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=mem.log -v ./ic

ic: src/ic.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) $^ $(HDRDIR) $(LIB)


$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CCompiler) -c -o $@ $(CFlags) $^ $(HDRDIR) $(HE_HDRDIR)

# linux clean
.PHONY: clean
lclean:
	rm -rf obj
	mkdir obj


wclean:
	rmdir obj /S /Q
	mkdir obj

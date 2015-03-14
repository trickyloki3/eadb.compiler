# c compiler and flags
CCompiler = gcc
CFlags = -std=c99 -pedantic -Wall -g

# db library variables
HDRDIR = -Ihdr/
OBJDIR = obj/
SRCDIR = src/
LIB = -lsqlite3 -ldl -lpthread -lm
_OBJ = util.o load.o range.o name_range.o table.o db_eathena.o db_rathena.o db_hercules.o db_resources.o db_search.o script.o
OBJ = $(patsubst %,$(OBJDIR)%,$(_OBJ))

# hercule item database require libconfig
HE_HDRDIR = -I../lib
HE_LIBDIR = ../lib/hconfig.a

# compile all applications or projects
all: 
	make lclean
	make loki
	make item

memchk: item
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=itemc_mem_check.log -v ./item rathena

memloki: loki	
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=itemc_mem_check.log -v ./loki

item: src/item.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) $^ $(HDRDIR) $(HE_HDRDIR) $(LIB) $(HE_LIBDIR)

loki: src/loki.c $(OBJ)
	$(CCompiler) -o $@ $(CFlags) $^ $(HDRDIR) $(HE_HDRDIR) $(LIB) $(HE_LIBDIR)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CCompiler) -c -o $@ $(CFlags) $^ $(HDRDIR) $(HE_HDRDIR)

# linux clean
.PHONY: clean
lclean:
	rm -rf obj
	mkdir obj
	rm -f item
	rm -f itemr
	rm -f loki
	rm -f dump.txt
	rm -f item.txt
	rm -f itemc_mem_check.log
	rm -rf *.dSYM
	
wclean:
	rmdir obj /S /Q
	mkdir obj

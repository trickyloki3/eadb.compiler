# build paramaters
CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -g -O -I/usr/local/include -Ilib/libsort -Ilib/librbt
LDFLAGS=/usr/local/lib/liblua.a lib/libsort/libsort.a lib/librbt/librbt.a
LIB=-lsqlite3 -lm

OBJ_DIR=src
DB_DIR=src/db
IC_DIR=src/ic
CM_DIR=src/common

CFLAGS+=-I$(DB_DIR) -I$(IC_DIR) -I$(CM_DIR)

# item script compiler object files
IC_OBJ:=db_search.c rbt_name_range.c rbt_range.c script.c util.c lib/libsort/libsort.a
IC_OBJ:=$(patsubst %.c,$(OBJ_DIR)/%.o,$(IC_OBJ))

# item database loader object files
DB_OBJ:=db_eathena.c db_rathena.c db_resources.c load.c util.c
DB_OBJ:=$(patsubst %.c,$(OBJ_DIR)/%.o,$(DB_OBJ))

TEST_OBJ:=rbt_range.c rbt_name_range.c
TEST_OBJ:=$(patsubst %.c,$(OBJ_DIR)/%.o,$(TEST_OBJ))

# default target compiles the tools
all: ic dbc

lib/libsort/libsort.a:
	$(MAKE) -C lib/libsort

lib/librbt/librbt.a:
	$(MAKE) -C lib/librbt

ic: $(IC_DIR)/ic.c $(IC_OBJ) lib/librbt/librbt.a
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) $(LIB)

dbc: $(DB_DIR)/db.c $(DB_OBJ) lib/librbt/librbt.a lib/libsort/libsort.a
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) $(LIB)

test: src/test/main.c $(TEST_OBJ) lib/librbt/librbt.a lib/libsort/libsort.a
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) $(LIB)

$(OBJ_DIR)/%.o: $(DB_DIR)/%.c
	$(CC) -c -o $@ $(CFLAGS) $^

$(OBJ_DIR)/%.o: $(IC_DIR)/%.c
	$(CC) -c -o $@ $(CFLAGS) $^

$(OBJ_DIR)/%.o: $(CM_DIR)/%.c
	$(CC) -c -o $@ $(CFLAGS) $^

# clean object files for git inclusion
.PHONY: clean

clean:
	rm -rf src/*.o
	rm -f ic
	rm -f dbc
	rm -f test
	rm -rf ic.dSYM
	rm -rf dbc.dSYM
	rm -rf test.dSYM
	rm -f .DS_Store
	$(MAKE) -C lib/librbt clean
	$(MAKE) -C lib/libsort clean

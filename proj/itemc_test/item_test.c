#include "script.h"

#define STACK_ITEM_TEST 1
int unit_test(db_search_t *, int, char * [], int);

int main(int argc, char * argv[]) {
	/* load item database */
	db_search_t db;
	int mode = MODE_EATHENA;
	memset(&db, 0, sizeof(db_search_t));
	init_db(&db, MODE_EATHENA,
		"C:/Users/jim/Desktop/eadb.compiler/resources.db",
		"C:/Users/jim/Desktop/eadb.compiler/eathena.db");

	char * stack_item_test[] = { "1101", "sword", "1101 + getrefine()", NULL };
	unit_test(&db, mode, stack_item_test, STACK_ITEM_TEST);

	deit_db(&db);
	return 0;
}

int unit_test(db_search_t * db, int mode, char * test[], int func) {
	int i = 0;
	int ret = 0;
	char * string = NULL;
	script_t * script = NULL;
	block_r * block = NULL;

	script = calloc(1, sizeof(script_t));
	if (script == NULL) {
		exit_abt_safe("out of memory");
		goto failed;
	}
	script->db = db;
	script->mode = mode;

	for (i = 0; test[i] != NULL; i++) {
		if (script_block_alloc(script, &block) || block == NULL)
			goto failed;

		block->type = 0;
		block->db = script->db;
		block->mode = script->mode;
		block->arg_cnt = 0;

		string = convert_string(test[i]);
		if (string == NULL) {
			exit_abt_safe("out of memory");
			goto failed;
		}

		/* test function */
		switch (func) {
			case STACK_ITEM_TEST: ret = stack_item(block, string); break;
		}

		(ret) ?
			fprintf(stderr, "Passed; %s\n", string) :
			fprintf(stderr, "Failed; %s\n", string);

		free(string);

		if (script_block_dump(script, stderr) ||
			script_block_release(block) ||
			script_block_reset(script))
			goto failed;
	}
	script_block_finalize(script);
	free(script);
	return CHECK_PASSED;

failed:
	if (string != NULL)
		free(script);
	if (script != NULL) {
		script_block_finalize(script);
		free(script);
	}
	return CHECK_FAILED;
}
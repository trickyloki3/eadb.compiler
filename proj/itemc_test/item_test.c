#include "script.h"

int unit_test(db_search_t * db, int mode, char * test[]);

int main(int argc, char * argv[]) {
	/* load item database */
	db_search_t db;
	int mode = MODE_EATHENA;
	memset(&db, 0, sizeof(db_search_t));
	init_db(&db, MODE_EATHENA,
		"C:/Users/jim/Desktop/eadb.compiler/resources.db",
		"C:/Users/jim/Desktop/eadb.compiler/eathena.db");

	char * translate_item_new_test[] = { "1101", "sword", "1101 + getrefine()", NULL };
	unit_test(&db, mode, translate_item_new_test);

	deit_db(&db);
	return 0;
}

int unit_test(db_search_t * db, int mode, char * test[]) {
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

		string = convert_string(test[i]);
		if (string == NULL) {
			exit_abt_safe("out of memory");
			goto failed;
		}

		/* test function */
		ret = translate_item_new(block, string);
		(ret) ?
			fprintf(stderr, "Passed; %s\n", string) :
			fprintf(stderr, "Failed; %s\n", string);

		free(string);

		if (script_block_dump(script, stderr) ||
			script_block_release(block) ||
			script_block_reset(script, &block))
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
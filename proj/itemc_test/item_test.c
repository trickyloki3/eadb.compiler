#include "script.h"

/* KNOWN BUGS
 *  stack_item_test - case 4 - calcrange's arithmetic operators ignores negated values (0-9) U (11-15) + 1101 => (1101, 1116)
 */

#define STACK_ITEM_TEST 1
#define STACK_INT_TEST  2
#define STACK_CALL_TEST 3
#define STACK_TIME_TEST 4
int unit_test(db_search_t *, int, char * [], int, FILE *);
int unit_test_2(db_search_t * db, int mode, char * test[]);

int main(int argc, char * argv[]) {
	/* load item database */
	db_search_t db;
	int mode = MODE_EATHENA;
	memset(&db, 0, sizeof(db_search_t));
	init_db(&db, MODE_EATHENA,
		"C:/Users/jim/Desktop/eadb.compiler/resources.db",
		"C:/Users/jim/Desktop/eadb.compiler/eathena.db");

	char * stack_item_test[] = { 
		"1",
		"1101 + getrefiner()",
		"1101", 
		"sword", 
		"1101 + getrefine()", 
		"(getrefine() != 10)?1101 + getrefine():1101", 
		NULL 
	};
	char * stack_int_test[] = { 
		"1101", 
		"1101 + getrefine()", 
		"(getrefine() != 10)?1101 + getrefine():1101", 
		NULL 
	};
	char * stack_ptr_test[] = { 
		"(1, getrefine(), (1)?0:1)", 
		"(getrefine(), getitem(1101,1))", 
		"((1)?1:getrefine(), 0)",
		NULL 
	};
	char * stack_time_test[] = {
		"getrefine() * 10000",
		"3600000 * ((1)?1 + getrefine():1)",
		"86400000 * 10",
		NULL
	};

	/*unit_test(&db, mode, stack_item_test, STACK_ITEM_TEST, NULL);
	unit_test(&db, mode, stack_int_test, STACK_INT_TEST, NULL);
	unit_test(&db, mode, stack_ptr_test, STACK_CALL_TEST, NULL);
	unit_test(&db, mode, stack_time_test, STACK_TIME_TEST, stderr);*/

	char * translate_getitem[] = { 
		"getitem 1101, 1;",
		"getitem(sword, 1);",
		"getitem(1101 + getrefine(), getrefine());",
		"getitem(getrefine() + 1101, getrefine());",
		NULL 
	};
	char * translate_delitem[] = {
		"delitem(1101, 1);",
		"delitem(1101 + getrefine(), getrefine());",
		"delitem(getrefine() + 1101, getrefine());",
		NULL
	};
	char * translate_rentitem[] = {
		"rentitem sword, 10000;",
		"rentitem(1101 + getrefine(), 10000 * getrefine());",
		"rentitem(getrefine() + 1101, 10000);",
		NULL
	};
	/*unit_test_2(&db, mode, translate_getitem);
	unit_test_2(&db, mode, translate_delitem);*/
	unit_test_2(&db, mode, translate_rentitem);

	deit_db(&db);
	return 0;
}

/* test script stack functions */
int unit_test(db_search_t * db, int mode, char * test[], int func, FILE * file) {
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
		switch (func) {
			case STACK_ITEM_TEST: ret = stack_eng_item(block, string); break;
			case STACK_INT_TEST:  ret = stack_eng_int(block, string);  break;
			case STACK_CALL_TEST: ret = stack_ptr_call(block, string); break;
			case STACK_TIME_TEST: ret = stack_eng_time(block, string); break;
			default: break;
		}

		(ret > 0) ?
			fprintf(stderr, "Passed; %d; %s\n", ret, string) :
			fprintf(stderr, "Failed; %d; %s\n", ret, string);

		free(string);

		if (script_block_dump(script, file) ||
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

/* test script translate functions */
int unit_test_2(db_search_t * db, int mode, char * test[]) {
	int i = 0;
	int ret = 0;
	char * script = NULL;
	script_t * compiler = NULL;

	/* initialize script compiler */
	compiler = calloc(1, sizeof(script_t));
	compiler->db = db;
	compiler->mode = mode;
	compiler->item_id = 0;
	list_init_head(&compiler->block);

	/* run each test case */
	for (i = 0; test[i] != NULL; i++) {
		script = convert_string(test[i]);

		if (!script_lexical(&compiler->token, script)) {
			if (!script_analysis(compiler, &compiler->token, NULL, NULL)) {
				if (!script_translate(compiler))
					fprintf(stderr, "Passed; %s\n", script);
				else {
					fprintf(stderr, "Failed; script_translate; %s\n", script);
				}
			} else {
				fprintf(stderr, "Failed; script_analysis; %s\n", script);
			}
		} else {
			fprintf(stderr, "Failed; script_lexical; %s\n", script);
		}
		
		free(script);

		/* reset the script compiler */
		script_block_dump(compiler, stderr);
		script_block_reset(compiler);
	}

	script_block_finalize(compiler);
	free(compiler);
	return SCRIPT_PASSED;
}
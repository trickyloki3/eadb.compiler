#include "script.h"

void unittest(db_search_t * db, int mode, char *test[], char * func);

int main(int argc, char * argv[]) {
	/* load item database */
	db_search_t db;
	memset(&db, 0, sizeof(db_search_t));
	init_db(&db, MODE_EATHENA,
		"C:/Users/jim/Desktop/eadb.compiler/resources.db",
		"C:/Users/jim/Desktop/eadb.compiler/eathena.db");

	char * translate_getitem_test[] = {
		"getitem 1101, 1;",
		"getitem(1101, 1);",
		"getitem 1101, getrefine();",
		"getitem(1101, getrefine());",
		NULL
	};
	unittest(&db, MODE_EATHENA, translate_getitem_test, "translate_getitem");

	deit_db(&db);
	return 0;
}

void unittest(db_search_t * db, int mode, char *test[], char * func) {
	int i = 0;
	char * result = NULL;

	for (i = 0; test[i] != NULL; i++) {
		result = script_compile_raw(test[i], 0, NULL, db, mode);
		fprintf(stderr, "[unittest]: %s; %s; TEST %d\n%s\n%s\n", func, 
			result != NULL ? "PASSED" : "FAILED", i, test[i], result);
		if (result != NULL) free(result);
	}
}
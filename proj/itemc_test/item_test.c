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
		"getitem Sword, 1;",
		"getitem(Sword, 1);",
		NULL
	};

	char * translate_time_test[] = {
		"rentitem 1101, 86400000 + getrefine();",
		"rentitem 1101, 3600000 + getrefine();",
		"rentitem 1101, 60000 + getrefine();",
		"rentitem 1101, 1000 + getrefine();",
		"rentitem Sword, 86400000;"
		"rentitem Sword, 3600000;",
		"rentitem Sword, 60000;",
		"rentitem Sword, 1000;",
		NULL
	};

	char * translate_delitem_test[] = {
		"delitem sword;",
		"delitem 1101;",
		NULL
	};

	unittest(&db, MODE_EATHENA, translate_getitem_test, "translate_getitem");
	/*unittest(&db, MODE_EATHENA, translate_time_test, "translate_time");
	unittest(&db, MODE_EATHENA, translate_delitem_test, "translate_time");*/

	deit_db(&db);
	return 0;
}

void unittest(db_search_t * db, int mode, char *test[], char * func) {
	int i = 0;
	char * result = NULL;

	for (i = 0; test[i] != NULL; i++) {
		result = script_compile_raw(test[i], 0, NULL, db, mode);
		fprintf(stderr, "[unittest]: %s; %s; TEST %d\n%s\n%s\n", func, result != NULL ? "PASSED" : "FAILED", i, test[i], result);
		if (result != NULL)
			free(result);
	}
}
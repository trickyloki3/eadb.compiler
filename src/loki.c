#include "load.h"
#include "util.h"
#include "db_search.h"

int main(int argc, char * argv[]) {
	/* test loading all the databases */
	/*load_rathena_database(NULL);
	load_eathena_database(NULL);
	load_hercules_database(NULL);
	load_resource_database(NULL);*/

	/* test loading all the databases to sqlite3 */
	default_eathena_database();
	/*default_rathena_database();
	default_hercules_database();*/
	default_resource_database();
	/*test_db();*/
	return 0;
}

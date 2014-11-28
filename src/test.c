#include <db.h>
#include <api.h>
int main(int argc, char * argv[]) {	
	int i = 0;
	struct lt_db_t * sql = init_db("test.db", INITIALIZE_DB);
	ra_item_group_t * db = NULL;
	load_db_t * db33 = load("../radb/item_group/item_bluebox.txt", trim_alpha, load_general, ra_item_group_load);
	load_ra_item_group(sql, sql->ea_item_group_insert, db33->db, db33->size);
	db33->dealloc(db33->db, db33->size);
	free(db33);
	deit_db(sql);
	return 0;
}
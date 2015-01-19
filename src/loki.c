#include <db.h>
#include <script.h>

int main(int argc, char * argv[]) {
	int i = 0;

	/* setup for item compilation */
	global_mode = RATHENA;
	global_db = init_ic_db("athena.db");
	FILE * file_itm = fopen("item.txt", "w");
	if(file_itm == NULL) exit_abt("failed to open item.txt.");

	/*load_db_t * db1 = load("../radb/item_combo_db.txt", trim_numeric, load_general, ra_item_combo_load);
	ra_item_combo_t * item_combo_db = (ra_item_combo_t *) db1->db;
	for(i = 0; i < db1->size; i++) {
		item_combo_db[i].description = script_compile(item_combo_db[i].script, convert_integer(item_combo_db[i].item_id_list, 10));
		fprintf(file_itm,"%s#\n%s#\n", item_combo_db[i].item_id_list, item_combo_db[i].description);
		free(item_combo_db[i].description);
	}*/

	db1->dealloc(db1->db, db1->size);
	deit_ic_db(global_db);
	fclose(file_itm);
	return 0;
}
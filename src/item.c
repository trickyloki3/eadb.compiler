/*
 *   file: item.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#define DEBUG 1
#include "api.h"
#include "script.h"
int load_by_mode(int mode, struct ic_db_t * db, ic_item_t * item);

int main(int argc, char * argv[]) {
	if(argc >= 2) {
		node_dbg = NULL; /* by default don't dump node output */
		global_mode = (ncs_strcmp(argv[1],"rathena") == 0) ? RATHENA : 
					  	(((ncs_strcmp(argv[1],"eathena") == 0) ? EATHENA : 
					  ((ncs_strcmp(argv[1],"hercules") == 0) ? HERCULES : -1)));

		if(global_mode != RATHENA && global_mode != EATHENA && global_mode != HERCULES)
			exit_abt("invalid database type; only 'eathena', 'rathena', or 'hercules' is supported.");
	} else {
		exit_buf("syntax '%s [eathena | rathena | hercules]'", argv[0]);
	}

	int sqlite_status = 0;
	ic_item_t item;
	char * item_desc = NULL;
	FILE * file_dgb;
	FILE * file_itm;
	if(DEBUG) {
		file_dgb = fopen("dump.txt", "w");
		if(file_dgb == NULL) exit_abt("failed to open dump.txt.");
	}
	/* write the item translations to item.txt */
	file_itm = fopen("item.txt", "w");
	if(file_itm == NULL) exit_abt("failed to open item.txt.");

	global_db = init_ic_db("athena.db");
	memset(&item, 0, sizeof(ic_item_t));
	sqlite_status = load_by_mode(global_mode, global_db, &item);
	if(sqlite_status == SQLITE_DONE) {
		printf("%s: athena.db must be loaded with at least one database; eAthena, rAthena, or Hercules.\n"
				 "%s: use 'conv [ eathena | rathena | hercules | all ]' to load a database.\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	while(sqlite_status == SQLITE_ROW) {
		if(item.script != NULL && strlen(item.script) > 0) {
			item_desc = script_compile_raw(item.script, item.id, file_dgb);
			fprintf(file_itm,"%d#\n%s#\n", item.id, (item_desc) ? item_desc : "");
			if(item_desc != NULL) free(item_desc);
		}
		sqlite_status = load_by_mode(global_mode, global_db, &item);
	}
	deit_ic_db(global_db);
	if(DEBUG) fclose(file_dgb);
	fclose(file_itm);
	if(item.name != NULL) free(item.name);
	if(item.script != NULL) free(item.script);
	return 0;
}

int load_by_mode(int mode, struct ic_db_t * db, ic_item_t * item) {
	int status = 0;
	exit_null("db is null", 1, db);
	exit_null("item is null", 1, item);
	switch(mode) {
		case EATHENA:
			status = sqlite3_step(db->ea_item_iterate);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->ea_item_iterate, 0);
				if(item->name != NULL) free(item->name);
				item->name = convert_string((const char *) sqlite3_column_text(db->ea_item_iterate, 2));
				if(item->script != NULL) free(item->script);
				item->script = convert_string((const char *) sqlite3_column_text(db->ea_item_iterate, 19));
			}
			break;
		case RATHENA:
			status = sqlite3_step(db->ra_item_iterate);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->ra_item_iterate, 0);
				if(item->name != NULL) free(item->name);
				item->name = convert_string((const char *) sqlite3_column_text(db->ra_item_iterate, 2));
				if(item->script != NULL) free(item->script);
				item->script = convert_string((const char *) sqlite3_column_text(db->ra_item_iterate, 20));
			}
			break;
		case HERCULES:
			status = sqlite3_step(db->he_item_iterate);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->he_item_iterate, 0);
				if(item->name != NULL) free(item->name);
				item->name = convert_string((const char *) sqlite3_column_text(db->he_item_iterate, 2));
				if(item->script != NULL) free(item->script);
				item->script = convert_string((const char *) sqlite3_column_text(db->he_item_iterate, 39));
			}
			break;
		default: exit_abt("invalid database mode.");
	}
	return status;
}

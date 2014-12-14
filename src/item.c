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
	int script_status = 0;
	char err[BUF_SIZE];
	char translate[BUF_SIZE];
	int translate_off = 0;

	ic_item_t item;
	token_r token_list;
	int block_cnt = 0;
	block_r * block_list;
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
	block_init(&block_list, BLOCK_SIZE);
	while(sqlite_status == SQLITE_ROW) {
		block_cnt = 0;
		if(item.script != NULL && strlen(item.script) > 0) {
			/* perform lexical analysis */
			script_status = script_lexical(&token_list, item.script);
			if(script_status == SCRIPT_FAILED) {
				exit_abt(build_buffer(err, "failed to perform lexical analysis on item %d\n", item.id));
			} else {
				/* perform structual analysis */
				script_status = script_analysis(&token_list, block_list, &block_cnt, item.id, 0x01, 0);
				if(script_status == SCRIPT_FAILED) {
					exit_abt(build_buffer(err, "failed to perform structual analysis on item %d\n", item.id));
				} else if(block_cnt > 0) {
					/* perform set block inheritance */
					script_dependencies(block_list, block_cnt);
					/* translate the script */
					script_translate(block_list, block_cnt);
					/* write bonus translation after minimization */
					script_bonus(block_list, block_cnt);
					/* generate the translation */
					translate_off = 0;
					script_generate(block_list, block_cnt, translate, &translate_off);
					fprintf(file_itm,"%d#\n%s#\n", block_list->item_id, translate);
					/* translate the block list */
					if(DEBUG) block_debug_dump_all(block_list, block_cnt, file_dgb);
				}
			}
		}

		block_deinit(block_list, BLOCK_SIZE);
		sqlite_status = load_by_mode(global_mode, global_db, &item);
	}
	block_finalize(block_list, BLOCK_SIZE);
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

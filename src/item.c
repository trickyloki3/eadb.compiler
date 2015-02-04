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
	int global_mode = 0;
	struct ic_db_t * global_db = NULL;
	if(argc >= 2) {
		global_mode = (ncs_strcmp(argv[1],"rathena") == 0) ? RATHENA : 
					  	(((ncs_strcmp(argv[1],"eathena") == 0) ? EATHENA : 
					  ((ncs_strcmp(argv[1],"hercules") == 0) ? HERCULES : -1)));

		if(global_mode != RATHENA && global_mode != EATHENA && global_mode != HERCULES) {
			fprintf(stderr,"[error]: invalid database type; only 'eathena', 'rathena', or 'hercules' is supported.");
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr,"[error]: syntax '%s [eathena | rathena | hercules]'", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sqlite_status = 0;
	ic_item_t item;
	char * item_desc = NULL;
	FILE * file_dgb;
	FILE * file_itm;
	if(DEBUG) {
		file_dgb = fopen("dump.txt", "w");
		if(file_dgb == NULL) {
			fprintf(stderr,"[error]: failed to load debug file.\n");
			exit(EXIT_FAILURE);
		}
	}
	/* write the item translations to item.txt */
	file_itm = fopen("item.txt", "w");
	if(file_itm == NULL) {
		fprintf(stderr,"[error]: failed to load item file.\n");
		exit(EXIT_FAILURE);
	}

	global_db = init_ic_db("athena.db");
	memset(&item, 0, sizeof(ic_item_t));
	sqlite_status = iter_item_db(global_mode, global_db, &item);
	if(sqlite_status == SQLITE_DONE) {
		printf("%s: athena.db must be loaded with at least one database; eAthena, rAthena, or Hercules.\n"
				 "%s: use 'conv [ eathena | rathena | hercules | all ]' to load a database.\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	while(sqlite_status == SQLITE_ROW) {
		if(item.script != NULL && strlen(item.script) > 0) {
			item_desc = script_compile_raw(item.script, item.id, file_dgb, global_db, global_mode);
			if(item_desc != NULL) {
				fprintf(file_itm,"%d#\n%s#\n", item.id, (item_desc) ? item_desc : "");
				free(item_desc);
			}
		}
		sqlite_status = iter_item_db(global_mode, global_db, &item);
	}
	deit_ic_db(global_db);
	if(DEBUG) fclose(file_dgb);
	fclose(file_itm);
	return 0;
}

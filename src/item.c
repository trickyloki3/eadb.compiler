/*
 *   file: item.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "time.h"
#include "db_search.h"
#include "script.h"

int main(int argc, char * argv[]) {
	int skip = 0;
	int ret = 0;
	int cnt = 0;
	int start_time = time(NULL);
	FILE * fitem = NULL;
	FILE * debug = NULL;
	item_t item;
	script_t script;
	char buffer[BUF_SIZE];
	int offset = 0;

	/* init resources */
	db_search_t db;
	memset(&script, 0, sizeof(script_t));
	memset(&item, 0, sizeof(item_t));
	fitem = fopen("item.txt", "w");
	debug = fopen("dump.txt", "w");
	if(fitem == NULL || debug == NULL) {
		exit_abt_safe("fail to open item.txt or dump.txt");
		exit(EXIT_FAILURE);
	}
	
	/* attach database to script */
	script.mode = MODE_HERCULES;
	script.db = &db;
	init_db(script.db, script.mode, "/root/Desktop/dev/eAdb.Compiler3/resources.db", "/root/Desktop/dev/eAdb.Compiler3/hercules.db");
	
	/* process all items in database */
	ret = item_iterate(script.db, &item);
	if(ret == SQLITE_ROW) {
		while(ret == SQLITE_ROW) {
			if(item.script != NULL && strlen(item.script) > 0) {
				/* compile the item script */
				fprintf(stderr,"\r%d\r", script.item_id);
				offset = 0;
				script.item_id = item.id;
				if(!script_lexical(&script.token, item.script)) {
					skip = script_analysis(&script, &script.token, NULL, NULL);
					if(skip == SCRIPT_PASSED) {
						if(!script_translate(&script)) {
							if(!script_bonus(&script)) {
								if(!script_generate(&script, buffer, &offset)) {
									if(!script_generate_combo(script.item_id, buffer, &offset, script.db, script.mode)) {
										fprintf(fitem,"%d#\n%s#\n", script.item_id, buffer);
										script_block_dump(&script, debug);
									} else {
										/*fprintf(stderr,"[warn]: failed to item combo '%s' on item %d\n", item.script, script.item_id);*/
									}
								} else {
									/*fprintf(stderr,"[warn]: failed to generate '%s' on item %d\n", item.script, script.item_id);*/
								}
							} else {
								/*fprintf(stderr,"[warn]: failed to bonus '%s' on item %d\n", item.script, script.item_id);*/
							}
						} else {
							/*fprintf(stderr,"[warn]: failed to translate '%s' on item %d\n", item.script, script.item_id);*/
						}
					} else if(skip == SCRIPT_SKIP) {
						/* empty script */
					} else {
						/*fprintf(stderr,"[warn]: failed to parser '%s' on item %d\n", item.script, script.item_id);*/
					}
				} else {
					/*fprintf(stderr,"[warn]: failed to lex '%s' on item %d\n", item.script, script.item_id);*/
				}
				script_block_reset(&script);
				cnt++;
			}
			ret = item_iterate(script.db, &item);
		}
	} else if(ret == SQLITE_DONE) {
		exit_abt_safe("database contains zero item entries");
	} else {
		exit_abt_safe("failed to query item database");
	}
	fprintf(stderr,"[info]: compiled all %d items in %d seconds.\n", cnt, (int) (time(NULL) - start_time));

	/* deit resources */
	fclose(debug);
	fclose(fitem);
	script_block_finalize(&script);
	deit_db(&db);
	return 0;
}

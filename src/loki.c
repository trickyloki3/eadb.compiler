/*
 *   file: loki.c
 *   date: 03/15/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "lua.h"
#include "lauxlib.h"
#include "script.h"
#include "db_search.h"

void dump_lua_stack(lua_State * lstate);

int main(int argc, char * argv[]) {
	int i = 0;
	int j = 0;
	int len = 0;
	int line_main = 0;							/* description line numbers */
	int line_number = 0;
	char command[BUF_SIZE];						/* command buffer */
	char buffer[BUF_SIZE];						/* concatenation buffer */
	int length = 0;
	/* testing manual flavour text data miner */
	int item_id = 0;
	int check_field = 0;
	int entry_count = 0;
	int table_index = 0;						/* the primary table index on the lua stack */
	int entry_index = 0;						/* the entry table index on the lua stack */
	int desc_index = 0;
	lua_State * lstate = luaL_newstate();		/* the lua state maintains the lua stack */
	/* item description entry */
	const char * unidentifiedDisplayName = NULL;
	const char * unidentifiedResourceName = NULL;
	const char * identifiedDisplayName = NULL;
	const char * identifiedResourceName = NULL;
	int slotCount = 0;
	int classNum = 0;
	/* initialize the script compilers and databases */
	int ea_flag = 0;
	int ra_flag = 0;
	int he_flag = 0;
	script_t ea_script;
	script_t ra_script;
	script_t he_script;
	db_search_t ea_db;
	db_search_t ra_db;
	db_search_t he_db;
	item_t ea_item;
	item_t ra_item;
	item_t he_item;
	char * ea_translate = NULL;
	char * ra_translate = NULL;
	char * he_translate = NULL;
	memset(&ea_script, 0, sizeof(script_t));
	memset(&ra_script, 0, sizeof(script_t));
	memset(&he_script, 0, sizeof(script_t));
	memset(&ea_db, 0, sizeof(db_search_t));
	memset(&ra_db, 0, sizeof(db_search_t));
	memset(&he_db, 0, sizeof(db_search_t));
	memset(&ea_item, 0, sizeof(item_t));
	memset(&ra_item, 0, sizeof(item_t));
	memset(&he_item, 0, sizeof(item_t));
	ea_script.mode = MODE_EATHENA;
	ra_script.mode = MODE_RATHENA;
	he_script.mode = MODE_HERCULES;
	ea_script.db = &ea_db;
	ra_script.db = &ra_db;
	he_script.db = &he_db;
	init_db(ea_script.db, ea_script.mode, "resources.db", "eathena.db");
	init_db(ra_script.db, ra_script.mode, "resources.db", "rathena.db");
	init_db(he_script.db, he_script.mode, "resources.db", "hercules.db");

	/* initialize the flavour text database */
	sqlite3 * text_db = NULL;
	sqlite3_stmt * text_search_stmt = NULL;
	sqlite3_stmt * text_insert_stmt = NULL;
	if(sqlite3_open("flavour_text.db", &text_db) != SQLITE_OK)
		exit_abt_safe("failed to open text database");

	/* create the table only if not exist */
	sqlite3_exec(text_db, "CREATE TABLE IF NOT EXISTS flavour_text(id INTEGER, "
					      "uniDisplayName TEXT, uniResName TEXT, uniDescName TEXT,"
					      "idDisplayName TEXT, idResName TEXT, idDescName TEXT,"
					      "slotCount INTEGER, classnum INTEGER);", NULL, NULL, NULL);

	/* initialize the prepare statements */
	if(sqlite3_prepare_v2(text_db, "INSERT INTO flavour_text VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);", -1, &text_insert_stmt, NULL) != SQLITE_OK)
		exit_abt_safe("failed to compile text insert statement");
	if(sqlite3_prepare_v2(text_db, "SELECT * FROM flavour_text WHERE id = ?;", -1, &text_search_stmt, NULL) != SQLITE_OK)
		exit_abt_safe("failed to compile text search statement");

	if(lstate == NULL) 
		exit_abt_safe("failed to allocate lua state");
	if(luaL_loadfile(lstate, "../misc/itemInfo.lub"))
		return exit_func_safe("syntax error; %s", lua_tostring(lstate, -1));
	if(lua_pcall(lstate, 0, 0, 0))
		return exit_func_safe("syntax error; %s", lua_tostring(lstate, -1));
	lua_getglobal(lstate, "tbl");
	if(!lua_istable(lstate, -1))
		return exit_abt_safe("the 'tbl' must be a lua table");
	table_index = lua_gettop(lstate);

	/* iterate rate through the table */
	lua_pushnil(lstate);
	while(lua_next(lstate, 1)) {
		entry_index = lua_gettop(lstate);
		item_id = lua_tonumber(lstate, -2);

		/* skip entry if it is already added to the database */
		sqlite3_bind_int(text_search_stmt, 1, item_id);
		if(sqlite3_step(text_search_stmt) != SQLITE_DONE) {
			sqlite3_reset(text_search_stmt);
			lua_pop(lstate, 1);
			continue;
		} else {
			sqlite3_reset(text_search_stmt);
		}
		/* push every single table element on the stack */
		check_field = lua_gettop(lstate);
		lua_getfield(lstate, entry_index, "unidentifiedDisplayName");
		lua_getfield(lstate, entry_index, "unidentifiedResourceName");
		lua_getfield(lstate, entry_index, "unidentifiedDescriptionName");
		lua_getfield(lstate, entry_index, "identifiedDisplayName");
		lua_getfield(lstate, entry_index, "identifiedResourceName");
		lua_getfield(lstate, entry_index, "identifiedDescriptionName");
		desc_index = lua_gettop(lstate);
		lua_getfield(lstate, entry_index, "slotCount");
		lua_getfield(lstate, entry_index, "ClassNum");
		if(check_field + 8 != lua_gettop(lstate))
			fprintf(stderr,"missing a field for item id %d\n", item_id);
		
		/* query all three database with the id */
		fprintf(stdout, "======================================\n"
						"Item ID: %d\n"
						"======================================\n", 
						item_id);

		ea_flag = 0;
		ra_flag = 0;
		he_flag = 0;

		if(!item_db_search_id(&ea_db, &ea_item, item_id)) {
			ea_flag |= 0x01;
			if(strlen(ea_item.script) > 0) {
				ea_translate = script_compile_raw(ea_item.script, ea_item.id, NULL, ea_script.db, ea_script.mode);
				fprintf(stdout,"eAthena Name: %s\n%s", ea_item.name, (ea_translate == NULL) ? "Empty Script\n" : ea_translate);
				if(ea_translate != NULL) ea_flag |= 0x02;
				free(ea_translate);
			} else {
				fprintf(stdout,"eAthena Name: %s\n", ea_item.name);
			}
		} else {
			fprintf(stdout,"eAthena Name: N\\A\n");
		}
		fprintf(stdout, "======================================\n");
		if(!item_db_search_id(&ra_db, &ra_item, item_id)) {
			ra_flag |= 0x01;
			if(strlen(ra_item.script) > 0) {
				ra_translate = script_compile_raw(ra_item.script, ra_item.id, NULL, ra_script.db, ra_script.mode);
				fprintf(stdout,"rAthena Name: %s\n%s", ra_item.name, (ra_translate == NULL) ? "Empty Script\n" : ra_translate);
				if(ra_translate != NULL) ra_flag |= 0x02;
				free(ra_translate);
			} else {
				fprintf(stdout,"rAthena Name: %s\n", ra_item.name);
			}
		} else {
			fprintf(stdout,"rAthena Name: N\\A\n");
		}
		fprintf(stdout, "======================================\n");
		if(!item_db_search_id(&he_db, &he_item, item_id)) {
			he_flag |= 0x01;
			if(strlen(he_item.script) > 0) {
				he_translate = script_compile_raw(he_item.script, he_item.id, NULL, he_script.db, he_script.mode);
				fprintf(stdout,"Hercules Name: %s\n%s", he_item.name, (he_translate == NULL) ? "Empty Script\n" : he_translate);
				if(he_translate != NULL) he_flag |= 0x02;
				free(he_translate);
			} else {
				fprintf(stdout,"Hercules Name: %s\n", he_item.name);
			}
		} else {
			fprintf(stdout,"Hercules Name: N\\A\n");
		}
		fprintf(stdout, "======================================\n");
		lua_pushnil(lstate);
		while(lua_next(lstate, desc_index)) {
			lua_pushnil(lstate);
			lua_copy(lstate, -2, -1);
			lua_copy(lstate, -3, -2);
			lua_copy(lstate, -1, -3);
			lua_pop(lstate, 1);
		}

		for(i = desc_index + 3; i <= lua_gettop(lstate); i++) {
			printf("[%d] %s\n", i, lua_tostring(lstate, i));
		}

		/* user enter the next commands */
		command[0] = 0;
		while(command[0] != 'n' && command[0] != 'd' && command[0] != 'a' && command[0] != 'x') {
			fprintf(stdout, "next(n) done (d) exit(e) concatenate lines (c) modify lines (m) add item (a) add empty(x) add only first line(s): ");
			fgets(command, BUF_SIZE, stdin);
			switch(command[0]) {
				case 's':
					length = sprintf(buffer, "%s", lua_tolstring(lstate, desc_index + 3, NULL));
					buffer[length] = 0;
					fprintf(stdout, "=================================\n"
									"Item Id: %d\nFlavour Text: %s\n"
									"=================================\n", 
									item_id, buffer);
					/* pop all the lines from the stack */
					lua_pop(lstate, lua_gettop(lstate) - desc_index - 2);

					/* get reference to the item description entry */
					unidentifiedDisplayName = lua_tostring(lstate, -8);
					unidentifiedResourceName = lua_tostring(lstate, -7);
					identifiedDisplayName = lua_tostring(lstate, -5);
					identifiedResourceName = lua_tostring(lstate, -4);
					slotCount = lua_tonumber(lstate, -2);
					classNum = lua_tonumber(lstate, -1);
					sqlite3_bind_int(text_insert_stmt, 1, item_id);
					sqlite3_bind_text(text_insert_stmt, 2, unidentifiedDisplayName, strlen(unidentifiedDisplayName), SQLITE_STATIC);
					sqlite3_bind_text(text_insert_stmt, 3, unidentifiedResourceName, strlen(unidentifiedResourceName), SQLITE_STATIC);
					sqlite3_bind_text(text_insert_stmt, 4, buffer, strlen(buffer), SQLITE_STATIC);
			      	sqlite3_bind_text(text_insert_stmt, 5, identifiedDisplayName, strlen(identifiedDisplayName), SQLITE_STATIC);
			      	sqlite3_bind_text(text_insert_stmt, 6, identifiedResourceName, strlen(identifiedResourceName), SQLITE_STATIC);
			      	sqlite3_bind_text(text_insert_stmt, 7, buffer, strlen(buffer), SQLITE_STATIC);
			      	sqlite3_bind_int(text_insert_stmt, 8, slotCount);
			      	sqlite3_bind_int(text_insert_stmt, 9, classNum);
			      	sqlite3_step(text_insert_stmt);
			      	sqlite3_reset(text_insert_stmt);
			      	command[0] = 'a';
			      	break;
				case 'a':
					buffer[0] = '\0';
					/* select which lines to include in final description */
					do {
						/* check that there are enough lines to add */
						if(desc_index + 3 >= lua_gettop(lstate)) {
							fprintf(stdout, "no more lines to add\n");
							fgets(command, BUF_SIZE, stdin);
							break;
						}

						/* print the line numbers and the text */
						for(i = desc_index + 3; i <= lua_gettop(lstate); i++)
							printf("[%d] %s\n", i, lua_tostring(lstate, i));
						fprintf(stdout, "current description: %s\n", buffer);
						fprintf(stdout, "enter line to add (zero to stop): ");
						if(scanf("%d", &line_number) == 0 || line_number <= 0) {
							fgets(command, BUF_SIZE, stdin);
							break;
						}
						/* check the line is within range */
						if(line_number < desc_index || line_number > lua_gettop(lstate)) {
							fprintf(stdout, "invalid line number\n");
						} else {
							length = sprintf(buffer, "%s\n%s", buffer, lua_tolstring(lstate, line_number, NULL));
							buffer[length] = 0;
						}
					} while(line_number > 0);

					/* pop all the lines from the stack */
					lua_pop(lstate, lua_gettop(lstate) - desc_index - 2);

					/* get reference to the item description entry */
					unidentifiedDisplayName = lua_tostring(lstate, -8);
					unidentifiedResourceName = lua_tostring(lstate, -7);
					identifiedDisplayName = lua_tostring(lstate, -5);
					identifiedResourceName = lua_tostring(lstate, -4);
					slotCount = lua_tonumber(lstate, -2);
					classNum = lua_tonumber(lstate, -1);

					fprintf(stdout, "=================================\n"
									"Item Id: %d\nFlavour Text: %s\n"
									"=================================\n", 
									item_id, buffer);
					fprintf(stdout, "Do you want to add this item? (A/Z)");
					fgets(command, BUF_SIZE, stdin);
					switch(command[0]) {
						case 'A': case 'a':
							sqlite3_bind_int(text_insert_stmt, 1, item_id);
							sqlite3_bind_text(text_insert_stmt, 2, unidentifiedDisplayName, strlen(unidentifiedDisplayName), SQLITE_STATIC);
							sqlite3_bind_text(text_insert_stmt, 3, unidentifiedResourceName, strlen(unidentifiedResourceName), SQLITE_STATIC);
							sqlite3_bind_text(text_insert_stmt, 4, buffer, strlen(buffer), SQLITE_STATIC);
					      	sqlite3_bind_text(text_insert_stmt, 5, identifiedDisplayName, strlen(identifiedDisplayName), SQLITE_STATIC);
					      	sqlite3_bind_text(text_insert_stmt, 6, identifiedResourceName, strlen(identifiedResourceName), SQLITE_STATIC);
					      	sqlite3_bind_text(text_insert_stmt, 7, buffer, strlen(buffer), SQLITE_STATIC);
					      	sqlite3_bind_int(text_insert_stmt, 8, slotCount);
					      	sqlite3_bind_int(text_insert_stmt, 9, classNum);
					      	sqlite3_step(text_insert_stmt);
					      	sqlite3_reset(text_insert_stmt);
					      	command[0] = 'a';
							break;
						default:
							fprintf(stdout,"did not added the item description\n");
							break;
					}
					break;
				case 'c':
					/* ask the user for the starting line */
					fprintf(stdout, "enter starting line to concatenate to (zero to stop): ");
					if(scanf("%d", &line_main) == 0 || line_main <= 0) { 
						fgets(buffer, BUF_SIZE, stdin); 
						break; 
					}

					/* check the line is within range */
					if(line_main < desc_index || line_main > lua_gettop(lstate)) {
						fprintf(stdout, "invalid line number\n");
						fgets(buffer, BUF_SIZE, stdin); 
						break;
					}

					/* concatenate all the lines requested */
					do {
						/* check that there are enough lines to concatenate */
						if(desc_index + 3 >= lua_gettop(lstate)) {
							fprintf(stdout, "no more lines to concatenate\n");
							fgets(buffer, BUF_SIZE, stdin);
							break;
						}

						/* print the line numbers and the text */
						for(i = desc_index + 3; i <= lua_gettop(lstate); i++)
							printf("[%d] %s\n", i, lua_tostring(lstate, i));

						fprintf(stdout, "enter line to concatenate (zero to stop): ");
						if(scanf("%d", &line_number) == 0 || line_number <= 0) {
							fgets(buffer, BUF_SIZE, stdin);
							break;
						}
						/* check the line is within range */
						if(line_number < desc_index || line_number > lua_gettop(lstate)) {
							fprintf(stdout, "invalid line number\n");
						} else if(line_number == line_main) {
							fprintf(stdout, "cannot concatenate a line to itself\n");
						} else {
							length = sprintf(buffer, "%s %s", lua_tolstring(lstate, line_main, NULL), lua_tolstring(lstate, line_number, NULL));
							buffer[length] = 0;
							lua_pushstring(lstate, buffer);
							lua_replace(lstate, line_main);
							lua_remove(lstate, line_number);
						}
					} while(line_number > 0);

					/* print the end result */
					for(i = desc_index + 3; i <= lua_gettop(lstate); i++)
						printf("[%d] %s\n", i, lua_tostring(lstate, i));
					break;
				case 'm':
					do {
						/* ask the user which line to modified */
						fprintf(stdout, "enter line to modify to (zero to stop): ");
						if(scanf("%d", &line_main) == 0 || line_main <= 0) { 
							fgets(buffer, BUF_SIZE, stdin); 
							break; 
						}

						/* check the line is within range */
						if(line_main < desc_index || line_main > lua_gettop(lstate)) {
							fprintf(stdout, "invalid line number\n");
							fgets(buffer, BUF_SIZE, stdin); 
							break;
						}
						fgets(buffer, BUF_SIZE, stdin);

						/* ask the user to enter the new line */
						fprintf(stdout, "enter the new line: ");
						fgets(buffer, BUF_SIZE, stdin);
						len = strlen(buffer);
						for(j = len - 1; j >= 0; j--)
							if(buffer[j] == '\n') {
								buffer[j] = 0;
								break;
							}

						/* replace the previous string */
						lua_pushstring(lstate, buffer);
						lua_replace(lstate, line_main);

						for(i = desc_index + 3; i <= lua_gettop(lstate); i++)
							printf("[%d] %s\n", i, lua_tostring(lstate, i));
					} while(line_main > 0);
					break;
				case 'x':
					/* pop all the lines from the stack */
					buffer[0] = '\0';
					lua_pop(lstate, lua_gettop(lstate) - desc_index - 2);
					unidentifiedDisplayName = lua_tostring(lstate, -8);
					unidentifiedResourceName = lua_tostring(lstate, -7);
					identifiedDisplayName = lua_tostring(lstate, -5);
					identifiedResourceName = lua_tostring(lstate, -4);
					slotCount = lua_tonumber(lstate, -2);
					classNum = lua_tonumber(lstate, -1);
					sqlite3_bind_int(text_insert_stmt, 1, item_id);
					sqlite3_bind_text(text_insert_stmt, 2, unidentifiedDisplayName, strlen(unidentifiedDisplayName), SQLITE_STATIC);
					sqlite3_bind_text(text_insert_stmt, 3, unidentifiedResourceName, strlen(unidentifiedResourceName), SQLITE_STATIC);
					sqlite3_bind_text(text_insert_stmt, 4, buffer, strlen(buffer), SQLITE_STATIC);
			      	sqlite3_bind_text(text_insert_stmt, 5, identifiedDisplayName, strlen(identifiedDisplayName), SQLITE_STATIC);
			      	sqlite3_bind_text(text_insert_stmt, 6, identifiedResourceName, strlen(identifiedResourceName), SQLITE_STATIC);
			      	sqlite3_bind_text(text_insert_stmt, 7, buffer, strlen(buffer), SQLITE_STATIC);
			      	sqlite3_bind_int(text_insert_stmt, 8, slotCount);
			      	sqlite3_bind_int(text_insert_stmt, 9, classNum);
			      	sqlite3_step(text_insert_stmt);
			      	sqlite3_reset(text_insert_stmt);
			      	break;
				case 'e':
					exit(EXIT_SUCCESS);
				default: 
					break;
			}
		}
		if(command[0] == 'd') break;
		lua_pop(lstate, lua_gettop(lstate) - table_index - 1);
		entry_count++;
	}


	fprintf(stderr,"[loki]: added %d item description entries.\n", entry_count);
	sqlite3_close(text_db);
	deit_db(&ea_db);
	deit_db(&ra_db);
	deit_db(&he_db);
	script_block_finalize(&ea_script);
	script_block_finalize(&ra_script);
	script_block_finalize(&he_script);
	lua_close(lstate);
	return 0;
}

void dump_lua_stack(lua_State * lstate) {
	int index = 0;
	int length = 0;
	length = lua_gettop(lstate);
	fprintf(stderr,"-- stack --\n");
	for(index = length; index >= 1; index--) {
		switch(lua_type(lstate, index)) {
			case LUA_TNIL:      fprintf(stderr, "index: %d type: %s\n", index, "nil"); break;
			case LUA_TBOOLEAN:  fprintf(stderr, "index: %d type: %s\n", index, "boolean"); break;
			case LUA_TNUMBER:   fprintf(stderr, "index: %d type: %s\n", index, "number"); break;
			case LUA_TSTRING:   fprintf(stderr, "index: %d type: %s\n", index, "string"); break;
			case LUA_TTABLE:    fprintf(stderr, "index: %d type: %s\n", index, "table"); break;
			case LUA_TTHREAD:   fprintf(stderr, "index: %d type: %s\n", index, "thread"); break;
			case LUA_TUSERDATA: fprintf(stderr, "index: %d type: %s\n", index, "userdata"); break;
			case LUA_TFUNCTION: fprintf(stderr, "index: %d type: %s\n", index, "function"); break;
			default: exit_abt_safe("invalid type");
		}
	}
}
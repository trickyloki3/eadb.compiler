/*
 *   file: item.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
 #include "lua.h"
#include "lauxlib.h"
#include "time.h"
#include "db_search.h"
#include "script.h"

int table_getfieldstring(lua_State * lstate, const char * field, const char ** str, int table);

int main(int argc, char * argv[]) {
	item_t item;						/* item databse record holding all item information */
	script_t script;					/* primary structure that contain references to all resources */
	db_search_t db;						/* contains sqlite3 database queries to convert id to name */
	int item_count = 0;					/* total number of items translated */
	int item_status = 0;				/* status of sqlite3 item database iteration */
	int start_time = 0;					/* starting time of the compilation */
	char script_translation[BUF_SIZE];	/* hold the script translation */
	int script_offset = 0;
	FILE * description_file = NULL;		/* open file descriptor to the lua or txt file to produce */
	
	/* lua state */
	int table_index = 0;						/* the primary table index on the lua stack */
	lua_State * lstate = luaL_newstate();		/* the lua state maintains the lua stack */
	const char * description_format = NULL;
	const char * description_filename = NULL;
	const char * server_type = NULL;
	const char * resource_db_path = NULL;
	const char * server_db_path = NULL;
	int file_format_type = 0;

	if(lstate == NULL) 
		exit_abt_safe("failed to allocate lua state");
	if(luaL_loadfile(lstate, "itemc.cfg"))
		return exit_func_safe("item configuration syntax error; %s", lua_tostring(lstate, -1));
	if(lua_pcall(lstate, 0, 0, 0))
		return exit_func_safe("item configuration syntax error; %s", lua_tostring(lstate, -1));
	lua_getglobal(lstate, "compilation_table");
	if(!lua_istable(lstate, -1))
		return exit_abt_safe("the 'compilation_table' must be a lua table");

	lua_pushnil(lstate);
	while(lua_next(lstate, 1)) {
		/* save the current primary table index */
		table_index = lua_gettop(lstate);
		table_getfieldstring(lstate, "output_format", &description_format, table_index);
		table_getfieldstring(lstate, "output_filename", &description_filename, table_index);
		table_getfieldstring(lstate, "server_type", &server_type, table_index);
		table_getfieldstring(lstate, "resource_db_path", &resource_db_path, table_index);
		table_getfieldstring(lstate, "server_db_path", &server_db_path, table_index);

		/* initialize all structures */
		memset(&item, 0, sizeof(item_t));
		memset(&script, 0, sizeof(script_t));
		memset(&db, 0, sizeof(db_search_t));
		start_time = time(NULL);

		/* open the item description file */
		description_file = fopen(description_filename, "w");
		if(description_file == NULL)
			return exit_func_safe("fail to create %s", description_filename);

		/* connect the sqlite3 database to the script compiler */
		script.db = &db;
		if(ncs_strcmp(server_type,"eathena") == 0)
			script.mode = MODE_EATHENA;
		else if(ncs_strcmp(server_type,"rathena") == 0)
			script.mode = MODE_RATHENA;
		else if(ncs_strcmp(server_type,"hercules") == 0)
			script.mode = MODE_HERCULES;
		init_db(script.db, script.mode, resource_db_path, server_db_path);
		fprintf(stdout, "[info]: compiling database %s into item description table %s.\n", server_db_path, description_filename);

		if(ncs_strcmp(description_format, "txt") == 0)
			file_format_type = FORMAT_TXT;
		else if(ncs_strcmp(description_format, "lua") == 0)			
			file_format_type = FORMAT_LUA;

		/* load the item format information */

		/* process all items in database */
		item_status = item_iterate(script.db, &item);
		if(item_status == SQLITE_ROW) {
			while(item_status == SQLITE_ROW) {
				if(item.script != NULL && strlen(item.script) > 0) {
					/* compile the item script */
					fprintf(stderr,"\r%d\r", script.item_id);
					script_offset = 0;
					script.item_id = item.id;
					if(!script_lexical(&script.token, item.script)) {
						if(script_analysis(&script, &script.token, NULL, NULL) == SCRIPT_PASSED)
							if(!script_translate(&script))
								if(!script_bonus(&script))
									if(!script_generate(&script, script_translation, &script_offset))
										if(!script_generate_combo(script.item_id, script_translation, &script_offset, script.db, script.mode))
											;
					}
					script_block_reset(&script);	
					item_count++;
				}

				/* process the item */

				/* write the database record */
				if(script_offset > 0) {
					if(file_format_type & FORMAT_TXT) {
						fprintf(description_file,"%d#\n%s#\n", script.item_id, script_translation);
					} else if(file_format_type & FORMAT_LUA) {
						fprintf(description_file,"%d#\n%s#\n", script.item_id, script_translation);
					}
				}
				item_status = item_iterate(script.db, &item);
			}
		} else if(item_status == SQLITE_DONE) {
			exit_abt_safe("database contains zero item entries");
		} else {
			exit_abt_safe("failed to query item database");
		}
		fprintf(stderr,"[info]: compiled all %d items in %d seconds.\n", item_count, (int) (time(NULL) - start_time));

		/* free all the resources */
		deit_db(&db);
		fclose(description_file);
		script_block_finalize(&script);

		/* pop the current table and get the next table */
		lua_pop(lstate, 6);
	}
	lua_close(lstate);
	return 0;
}

int table_getfieldstring(lua_State * lstate, const char * field, const char ** str, int table) {
	lua_getfield(lstate, table, field);
	if(!lua_isstring(lstate, -1))
		return exit_abt_safe("the 'server_type' must have a string value");
	*str = lua_tolstring(lstate, -1, NULL);
	return CHECK_PASSED;
}
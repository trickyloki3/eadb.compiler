/*
 *   file: iteml.c
 *   date: 03/15/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "lua.h"
#include "lauxlib.h"
#include "db_search.h"

void path_concat(char * buffer, const char * base, const char * filename);
int table_getfieldstring(lua_State * lstate, const char * field, const char ** str, int table);
int table_getfieldtable(lua_State * lstate, const char * field, int table);
int load_eathena(lua_State * lstate, char * path, int table);
int load_rathena(lua_State * lstate, char * path, int table);
int load_hercules(lua_State * lstate, char * path, int table);
int load_resource(lua_State * lstate, char * path, int table);
void dump_lua_stack(lua_State * lstate);

int main(int argc, char * argv[]) {
	int table = 0;
	lua_State * lstate = NULL;
	const char * server_type = NULL;

	char database[BUF_SIZE];
	const char * database_name = NULL;
	const char * database_path = NULL;

	/* load the item configuration file */
	lstate = luaL_newstate();
	if(lstate == NULL) 
		exit_abt_safe("failed to allocate lua state");
	if(luaL_loadfile(lstate, "iteml.cfg"))
		return exit_func_safe("item configuration syntax error; %s", lua_tostring(lstate, -1));
	if(lua_pcall(lstate, 0, 0, 0))
		return exit_func_safe("item configuration syntax error; %s", lua_tostring(lstate, -1));
	lua_getglobal(lstate, "database_table");
	if(!lua_istable(lstate, -1))
		return exit_abt_safe("the 'database_table' must be a lua table");

	/* iterate through the table */
	lua_pushnil(lstate);
	while(lua_next(lstate, 1)) {
		/* load the database per the server type */
		table = lua_gettop(lstate);
		table_getfieldstring(lstate, "server_type", &server_type, table);
		table_getfieldstring(lstate, "database_name", &database_name, table);
		table_getfieldstring(lstate, "database_path", &database_path, table);
		path_concat(database, database_path, database_name);
		lua_pop(lstate, 2);
		if(ncs_strcmp(server_type, "eathena") == 0) {
			load_eathena(lstate, database, table);
		} else if(ncs_strcmp(server_type, "rathena_re") == 0) {
			load_rathena(lstate, database, table);
		} else if(ncs_strcmp(server_type, "hercules_re") == 0) {
			load_hercules(lstate, database, table);
		} else if(ncs_strcmp(server_type, "resource") == 0) {
			load_resource(lstate, database, table);
		} else {
			exit_func_safe("invalid server type %s", server_type);
		}
		lua_pop(lstate, 2);
	}

	/* release all resources */
	lua_close(lstate);
	return 0;
} 

void path_concat(char * buffer, const char * base, const char * filename) {
	int length = 0;
	int offset = 0;
	if(exit_null_safe(3, buffer, base, filename)) return;
	length = strlen(base);
	offset = (base[length - 1] != '/')?
		sprintf(buffer,"%s%c%s", base, '/', filename):
		sprintf(buffer,"%s%s", base, filename);
	buffer[offset] = '\0';
}

int table_getfieldstring(lua_State * lstate, const char * field, const char ** str, int table) {
	lua_getfield(lstate, table, field);
	if(!lua_isstring(lstate, -1))
		return exit_abt_safe("the 'server_type' must have a string value");
	*str = lua_tolstring(lstate, -1, NULL);
	return CHECK_PASSED;
}

int table_getfieldtable(lua_State * lstate, const char * field, int table) {
	lua_getfield(lstate, table, field);
	if(!lua_istable(lstate, -1))
		return exit_func_safe("the '%s' must be a lua table", field);
	return lua_gettop(lstate);
}

int load_eathena(lua_State * lstate, char * path, int table) {
	db_ea_t db;
	char server_database_path[BUF_SIZE];
	const char * server_path = NULL;
	const char * database_name = NULL;
	int item_group_table = 0;

	memset(&db, 0, sizeof(db_ea_t));
	table_getfieldstring(lstate, "server_path", &server_path, table);
	create_eathena_database(&db, path);
	fprintf(stdout, "[info]: creating eathena database %s\n", path);

	table_getfieldstring(lstate, "item_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	item_ea_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "mob_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	mob_ea_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "skill_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	skill_ea_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "produce_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	produce_ea_sql_load(&db, server_database_path);	
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "mercenary_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	mercenary_ea_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "pet_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	pet_ea_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "const_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	const_ea_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	item_group_table = table_getfieldtable(lstate, "item_group_db_filename", table);
	lua_pushnil(lstate);
	while(lua_next(lstate, item_group_table)) {
		path_concat(server_database_path, server_path, lua_tolstring(lstate, -1, NULL));
		item_group_ea_sql_load(&db, server_database_path);
		fprintf(stdout, "[info]: loading %s\n", server_database_path);
		lua_pop(lstate, 1);
	}
	lua_pop(lstate, 1);

	finalize_eathena_database(&db);
	lua_pop(lstate, 1);
	return CHECK_PASSED;
}

int load_rathena(lua_State * lstate, char * path, int table) {
	db_ra_t db;
	db_ra_aux_t db_search;
	char server_database_path[BUF_SIZE];
	const char * server_path = NULL;
	const char * database_name = NULL;
	int item_group_table = 0;

	memset(&db, 0, sizeof(db_ra_t));
	memset(&db, 0, sizeof(db_ra_aux_t));
	table_getfieldstring(lstate, "server_path", &server_path, table);
	create_rathena_database(&db, path);
	fprintf(stdout, "[info]: creating rathena database %s\n", path);

	table_getfieldstring(lstate, "item_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	item_ra_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "mob_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	mob_ra_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "skill_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	skill_ra_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "produce_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	produce_ra_sql_load(&db, server_database_path);	
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "mercenary_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	mercenary_ra_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "pet_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	pet_ra_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "const_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	const_ra_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	init_ra_search(&db, &db_search);
	item_group_table = table_getfieldtable(lstate, "item_group_db_filename", table);
	lua_pushnil(lstate);
	while(lua_next(lstate, item_group_table)) {
		path_concat(server_database_path, server_path, lua_tolstring(lstate, -1, NULL));
		item_group_ra_sql_load(&db, server_database_path, &db_search);
		fprintf(stdout, "[info]: loading %s\n", server_database_path);
		lua_pop(lstate, 1);
	}
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "package_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	item_package_ra_sql_load(&db, server_database_path, &db_search);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "combo_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	item_combo_ra_sql_load(&db, server_database_path, &db_search);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	deit_ra_search(&db_search);
	finalize_rathena_database(&db);
	lua_pop(lstate, 1);
	return CHECK_PASSED;
}

int load_hercules(lua_State * lstate, char * path, int table) {
	db_he_t db;
	db_he_aux_t db_search;
	char server_database_path[BUF_SIZE];
	const char * server_path = NULL;
	const char * database_name = NULL;

	memset(&db, 0, sizeof(db_he_t));
	memset(&db_search, 0, sizeof(db_he_aux_t));
	table_getfieldstring(lstate, "server_path", &server_path, table);
	create_hercules_database(&db, path);
	fprintf(stdout, "[info]: creating hercules database %s\n", path);

	table_getfieldstring(lstate, "item_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	item_he_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "mob_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	mob_he_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "skill_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	skill_he_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "produce_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	produce_he_sql_load(&db, server_database_path);	
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "mercenary_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	mercenary_he_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "pet_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	pet_he_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "const_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	const_he_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	init_he_search(&db, &db_search);
	table_getfieldstring(lstate, "combo_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	item_combo_he_sql_load(&db, server_database_path, &db_search);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);
	deit_he_search(&db_search);
	finalize_hercules_database(&db);
	lua_pop(lstate, 1);
	return CHECK_PASSED;
}

int load_resource(lua_State * lstate, char * path, int table) {
	db_res_t db;
	char server_database_path[BUF_SIZE];
	const char * server_path = NULL;
	const char * database_name = NULL;

	memset(&db, 0, sizeof(db_res_t));
	table_getfieldstring(lstate, "server_path", &server_path, table);
	create_resource_database(&db, path);
	fprintf(stdout, "[info]: creating resources database %s\n", path);

	table_getfieldstring(lstate, "option_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	resource_option_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "map_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	resource_map_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "bonus_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	resource_bonus_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "status_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	resource_status_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "var_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	resource_var_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	table_getfieldstring(lstate, "block_db_filename", &database_name, table);
	path_concat(server_database_path, server_path, database_name);
	resource_block_sql_load(&db, server_database_path);
	fprintf(stdout, "[info]: loading %s\n", server_database_path);
	lua_pop(lstate, 1);

	finalize_resource_database(&db);
	lua_pop(lstate, 1);
	return CHECK_PASSED;
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

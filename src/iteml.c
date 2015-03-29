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
	int config_table = 0;
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

	/* push the database table onto the stack */
	lua_getglobal(lstate, "database_table");
	if(lua_isnil(lstate, -1))
		return exit_abt_safe("missing 'database_table' from configuration");
	if(!lua_istable(lstate, -1))
		return exit_abt_safe("the 'database_table' must be a table value");
	config_table = lua_gettop(lstate);

	/* iterate through the table */
	lua_pushnil(lstate);
	while(lua_next(lstate, 1)) {
		/* check that the configuration have table values */
		if(!lua_istable(lstate, -1)) {
			exit_abt_safe("the 'database_path' must only have table values");
			lua_pop(lstate, lua_gettop(lstate) - config_table - 1);
			continue;
		}

		/* load the database per the server type */
		table = lua_gettop(lstate);
		if(table_getfieldstring(lstate, "server_type", &server_type, table)) {
			exit_abt_safe("server type ('server_type') missing from configuration");
			lua_pop(lstate, lua_gettop(lstate) - config_table - 1);
			continue;
		}
		if(table_getfieldstring(lstate, "database_name", &database_name, table)) {
			exit_abt_safe("database name ('database_name') missing from configuration");
			lua_pop(lstate, lua_gettop(lstate) - config_table - 1);
			continue;
		}
		if(table_getfieldstring(lstate, "database_path", &database_path, table)) {
			exit_abt_safe("database path ('database_path') missing from configuration");
			lua_pop(lstate, lua_gettop(lstate) - config_table - 1);
			continue;	
		}

		path_concat(database, database_path, database_name);
		
		if(ncs_strcmp(server_type, "eathena") == 0) {
			if(load_eathena(lstate, database, table))
				exit_abt_safe("failed to load eathena database files");
		} else if(ncs_strcmp(server_type, "rathena") == 0) {
			if(load_rathena(lstate, database, table))
				exit_abt_safe("failed to load rathena database files");
		} else if(ncs_strcmp(server_type, "hercules") == 0) {
			if(load_hercules(lstate, database, table))
				exit_abt_safe("failed to load hercules database files");
		} else if(ncs_strcmp(server_type, "resource") == 0) {
			if(load_resource(lstate, database, table))
				exit_abt_safe("failed to load resource database files");
		} else {
			exit_func_safe("invalid server type '%s'; only 'eathena', "
			"'rathena', 'hercules', and 'resource' is supported", server_type);
		}

		/* pop everything except the last key and configuration table */
		lua_pop(lstate, lua_gettop(lstate) - config_table - 1);
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
	if(lua_isnil(lstate, -1)) {
		exit_func_safe("failed to find '%s'", field);
		return CHECK_FAILED;
	}
	if(!lua_isstring(lstate, -1)) {
		exit_func_safe("the '%s' must have a string value", field);
		return CHECK_FAILED;
	}
	*str = lua_tolstring(lstate, -1, NULL);
	return CHECK_PASSED;
}

int table_getfieldtable(lua_State * lstate, const char * field, int table) {
	lua_getfield(lstate, table, field);
	if(lua_isnil(lstate, -1)) {
		exit_func_safe("failed to find '%s'", field);
		return CHECK_FAILED;
	}
	if(!lua_istable(lstate, -1)) {
		exit_func_safe("the '%s' must be a lua table", field);
		return CHECK_FAILED;
	}
	return lua_gettop(lstate);
}

int load_eathena(lua_State * lstate, char * path, int table) {
	db_ea_t db;
	char server_database_path[BUF_SIZE];
	const char * server_path = NULL;
	const char * database_name = NULL;
	int item_group_table = 0;
	memset(&db, 0, sizeof(db_ea_t));

	if(table_getfieldstring(lstate, "server_path", &server_path, table)) {
		exit_abt_safe("server database path ('server_path') is missing from database");
		goto load_eathena_fail;
	} else {
		fprintf(stdout, "[info]: creating eathena database %s.\n", path);
		if(create_eathena_database(&db, path)) {
			exit_func_safe("failed to create rathena database on path %s", path);
			goto load_eathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "item_db_filename", &database_name, table)) {
		exit_abt_safe("item database file name ('item_db_filename') missing from configuration");
		goto load_eathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(item_ea_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load item database on path %s", server_database_path);
			goto load_eathena_fail;
		}
	}
	
	if(table_getfieldstring(lstate, "mob_db_filename", &database_name, table)) {
		exit_abt_safe("mob database file name ('mob_db_filename') missing from configuration");
		goto load_eathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(mob_ea_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load mob database on path %s", server_database_path);
			goto load_eathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "skill_db_filename", &database_name, table)) {
		exit_abt_safe("skill database file name ('skill_db_filename') missing from configuration");
		goto load_eathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(skill_ea_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load skill database on path %s", server_database_path);
			goto load_eathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "produce_db_filename", &database_name, table)) {
		exit_abt_safe("produce database file name ('produce_db_filename') missing from configuration");
		goto load_eathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(produce_ea_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load produce database on path %s", server_database_path);
			goto load_eathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "mercenary_db_filename", &database_name, table)) {
		exit_abt_safe("mercenary database file name ('mercenary_db_filename') missing from configuration");
		goto load_eathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(mercenary_ea_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load mercenary database on path %s", server_database_path);
			goto load_eathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "pet_db_filename", &database_name, table)) {
		exit_abt_safe("pet database file name ('pet_db_filename') missing from configuration");
		goto load_eathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(pet_ea_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load pet database on path %s", server_database_path);
			goto load_eathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "const_db_filename", &database_name, table)) {
		exit_abt_safe("constant database file name ('const_db_filename') missing from configuration");
		goto load_eathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(const_ea_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to constant pet database on path %s", server_database_path);
			goto load_eathena_fail;
		}
	}

	item_group_table = table_getfieldtable(lstate, "item_group_db_filename", table);
	if(item_group_table != lua_gettop(lstate)) {
		exit_abt_safe("item group database files ('item_group_db_filename') missing from configuration");
		goto load_eathena_fail;
	}
	lua_pushnil(lstate);
	while(lua_next(lstate, item_group_table)) {
		/* check the value is string value */
		if(lua_type(lstate, -1) != LUA_TSTRING) {
			exit_abt_safe("item group database file ('item_group_db_filename') must only have string value");
			goto load_eathena_fail;
		} else {
			database_name = lua_tostring(lstate, -1);
			path_concat(server_database_path, server_path, database_name);
			fprintf(stdout, "[info]: loading %s.\n", server_database_path);
			if(item_group_ea_sql_load(&db, server_database_path)) {
				exit_func_safe("failed to load item group database on path %s", server_database_path);
				goto load_eathena_fail;
			}
		}
		lua_pop(lstate, lua_gettop(lstate) - item_group_table - 1);
	}

	finalize_eathena_database(&db);
	return CHECK_PASSED;

	load_eathena_fail:
		finalize_eathena_database(&db);
		return CHECK_FAILED;
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

	if(table_getfieldstring(lstate, "server_path", &server_path, table)) {
		exit_abt_safe("server database path ('server_path') is missing from database");
		goto load_rathena_fail;
	} else {
		fprintf(stdout, "[info]: creating rathena database %s.\n", path);
		if(create_rathena_database(&db, path)) {
			exit_func_safe("failed to create rathena database on path %s", path);
			goto load_rathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "item_db_filename", &database_name, table)) {
		exit_abt_safe("item database file name ('item_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(item_ra_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load item database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}
	
	if(table_getfieldstring(lstate, "mob_db_filename", &database_name, table)) {
		exit_abt_safe("mob database file name ('mob_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(mob_ra_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load mob database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "skill_db_filename", &database_name, table)) {
		exit_abt_safe("skill database file name ('skill_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(skill_ra_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load skill database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "produce_db_filename", &database_name, table)) {
		exit_abt_safe("produce database file name ('produce_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(produce_ra_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load produce database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}

	if(table_getfieldstring(lstate, "mercenary_db_filename", &database_name, table)) {
		exit_abt_safe("mercenary database file name ('mercenary_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(mercenary_ra_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load mercenary database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}
	
	if(table_getfieldstring(lstate, "pet_db_filename", &database_name, table)) {
		exit_abt_safe("pet database file name ('pet_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(pet_ra_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load pet database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}
	
	if(table_getfieldstring(lstate, "const_db_filename", &database_name, table)) {
		exit_abt_safe("constant database file name ('const_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(const_ra_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load constant database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}

	init_ra_search(&db, &db_search);
	item_group_table = table_getfieldtable(lstate, "item_group_db_filename", table);
	if(item_group_table != lua_gettop(lstate)) {
		exit_abt_safe("item group database files ('item_group_db_filename') missing from configuration");
		goto load_rathena_fail;
	}
	lua_pushnil(lstate);
	while(lua_next(lstate, item_group_table)) {
		/* check the value is string value */
		if(lua_type(lstate, -1) != LUA_TSTRING) {
			exit_abt_safe("item group database file ('item_group_db_filename') must only have string value");
			goto load_rathena_fail;
		} else {
			database_name = lua_tostring(lstate, -1);
			path_concat(server_database_path, server_path, database_name);
			fprintf(stdout, "[info]: loading %s.\n", server_database_path);
			if(item_group_ra_sql_load(&db, server_database_path, &db_search)) {
				exit_func_safe("failed to load item group database on path %s", server_database_path);
				goto load_rathena_fail;
			}
		}
		lua_pop(lstate, lua_gettop(lstate) - item_group_table - 1);
	}
	

	if(table_getfieldstring(lstate, "combo_db_filename", &database_name, table)) {
		exit_abt_safe("item combo database file name ('combo_db_filename') missing from configuration");
		goto load_rathena_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(item_combo_ra_sql_load(&db, server_database_path, &db_search)) {
			exit_func_safe("failed to load item combo database on path %s", server_database_path);
			goto load_rathena_fail;
		}
	}

	deit_ra_search(&db_search);
	finalize_rathena_database(&db);
	return CHECK_PASSED;

	load_rathena_fail:
		deit_ra_search(&db_search);
		finalize_rathena_database(&db);
		return CHECK_FAILED;
}

int load_hercules(lua_State * lstate, char * path, int table) {
	db_he_t db;
	db_he_aux_t db_search;
	char server_database_path[BUF_SIZE];
	const char * server_path = NULL;
	const char * database_name = NULL;

	memset(&db, 0, sizeof(db_he_t));
	memset(&db_search, 0, sizeof(db_he_aux_t));

	if(table_getfieldstring(lstate, "server_path", &server_path, table)) {
		exit_abt_safe("server database path ('server_path') is missing from database");
		goto load_hercules_fail;
	} else {
		fprintf(stdout, "[info]: creating hercules database %s\n", path);
		if(create_hercules_database(&db, path)) {
			exit_func_safe("failed to create hercules database on path %s", path);
			goto load_hercules_fail;
		}
	}

	if(table_getfieldstring(lstate, "item_db_filename", &database_name, table)) {
		exit_abt_safe("item database file name ('item_db_filename') missing from configuration");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(item_he_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load item database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}

	if(table_getfieldstring(lstate, "mob_db_filename", &database_name, table)) {
		exit_abt_safe("mob database path ('mob_db_filename') is missing from database");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(mob_he_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load mob database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}

	if(table_getfieldstring(lstate, "skill_db_filename", &database_name, table)) {
		exit_abt_safe("skill database path ('skill_db_filename') is missing from database");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(skill_he_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load skill database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}

	if(table_getfieldstring(lstate, "produce_db_filename", &database_name, table)) {
		exit_abt_safe("produce database path ('produce_db_filename') is missing from database");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(produce_he_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load produce database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}
	

	if(table_getfieldstring(lstate, "mercenary_db_filename", &database_name, table)) {
		exit_abt_safe("mercenary database path ('mercenary_db_filename') is missing from database");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(mercenary_he_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load mercenary database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}

	if(table_getfieldstring(lstate, "pet_db_filename", &database_name, table)) {
		exit_abt_safe("pet database path ('pet_db_filename') is missing from database");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(pet_he_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load pet database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}
	
	if(table_getfieldstring(lstate, "const_db_filename", &database_name, table)) {
		exit_abt_safe("constant database path ('const_db_filename') is missing from database");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(const_he_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load constant database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}
	
	init_he_search(&db, &db_search);
	if(table_getfieldstring(lstate, "combo_db_filename", &database_name, table)) {
		exit_abt_safe("item combo database path ('combo_db_filename') is missing from database");
		goto load_hercules_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(item_combo_he_sql_load(&db, server_database_path, &db_search)) {
			exit_func_safe("failed to load item combo database on path %s", server_database_path);
			goto load_hercules_fail;
		}
	}
	deit_he_search(&db_search);

	finalize_hercules_database(&db);
	return CHECK_PASSED;

	load_hercules_fail:
		deit_he_search(&db_search);
		finalize_hercules_database(&db);
		return CHECK_FAILED;
}

int load_resource(lua_State * lstate, char * path, int table) {
	db_res_t db;
	char server_database_path[BUF_SIZE];
	const char * server_path = NULL;
	const char * database_name = NULL;
	memset(&db, 0, sizeof(db_res_t));

	if(table_getfieldstring(lstate, "server_path", &server_path, table)) {
		exit_abt_safe("server database path ('server_path') is missing from database");
		goto load_resource_fail;
	} else {
		fprintf(stdout, "[info]: creating resources database %s\n", path);
		if(create_resource_database(&db, path)) {
			exit_func_safe("failed to create resources database on path %s", path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "option_db_filename", &database_name, table)) {
		exit_abt_safe("option database path ('option_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_option_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load option database on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "map_db_filename", &database_name, table)) {
		exit_abt_safe("map database path ('map_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_map_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load map database on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "bonus_db_filename", &database_name, table)) {
		exit_abt_safe("bonus database path ('bonus_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_bonus_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load bonus database on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "status_db_filename", &database_name, table)) {
		exit_abt_safe("status database path ('status_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_status_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load status database on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "var_db_filename", &database_name, table)) {
		exit_abt_safe("variable and function database path ('var_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_var_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load variable and function database on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "block_db_filename", &database_name, table)) {
		exit_abt_safe("block database path ('block_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_block_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load block database on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "idres_db_filename", &database_name, table)) {
		exit_abt_safe("identified item resource table path ('idres_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_id_res_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load identified item resource table on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	if(table_getfieldstring(lstate, "numidres_db_filename", &database_name, table)) {
		exit_abt_safe("unidentified item resource table path ('numidres_db_filename') is missing from database");
		goto load_resource_fail;
	} else {
		path_concat(server_database_path, server_path, database_name);
		fprintf(stdout, "[info]: loading %s.\n", server_database_path);
		if(resource_num_id_res_sql_load(&db, server_database_path)) {
			exit_func_safe("failed to load unidentified item resource table on path %s", server_database_path);
			goto load_resource_fail;
		}
	}

	finalize_resource_database(&db);
	return CHECK_PASSED;

	load_resource_fail:
		finalize_resource_database(&db);
		return CHECK_FAILED;
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

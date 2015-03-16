/*
 *   file: db_search.c
 *   date: 03/08/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "db_search.h"

int init_db(db_search_t * db, int mode, const char * resource_path, const char * database_path) {
	int status = 0;
	const char * error = NULL;
	memset(db, 0, sizeof(db_search_t));

	/* open database connection to resource database */
	if(sqlite3_open(resource_path, &db->resource) != SQLITE_OK) {
		/* print sqlite3 error before exiting */
		status = sqlite3_errcode(db->resource);
		error = sqlite3_errmsg(db->resource);
		exit_func_safe("sqlite3 code %d; %s", status, error);
		if(db->resource != NULL) sqlite3_close(db->resource);
		return CHECK_FAILED;
	}

	/* open connection to athena database */
	if(sqlite3_open(database_path, &db->athena) != SQLITE_OK) {
		/* print sqlite3 error before exiting */
		status = sqlite3_errcode(db->athena);
		error = sqlite3_errmsg(db->athena);
		exit_func_safe("sqlite3 code %d; %s", status, error);
		if(db->athena != NULL) sqlite3_close(db->athena);
		return CHECK_FAILED;
	}

	/* build all resource search queries */
	if( sqlite3_prepare_v2(db->resource, option_res_name_sql, 	strlen(option_res_name_sql),&db->option_res_name_search, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->resource, map_res_name_sql, 		strlen(map_res_name_sql), 	&db->map_res_name_search, 			NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->resource, map_res_id_sql, 		strlen(map_res_id_sql), 	&db->map_res_id_search, 			NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->resource, bonus_search_sql, 		strlen(bonus_search_sql), 	&db->bonus_res_prefix_name_search, 	NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->resource, status_search_sql, 	strlen(status_search_sql), 	&db->status_res_id_name_search, 	NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->resource, var_search_sql, 		strlen(var_search_sql), 	&db->var_res_id_search, 			NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->resource, block_res_key_sql, 	strlen(block_res_key_sql), 	&db->block_res_key_search, 			NULL)) {
			/* failed to load resource queries */
			status = sqlite3_errcode(db->resource);
			error = sqlite3_errmsg(db->resource);
			exit_func_safe("sqlite3 code %d; %s", status, error);
			if(db->resource != NULL) sqlite3_close(db->resource);
			if(db->athena != NULL) sqlite3_close(db->athena);
			return CHECK_FAILED;
		}

	/* build all athena search queries */
	db->mode = mode;
	switch(mode) {
		case MODE_EATHENA:
			if(sqlite3_prepare_v2(db->athena, item_ea_iterate, strlen(item_ea_iterate), &db->item_iterate, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, const_ea_name_search, strlen(const_ea_name_search), &db->const_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, const_ea_id_search, strlen(const_ea_id_search), &db->const_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, skill_ea_name_search, strlen(skill_ea_name_search), &db->skill_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, skill_ea_id_search, strlen(skill_ea_id_search), &db->skill_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_ea_name_search, strlen(item_ea_name_search), &db->item_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_ea_id_search, strlen(item_ea_id_search), &db->item_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, mob_ea_id_search, strlen(mob_ea_id_search), &db->mob_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, merc_ea_id_search, strlen(merc_ea_id_search), &db->merc_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, pet_ea_id_search, strlen(pet_ea_id_search), &db->pet_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, produce_ea_id_search, strlen(produce_ea_id_search), &db->produce_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, ea_item_group_search, strlen(ea_item_group_search), &db->item_group_id_search, NULL) != SQLITE_OK)
				goto abort;
			break;
		case MODE_RATHENA:
			if(sqlite3_prepare_v2(db->athena, item_ra_iterate, strlen(item_ra_iterate), &db->item_iterate, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, const_ra_name_search, strlen(const_ra_name_search), &db->const_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, const_ra_id_search, strlen(const_ra_id_search), &db->const_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, skill_ra_name_search, strlen(skill_ra_name_search), &db->skill_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, skill_ra_id_search, strlen(skill_ra_id_search), &db->skill_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_ra_name_search, strlen(item_ra_name_search), &db->item_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_ra_id_search, strlen(item_ra_id_search), &db->item_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, mob_ra_id_search, strlen(mob_ra_id_search), &db->mob_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, merc_ra_id_search, strlen(merc_ra_id_search), &db->merc_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, pet_ra_id_search, strlen(pet_ra_id_search), &db->pet_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, produce_ra_id_search, strlen(produce_ra_id_search), &db->produce_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, ra_item_group_search, strlen(ra_item_group_search), &db->item_group_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_combo_ra_id_search, strlen(item_combo_ra_id_search), &db->item_combo_id_search, NULL) != SQLITE_OK)
				goto abort;
			break;
		case MODE_HERCULES:
			if(sqlite3_prepare_v2(db->athena, item_he_iterate, strlen(item_he_iterate), &db->item_iterate, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, const_he_name_search, strlen(const_he_name_search), &db->const_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, const_he_id_search, strlen(const_he_id_search), &db->const_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, skill_he_name_search, strlen(skill_he_name_search), &db->skill_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, skill_he_id_search, strlen(skill_he_id_search), &db->skill_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_he_name_search, strlen(item_he_name_search), &db->item_db_name_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_he_id_search, strlen(item_he_id_search), &db->item_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, mob_he_id_search, strlen(mob_he_id_search), &db->mob_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, merc_he_id_search, strlen(merc_he_id_search), &db->merc_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, pet_he_id_search, strlen(pet_he_id_search), &db->pet_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, produce_he_id_search, strlen(produce_he_id_search), &db->produce_db_id_search, NULL) != SQLITE_OK ||
			   sqlite3_prepare_v2(db->athena, item_combo_he_id_search, strlen(item_combo_he_id_search), &db->item_combo_id_search, NULL) != SQLITE_OK)
				goto abort;
			break;
		default:
			break;
	}
	return CHECK_PASSED;

	abort:
		/* failed to load resource queries */
		status = sqlite3_errcode(db->resource);
		error = sqlite3_errmsg(db->resource);
		exit_func_safe("sqlite3 code %d; %s", status, error);
		if(db->resource != NULL) sqlite3_close(db->resource);
		if(db->athena != NULL) sqlite3_close(db->athena);
		return CHECK_FAILED;
}

int deit_db(db_search_t * db) {
	sqlite3_finalize(db->item_iterate);
	sqlite3_finalize(db->option_res_name_search);
	sqlite3_finalize(db->map_res_name_search);
	sqlite3_finalize(db->map_res_id_search);
	sqlite3_finalize(db->bonus_res_prefix_name_search);
	sqlite3_finalize(db->status_res_id_name_search);
	sqlite3_finalize(db->var_res_id_search);
	sqlite3_finalize(db->block_res_key_search);
	sqlite3_finalize(db->const_db_name_search);
	sqlite3_finalize(db->const_db_id_search);
	sqlite3_finalize(db->skill_db_name_search);
	sqlite3_finalize(db->skill_db_id_search);
	sqlite3_finalize(db->item_db_name_search);
	sqlite3_finalize(db->item_db_id_search);
	sqlite3_finalize(db->mob_db_id_search);
	sqlite3_finalize(db->merc_db_id_search);
	sqlite3_finalize(db->pet_db_id_search);
	sqlite3_finalize(db->produce_db_id_search);
	sqlite3_finalize(db->item_group_id_search);
	sqlite3_finalize(db->item_combo_id_search);
	sqlite3_close(db->resource);
	sqlite3_close(db->athena);
	return CHECK_PASSED;
}

int test_db(void) {
	db_search_t 	db;
	option_res 		option;
	map_res 		map;
	bonus_res 		bonus;
	status_res 		status;
	var_res 		var;
	block_res 		block;
	
	const_t 		const_search;
	skill_t 		skill_search;
	item_t 			item_search;
	mob_t 			mob_search;
	merc_t 			merc_search;
	pet_t 			pet_search;
	produce_t *		produce_search = NULL;
	item_group_t	item_group_search;
	combo_t * 		combo_search;
	char buffer[BUF_SIZE];

	init_db(&db, MODE_RATHENA, "/root/Desktop/dev/eAdb.Compiler3/resources.db", "/root/Desktop/dev/eAdb.Compiler3/rathena.db");
	/* resource query test */
	fprintf(stderr,"[test]: %-25s ... %6s\n", "option_search_name", 		(option_search_name(&db, &option, "Option_Nothing") == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "map_search_name",  			(map_search_name(&db, &map, "moro_vol") == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "map_search_id", 				(map_search_id(&db, &map, 3) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "bonus_search_name", 			(bonus_search_name(&db, &bonus, "bonus3", "bSPVanishRate") == CHECK_PASSED) ? "PASSED" : "FAILED");	
	fprintf(stderr,"[test]: %-25s ... %6s\n", "status_search_id_name", 		(status_search_id_name(&db, &status, 1, "sc_freeze") == CHECK_PASSED) ? "PASSED" : "FAILED");	
	fprintf(stderr,"[test]: %-25s ... %6s\n", "var_search_id", 				(var_search_id(&db, &var, "getrefine") == CHECK_PASSED) ? "PASSED" : "FAILED");	
	fprintf(stderr,"[test]: %-25s ... %6s\n", "block_search_name", 			(block_search_name(&db, &block, "if") == CHECK_PASSED) ? "PASSED" : "FAILED");
	/* rathena query test */	
	fprintf(stderr,"[test]: %-25s ... %6s\n", "const_db_search_name", 		(const_db_search_name(&db, &const_search, "Job_Gunslinger") == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "const_db_search_id", 		(const_db_search_id(&db, &const_search, 24) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "skill_db_search_name", 		(skill_db_search_name(&db, &skill_search, "SM_SWORD") == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "skill_db_search_id", 		(skill_db_search_id(&db, &skill_search, 2) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "item_db_search_name", 		(item_db_search_name(&db, &item_search, "Red_Potion") == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "item_db_search_id", 			(item_db_search_id(&db, &item_search, 501) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "mob_db_search_id", 			(mob_db_search_id(&db, &mob_search, 1001) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "merc_db_search_id", 			(merc_db_search_id(&db, &merc_search, 1191) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "pet_db_search_id", 			(pet_db_search_id(&db, &pet_search, 1002) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "produce_db_search_id", 		(produce_db_search_id(&db, &produce_search, 1) == CHECK_PASSED) ? "PASSED" : "FAILED");
	produce_free(produce_search);
	fprintf(stderr,"[test]: %-25s ... %6s\n", "item_group_db_search_id", 	(item_group_db_search_id(&db, &item_group_search, 1, buffer) == CHECK_PASSED) ? "PASSED" : "FAILED");
	fprintf(stderr,"[test]: %-25s ... %6s\n", "item_combo_db_search_id", 	(item_combo_db_search_id(&db, &combo_search, 1166) == CHECK_PASSED) ? "PASSED" : "FAILED");
	combo_free(combo_search);
	deit_db(&db);
	return 0;
}

int item_iterate(db_search_t * db, item_t * item) {
	int status = 0;
	status = sqlite3_step(db->item_iterate);
	memset(item, 0, sizeof(item_t));
    switch(db->mode) {
        case MODE_EATHENA:
            if(status == SQLITE_ROW) {
            	item->id = sqlite3_column_int(db->item_iterate, 0);
                strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(db->item_iterate, 1));
                item->type = sqlite3_column_int(db->item_iterate, 2);
                item->buy = sqlite3_column_int(db->item_iterate, 3);
                item->sell = sqlite3_column_int(db->item_iterate, 4);
                if(item->sell <= 0) item->sell = item->buy / 2;
                if(item->buy <= 0) item->buy = item->sell * 2;
                item->weight = sqlite3_column_int(db->item_iterate, 5);
                item->atk = sqlite3_column_int(db->item_iterate, 6);
                item->matk = -1;
                item->def = sqlite3_column_int(db->item_iterate, 7);
                item->range = sqlite3_column_int(db->item_iterate, 8);
                item->slots = sqlite3_column_int(db->item_iterate, 9);
                item->job = sqlite3_column_int(db->item_iterate, 10);
                item->upper = sqlite3_column_int(db->item_iterate, 11);
                item->gender = sqlite3_column_int(db->item_iterate, 12);
                item->loc = sqlite3_column_int(db->item_iterate, 13);
                item->wlv = sqlite3_column_int(db->item_iterate, 14);
                item->elv_min = sqlite3_column_int(db->item_iterate, 15);
                item->elv_max = item->elv_min;
                item->refineable = sqlite3_column_int(db->item_iterate, 16);
                item->view = sqlite3_column_int(db->item_iterate, 17);
                strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(db->item_iterate, 18));
            }
            break;
        case MODE_RATHENA:
            if(status == SQLITE_ROW) {
                item->id = sqlite3_column_int(db->item_iterate, 0);
                strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(db->item_iterate, 2));
                strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(db->item_iterate, 20));
            }
            break;
        case MODE_HERCULES:
            if(status == SQLITE_ROW) {
                item->id = sqlite3_column_int(db->item_iterate, 0);
                strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(db->item_iterate, 2));
                strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(db->item_iterate, 39));
            }
            break;
    }
    return status;
}

int option_search_name(db_search_t * db, option_res * option, const char * name) {
	int status = 0;
	if(exit_null_safe(3, db, option, name)) return CHECK_FAILED;
	sqlite3_clear_bindings(db->option_res_name_search);
	sqlite3_bind_text(db->option_res_name_search, 1, name, strlen(name), SQLITE_STATIC);
	status = sqlite3_step(db->option_res_name_search);
	if(status == SQLITE_ROW) {
		strncopy(option->option, 	MAX_NAME_SIZE, sqlite3_column_text(db->option_res_name_search, 0));
		strncopy(option->name, 		MAX_NAME_SIZE, sqlite3_column_text(db->option_res_name_search, 1));
	}
	if(sqlite3_reset(db->option_res_name_search) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->resource), sqlite3_errmsg(db->resource));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int map_search_name(db_search_t * db, map_res * map, const char * name) {
	int status = 0;
	if(exit_null_safe(3, db, map, name)) return CHECK_FAILED;
	sqlite3_clear_bindings(db->map_res_name_search);
	sqlite3_bind_text(db->map_res_name_search, 1, name, strlen(name), SQLITE_STATIC);
	status = sqlite3_step(db->map_res_name_search);
	if(status == SQLITE_ROW) {
		map->id = sqlite3_column_int(db->map_res_name_search, 0);
		strncopy(map->map, 	MAX_NAME_SIZE, sqlite3_column_text(db->map_res_name_search, 1));
		strncopy(map->name, MAX_NAME_SIZE, sqlite3_column_text(db->map_res_name_search, 2));
	}
	if(sqlite3_reset(db->map_res_name_search) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->resource), sqlite3_errmsg(db->resource));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int map_search_id(db_search_t * db, map_res * map, int id) {
	int status = 0;
	if(exit_null_safe(2, db, map)) return CHECK_FAILED;
	sqlite3_clear_bindings(db->map_res_id_search);
	sqlite3_bind_int(db->map_res_id_search, 1, id);
	status = sqlite3_step(db->map_res_id_search);
	if(status == SQLITE_ROW) {
		map->id = sqlite3_column_int(db->map_res_id_search, 0);
		strncopy(map->map, 	MAX_NAME_SIZE, sqlite3_column_text(db->map_res_id_search, 1));
		strncopy(map->name, MAX_NAME_SIZE, sqlite3_column_text(db->map_res_id_search, 2));
	}
	if(sqlite3_reset(db->map_res_id_search) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->resource), sqlite3_errmsg(db->resource));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int bonus_search_name(db_search_t * db, bonus_res * bonus, const char * prefix, const char * name) {
	int status = 0;
	if(exit_null_safe(4, db, bonus, prefix, name)) return CHECK_FAILED;
	sqlite3_clear_bindings(db->bonus_res_prefix_name_search);
	sqlite3_bind_text(db->bonus_res_prefix_name_search, 1, prefix, strlen(prefix), SQLITE_STATIC);
	sqlite3_bind_text(db->bonus_res_prefix_name_search, 2, name, strlen(name), SQLITE_STATIC);
	status = sqlite3_step(db->bonus_res_prefix_name_search);
	if(status == SQLITE_ROW) {
		bonus->id = sqlite3_column_int(db->bonus_res_prefix_name_search, 0);
		bonus->flag = sqlite3_column_int(db->bonus_res_prefix_name_search, 1);
		bonus->attr = sqlite3_column_int(db->bonus_res_prefix_name_search, 2);
		strncopy(bonus->prefix, MAX_NAME_SIZE, sqlite3_column_text(db->bonus_res_prefix_name_search, 3));
		strncopy(bonus->bonus, MAX_NAME_SIZE, sqlite3_column_text(db->bonus_res_prefix_name_search, 4));
		strncopy(bonus->format, MAX_NAME_SIZE, sqlite3_column_text(db->bonus_res_prefix_name_search, 5));
		bonus->type_cnt = sqlite3_column_int(db->bonus_res_prefix_name_search, 6);
		convert_integer_list_static((const char *) sqlite3_column_text(db->bonus_res_prefix_name_search, 7), ":", bonus->type, MAX_BONUS);
		bonus->order_cnt = sqlite3_column_int(db->bonus_res_prefix_name_search, 8);
		convert_integer_list_static((const char *) sqlite3_column_text(db->bonus_res_prefix_name_search, 9), ":", bonus->order, MAX_BONUS);
	}
	if(sqlite3_reset(db->bonus_res_prefix_name_search) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->resource), sqlite3_errmsg(db->resource));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int status_search_id_name(db_search_t * db, status_res * status, int id, const char * name) {
	int code = 0;
	if(exit_null_safe(3, db, status, name)) return CHECK_FAILED;
	sqlite3_clear_bindings(db->status_res_id_name_search);
	sqlite3_bind_int(db->status_res_id_name_search, 1, id);
	sqlite3_bind_text(db->status_res_id_name_search, 2, name, strlen(name), SQLITE_STATIC);
	code = sqlite3_step(db->status_res_id_name_search);
	if(code == SQLITE_ROW) {
		status->scid = sqlite3_column_int(db->status_res_id_name_search, 0);
		strncopy(status->scstr, MAX_NAME_SIZE, sqlite3_column_text(db->status_res_id_name_search, 1));
		status->type = sqlite3_column_int(db->status_res_id_name_search, 2);
		strncopy(status->scfmt, MAX_FORMAT_SIZE, sqlite3_column_text(db->status_res_id_name_search, 3));
		strncopy(status->scend, MAX_FORMAT_SIZE, sqlite3_column_text(db->status_res_id_name_search, 4));
		status->vcnt = sqlite3_column_int(db->status_res_id_name_search, 5);
		convert_integer_list_static((const char *) sqlite3_column_text(db->status_res_id_name_search, 6), ":", status->vmod, 4);
		convert_integer_list_static((const char *) sqlite3_column_text(db->status_res_id_name_search, 7), ":", status->voff, 4);
	}
	if(sqlite3_reset(db->status_res_id_name_search) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->resource), sqlite3_errmsg(db->resource));
	return (code == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int var_search_id(db_search_t * db, var_res * var, const char * id) {
	int status = 0;
	if(exit_null_safe(3, db, var, id)) return CHECK_FAILED;
	sqlite3_clear_bindings(db->var_res_id_search);
	sqlite3_bind_text(db->var_res_id_search, 1, id, strlen(id), SQLITE_STATIC);
	status = sqlite3_step(db->var_res_id_search);
	if(status == SQLITE_ROW) {
		var->tag = sqlite3_column_int(db->var_res_id_search, 0);
		strncopy(var->id, MAX_NAME_SIZE, sqlite3_column_text(db->var_res_id_search, 1));
		var->type = sqlite3_column_int(db->var_res_id_search, 2);
		var->vflag = sqlite3_column_int(db->var_res_id_search, 3);
		var->fflag = sqlite3_column_int(db->var_res_id_search, 4);
		var->min = sqlite3_column_int(db->var_res_id_search, 5);
		var->max = sqlite3_column_int(db->var_res_id_search, 6);
		strncopy(var->str, MAX_NAME_SIZE, sqlite3_column_text(db->var_res_id_search, 7));
	}
	if(sqlite3_reset(db->var_res_id_search) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->resource), sqlite3_errmsg(db->resource));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int block_search_name(db_search_t * db, block_res * block, const char * name) {
	int status = 0;
	if(exit_null_safe(3, db, block, name)) return CHECK_FAILED;
	sqlite3_clear_bindings(db->block_res_key_search);
	sqlite3_bind_text(db->block_res_key_search, 1, name, strlen(name), SQLITE_STATIC);
	status = sqlite3_step(db->block_res_key_search);
	if(status == SQLITE_ROW) {
		block->id = sqlite3_column_int(db->block_res_key_search, 0);
		strncopy(block->name, MAX_NAME_SIZE, sqlite3_column_text(db->block_res_key_search, 1));
		block->flag = sqlite3_column_int(db->block_res_key_search, 2);
	}
	if(sqlite3_reset(db->block_res_key_search) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->resource), sqlite3_errmsg(db->resource));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int const_db_search_name(db_search_t * db, const_t * search, const char * name) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(4, db, search, name, db->const_db_name_search)) return CHECK_FAILED;

	stmt = db->const_db_name_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 0));
		search->value = sqlite3_column_int(stmt, 1);
		search->type = sqlite3_column_int(stmt, 2);
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int const_db_search_id(db_search_t * db, const_t * search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(3, db, search, db->const_db_id_search)) return CHECK_FAILED;

	stmt = db->const_db_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 0));
		search->value = sqlite3_column_int(stmt, 1);
		search->type = sqlite3_column_int(stmt, 2);
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int skill_db_search_name(db_search_t * db, skill_t * search, const char * name) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(4, db, search, name, db->skill_db_name_search)) return CHECK_FAILED;
	stmt = db->skill_db_name_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		search->id = sqlite3_column_int(stmt, 0);
		search->maxlv = sqlite3_column_int(stmt, 1);
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 2));
		strncopy(search->desc, MAX_NAME_SIZE, sqlite3_column_text(stmt, 3));
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int skill_db_search_id(db_search_t * db, skill_t * search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(3, db, search, db->skill_db_id_search)) return CHECK_FAILED;
	stmt = db->skill_db_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		search->id = sqlite3_column_int(stmt, 0);
		search->maxlv = sqlite3_column_int(stmt, 1);
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 2));
		strncopy(search->desc, MAX_NAME_SIZE, sqlite3_column_text(stmt, 3));
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int item_db_search_name(db_search_t * db, item_t * search, const char * name) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(4, db, search, name, db->item_db_name_search)) return CHECK_FAILED;
	stmt = db->item_db_name_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_STATIC);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		search->id = sqlite3_column_int(stmt, 0);
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    	strncopy(search->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 2));
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int item_db_search_id(db_search_t * db, item_t * search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(3, db, search, db->item_db_id_search)) return CHECK_FAILED;
	stmt = db->item_db_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		search->id = sqlite3_column_int(stmt, 0);
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    	strncopy(search->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 2));
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int mob_db_search_id(db_search_t * db, mob_t * search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(3, db, search, db->mob_db_id_search)) return CHECK_FAILED;
	stmt = db->mob_db_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		search->id = sqlite3_column_int(stmt, 0);
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int merc_db_search_id(db_search_t * db, merc_t * search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(3, db, search, db->merc_db_id_search)) return CHECK_FAILED;
	stmt = db->merc_db_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		search->id = sqlite3_column_int(stmt, 0);
		strncopy(search->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int pet_db_search_id(db_search_t * db, pet_t * search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(3, db, search, db->pet_db_id_search)) return CHECK_FAILED;
	stmt = db->pet_db_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		search->mob_id = sqlite3_column_int(stmt, 0);
		strncopy(search->pet_script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 1));
		strncopy(search->loyal_script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 2));
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int produce_db_search_id(db_search_t * db, produce_t ** search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	produce_t * temp = NULL;
	produce_t * root = NULL;
	produce_t * iter = NULL;
	if(exit_null_safe(3, db, search, db->produce_db_id_search)) return CHECK_FAILED;
	stmt = db->produce_db_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_DONE) {
		if(sqlite3_reset(stmt) != SQLITE_OK) 
			exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
		return CHECK_FAILED;
	} else {
		while(status != SQLITE_DONE) {
			temp = calloc(1, sizeof(produce_t));
			temp->item_id = sqlite3_column_int(stmt, 0);
			temp->item_lv = sqlite3_column_int(stmt, 1);
			temp->skill_id = sqlite3_column_int(stmt, 2);
			temp->skill_lv = sqlite3_column_int(stmt, 3);
			temp->ingredient_count = convert_integer_list_static((const char *) 
			sqlite3_column_text(stmt, 4), ":", temp->item_id_req, MAX_INGREDIENT);
			temp->ingredient_count = convert_integer_list_static((const char *) 
			sqlite3_column_text(stmt, 5), ":", temp->item_amount_req, MAX_INGREDIENT);
			temp->next = NULL;
			(root == NULL) ?
				(root = iter = temp) :
				(iter = iter->next = temp);
			status = sqlite3_step(stmt);
		}
		*search = root;
	}
	
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (*search != NULL) ? CHECK_PASSED : CHECK_FAILED;
}

void produce_free(produce_t * produce) {
	produce_t * temp = NULL;
	while(produce != NULL) {
		temp = produce;
		produce = produce->next;
		free(temp);
	}
}

int item_group_db_search_id(db_search_t * db, item_group_t * search, int id, char * buffer) {
	int status = 0;
	int offset = 0;
	int count = 0;		/* allow up to a certain amount of items */
	item_t item;
	sqlite3_stmt * stmt = NULL;
	if(exit_null_safe(4, db, search, db->item_group_id_search, buffer)) return CHECK_FAILED;
	stmt = db->item_group_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_DONE) {
		if(sqlite3_reset(stmt) != SQLITE_OK) 
			exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
		return CHECK_FAILED;
	} else {
		while(status == SQLITE_ROW) {
			search->group_id = sqlite3_column_int(stmt, 0);
			search->item_id = sqlite3_column_int(stmt, 1);
			search->rate = sqlite3_column_int(stmt, 2);
			/* map the item id to item name */
			item_db_search_id(db, &item, search->item_id);

			if(item.name != NULL && count < 5) {
				offset += sprintf(buffer + offset,"%s, ", item.name);
				count++;
			}
			status = sqlite3_step(stmt);
		}

		/* make a shorten list or report error */
		if(offset > 2) {
			if(count < 5) {
				buffer[offset - 2] = '\0';
			} else {
				offset += sprintf(buffer + offset, "..., %s", item.name);
				buffer[offset] = '\0';
			}
		} else {
			offset += sprintf(buffer + offset, "error");
			buffer[offset] = '\0';
		}

	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (offset > 0) ? CHECK_PASSED : CHECK_FAILED;
}

int item_combo_db_search_id(db_search_t * db, combo_t ** search, int id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	combo_t * temp = NULL;
	combo_t * root = NULL;
	combo_t * iter = NULL;
	/* eathena has not support item combo databases */
	if(db->mode == MODE_EATHENA) return CHECK_FAILED;
	if(exit_null_safe(3, db, search, db->item_combo_id_search)) return CHECK_FAILED;
	stmt = db->item_combo_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_DONE) {
		if(sqlite3_reset(stmt) != SQLITE_OK) 
			exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
		return CHECK_FAILED;
	} else {
		while(status == SQLITE_ROW) {
			temp = calloc(1, sizeof(combo_t));
			strncopy(temp->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 0));
			strncopy(temp->group, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 1));
			(root == NULL)?
				(root = iter = temp) :
				(iter = iter->next = temp);
			status = sqlite3_step(stmt);
		}
		*search = root;
	}
	if(sqlite3_reset(stmt) != SQLITE_OK) 
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(db->athena), sqlite3_errmsg(db->athena));
	return (*search != NULL) ? CHECK_PASSED : CHECK_FAILED;
}

void combo_free(combo_t * combo) {
	combo_t * temp = NULL;
	while(combo != NULL) {
		temp = combo;
		combo = combo->next;
		free(temp);
	}
}

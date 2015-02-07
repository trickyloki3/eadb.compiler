/*
 *   file: api.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "api.h"

struct ic_db_t * init_ic_db(const char * filename) {
	int status = 0;
	sqlite3 * db;
	struct ic_db_t * ic_db = NULL;

	/* initialize api container to athena database */
	ic_db = calloc(1, sizeof(struct ic_db_t));

	/* open connection to sqlite3 database */
	status = sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE, NULL);

	/* debug errors */
	/*sqlite3_trace(db, trace_db, NULL);*/

	/* prepare all the statements */
	sqlite3_prepare_v2(db, ea_item_itr, strlen(ea_item_itr) + 1, &ic_db->ea_item_iterate, NULL);
	sqlite3_prepare_v2(db, ra_item_itr, strlen(ra_item_itr) + 1, &ic_db->ra_item_iterate, NULL);
	sqlite3_prepare_v2(db, he_item_itr, strlen(he_item_itr) + 1, &ic_db->he_item_iterate, NULL);
	sqlite3_prepare_v2(db, block_search, strlen(block_search) + 1, &ic_db->blk_search, NULL);
	sqlite3_prepare_v2(db, ea_const_search_sql, strlen(ea_const_search_sql) + 1, &ic_db->ea_const_search, NULL);
	sqlite3_prepare_v2(db, ra_const_search_sql, strlen(ra_const_search_sql) + 1, &ic_db->ra_const_search, NULL);
	sqlite3_prepare_v2(db, he_const_search_sql, strlen(he_const_search_sql) + 1, &ic_db->he_const_search, NULL);
	sqlite3_prepare_v2(db, var_search_sql, strlen(var_search_sql) + 1, &ic_db->var_search, NULL);
	sqlite3_prepare_v2(db, ea_skill_search_sql, strlen(ea_skill_search_sql) + 1, &ic_db->ea_skill_search, NULL);
	sqlite3_prepare_v2(db, ra_skill_search_sql, strlen(ra_skill_search_sql) + 1, &ic_db->ra_skill_search, NULL);
	sqlite3_prepare_v2(db, he_skill_search_sql, strlen(he_skill_search_sql) + 1, &ic_db->he_skill_search, NULL);
	sqlite3_prepare_v2(db, ea_skill_search_id_sql, strlen(ea_skill_search_id_sql) + 1, &ic_db->ea_skill_id_search, NULL);
	sqlite3_prepare_v2(db, ra_skill_search_id_sql, strlen(ra_skill_search_id_sql) + 1, &ic_db->ra_skill_id_search, NULL);
	sqlite3_prepare_v2(db, he_skill_search_id_sql, strlen(he_skill_search_id_sql) + 1, &ic_db->he_skill_id_search, NULL);
	sqlite3_prepare_v2(db, ea_item_search_sql, strlen(ea_item_search_sql) + 1, &ic_db->ea_item_search, NULL);
	sqlite3_prepare_v2(db, ra_item_search_sql, strlen(ra_item_search_sql) + 1, &ic_db->ra_item_search, NULL);
	sqlite3_prepare_v2(db, he_item_search_sql, strlen(he_item_search_sql) + 1, &ic_db->he_item_search, NULL);
	sqlite3_prepare_v2(db, ea_item_search_id_sql, strlen(ea_item_search_id_sql) + 1, &ic_db->ea_item_id_search, NULL);
	sqlite3_prepare_v2(db, ra_item_search_id_sql, strlen(ra_item_search_id_sql) + 1, &ic_db->ra_item_id_search, NULL);
	sqlite3_prepare_v2(db, he_item_search_id_sql, strlen(he_item_search_id_sql) + 1, &ic_db->he_item_id_search, NULL);
	sqlite3_prepare_v2(db, ea_mob_search_sql, strlen(ea_mob_search_sql) + 1, &ic_db->ea_mob_id_search, NULL);
	sqlite3_prepare_v2(db, ra_mob_search_sql, strlen(ra_mob_search_sql) + 1, &ic_db->ra_mob_id_search, NULL);
	sqlite3_prepare_v2(db, he_mob_search_sql, strlen(he_mob_search_sql) + 1, &ic_db->he_mob_id_search, NULL);
	sqlite3_prepare_v2(db, ea_merc_search_sql, strlen(ea_merc_search_sql) + 1, &ic_db->ea_merc_id_search, NULL);
	sqlite3_prepare_v2(db, ra_merc_search_sql, strlen(ra_merc_search_sql) + 1, &ic_db->ra_merc_id_search, NULL);
	sqlite3_prepare_v2(db, he_merc_search_sql, strlen(he_merc_search_sql) + 1, &ic_db->he_merc_id_search, NULL);
	sqlite3_prepare_v2(db, ea_pet_search_sql, strlen(ea_pet_search_sql) + 1, &ic_db->ea_pet_id_search, NULL);
	sqlite3_prepare_v2(db, ra_pet_search_sql, strlen(ra_pet_search_sql) + 1, &ic_db->ra_pet_id_search, NULL);
	sqlite3_prepare_v2(db, he_pet_search_sql, strlen(he_pet_search_sql) + 1, &ic_db->he_pet_id_search, NULL);
	sqlite3_prepare_v2(db, bonus_search_sql, strlen(bonus_search_sql) + 1, &ic_db->bonus_search, NULL);
	sqlite3_prepare_v2(db, ea_prod_search_sql, strlen(ea_prod_search_sql) + 1, &ic_db->ea_prod_lv_search, NULL);
	sqlite3_prepare_v2(db, ra_prod_search_sql, strlen(ra_prod_search_sql) + 1, &ic_db->ra_prod_lv_search, NULL);
	sqlite3_prepare_v2(db, he_prod_search_sql, strlen(he_prod_search_sql) + 1, &ic_db->he_prod_lv_search, NULL);
	sqlite3_prepare_v2(db, status_search_sql, strlen(status_search_sql) + 1, &ic_db->status_search, NULL);
	sqlite3_prepare_v2(db, ea_item_group_search_sql, strlen(ea_item_group_search_sql) + 1, &ic_db->ea_item_group_search, NULL);
	sqlite3_prepare_v2(db, ra_item_group_search_sql, strlen(ra_item_group_search_sql) + 1, &ic_db->ra_item_group_search, NULL);
	sqlite3_prepare_v2(db, ra_const_search_id_sql, strlen(ra_const_search_id_sql) + 1, &ic_db->ra_const_id_search, NULL);
	sqlite3_prepare_v2(db, ra_item_combo_search_sql, strlen(ra_item_combo_search_sql) + 1, &ic_db->ra_item_combo_search, NULL);
	sqlite3_prepare_v2(db, ra_option_search_sql, strlen(ra_option_search_sql) + 1, &ic_db->ra_option_search, NULL);
	sqlite3_prepare_v2(db, client_map_search_sql, strlen(client_map_search_sql) + 1, &ic_db->client_map_search, NULL);
	sqlite3_prepare_v2(db, client_map_id_search_sql, strlen(client_map_id_search_sql) + 1, &ic_db->client_map_id_search, NULL);	
	/*fprintf(stderr,"%s\n", sqlite3_errmsg(db));*/
	assert(ic_db->ea_item_iterate != NULL);
	assert(ic_db->ra_item_iterate != NULL);
	assert(ic_db->he_item_iterate != NULL);
	assert(ic_db->blk_search != NULL);
	assert(ic_db->ea_const_search != NULL);
	assert(ic_db->ra_const_search != NULL);
	assert(ic_db->he_const_search != NULL);
	assert(ic_db->var_search != NULL);
	assert(ic_db->ea_skill_search != NULL);
	assert(ic_db->ra_skill_search != NULL);
	assert(ic_db->he_skill_search != NULL);
	assert(ic_db->ea_skill_id_search != NULL);
	assert(ic_db->ra_skill_id_search != NULL);
	assert(ic_db->he_skill_id_search != NULL);
	assert(ic_db->ea_item_search != NULL);
	assert(ic_db->ra_item_search != NULL);
	assert(ic_db->he_item_search != NULL);
	assert(ic_db->ea_item_id_search != NULL);
	assert(ic_db->ra_item_id_search != NULL);
	assert(ic_db->he_item_id_search != NULL);
	assert(ic_db->ea_mob_id_search != NULL);
	assert(ic_db->ra_mob_id_search != NULL);
	assert(ic_db->he_mob_id_search != NULL);
	assert(ic_db->ea_merc_id_search != NULL);
	assert(ic_db->ra_merc_id_search != NULL);
	assert(ic_db->he_merc_id_search != NULL);
	assert(ic_db->ea_pet_id_search != NULL);
	assert(ic_db->ra_pet_id_search != NULL);
	assert(ic_db->he_pet_id_search != NULL);
	assert(ic_db->bonus_search != NULL);
	assert(ic_db->ea_prod_lv_search != NULL);
	assert(ic_db->ra_prod_lv_search != NULL);
	assert(ic_db->he_prod_lv_search != NULL);
	assert(ic_db->status_search != NULL);
	assert(ic_db->ea_item_group_search != NULL);
	assert(ic_db->ra_item_group_search != NULL);
	assert(ic_db->ra_const_id_search != NULL);
	assert(ic_db->ra_item_combo_search != NULL);
	assert(ic_db->ra_option_search != NULL);
	assert(ic_db->client_map_search != NULL);
	assert(ic_db->client_map_id_search != NULL);
	/* return api container */
	ic_db->db = db;
	return ic_db;
}

int block_keyword_search(struct ic_db_t * db, block_db_t * info, char * keyword) {
	int ret = 0;
	/* check null paramaters */
	if(exit_null_safe(3, db, info, keyword))
		return CHECK_FAILED;

	/* check identifier (start with alpha) */
	if(!isalpha(keyword[0])) 
		return CHECK_FAILED;

	sqlite3_clear_bindings(db->blk_search);
	sqlite3_bind_text(db->blk_search, 1, keyword, strlen(keyword), SQLITE_STATIC);
	ret = sqlite3_step(db->blk_search);
	if(ret == SQLITE_ROW) {
		info->id = sqlite3_column_int(db->blk_search, 0);
		strncopy(info->name, BUF_SIZE, sqlite3_column_text(db->blk_search, 1));
		info->flag = sqlite3_column_int(db->blk_search, 2);
	} else {
		sqlite3_reset(db->blk_search);
		return CHECK_FAILED;
	}
	sqlite3_reset(db->blk_search);
	return CHECK_PASSED;
}

int var_keyword_search(struct ic_db_t * db, ic_var_t * info, char * keyword) {
	int status = 0;

	/* check null paramaters */
	if(exit_null_safe(3, db, info, keyword))
		return CHECK_FAILED;

	sqlite3_clear_bindings(db->var_search);
	sqlite3_bind_text(db->var_search, 1, keyword, strlen(keyword), SQLITE_STATIC);
	status = sqlite3_step(db->var_search);
	if(status == SQLITE_ROW) {
		info->tag = sqlite3_column_int(db->var_search, 0);
		strncopy(info->id, VAR_ID, sqlite3_column_text(db->var_search, 1));
		info->type = sqlite3_column_int(db->var_search, 2);
		info->vflag = sqlite3_column_int(db->var_search, 3);
		info->fflag = sqlite3_column_int(db->var_search, 4);
		info->min = sqlite3_column_int(db->var_search, 5);
		info->max = sqlite3_column_int(db->var_search, 6);
		strncopy(info->str, VAR_NAME, sqlite3_column_text(db->var_search, 7));
	}
	sqlite3_reset(db->var_search);
	return (status == SQLITE_ROW) ? 0 : -1;
}

int const_keyword_search(struct ic_db_t * db, ic_const_t * info, char * keyword, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA: 
			sqlite3_clear_bindings(db->ea_const_search);
			sqlite3_bind_text(db->ea_const_search, 1, keyword, strlen(keyword), SQLITE_STATIC);
			status = sqlite3_step(db->ea_const_search);
			if(status == SQLITE_ROW) {
				strncopy(info->name, CONST_NAME_SIZE, sqlite3_column_text(db->ea_const_search, 0));
				info->value = sqlite3_column_int(db->ea_const_search, 1);
				info->type = sqlite3_column_int(db->ea_const_search, 2);
			}
			sqlite3_reset(db->ea_const_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_const_search);
			sqlite3_bind_text(db->ra_const_search, 1, keyword, strlen(keyword), SQLITE_STATIC);
			status = sqlite3_step(db->ra_const_search);
			if(status == SQLITE_ROW) {
				strncopy(info->name, CONST_NAME_SIZE, sqlite3_column_text(db->ra_const_search, 0));
				info->value = sqlite3_column_int(db->ra_const_search, 1);
				info->type = sqlite3_column_int(db->ra_const_search, 2);
			}
			sqlite3_reset(db->ra_const_search);
			break;
		case HERCULES: 
			sqlite3_clear_bindings(db->he_const_search);
			sqlite3_bind_text(db->he_const_search, 1, keyword, strlen(keyword), SQLITE_STATIC);
			status = sqlite3_step(db->he_const_search);
			if(status == SQLITE_ROW) {
				strncopy(info->name, CONST_NAME_SIZE, sqlite3_column_text(db->he_const_search, 0));
				info->value = sqlite3_column_int(db->he_const_search, 1);
				info->type = sqlite3_column_int(db->he_const_search, 2);
			}
			sqlite3_reset(db->he_const_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int ra_const_id_search(struct ic_db_t * db, const_t * info, int id) {
	int status = 0;
	sqlite3_bind_int(db->ra_const_id_search, 1, id);
	status = sqlite3_step(db->ra_const_id_search);
	if(status == SQLITE_ROW) {
		if(info->name != NULL) free(info->name);
		info->name = convert_string((const char *) sqlite3_column_text(db->ra_const_id_search, 0));
		info->value = sqlite3_column_int(db->ra_const_id_search, 1);
		info->type = sqlite3_column_int(db->ra_const_id_search, 2);
	}
	sqlite3_reset(db->ra_const_id_search);
	return (status == SQLITE_ROW) ? 0 : -1;	
}

int skill_name_search(struct ic_db_t * db, ic_skill_t * skill, char * name, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_skill_search);
			sqlite3_bind_text(db->ea_skill_search, 1, name, strlen(name), SQLITE_STATIC);
			status = sqlite3_step(db->ea_skill_search);
			if(status == SQLITE_ROW) {
				skill->id = sqlite3_column_int(db->ea_skill_search, 0);
				skill->max = sqlite3_column_int(db->ea_skill_search, 1);
				strncopy(skill->name, SKILL_NAME, sqlite3_column_text(db->ea_skill_search, 2));
				strncopy(skill->desc, SKILL_FORMAT, sqlite3_column_text(db->ea_skill_search, 3));
			}
			sqlite3_reset(db->ea_skill_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_skill_search);
			sqlite3_bind_text(db->ra_skill_search, 1, name, strlen(name), SQLITE_STATIC);
			status = sqlite3_step(db->ra_skill_search);
			if(status == SQLITE_ROW) {
				skill->id = sqlite3_column_int(db->ra_skill_search, 0);
				skill->max = sqlite3_column_int(db->ra_skill_search, 1);
				strncopy(skill->name, SKILL_NAME, sqlite3_column_text(db->ra_skill_search, 2));
				strncopy(skill->desc, SKILL_FORMAT, sqlite3_column_text(db->ra_skill_search, 3));
			}
			sqlite3_reset(db->ra_skill_search);
			break;
		case HERCULES:
			sqlite3_clear_bindings(db->he_skill_search);
			sqlite3_bind_text(db->he_skill_search, 1, name, strlen(name), SQLITE_STATIC);
			status = sqlite3_step(db->he_skill_search);
			if(status == SQLITE_ROW) {
				skill->id = sqlite3_column_int(db->he_skill_search, 0);
				skill->max = sqlite3_column_int(db->he_skill_search, 1);
				strncopy(skill->name, SKILL_NAME, sqlite3_column_text(db->he_skill_search, 2));
				strncopy(skill->desc, SKILL_FORMAT, sqlite3_column_text(db->he_skill_search, 3));
			}
			sqlite3_reset(db->he_skill_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int skill_name_search_id(struct ic_db_t * db, ic_skill_t * skill, int id, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_skill_id_search);
			sqlite3_bind_int(db->ea_skill_id_search, 1, id);
			status = sqlite3_step(db->ea_skill_id_search);
			if(status == SQLITE_ROW) {
				skill->id = sqlite3_column_int(db->ea_skill_id_search, 0);
				skill->max = sqlite3_column_int(db->ea_skill_id_search, 1);
				strncopy(skill->name, SKILL_NAME, sqlite3_column_text(db->ea_skill_id_search, 2));
				strncopy(skill->desc, SKILL_FORMAT, sqlite3_column_text(db->ea_skill_id_search, 3));
			}
			sqlite3_reset(db->ea_skill_id_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_skill_id_search);
			sqlite3_bind_int(db->ra_skill_id_search, 1, id);
			status = sqlite3_step(db->ra_skill_id_search);
			if(status == SQLITE_ROW) {
				skill->id = sqlite3_column_int(db->ra_skill_id_search, 0);
				skill->max = sqlite3_column_int(db->ra_skill_id_search, 1);
				strncopy(skill->name, SKILL_NAME, sqlite3_column_text(db->ra_skill_id_search, 2));
				strncopy(skill->desc, SKILL_FORMAT, sqlite3_column_text(db->ra_skill_id_search, 3));
			}
			sqlite3_reset(db->ra_skill_id_search);
			break;
		case HERCULES:
			sqlite3_clear_bindings(db->he_skill_id_search);
			sqlite3_bind_int(db->he_skill_id_search, 1, id);
			status = sqlite3_step(db->he_skill_id_search);
			if(status == SQLITE_ROW) {
				skill->id = sqlite3_column_int(db->he_skill_id_search, 0);
				skill->max = sqlite3_column_int(db->he_skill_id_search, 1);
				strncopy(skill->name, SKILL_NAME, sqlite3_column_text(db->he_skill_id_search, 2));
				strncopy(skill->desc, SKILL_FORMAT, sqlite3_column_text(db->he_skill_id_search, 3));
			}
			sqlite3_reset(db->he_skill_id_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int item_name_search(struct ic_db_t * db, ic_item_t * item, char * name, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_item_search);
			sqlite3_bind_text(db->ea_item_search, 1, name, strlen(name), SQLITE_STATIC);
			sqlite3_bind_text(db->he_item_search, 2, name, strlen(name), SQLITE_STATIC);
			status = sqlite3_step(db->ea_item_search);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->ea_item_search, 0);
				strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->ea_item_search, 1));
            strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->ea_item_search, 2));
			}
			sqlite3_reset(db->ea_item_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_item_search);
			sqlite3_bind_text(db->ra_item_search, 1, name, strlen(name), SQLITE_STATIC);
			sqlite3_bind_text(db->ra_item_search, 2, name, strlen(name), SQLITE_STATIC);
			status = sqlite3_step(db->ra_item_search);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->ra_item_search, 0);
				strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->ra_item_search, 1));
            strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->ra_item_search, 2));
			}
			sqlite3_reset(db->ra_item_search);
			break;
		case HERCULES:
			sqlite3_clear_bindings(db->he_item_search);
			sqlite3_bind_text(db->he_item_search, 1, name, strlen(name), SQLITE_STATIC);
			sqlite3_bind_text(db->he_item_search, 2, name, strlen(name), SQLITE_STATIC);
			status = sqlite3_step(db->he_item_search);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->he_item_search, 0);
				strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->he_item_search, 1));
            strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->he_item_search, 2));
			}
			sqlite3_reset(db->he_item_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int item_name_id_search(struct ic_db_t * db, ic_item_t * item, int id, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_item_id_search);
			sqlite3_bind_int(db->ea_item_id_search, 1, id);
			status = sqlite3_step(db->ea_item_id_search);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->ea_item_id_search, 0);
				strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->ea_item_id_search, 1));
            strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->ea_item_id_search, 2));
			}
			sqlite3_reset(db->ea_item_id_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_item_id_search);
			sqlite3_bind_int(db->ra_item_id_search, 1, id);
			status = sqlite3_step(db->ra_item_id_search);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->ra_item_id_search, 0);
				strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->ra_item_id_search, 1));
            strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->ra_item_id_search, 2));
			}
			sqlite3_reset(db->ra_item_id_search);
			break;
		case HERCULES:
			sqlite3_clear_bindings(db->he_item_id_search);
			sqlite3_bind_int(db->he_item_id_search, 1, id);
			status = sqlite3_step(db->he_item_id_search);
			if(status == SQLITE_ROW) {
				item->id = sqlite3_column_int(db->he_item_id_search, 0);
				strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->he_item_id_search, 1));
            strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->he_item_id_search, 2));
			}
			sqlite3_reset(db->he_item_id_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int mob_id_search(struct ic_db_t * db, ic_mob_t * mob, int id, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_mob_id_search);
			sqlite3_bind_int(db->ea_mob_id_search, 1, id);
			status = sqlite3_step(db->ea_mob_id_search);
			if(status == SQLITE_ROW) {
				mob->id = sqlite3_column_int(db->ea_mob_id_search, 0);
				strncopy(mob->iro, MOB_NAME_SIZE, sqlite3_column_text(db->ea_mob_id_search, 1));
			}
			sqlite3_reset(db->ea_mob_id_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_mob_id_search);
			sqlite3_bind_int(db->ra_mob_id_search, 1, id);
			status = sqlite3_step(db->ra_mob_id_search);
			if(status == SQLITE_ROW) {
				mob->id = sqlite3_column_int(db->ra_mob_id_search, 0);
				strncopy(mob->iro, MOB_NAME_SIZE, sqlite3_column_text(db->ra_mob_id_search, 1));
			}
			sqlite3_reset(db->ra_mob_id_search);
			break;
		case HERCULES:
			sqlite3_clear_bindings(db->he_mob_id_search);
			sqlite3_bind_int(db->he_mob_id_search, 1, id);
			status = sqlite3_step(db->he_mob_id_search);
			if(status == SQLITE_ROW) {
				mob->id = sqlite3_column_int(db->he_mob_id_search, 0);
				strncopy(mob->iro, MOB_NAME_SIZE, sqlite3_column_text(db->he_mob_id_search, 1));
			}
			sqlite3_reset(db->he_mob_id_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int merc_id_search(struct ic_db_t * db, ic_merc_t * merc, int id, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_merc_id_search);
			sqlite3_bind_int(db->ea_merc_id_search, 1, id);
			status = sqlite3_step(db->ea_merc_id_search);
			if(status == SQLITE_ROW) {
				merc->id = sqlite3_column_int(db->ea_merc_id_search, 0);
				strncopy(merc->name, MERC_NAME_SIZE, sqlite3_column_text(db->ea_merc_id_search, 1));
			}
			sqlite3_reset(db->ea_merc_id_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_merc_id_search);
			sqlite3_bind_int(db->ra_merc_id_search, 1, id);
			status = sqlite3_step(db->ra_merc_id_search);
			if(status == SQLITE_ROW) {
				merc->id = sqlite3_column_int(db->ra_merc_id_search, 0);
				strncopy(merc->name, MERC_NAME_SIZE, sqlite3_column_text(db->ra_merc_id_search, 1));
			}
			sqlite3_reset(db->ra_merc_id_search);
			break;
		case HERCULES:
			sqlite3_clear_bindings(db->he_merc_id_search);
			sqlite3_bind_int(db->he_merc_id_search, 1, id);
			status = sqlite3_step(db->he_merc_id_search);
			if(status == SQLITE_ROW) {
				merc->id = sqlite3_column_int(db->he_merc_id_search, 0);
				strncopy(merc->name, MERC_NAME_SIZE, sqlite3_column_text(db->he_merc_id_search, 1));
			}
			sqlite3_reset(db->he_merc_id_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int pet_id_search(struct ic_db_t * db, ic_pet_t * pet, int id, int mode) {
	int status = 0;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_pet_id_search);
			sqlite3_bind_int(db->ea_pet_id_search, 1, id);
			status = sqlite3_step(db->ea_pet_id_search);
			if(status == SQLITE_ROW) {
				strncopy(pet->pet_script, BUF_SIZE, sqlite3_column_text(db->ea_pet_id_search, 0));
				strncopy(pet->loyal_script, BUF_SIZE, sqlite3_column_text(db->ea_pet_id_search, 1));
			}
			sqlite3_reset(db->ea_pet_id_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_pet_id_search);
			sqlite3_bind_int(db->ra_pet_id_search, 1, id);
			status = sqlite3_step(db->ra_pet_id_search);
			if(status == SQLITE_ROW) {
				strncopy(pet->pet_script, BUF_SIZE, sqlite3_column_text(db->ra_pet_id_search, 0));
				strncopy(pet->loyal_script, BUF_SIZE, sqlite3_column_text(db->ra_pet_id_search, 1));
			}
			sqlite3_reset(db->ra_pet_id_search);
			break;
		case HERCULES:
			sqlite3_clear_bindings(db->he_pet_id_search);
			sqlite3_bind_int(db->he_pet_id_search, 1, id);
			status = sqlite3_step(db->he_pet_id_search);
			if(status == SQLITE_ROW) {
				strncopy(pet->pet_script, BUF_SIZE, sqlite3_column_text(db->he_pet_id_search, 0));
				strncopy(pet->loyal_script, BUF_SIZE, sqlite3_column_text(db->he_pet_id_search, 1));
			}
			sqlite3_reset(db->he_pet_id_search);
			break;
	}
	return (status == SQLITE_ROW) ? 0 : -1;
}

int bonus_name_search(struct ic_db_t * db, ic_bonus_t * bonus, char * prefix, char * attribute) {
	int status = 0;

	sqlite3_clear_bindings(db->bonus_search);
	sqlite3_bind_text(db->bonus_search, 1, prefix, strlen(prefix), SQLITE_STATIC);
	sqlite3_bind_text(db->bonus_search, 2, attribute, strlen(attribute), SQLITE_STATIC);
	status = sqlite3_step(db->bonus_search);
	if(status == SQLITE_ROW) {
		bonus->id = sqlite3_column_int(db->bonus_search, 0);
		bonus->flag = sqlite3_column_int(db->bonus_search, 1);
		bonus->attr = sqlite3_column_int(db->bonus_search, 2);
		strncopy(bonus->prefix, BONUS_PREFIX_SIZE, sqlite3_column_text(db->bonus_search, 3));
		strncopy(bonus->bonus, BONUS_BONUS_SIZE, sqlite3_column_text(db->bonus_search, 4));
		strncopy(bonus->format, BONUS_FORMAT_SIZE, sqlite3_column_text(db->bonus_search, 5));
		bonus->type_cnt = sqlite3_column_int(db->bonus_search, 6);
		convert_integer_list_static((const char *) sqlite3_column_text(db->bonus_search, 7), ":", bonus->type, BONUS_PARAMATER_SIZE);
		bonus->order_cnt = sqlite3_column_int(db->bonus_search, 8);
		convert_integer_list_static((const char *) sqlite3_column_text(db->bonus_search, 9), ":", bonus->order, BONUS_PARAMATER_SIZE);
	}
	sqlite3_reset(db->bonus_search);
	return (status == SQLITE_ROW) ? 0 : -1;
}

int prod_lv_search(struct ic_db_t * db, ic_produce_t ** prod, int lv, int mode) {
	int status = 0;
	array_w array;
	ic_produce_t * root = NULL;
	ic_produce_t * iter = NULL;
	ic_produce_t * temp = NULL;
	switch(mode) {
		case EATHENA:
			sqlite3_clear_bindings(db->ea_prod_lv_search);
			sqlite3_bind_int(db->ea_prod_lv_search, 1, lv);
			status = sqlite3_step(db->ea_prod_lv_search);
			while(status != SQLITE_DONE) {
				temp = calloc(1, sizeof(ic_produce_t));
				temp->item_id = sqlite3_column_int(db->ea_prod_lv_search, 0);
				temp->item_lv = sqlite3_column_int(db->ea_prod_lv_search, 1);
				temp->req_skill = sqlite3_column_int(db->ea_prod_lv_search, 2);
				temp->req_skill_lv = sqlite3_column_int(db->ea_prod_lv_search, 3);
				convert_integer_list((char *)sqlite3_column_text(db->ea_prod_lv_search, 4), ":", &array);
				temp->material = array.array;
				convert_integer_list((char *)sqlite3_column_text(db->ea_prod_lv_search, 5), ":", &array);
				temp->amount = array.array;
				temp->next = NULL;
				if(root == NULL)
					root = iter = temp;
				else
					iter = iter->next = temp;
				status = sqlite3_step(db->ea_prod_lv_search);
			}
			sqlite3_reset(db->ea_prod_lv_search);
			break;
		case RATHENA:
			sqlite3_clear_bindings(db->ra_prod_lv_search);
			sqlite3_bind_int(db->ra_prod_lv_search, 1, lv);
			status = sqlite3_step(db->ra_prod_lv_search);
			while(status != SQLITE_DONE) {
				temp = calloc(1, sizeof(ic_produce_t));
				temp->item_id = sqlite3_column_int(db->ra_prod_lv_search, 1);
				temp->item_lv = sqlite3_column_int(db->ra_prod_lv_search, 2);
				temp->req_skill = sqlite3_column_int(db->ra_prod_lv_search, 3);
				temp->req_skill_lv = sqlite3_column_int(db->ra_prod_lv_search, 4);
				convert_integer_list((char *)sqlite3_column_text(db->ra_prod_lv_search, 5), ":", &array);
				temp->material = array.array;
				temp->req_cnt = array.size;
				convert_integer_list((char *)sqlite3_column_text(db->ra_prod_lv_search, 6), ":", &array);
				temp->amount = array.array;
				temp->next = NULL;
				if(root == NULL)
					root = iter = temp;
				else
					iter = iter->next = temp;
				status = sqlite3_step(db->ra_prod_lv_search);
			}
			sqlite3_reset(db->ra_prod_lv_search);
			break;
		case HERCULES: 
			sqlite3_clear_bindings(db->he_prod_lv_search);
			sqlite3_bind_int(db->he_prod_lv_search, 1, lv);
			status = sqlite3_step(db->he_prod_lv_search);
			while(status != SQLITE_DONE) {
				temp = calloc(1, sizeof(ic_produce_t));
				temp->item_id = sqlite3_column_int(db->he_prod_lv_search, 0);
				temp->item_lv = sqlite3_column_int(db->he_prod_lv_search, 1);
				temp->req_skill = sqlite3_column_int(db->he_prod_lv_search, 2);
				temp->req_skill_lv = sqlite3_column_int(db->he_prod_lv_search, 3);
				convert_integer_list((char *)sqlite3_column_text(db->he_prod_lv_search, 4), ":", &array);
				temp->material = array.array;
				convert_integer_list((char *)sqlite3_column_text(db->he_prod_lv_search, 5), ":", &array);
				temp->amount = array.array;
				temp->next = NULL;
				if(root == NULL)
					root = iter = temp;
				else
					iter = iter->next = temp;
				status = sqlite3_step(db->he_prod_lv_search);
			}
			sqlite3_reset(db->he_prod_lv_search);
			break;
	}
	*prod = root;
	return (root != NULL) ? 0 : -1;
}

int ea_item_group_search(struct ic_db_t * db, int id, int mode, char * buffer) {
	int status = 0;
	int offset = 0;
	int count = 0;		/* allow up to a certain amount of items */
	ic_item_t item;
	ea_item_group_t item_group;
	sqlite3_clear_bindings(db->ea_item_group_search);
	sqlite3_bind_int(db->ea_item_group_search, 1, id);
	memset(&item, 0, sizeof(ic_item_t));
	status = sqlite3_step(db->ea_item_group_search);
	offset += sprintf(buffer + offset,"item from the list of ");
	while(status == SQLITE_ROW) {
		item_group.group_id = sqlite3_column_int(db->ea_item_group_search, 0);
		item_group.item_id = sqlite3_column_int(db->ea_item_group_search, 1);
		item_group.rate = sqlite3_column_int(db->ea_item_group_search, 2);
		/* map the item id to item name */
		item_name_id_search(db, &item, item_group.item_id, mode);

		if(item.name != NULL && count < 5) {
			offset += sprintf(buffer + offset,"%s, ", item.name);
			count++;
		}
		status = sqlite3_step(db->ea_item_group_search);
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

	sqlite3_reset(db->ea_item_group_search);
	return (status == SQLITE_DONE) ? 0 : -1;	
}

int ra_item_group_search(struct ic_db_t * db, int id, int mode, char * buffer) {
	int status = 0;
	int offset = 0;
	int count = 0;		/* allow up to a certain amount of items */
	ic_item_t item;
	ra_item_group_t item_group;
	sqlite3_clear_bindings(db->ra_item_group_search);
	sqlite3_bind_int(db->ra_item_group_search, 1, id);
	memset(&item, 0, sizeof(ic_item_t));
	status = sqlite3_step(db->ra_item_group_search);
	if(status == SQLITE_ROW) offset += sprintf(buffer + offset,"the list of ");
	while(status == SQLITE_ROW) {
		item_group.group_id = sqlite3_column_int(db->ra_item_group_search, 0);
		item_group.item_id = sqlite3_column_int(db->ra_item_group_search, 1);
		item_group.rate = sqlite3_column_int(db->ra_item_group_search, 2);
		/* map the item id to item name */
		item_name_id_search(db, &item, item_group.item_id, mode);

		if(item.name != NULL && count < 5) {
			offset += sprintf(buffer + offset,"%s, ", item.name);
			count++;
		}
		status = sqlite3_step(db->ra_item_group_search);
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

	sqlite3_reset(db->ra_item_group_search);
	return (status == SQLITE_DONE) ? 0 : -1;	
}

void free_prod(ic_produce_t * prod) {
	ic_produce_t * temp = NULL;
	while(prod != NULL) {
		temp = prod;
		prod = prod->next;
		free(temp->material);
		free(temp->amount);
		free(temp);
	}
}

int status_id_search(struct ic_db_t * db, ic_status_t * status, int id, char * name) {
	int code = 0;
	sqlite3_clear_bindings(db->status_search);
	sqlite3_bind_int(db->status_search, 1, id);
	sqlite3_bind_text(db->status_search, 2, name, strlen(name), SQLITE_STATIC);
	code = sqlite3_step(db->status_search);
	if(code == SQLITE_ROW) {
		status->scid = sqlite3_column_int(db->status_search, 0);
		strncopy(status->scstr, STATUS_NAME_SIZE, sqlite3_column_text(db->status_search, 1));
		status->type = sqlite3_column_int(db->status_search, 2);
		strncopy(status->scfmt, STATUS_FORMAT_SIZE, sqlite3_column_text(db->status_search, 3));
		strncopy(status->scend, STATUS_NAME_SIZE, sqlite3_column_text(db->status_search, 4));
		status->vcnt = sqlite3_column_int(db->status_search, 5);
		convert_integer_list_static((const char *) sqlite3_column_text(db->status_search, 6), ":", status->vmod, STATUS_PARAMATER_SIZE);
		convert_integer_list_static((const char *) sqlite3_column_text(db->status_search, 7), ":", status->voff, STATUS_PARAMATER_SIZE);
	}
	sqlite3_reset(db->status_search);
	return (code == SQLITE_ROW) ? 0 : -1;
}

int ra_item_combo_search_id(struct ic_db_t * db, ra_item_combo_t ** combo, int id) {
	int code = 0;
	ra_item_combo_t * temp = NULL;
	ra_item_combo_t * iter = NULL;
	ra_item_combo_t * root = NULL;
	sqlite3_clear_bindings(db->ra_item_combo_search);
	sqlite3_bind_int(db->ra_item_combo_search, 1, id);
	code = sqlite3_step(db->ra_item_combo_search);
	while(code == SQLITE_ROW) {
		temp = calloc(1, sizeof(ra_item_combo_t));
		temp->script = convert_string((const char *) sqlite3_column_text(db->ra_item_combo_search, 0));
		temp->combo = convert_string((const char *) sqlite3_column_text(db->ra_item_combo_search, 1));
		(root == NULL)?
			(*combo = root = iter = temp) :
			(iter = iter->next = temp);
		code = sqlite3_step(db->ra_item_combo_search);
	}
	sqlite3_reset(db->ra_item_combo_search);
	return (combo != NULL) ? 0: -1;
}

void free_combo(ra_item_combo_t * combo) {
	ra_item_combo_t * temp = NULL;
	while(combo != NULL) {
		temp = combo;
		combo = combo->next;
		free(temp->script);
		free(temp->combo);
		free(temp);
	}
}

int ra_option_search_str(struct ic_db_t * db, option_t * option, char * val) {
	int code = 0;
	sqlite3_clear_bindings(db->ra_option_search);
	sqlite3_bind_text(db->ra_option_search, 1, val, strlen(val), SQLITE_STATIC);
	code = sqlite3_step(db->ra_option_search);
	if(code == SQLITE_ROW) {
		strncopy(option->option, OPTION_SIZE, sqlite3_column_text(db->ra_option_search, 0));
		strncopy(option->name, OPTION_NAME_SIZE, sqlite3_column_text(db->ra_option_search, 1));
	}
	sqlite3_reset(db->ra_option_search);
	return (code == SQLITE_ROW) ? 0 : -1;
}

int client_map_search(struct ic_db_t * db, map_t * map, char * val) {
	int code = 0;
	sqlite3_clear_bindings(db->client_map_search);
	sqlite3_bind_text(db->client_map_search, 1, val, strlen(val), SQLITE_STATIC);
	code = sqlite3_step(db->client_map_search);
	if(code == SQLITE_ROW) {
		map->id = sqlite3_column_int(db->client_map_search, 0);
		strncopy(map->map, MAP_SIZE, sqlite3_column_text(db->client_map_search, 1));
		strncopy(map->name, MAP_SIZE, sqlite3_column_text(db->client_map_search, 2));
	}
	sqlite3_reset(db->client_map_search);
	return (code == SQLITE_ROW) ? 0 : -1;
}

int client_map_id_search(struct ic_db_t * db, map_t * map, int id) {
	int code = 0;
	sqlite3_clear_bindings(db->client_map_id_search);
	sqlite3_bind_int(db->client_map_id_search, 1, id);
	code = sqlite3_step(db->client_map_id_search);
	if(code == SQLITE_ROW) {
		map->id = sqlite3_column_int(db->client_map_id_search, 0);
		strncopy(map->map, MAP_SIZE, sqlite3_column_text(db->client_map_id_search, 1));
		strncopy(map->name, MAP_SIZE, sqlite3_column_text(db->client_map_id_search, 2));
	}
	sqlite3_reset(db->client_map_id_search);
	return (code == SQLITE_ROW) ? 0 : -1;
}

void deit_ic_db(struct ic_db_t * db) {
	/* cache compiled sqlite3 statment must be free here */
	sqlite3_finalize(db->ea_item_iterate);
	sqlite3_finalize(db->ra_item_iterate);
	sqlite3_finalize(db->he_item_iterate);
	sqlite3_finalize(db->blk_search);
	sqlite3_finalize(db->var_search);
	sqlite3_finalize(db->bonus_search);
	sqlite3_finalize(db->status_search);
	sqlite3_finalize(db->ea_const_search);
	sqlite3_finalize(db->ra_const_search);
	sqlite3_finalize(db->he_const_search);
	sqlite3_finalize(db->ra_const_id_search);
	sqlite3_finalize(db->ea_skill_search);
	sqlite3_finalize(db->ra_skill_search);
	sqlite3_finalize(db->he_skill_search);
	sqlite3_finalize(db->ea_skill_id_search);
	sqlite3_finalize(db->ra_skill_id_search);
	sqlite3_finalize(db->he_skill_id_search);
	sqlite3_finalize(db->ea_item_search);
	sqlite3_finalize(db->ra_item_search);
	sqlite3_finalize(db->he_item_search);
	sqlite3_finalize(db->ea_item_id_search);
	sqlite3_finalize(db->ra_item_id_search);
	sqlite3_finalize(db->he_item_id_search);
	sqlite3_finalize(db->ea_mob_id_search);
	sqlite3_finalize(db->ra_mob_id_search);
	sqlite3_finalize(db->he_mob_id_search);
	sqlite3_finalize(db->ea_merc_id_search);
	sqlite3_finalize(db->ra_merc_id_search);
	sqlite3_finalize(db->he_merc_id_search);
	sqlite3_finalize(db->ea_pet_id_search);
	sqlite3_finalize(db->ra_pet_id_search);
	sqlite3_finalize(db->he_pet_id_search);
	sqlite3_finalize(db->ea_prod_lv_search);
	sqlite3_finalize(db->ra_prod_lv_search);
	sqlite3_finalize(db->he_prod_lv_search);
	sqlite3_finalize(db->ea_item_group_search);
	sqlite3_finalize(db->ra_item_group_search);
	sqlite3_finalize(db->ra_item_combo_search);
	sqlite3_finalize(db->ra_option_search);
	sqlite3_close(db->db);
	free(db);
}


struct lt_db_t * init_db(const char * filename, int flag) {
	int status = 0;
	sqlite3 * db = NULL;
	
	struct lt_db_t * lt_db = calloc(1, sizeof(struct lt_db_t));	

	/* create the sqlite3 database */
	status = sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	/*sqlite3_trace(db, trace_db, NULL);*/

	/* item tables */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, he_item_des, NULL, NULL, NULL);
		sqlite3_exec(db, he_item_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ea_item_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_item_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_item_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_item_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, he_item_ins, strlen(he_item_ins) + 1, &lt_db->he_item_insert, NULL);
	sqlite3_prepare_v2(db, ea_item_ins, strlen(ea_item_ins) + 1, &lt_db->ea_item_insert, NULL);
	sqlite3_prepare_v2(db, ra_item_ins, strlen(ra_item_ins) + 1, &lt_db->ra_item_insert, NULL);
	assert(lt_db->he_item_insert != NULL);
	assert(lt_db->ea_item_insert != NULL);
	assert(lt_db->ra_item_insert != NULL);

	/* pet tables */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ea_pet_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_pet_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_pet_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_pet_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, he_pet_des, NULL, NULL, NULL);
		sqlite3_exec(db, he_pet_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ea_pet_ins, strlen(ea_pet_ins) + 1, &lt_db->ea_pet_insert, NULL);
	sqlite3_prepare_v2(db, ra_pet_ins, strlen(ra_pet_ins) + 1, &lt_db->ra_pet_insert, NULL);
	sqlite3_prepare_v2(db, he_pet_ins, strlen(he_pet_ins) + 1, &lt_db->he_pet_insert, NULL);
	assert(lt_db->ea_pet_insert != NULL);
	assert(lt_db->ra_pet_insert != NULL);
	assert(lt_db->he_pet_insert != NULL);

	/* merc table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ea_merc_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_merc_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_merc_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_merc_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, he_merc_des, NULL, NULL, NULL);
		sqlite3_exec(db, he_merc_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ea_merc_ins, strlen(ea_merc_ins) + 1, &lt_db->ea_merc_insert, NULL);
	sqlite3_prepare_v2(db, ra_merc_ins, strlen(ra_merc_ins) + 1, &lt_db->ra_merc_insert, NULL);
	sqlite3_prepare_v2(db, he_merc_ins, strlen(he_merc_ins) + 1, &lt_db->he_merc_insert, NULL);
	assert(lt_db->ea_merc_insert != NULL);
	assert(lt_db->ra_merc_insert != NULL);
	assert(lt_db->he_merc_insert != NULL);

	/* produce table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ea_prod_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_prod_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_prod_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_prod_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, he_prod_des, NULL, NULL, NULL);
		sqlite3_exec(db, he_prod_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ea_prod_ins, strlen(ea_prod_ins) + 1, &lt_db->ea_prod_insert, NULL);
	sqlite3_prepare_v2(db, ra_prod_ins, strlen(ra_prod_ins) + 1, &lt_db->ra_prod_insert, NULL);
	sqlite3_prepare_v2(db, he_prod_ins, strlen(he_prod_ins) + 1, &lt_db->he_prod_insert, NULL);
	assert(lt_db->ea_prod_insert != NULL);
	assert(lt_db->ra_prod_insert != NULL);
	assert(lt_db->he_prod_insert != NULL);

	/* skill table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ea_skill_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_skill_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_skill_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_skill_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, he_skill_des, NULL, NULL, NULL);
		sqlite3_exec(db, he_skill_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ea_skill_ins, strlen(ea_skill_ins) + 1, &lt_db->ea_skill_insert, NULL);
	sqlite3_prepare_v2(db, ra_skill_ins, strlen(ra_skill_ins) + 1, &lt_db->ra_skill_insert, NULL);
	sqlite3_prepare_v2(db, he_skill_ins, strlen(he_skill_ins) + 1, &lt_db->he_skill_insert, NULL);
	assert(lt_db->ea_skill_insert != NULL);
	assert(lt_db->ra_skill_insert != NULL);
	assert(lt_db->he_skill_insert != NULL);

	/* mob table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ea_mob_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_mob_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_mob_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_mob_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, he_mob_des, NULL, NULL, NULL);
		sqlite3_exec(db, he_mob_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ea_mob_ins, strlen(ea_mob_ins) + 1, &lt_db->ea_mob_insert, NULL);
	sqlite3_prepare_v2(db, ra_mob_ins, strlen(ra_mob_ins) + 1, &lt_db->ra_mob_insert, NULL);
	sqlite3_prepare_v2(db, he_mob_ins, strlen(he_mob_ins) + 1, &lt_db->he_mob_insert, NULL);
	assert(lt_db->ea_mob_insert != NULL);
	assert(lt_db->ra_mob_insert != NULL);
	assert(lt_db->he_mob_insert != NULL);

	/* block table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, itm_block_des, NULL, NULL, NULL);
		sqlite3_exec(db, itm_block_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, itm_block_ins, strlen(itm_block_ins) + 1, &lt_db->block_insert, NULL);
	assert(lt_db->block_insert != NULL);

	/* var table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, itm_var_des, NULL, NULL, NULL);
		sqlite3_exec(db, itm_var_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, itm_var_ins, strlen(itm_var_ins) + 1, &lt_db->var_insert, NULL);
	assert(lt_db->var_insert != NULL);

	/* status table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, itm_status_des, NULL, NULL, NULL);
		sqlite3_exec(db, itm_status_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, itm_status_ins, strlen(itm_status_ins) + 1, &lt_db->status_insert, NULL);
	assert(lt_db->status_insert != NULL);

	/* bonus table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, itm_bonus_des, NULL, NULL, NULL);
		sqlite3_exec(db, itm_bonus_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, itm_bonus_ins, strlen(itm_bonus_ins) + 1, &lt_db->bonus_insert, NULL);
	assert(lt_db->bonus_insert != NULL);

	/* const table */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ea_const_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_const_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_const_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_const_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, he_const_des, NULL, NULL, NULL);
		sqlite3_exec(db, he_const_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ea_const_ins, strlen(ea_const_ins) + 1, &lt_db->ea_const_insert, NULL);
	sqlite3_prepare_v2(db, ra_const_ins, strlen(ra_const_ins) + 1, &lt_db->ra_const_insert, NULL);
	sqlite3_prepare_v2(db, he_const_ins, strlen(he_const_ins) + 1, &lt_db->he_const_insert, NULL);
	assert(lt_db->ea_const_insert != NULL);	
	assert(lt_db->ra_const_insert != NULL);	
	assert(lt_db->he_const_insert != NULL);	

	/* item group */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ea_item_group_des, NULL, NULL, NULL);
		sqlite3_exec(db, ea_item_group_tbl, NULL, NULL, NULL);
		sqlite3_exec(db, ra_item_group_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_item_group_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ea_item_group_ins, strlen(ea_item_group_ins) + 1, &lt_db->ea_item_group_insert, NULL);
	sqlite3_prepare_v2(db, ra_item_group_ins, strlen(ra_item_group_ins) + 1, &lt_db->ra_item_group_insert, NULL);
	assert(lt_db->ea_item_group_insert != NULL);
	assert(lt_db->ra_item_group_insert != NULL);

	/* item combo */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, ra_itm_combo_des, NULL, NULL, NULL);
		sqlite3_exec(db, ra_itm_combo_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, ra_itm_combo_ins, strlen(ra_itm_combo_ins) + 1, &lt_db->ra_item_combo_insert, NULL);
	assert(lt_db->ra_item_combo_insert != NULL);

	/* option */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, option_des, NULL, NULL, NULL);
		sqlite3_exec(db, option_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, option_ins, strlen(option_ins) + 1, &lt_db->ra_option_insert, NULL);
	assert(lt_db->ra_option_insert != NULL);

	/* map */
	if(flag & INITIALIZE_DB) {
		sqlite3_exec(db, map_des, NULL, NULL, NULL);
		sqlite3_exec(db, map_tbl, NULL, NULL, NULL);
	}
	sqlite3_prepare_v2(db, map_ins, strlen(map_ins) + 1, &lt_db->client_map, NULL);
	assert(lt_db->client_map != NULL);

	lt_db->db = db;
	return lt_db;
}

void deit_db(struct lt_db_t * db) {
	sqlite3_finalize(db->he_item_insert);
	sqlite3_finalize(db->ea_item_insert);
	sqlite3_finalize(db->ra_item_insert);
	sqlite3_finalize(db->ea_pet_insert);
	sqlite3_finalize(db->ra_pet_insert);
	sqlite3_finalize(db->he_pet_insert);
	sqlite3_finalize(db->ea_merc_insert);
	sqlite3_finalize(db->ra_merc_insert);
	sqlite3_finalize(db->he_merc_insert);
	sqlite3_finalize(db->ea_prod_insert);
	sqlite3_finalize(db->ra_prod_insert);
	sqlite3_finalize(db->he_prod_insert);
	sqlite3_finalize(db->ea_skill_insert);
	sqlite3_finalize(db->ra_skill_insert);
	sqlite3_finalize(db->he_skill_insert);
	sqlite3_finalize(db->ea_mob_insert);
	sqlite3_finalize(db->ra_mob_insert);
	sqlite3_finalize(db->he_mob_insert);
	sqlite3_finalize(db->block_insert);
	sqlite3_finalize(db->var_insert);
	sqlite3_finalize(db->status_insert);
	sqlite3_finalize(db->bonus_insert);
	sqlite3_finalize(db->ea_const_insert);
	sqlite3_finalize(db->ra_const_insert);
	sqlite3_finalize(db->he_const_insert);
	sqlite3_finalize(db->ea_item_group_insert);
	sqlite3_finalize(db->ra_item_group_insert);
	sqlite3_finalize(db->ra_item_combo_insert);
	sqlite3_finalize(db->ra_option_insert);
	sqlite3_finalize(db->client_map);
	sqlite3_close(db->db);
	free(db);
}

void trace_db(void * bundle_data, const char * sql_stmt) {
	fprintf(stderr, "trace_db; %s\n", sql_stmt);
}

void load_ea_item(struct lt_db_t * sql, ea_item_t * db, int size) {
	int i = 0;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(sql->ea_item_insert);
		sqlite3_bind_int(sql->ea_item_insert, 1, db[i].id);
		sqlite3_bind_text(sql->ea_item_insert, 2, db[i].aegis, strlen(db[i].aegis), SQLITE_STATIC);
		sqlite3_bind_text(sql->ea_item_insert, 3, db[i].eathena, strlen(db[i].eathena), SQLITE_STATIC);
		sqlite3_bind_int(sql->ea_item_insert, 4, db[i].type);
		sqlite3_bind_int(sql->ea_item_insert, 5, db[i].buy);
		sqlite3_bind_int(sql->ea_item_insert, 6, db[i].sell);
		sqlite3_bind_int(sql->ea_item_insert, 7, db[i].weight);
		sqlite3_bind_int(sql->ea_item_insert, 8, db[i].atk);
		sqlite3_bind_int(sql->ea_item_insert, 9, db[i].def);
		sqlite3_bind_int(sql->ea_item_insert, 10, db[i].range);
		sqlite3_bind_int(sql->ea_item_insert, 11, db[i].slots);
		sqlite3_bind_int(sql->ea_item_insert, 12, db[i].job);
		sqlite3_bind_int(sql->ea_item_insert, 13, db[i].upper);
		sqlite3_bind_int(sql->ea_item_insert, 14, db[i].gender);
		sqlite3_bind_int(sql->ea_item_insert, 15, db[i].loc);
		sqlite3_bind_int(sql->ea_item_insert, 16, db[i].wlv);
		sqlite3_bind_int(sql->ea_item_insert, 17, db[i].elv);
		sqlite3_bind_int(sql->ea_item_insert, 18, db[i].refineable);
		sqlite3_bind_int(sql->ea_item_insert, 19, db[i].view);
		sqlite3_bind_text(sql->ea_item_insert, 20, db[i].script, strlen(db[i].script), SQLITE_STATIC);
		sqlite3_bind_text(sql->ea_item_insert, 21, db[i].onequip, strlen(db[i].onequip), SQLITE_STATIC);
		sqlite3_bind_text(sql->ea_item_insert, 22, db[i].onunequip, strlen(db[i].onunequip), SQLITE_STATIC);
		sqlite3_step(sql->ea_item_insert);
		sqlite3_reset(sql->ea_item_insert);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_ra_item(struct lt_db_t * sql, ra_item_t * db, int size) {
	int i = 0;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(sql->ra_item_insert);
		sqlite3_bind_int(sql->ra_item_insert, 1, db[i].id);
		sqlite3_bind_text(sql->ra_item_insert, 2, db[i].aegis, strlen(db[i].aegis), SQLITE_STATIC);
		sqlite3_bind_text(sql->ra_item_insert, 3, db[i].eathena, strlen(db[i].eathena), SQLITE_STATIC);
		sqlite3_bind_int(sql->ra_item_insert, 4, db[i].type);
		sqlite3_bind_int(sql->ra_item_insert, 5, db[i].buy);
		sqlite3_bind_int(sql->ra_item_insert, 6, db[i].sell);
		sqlite3_bind_int(sql->ra_item_insert, 7, db[i].weight);
		sqlite3_bind_int(sql->ra_item_insert, 8, db[i].matk);
		sqlite3_bind_int(sql->ra_item_insert, 9, db[i].atk);
		sqlite3_bind_int(sql->ra_item_insert, 10, db[i].def);
		sqlite3_bind_int(sql->ra_item_insert, 11, db[i].range);
		sqlite3_bind_int(sql->ra_item_insert, 12, db[i].slots);
		sqlite3_bind_int(sql->ra_item_insert, 13, db[i].job);
		sqlite3_bind_int(sql->ra_item_insert, 14, db[i].upper);
		sqlite3_bind_int(sql->ra_item_insert, 15, db[i].gender);
		sqlite3_bind_int(sql->ra_item_insert, 16, db[i].loc);
		sqlite3_bind_int(sql->ra_item_insert, 17, db[i].wlv);
		sqlite3_bind_int(sql->ra_item_insert, 18, db[i].elv);
		sqlite3_bind_int(sql->ra_item_insert, 19, db[i].refineable);
		sqlite3_bind_int(sql->ra_item_insert, 20, db[i].view);
		sqlite3_bind_text(sql->ra_item_insert, 21, db[i].script, strlen(db[i].script), SQLITE_STATIC);
		sqlite3_bind_text(sql->ra_item_insert, 22, db[i].onequip, strlen(db[i].onequip), SQLITE_STATIC);
		sqlite3_bind_text(sql->ra_item_insert, 23, db[i].onunequip, strlen(db[i].onunequip), SQLITE_STATIC);
		sqlite3_step(sql->ra_item_insert);
		sqlite3_reset(sql->ra_item_insert);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_pet(struct lt_db_t * sql, sqlite3_stmt * ins, pet_t * db, int size) {
	int i = 0;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].mob_id);
		sqlite3_bind_text(ins, 2, db[i].pet_name, strlen(db[i].pet_name), SQLITE_STATIC);
		sqlite3_bind_text(ins, 3, db[i].pet_jname, strlen(db[i].pet_jname), SQLITE_STATIC);
		sqlite3_bind_int(ins, 4, db[i].lure_id);
		sqlite3_bind_int(ins, 5, db[i].egg_id);
		sqlite3_bind_int(ins, 6, db[i].equip_id);
		sqlite3_bind_int(ins, 7, db[i].food_id);
		sqlite3_bind_int(ins, 8, db[i].fullness);
		sqlite3_bind_int(ins, 9, db[i].hungry_delay);
		sqlite3_bind_int(ins, 10, db[i].r_hungry);
		sqlite3_bind_int(ins, 11, db[i].r_full);
		sqlite3_bind_int(ins, 12, db[i].intimate);
		sqlite3_bind_int(ins, 13, db[i].die);
		sqlite3_bind_int(ins, 14, db[i].speed);
		sqlite3_bind_int(ins, 15, db[i].capture);
		sqlite3_bind_int(ins, 16, db[i].s_performance);
		sqlite3_bind_int(ins, 17, db[i].talk_convert);
		sqlite3_bind_int(ins, 18, db[i].attack_rate);
		sqlite3_bind_int(ins, 19, db[i].defence_attack_rate);
		sqlite3_bind_int(ins, 20, db[i].change_target_rate);
		sqlite3_bind_text(ins, 21, db[i].pet_script, strlen(db[i].pet_script), SQLITE_STATIC);
		sqlite3_bind_text(ins, 22, db[i].loyal_script, strlen(db[i].loyal_script), SQLITE_STATIC);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_merc(struct lt_db_t * sql, sqlite3_stmt * ins, merc_t * db, int size) {
	int i = 0;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].id);
		sqlite3_bind_text(ins, 2, db[i].sprite, strlen(db[i].sprite), SQLITE_STATIC);
		sqlite3_bind_text(ins, 3, db[i].name, strlen(db[i].name), SQLITE_STATIC);
		sqlite3_bind_int(ins, 4, db[i].lv);
		sqlite3_bind_int(ins, 5, db[i].hp);
		sqlite3_bind_int(ins, 6, db[i].sp);
		sqlite3_bind_int(ins, 7, db[i].range1);
		sqlite3_bind_int(ins, 8, db[i].atk1);
		sqlite3_bind_int(ins, 9, db[i].atk2);
		sqlite3_bind_int(ins, 10, db[i].def);
		sqlite3_bind_int(ins, 11, db[i].mdef);
		sqlite3_bind_int(ins, 12, db[i].str);
		sqlite3_bind_int(ins, 13, db[i].agi);
		sqlite3_bind_int(ins, 14, db[i].vit);
		sqlite3_bind_int(ins, 15, db[i].intr);
		sqlite3_bind_int(ins, 16, db[i].dex);
		sqlite3_bind_int(ins, 17, db[i].luk);
		sqlite3_bind_int(ins, 18, db[i].range2);
		sqlite3_bind_int(ins, 19, db[i].range3);
		sqlite3_bind_int(ins, 20, db[i].scale);
		sqlite3_bind_int(ins, 21, db[i].race);
		sqlite3_bind_int(ins, 22, db[i].element);
		sqlite3_bind_int(ins, 23, db[i].speed);
		sqlite3_bind_int(ins, 24, db[i].adelay);
		sqlite3_bind_int(ins, 25, db[i].amotion);
		sqlite3_bind_int(ins, 26, db[i].dmotion);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_prod(struct lt_db_t * sql, sqlite3_stmt * ins, produce_t * db, int size) {
	int i = 0;
	char buf[4096];
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].item_id);
		sqlite3_bind_int(ins, 2, db[i].item_lv);
		sqlite3_bind_int(ins, 3, db[i].req_skill);
		sqlite3_bind_int(ins, 4, db[i].req_skill_lv);
		array_to_string(buf, db[i].material);
		sqlite3_bind_text(ins, 5, buf, strlen(buf), SQLITE_TRANSIENT);
		array_to_string_cnt(buf, db[i].amount, array_field_cnt(buf) + 1);
		sqlite3_bind_text(ins, 6, buf, strlen(buf), SQLITE_TRANSIENT);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void ra_load_prod(struct lt_db_t * sql, sqlite3_stmt * ins, ra_produce_t * db, int size) {
	int i = 0;
	char buf[4096];
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].id);
		sqlite3_bind_int(ins, 2, db[i].item_id);
		sqlite3_bind_int(ins, 3, db[i].item_lv);
		sqlite3_bind_int(ins, 4, db[i].req_skill);
		sqlite3_bind_int(ins, 5, db[i].req_skill_lv);
		array_to_string(buf, db[i].material);
		sqlite3_bind_text(ins, 6, buf, strlen(buf), SQLITE_TRANSIENT);
		array_to_string_cnt(buf, db[i].amount, array_field_cnt(buf) + 1);
		sqlite3_bind_text(ins, 7, buf, strlen(buf), SQLITE_TRANSIENT);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void ea_load_mob(struct lt_db_t * sql, sqlite3_stmt * ins, ea_mob_t * db, int size) {
	int i = 0;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].id);
		sqlite3_bind_text(ins, 2, db[i].sprite, strlen(db[i].sprite), SQLITE_STATIC);
		sqlite3_bind_text(ins, 3, db[i].kro, strlen(db[i].kro), SQLITE_STATIC);
		sqlite3_bind_text(ins, 4, db[i].iro, strlen(db[i].iro), SQLITE_STATIC);
		sqlite3_bind_int(ins, 5, db[i].lv);
		sqlite3_bind_int(ins, 6, db[i].hp);
		sqlite3_bind_int(ins, 7, db[i].sp);
		sqlite3_bind_int(ins, 8, db[i].exp);
		sqlite3_bind_int(ins, 9, db[i].jexp);
		sqlite3_bind_int(ins, 10, db[i].range);
		sqlite3_bind_int(ins, 11, db[i].atk1);
		sqlite3_bind_int(ins, 12, db[i].atk2);
		sqlite3_bind_int(ins, 13, db[i].def);
		sqlite3_bind_int(ins, 14, db[i].mdef);
		sqlite3_bind_int(ins, 15, db[i].str);
		sqlite3_bind_int(ins, 16, db[i].agi);
		sqlite3_bind_int(ins, 17, db[i].vit);
		sqlite3_bind_int(ins, 18, db[i].intr);
		sqlite3_bind_int(ins, 19, db[i].dex);
		sqlite3_bind_int(ins, 20, db[i].luk);
		sqlite3_bind_int(ins, 21, db[i].range2);
		sqlite3_bind_int(ins, 22, db[i].range3);
		sqlite3_bind_int(ins, 23, db[i].scale);
		sqlite3_bind_int(ins, 24, db[i].race);
		sqlite3_bind_int(ins, 25, db[i].element);
		sqlite3_bind_int(ins, 26, db[i].mode);
		sqlite3_bind_int(ins, 27, db[i].speed);
		sqlite3_bind_int(ins, 28, db[i].adelay);
		sqlite3_bind_int(ins, 29, db[i].amotion);
		sqlite3_bind_int(ins, 30, db[i].dmotion);
		sqlite3_bind_int(ins, 31, db[i].mexp);
		sqlite3_bind_int(ins, 32, db[i].expper);
		sqlite3_bind_int(ins, 33, db[i].mvp1id);
		sqlite3_bind_int(ins, 34, db[i].mvp1per);
		sqlite3_bind_int(ins, 35, db[i].mvp2id);
		sqlite3_bind_int(ins, 36, db[i].mvp2per);
		sqlite3_bind_int(ins, 37, db[i].mvp3id);
		sqlite3_bind_int(ins, 38, db[i].mvp3per);
		sqlite3_bind_int(ins, 39, db[i].drop1id);
		sqlite3_bind_int(ins, 40, db[i].drop1per);
		sqlite3_bind_int(ins, 41, db[i].drop2id);
		sqlite3_bind_int(ins, 42, db[i].drop2per);
		sqlite3_bind_int(ins, 43, db[i].drop3id);
		sqlite3_bind_int(ins, 44, db[i].drop3per);
		sqlite3_bind_int(ins, 45, db[i].drop4id);
		sqlite3_bind_int(ins, 46, db[i].drop4per);
		sqlite3_bind_int(ins, 47, db[i].drop5id);
		sqlite3_bind_int(ins, 48, db[i].drop5per);
		sqlite3_bind_int(ins, 49, db[i].drop6id);
		sqlite3_bind_int(ins, 50, db[i].drop6per);
		sqlite3_bind_int(ins, 51, db[i].drop7id);
		sqlite3_bind_int(ins, 52, db[i].drop7per);
		sqlite3_bind_int(ins, 53, db[i].drop8id);
		sqlite3_bind_int(ins, 54, db[i].drop8per);
		sqlite3_bind_int(ins, 55, db[i].drop9id);
		sqlite3_bind_int(ins, 56, db[i].drop9per);
		sqlite3_bind_int(ins, 57, db[i].dropcardid);
		sqlite3_bind_int(ins, 58, db[i].dropcardper);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_mob(struct lt_db_t * sql, sqlite3_stmt * ins, mob_t * db, int size) {
	int i = 0;
	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].id);
		sqlite3_bind_text(ins, 2, db[i].sprite, strlen(db[i].sprite), SQLITE_STATIC);
		sqlite3_bind_text(ins, 3, db[i].kro, strlen(db[i].kro), SQLITE_STATIC);
		sqlite3_bind_text(ins, 4, db[i].iro, strlen(db[i].iro), SQLITE_STATIC);
		sqlite3_bind_int(ins, 5, db[i].lv);
		sqlite3_bind_int(ins, 6, db[i].hp);
		sqlite3_bind_int(ins, 7, db[i].sp);
		sqlite3_bind_int(ins, 8, db[i].exp);
		sqlite3_bind_int(ins, 9, db[i].jexp);
		sqlite3_bind_int(ins, 10, db[i].range);
		sqlite3_bind_int(ins, 11, db[i].atk1);
		sqlite3_bind_int(ins, 12, db[i].atk2);
		sqlite3_bind_int(ins, 13, db[i].def);
		sqlite3_bind_int(ins, 14, db[i].mdef);
		sqlite3_bind_int(ins, 15, db[i].str);
		sqlite3_bind_int(ins, 16, db[i].agi);
		sqlite3_bind_int(ins, 17, db[i].vit);
		sqlite3_bind_int(ins, 18, db[i].intr);
		sqlite3_bind_int(ins, 19, db[i].dex);
		sqlite3_bind_int(ins, 20, db[i].luk);
		sqlite3_bind_int(ins, 21, db[i].range2);
		sqlite3_bind_int(ins, 22, db[i].range3);
		sqlite3_bind_int(ins, 23, db[i].scale);
		sqlite3_bind_int(ins, 24, db[i].race);
		sqlite3_bind_int(ins, 25, db[i].element);
		sqlite3_bind_int(ins, 26, db[i].mode);
		sqlite3_bind_int(ins, 27, db[i].speed);
		sqlite3_bind_int(ins, 28, db[i].adelay);
		sqlite3_bind_int(ins, 29, db[i].amotion);
		sqlite3_bind_int(ins, 30, db[i].dmotion);
		sqlite3_bind_int(ins, 31, db[i].mexp);
		sqlite3_bind_int(ins, 32, db[i].mvp1id);
		sqlite3_bind_int(ins, 33, db[i].mvp1per);
		sqlite3_bind_int(ins, 34, db[i].mvp2id);
		sqlite3_bind_int(ins, 35, db[i].mvp2per);
		sqlite3_bind_int(ins, 36, db[i].mvp3id);
		sqlite3_bind_int(ins, 37, db[i].mvp3per);
		sqlite3_bind_int(ins, 38, db[i].drop1id);
		sqlite3_bind_int(ins, 39, db[i].drop1per);
		sqlite3_bind_int(ins, 40, db[i].drop2id);
		sqlite3_bind_int(ins, 41, db[i].drop2per);
		sqlite3_bind_int(ins, 42, db[i].drop3id);
		sqlite3_bind_int(ins, 43, db[i].drop3per);
		sqlite3_bind_int(ins, 44, db[i].drop4id);
		sqlite3_bind_int(ins, 45, db[i].drop4per);
		sqlite3_bind_int(ins, 46, db[i].drop5id);
		sqlite3_bind_int(ins, 47, db[i].drop5per);
		sqlite3_bind_int(ins, 48, db[i].drop6id);
		sqlite3_bind_int(ins, 49, db[i].drop6per);
		sqlite3_bind_int(ins, 50, db[i].drop7id);
		sqlite3_bind_int(ins, 51, db[i].drop7per);
		sqlite3_bind_int(ins, 52, db[i].drop8id);
		sqlite3_bind_int(ins, 53, db[i].drop8per);
		sqlite3_bind_int(ins, 54, db[i].drop9id);
		sqlite3_bind_int(ins, 55, db[i].drop9per);
		sqlite3_bind_int(ins, 56, db[i].dropcardid);
		sqlite3_bind_int(ins, 57, db[i].dropcardper);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_skill(struct lt_db_t * sql, sqlite3_stmt * ins, skill_t * db, int size) {
	int i = 0;
	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].id);
		sqlite3_bind_text(ins, 2, db[i].range, strlen(db[i].range), SQLITE_STATIC);
		sqlite3_bind_int(ins, 3, db[i].hit);
		sqlite3_bind_int(ins, 4, db[i].inf);
		sqlite3_bind_text(ins, 5, db[i].element, strlen(db[i].element), SQLITE_STATIC);
		sqlite3_bind_int(ins, 6, db[i].nk);
		sqlite3_bind_text(ins, 7, db[i].splash, strlen(db[i].splash), SQLITE_STATIC);
		sqlite3_bind_int(ins, 8, db[i].max);
		sqlite3_bind_text(ins, 9, db[i].hit_amount, strlen(db[i].hit_amount), SQLITE_STATIC);
		sqlite3_bind_text(ins, 10, db[i].cast_cancel, strlen(db[i].cast_cancel), SQLITE_STATIC);
		sqlite3_bind_int(ins, 11, db[i].cast_def_reduce_rate);
		sqlite3_bind_int(ins, 12, db[i].inf2);
		sqlite3_bind_text(ins, 13, db[i].maxcount, strlen(db[i].maxcount), SQLITE_STATIC);
		sqlite3_bind_text(ins, 14, db[i].type, strlen(db[i].type), SQLITE_STATIC);
		sqlite3_bind_text(ins, 15, db[i].blow_count, strlen(db[i].blow_count), SQLITE_STATIC);
		sqlite3_bind_text(ins, 16, db[i].name, strlen(db[i].name), SQLITE_STATIC);
		sqlite3_bind_text(ins, 17, db[i].desc, strlen(db[i].desc), SQLITE_STATIC);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void ra_load_skill(struct lt_db_t * sql, sqlite3_stmt * ins, ra_skill_t * db, int size) {
	int i = 0;
	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].id);
		sqlite3_bind_text(ins, 2, db[i].range, strlen(db[i].range), SQLITE_STATIC);
		sqlite3_bind_int(ins, 3, db[i].hit);
		sqlite3_bind_int(ins, 4, db[i].inf);
		sqlite3_bind_text(ins, 5, db[i].element, strlen(db[i].element), SQLITE_STATIC);
		sqlite3_bind_int(ins, 6, db[i].nk);
		sqlite3_bind_text(ins, 7, db[i].splash, strlen(db[i].splash), SQLITE_STATIC);
		sqlite3_bind_int(ins, 8, db[i].max);
		sqlite3_bind_text(ins, 9, db[i].hit_amount, strlen(db[i].hit_amount), SQLITE_STATIC);
		sqlite3_bind_text(ins, 10, db[i].cast_cancel, strlen(db[i].cast_cancel), SQLITE_STATIC);
		sqlite3_bind_int(ins, 11, db[i].cast_def_reduce_rate);
		sqlite3_bind_int(ins, 12, db[i].inf2);
		sqlite3_bind_text(ins, 13, db[i].maxcount, strlen(db[i].maxcount), SQLITE_STATIC);
		sqlite3_bind_text(ins, 14, db[i].type, strlen(db[i].type), SQLITE_STATIC);
		sqlite3_bind_text(ins, 15, db[i].blow_count, strlen(db[i].blow_count), SQLITE_STATIC);
		sqlite3_bind_int(ins, 16, db[i].inf3);
		sqlite3_bind_text(ins, 17, db[i].name, strlen(db[i].name), SQLITE_STATIC);
		sqlite3_bind_text(ins, 18, db[i].desc, strlen(db[i].desc), SQLITE_STATIC);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_block(struct lt_db_t * sql, sqlite3_stmt * ins, block_t * db, int size) {
	int i = 0;
	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].bk_id);
		sqlite3_bind_text(ins, 2, db[i].bk_kywd, strlen(db[i].bk_kywd), SQLITE_STATIC);
		sqlite3_bind_int(ins, 3, db[i].bk_flag);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_var(struct lt_db_t * sql, sqlite3_stmt * ins, var_t * db, int size) {
	int i = 0;
	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].tag);
		sqlite3_bind_text(ins, 2, db[i].id, strlen(db[i].id), SQLITE_STATIC);
		sqlite3_bind_int(ins, 3, db[i].type);
		sqlite3_bind_int(ins, 4, db[i].vflag);
		sqlite3_bind_int(ins, 5, db[i].fflag);
		sqlite3_bind_int(ins, 6, db[i].min);
		sqlite3_bind_int(ins, 7, db[i].max);
		sqlite3_bind_text(ins, 8, db[i].str, strlen(db[i].str), SQLITE_STATIC);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_status(struct lt_db_t * sql, sqlite3_stmt * ins, status_t * db, int size) {
	int i = 0;
	
	char buf[4096];
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].scid);
		sqlite3_bind_text(ins, 2, db[i].scstr, strlen(db[i].scstr), SQLITE_STATIC);
		sqlite3_bind_int(ins, 3, db[i].type);
		if(db[i].scfmt != NULL)
			sqlite3_bind_text(ins, 4, db[i].scfmt, strlen(db[i].scfmt), SQLITE_STATIC);
		if(db[i].scend != NULL)
			sqlite3_bind_text(ins, 5, db[i].scend, strlen(db[i].scend), SQLITE_STATIC);
		sqlite3_bind_int(ins, 6, db[i].vcnt);
		array_to_string_cnt(buf, db[i].vmod, 4);
		sqlite3_bind_text(ins, 7, buf, strlen(buf), SQLITE_TRANSIENT);
		array_to_string_cnt(buf, db[i].voff, 4);
		sqlite3_bind_text(ins, 8, buf, strlen(buf), SQLITE_TRANSIENT);
		sqlite3_step(ins);

		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_bonus(struct lt_db_t * sql, sqlite3_stmt * ins, bonus_t * db, int size) {
	int i = 0;
	
	char buf[4096];
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].id);
		sqlite3_bind_int(ins, 2, db[i].flag);
		sqlite3_bind_int(ins, 3, db[i].attr);
		sqlite3_bind_text(ins, 4, db[i].pref, strlen(db[i].pref), SQLITE_STATIC);
		sqlite3_bind_text(ins, 5, db[i].buff, strlen(db[i].buff), SQLITE_STATIC);		
		sqlite3_bind_text(ins, 6, db[i].desc, strlen(db[i].desc), SQLITE_STATIC);
		sqlite3_bind_int(ins, 7, db[i].type_cnt);
		array_to_string_cnt(buf, db[i].type, db[i].type_cnt);
		sqlite3_bind_text(ins, 8, buf, strlen(buf), SQLITE_TRANSIENT);
		sqlite3_bind_int(ins, 9, db[i].order_cnt);
		array_to_string_cnt(buf, db[i].order, db[i].order_cnt);
		sqlite3_bind_text(ins, 10, buf, strlen(buf), SQLITE_TRANSIENT);	
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_const(struct lt_db_t * sql, sqlite3_stmt * ins, const_t * db, int size) {
	int i = 0;
	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_text(ins, 1, db[i].name, strlen(db[i].name), SQLITE_STATIC);
		sqlite3_bind_int(ins, 2, db[i].value);
		sqlite3_bind_int(ins, 3, db[i].type);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_ea_item_group(struct lt_db_t * sql, sqlite3_stmt * ins, ea_item_group_t * db, int size) {
	int i = 0;
	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		sqlite3_bind_int(ins, 1, db[i].group_id);
		sqlite3_bind_int(ins, 2, db[i].item_id);
		sqlite3_bind_int(ins, 3, db[i].rate);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_ra_item_group(struct lt_db_t * sql, sqlite3_stmt * ins, ra_item_group_t * db, int size) {
	int i = 0;	
	struct ic_db_t * const_db = init_ic_db("athena.db");
	ic_const_t const_info;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		if(const_keyword_search(const_db, &const_info, db[i].group_name, RATHENA)) {
			fprintf(stderr,"[warn]: failed to resolve constant %s\n", db[i].group_name);
			break;
		}
		sqlite3_bind_int(ins, 1, const_info.value);
		sqlite3_bind_int(ins, 2, db[i].item_id);
		sqlite3_bind_int(ins, 3, db[i].rate);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	deit_ic_db(const_db);
}

void load_ra_item_package(struct lt_db_t * sql, sqlite3_stmt * ins, ra_item_package_t * db, int size) {
	int i = 0;
	struct ic_db_t * athena = init_ic_db("athena.db");
	ic_const_t const_info;
	ic_item_t item_info;
	memset(&const_info, 0, sizeof(const_t));
	memset(&item_info, 0, sizeof(ic_item_t));
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(ins);
		if(const_keyword_search(athena, &const_info, db[i].group_name, RATHENA)) {
			fprintf(stderr,"[warn]: failed to resolve constant %s\n", db[i].group_name);
			break;
		}
		if(item_name_search(athena, &item_info, db[i].item_name, RATHENA))
			if(item_name_id_search(athena, &item_info, convert_integer(db[i].item_name, 10), RATHENA)) {
				fprintf(stderr,"[warn]: failed to resolve item %s\n", db[i].item_name);
				break;
			}
		sqlite3_bind_int(ins, 1, const_info.value);
		sqlite3_bind_int(ins, 2, item_info.id);
		sqlite3_bind_int(ins, 3, db[i].rate);
		sqlite3_step(ins);
		sqlite3_reset(ins);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	deit_ic_db(athena);
}

void load_ra_item_combo(struct lt_db_t * sql, ra_item_combo_t * db, int size) {
	int i = 0;
	int j = 0;
	int offset = 0;
	char buffer[BUF_SIZE];
	struct ic_db_t * athena = init_ic_db("athena.db");
	ic_item_t item_info;
	array_w item_id_list;
	int * item_id_array = NULL;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		offset = 0;
		/* convert the item group string into an array of integers */
		memset(&item_id_list, 0, sizeof(array_w));
		convert_integer_list(db[i].item_id_list, ":", &item_id_list);
		item_id_array = item_id_list.array;

		/* add the item id and script in the array of integers */
		if(item_id_array != NULL) {
			/* build a buffer of all item names */
			memset(&item_info, 0, sizeof(ic_item_t));
			for(j = 0; j < item_id_list.size; j++) {
				if(!item_name_id_search(athena, &item_info, item_id_array[j], RATHENA)) {
					offset += (offset == 0)?
						sprintf(buffer + offset, "[%s", item_info.name):
						sprintf(buffer + offset, ", %s", item_info.name);
					buffer[offset] = '\0';
				}
			}
			offset += sprintf(buffer + offset, " Combo]");

			for(j = 0; j < item_id_list.size; j++) {
				sqlite3_clear_bindings(sql->ra_item_combo_insert);
				sqlite3_bind_int(sql->ra_item_combo_insert, 1, item_id_array[j]);
				sqlite3_bind_text(sql->ra_item_combo_insert, 2, db[i].script, strlen(db[i].script), SQLITE_STATIC);
				sqlite3_bind_text(sql->ra_item_combo_insert, 3, buffer, offset, SQLITE_STATIC);
				sqlite3_step(sql->ra_item_combo_insert);
				sqlite3_reset(sql->ra_item_combo_insert);
			}
			/* free the list of integers */
			free(item_id_list.array);
		}
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	deit_ic_db(athena);
}

void load_option(struct lt_db_t * sql, option_t * db, int size) {
	int i = 0;	
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(sql->ra_option_insert);
		sqlite3_bind_text(sql->ra_option_insert, 1, db[i].option, strlen(db[i].option), SQLITE_STATIC);
		sqlite3_bind_text(sql->ra_option_insert, 2, db[i].name, strlen(db[i].name), SQLITE_STATIC);
		sqlite3_step(sql->ra_option_insert);
		sqlite3_reset(sql->ra_option_insert);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

void load_client_map(struct lt_db_t * sql, map_t * db, int size) {
	int i = 0;
	sqlite3_exec(sql->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0; i < size; i++) {
		sqlite3_clear_bindings(sql->client_map);
		sqlite3_bind_int(sql->client_map, 1, db[i].id);
		sqlite3_bind_text(sql->client_map, 2, db[i].map, strlen(db[i].map), SQLITE_STATIC);
		sqlite3_bind_text(sql->client_map, 3, db[i].name, strlen(db[i].name), SQLITE_STATIC);
		sqlite3_step(sql->client_map);
		sqlite3_reset(sql->client_map);
	}
	sqlite3_exec(sql->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

char * array_to_string(char * buffer, int * array) {
	int offset = 0;
	for(int i = 0; array[i] > 0; i++)
		offset += sprintf(buffer + offset, "%d%s", array[i], (array[i + 1] > 0) ? ":" : "");
	buffer[offset] = '\0';
	return buffer;
}

char * array_to_string_cnt(char * buffer, int * array, int size) {
	int offset = 0;
	for(int i = 0; i < size; i++)
		offset += sprintf(buffer + offset, "%d%s", array[i], (i + 1 < size) ? ":" : "");
	buffer[offset] = '\0';
	return buffer;
}

int array_field_cnt(char * buf) {
	int i = 0;
	int cnt = 0;
	int len = strlen(buf);
	for(i = 0; i < len; i++)
		if(buf[i] == ':') cnt++;
	return cnt;
}

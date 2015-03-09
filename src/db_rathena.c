/*
 *   file: db_rathena.c
 *   date: 03/01/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
 #include "db_rathena.h"

native_config_t load_ra_native[RATHENA_DB_COUNT] = {
	{ item_ra_load, sentinel_newline, delimit_cvs, CHECK_BRACKET | SKIP_NEXT_WS | CHECK_FIELD_COUNT, ITEM_RA_FIELD_COUNT, sizeof(item_ra) },
	{ mob_ra_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, MOB_RA_FIELD_COUNT, sizeof(mob_ra) },
	{ skill_ra_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, SKILL_RA_FIELD_COUNT, sizeof(skill_ra) },
	{ produce_ra_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS, 0, sizeof(produce_ra) },
	{ mercenary_ra_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, MERCENARY_RA_FIELD_COUNT, sizeof(mercenary_ra) },
	{ pet_ra_load, sentinel_newline, delimit_cvs, CHECK_BRACKET | SKIP_NEXT_WS | CHECK_FIELD_COUNT, PET_RA_FIELD_COUNT, sizeof(pet_ra) },
	{ item_group_ra_load, sentinel_whitespace, delimit_cvs_whitespace, SKIP_NEXT_WS | CHECK_FIELD_COUNT, ITEM_GROUP_RA_FIELD_COUNT, sizeof(item_group_ra) },
	{ const_ra_load, sentinel_newline, delimit_cvs_whitespace, SKIP_NEXT_WS, 0, sizeof(const_ra) },
	{ package_ra_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS, 0, sizeof(package_ra) },
	{ combo_ra_load, sentinel_newline, delimit_cvs, CHECK_BRACKET | SKIP_NEXT_WS | CHECK_FIELD_COUNT, ITEM_COMBO_FIELD_COUNT, sizeof(combo_ra) }
};

int item_ra_load(void * db, int row, int col, char * val) {
	item_ra * record = &((item_ra *) db)[row];
	switch(col) {
      case 0:   record->id = convert_integer(val, 10); 									break;
      case 1:   strnload(record->aegis, MAX_NAME_SIZE, val); 							break;
      case 2:   strnload(record->eathena, MAX_NAME_SIZE, val); 							break;
      case 3:   record->type = convert_integer(val, 10); 								break;
      case 4:   record->buy = convert_integer(val, 10); 								break;
      case 5:   record->sell = convert_integer(val, 10); 								break;
      case 6:   record->weight = convert_integer(val, 10); 								break;
      case 7:   convert_delimit_integer(val, ':', 2, &record->atk, &record->matk); 		break;
      case 8:   record->def = convert_integer(val, 10); 								break;
      case 9:   record->range = convert_integer(val, 10); 								break;
      case 10:  record->slots = convert_integer(val, 10); 								break;
      case 11:  record->job = convert_uinteger(val, 16); 								break;
      case 12:  record->upper = convert_integer(val, 10); 								break;
      case 13:  record->gender = convert_integer(val, 10); 								break;
      case 14:  record->loc = convert_integer(val, 10); 								break;
      case 15:  record->wlv = convert_integer(val, 10); 								break;
      case 16:  record->elv = convert_integer(val, 10); 								break;
      case 17:  record->refineable = convert_integer(val, 10); 							break;
      case 18:  record->view = convert_integer(val, 10); 								break;
      case 19:  strnload(record->script, MAX_SCRIPT_SIZE, val); 						break;
      case 20:  strnload(record->onequip, MAX_SCRIPT_SIZE, val); 						break;
      case 21:  strnload(record->onunequip, MAX_SCRIPT_SIZE, val); 						break;
      default:  exit_func_safe("invalid column field %d in rathena item database.\n", col);
   }
   return 0;
}

int mob_ra_load(void * db, int row, int col, char * val) {
	mob_ra * record = &((mob_ra *) db)[row];
	switch(col) {
		case 0: record->id = convert_integer(val,10);            	break;
		case 1: strnload(record->sprite, MAX_NAME_SIZE, val);		break;
		case 2: strnload(record->kro, MAX_NAME_SIZE, val);			break;
		case 3: strnload(record->iro, MAX_NAME_SIZE, val);			break;
		case 4: record->lv = convert_integer(val,10);            	break;
		case 5: record->hp = convert_integer(val,10);            	break;
		case 6: record->sp = convert_integer(val,10);            	break;
		case 7: record->exp = convert_integer(val,10);           	break;
		case 8: record->jexp = convert_integer(val,10);          	break;
		case 9: record->range = convert_integer(val,10);         	break;
		case 10: record->atk1 = convert_integer(val,10);         	break;
		case 11: record->atk2 = convert_integer(val,10);         	break;
		case 12: record->def = convert_integer(val,10);          	break;
		case 13: record->mdef = convert_integer(val,10);         	break;
		case 14: record->str = convert_integer(val,10);          	break;
		case 15: record->agi = convert_integer(val,10);          	break;
		case 16: record->vit = convert_integer(val,10);          	break;
		case 17: record->intr = convert_integer(val,10);         	break;
		case 18: record->dex = convert_integer(val,10);          	break;
		case 19: record->luk = convert_integer(val,10);          	break;
		case 20: record->range2 = convert_integer(val,10);       	break;
		case 21: record->range3 = convert_integer(val,10);       	break;
		case 22: record->scale = convert_integer(val,10);        	break;
		case 23: record->race = convert_integer(val,10);         	break;
		case 24: record->element = convert_integer(val,10);      	break;
		case 25: record->mode = convert_uinteger(val,16);        	break;
		case 26: record->speed = convert_integer(val,10);        	break;
		case 27: record->adelay = convert_integer(val,10);       	break;
		case 28: record->amotion = convert_integer(val,10);      	break;
		case 29: record->dmotion = convert_integer(val,10);      	break;
		case 30: record->mexp = convert_integer(val,10);         	break;
		case 31: record->mvp1id = convert_integer(val,10);       	break;
		case 32: record->mvp1per = convert_integer(val,10);      	break;
		case 33: record->mvp2id = convert_integer(val,10);       	break;
		case 34: record->mvp2per = convert_integer(val,10);      	break;
		case 35: record->mvp3id = convert_integer(val,10);       	break;
		case 36: record->mvp3per = convert_integer(val,10);      	break;
		case 37: record->drop1id = convert_integer(val,10);      	break;
		case 38: record->drop1per = convert_integer(val,10);     	break;
		case 39: record->drop2id = convert_integer(val,10);      	break;
		case 40: record->drop2per = convert_integer(val,10);     	break;
		case 41: record->drop3id = convert_integer(val,10);      	break;
		case 42: record->drop3per = convert_integer(val,10);     	break;
		case 43: record->drop4id = convert_integer(val,10);      	break;
		case 44: record->drop4per = convert_integer(val,10);     	break;
		case 45: record->drop5id = convert_integer(val,10);      	break;
		case 46: record->drop5per = convert_integer(val,10);     	break;
		case 47: record->drop6id = convert_integer(val,10);      	break;
		case 48: record->drop6per = convert_integer(val,10);     	break;
		case 49: record->drop7id = convert_integer(val,10);      	break;
		case 50: record->drop7per = convert_integer(val,10);     	break;
		case 51: record->drop8id = convert_integer(val,10);      	break;
		case 52: record->drop8per = convert_integer(val,10);     	break;
		case 53: record->drop9id = convert_integer(val,10);      	break;
		case 54: record->drop9per = convert_integer(val,10);     	break;
		case 55: record->dropcardid = convert_integer(val,10);   	break;
		case 56: record->dropcardper = convert_integer(val,10);  	break;
		default: exit_func_safe("invalid column field %d in rathena mob database.\n", col);
	}
	return 0;
}

int skill_ra_load(void * db, int row, int col, char * val) {
	skill_ra * record = &((skill_ra *) db)[row];
	switch(col) {
		case 0: record->id = convert_integer(val,10); 						break;
		case 1: strnload(record->range.str, MAX_VARARG_SIZE, val); 			break;
		case 2: record->hit = convert_integer(val,10); 						break;
		case 3: record->inf = convert_integer(val,10); 						break;
		case 4: strnload(record->element.str, MAX_VARARG_SIZE, val); 		break;
		case 5: record->nk = convert_uinteger(val,16); 						break;
		case 6: strnload(record->splash.str, MAX_VARARG_SIZE, val); 		break;
		case 7: record->maxlv = convert_integer(val,10); 					break;
		case 8: strnload(record->hit_amount.str, MAX_VARARG_SIZE, val); 	break;
		case 9: strnload(record->cast_cancel, MAX_NAME_SIZE, val); 			break;
		case 10: record->cast_def_reduce_rate = convert_integer(val,10); 	break;
		case 11: record->inf2 = convert_uinteger(val,16); 					break;
		case 12: strnload(record->maxcount.str, MAX_VARARG_SIZE, val); 		break;
		case 13: strnload(record->type, MAX_NAME_SIZE, val); 				break;
		case 14: strnload(record->blow_count.str, MAX_VARARG_SIZE, val); 	break;
		case 15: record->inf3 = convert_integer(val,16); 					break;
		case 16: strnload(record->name, MAX_NAME_SIZE, val);				break;
		case 17: strnload(record->desc, MAX_NAME_SIZE, val); 				break;
		default: exit_func_safe("invalid column field %d in rathena skill database.\n", col);
	}
	return 0;
}

int produce_ra_load(void * db, int row, int col, char * val) {
	/* non-reentrant produce loading function */
	static int material_cnt = 0;
	static int alternate = 0;
	produce_ra * record = &((produce_ra *) db)[row];
	switch(col) {
		case 0:
			material_cnt = 0;
			alternate = 0;
			record->id = convert_integer(val,10);
			break;
		case 1: record->item_id = convert_integer(val,10); break;
		case 2: record->item_lv = convert_integer(val,10); break;
		case 3: record->skill_id = convert_integer(val,10); break;
		case 4: record->skill_lv = convert_integer(val,10); break;
		default: 
			(!alternate) ?
				(record->item_id_req[material_cnt] = convert_integer(val,10)):
				(record->item_amount_req[material_cnt++] = convert_integer(val,10));
			alternate = !alternate;
			record->ingredient_count = material_cnt;
		break;
	}
	return 0;
}

int mercenary_ra_load(void * db, int row, int col, char * val) {
	mercenary_ra * record = &((mercenary_ra *) db)[row];
	switch(col) {
		case 0: record->id = convert_integer(val, 10); 				break;
		case 1: strnload(record->sprite, MAX_NAME_SIZE, val); 		break;
		case 2: strnload(record->name, MAX_NAME_SIZE, val); 		break;
		case 3: record->lv = convert_integer(val, 10); 				break;
		case 4: record->hp = convert_integer(val, 10); 				break;
		case 5: record->sp = convert_integer(val, 10); 				break;
		case 6: record->range1 = convert_integer(val, 10); 			break;
		case 7: record->atk1 = convert_integer(val, 10); 			break;
		case 8: record->atk2 = convert_integer(val, 10); 			break;
		case 9: record->def = convert_integer(val, 10); 			break;
		case 10: record->mdef = convert_integer(val, 10); 			break;
		case 11: record->str = convert_integer(val, 10); 			break;
		case 12: record->agi = convert_integer(val, 10); 			break;
		case 13: record->vit = convert_integer(val, 10); 			break;
		case 14: record->intr = convert_integer(val, 10); 			break;
		case 15: record->dex = convert_integer(val, 10); 			break;
		case 16: record->luk = convert_integer(val, 10); 			break;
		case 17: record->range2 = convert_integer(val, 10); 		break;
		case 18: record->range3 = convert_integer(val, 10); 		break;
		case 19: record->scale = convert_integer(val, 10); 			break;
		case 20: record->race = convert_integer(val, 10); 			break;
		case 21: record->element = convert_integer(val, 10); 		break;
		case 22: record->speed = convert_integer(val, 10); 			break;
		case 23: record->adelay = convert_integer(val, 10); 		break;
		case 24: record->amotion = convert_integer(val, 10); 		break;
		case 25: record->dmotion = convert_integer(val, 10); 		break;
		default: exit_func_safe("invalid column field %d in rathena mercenary database.\n", col);
	}
	return 0;
}

int pet_ra_load(void * db, int row, int col, char * val) {
	pet_ra * record = &((pet_ra *) db)[row];
	switch(col) {
		case 0: record->mob_id = convert_integer(val,10); 				break;
		case 1: strnload(record->name, MAX_NAME_SIZE, val); 			break;
		case 2: strnload(record->jname, MAX_NAME_SIZE, val); 			break;
		case 3: record->lure_id = convert_integer(val,10); 				break;
		case 4: record->egg_id = convert_integer(val,10); 				break;
		case 5: record->equip_id = convert_integer(val,10); 			break;
		case 6: record->food_id = convert_integer(val,10); 				break;
		case 7: record->fullness = convert_integer(val,10); 			break;
		case 8: record->hungry_delay = convert_integer(val,10); 		break;
		case 9: record->r_hungry = convert_integer(val,10); 			break;
		case 10: record->r_full = convert_integer(val,10); 				break;
		case 11: record->intimate = convert_integer(val,10); 			break;
		case 12: record->die = convert_integer(val,10); 				break;
		case 13: record->capture = convert_integer(val,10); 			break;
		case 14: record->speed = convert_integer(val,10); 				break;
		case 15: record->s_performance = convert_integer(val,10); 		break;
		case 16: record->talk_convert = convert_integer(val,10); 		break;
		case 17: record->attack_rate = convert_integer(val,10); 		break;
		case 18: record->defence_attack_rate = convert_integer(val,10); break;
		case 19: record->change_target_rate = convert_integer(val,10); 	break;
		case 20: strnload(record->pet_script, MAX_SCRIPT_SIZE, val);	break;
		case 21: strnload(record->loyal_script, MAX_SCRIPT_SIZE, val);	break;
		default: exit_func_safe("invalid column field %d in rathena pet database.\n", col);
	}
	return 0;
}

int item_group_ra_load(void * db, int row, int col, char * val) {
	item_group_ra * record = &((item_group_ra *) db)[row];
	switch(col) {
		case 0: strnload(record->name, MAX_NAME_SIZE, val); break;
		case 1: record->item_id = convert_integer(val, 10); break;
		case 2: record->rate = convert_integer(val, 10); 	break;
		default: exit_func_safe("invalid column field %d in rathena item group database.\n", col);
	}
	return 0;
}

int const_ra_load(void * db, int row, int col, char * val) {
	const_ra * record = &((const_ra *) db)[row];
	switch(col) {
		case 0: strnload(record->name, MAX_NAME_SIZE, val); 							break;
		case 1:  
			/* constant can be represented as hexadecimal or decimal */
			record->value = (strlen(val) > 2 && val[0] == '0' && val[1] == 'x') ?
				convert_integer(val, 16):
				convert_integer(val, 10);												break;
		case 2: record->type = convert_integer(val, 10); 								break;
		default: exit_func_safe("invalid column field %d in rathena const database.\n", col);
	}
	return 0;
}

int package_ra_load(void * db, int row, int col, char * val) {
	package_ra * record = &((package_ra *) db)[row];
	switch(col) {
		case 0: strnload(record->group_name, MAX_NAME_SIZE, val);		break;
		case 1: strnload(record->item_name, MAX_NAME_SIZE, val); 		break;
		case 2: record->rate = convert_integer(val, 10); 				break;
		case 3: record->amount = convert_integer(val, 10); 				break;
		case 4: record->random = convert_integer(val, 10); 				break;
		case 5: record->announced = convert_integer(val, 10); 			break;
		case 6: record->duration = convert_integer(val, 10); 			break;
		case 7: record->guid = convert_integer(val, 10); 				break;
		case 8: record->bound = convert_integer(val, 10); 				break;
		case 9: record->named = convert_integer(val, 10); 				break;
		default: exit_func_safe("invalid column field %d in rathena package database.\n", col);
	}
	return 0;
}

int combo_ra_load(void * db, int row, int col, char * val) {
	combo_ra * record = &((combo_ra *) db)[row];
	switch(col) {
		case 0: strnload(record->item_id.str, MAX_VARARG_SIZE, val); 	break;
		case 1: strnload(record->script, MAX_SCRIPT_SIZE, val); 		break;
		default: break;
	}
	return 0;
}

int load_rathena_database(const char * rathena_path) {
	int status = 0;

	native_t item_db;
	status = load_native("/root/Desktop/git/rathena/db/re/item_db.txt",
	trim_numeric, load_native_general, &item_db, &load_ra_native[0]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena item database.\n");
		exit(EXIT_FAILURE);
	}
	free(item_db.db);

	native_t mob_db;
	status = load_native("/root/Desktop/git/rathena/db/re/mob_db.txt",
	trim_numeric, load_native_general, &mob_db, &load_ra_native[1]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena mob database.\n");
		exit(EXIT_FAILURE);
	}
	free(mob_db.db);

	native_t skill_db;
	status = load_native("/root/Desktop/git/rathena/db/re/skill_db.txt",
	trim_numeric, load_native_general, &skill_db, &load_ra_native[2]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena skill database.\n");
		exit(EXIT_FAILURE);
	}
	free(skill_db.db);

	native_t produce_db;
	status = load_native("/root/Desktop/git/rathena/db/produce_db.txt",
	trim_numeric, load_native_general, &produce_db, &load_ra_native[3]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena produce database.\n");
		exit(EXIT_FAILURE);
	}
	free(produce_db.db);

	native_t mercenary_db;
	status = load_native("/root/Desktop/git/rathena/db/mercenary_db.txt",
	trim_numeric, load_native_general, &mercenary_db, &load_ra_native[4]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena mercenary database.\n");
		exit(EXIT_FAILURE);
	}
	free(mercenary_db.db);

	native_t pet_db;
	status = load_native("/root/Desktop/git/rathena/db/re/pet_db.txt",
	trim_numeric, load_native_general, &pet_db, &load_ra_native[5]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena pet database.\n");
		exit(EXIT_FAILURE);
	}
	free(pet_db.db);

	native_t item_group_db;
	status = load_native("/root/Desktop/git/rathena/db/re/item_violetbox.txt",
	trim_alpha, load_native_general, &item_group_db, &load_ra_native[6]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena item group database.\n");
		exit(EXIT_FAILURE);
	}
	free(item_group_db.db);

	native_t const_db;
	status = load_native("/root/Desktop/git/rathena/db/const.txt",
	trim_general, load_native_general, &const_db, &load_ra_native[7]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena item group database.\n");
		exit(EXIT_FAILURE);
	}
	free(const_db.db);

	native_t package_db;
	status = load_native("/root/Desktop/git/rathena/db/re/item_package.txt",
	trim_alpha, load_native_general, &package_db, &load_ra_native[8]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena item package database.\n");
		exit(EXIT_FAILURE);
	}
	free(package_db.db);

	native_t combo_db;
	status = load_native("/root/Desktop/git/rathena/db/re/item_combo_db.txt",
	trim_numeric, load_native_general, &combo_db, &load_ra_native[9]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load rathena item combo database.\n");
		exit(EXIT_FAILURE);
	}
	free(combo_db.db);
	
	return 0;
}

int create_rathena_database(db_ra_t * db, const char * path) {
	int status = 0;
	const char * error = NULL;
	char * sql_error = NULL;	
	if( /* open database connection specified by path */
		sqlite3_open(path, &db->db) != SQLITE_OK ||
		/* execute rathena table creation script */
		sqlite3_exec(db->db, rathena_database_sql, NULL, NULL, &sql_error) != SQLITE_OK ||
		/* prepare the insertion statements */
		sqlite3_prepare_v2(db->db, item_ra_insert, 	 	 strlen(item_ra_insert), 		&db->item_ra_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, mob_ra_insert, 		 strlen(mob_ra_insert), 		&db->mob_ra_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, skill_ra_insert, 	 strlen(skill_ra_insert), 		&db->skill_ra_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, produce_ra_insert, 	 strlen(produce_ra_insert), 	&db->produce_ra_sql_insert, 	NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, mercenary_ra_insert,  strlen(mercenary_ra_insert), 	&db->mercenary_ra_sql_insert, 	NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, pet_ra_insert, 		 strlen(pet_ra_insert), 		&db->pet_ra_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, item_group_ra_insert, strlen(item_group_ra_insert), 	&db->item_group_ra_sql_insert, 	NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, const_ra_insert, 	 strlen(const_ra_insert), 		&db->const_ra_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, item_combo_ra_insert, strlen(item_combo_ra_insert), 	&db->item_combo_ra_sql_insert, 	NULL) != SQLITE_OK) {
		/* print sqlite3 error before exiting */
		status = sqlite3_errcode(db->db);
		error = sqlite3_errmsg(db->db);
		exit_func_safe("sqlite3 code %d; %s", status, (sql_error != NULL) ? sql_error : error);
		/* release the error string and database handle */
		if(sql_error != NULL) sqlite3_free(sql_error);
		if(db->db != NULL) sqlite3_close(db->db);
		return CHECK_FAILED;
	}
	return CHECK_PASSED;
}

int finalize_rathena_database(db_ra_t * db) {
	sqlite3_finalize(db->item_ra_sql_insert);
	sqlite3_finalize(db->mob_ra_sql_insert);
	sqlite3_finalize(db->skill_ra_sql_insert);
	sqlite3_finalize(db->produce_ra_sql_insert);
	sqlite3_finalize(db->mercenary_ra_sql_insert);
	sqlite3_finalize(db->pet_ra_sql_insert);
	sqlite3_finalize(db->item_group_ra_sql_insert);
	sqlite3_finalize(db->const_ra_sql_insert);
	sqlite3_finalize(db->item_combo_ra_sql_insert);
	sqlite3_close(db->db);
	return CHECK_PASSED;
}

int item_ra_sql_load(db_ra_t * db, const char * path) {
	int i = 0;
	native_t item_db;
	item_ra * item_ra_db = NULL;
	memset(&item_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &item_db, &load_ra_native[0]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena item database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(item_db.db == NULL || item_db.size <= 0) {
		exit_func_safe("failed to load rathena item database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, item_ra_db = item_db.db; i < item_db.size; i++) {
		sqlite3_clear_bindings(db->item_ra_sql_insert);
		sqlite3_bind_int(db->item_ra_sql_insert, 	1, 	item_ra_db[i].id);
		sqlite3_bind_text(db->item_ra_sql_insert, 	2, 	item_ra_db[i].aegis, 		strlen(item_ra_db[i].aegis), SQLITE_STATIC);
		sqlite3_bind_text(db->item_ra_sql_insert, 	3, 	item_ra_db[i].eathena, 		strlen(item_ra_db[i].eathena), SQLITE_STATIC);
		sqlite3_bind_int(db->item_ra_sql_insert, 	4, 	item_ra_db[i].type);
		sqlite3_bind_int(db->item_ra_sql_insert, 	5, 	item_ra_db[i].buy);
		sqlite3_bind_int(db->item_ra_sql_insert, 	6, 	item_ra_db[i].sell);
		sqlite3_bind_int(db->item_ra_sql_insert, 	7, 	item_ra_db[i].weight);
		sqlite3_bind_int(db->item_ra_sql_insert, 	8, 	item_ra_db[i].matk);
		sqlite3_bind_int(db->item_ra_sql_insert, 	9, 	item_ra_db[i].atk);
		sqlite3_bind_int(db->item_ra_sql_insert, 	10, item_ra_db[i].def);
		sqlite3_bind_int(db->item_ra_sql_insert, 	11, item_ra_db[i].range);
		sqlite3_bind_int(db->item_ra_sql_insert, 	12, item_ra_db[i].slots);
		sqlite3_bind_int(db->item_ra_sql_insert, 	13, item_ra_db[i].job);
		sqlite3_bind_int(db->item_ra_sql_insert, 	14, item_ra_db[i].upper);
		sqlite3_bind_int(db->item_ra_sql_insert, 	15, item_ra_db[i].gender);
		sqlite3_bind_int(db->item_ra_sql_insert, 	16, item_ra_db[i].loc);
		sqlite3_bind_int(db->item_ra_sql_insert, 	17, item_ra_db[i].wlv);
		sqlite3_bind_int(db->item_ra_sql_insert, 	18, item_ra_db[i].elv);
		sqlite3_bind_int(db->item_ra_sql_insert, 	19, item_ra_db[i].refineable);
		sqlite3_bind_int(db->item_ra_sql_insert, 	20, item_ra_db[i].view);
		sqlite3_bind_text(db->item_ra_sql_insert, 	21, item_ra_db[i].script, 		strlen(item_ra_db[i].script), SQLITE_STATIC);
		sqlite3_bind_text(db->item_ra_sql_insert, 	22, item_ra_db[i].onequip, 		strlen(item_ra_db[i].onequip), SQLITE_STATIC);
		sqlite3_bind_text(db->item_ra_sql_insert, 	23, item_ra_db[i].onunequip, 	strlen(item_ra_db[i].onunequip), SQLITE_STATIC);
		sqlite3_step(db->item_ra_sql_insert);
		sqlite3_reset(db->item_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(item_db.db);
	return CHECK_PASSED;
}

int mob_ra_sql_load(db_ra_t * db, const char * path) {
	int i = 0;
	native_t mob_db;
	mob_ra * mob_ra_db = NULL;
	memset(&mob_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &mob_db, &load_ra_native[1]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena mob database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(mob_db.db == NULL || mob_db.size <= 0) {
		exit_func_safe("failed to load rathena mob database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, mob_ra_db = mob_db.db; i < mob_db.size; i++) {
		sqlite3_clear_bindings(db->mob_ra_sql_insert);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	1, 	mob_ra_db[i].id);
		sqlite3_bind_text(db->mob_ra_sql_insert, 	2, 	mob_ra_db[i].sprite, 	strlen(mob_ra_db[i].sprite), 	SQLITE_STATIC);
		sqlite3_bind_text(db->mob_ra_sql_insert, 	3, 	mob_ra_db[i].kro, 		strlen(mob_ra_db[i].kro), 		SQLITE_STATIC);
		sqlite3_bind_text(db->mob_ra_sql_insert, 	4, 	mob_ra_db[i].iro, 		strlen(mob_ra_db[i].iro), 		SQLITE_STATIC);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	5, 	mob_ra_db[i].lv);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	6, 	mob_ra_db[i].hp);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	7, 	mob_ra_db[i].sp);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	8, 	mob_ra_db[i].exp);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	9, 	mob_ra_db[i].jexp);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	10, mob_ra_db[i].range);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	11, mob_ra_db[i].atk1);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	12, mob_ra_db[i].atk2);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	13, mob_ra_db[i].def);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	14, mob_ra_db[i].mdef);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	15, mob_ra_db[i].str);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	16, mob_ra_db[i].agi);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	17, mob_ra_db[i].vit);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	18, mob_ra_db[i].intr);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	19, mob_ra_db[i].dex);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	20, mob_ra_db[i].luk);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	21, mob_ra_db[i].range2);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	22, mob_ra_db[i].range3);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	23, mob_ra_db[i].scale);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	24, mob_ra_db[i].race);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	25, mob_ra_db[i].element);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	26, mob_ra_db[i].mode);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	27, mob_ra_db[i].speed);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	28, mob_ra_db[i].adelay);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	29, mob_ra_db[i].amotion);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	30, mob_ra_db[i].dmotion);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	31, mob_ra_db[i].mexp);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	32, mob_ra_db[i].mvp1id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	33, mob_ra_db[i].mvp1per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	34, mob_ra_db[i].mvp2id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	35, mob_ra_db[i].mvp2per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	36, mob_ra_db[i].mvp3id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	37, mob_ra_db[i].mvp3per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	38, mob_ra_db[i].drop1id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	39, mob_ra_db[i].drop1per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	40, mob_ra_db[i].drop2id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	41, mob_ra_db[i].drop2per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	42, mob_ra_db[i].drop3id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	43, mob_ra_db[i].drop3per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	44, mob_ra_db[i].drop4id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	45, mob_ra_db[i].drop4per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	46, mob_ra_db[i].drop5id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	47, mob_ra_db[i].drop5per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	48, mob_ra_db[i].drop6id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	49, mob_ra_db[i].drop6per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	50, mob_ra_db[i].drop7id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	51, mob_ra_db[i].drop7per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	52, mob_ra_db[i].drop8id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	53, mob_ra_db[i].drop8per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	54, mob_ra_db[i].drop9id);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	55, mob_ra_db[i].drop9per);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	56, mob_ra_db[i].dropcardid);
		sqlite3_bind_int(db->mob_ra_sql_insert, 	57, mob_ra_db[i].dropcardper);
		sqlite3_step(db->mob_ra_sql_insert);
		sqlite3_reset(db->mob_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(mob_db.db);
	return CHECK_PASSED;
}

int skill_ra_sql_load(db_ra_t * db, const char * path) {
	int i = 0;
	native_t skill_db;
	skill_ra * skill_ra_db = NULL;
	memset(&skill_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &skill_db, &load_ra_native[2]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena skill database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(skill_db.db == NULL || skill_db.size <= 0) {
		exit_func_safe("failed to load rathena skill database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, skill_ra_db = skill_db.db; i < skill_db.size; i++) {
		sqlite3_clear_bindings(db->skill_ra_sql_insert);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	1, skill_ra_db[i].id);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	2, skill_ra_db[i].range.str, 		strlen(skill_ra_db[i].range.str), 		SQLITE_STATIC);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	3, skill_ra_db[i].hit);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	4, skill_ra_db[i].inf);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	5, skill_ra_db[i].element.str, 		strlen(skill_ra_db[i].element.str), 	SQLITE_STATIC);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	6, skill_ra_db[i].nk);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	7, skill_ra_db[i].splash.str, 		strlen(skill_ra_db[i].splash.str), 		SQLITE_STATIC);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	8, skill_ra_db[i].maxlv);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	9, skill_ra_db[i].hit_amount.str, 	strlen(skill_ra_db[i].hit_amount.str), 	SQLITE_STATIC);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	10, skill_ra_db[i].cast_cancel, 	strlen(skill_ra_db[i].cast_cancel), 	SQLITE_STATIC);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	11, skill_ra_db[i].cast_def_reduce_rate);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	12, skill_ra_db[i].inf2);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	13, skill_ra_db[i].maxcount.str, 	strlen(skill_ra_db[i].maxcount.str), 	SQLITE_STATIC);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	14, skill_ra_db[i].type, 			strlen(skill_ra_db[i].type), 			SQLITE_STATIC);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	15, skill_ra_db[i].blow_count.str, 	strlen(skill_ra_db[i].blow_count.str), 	SQLITE_STATIC);
		sqlite3_bind_int(db->skill_ra_sql_insert, 	16, skill_ra_db[i].inf3);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	17, skill_ra_db[i].name, 			strlen(skill_ra_db[i].name), 			SQLITE_STATIC);
		sqlite3_bind_text(db->skill_ra_sql_insert, 	18, skill_ra_db[i].desc, 			strlen(skill_ra_db[i].desc), 			SQLITE_STATIC);
		sqlite3_step(db->skill_ra_sql_insert);
		sqlite3_reset(db->skill_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(skill_db.db);
	return CHECK_PASSED;
}

int produce_ra_sql_load(db_ra_t * db, const char * path) {
	int i = 0;
	char buf[BUF_SIZE];
	native_t produce_db;
	produce_ra * produce_ra_db = NULL;
	memset(&produce_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &produce_db, &load_ra_native[3]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena produce database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(produce_db.db == NULL || produce_db.size <= 0) {
		exit_func_safe("failed to load rathena produce database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, produce_ra_db = produce_db.db; i < produce_db.size; i++) {
		sqlite3_clear_bindings(db->produce_ra_sql_insert);
		sqlite3_bind_int(db->produce_ra_sql_insert, 	1, produce_ra_db[i].id);
		sqlite3_bind_int(db->produce_ra_sql_insert, 	2, produce_ra_db[i].item_id);
		sqlite3_bind_int(db->produce_ra_sql_insert, 	3, produce_ra_db[i].item_lv);
		sqlite3_bind_int(db->produce_ra_sql_insert, 	4, produce_ra_db[i].skill_id);
		sqlite3_bind_int(db->produce_ra_sql_insert, 	5, produce_ra_db[i].skill_lv);
		array_to_string(buf, produce_ra_db[i].item_id_req);
		sqlite3_bind_text(db->produce_ra_sql_insert, 	6, buf, strlen(buf), SQLITE_TRANSIENT);
		array_to_string_cnt(buf, produce_ra_db[i].item_amount_req, array_field_cnt(buf) + 1);
		sqlite3_bind_text(db->produce_ra_sql_insert, 	7, buf, strlen(buf), SQLITE_TRANSIENT);
		sqlite3_step(db->produce_ra_sql_insert);
		sqlite3_reset(db->produce_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(produce_db.db);
	return CHECK_PASSED;
}

int mercenary_ra_sql_load(db_ra_t * db, const char * path) {
	int i = 0;
	native_t mercenary_db;
	mercenary_ra * mercenary_ra_db = NULL;
	memset(&mercenary_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &mercenary_db, &load_ra_native[4]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena mercenary database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(mercenary_db.db == NULL || mercenary_db.size <= 0) {
		exit_func_safe("failed to load rathena mercenary database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, mercenary_ra_db = mercenary_db.db; i < mercenary_db.size; i++) {
		sqlite3_clear_bindings(db->mercenary_ra_sql_insert);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	1, 	mercenary_ra_db[i].id);
		sqlite3_bind_text(db->mercenary_ra_sql_insert, 	2, 	mercenary_ra_db[i].sprite, 	strlen(mercenary_ra_db[i].sprite), 	SQLITE_STATIC);
		sqlite3_bind_text(db->mercenary_ra_sql_insert, 	3, 	mercenary_ra_db[i].name, 	strlen(mercenary_ra_db[i].name), 	SQLITE_STATIC);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	4, 	mercenary_ra_db[i].lv);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	5, 	mercenary_ra_db[i].hp);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	6, 	mercenary_ra_db[i].sp);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	7, 	mercenary_ra_db[i].range1);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	8, 	mercenary_ra_db[i].atk1);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	9, 	mercenary_ra_db[i].atk2);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	10, mercenary_ra_db[i].def);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	11, mercenary_ra_db[i].mdef);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	12, mercenary_ra_db[i].str);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	13, mercenary_ra_db[i].agi);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	14, mercenary_ra_db[i].vit);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	15, mercenary_ra_db[i].intr);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	16, mercenary_ra_db[i].dex);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	17, mercenary_ra_db[i].luk);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	18, mercenary_ra_db[i].range2);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	19, mercenary_ra_db[i].range3);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	20, mercenary_ra_db[i].scale);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	21, mercenary_ra_db[i].race);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	22, mercenary_ra_db[i].element);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	23, mercenary_ra_db[i].speed);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	24, mercenary_ra_db[i].adelay);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	25, mercenary_ra_db[i].amotion);
		sqlite3_bind_int(db->mercenary_ra_sql_insert, 	26, mercenary_ra_db[i].dmotion);
		sqlite3_step(db->mercenary_ra_sql_insert);
		sqlite3_reset(db->mercenary_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(mercenary_db.db);
	return CHECK_PASSED;
}

int pet_ra_sql_load(db_ra_t * db, const char * path) {
	int i = 0;
	native_t pet_db;
	pet_ra * pet_ra_db = NULL;
	memset(&pet_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &pet_db, &load_ra_native[5]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena mercenary database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(pet_db.db == NULL || pet_db.size <= 0) {
		exit_func_safe("failed to load rathena mercenary database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, pet_ra_db = pet_db.db; i < pet_db.size; i++) {
		sqlite3_clear_bindings(db->pet_ra_sql_insert);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	1, 	pet_ra_db[i].mob_id);
		sqlite3_bind_text(db->pet_ra_sql_insert, 	2, 	pet_ra_db[i].name, 			strlen(pet_ra_db[i].name), 			SQLITE_STATIC);
		sqlite3_bind_text(db->pet_ra_sql_insert, 	3, 	pet_ra_db[i].jname, 		strlen(pet_ra_db[i].jname), 		SQLITE_STATIC);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	4, 	pet_ra_db[i].lure_id);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	5, 	pet_ra_db[i].egg_id);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	6, 	pet_ra_db[i].equip_id);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	7, 	pet_ra_db[i].food_id);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	8, 	pet_ra_db[i].fullness);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	9, 	pet_ra_db[i].hungry_delay);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	10, pet_ra_db[i].r_hungry);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	11, pet_ra_db[i].r_full);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	12, pet_ra_db[i].intimate);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	13, pet_ra_db[i].die);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	14, pet_ra_db[i].speed);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	15, pet_ra_db[i].capture);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	16, pet_ra_db[i].s_performance);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	17, pet_ra_db[i].talk_convert);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	18, pet_ra_db[i].attack_rate);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	19, pet_ra_db[i].defence_attack_rate);
		sqlite3_bind_int(db->pet_ra_sql_insert, 	20, pet_ra_db[i].change_target_rate);
		sqlite3_bind_text(db->pet_ra_sql_insert, 	21, pet_ra_db[i].pet_script, 	strlen(pet_ra_db[i].pet_script), 	SQLITE_STATIC);
		sqlite3_bind_text(db->pet_ra_sql_insert, 	22, pet_ra_db[i].loyal_script, 	strlen(pet_ra_db[i].loyal_script), 	SQLITE_STATIC);
		sqlite3_step(db->pet_ra_sql_insert);
		sqlite3_reset(db->pet_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(pet_db.db);
	return CHECK_PASSED;
}

int const_ra_sql_load(db_ra_t * db, const char * path) {
	int i = 0;
	native_t const_db;
	const_ra * const_ra_db = NULL;
	memset(&const_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_general, load_native_general, &const_db, &load_ra_native[7]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena constant database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(const_db.db == NULL || const_db.size <= 0) {
		exit_func_safe("failed to load rathena constant database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, const_ra_db = const_db.db; i < const_db.size; i++) {
		sqlite3_clear_bindings(db->const_ra_sql_insert);
		sqlite3_bind_text(db->const_ra_sql_insert, 	1, const_ra_db[i].name, strlen(const_ra_db[i].name), SQLITE_STATIC);
		sqlite3_bind_int(db->const_ra_sql_insert, 	2, const_ra_db[i].value);
		sqlite3_bind_int(db->const_ra_sql_insert, 	3, const_ra_db[i].type);
		sqlite3_step(db->const_ra_sql_insert);
		sqlite3_reset(db->const_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(const_db.db);
	return CHECK_PASSED;
}

int item_group_ra_sql_load(db_ra_t * db, const char * path, db_ra_aux_t * db_search) {
	int i = 0;
	native_t item_group_db;
	const_ra const_name_search;
	item_group_ra * item_group_ra_db = NULL;
	memset(&item_group_db, 0, sizeof(native_t));
	memset(&const_name_search, 0, sizeof(const_ra));

	/* load the native database */
	if(load_native(path, trim_alpha, load_native_general, &item_group_db, &load_ra_native[6]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena item group database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(item_group_db.db == NULL || item_group_db.size <= 0) {
		exit_func_safe("failed to load rathena item group database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, item_group_ra_db = item_group_db.db; i < item_group_db.size; i++) {
		/* search for group id from group name */
		if(const_ra_name_search(db_search, item_group_ra_db[i].name, &const_name_search) != CHECK_PASSED) {
			exit_func_safe("failed to search for group id from group name %s", item_group_ra_db[i].name);
			break;
		}

		/* load the item group record */
		sqlite3_clear_bindings(db->item_group_ra_sql_insert);
		sqlite3_bind_int(db->item_group_ra_sql_insert, 1, const_name_search.value);
		sqlite3_bind_int(db->item_group_ra_sql_insert, 2, item_group_ra_db[i].item_id);
		sqlite3_bind_int(db->item_group_ra_sql_insert, 3, item_group_ra_db[i].rate);
		sqlite3_step(db->item_group_ra_sql_insert);
		sqlite3_reset(db->item_group_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(item_group_db.db);
	return CHECK_PASSED;
}

int item_package_ra_sql_load(db_ra_t * db, const char * path, db_ra_aux_t * db_search) {
	int i = 0;
	native_t package_db;
	const_ra const_name_search;
	item_ra item_name_search;
	package_ra * package_ra_db = NULL;
	memset(&package_db, 0, sizeof(native_t));
	memset(&const_name_search, 0, sizeof(const_ra));
	memset(&item_name_search, 0, sizeof(item_ra));

	/* load the native database */
	if(load_native(path, trim_alpha, load_native_general, &package_db, &load_ra_native[8]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena item package database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(package_db.db == NULL || package_db.size <= 0) {
		exit_func_safe("failed to load rathena item package database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, package_ra_db = package_db.db; i < package_db.size; i++) {
		/* search for group id from group name */
		if(const_ra_name_search(db_search, package_ra_db[i].group_name, &const_name_search) != CHECK_PASSED) {
			exit_func_safe("failed to search for group id from group name %s", package_ra_db[i].group_name);
			break;
		}
		/* search for item id from item name or verified item id exists */
		if(item_ra_name_search(db_search, package_ra_db[i].item_name, &item_name_search) != CHECK_PASSED) {
			if(item_ra_id_search(db_search, convert_integer(package_ra_db[i].item_name, 10), &item_name_search) != CHECK_PASSED) {
				exit_func_safe("failed to search for item id from item name %s", package_ra_db[i].item_name);
				break;	
			}
		}
		/* load the package into the item group database */
		sqlite3_clear_bindings(db->item_group_ra_sql_insert);
		sqlite3_bind_int(db->item_group_ra_sql_insert, 1, const_name_search.value);
		sqlite3_bind_int(db->item_group_ra_sql_insert, 2, item_name_search.id);
		sqlite3_bind_int(db->item_group_ra_sql_insert, 3, package_ra_db[i].rate);
		sqlite3_step(db->item_group_ra_sql_insert);
		sqlite3_reset(db->item_group_ra_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(package_db.db);
	return CHECK_PASSED;
}

int item_combo_ra_sql_load(db_ra_t * db, const char * path, db_ra_aux_t * db_search) {
	int i = 0;
	int j = 0;
	int offset = 0;
	char buffer[BUF_SIZE];
	native_t combo_db;
	combo_ra * combo_ra_db = NULL;
	item_ra item_name_search;
	array_w item_id_list;
	int * item_id_array = NULL;
	memset(&combo_db, 0, sizeof(native_t));
	memset(&item_name_search, 0, sizeof(item_ra));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &combo_db, &load_ra_native[9]) == CHECK_FAILED) {
		exit_func_safe("failed to load rathena combo database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(combo_db.db == NULL || combo_db.size <= 0) {
		exit_func_safe("failed to load rathena combo database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 rathena database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, combo_ra_db = combo_db.db; i < combo_db.size; i++) {
		offset = 0;
		/* convert the item group string into an array of integers */
		memset(&item_id_list, 0, sizeof(array_w));
		convert_integer_list(combo_ra_db[i].item_id.str, ":", &item_id_list);
		item_id_array = item_id_list.array;

		/* add the item id and script in the array of integers */
		if(item_id_array != NULL) {
			/* build a buffer of all item names */
			memset(&item_name_search, 0, sizeof(item_ra));
			for(j = 0; j < item_id_list.size; j++) {
				if(item_ra_id_search(db_search, item_id_array[j], &item_name_search) == CHECK_PASSED) {
					offset += (offset == 0)?
						sprintf(buffer + offset, "[%s", item_name_search.eathena):
						sprintf(buffer + offset, ", %s", item_name_search.eathena);
					buffer[offset] = '\0';
				}
			}
			offset += sprintf(buffer + offset, " Combo]");

			for(j = 0; j < item_id_list.size; j++) {
				sqlite3_clear_bindings(db->item_combo_ra_sql_insert);
				sqlite3_bind_int(db->item_combo_ra_sql_insert, 1, item_id_array[j]);
				sqlite3_bind_text(db->item_combo_ra_sql_insert, 2, combo_ra_db[i].script, strlen(combo_ra_db[i].script), SQLITE_STATIC);
				sqlite3_bind_text(db->item_combo_ra_sql_insert, 3, buffer, offset, SQLITE_STATIC);
				sqlite3_step(db->item_combo_ra_sql_insert);
				sqlite3_reset(db->item_combo_ra_sql_insert);
			}
			/* free the list of integers */
			free(item_id_list.array);
		}
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(combo_db.db);
	return CHECK_PASSED;
}

int init_ra_search(db_ra_t * db, db_ra_aux_t * db_search) {
	static const char * const_name_sql = "SELECT * FROM const_ra WHERE name = ? COLLATE NOCASE;";
	static const char * item_name_sql = "SELECT id, eathena, script FROM item_ra WHERE eathena = ? OR aegis = ? COLLATE NOCASE;";
	static const char * item_id_sql = "SELECT id, eathena, script FROM item_ra WHERE id = ? COLLATE NOCASE;";
	if(sqlite3_prepare_v2(db->db, const_name_sql, strlen(const_name_sql), &db_search->const_ra_name_search, NULL) != SQLITE_OK ||
	   sqlite3_prepare_v2(db->db, item_name_sql, strlen(item_name_sql), &db_search->item_ra_name_search, NULL) != SQLITE_OK ||
	   sqlite3_prepare_v2(db->db, item_id_sql, strlen(item_id_sql), &db_search->item_ra_id_search, NULL) != SQLITE_OK) {
		exit_abt_safe("item and constant database must be loaded");
		return CHECK_FAILED;
	}
	return CHECK_PASSED;
}

int const_ra_name_search(db_ra_aux_t * db_search, char * group_name, const_ra * const_name_search) {
	int status = 0;
	sqlite3_clear_bindings(db_search->const_ra_name_search);
	sqlite3_bind_text(db_search->const_ra_name_search, 1, group_name, strlen(group_name), SQLITE_STATIC);
	status = sqlite3_step(db_search->const_ra_name_search);
	if(status == SQLITE_ROW) {
		strncopy(const_name_search->name, MAX_NAME_SIZE, sqlite3_column_text(db_search->const_ra_name_search, 0));
		const_name_search->value = sqlite3_column_int(db_search->const_ra_name_search, 1);
		const_name_search->type = sqlite3_column_int(db_search->const_ra_name_search, 2);
	}
	sqlite3_reset(db_search->const_ra_name_search);
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int item_ra_name_search(db_ra_aux_t * db_search, char * item_name, item_ra * item_name_search) {
	int status = 0;
	sqlite3_clear_bindings(db_search->item_ra_name_search);
	sqlite3_bind_text(db_search->item_ra_name_search, 1, item_name, strlen(item_name), SQLITE_STATIC);
	sqlite3_bind_text(db_search->item_ra_name_search, 2, item_name, strlen(item_name), SQLITE_STATIC);
	status = sqlite3_step(db_search->item_ra_name_search);
	if(status == SQLITE_ROW) {
		item_name_search->id = sqlite3_column_int(db_search->item_ra_name_search, 0);
		strncopy(item_name_search->eathena, MAX_NAME_SIZE, sqlite3_column_text(db_search->item_ra_name_search, 1));
		strncopy(item_name_search->script, MAX_SCRIPT_SIZE, sqlite3_column_text(db_search->item_ra_name_search, 2));
	}
	sqlite3_reset(db_search->item_ra_name_search);
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int item_ra_id_search(db_ra_aux_t * db_search, int item_id, item_ra * item_name_search) {
	int status = 0;
	sqlite3_clear_bindings(db_search->item_ra_id_search);
	sqlite3_bind_int(db_search->item_ra_id_search, 1, item_id);
	status = sqlite3_step(db_search->item_ra_id_search);
	if(status == SQLITE_ROW) {
		item_name_search->id = sqlite3_column_int(db_search->item_ra_id_search, 0);
		strncopy(item_name_search->eathena, MAX_NAME_SIZE, sqlite3_column_text(db_search->item_ra_id_search, 1));
    	strncopy(item_name_search->script, MAX_SCRIPT_SIZE, sqlite3_column_text(db_search->item_ra_id_search, 2));
	}
	sqlite3_reset(db_search->item_ra_id_search);
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int deit_ra_search(db_ra_aux_t * db_search) {
	sqlite3_finalize(db_search->const_ra_name_search);
	sqlite3_finalize(db_search->item_ra_name_search);
	sqlite3_finalize(db_search->item_ra_id_search);
	return CHECK_PASSED;
}

int default_rathena_database(void) {
	db_ra_t db;
	db_ra_aux_t db_search;
	create_rathena_database(&db, "/root/Desktop/dev/eAdb.Compiler3/rathena.db");
	item_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_db.txt");
	mob_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/mob_db.txt");
	skill_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/skill_db.txt");
	produce_ra_sql_load(&db, "/root/Desktop/git/rathena/db/produce_db.txt");
	mercenary_ra_sql_load(&db, "/root/Desktop/git/rathena/db/mercenary_db.txt");
	pet_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/pet_db.txt");
	const_ra_sql_load(&db, "/root/Desktop/git/rathena/db/const.txt");

	/* load item and constant search queries */
	init_ra_search(&db, &db_search);
	item_group_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_bluebox.txt", &db_search);
	item_group_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_cardalbum.txt", &db_search);
	item_group_ra_sql_load(&db, "/root/Desktop/git/rathena/db/item_findingore.txt", &db_search);
	item_group_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_giftbox.txt", &db_search);
	item_group_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_violetbox.txt", &db_search);
	item_group_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_misc.txt", &db_search);
	item_package_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_package.txt", &db_search);
	item_combo_ra_sql_load(&db, "/root/Desktop/git/rathena/db/re/item_combo_db.txt", &db_search);
	deit_ra_search(&db_search);
	finalize_rathena_database(&db);
	return CHECK_PASSED;
}

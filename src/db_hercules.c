/*
 *   file: db_hercules.c
 *   date: 03/01/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
 #include "db_hercules.h"

native_config_t load_he_native[HERCULES_DB_COUNT] = {
	{ mob_he_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, MOB_HE_FIELD_COUNT, sizeof(mob_he) },
	{ skill_he_load, sentinel_comment, delimit_cvs_comment, SKIP_NEXT_WS | CHECK_FIELD_COUNT, SKILL_HE_FIELD_COUNT, sizeof(skill_he) },
	{ produce_he_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS, 0, sizeof(produce_he) },
	{ mercenary_he_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, MERCENARY_HE_FIELD_COUNT, sizeof(mercenary_he) },
	{ pet_he_load, sentinel_newline, delimit_cvs, CHECK_BRACKET | SKIP_NEXT_WS | CHECK_FIELD_COUNT, PET_HE_FIELD_COUNT, sizeof(pet_he) },
	{ const_he_load, sentinel_newline, delimit_cvs_whitespace, SKIP_NEXT_WS, 0, sizeof(const_he) },
	{ combo_he_load, sentinel_newline, delimit_cvs, CHECK_BRACKET | SKIP_NEXT_WS | CHECK_FIELD_COUNT, ITEM_COMBO_HE_FIELD_COUNT, sizeof(combo_he) }
};

int item_he_load(const char * file_name, native_t * native) {
	int i = 0;
	int cnt = 0;
	int status = 0;

	/* error reporting */
	int offset = 0;
	char error[BUF_SIZE];

	/* libconfig */
	config_t * cfg = calloc(1, sizeof(config_t));
	config_setting_t * item_db = NULL;
	config_setting_t * item_row = NULL;
	config_setting_t * item_sub = NULL;

	/* item_db */
	item_he * item = NULL;
	const char * str = NULL;



	config_init(cfg);

	/* read item_db.conf */
	status = config_read_file(cfg, file_name);
	if(status != CONFIG_TRUE) {
		offset = sprintf(error, "%s;%d;%s",
			config_error_file(cfg),
			config_error_line(cfg),
			config_error_text(cfg));
		error[offset] = '\0';
		exit_abt_safe(error);
	}

	/* retrieve data */
	item_db = config_lookup(cfg, "item_db");
	if(config_setting_is_list(item_db)) {
		cnt = config_setting_length(item_db);
		item = calloc(cnt, sizeof(item_he));
		if(item == NULL) {
			config_destroy(cfg);
			free(cfg);
			exit_func_safe("failed to allocated memory for item database %s file.\n", file_name);
			return CHECK_FAILED;
		}
		for(i = 0; i < cnt; i++) {
			item_row = config_setting_get_elem(item_db, i);

			config_setting_lookup_int(item_row, "Id", &item[i].id);
			config_setting_lookup_string(item_row, "AegisName", &str);
			strnload(item[i].aegis, MAX_NAME_SIZE, (char *) str);
			config_setting_lookup_string(item_row, "Name", &str);
			strnload(item[i].name, MAX_NAME_SIZE, (char *) str);
			config_setting_lookup_int(item_row, "Type", &item[i].type);
			config_setting_lookup_int(item_row, "Buy", &item[i].buy);
			config_setting_lookup_int(item_row, "Sell", &item[i].sell);
			config_setting_lookup_int(item_row, "Weight", &item[i].weight);
			config_setting_lookup_int(item_row, "Atk", &item[i].atk);
			config_setting_lookup_int(item_row, "Matk", &item[i].matk);
			config_setting_lookup_int(item_row, "Def", &item[i].def);
			config_setting_lookup_int(item_row, "Range", &item[i].range);
			config_setting_lookup_int(item_row, "Slots", &item[i].slots);
			if(config_setting_lookup_int(item_row, "Job", &item[i].job) == CONFIG_FALSE)
				item[i].job = 0xFFFFFFFF;	/* all job */
			if(config_setting_lookup_int(item_row, "Upper", &item[i].upper) == CONFIG_FALSE)
				item[i].upper = 0x3F;		/* all tier */
			if(config_setting_lookup_int(item_row, "Gender", &item[i].gender) == CONFIG_FALSE)
				item[i].gender = 0x2;		/* all sex */
			config_setting_lookup_int(item_row, "Loc", &item[i].loc);
			config_setting_lookup_int(item_row, "WeaponLv", &item[i].weaponlv);
			item_sub = config_setting_get_member(item_row, "EquipLv");
			if(item_sub != NULL && config_setting_is_list(item_sub) == CONFIG_TRUE) {
				item[i].equiplv[EQUIP_MIN] = config_setting_get_int_elem(item_sub, EQUIP_MIN);
				item[i].equiplv[EQUIP_MAX] = config_setting_get_int_elem(item_sub, EQUIP_MAX);
			} else if(item_sub != NULL) {
				config_setting_lookup_int(item_sub, "EquipLv", &item[i].equiplv[EQUIP_MIN]);
				item[i].equiplv[EQUIP_MAX] = item[i].equiplv[EQUIP_MIN];
			}
			if(config_setting_lookup_bool(item_row, "Refine", &item[i].refine) == CONFIG_FALSE)
				item[i].refine = 1;
			config_setting_lookup_int(item_row, "View", &item[i].view);
			config_setting_lookup_bool(item_row, "BindOnEquip", &item[i].bindonequip);
			config_setting_lookup_bool(item_row, "BuyingStore", &item[i].buyingstore);
			config_setting_lookup_int(item_row, "Delay", &item[i].delay);
			item_sub = config_setting_get_member(item_row, "Trade");
			if(item_sub != NULL && config_setting_is_group(item_sub) == CONFIG_TRUE) {
				if(config_setting_lookup_int(item_sub, "override", &item[i].trade[TRADE_OVERRIDE]) == CONFIG_FALSE)
					item[i].trade[TRADE_OVERRIDE] = 100; /* every group */
				config_setting_lookup_bool(item_sub, "nodrop", &item[i].trade[TRADE_NODROP]);
				config_setting_lookup_bool(item_sub, "notrade", &item[i].trade[TRADE_NOTRADE]);
				config_setting_lookup_bool(item_sub, "partneroverride", &item[i].trade[TRADE_PARTNEROVERRIDE]);
				config_setting_lookup_bool(item_sub, "noselltonpc", &item[i].trade[TRADE_NOSELLTONPC]);
				config_setting_lookup_bool(item_sub, "nocart", &item[i].trade[TRADE_NOCART]);
				config_setting_lookup_bool(item_sub, "nostorage", &item[i].trade[TRADE_NOSTORAGE]);
				config_setting_lookup_bool(item_sub, "nogstorage", &item[i].trade[TRADE_NOGSTORAGE]);
				config_setting_lookup_bool(item_sub, "nomail", &item[i].trade[TRADE_NOMAIL]);
				config_setting_lookup_bool(item_sub, "noauction", &item[i].trade[TRADE_NOAUCTION]);
			}
			item_sub = config_setting_get_member(item_row, "Nouse");
			if(item_sub != NULL && config_setting_is_group(item_sub) == CONFIG_TRUE) {
				if(config_setting_lookup_int(item_row, "override", &item[i].trade[NOUSE_OVERRIDE]) == CONFIG_FALSE)
					item[i].trade[NOUSE_OVERRIDE] = 100; /* every group */
				config_setting_lookup_bool(item_row, "sitting", &item[i].trade[NOUSE_SITTING]);
			}
			item_sub = config_setting_get_member(item_row, "Stack");
			if(item_sub != NULL && config_setting_is_array(item_sub) == CONFIG_TRUE) {
				item[i].stack[STACK_AMOUNT] = config_setting_get_int_elem(item_sub, STACK_AMOUNT);
				item[i].stack[STACK_TYPE] = config_setting_get_int_elem(item_sub, STACK_TYPE);
			}
			if(config_setting_lookup_string(item_row, "Script", &str) == CONFIG_FALSE)
				strnload(item[i].script, MAX_SCRIPT_SIZE, "");
			else
				strnload(item[i].script, MAX_SCRIPT_SIZE, (char *) str);

			if(config_setting_lookup_string(item_row, "OnEquipScript", &str) == CONFIG_FALSE)
				strnload(item[i].onequipscript, MAX_SCRIPT_SIZE, "");
			else
				strnload(item[i].onequipscript, MAX_SCRIPT_SIZE, (char *) str);

			if(config_setting_lookup_string(item_row, "OnUnequipScript", &str) == CONFIG_FALSE)
				strnload(item[i].onunequipscript, MAX_SCRIPT_SIZE, "");
			else
				strnload(item[i].onunequipscript, MAX_SCRIPT_SIZE, (char *) str);
		}
	} else {
		exit_abt_safe("failed to find item configuration file root setting.");
	}
	config_destroy(cfg);
	free(cfg);

	native->db = item;
	native->size = cnt;
	return CHECK_PASSED;
}

int mob_he_load(void * db, int row, int col, char * val) {
	mob_he * record = &((mob_he *) db)[row];
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
		default: exit_func_safe("invalid column field %d in hercules mob database", col);
	}
	return 0;
}

int skill_he_load(void * db, int row, int col, char * val) {
	skill_he * record = &((skill_he *) db)[row];
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
		case 15: strnload(record->name, MAX_NAME_SIZE, val);				break;
		case 16: strnload(record->desc, MAX_NAME_SIZE, val); 				break;
		default: exit_func_safe("invalid column field %d (value '%s') in hercules skill database", col, val);
	}
	return 0;
}

int produce_he_load(void * db, int row, int col, char * val) {
	/* non-reentrant produce loading function */
	static int material_cnt = 0;
	static int alternate = 0;
	produce_he * record = &((produce_he *) db)[row];
	switch(col) {
		case 0:
			material_cnt = 0;
			alternate = 0;
			record->item_id = convert_integer(val,10);							break;
		case 1: record->item_lv = convert_integer(val,10); 						break;
		case 2: record->skill_id = convert_integer(val,10); 					break;
		case 3: record->skill_lv = convert_integer(val,10); 					break;
		default:
			(!alternate) ?
				(record->item_id_req[material_cnt] = convert_integer(val,10)):
				(record->item_amount_req[material_cnt++] = convert_integer(val,10));
			alternate = !alternate;
			record->ingredient_count = material_cnt;							break;
	}
	return 0;
}

int mercenary_he_load(void * db, int row, int col, char * val) {
	mercenary_he * record = &((mercenary_he *) db)[row];
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
		default: exit_func_safe("invalid column field %d in hercules mercenary database.\n", col);
	}
	return 0;
}

int pet_he_load(void * db, int row, int col, char * val) {
	pet_he * record = &((pet_he *) db)[row];
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
		default: exit_func_safe("invalid column field %d in hercules pet database.\n", col);
	}
	return 0;
}

int const_he_load(void * db, int row, int col, char * val) {
	const_he * record = &((const_he *) db)[row];
	switch(col) {
		case 0: strnload(record->name, MAX_NAME_SIZE, val); 							break;
		case 1:
			/* constant can be represented as hexadecimal or decimal */
			record->value = (strlen(val) > 2 && val[0] == '0' && val[1] == 'x') ?
				convert_integer(val, 16):
				convert_integer(val, 10);												break;
		case 2: record->type = convert_integer(val, 10); 								break;
		default: exit_func_safe("invalid column field %d in hercules const database.\n", col);
	}
	return 0;
}

int combo_he_load(void * db, int row, int col, char * val) {
	combo_he * record = &((combo_he *) db)[row];
	switch(col) {
		case 0: strnload(record->item_id.str, MAX_VARARG_SIZE, val); 	break;
		case 1: strnload(record->script, MAX_SCRIPT_SIZE, val); 		break;
		default: break;
	}
	return 0;
}

int herc_db_init(herc_db_t ** db, const char * path) {
	char * error = NULL;
	herc_db_t * herc = NULL;

	if(exit_null_safe(2, db, path))
		return CHECK_FAILED;

	herc = calloc(1, sizeof(herc_db_t));
	if(NULL == herc)
		goto failed;

	/* create database */
	if( SQLITE_OK != sqlite3_open_v2(path, &herc->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL)) {
		if(NULL != herc->db)
			fprintf(stderr, "Failed to create %s; %s.\n", path, sqlite3_errmsg(herc->db));
		goto failed;
	}

	/* load database schema */
	if(SQLITE_OK != sqlite3_exec(herc->db,
		HERC_ITEM_DELETE
		HERC_ITEM_CREATE
		HERC_MOB_DELETE
		HERC_MOB_CREATE
		HERC_SKILL_DELETE
		HERC_SKILL_CREATE
		HERC_PRODUCE_DELETE
		HERC_PRODUCE_CREATE
		HERC_MERC_DELETE
		HERC_MERC_CREATE
		HERC_PET_DELETE
		HERC_PET_CREATE
		HERC_CONST_DELETE
		HERC_CONST_CREATE
		HERC_COMBO_DELETE
		HERC_COMBO_CREATE,
		NULL, NULL, &error)) {
		if(NULL != error) {
			fprintf(stderr, "Failed to load schema %s; %s.\n", path, error);
			sqlite3_free(error);
		}
		goto failed;
	}

	/* compile sql statements */
	if( SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_ITEM_INSERT, strlen(HERC_ITEM_INSERT) + 1, &herc->item_he_sql_insert, NULL)		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_MOB_INSERT, strlen(HERC_MOB_INSERT) + 1, &herc->mob_he_sql_insert, NULL) 		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_SKILL_INSERT, strlen(HERC_SKILL_INSERT), &herc->skill_he_sql_insert, NULL)		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_PRODUCE_INSERT, strlen(HERC_PRODUCE_INSERT), &herc->produce_he_sql_insert, NULL) ||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_MERC_INSERT, strlen(HERC_MERC_INSERT), &herc->mercenary_he_sql_insert, NULL) 	||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_PET_INSERT, strlen(HERC_PET_INSERT), &herc->pet_he_sql_insert, NULL) 			||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_CONST_INSERT, strlen(HERC_CONST_INSERT), &herc->const_he_sql_insert, NULL) 		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_COMBO_INSERT, strlen(HERC_COMBO_INSERT), &herc->item_combo_he_sql_insert, NULL)	) {
		fprintf(stderr, "Failed to sql query; %s.\n", sqlite3_errmsg(herc->db));
		goto failed;
	}

	*db = herc;
	return CHECK_PASSED;

failed:
	if(NULL != herc->db)
		if(SQLITE_OK != sqlite3_close(herc->db)) {
			fprintf(stderr, "Failed to close %s; %s.\n", path, sqlite3_errmsg(herc->db));
			exit(EXIT_FAILURE);
		}
	return CHECK_FAILED;
}

int herc_db_deit(herc_db_t ** db) {
	herc_db_t * herc = NULL;

	if(exit_null_safe(2, db, *db))
		return CHECK_FAILED;

	herc = *db;
	if( SQLITE_OK != sqlite3_finalize(herc->item_he_sql_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->mob_he_sql_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->skill_he_sql_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->produce_he_sql_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->mercenary_he_sql_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->pet_he_sql_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->const_he_sql_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->item_combo_he_sql_insert) ||
		SQLITE_OK != sqlite3_close(herc->db)) {
		fprintf(stderr, "Failed to unload database; %s.\n", sqlite3_errmsg(herc->db));
		exit(EXIT_FAILURE);
	}
	*db = NULL;
	return CHECK_PASSED;
}

int herc_db_exec(herc_db_t * db, char * sql) {
	char * error = NULL;

	if (SQLITE_OK != sqlite3_exec(db->db, sql, NULL, NULL, &error)) {
		if(NULL != error) {
			fprintf(stderr, "Failed to exec %s; %s.\n", sql, error);
			sqlite3_free(error);
		}
		return CHECK_FAILED;
	}

	return CHECK_PASSED;
}

int herc_load_item_db(herc_db_t * db, const char * path) {
	native_t items;
	if( item_he_load(path, &items) ||
		NULL == items.db ||
		0 >= items.size) {
		fprintf(stderr, "Failed to load item db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_item_db_insert(items.db, items.size, db->item_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load item db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	SAFE_FREE(items.db);
	return CHECK_PASSED;
}

int herc_load_item_db_insert(item_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	item_he * item = NULL;

	for(i = 0; i < size; i++) {
		item = &db[i];
		if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_int(sql, 1, item->id) ||
			SQLITE_OK != sqlite3_bind_text(sql, 2, item->aegis, strlen(item->aegis), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 3, item->name, strlen(item->name), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 4, item->type) ||
			SQLITE_OK != sqlite3_bind_int(sql, 5, item->buy) ||
			SQLITE_OK != sqlite3_bind_int(sql, 6, item->sell) ||
			SQLITE_OK != sqlite3_bind_int(sql, 7, item->weight) ||
			SQLITE_OK != sqlite3_bind_int(sql, 8, item->atk) ||
			SQLITE_OK != sqlite3_bind_int(sql, 9, item->matk) ||
			SQLITE_OK != sqlite3_bind_int(sql, 10, item->def) ||
			SQLITE_OK != sqlite3_bind_int(sql, 11, item->range) ||
			SQLITE_OK != sqlite3_bind_int(sql, 12, item->slots) ||
			SQLITE_OK != sqlite3_bind_int(sql, 13, item->job) ||
			SQLITE_OK != sqlite3_bind_int(sql, 14, item->upper) ||
			SQLITE_OK != sqlite3_bind_int(sql, 15, item->gender) ||
			SQLITE_OK != sqlite3_bind_int(sql, 16, item->loc) ||
			SQLITE_OK != sqlite3_bind_int(sql, 17, item->weaponlv) ||
			SQLITE_OK != sqlite3_bind_int(sql, 18, item->equiplv[EQUIP_MIN]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 19, item->equiplv[EQUIP_MAX]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 20, item->refine) ||
			SQLITE_OK != sqlite3_bind_int(sql, 21, item->view) ||
			SQLITE_OK != sqlite3_bind_int(sql, 22, item->bindonequip) ||
			SQLITE_OK != sqlite3_bind_int(sql, 23, item->buyingstore) ||
			SQLITE_OK != sqlite3_bind_int(sql, 24, item->delay) ||
			SQLITE_OK != sqlite3_bind_int(sql, 25, item->trade[TRADE_OVERRIDE]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 26, item->trade[TRADE_NODROP]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 27, item->trade[TRADE_NOTRADE]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 28, item->trade[TRADE_PARTNEROVERRIDE]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 29, item->trade[TRADE_NOSELLTONPC]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 30, item->trade[TRADE_NOCART]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 31, item->trade[TRADE_NOSTORAGE]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 32, item->trade[TRADE_NOGSTORAGE]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 33, item->trade[TRADE_NOMAIL]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 34, item->trade[TRADE_NOAUCTION]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 35, item->nouse[NOUSE_OVERRIDE]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 36, item->nouse[NOUSE_SITTING]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 37, item->stack[STACK_AMOUNT]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 38, item->stack[STACK_TYPE]) ||
			SQLITE_OK != sqlite3_bind_int(sql, 39, item->sprite) ||
			SQLITE_OK != sqlite3_bind_text(sql, 40, item->script, strlen(item->script), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 41, item->onequipscript, strlen(item->onequipscript), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 42, item->onunequipscript, strlen(item->onunequipscript), SQLITE_STATIC) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			if(SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
			fprintf(stderr, "[load]: failed to add %s to item db; %s.\n");
		} else {
			fprintf(stderr,"[load]: %d/%d ... %-100s\r", i, size, item->aegis);
		}
	}
	return CHECK_PASSED;
}

int herc_load_mob_db(herc_db_t * db, const char * path) {
	native_t mobs;
	if( load_native(path, trim_numeric, load_native_general, &mobs, &load_he_native[0]) ||
		NULL == mobs.db ||
		0 >= mobs.size) {
		fprintf(stderr, "Failed to load mob db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_mob_db_insert(mobs.db, mobs.size, db->mob_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load mob db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	SAFE_FREE(mobs.db);
	return CHECK_PASSED;
}

int herc_load_mob_db_insert(mob_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	mob_he * mob = NULL;

	if(db == NULL || size <= 0) {
		fprintf(stderr, "Failed to load mob database.\n");
		return CHECK_FAILED;
	}

	for (i = 0; i < size; i++) {
		mob = &db[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_int(sql, 1, mob->id) ||
			SQLITE_OK != sqlite3_bind_text(sql, 2, mob->sprite, strlen(mob->sprite), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 3, mob->kro, strlen(mob->kro), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 4, mob->iro, strlen(mob->iro), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 5, mob->lv) ||
			SQLITE_OK != sqlite3_bind_int(sql, 6, mob->hp) ||
			SQLITE_OK != sqlite3_bind_int(sql, 7, mob->sp) ||
			SQLITE_OK != sqlite3_bind_int(sql, 8, mob->exp) ||
			SQLITE_OK != sqlite3_bind_int(sql, 9, mob->jexp) ||
			SQLITE_OK != sqlite3_bind_int(sql, 10, mob->range) ||
			SQLITE_OK != sqlite3_bind_int(sql, 11, mob->atk1) ||
			SQLITE_OK != sqlite3_bind_int(sql, 12, mob->atk2) ||
			SQLITE_OK != sqlite3_bind_int(sql, 13, mob->def) ||
			SQLITE_OK != sqlite3_bind_int(sql, 14, mob->mdef) ||
			SQLITE_OK != sqlite3_bind_int(sql, 15, mob->str) ||
			SQLITE_OK != sqlite3_bind_int(sql, 16, mob->agi) ||
			SQLITE_OK != sqlite3_bind_int(sql, 17, mob->vit) ||
			SQLITE_OK != sqlite3_bind_int(sql, 18, mob->intr) ||
			SQLITE_OK != sqlite3_bind_int(sql, 19, mob->dex) ||
			SQLITE_OK != sqlite3_bind_int(sql, 20, mob->luk) ||
			SQLITE_OK != sqlite3_bind_int(sql, 21, mob->range2) ||
			SQLITE_OK != sqlite3_bind_int(sql, 22, mob->range3) ||
			SQLITE_OK != sqlite3_bind_int(sql, 23, mob->scale) ||
			SQLITE_OK != sqlite3_bind_int(sql, 24, mob->race) ||
			SQLITE_OK != sqlite3_bind_int(sql, 25, mob->element) ||
			SQLITE_OK != sqlite3_bind_int(sql, 26, mob->mode) ||
			SQLITE_OK != sqlite3_bind_int(sql, 27, mob->speed) ||
			SQLITE_OK != sqlite3_bind_int(sql, 28, mob->adelay) ||
			SQLITE_OK != sqlite3_bind_int(sql, 29, mob->amotion) ||
			SQLITE_OK != sqlite3_bind_int(sql, 30, mob->dmotion) ||
			SQLITE_OK != sqlite3_bind_int(sql, 31, mob->mexp) ||
			SQLITE_OK != sqlite3_bind_int(sql, 32, mob->mvp1id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 33, mob->mvp1per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 34, mob->mvp2id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 35, mob->mvp2per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 36, mob->mvp3id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 37, mob->mvp3per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 38, mob->drop1id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 39, mob->drop1per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 40, mob->drop2id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 41, mob->drop2per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 42, mob->drop3id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 43, mob->drop3per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 44, mob->drop4id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 45, mob->drop4per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 46, mob->drop5id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 47, mob->drop5per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 48, mob->drop6id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 49, mob->drop6per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 50, mob->drop7id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 51, mob->drop7per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 52, mob->drop8id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 53, mob->drop8per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 54, mob->drop9id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 55, mob->drop9per) ||
			SQLITE_OK != sqlite3_bind_int(sql, 56, mob->dropcardid) ||
			SQLITE_OK != sqlite3_bind_int(sql, 57, mob->dropcardper) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %s (%d) to mob db.\n", mob->iro, mob->id);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			fprintf(stderr,"[load]: %d/%d ... %-100s\r", i, size, mob->iro);
		}
	}
	return CHECK_PASSED;
}

int skill_he_sql_load(herc_db_t * db, const char * path) {
	int i = 0;
	native_t skill_db;
	skill_he * skill_he_db = NULL;
	memset(&skill_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &skill_db, &load_he_native[1]) == CHECK_FAILED) {
		exit_func_safe("failed to load hercules skill database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(skill_db.db == NULL || skill_db.size <= 0) {
		exit_func_safe("failed to load hercules skill database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 hercules database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, skill_he_db = skill_db.db; i < skill_db.size; i++) {
		sqlite3_clear_bindings(db->skill_he_sql_insert);
		sqlite3_bind_int(db->skill_he_sql_insert, 	1, 	skill_he_db[i].id);
		sqlite3_bind_text(db->skill_he_sql_insert, 	2, 	skill_he_db[i].range.str, 		strlen(skill_he_db[i].range.str), 		SQLITE_STATIC);
		sqlite3_bind_int(db->skill_he_sql_insert, 	3, 	skill_he_db[i].hit);
		sqlite3_bind_int(db->skill_he_sql_insert, 	4, 	skill_he_db[i].inf);
		sqlite3_bind_text(db->skill_he_sql_insert, 	5, 	skill_he_db[i].element.str, 	strlen(skill_he_db[i].element.str), 	SQLITE_STATIC);
		sqlite3_bind_int(db->skill_he_sql_insert, 	6, 	skill_he_db[i].nk);
		sqlite3_bind_text(db->skill_he_sql_insert, 	7, 	skill_he_db[i].splash.str, 		strlen(skill_he_db[i].splash.str), 		SQLITE_STATIC);
		sqlite3_bind_int(db->skill_he_sql_insert, 	8, 	skill_he_db[i].maxlv);
		sqlite3_bind_text(db->skill_he_sql_insert, 	9, 	skill_he_db[i].hit_amount.str, 	strlen(skill_he_db[i].hit_amount.str), 	SQLITE_STATIC);
		sqlite3_bind_text(db->skill_he_sql_insert, 	10, skill_he_db[i].cast_cancel, 	strlen(skill_he_db[i].cast_cancel), SQLITE_STATIC);
		sqlite3_bind_int(db->skill_he_sql_insert, 	11, skill_he_db[i].cast_def_reduce_rate);
		sqlite3_bind_int(db->skill_he_sql_insert, 	12, skill_he_db[i].inf2);
		sqlite3_bind_text(db->skill_he_sql_insert, 	13, skill_he_db[i].maxcount.str, 	strlen(skill_he_db[i].maxcount.str), 	SQLITE_STATIC);
		sqlite3_bind_text(db->skill_he_sql_insert, 	14, skill_he_db[i].type, 			strlen(skill_he_db[i].type), 		SQLITE_STATIC);
		sqlite3_bind_text(db->skill_he_sql_insert, 	15, skill_he_db[i].blow_count.str, 	strlen(skill_he_db[i].blow_count.str), 	SQLITE_STATIC);
		sqlite3_bind_text(db->skill_he_sql_insert, 	16, skill_he_db[i].name, 			strlen(skill_he_db[i].name), 		SQLITE_STATIC);
		sqlite3_bind_text(db->skill_he_sql_insert, 	17, skill_he_db[i].desc, 			strlen(skill_he_db[i].desc), 		SQLITE_STATIC);
		sqlite3_step(db->skill_he_sql_insert);
		sqlite3_reset(db->skill_he_sql_insert);
		fprintf(stderr,"[load]: %d/%d\r", i, skill_db.size);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(skill_db.db);
	return CHECK_PASSED;
}

int produce_he_sql_load(herc_db_t * db, const char * path) {
	int i = 0;
	char buf[BUF_SIZE];
	native_t produce_db;
	produce_he * produce_he_db = NULL;
	memset(&produce_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &produce_db, &load_he_native[2]) == CHECK_FAILED) {
		exit_func_safe("failed to load hercules produce database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(produce_db.db == NULL || produce_db.size <= 0) {
		exit_func_safe("failed to load hercules produce database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 hercules database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, produce_he_db = produce_db.db; i < produce_db.size; i++) {
		sqlite3_clear_bindings(db->produce_he_sql_insert);
		sqlite3_bind_int(db->produce_he_sql_insert, 	1, produce_he_db[i].item_id);
		sqlite3_bind_int(db->produce_he_sql_insert, 	2, produce_he_db[i].item_lv);
		sqlite3_bind_int(db->produce_he_sql_insert, 	3, produce_he_db[i].skill_id);
		sqlite3_bind_int(db->produce_he_sql_insert, 	4, produce_he_db[i].skill_lv);
		array_to_string(buf, produce_he_db[i].item_id_req);
		sqlite3_bind_text(db->produce_he_sql_insert, 	5, buf, strlen(buf), SQLITE_TRANSIENT);
		array_to_string_cnt(buf, produce_he_db[i].item_amount_req, array_field_cnt(buf) + 1);
		sqlite3_bind_text(db->produce_he_sql_insert, 	6, buf, strlen(buf), SQLITE_TRANSIENT);
		sqlite3_step(db->produce_he_sql_insert);
		sqlite3_reset(db->produce_he_sql_insert);
		fprintf(stderr,"[load]: %d/%d\r", i, produce_db.size);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(produce_db.db);
	return CHECK_PASSED;
}

int mercenary_he_sql_load(herc_db_t * db, const char * path) {
	int i = 0;
	native_t mercenary_db;
	mercenary_he * mercenary_he_db = NULL;
	memset(&mercenary_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &mercenary_db, &load_he_native[3]) == CHECK_FAILED) {
		exit_func_safe("failed to load hercules mercenary database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(mercenary_db.db == NULL || mercenary_db.size <= 0) {
		exit_func_safe("failed to load hercules mercenary database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 hercules database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, mercenary_he_db = mercenary_db.db; i < mercenary_db.size; i++) {
		sqlite3_clear_bindings(db->mercenary_he_sql_insert);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	1, mercenary_he_db[i].id);
		sqlite3_bind_text(db->mercenary_he_sql_insert, 	2, mercenary_he_db[i].sprite, 		strlen(mercenary_he_db[i].sprite), SQLITE_STATIC);
		sqlite3_bind_text(db->mercenary_he_sql_insert, 	3, mercenary_he_db[i].name, 		strlen(mercenary_he_db[i].name), SQLITE_STATIC);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	4, mercenary_he_db[i].lv);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	5, mercenary_he_db[i].hp);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	6, mercenary_he_db[i].sp);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	7, mercenary_he_db[i].range1);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	8, mercenary_he_db[i].atk1);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	9, mercenary_he_db[i].atk2);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	10, mercenary_he_db[i].def);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	11, mercenary_he_db[i].mdef);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	12, mercenary_he_db[i].str);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	13, mercenary_he_db[i].agi);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	14, mercenary_he_db[i].vit);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	15, mercenary_he_db[i].intr);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	16, mercenary_he_db[i].dex);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	17, mercenary_he_db[i].luk);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	18, mercenary_he_db[i].range2);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	19, mercenary_he_db[i].range3);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	20, mercenary_he_db[i].scale);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	21, mercenary_he_db[i].race);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	22, mercenary_he_db[i].element);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	23, mercenary_he_db[i].speed);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	24, mercenary_he_db[i].adelay);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	25, mercenary_he_db[i].amotion);
		sqlite3_bind_int(db->mercenary_he_sql_insert, 	26, mercenary_he_db[i].dmotion);
		sqlite3_step(db->mercenary_he_sql_insert);
		sqlite3_reset(db->mercenary_he_sql_insert);
		fprintf(stderr,"[load]: %d/%d\r", i, mercenary_db.size);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(mercenary_db.db);
	return CHECK_PASSED;
}

int pet_he_sql_load(herc_db_t * db, const char * path) {
	int i = 0;
	native_t pet_db;
	pet_he * pet_he_db = NULL;
	memset(&pet_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &pet_db, &load_he_native[4]) == CHECK_FAILED) {
		exit_func_safe("failed to load hercules pet database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(pet_db.db == NULL || pet_db.size <= 0) {
		exit_func_safe("failed to load hercules pet database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 hercules database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, pet_he_db = pet_db.db; i < pet_db.size; i++) {
		sqlite3_clear_bindings(db->pet_he_sql_insert);
		sqlite3_bind_int(db->pet_he_sql_insert, 	1, pet_he_db[i].mob_id);
		sqlite3_bind_text(db->pet_he_sql_insert, 	2, pet_he_db[i].name, 		strlen(pet_he_db[i].name), SQLITE_STATIC);
		sqlite3_bind_text(db->pet_he_sql_insert, 	3, pet_he_db[i].jname, 		strlen(pet_he_db[i].jname), SQLITE_STATIC);
		sqlite3_bind_int(db->pet_he_sql_insert, 	4, pet_he_db[i].lure_id);
		sqlite3_bind_int(db->pet_he_sql_insert, 	5, pet_he_db[i].egg_id);
		sqlite3_bind_int(db->pet_he_sql_insert, 	6, pet_he_db[i].equip_id);
		sqlite3_bind_int(db->pet_he_sql_insert, 	7, pet_he_db[i].food_id);
		sqlite3_bind_int(db->pet_he_sql_insert, 	8, pet_he_db[i].fullness);
		sqlite3_bind_int(db->pet_he_sql_insert, 	9, pet_he_db[i].hungry_delay);
		sqlite3_bind_int(db->pet_he_sql_insert, 	10, pet_he_db[i].r_hungry);
		sqlite3_bind_int(db->pet_he_sql_insert, 	11, pet_he_db[i].r_full);
		sqlite3_bind_int(db->pet_he_sql_insert, 	12, pet_he_db[i].intimate);
		sqlite3_bind_int(db->pet_he_sql_insert, 	13, pet_he_db[i].die);
		sqlite3_bind_int(db->pet_he_sql_insert, 	14, pet_he_db[i].speed);
		sqlite3_bind_int(db->pet_he_sql_insert, 	15, pet_he_db[i].capture);
		sqlite3_bind_int(db->pet_he_sql_insert, 	16, pet_he_db[i].s_performance);
		sqlite3_bind_int(db->pet_he_sql_insert, 	17, pet_he_db[i].talk_convert);
		sqlite3_bind_int(db->pet_he_sql_insert, 	18, pet_he_db[i].attack_rate);
		sqlite3_bind_int(db->pet_he_sql_insert, 	19, pet_he_db[i].defence_attack_rate);
		sqlite3_bind_int(db->pet_he_sql_insert, 	20, pet_he_db[i].change_target_rate);
		sqlite3_bind_text(db->pet_he_sql_insert, 	21, pet_he_db[i].pet_script, 	strlen(pet_he_db[i].pet_script), SQLITE_STATIC);
		sqlite3_bind_text(db->pet_he_sql_insert, 	22, pet_he_db[i].loyal_script, 	strlen(pet_he_db[i].loyal_script), SQLITE_STATIC);
		sqlite3_step(db->pet_he_sql_insert);
		sqlite3_reset(db->pet_he_sql_insert);
		fprintf(stderr,"[load]: %d/%d\r", i, pet_db.size);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(pet_db.db);
	return CHECK_PASSED;
}

int const_he_sql_load(herc_db_t * db, const char * path) {
	int i = 0;
	native_t const_db;
	const_he * const_he_db = NULL;
	memset(&const_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_general, load_native_general, &const_db, &load_he_native[5]) == CHECK_FAILED) {
		exit_func_safe("failed to load hercules constant database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(const_db.db == NULL || const_db.size <= 0) {
		exit_func_safe("failed to load hercules constant database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 hercules database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, const_he_db = const_db.db; i < const_db.size; i++) {
		sqlite3_clear_bindings(db->const_he_sql_insert);
		sqlite3_bind_text(db->const_he_sql_insert, 	1, const_he_db[i].name, strlen(const_he_db[i].name), SQLITE_STATIC);
		sqlite3_bind_int(db->const_he_sql_insert, 	2, const_he_db[i].value);
		sqlite3_bind_int(db->const_he_sql_insert, 	3, const_he_db[i].type);
		sqlite3_step(db->const_he_sql_insert);
		sqlite3_reset(db->const_he_sql_insert);
		fprintf(stderr,"[load]: %d/%d\r", i, const_db.size);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(const_db.db);
	return CHECK_PASSED;
}

int item_combo_he_sql_load(herc_db_t * db, const char * path, db_he_aux_t * db_search) {
	int i = 0;
	int j = 0;
	int offset = 0;
	char buffer[BUF_SIZE];
	native_t combo_db;
	combo_he * combo_he_db = NULL;
	item_he item_name_search;
	array_w item_id_list;
	int * item_id_array = NULL;
	memset(&combo_db, 0, sizeof(native_t));
	memset(&item_name_search, 0, sizeof(item_he));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &combo_db, &load_he_native[6]) == CHECK_FAILED) {
		exit_func_safe("failed to load hercules combo database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(combo_db.db == NULL || combo_db.size <= 0) {
		exit_func_safe("failed to load hercules combo database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 hercules database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, combo_he_db = combo_db.db; i < combo_db.size; i++) {
		offset = 0;
		/* convert the item group string into an array of integers */
		memset(&item_id_list, 0, sizeof(array_w));
		convert_integer_delimit_array(combo_he_db[i].item_id.str, ":", &item_id_list);
		item_id_array = item_id_list.array;

		/* add the item id and script in the array of integers */
		if(item_id_array != NULL) {
			/* build a buffer of all item names */
			memset(&item_name_search, 0, sizeof(item_he));
			for(j = 0; j < item_id_list.size; j++) {
				if(item_he_id_search(db_search, item_id_array[j], &item_name_search) == CHECK_PASSED) {
					offset += (offset == 0)?
						sprintf(buffer + offset, "[%s", item_name_search.name):
						sprintf(buffer + offset, ", %s", item_name_search.name);
					buffer[offset] = '\0';
				}
			}
			offset += sprintf(buffer + offset, " Combo]");

			for(j = 0; j < item_id_list.size; j++) {
				sqlite3_clear_bindings(db->item_combo_he_sql_insert);
				sqlite3_bind_int(db->item_combo_he_sql_insert, 1, item_id_array[j]);
				sqlite3_bind_text(db->item_combo_he_sql_insert, 2, combo_he_db[i].script, strlen(combo_he_db[i].script), SQLITE_STATIC);
				sqlite3_bind_text(db->item_combo_he_sql_insert, 3, buffer, offset, SQLITE_STATIC);
				sqlite3_step(db->item_combo_he_sql_insert);
				sqlite3_reset(db->item_combo_he_sql_insert);
				fprintf(stderr,"[load]: %d/%d\r", i, combo_db.size);
			}
			/* free the list of integers */
			free(item_id_list.array);
		}
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(combo_db.db);
	return CHECK_PASSED;
}

int init_he_search(herc_db_t * db, db_he_aux_t * db_search) {
	static const char * item_id_sql = "SELECT Id, Name, Script FROM item_he WHERE id = ? COLLATE NOCASE;";
	if(sqlite3_prepare_v2(db->db, item_id_sql, strlen(item_id_sql), &db_search->item_he_id_search, NULL) != SQLITE_OK) {
		exit_abt_safe("item database must be loaded");
		return CHECK_FAILED;
	}
	return CHECK_PASSED;
}

int deit_he_search(db_he_aux_t * db_search) {
	sqlite3_finalize(db_search->item_he_id_search);
	return CHECK_PASSED;
}

int item_he_id_search(db_he_aux_t * db_search, int item_id, item_he * item_name_search) {
	int status = 0;
	sqlite3_clear_bindings(db_search->item_he_id_search);
	sqlite3_bind_int(db_search->item_he_id_search, 1, item_id);
	status = sqlite3_step(db_search->item_he_id_search);
	if(status == SQLITE_ROW) {
		item_name_search->id = sqlite3_column_int(db_search->item_he_id_search, 0);
		strncopy(item_name_search->name, MAX_NAME_SIZE, sqlite3_column_text(db_search->item_he_id_search, 1));
    	strncopy(item_name_search->script, MAX_SCRIPT_SIZE, sqlite3_column_text(db_search->item_he_id_search, 2));
	}
	sqlite3_reset(db_search->item_he_id_search);
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}
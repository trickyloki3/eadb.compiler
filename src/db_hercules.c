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

int load_he_item(const char * file_name, native_t * native) {
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

int load_hercules_database(const char * hercules_path) {
	int status = 0;

	native_t item_db;
	load_he_item("/root/Desktop/git/Hercules/db/re/item_db.conf", &item_db);
	free(item_db.db);

	native_t mob_db;
	status = load_native("/root/Desktop/git/Hercules/db/re/mob_db.txt",
	trim_numeric, load_native_general, &mob_db, &load_he_native[0]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules mob database.\n");
		exit(EXIT_FAILURE);
	}
	free(mob_db.db);

	native_t skill_db;
	status = load_native("/root/Desktop/git/Hercules/db/re/skill_db.txt",
	trim_numeric, load_native_general, &skill_db, &load_he_native[1]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules skill database.\n");
		exit(EXIT_FAILURE);
	}
	free(skill_db.db);

	native_t produce_db;
	status = load_native("/root/Desktop/git/Hercules/db/produce_db.txt",
	trim_numeric, load_native_general, &produce_db, &load_he_native[2]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules produce database.\n");
		exit(EXIT_FAILURE);
	}
	free(produce_db.db);

	native_t mercenary_db;
	status = load_native("/root/Desktop/git/Hercules/db/mercenary_db.txt",
	trim_numeric, load_native_general, &mercenary_db, &load_he_native[3]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules mercenary database.\n");
		exit(EXIT_FAILURE);
	}
	free(mercenary_db.db);

	native_t pet_db;
	status = load_native("/root/Desktop/git/Hercules/db/pet_db.txt",
	trim_numeric, load_native_general, &pet_db, &load_he_native[4]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules pet database.\n");
		exit(EXIT_FAILURE);
	}
	free(pet_db.db);

	native_t const_db;
	status = load_native("/root/Desktop/git/Hercules/db/const.txt",
	trim_general, load_native_general, &const_db, &load_he_native[5]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules const database.\n");
		exit(EXIT_FAILURE);
	}
	free(const_db.db);

	native_t combo_db;
	status = load_native("/root/Desktop/git/Hercules/db/re/item_combo_db.txt",
	trim_numeric, load_native_general, &combo_db, &load_he_native[6]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules item combo database.\n");
		exit(EXIT_FAILURE);
	}
	free(combo_db.db);

	return 0;
}

int default_hercules_database(void) {
	db_he_t db;
	db_he_aux_t db_search;
	create_hercules_database(&db, "/root/Desktop/dev/eAdb.Compiler3/hercules.db");
	item_he_sql_load(&db, "/root/Desktop/git/Hercules/db/re/item_db.conf");
	mob_he_sql_load(&db, "/root/Desktop/git/Hercules/db/re/mob_db.txt");
	skill_he_sql_load(&db, "/root/Desktop/git/Hercules/db/re/skill_db.txt");
	produce_he_sql_load(&db, "/root/Desktop/git/Hercules/db/produce_db.txt");
	mercenary_he_sql_load(&db, "/root/Desktop/git/Hercules/db/mercenary_db.txt");
	pet_he_sql_load(&db, "/root/Desktop/git/Hercules/db/pet_db.txt");
	const_he_sql_load(&db, "/root/Desktop/git/Hercules/db/const.txt");

	init_he_search(&db, &db_search);
	item_combo_he_sql_load(&db, "/root/Desktop/git/Hercules/db/re/item_combo_db.txt", &db_search);
	deit_he_search(&db_search);
	finalize_hercules_database(&db);
	return 0;
}

int create_hercules_database(db_he_t * db, const char * path) {
	int status = 0;
	const char * error = NULL;
	char * sql_error = NULL;	
	if( /* open database connection specified by path */
		sqlite3_open(path, &db->db) != SQLITE_OK ||
		/* execute hercules table creation script */
		sqlite3_exec(db->db, hercules_database_sql, NULL, NULL, &sql_error) != SQLITE_OK ||
		/* prepare the insertion statements */
		sqlite3_prepare_v2(db->db, item_he_insert, 	 	 strlen(item_he_insert), 		&db->item_he_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, mob_he_insert, 		 strlen(mob_he_insert), 		&db->mob_he_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, skill_he_insert, 	 strlen(skill_he_insert), 		&db->skill_he_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, produce_he_insert, 	 strlen(produce_he_insert), 	&db->produce_he_sql_insert, 	NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, mercenary_he_insert,  strlen(mercenary_he_insert), 	&db->mercenary_he_sql_insert, 	NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, pet_he_insert, 		 strlen(pet_he_insert), 		&db->pet_he_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, const_he_insert, 	 strlen(const_he_insert), 		&db->const_he_sql_insert, 		NULL) != SQLITE_OK ||
		sqlite3_prepare_v2(db->db, item_combo_he_insert, strlen(item_combo_he_insert), 	&db->item_combo_he_sql_insert, 	NULL) != SQLITE_OK) {

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

int finalize_hercules_database(db_he_t * db) {
	sqlite3_finalize(db->item_he_sql_insert);
	sqlite3_finalize(db->mob_he_sql_insert);
	sqlite3_finalize(db->skill_he_sql_insert);
	sqlite3_finalize(db->produce_he_sql_insert);
	sqlite3_finalize(db->mercenary_he_sql_insert);
	sqlite3_finalize(db->pet_he_sql_insert);
	sqlite3_finalize(db->const_he_sql_insert);
	sqlite3_finalize(db->item_combo_he_sql_insert);
	sqlite3_close(db->db);
	return CHECK_PASSED;
}

int item_he_sql_load(db_he_t * db, const char * path) {
	int i = 0;
	int status = 0;
	native_t item_db;
	item_he * item_he_db = NULL;
	memset(&item_db, 0, sizeof(native_t));

	load_he_item(path, &item_db);

	/* load the native database into the sqlite3 hercules database */
	for(i = 0, item_he_db = item_db.db; i < item_db.size; i++) {
		sqlite3_clear_bindings(db->item_he_sql_insert);
		sqlite3_bind_int(db->item_he_sql_insert, 	1, 	item_he_db[i].id);
		sqlite3_bind_text(db->item_he_sql_insert, 	2, 	item_he_db[i].aegis, 			strlen(item_he_db[i].aegis), SQLITE_STATIC);
		sqlite3_bind_text(db->item_he_sql_insert, 	3, 	item_he_db[i].name, 			strlen(item_he_db[i].name), SQLITE_STATIC);
		sqlite3_bind_int(db->item_he_sql_insert, 	4, 	item_he_db[i].type);
		sqlite3_bind_int(db->item_he_sql_insert, 	5, 	item_he_db[i].buy);
		sqlite3_bind_int(db->item_he_sql_insert, 	6, 	item_he_db[i].sell);
		sqlite3_bind_int(db->item_he_sql_insert, 	7, 	item_he_db[i].weight);
		sqlite3_bind_int(db->item_he_sql_insert, 	8, 	item_he_db[i].atk);
		sqlite3_bind_int(db->item_he_sql_insert, 	9, 	item_he_db[i].matk);
		sqlite3_bind_int(db->item_he_sql_insert, 	10, item_he_db[i].def);
		sqlite3_bind_int(db->item_he_sql_insert, 	11, item_he_db[i].range);
		sqlite3_bind_int(db->item_he_sql_insert, 	12, item_he_db[i].slots);
		sqlite3_bind_int(db->item_he_sql_insert, 	13, item_he_db[i].job);
		sqlite3_bind_int(db->item_he_sql_insert, 	14, item_he_db[i].upper);
		sqlite3_bind_int(db->item_he_sql_insert, 	15, item_he_db[i].gender);
		sqlite3_bind_int(db->item_he_sql_insert, 	16, item_he_db[i].loc);
		sqlite3_bind_int(db->item_he_sql_insert, 	17, item_he_db[i].weaponlv);
		sqlite3_bind_int(db->item_he_sql_insert, 	18, item_he_db[i].equiplv[EQUIP_MIN]);
		sqlite3_bind_int(db->item_he_sql_insert, 	19, item_he_db[i].equiplv[EQUIP_MAX]);
		sqlite3_bind_int(db->item_he_sql_insert, 	20, item_he_db[i].refine);
		sqlite3_bind_int(db->item_he_sql_insert, 	21, item_he_db[i].view);
		sqlite3_bind_int(db->item_he_sql_insert, 	22, item_he_db[i].bindonequip);
		sqlite3_bind_int(db->item_he_sql_insert, 	23, item_he_db[i].buyingstore);
		sqlite3_bind_int(db->item_he_sql_insert, 	24, item_he_db[i].delay);
		sqlite3_bind_int(db->item_he_sql_insert, 	25, item_he_db[i].trade[TRADE_OVERRIDE]);
		sqlite3_bind_int(db->item_he_sql_insert, 	26, item_he_db[i].trade[TRADE_NODROP]);
		sqlite3_bind_int(db->item_he_sql_insert, 	27, item_he_db[i].trade[TRADE_NOTRADE]);
		sqlite3_bind_int(db->item_he_sql_insert, 	28, item_he_db[i].trade[TRADE_PARTNEROVERRIDE]);
		sqlite3_bind_int(db->item_he_sql_insert, 	29, item_he_db[i].trade[TRADE_NOSELLTONPC]);
		sqlite3_bind_int(db->item_he_sql_insert, 	30, item_he_db[i].trade[TRADE_NOCART]);
		sqlite3_bind_int(db->item_he_sql_insert, 	31, item_he_db[i].trade[TRADE_NOSTORAGE]);
		sqlite3_bind_int(db->item_he_sql_insert, 	32, item_he_db[i].trade[TRADE_NOGSTORAGE]);
		sqlite3_bind_int(db->item_he_sql_insert, 	33, item_he_db[i].trade[TRADE_NOMAIL]);
		sqlite3_bind_int(db->item_he_sql_insert, 	34, item_he_db[i].trade[TRADE_NOAUCTION]);
		sqlite3_bind_int(db->item_he_sql_insert, 	35, item_he_db[i].nouse[NOUSE_OVERRIDE]);
		sqlite3_bind_int(db->item_he_sql_insert, 	36, item_he_db[i].nouse[NOUSE_SITTING]);
		sqlite3_bind_int(db->item_he_sql_insert, 	37, item_he_db[i].stack[STACK_AMOUNT]);
		sqlite3_bind_int(db->item_he_sql_insert, 	38, item_he_db[i].stack[STACK_TYPE]);
		sqlite3_bind_int(db->item_he_sql_insert, 	39, item_he_db[i].sprite);
		sqlite3_bind_text(db->item_he_sql_insert, 	40, item_he_db[i].script, 			strlen(item_he_db[i].script), SQLITE_STATIC);
		sqlite3_bind_text(db->item_he_sql_insert, 	41, item_he_db[i].onequipscript, 	strlen(item_he_db[i].onequipscript), SQLITE_STATIC);
		sqlite3_bind_text(db->item_he_sql_insert, 	42, item_he_db[i].onunequipscript, 	strlen(item_he_db[i].onunequipscript), SQLITE_STATIC);

		status = sqlite3_step(db->item_he_sql_insert);
		if(status != SQLITE_DONE) exit_abt_safe("fail to insert hercules item record");
		sqlite3_reset(db->item_he_sql_insert);
	}
	free(item_db.db);
	return CHECK_PASSED;
}

int mob_he_sql_load(db_he_t * db, const char * path) {
	int i = 0;
	native_t mob_db;
	mob_he * mob_he_db = NULL;
	memset(&mob_db, 0, sizeof(native_t));

	/* load the native database */
	if(load_native(path, trim_numeric, load_native_general, &mob_db, &load_he_native[0]) == CHECK_FAILED) {
		exit_func_safe("failed to load hercules mob database at %s; invalid path", path);
		return CHECK_FAILED;
	}

	/* check the native database */
	if(mob_db.db == NULL || mob_db.size <= 0) {
		exit_func_safe("failed to load hercules mob database at %s; detected zero entries", path);
		return CHECK_FAILED;
	}

	/* load the native database into the sqlite3 hercules database */
	sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
	for(i = 0, mob_he_db = mob_db.db; i < mob_db.size; i++) {
		sqlite3_clear_bindings(db->mob_he_sql_insert);
		sqlite3_bind_int(db->mob_he_sql_insert, 	1, mob_he_db[i].id);
		sqlite3_bind_text(db->mob_he_sql_insert, 	2, mob_he_db[i].sprite, 	strlen(mob_he_db[i].sprite), SQLITE_STATIC);
		sqlite3_bind_text(db->mob_he_sql_insert, 	3, mob_he_db[i].kro, 		strlen(mob_he_db[i].kro), SQLITE_STATIC);
		sqlite3_bind_text(db->mob_he_sql_insert, 	4, mob_he_db[i].iro, 		strlen(mob_he_db[i].iro), SQLITE_STATIC);
		sqlite3_bind_int(db->mob_he_sql_insert, 	5, mob_he_db[i].lv);
		sqlite3_bind_int(db->mob_he_sql_insert, 	6, mob_he_db[i].hp);
		sqlite3_bind_int(db->mob_he_sql_insert, 	7, mob_he_db[i].sp);
		sqlite3_bind_int(db->mob_he_sql_insert, 	8, mob_he_db[i].exp);
		sqlite3_bind_int(db->mob_he_sql_insert, 	9, mob_he_db[i].jexp);
		sqlite3_bind_int(db->mob_he_sql_insert, 	10, mob_he_db[i].range);
		sqlite3_bind_int(db->mob_he_sql_insert, 	11, mob_he_db[i].atk1);
		sqlite3_bind_int(db->mob_he_sql_insert, 	12, mob_he_db[i].atk2);
		sqlite3_bind_int(db->mob_he_sql_insert, 	13, mob_he_db[i].def);
		sqlite3_bind_int(db->mob_he_sql_insert, 	14, mob_he_db[i].mdef);
		sqlite3_bind_int(db->mob_he_sql_insert, 	15, mob_he_db[i].str);
		sqlite3_bind_int(db->mob_he_sql_insert, 	16, mob_he_db[i].agi);
		sqlite3_bind_int(db->mob_he_sql_insert, 	17, mob_he_db[i].vit);
		sqlite3_bind_int(db->mob_he_sql_insert, 	18, mob_he_db[i].intr);
		sqlite3_bind_int(db->mob_he_sql_insert, 	19, mob_he_db[i].dex);
		sqlite3_bind_int(db->mob_he_sql_insert, 	20, mob_he_db[i].luk);
		sqlite3_bind_int(db->mob_he_sql_insert, 	21, mob_he_db[i].range2);
		sqlite3_bind_int(db->mob_he_sql_insert, 	22, mob_he_db[i].range3);
		sqlite3_bind_int(db->mob_he_sql_insert, 	23, mob_he_db[i].scale);
		sqlite3_bind_int(db->mob_he_sql_insert, 	24, mob_he_db[i].race);
		sqlite3_bind_int(db->mob_he_sql_insert, 	25, mob_he_db[i].element);
		sqlite3_bind_int(db->mob_he_sql_insert, 	26, mob_he_db[i].mode);
		sqlite3_bind_int(db->mob_he_sql_insert, 	27, mob_he_db[i].speed);
		sqlite3_bind_int(db->mob_he_sql_insert, 	28, mob_he_db[i].adelay);
		sqlite3_bind_int(db->mob_he_sql_insert, 	29, mob_he_db[i].amotion);
		sqlite3_bind_int(db->mob_he_sql_insert, 	30, mob_he_db[i].dmotion);
		sqlite3_bind_int(db->mob_he_sql_insert, 	31, mob_he_db[i].mexp);
		sqlite3_bind_int(db->mob_he_sql_insert, 	32, mob_he_db[i].mvp1id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	33, mob_he_db[i].mvp1per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	34, mob_he_db[i].mvp2id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	35, mob_he_db[i].mvp2per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	36, mob_he_db[i].mvp3id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	37, mob_he_db[i].mvp3per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	38, mob_he_db[i].drop1id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	39, mob_he_db[i].drop1per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	40, mob_he_db[i].drop2id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	41, mob_he_db[i].drop2per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	42, mob_he_db[i].drop3id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	43, mob_he_db[i].drop3per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	44, mob_he_db[i].drop4id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	45, mob_he_db[i].drop4per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	46, mob_he_db[i].drop5id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	47, mob_he_db[i].drop5per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	48, mob_he_db[i].drop6id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	49, mob_he_db[i].drop6per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	50, mob_he_db[i].drop7id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	51, mob_he_db[i].drop7per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	52, mob_he_db[i].drop8id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	53, mob_he_db[i].drop8per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	54, mob_he_db[i].drop9id);
		sqlite3_bind_int(db->mob_he_sql_insert, 	55, mob_he_db[i].drop9per);
		sqlite3_bind_int(db->mob_he_sql_insert, 	56, mob_he_db[i].dropcardid);
		sqlite3_bind_int(db->mob_he_sql_insert, 	57, mob_he_db[i].dropcardper);
		sqlite3_step(db->mob_he_sql_insert);
		sqlite3_reset(db->mob_he_sql_insert);
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(mob_db.db);
	return CHECK_PASSED;
}

int skill_he_sql_load(db_he_t * db, const char * path) {
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
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(skill_db.db);
	return CHECK_PASSED;
}

int produce_he_sql_load(db_he_t * db, const char * path) {
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
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(produce_db.db);
	return CHECK_PASSED;
}

int mercenary_he_sql_load(db_he_t * db, const char * path) {
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
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(mercenary_db.db);
	return CHECK_PASSED;
}

int pet_he_sql_load(db_he_t * db, const char * path) {
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
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(pet_db.db);
	return CHECK_PASSED;
}

int const_he_sql_load(db_he_t * db, const char * path) {
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
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(const_db.db);
	return CHECK_PASSED;
}

int item_combo_he_sql_load(db_he_t * db, const char * path, db_he_aux_t * db_search) {
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
		convert_integer_list(combo_he_db[i].item_id.str, ":", &item_id_list);
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
			}
			/* free the list of integers */
			free(item_id_list.array);
		}
	}
	sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	free(combo_db.db);
	return CHECK_PASSED;
}

int init_he_search(db_he_t * db, db_he_aux_t * db_search) {
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
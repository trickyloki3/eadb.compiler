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
	{ const_he_load, sentinel_newline, delimit_cvs_whitespace, SKIP_NEXT_WS, 0, sizeof(const_he) }
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
			config_setting_lookup_string(item_row, "AegisName", &item[i].aegis);
			config_setting_lookup_string(item_row, "Name", &item[i].name);
			config_setting_lookup_int(item_row, "Type", &item[i].type);
			config_setting_lookup_int(item_row, "Buy", &item[i].buy);
			config_setting_lookup_int(item_row, "Sell", &item[i].sell);
			config_setting_lookup_int(item_row, "Weight", &item[i].weight);
			config_setting_lookup_int(item_row, "Atk", &item[i].atk);
			config_setting_lookup_int(item_row, "Matk", &item[i].matk);
			config_setting_lookup_int(item_row, "Def", &item[i].def);
			config_setting_lookup_int(item_row, "Range", &item[i].range);
			config_setting_lookup_int(item_row, "Slots", &item[i].slots);
			config_setting_lookup_int(item_row, "Job", &item[i].job);
			config_setting_lookup_int(item_row, "Upper", &item[i].upper);
			config_setting_lookup_int(item_row, "Gender", &item[i].gender);
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
			config_setting_lookup_bool(item_row, "Refine", &item[i].refine);
			config_setting_lookup_int(item_row, "View", &item[i].view);
			config_setting_lookup_bool(item_row, "BindOnEquip", &item[i].bindonequip);
			config_setting_lookup_bool(item_row, "BuyingStore", &item[i].buyingstore);
			config_setting_lookup_int(item_row, "Delay", &item[i].delay);
			item_sub = config_setting_get_member(item_row, "Trade");
			if(item_sub != NULL && config_setting_is_group(item_sub) == CONFIG_TRUE) {
				config_setting_lookup_int(item_row, "override", &item[i].trade[TRADE_OVERRIDE]);
				config_setting_lookup_bool(item_row, "nodrop", &item[i].trade[TRADE_NODROP]);
				config_setting_lookup_bool(item_row, "notrade", &item[i].trade[TRADE_NOTRADE]);
				config_setting_lookup_bool(item_row, "partneroverride", &item[i].trade[TRADE_PARTNEROVERRIDE]);
				config_setting_lookup_bool(item_row, "noselltonpc", &item[i].trade[TRADE_NOSELLTONPC]);
				config_setting_lookup_bool(item_row, "nocart", &item[i].trade[TRADE_NOCART]);
				config_setting_lookup_bool(item_row, "nostorage", &item[i].trade[TRADE_NOSTORAGE]);
				config_setting_lookup_bool(item_row, "nogstorage", &item[i].trade[TRADE_NOGSTORAGE]);
				config_setting_lookup_bool(item_row, "nomail", &item[i].trade[TRADE_NOMAIL]);
				config_setting_lookup_bool(item_row, "noauction", &item[i].trade[TRADE_NOAUCTION]);
			}
			item_sub = config_setting_get_member(item_row, "Nouse");
			if(item_sub != NULL && config_setting_is_group(item_sub) == CONFIG_TRUE) {
				config_setting_lookup_int(item_row, "override", &item[i].trade[NOUSE_OVERRIDE]);
				config_setting_lookup_bool(item_row, "sitting", &item[i].trade[NOUSE_SITTING]);
			}
			item_sub = config_setting_get_member(item_row, "Stack");
			if(item_sub != NULL && config_setting_is_list(item_sub) == CONFIG_TRUE) {
				item[i].equiplv[STACK_AMOUNT] = config_setting_get_int_elem(item_sub, STACK_AMOUNT);
				item[i].equiplv[STACK_TYPE] = config_setting_get_int_elem(item_sub, STACK_TYPE);
			}
			config_setting_lookup_string(item_row, "Script", &item[i].script);
			config_setting_lookup_string(item_row, "OnEquipScript", &item[i].onequipscript);
			config_setting_lookup_string(item_row, "OnUnequipScript", &item[i].onunequipscript);
			if(item[i].script == NULL) item[i].script = "";
			if(item[i].onequipscript == NULL) item[i].onequipscript = "";
			if(item[i].onunequipscript == NULL) item[i].onunequipscript = "";
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
	status = load_native("/root/Desktop/git/Hercules/db/re/skill_db_copy.txt",
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
	trim_numeric, load_native_general, &const_db, &load_he_native[5]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load hercules const database.\n");
		exit(EXIT_FAILURE);
	}
	free(const_db.db);

	return 0;
}

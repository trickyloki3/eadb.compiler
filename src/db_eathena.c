/*
 *   file: db_eathena.c
 *   date: 02/28/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "db_eathena.h"

native_config_t load_ea_native[EATHENA_DB_COUNT] = {
	{ item_ea_load, sentinel_newline, delimit_cvs, CHECK_BRACKET | SKIP_NEXT_WS | CHECK_FIELD_COUNT, ITEM_EA_FIELD_COUNT, sizeof(item_ea) },
	{ mob_ea_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, MOB_EA_FIELD_COUNT, sizeof(mob_ea) },
	{ skill_ea_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, SKILL_EA_FIELD_COUNT, sizeof(skill_ea) },
	{ produce_ea_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS, 0, sizeof(produce_ea) },
	{ mercenary_ea_load, sentinel_newline, delimit_cvs, SKIP_NEXT_WS | CHECK_FIELD_COUNT, MERCENARY_EA_FIELD_COUNT, sizeof(mercenary_ea) },
	{ pet_ea_load, sentinel_newline, delimit_cvs, CHECK_BRACKET | SKIP_NEXT_WS | CHECK_FIELD_COUNT, PET_EA_FIELD_COUNT, sizeof(pet_ea) },
	{ item_group_ea_load, sentinel_whitespace, delimit_cvs_whitespace, SKIP_NEXT_WS | CHECK_FIELD_COUNT, ITEM_GROUP_EA_FIELD_COUNT, sizeof(item_group_ea) },
	{ const_ea_load, sentinel_newline, delimit_cvs_whitespace, SKIP_NEXT_WS, 0, sizeof(const_ea) }
};

int item_ea_load(void * db, int row, int col, char * val) {
	item_ea * record = &((item_ea *) db)[row];
	switch(col) {
		case 0:   record->id = convert_integer(val, 10); 				break;
		case 1:   strnload(record->aegis, MAX_NAME_SIZE, val); 			break;
		case 2:   strnload(record->eathena, MAX_NAME_SIZE, val); 		break;
		case 3:   record->type = convert_integer(val, 10); 				break;
		case 4:   record->buy = convert_integer(val, 10); 				break;
		case 5:   record->sell = convert_integer(val, 10); 				break;
		case 6:   record->weight = convert_integer(val, 10); 			break;
		case 7:   record->atk = convert_integer(val, 10); 				break;
		case 8:   record->def = convert_integer(val, 10); 				break;
		case 9:   record->range = convert_integer(val, 10); 			break;
		case 10:  record->slots = convert_integer(val, 10); 			break;
		case 11:  record->job = convert_uinteger(val, 16); 				break;
		case 12:  record->upper = convert_integer(val, 10); 			break;
		case 13:  record->gender = convert_integer(val, 10); 			break;
		case 14:  record->loc = convert_integer(val, 10); 				break;
		case 15:  record->wlv = convert_integer(val, 10); 				break;
		case 16:  record->elv = convert_integer(val, 10); 				break;
		case 17:  record->refineable = convert_integer(val, 10); 		break;
		case 18:  record->view = convert_integer(val, 10); 				break;
		case 19:  strnload(record->script, MAX_SCRIPT_SIZE, val); 		break;
		case 20:  strnload(record->onequip, MAX_SCRIPT_SIZE, val); 		break;
		case 21:  strnload(record->onunequip, MAX_SCRIPT_SIZE, val); 	break;
		default:  exit_func_safe("invalid column field %d in eathena item database.\n", col);
	}
	return 0;
}

int mob_ea_load(void * db, int row, int col, char * val) {
	mob_ea * record = &((mob_ea *) db)[row];
	switch(col) {
		case 0: record->id = convert_integer(val,10);              	break;
		case 1: strnload(record->sprite, MAX_NAME_SIZE, val);		break;
		case 2: strnload(record->kro, MAX_NAME_SIZE, val);			break;
		case 3: strnload(record->iro, MAX_NAME_SIZE, val);			break;
		case 4: record->lv = convert_integer(val,10);              	break;
		case 5: record->hp = convert_integer(val,10);              	break;
		case 6: record->sp = convert_integer(val,10);              	break;
		case 7: record->exp = convert_integer(val,10);             	break;
		case 8: record->jexp = convert_integer(val,10);            	break;
		case 9: record->range = convert_integer(val,10);           	break;
		case 10: record->atk1 = convert_integer(val,10);           	break;
		case 11: record->atk2 = convert_integer(val,10);           	break;
		case 12: record->def = convert_integer(val,10);            	break;
		case 13: record->mdef = convert_integer(val,10);           	break;
		case 14: record->str = convert_integer(val,10);            	break;
		case 15: record->agi = convert_integer(val,10);            	break;
		case 16: record->vit = convert_integer(val,10);            	break;
		case 17: record->intr = convert_integer(val,10);           	break;
		case 18: record->dex = convert_integer(val,10);            	break;
		case 19: record->luk = convert_integer(val,10);            	break;
		case 20: record->range2 = convert_integer(val,10);         	break;
		case 21: record->range3 = convert_integer(val,10);         	break;
		case 22: record->scale = convert_integer(val,10);          	break;
		case 23: record->race = convert_integer(val,10);           	break;
		case 24: record->element = convert_integer(val,10);        	break;
		case 25: record->mode = convert_uinteger(val,16);          	break;
		case 26: record->speed = convert_integer(val,10);          	break;
		case 27: record->adelay = convert_integer(val,10);         	break;
		case 28: record->amotion = convert_integer(val,10);        	break;
		case 29: record->dmotion = convert_integer(val,10);        	break;
		case 30: record->mexp = convert_integer(val,10);           	break;
		case 31: record->expper = convert_integer(val,10);         	break;
		case 32: record->mvp1id = convert_integer(val,10);         	break;
		case 33: record->mvp1per = convert_integer(val,10);        	break;
		case 34: record->mvp2id = convert_integer(val,10);         	break;
		case 35: record->mvp2per = convert_integer(val,10);        	break;
		case 36: record->mvp3id = convert_integer(val,10);         	break;
		case 37: record->mvp3per = convert_integer(val,10);        	break;
		case 38: record->drop1id = convert_integer(val,10);        	break;
		case 39: record->drop1per = convert_integer(val,10);       	break;
		case 40: record->drop2id = convert_integer(val,10);        	break;
		case 41: record->drop2per = convert_integer(val,10);       	break;
		case 42: record->drop3id = convert_integer(val,10);        	break;
		case 43: record->drop3per = convert_integer(val,10);       	break;
		case 44: record->drop4id = convert_integer(val,10);        	break;
		case 45: record->drop4per = convert_integer(val,10);       	break;
		case 46: record->drop5id = convert_integer(val,10);        	break;
		case 47: record->drop5per = convert_integer(val,10);       	break;
		case 48: record->drop6id = convert_integer(val,10);        	break;
		case 49: record->drop6per = convert_integer(val,10);       	break;
		case 50: record->drop7id = convert_integer(val,10);        	break;
		case 51: record->drop7per = convert_integer(val,10);       	break;
		case 52: record->drop8id = convert_integer(val,10);        	break;
		case 53: record->drop8per = convert_integer(val,10);       	break;
		case 54: record->drop9id = convert_integer(val,10);        	break;
		case 55: record->drop9per = convert_integer(val,10);       	break;
		case 56: record->dropcardid = convert_integer(val,10);     	break;
		case 57: record->dropcardper = convert_integer(val,10);    	break;
		default: exit_func_safe("invalid column field %d in eathena mob database.\n", col);
	}
	return 0;
}

int skill_ea_load(void * db, int row, int col, char * val) {
	skill_ea * record = &((skill_ea *) db)[row];
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
		default: exit_func_safe("invalid column field %d in eathena skill database.\n", col);
	}
	return 0;
}

int produce_ea_load(void * db, int row, int col, char * val) {
	/* non-reentrant produce loading function */
	static int material_cnt = 0;
	static int alternate = 0;
	produce_ea * record = &((produce_ea *) db)[row];
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

int mercenary_ea_load(void * db, int row, int col, char * val) {
	mercenary_ea * record = &((mercenary_ea *) db)[row];
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
		default: exit_func_safe("invalid column field %d in eathena mercenary database.\n", col);
	}
	return 0;
}

int pet_ea_load(void * db, int row, int col, char * val) {
	pet_ea * record = &((pet_ea *) db)[row];
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
		default: exit_func_safe("invalid column field %d in eathena pet database.\n", col);
	}
	return 0;
}

int item_group_ea_load(void * db, int row, int col, char * val) {
	item_group_ea * record = &((item_group_ea *) db)[row];
	switch(col) {
		case 0: record->group_id = convert_integer(val, 10); break;
		case 1: record->item_id = convert_integer(val, 10); break;
		case 2: record->rate = convert_integer(val, 10); break;
		default: exit_func_safe("invalid column field %d in eathena item group database.\n", col);
	}
	return 0;
}

int const_ea_load(void * db, int row, int col, char * val) {
	const_ea * record = &((const_ea *) db)[row];
	switch(col) {
		case 0: strnload(record->name, MAX_NAME_SIZE, val); 							break;
		case 1:  
			/* constant can be represented as hexadecimal or decimal */
			record->value = (strlen(val) > 2 && val[0] == '0' && val[1] == 'x') ?
				convert_integer(val, 16):
				convert_integer(val, 10);												break;
		case 2: record->type = convert_integer(val, 10); 								break;
		default: exit_func_safe("invalid column field %d in eathena const database.\n", col);
	}
	return 0;
}

int load_eathena_database(const char * eathena_path) {
	int status = 0;

	native_t item_db;
	status = load_native("/root/Desktop/git/eathena/db/item_db.txt",
	trim_numeric, load_native_general, &item_db, &load_ea_native[0]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena item database.\n");
		exit(EXIT_FAILURE);
	}
	free(item_db.db);

	native_t mob_db;
	status = load_native("/root/Desktop/git/eathena/db/mob_db.txt",
	trim_numeric, load_native_general, &mob_db, &load_ea_native[1]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena mob database.\n");
		exit(EXIT_FAILURE);
	}
	free(mob_db.db);

	native_t skill_db;
	status = load_native("/root/Desktop/git/eathena/db/skill_db.txt",
	trim_numeric, load_native_general, &skill_db, &load_ea_native[2]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena skill database.\n");
		exit(EXIT_FAILURE);
	}
	free(skill_db.db);

	native_t produce_db;
	status = load_native("/root/Desktop/git/eathena/db/produce_db.txt",
	trim_numeric, load_native_general, &produce_db, &load_ea_native[3]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena produce database.\n");
		exit(EXIT_FAILURE);
	}
	free(produce_db.db);

	native_t mercenary_db;
	status = load_native("/root/Desktop/git/eathena/db/mercenary_db.txt",
	trim_numeric, load_native_general, &mercenary_db, &load_ea_native[4]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena mercenary database.\n");
		exit(EXIT_FAILURE);
	}
	free(mercenary_db.db);

	native_t pet_db;
	status = load_native("/root/Desktop/git/eathena/db/pet_db.txt",
	trim_numeric, load_native_general, &pet_db, &load_ea_native[5]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena pet database.\n");
		exit(EXIT_FAILURE);
	}
	free(pet_db.db);

	native_t item_group;
	status = load_native("/root/Desktop/git/eathena/db/item_violetbox.txt",
	trim_numeric, load_native_general, &item_group, &load_ea_native[6]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena item group database.\n");
		exit(EXIT_FAILURE);
	}
	free(item_group.db);

	native_t const_db;
	status = load_native("/root/Desktop/git/eathena/db/const.txt",
	trim_numeric, load_native_general, &const_db, &load_ea_native[7]);
	if(status == CHECK_FAILED) {
		fprintf(stderr,"failed to load eathena const database.\n");
		exit(EXIT_FAILURE);
	}
	free(const_db.db);
}

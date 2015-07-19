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

int item_chain_he_load(const char * path, native_t * native) {
	int i = 0;
	int len = 0;
	config_t * config = NULL;
	config_setting_t * root = NULL;
	item_chain_he * item_chains = NULL;

	config = calloc(1, sizeof(config_t));
	if (NULL == config)
		return CHECK_FAILED;

	config_init(config);

	if (CONFIG_TRUE != config_read_file(config, path)) {
		fprintf(stderr, "[load]: failed to load %s; %s.\n", path, config_error_text(config));
		goto failed;
	}

	root = config_root_setting(config);
	if (NULL == root)
		goto failed;

	len = config_setting_length(root);
	if (0 >= len) {
		fprintf(stderr, "[load]: \"item_chain\" is empty.\n");
		goto failed;
	}

	item_chains = calloc(len, sizeof(item_chain_he));
	if (NULL == item_chains)
		goto failed;

	for (i = 0; i < len; i++)
		if (item_chain_he_load_record(config_setting_get_elem(root, i), &item_chains[i]))
			goto failed;

	native->db = item_chains;
	native->size = len;
	config_destroy(config);
	SAFE_FREE(config);
	return CHECK_PASSED;

failed:
	SAFE_FREE(item_chains);
	config_destroy(config);
	SAFE_FREE(config);
	return CHECK_FAILED;
}

int item_chain_he_load_record(config_setting_t * record, item_chain_he * item_chain) {
	int i = 0;
	int len = 0;
	char * name = NULL;

	name = config_setting_name(record);
	if (NULL == name)
		return CHECK_FAILED;

	item_chain->name = convert_string(name);
	if (NULL == item_chain->name)
		return CHECK_FAILED;

	len = config_setting_length(record);
	if (0 >= len)
		goto failed;

	item_chain->items = calloc(len, sizeof(item_chain_record_he));
	if (NULL == item_chain->items)
		goto failed;

	for (i = 0; i < len; i++)
		if (item_chain_he_load_record_item(config_setting_get_elem(record, i), &item_chain->items[i]))
			goto failed;

	item_chain->size = len;
	return CHECK_PASSED;

failed:
	SAFE_FREE(item_chain->items);
	SAFE_FREE(item_chain->name);
	return CHECK_FAILED;
}

int item_chain_he_load_record_item(config_setting_t * record, item_chain_record_he * item) {
	char * name = NULL;

	name = config_setting_name(record);
	if (NULL == name)
		return CHECK_FAILED;

	item->name = convert_string(name);
	if (NULL == item->name)
		return CHECK_FAILED;

	item->chance = config_setting_get_int(record);

	return CHECK_PASSED;
}

int item_chain_he_free(item_chain_he * item_chains, int size) {
	int i = 0;
	int j = 0;
	if (NULL == item_chains || 0 >= size)
		return CHECK_FAILED;
	for (i = 0; i < size; i++) {
		for (j = 0; j < item_chains[i].size; j++)
			SAFE_FREE(item_chains[i].items[j].name);
		SAFE_FREE(item_chains[i].items);
		SAFE_FREE(item_chains[i].name);
	}
	SAFE_FREE(item_chains);
	return CHECK_PASSED;
}

int item_group_he_load(const char * path, native_t * native) {
	int i = 0;
	int len = 0;
	config_t * config = NULL;
	config_setting_t * root = NULL;
	item_group_he * item_groups = NULL;

	config = calloc(1, sizeof(config_t));
	if(NULL == config)
		return CHECK_FAILED;

	config_init(config);

	if(CONFIG_TRUE != config_read_file(config, path)) {
		fprintf(stderr, "[load]: failed to load %s; %s.\n", path, config_error_text(config));
		goto failed;
	}

	/* get the root group */
	root = config_root_setting(config);
	if (NULL == root)
		goto failed;

	len = config_setting_length(root);
	if(0 >= len) {
		fprintf(stderr, "[load]: \"item_group\" is empty.\n");
		goto failed;
	}

	item_groups = calloc(len, sizeof(item_group_he));
	if(item_groups == NULL)
		goto failed;

	/* load each group */
	for(i = 0; i < len; i++)
		if(item_group_he_load_record(config_setting_get_elem(root, i), &item_groups[i]))
			goto failed;

	native->db = item_groups;
	native->size = len;
	config_destroy(config);
	SAFE_FREE(config);
	return CHECK_PASSED;

failed:
	config_destroy(config);
	SAFE_FREE(config);
	return CHECK_FAILED;
}

int item_group_he_load_record(config_setting_t * record, item_group_he * item_group) {
	int i = 0;
	int len = 0;
	char * item_group_name = NULL;

	/* fail if item group name is null */
	item_group_name = config_setting_name(record);
	item_group->name = convert_string(item_group_name);
	if(NULL == item_group->name)
		return CHECK_FAILED;

	/* fail if item group list is empty */
	len = config_setting_length(record);
	item_group->size = len;
	if (0 >= len)
		goto failed;

	/* fail if out of memory */
	item_group->records = calloc(len, sizeof(item_group_record_he));
	if(NULL == item_group->records)
		goto failed;

	/* fail fail fail! */
	for (i = 0; i < len; i++)
		if (item_group_he_load_record_item(config_setting_get_elem(record, i), &item_group->records[i]))
			goto failed;

	return CHECK_PASSED;

failed:
	SAFE_FREE(item_group->records);
	SAFE_FREE(item_group->name);
	return CHECK_FAILED;
}

int item_group_he_load_record_item(config_setting_t * record, item_group_record_he * item) {
	int list_size = 0;
	const char * item_name = NULL;
	int item_rcount = 0;

	if(exit_null_safe(2, record, item))
		return CHECK_FAILED;

	switch(config_setting_type(record)) {
		case CONFIG_TYPE_STRING:
			/* extract item name */
			item_name = config_setting_get_string(record);
			item_rcount = 0;
			break;
		case CONFIG_TYPE_LIST:
			/* check list size */
			if(2 != config_setting_length(record))
				goto failed;
			/* extract item name and count */
			item_name = config_setting_get_string_elem(record, 0);
			item_rcount = config_setting_get_int_elem(record, 1);
			break;
		default:
			fprintf(stderr, "%d\n", config_setting_type(record));
			goto failed;
	}

	/* check item name */
	if(NULL == item_name)
		goto failed;

	/* load the item */
	item->name = convert_string(item_name);
	item->rcount = item_rcount;
	return CHECK_PASSED;

failed:
	return CHECK_FAILED;
}

int item_group_he_free(item_group_he * item_groups, int size) {
	int i = 0;
	int j = 0;
	item_group_he * item_group = NULL;
	if (NULL == item_groups || 0 >= size)
		return CHECK_FAILED;
	for (i = 0; i < size; i++) {
		item_group = &item_groups[i];
		for (j = 0; j < item_group->size; j++)
			SAFE_FREE(item_group->records[j].name);
		SAFE_FREE(item_group->records);
		SAFE_FREE(item_group->name);
	}
	SAFE_FREE(item_groups);
	return CHECK_PASSED;
}

int item_he_load(const char * path, native_t * native) {
	int i = 0;
	int size = 0;
	item_he * items = NULL;
	config_t * config = NULL;
	config_setting_t * db = NULL;

	config = calloc(1, sizeof(config_t));
	if(NULL == config)
		return CHECK_FAILED;

	config_init(config);

	/* read config file */
	if(CONFIG_TRUE != config_read_file(config, path)) {
		fprintf(stderr, "[load]: failed to load %s; %s.\n", path, config_error_text(config));
		goto failed;
	}

	/* search item db list */
	db = config_lookup(config, "item_db");
	if(NULL == db) {
		fprintf(stderr, "[load]: \"item_db\" is not founded.\n");
		goto failed;
	} else if(!config_setting_is_list(db)) {
		fprintf(stderr, "[load]: \"item_db\" is not a invalid item list.\n");
		goto failed;
	}

	/* check item db list is not empty */
	size = config_setting_length(db);
	if(0 >= size) {
		fprintf(stderr, "[load]: \"item_db\" is empty.\n");
		goto failed;
	}

	/* allocate item db space */
	items = calloc(size, sizeof(item_he));
	if(NULL == items)
		goto failed;

	/* load each item from item db */
	for(i = 0; i < size; i++)
		if(item_he_load_record(config_setting_get_elem(db, i), &items[i]))
			goto failed;

	native->db = items;
	native->size = size;
	config_destroy(config);
	SAFE_FREE(config);
	return CHECK_PASSED;

failed:
	SAFE_FREE(items);
	config_destroy(config);
	SAFE_FREE(config);
	return CHECK_FAILED;
}

int item_he_load_record(config_setting_t * row, item_he * item) {
	char * aegis = NULL;
	char * name = NULL;
	char * script = NULL;
	char * equip_script = NULL;
	char * unequip_script = NULL;
	config_setting_t * equip_lv = NULL;
	config_setting_t * trade = NULL;
	config_setting_t * nouse = NULL;
	config_setting_t * stack = NULL;

	config_setting_lookup_int(row, "Id", &item->id);
	config_setting_lookup_string(row, "AegisName", &aegis);
	config_setting_lookup_string(row, "Name", &name);

	strnload(item->aegis, MAX_NAME_SIZE, aegis);
	strnload(item->name, MAX_NAME_SIZE, name);

	config_setting_lookup_int(row, "Type", &item->type);
	config_setting_lookup_int(row, "Buy", &item->buy);
	config_setting_lookup_int(row, "Sell", &item->sell);
	config_setting_lookup_int(row, "Weight", &item->weight);
	config_setting_lookup_int(row, "Atk", &item->atk);
	config_setting_lookup_int(row, "Matk", &item->matk);
	config_setting_lookup_int(row, "Def", &item->def);
	config_setting_lookup_int(row, "Range", &item->range);
	config_setting_lookup_int(row, "Slots", &item->slots);

	/* default buy / sell */
	if (item->buy <= 0 && item->sell > 0)
		item->buy = item->sell * 2;
	else if (item->buy > 0 && item->sell <= 0)
		item->sell = item->buy / 2;

	/* default all jobs */
	if(CONFIG_FALSE == config_setting_lookup_int(row, "Job", &item->job))
		item->job = 0xFFFFFFFF;

	/* default all tier */
	if(CONFIG_FALSE == config_setting_lookup_int(row, "Upper", &item->upper))
		item->upper = 0x3F;

	/* default all geneder */
	if(CONFIG_FALSE == config_setting_lookup_int(row, "Gender", &item->gender))
		item->gender = 0x2;

	config_setting_lookup_int(row, "Loc", &item->loc);
	config_setting_lookup_int(row, "WeaponLv", &item->weaponlv);

	/* equiplv is either list or value */
	equip_lv = config_setting_get_member(row, "EquipLv");
	if(NULL != equip_lv && CONFIG_TRUE == config_setting_is_list(equip_lv)) {
		item->equiplv[EQUIP_MIN] = config_setting_get_int_elem(equip_lv, EQUIP_MIN);
		item->equiplv[EQUIP_MAX] = config_setting_get_int_elem(equip_lv, EQUIP_MAX);
	} else {
		config_setting_lookup_int(row, "EquipLv", &item->equiplv[EQUIP_MIN]);
		item->equiplv[EQUIP_MAX] = item->equiplv[EQUIP_MIN];
	}

	/* default is refinable */
	if(CONFIG_FALSE == config_setting_lookup_bool(row, "Refine", &item->refine))
		item->refine = 1;

	config_setting_lookup_int(row, "View", &item->view);
	config_setting_lookup_bool(row, "BindOnEquip", &item->bindonequip);
	config_setting_lookup_bool(row, "BuyingStore", &item->buyingstore);
	config_setting_lookup_int(row, "Delay", &item->delay);
	config_setting_lookup_bool(row, "KeepAfterUse", &item->keepafteruse);

	trade = config_setting_get_member(row, "Trade");
	if(NULL == trade) {
		/* default all false */
		memset(item->trade, 0, sizeof(int) * TRADE_TOTAL);
	} else {
		/* default every group */
		if(CONFIG_FALSE == config_setting_lookup_int(trade, "override", &item->trade[TRADE_OVERRIDE]))
			item->trade[TRADE_OVERRIDE] = 100;
		config_setting_lookup_bool(trade, "nodrop", &item->trade[TRADE_NODROP]);
		config_setting_lookup_bool(trade, "notrade", &item->trade[TRADE_NOTRADE]);
		config_setting_lookup_bool(trade, "partneroverride", &item->trade[TRADE_PARTNEROVERRIDE]);
		config_setting_lookup_bool(trade, "noselltonpc", &item->trade[TRADE_NOSELLTONPC]);
		config_setting_lookup_bool(trade, "nocart", &item->trade[TRADE_NOCART]);
		config_setting_lookup_bool(trade, "nostorage", &item->trade[TRADE_NOSTORAGE]);
		config_setting_lookup_bool(trade, "nogstorage", &item->trade[TRADE_NOGSTORAGE]);
		config_setting_lookup_bool(trade, "nomail", &item->trade[TRADE_NOMAIL]);
		config_setting_lookup_bool(trade, "noauction", &item->trade[TRADE_NOAUCTION]);
	}

	nouse = config_setting_get_member(row, "Nouse");
	if(NULL != nouse) {
		/* every group */
		if(CONFIG_FALSE == config_setting_lookup_int(row, "override", &item->nouse[NOUSE_OVERRIDE]))
			item->nouse[NOUSE_OVERRIDE] = 100;
		config_setting_lookup_bool(row, "sitting", &item->nouse[NOUSE_SITTING]);
	}

	stack = config_setting_get_member(row, "Stack");
	if(NULL != stack) {
		item->stack[STACK_AMOUNT] = config_setting_get_int_elem(stack, STACK_AMOUNT);
		item->stack[STACK_TYPE] = config_setting_get_int_elem(stack, STACK_TYPE);
	}

	config_setting_lookup_int(row, "Sprite", &item->sprite);

	(CONFIG_FALSE == config_setting_lookup_string(row, "Script", &script))?
		strnload(item->script, MAX_SCRIPT_SIZE, ""):
		strnload(item->script, MAX_SCRIPT_SIZE, script);

	(CONFIG_FALSE == config_setting_lookup_string(row, "OnEquipScript", &equip_script))?
		strnload(item->onequipscript, MAX_SCRIPT_SIZE, ""):
		strnload(item->onequipscript, MAX_SCRIPT_SIZE, equip_script);

	(CONFIG_FALSE == config_setting_lookup_string(row, "OnUnequipScript", &unequip_script))?
		strnload(item->onunequipscript, MAX_SCRIPT_SIZE, ""):
		strnload(item->onunequipscript, MAX_SCRIPT_SIZE, unequip_script);

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
		HERC_COMBO_CREATE
		ITEM_GROUP_DELETE
		ITEM_GROUP_CREATE
		ITEM_GROUP_RECORD_DELETE
		ITEM_GROUP_RECORD_CREATE
		ITEM_CHAIN_DELETE
		ITEM_CHAIN_CREATE
		ITEM_CHAIN_RECORD_DELETE
		ITEM_CHAIN_RECORD_CREATE,
		NULL, NULL, &error)) {
		if(NULL != error) {
			fprintf(stderr, "Failed to load schema %s; %s.\n", path, error);
			sqlite3_free(error);
		}
		goto failed;
	}

	/* compile sql statements */
	if( SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_ITEM_INSERT, strlen(HERC_ITEM_INSERT), &herc->item_he_sql_insert, NULL)			||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_MOB_INSERT, strlen(HERC_MOB_INSERT), &herc->mob_he_sql_insert, NULL) 			||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_SKILL_INSERT, strlen(HERC_SKILL_INSERT), &herc->skill_he_sql_insert, NULL)		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_PRODUCE_INSERT, strlen(HERC_PRODUCE_INSERT), &herc->produce_he_sql_insert, NULL) ||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_MERC_INSERT, strlen(HERC_MERC_INSERT), &herc->mercenary_he_sql_insert, NULL) 	||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_PET_INSERT, strlen(HERC_PET_INSERT), &herc->pet_he_sql_insert, NULL) 			||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_CONST_INSERT, strlen(HERC_CONST_INSERT), &herc->const_he_sql_insert, NULL) 		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_COMBO_INSERT, strlen(HERC_COMBO_INSERT), &herc->item_combo_he_sql_insert, NULL)	||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, HERC_ITEM_SEARCH, strlen(HERC_ITEM_SEARCH), &herc->item_he_sql_search, NULL)			||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, ITEM_GROUP_INSERT, strlen(ITEM_GROUP_INSERT), &herc->item_group_he_insert, NULL)		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, ITEM_GROUP_RECORD_INSERT, strlen(ITEM_GROUP_RECORD_INSERT), &herc->item_group_record_he_insert, NULL) ||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, ITEM_CHAIN_INSERT, strlen(ITEM_CHAIN_INSERT), &herc->item_chain_he_insert, NULL)		||
		SQLITE_OK != sqlite3_prepare_v2(herc->db, ITEM_CHAIN_RECORD_INSERT, strlen(ITEM_CHAIN_RECORD_INSERT), &herc->item_chain_record_he_insert, NULL)) {
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
		SQLITE_OK != sqlite3_finalize(herc->item_he_sql_search) ||
		SQLITE_OK != sqlite3_finalize(herc->item_group_he_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->item_group_record_he_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->item_chain_he_insert) ||
		SQLITE_OK != sqlite3_finalize(herc->item_chain_record_he_insert) ||
		SQLITE_OK != sqlite3_close(herc->db)) {
		fprintf(stderr, "Failed to unload database; %s.\n", sqlite3_errmsg(herc->db));
		exit(EXIT_FAILURE);
	}
	SAFE_FREE(herc);
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
	fprintf(stdout, "[load]: loaded item db; %s\n", path);
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
	fprintf(stdout, "[load]: loaded mob db; %s\n", path);
	SAFE_FREE(mobs.db);
	return CHECK_PASSED;
}

int herc_load_mob_db_insert(mob_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	mob_he * mob = NULL;

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

int herc_load_skill_db(herc_db_t * db, const char * path) {
	native_t skills;
	if( load_native(path, trim_numeric, load_native_general, &skills, &load_he_native[1]) ||
		NULL == skills.db ||
		0 >= skills.size) {
		fprintf(stderr, "Failed to load skill db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_skill_db_insert(skills.db, skills.size, db->skill_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load skill db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded skill db; %s\n", path);
	SAFE_FREE(skills.db);
	return CHECK_PASSED;
}

int herc_load_skill_db_insert(skill_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	skill_he * skill = NULL;

	for (i = 0; i < size; i++) {
		skill = &db[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_int(sql, 1, skill->id) ||
			SQLITE_OK != sqlite3_bind_text(sql, 2, skill->range.str, strlen(skill->range.str), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 3, skill->hit) ||
			SQLITE_OK != sqlite3_bind_int(sql, 4, skill->inf) ||
			SQLITE_OK != sqlite3_bind_text(sql, 5, skill->element.str, strlen(skill->element.str), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 6, skill->nk) ||
			SQLITE_OK != sqlite3_bind_text(sql, 7, skill->splash.str, strlen(skill->splash.str), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 8, skill->maxlv) ||
			SQLITE_OK != sqlite3_bind_text(sql, 9, skill->hit_amount.str, strlen(skill->hit_amount.str), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 10, skill->cast_cancel, strlen(skill->cast_cancel), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 11, skill->cast_def_reduce_rate) ||
			SQLITE_OK != sqlite3_bind_int(sql, 12, skill->inf2) ||
			SQLITE_OK != sqlite3_bind_text(sql, 13, skill->maxcount.str, strlen(skill->maxcount.str), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 14, skill->type, strlen(skill->type), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 15, skill->blow_count.str, strlen(skill->blow_count.str), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 16, skill->name, strlen(skill->name), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 17, skill->desc, strlen(skill->desc), SQLITE_STATIC) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %d to skill db.\n", skill->id);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			fprintf(stderr,"[load]: %d/%d ... %-100d\r", i, size, skill->id);
		}
	}

	return CHECK_PASSED;
}

int herc_load_produce_db(herc_db_t * db, const char * path) {
	native_t produces;
	if( load_native(path, trim_numeric, load_native_general, &produces, &load_he_native[2]) ||
		NULL == produces.db ||
		0 >= produces.size) {
		fprintf(stderr, "Failed to load produce db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_produce_db_insert(produces.db, produces.size, db->produce_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load produce db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded produce db; %s\n", path);
	SAFE_FREE(produces.db);
	return CHECK_PASSED;
}

int herc_load_produce_db_insert(produce_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	char buf[BUF_SIZE];
	produce_he * produce = NULL;

	for(i = 0; i < size; i++) {
		produce = &db[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_int(sql, 1, produce->item_id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 2, produce->item_lv) ||
			SQLITE_OK != sqlite3_bind_int(sql, 3, produce->skill_id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 4, produce->skill_lv) ||
			NULL == array_to_string(buf, produce->item_id_req) ||
			SQLITE_OK != sqlite3_bind_text(sql, 5, buf, strlen(buf), SQLITE_TRANSIENT) ||
			NULL == array_to_string_cnt(buf, produce->item_amount_req, array_field_cnt(buf) + 1) ||
			SQLITE_OK != sqlite3_bind_text(sql, 6, buf, strlen(buf), SQLITE_TRANSIENT) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %d to produce db.\n", produce->item_id);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			fprintf(stderr,"[load]: %d/%d ... %-100d\r", i, size, produce->item_id);
		}
	}

	return CHECK_PASSED;
}

int herc_load_merc_db(herc_db_t * db, const char * path) {
	native_t mercenaries;
	if( load_native(path, trim_numeric, load_native_general, &mercenaries, &load_he_native[3]) ||
		NULL == mercenaries.db ||
		0 >= mercenaries.size) {
		fprintf(stderr, "Failed to load mercenary db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_merc_db_insert(mercenaries.db, mercenaries.size, db->mercenary_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load mercenary db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded mercenary db; %s\n", path);
	SAFE_FREE(mercenaries.db);
	return CHECK_PASSED;
}

int herc_load_merc_db_insert(mercenary_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	mercenary_he * merc = NULL;

	for(i = 0; i < size; i++) {
		merc = &db[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_int(sql, 1, merc->id) ||
			SQLITE_OK != sqlite3_bind_text(sql, 2, merc->sprite, strlen(merc->sprite), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 3, merc->name, strlen(merc->name), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 4, merc->lv) ||
			SQLITE_OK != sqlite3_bind_int(sql, 5, merc->hp) ||
			SQLITE_OK != sqlite3_bind_int(sql, 6, merc->sp) ||
			SQLITE_OK != sqlite3_bind_int(sql, 7, merc->range1) ||
			SQLITE_OK != sqlite3_bind_int(sql, 8, merc->atk1) ||
			SQLITE_OK != sqlite3_bind_int(sql, 9, merc->atk2) ||
			SQLITE_OK != sqlite3_bind_int(sql, 10, merc->def) ||
			SQLITE_OK != sqlite3_bind_int(sql, 11, merc->mdef) ||
			SQLITE_OK != sqlite3_bind_int(sql, 12, merc->str) ||
			SQLITE_OK != sqlite3_bind_int(sql, 13, merc->agi) ||
			SQLITE_OK != sqlite3_bind_int(sql, 14, merc->vit) ||
			SQLITE_OK != sqlite3_bind_int(sql, 15, merc->intr) ||
			SQLITE_OK != sqlite3_bind_int(sql, 16, merc->dex) ||
			SQLITE_OK != sqlite3_bind_int(sql, 17, merc->luk) ||
			SQLITE_OK != sqlite3_bind_int(sql, 18, merc->range2) ||
			SQLITE_OK != sqlite3_bind_int(sql, 19, merc->range3) ||
			SQLITE_OK != sqlite3_bind_int(sql, 20, merc->scale) ||
			SQLITE_OK != sqlite3_bind_int(sql, 21, merc->race) ||
			SQLITE_OK != sqlite3_bind_int(sql, 22, merc->element) ||
			SQLITE_OK != sqlite3_bind_int(sql, 23, merc->speed) ||
			SQLITE_OK != sqlite3_bind_int(sql, 24, merc->adelay) ||
			SQLITE_OK != sqlite3_bind_int(sql, 25, merc->amotion) ||
			SQLITE_OK != sqlite3_bind_int(sql, 26, merc->dmotion) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %d to mercenary db.\n", merc->id);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			fprintf(stderr,"[load]: %d/%d ... %-100d\r", i, size, merc->id);
		}
	}

	return CHECK_PASSED;
}

int herc_load_pet_db(herc_db_t * db, const char * path) {
	native_t pets;
	if( load_native(path, trim_numeric, load_native_general, &pets, &load_he_native[4]) ||
		NULL == pets.db ||
		0 >= pets.size) {
		fprintf(stderr, "Failed to load pet db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_pet_db_insert(pets.db, pets.size, db->pet_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load pet db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded pet db; %s\n", path);
	SAFE_FREE(pets.db);
	return CHECK_PASSED;
}

int herc_load_pet_db_insert(pet_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	pet_he * pet = NULL;

	for(i = 0; i < size; i++) {
		pet = &db[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_int(sql, 1, pet->mob_id) ||
			SQLITE_OK != sqlite3_bind_text(sql, 2, pet->name, strlen(pet->name), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 3, pet->jname, strlen(pet->jname), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 4, pet->lure_id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 5, pet->egg_id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 6, pet->equip_id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 7, pet->food_id) ||
			SQLITE_OK != sqlite3_bind_int(sql, 8, pet->fullness) ||
			SQLITE_OK != sqlite3_bind_int(sql, 9, pet->hungry_delay) ||
			SQLITE_OK != sqlite3_bind_int(sql, 10, pet->r_hungry) ||
			SQLITE_OK != sqlite3_bind_int(sql, 11, pet->r_full) ||
			SQLITE_OK != sqlite3_bind_int(sql, 12, pet->intimate) ||
			SQLITE_OK != sqlite3_bind_int(sql, 13, pet->die) ||
			SQLITE_OK != sqlite3_bind_int(sql, 14, pet->speed) ||
			SQLITE_OK != sqlite3_bind_int(sql, 15, pet->capture) ||
			SQLITE_OK != sqlite3_bind_int(sql, 16, pet->s_performance) ||
			SQLITE_OK != sqlite3_bind_int(sql, 17, pet->talk_convert) ||
			SQLITE_OK != sqlite3_bind_int(sql, 18, pet->attack_rate) ||
			SQLITE_OK != sqlite3_bind_int(sql, 19, pet->defence_attack_rate) ||
			SQLITE_OK != sqlite3_bind_int(sql, 20, pet->change_target_rate) ||
			SQLITE_OK != sqlite3_bind_text(sql, 21, pet->pet_script, strlen(pet->pet_script), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_text(sql, 22, pet->loyal_script, strlen(pet->loyal_script), SQLITE_STATIC) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %d to pet db.\n", pet->mob_id);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			fprintf(stderr,"[load]: %d/%d ... %-100d\r", i, size, pet->mob_id);
		}
	}

	return CHECK_PASSED;
}

int herc_load_const_db(herc_db_t * db, const char * path) {
	native_t consts;
	if( load_native(path, trim_general, load_native_general, &consts, &load_he_native[5]) ||
		NULL == consts.db ||
		0 >= consts.size) {
		fprintf(stderr, "Failed to load constants db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_const_db_insert(consts.db, consts.size, db->const_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load constants db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded constant db; %s\n", path);
	SAFE_FREE(consts.db);
	return CHECK_PASSED;
}

int herc_load_const_db_insert(const_he * db, int size, sqlite3_stmt * sql) {
	int i = 0;
	const_he * constant = NULL;

	for(i = 0; i < size; i++) {
		constant = &db[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_text(sql, 1, constant->name, strlen(constant->name), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 2, constant->value) ||
			SQLITE_OK != sqlite3_bind_int(sql, 3, constant->type) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %s to constant db.\n", constant->name);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			fprintf(stderr,"[load]: %d/%d ... %-100s\r", i, size, constant->name);
		}
	}

	return CHECK_PASSED;
}

int herc_search_item(herc_db_t * db, int item_id, item_he * item) {
	sqlite3_stmt * sql = db->item_he_sql_search;

	if(	SQLITE_OK != sqlite3_clear_bindings(sql) ||
		SQLITE_OK != sqlite3_bind_int(sql, 1, item_id) ||
		SQLITE_ROW != sqlite3_step(sql)) {
		sqlite3_reset(sql);
		return CHECK_FAILED;
	}

	item->id = sqlite3_column_int(sql, 0);
	strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(sql, 1));
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(sql, 2));

	if(SQLITE_OK != sqlite3_reset(sql))
		return CHECK_FAILED;

	return CHECK_PASSED;
}

int herc_load_combo_db(herc_db_t * db, const char * path) {
	native_t combos;
	if( load_native(path, trim_numeric, load_native_general, &combos, &load_he_native[6]) ||
		NULL == combos.db ||
		0 >= combos.size) {
		fprintf(stderr, "Failed to load item combo db; %s.\n", path);
		return CHECK_FAILED;
	}
	if( herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_combo_db_insert(db, combos.db, combos.size, db->item_combo_he_sql_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load item combo db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded item combo db; %s\n", path);
	SAFE_FREE(combos.db);
	return CHECK_PASSED;
}

int herc_load_combo_db_insert(herc_db_t * db, combo_he * combos, int size, sqlite3_stmt * sql) {
	int i = 0;
	int j = 0;
	int offset = 0;
	char buffer[BUF_SIZE];
	combo_he * combo = NULL;
	/* item id list */
	array_w item_id_list;
	int * item_id_array = NULL;
	int item_id_size = 0;
	item_he item;

	for(i = 0; i < size; i++) {
		combo = &combos[i];

		/* reset buffer */
		offset = 0;

		/* get list of item id */
		memset(&item_id_list, 0, sizeof(array_w));
		convert_integer_delimit_array(combo->item_id.str, ":", &item_id_list);
		item_id_array = item_id_list.array;
		item_id_size = item_id_list.size;
		if(item_id_array != NULL && item_id_size > 0) {
			/* search for each item id */
			memset(&item, 0, sizeof(item_he));
			for(j = 0; j < item_id_size; j++) {
				if(CHECK_PASSED == herc_search_item(db, item_id_array[j], &item)) {
					offset += (offset == 0) ?
						sprintf(&buffer[offset], "[%s", item.name):
						sprintf(&buffer[offset], ", %s", item.name);
				}
			}
			offset += sprintf(&buffer[offset], " Combo]");

			for(j = 0; j < item_id_size; j++) {
				if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
					SQLITE_OK != sqlite3_bind_int(sql, 1, item_id_array[j]) ||
					SQLITE_OK != sqlite3_bind_text(sql, 2, combo->script, strlen(combo->script), SQLITE_STATIC) ||
					SQLITE_OK != sqlite3_bind_text(sql, 3, buffer, offset, SQLITE_STATIC) ||
					SQLITE_DONE != sqlite3_step(sql) ||
					SQLITE_OK != sqlite3_reset(sql)) {
					fprintf(stderr, "Failed to load item combo %s.\n", buffer);
					if (SQLITE_OK != sqlite3_reset(sql))
						return CHECK_FAILED;
				}
				fprintf(stderr,"[load]: %d/%d\r", i, size);
			}

			SAFE_FREE(item_id_array);
		}
	}

	return CHECK_PASSED;
}

int herc_load_item_group_db(herc_db_t * db, const char * path) {
	native_t item_groups;
	item_groups.db = NULL;
	item_groups.size = 0;

	if (item_group_he_load(path, &item_groups) ||
		NULL == item_groups.db ||
		0 >= item_groups.size) {
		fprintf(stderr, "Failed to load item group db; %s.\n", path);
		goto failed;
	}
	if (herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_item_group_db_insert(db, item_groups.db, item_groups.size, db->item_group_he_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load item group db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded item group db; %s\n", path);

	item_group_he_free(item_groups.db, item_groups.size);
	return CHECK_PASSED;

failed:
	item_group_he_free(item_groups.db, item_groups.size);
	return CHECK_FAILED;
}

int herc_load_item_group_db_insert(herc_db_t * db, item_group_he * item_groups, int size, sqlite3_stmt * sql) {
	int i = 0;
	int id = 0;
	item_group_he * item_group = NULL;

	for (i = 0; i < size; i++) {
		item_group = &item_groups[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_text(sql, 1, item_group->name, strlen(item_group->name), SQLITE_STATIC) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %s to item group db.\n", item_group->name);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			id = (int) sqlite3_last_insert_rowid(db->db);
			if (herc_load_item_group_record_db_insert(item_group->records, item_group->size, db->item_group_record_he_insert, id))
				return CHECK_FAILED;

			fprintf(stderr, "[load]: %d/%d ... %-100s\r", i, size, item_group->name);
		}
	}

	return CHECK_PASSED;
}

int herc_load_item_group_record_db_insert(item_group_record_he * items, int size, sqlite3_stmt * sql, int group_id) {
	int i = 0;
	item_group_record_he * item = NULL;

	for (i = 0; i < size; i++) {
		item = &items[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_text(sql, 1, item->name, strlen(item->name), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 2, item->rcount) ||
			SQLITE_OK != sqlite3_bind_int(sql, 3, group_id) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %s to item group record db.\n", item->name);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		}
		else {
			fprintf(stderr, "[load]: %d/%d ... %-100s\r", i, size, item->name);
		}
	}

	return CHECK_PASSED;
}

int herc_load_item_chain_db(herc_db_t * db, const char * path) {
	native_t item_chains;
	item_chains.db = NULL;
	item_chains.size = 0;

	if (item_chain_he_load(path, &item_chains) ||
		NULL == item_chains.db ||
		0 >= item_chains.size) {
		fprintf(stderr, "Failed to load item chain db; %s.\n", path);
		goto failed;
	}
	if (herc_db_exec(db, "BEGIN IMMEDIATE TRANSACTION;") ||
		herc_load_item_chain_db_insert(db, item_chains.db, item_chains.size, db->item_chain_he_insert) ||
		herc_db_exec(db, "COMMIT TRANSACTION;")) {
		fprintf(stderr, "Failed to load item group db; %s.\n", sqlite3_errmsg(db->db));
		return CHECK_FAILED;
	}
	fprintf(stdout, "[load]: loaded item chain db; %s\n", path);

	item_chain_he_free(item_chains.db, item_chains.size);
	return CHECK_PASSED;

failed:
	item_chain_he_free(item_chains.db, item_chains.size);
	return CHECK_FAILED;
}

int herc_load_item_chain_db_insert(herc_db_t * db, item_chain_he * item_chains, int size, sqlite3_stmt * sql) {
	int i = 0;
	int id = 0;
	item_chain_he * item_chain = NULL;

	for (i = 0; i < size; i++) {
		item_chain = &item_chains[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_text(sql, 1, item_chain->name, strlen(item_chain->name), SQLITE_STATIC) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %s to item group db.\n", item_chain->name);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			id = (int)sqlite3_last_insert_rowid(db->db);
			if (herc_load_item_chain_record_db_insert(item_chains->items, item_chains->size, db->item_chain_record_he_insert, id))
				return CHECK_FAILED;

			fprintf(stderr, "[load]: %d/%d ... %-100s\r", i, size, item_chain->name);
		}
	}

	return CHECK_PASSED;
}

int herc_load_item_chain_record_db_insert(item_chain_record_he * items, int size, sqlite3_stmt * sql, int chain_id) {
	int i = 0;
	item_chain_record_he * item = NULL;

	for (i = 0; i < size; i++) {
		item = &items[i];
		if (SQLITE_OK != sqlite3_clear_bindings(sql) ||
			SQLITE_OK != sqlite3_bind_text(sql, 1, item->name, strlen(item->name), SQLITE_STATIC) ||
			SQLITE_OK != sqlite3_bind_int(sql, 2, item->chance) ||
			SQLITE_OK != sqlite3_bind_int(sql, 3, chain_id) ||
			SQLITE_DONE != sqlite3_step(sql) ||
			SQLITE_OK != sqlite3_reset(sql)) {
			fprintf(stderr, "[load]: failed to add %s to item chain record db.\n", item->name);
			if (SQLITE_OK != sqlite3_reset(sql))
				return CHECK_FAILED;
		} else {
			fprintf(stderr, "[load]: %d/%d ... %-100s\r", i, size, item->name);
		}
	}

	return CHECK_PASSED;
}

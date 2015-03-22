/*
 *   file: format.c
 *   date: 03/21/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "format.h"

int init_flavour_db(format_t * format, const char * flavour_db_path) {
	int status = 0;
	const char * error = NULL;

	/* check null paramaters */
	if(exit_null_safe(2, format, flavour_db_path))
		return CHECK_FAILED;

	/* open the flavour text database and compiled sql statements */
	if(sqlite3_open(flavour_db_path, &format->flavour_text) != SQLITE_OK ||
	   sqlite3_prepare_v2(format->flavour_text, flavour_text_id_sql, strlen(flavour_text_id_sql), &format->flavour_text_id_search, NULL) != SQLITE_OK) {
		/* print sqlite3 error before exiting */
		status = sqlite3_errcode(format->flavour_text);
		error = sqlite3_errmsg(format->flavour_text);
		exit_func_safe("sqlite3 code %d; %s", status, error);
		if(format->flavour_text != NULL) sqlite3_close(format->flavour_text);
		return CHECK_FAILED;
	}
	return CHECK_PASSED;
}

int init_format(format_t * format, lua_State * lstate, int format_index, int file_format) {
	int table_index = 0;
	int item_type_id = 0;
	const char * item_type = NULL;

	/* check for null */
	if(exit_null_safe(2, format, lstate))
		return CHECK_FAILED;

	/* initialize the item rule array */
	memset(format->format_type_list, 0, sizeof(format_type_t *) * ITEM_TYPE_SIZE);
	format->file_format = file_format;

	/* get the item format table */
	lua_getfield(lstate, format_index, "item_format");
	table_index = lua_gettop(lstate);
	if(!lua_istable(lstate, table_index)) {
		exit_abt_safe("failed to find 'item_format' setting");
		return CHECK_FAILED;
	}

	/* iterate and initialize format structure */
	lua_pushnil(lstate);
	while(lua_next(lstate, table_index)) {
		if(!lua_istable(lstate, -1)) {
			exit_abt_safe("'item_format' table's value type must be a table");
			return CHECK_FAILED;
		}
		/* check that the key type is a string */
		if(!lua_isstring(lstate, -2)) {
			exit_abt_safe("'item_format' table's key type must be a string");
			return CHECK_FAILED;
		}

		/* map the string to constant */
		item_type = lua_tostring(lstate, -2);
		if(ncs_strcmp(item_type, "healing") == 0)
			item_type_id = HEALING_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "usable") == 0)
			item_type_id = USABLE_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "etc") == 0)
			item_type_id = ETC_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "weapon") == 0)
			item_type_id = WEAPON_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "armor") == 0)
			item_type_id = ARMOR_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "card") == 0)
			item_type_id = CARD_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "pet_egg") == 0)
			item_type_id = PET_EGG_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "pet_equip") == 0)
			item_type_id = PET_EQUIP_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "ammo") == 0)
			item_type_id = AMMO_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "delay_usable") == 0)
			item_type_id = DELAY_USABLE_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "confirm_usable") == 0)
			item_type_id = DELAY_CONFIRM_ITEM_TYPE;
		else if(ncs_strcmp(item_type, "shadow_equip") == 0)
			item_type_id = SHADOW_EQUIP_ITEM_TYPE;
		else {
			exit_func_safe("'item_format' has invalid item type %s\n", item_type);
			lua_pop(lstate, 3);
			return CHECK_FAILED;
		}

		/* load the item format linked list */
		if(init_format_type(format, lstate, lua_gettop(lstate), item_type_id) != CHECK_PASSED)
			return CHECK_FAILED;
		lua_pop(lstate, 1);
	}
	/* remove the key and item format table */
	lua_pop(lstate, 2);
	return CHECK_PASSED;
}

int init_format_type(format_t * format, lua_State * lstate, int type_table, int item_type_id) {
	format_type_t * format_type_temp = NULL;
	format_type_t * format_type_iter = NULL;

	/* check the item type id index */
	if(item_type_id < 0 || item_type_id > ITEM_TYPE_SIZE) {
		exit_func_safe("invalid format item type id %d", item_type_id);
		return CHECK_FAILED;
	}

	/* get the root of the format type's linked list */
	lua_pushnil(lstate);
	while(lua_next(lstate, type_table)) {
		format_type_temp = calloc(1, sizeof(format_type_t));

		/* check that each rule is a table */
		if(!lua_istable(lstate, -1)) {
			exit_abt_safe("item format's rules must be a table");
			return CHECK_FAILED;
		}

		/* load the item table and format */
		if(load_item_id(format_type_temp, lstate, lua_gettop(lstate)) != CHECK_PASSED) {
			exit_abt_safe("failed to load item id in item format rule");
			return CHECK_FAILED;
		}
		if(load_type_format(format_type_temp, lstate, lua_gettop(lstate)) != CHECK_PASSED) {
			exit_abt_safe("failed to load item format in item format rule");
			return CHECK_FAILED;
		}

		/* load item type specified filters */
		switch(item_type_id) {
			case HEALING_ITEM_TYPE:
			case USABLE_ITEM_TYPE:
			case ETC_ITEM_TYPE:
			case WEAPON_ITEM_TYPE:
			case ARMOR_ITEM_TYPE:
			case CARD_ITEM_TYPE:
			case PET_EGG_ITEM_TYPE:
			case PET_EQUIP_ITEM_TYPE:
			case AMMO_ITEM_TYPE:
			case DELAY_USABLE_ITEM_TYPE:
			case DELAY_CONFIRM_ITEM_TYPE:
			case SHADOW_EQUIP_ITEM_TYPE:
				break;
		}

		format_type_iter = (format->format_type_list[item_type_id] == NULL) ?
			(format->format_type_list[item_type_id] = format_type_temp):
			(format_type_iter->next = format_type_temp);
		lua_pop(lstate, lua_gettop(lstate) - type_table - 1);
	}
	return CHECK_PASSED;
}

int deit_format(format_t * format) {
	int i = 0;
	format_type_t * format_type_temp = NULL;
	format_type_t * format_type_iter = NULL;
	format_field_t * format_field_temp = NULL;
	format_field_t * format_field_iter = NULL;

	for(i = 0; i < ITEM_TYPE_SIZE;i++) {
		format_type_iter = format->format_type_list[i];
		while(format_type_iter != NULL) {
			format_type_temp = format_type_iter;
			format_type_iter = format_type_iter->next;
			/* free the item id range */
			if(format_type_temp->item_id != NULL)
				freerange(format_type_temp->item_id);
			/* free the format */
			format_field_iter = format_type_temp->format;
			while(format_field_iter != NULL) {
				format_field_temp = format_field_iter;
				format_field_iter = format_field_iter->next;
				free(format_field_temp);
			}
			free(format_type_temp);
		}
	}

	if(format->flavour_text != NULL) {
		sqlite3_finalize(format->flavour_text_id_search);
		sqlite3_close(format->flavour_text);
	}
}

int flavour_text_id_search(format_t * format, flavour_text_t * flavour_text, int item_id) {
	int status = 0;
	sqlite3_stmt * stmt = NULL;
	/* check null paramaters */
	if(exit_null_safe(2, format, flavour_text))
		return CHECK_FAILED;
	stmt = format->flavour_text_id_search;
	sqlite3_clear_bindings(stmt);
	sqlite3_bind_int(stmt, 1, item_id);
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		flavour_text->item_id = sqlite3_column_int(stmt, 0);
		strncopy(flavour_text->unidentified_display_name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
		strncopy(flavour_text->unidentified_resource_name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 2));
		strncopy(flavour_text->unidentified_description_name, MAX_DESCRIPTION_SIZE, sqlite3_column_text(stmt, 3));
		strncopy(flavour_text->identified_display_name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 4));
		strncopy(flavour_text->identified_resource_name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 5));
		strncopy(flavour_text->identified_description_name, MAX_DESCRIPTION_SIZE, sqlite3_column_text(stmt, 6));
		flavour_text->slot_count = sqlite3_column_int(stmt, 7);
		flavour_text->class_num = sqlite3_column_int(stmt, 8);
	}
	if(sqlite3_reset(stmt) != SQLITE_OK)
		exit_func_safe("sqlite3 code %d; %s", sqlite3_errcode(format->flavour_text), sqlite3_errmsg(format->flavour_text));
	return (status == SQLITE_ROW) ? CHECK_PASSED : CHECK_FAILED;
}

int load_item_id(format_type_t * type, lua_State * lstate, int table_index) {
	int status = 0;
	int item_id_table = 0;

	/* get the item id field */
	lua_getfield(lstate, table_index, "item_id");
	item_id_table = lua_gettop(lstate);
	if(!lua_istable(lstate, -1) || table_index == item_id_table) {
		exit_abt_safe("item rule's item_id value must be a table or item_id is missing");
		return CHECK_FAILED;
	}

	/* item id can specified an array of tuples or a single tuple */
	status = load_item_id_re(type, lstate, item_id_table);
	lua_pop(lstate, 1);
	return status;
}

int load_item_id_re(format_type_t * type, lua_State * lstate, int table_index) {
	int phase = 0;
	int item_id_start = 0;
	int item_id_final = 0;
	range_t * left_range = NULL;
	range_t * right_range = NULL;

	/* item id can be a single tuple or a list of tuple */
	lua_pushnil(lstate);
	while(lua_next(lstate, table_index)) {
		if(phase == 0) {
			/* either a list of tuple or tuple */
			if(lua_istable(lstate, -1)) {
				load_item_id_re(type, lstate, lua_gettop(lstate));
			} else {
				if(!lua_isnumber(lstate, -1)){
					exit_abt_safe("item_id tuple must contain numbers only");
					return CHECK_FAILED;
				}
				item_id_start = lua_tonumber(lstate, -1);
				phase++;
			}
		} else {
			if(!lua_isnumber(lstate, -1)){
				exit_abt_safe("item_id tuple must contain numbers only");
				return CHECK_FAILED;
			}
			item_id_final = lua_tonumber(lstate, -1);

			/* specifying start and final to be -1 means default */
			if(item_id_start == item_id_final && item_id_start == -1) {
				item_id_start = 0;
				item_id_final = INT_MAX;
			}

			if(type->item_id == NULL) {
				/* create the initial range */
				type->item_id = mkrange(INIT_OPERATOR, item_id_start, item_id_final, DONT_CARE);
			} else {
				/* merge both ranges */
				left_range = type->item_id;
				right_range = mkrange(INIT_OPERATOR, item_id_start, item_id_final, DONT_CARE);
				type->item_id = orrange(left_range, right_range);
				freerange(left_range);
				freerange(right_range);
			}
			/* each tuple only has start and final item id */
			lua_pop(lstate, lua_gettop(lstate) - table_index);
			break;
		}
		lua_pop(lstate, lua_gettop(lstate) - table_index - 1);
	}
	return CHECK_PASSED;
}

int load_type_format(format_type_t * type, lua_State * lstate, int table_index) {
	int item_format_table = 0;
	format_field_t * temp = NULL;
	format_field_t * iter = NULL;

	/* get the item id field */
	lua_getfield(lstate, table_index, "format");
	item_format_table = lua_gettop(lstate);
	if(!lua_istable(lstate, -1) || table_index == item_format_table) {
		exit_abt_safe("item rule's format value must be a table or format is missing");
		return CHECK_FAILED;
	}

	/* create the format linked list */
	lua_pushnil(lstate);
	while(lua_next(lstate, item_format_table)) {
		temp = calloc(1, sizeof(format_field_t));
		load_type_format_field(temp, lstate, lua_gettop(lstate));
		iter = (type->format == NULL) ?
			(type->format = temp):
			(iter->next = temp);
		lua_pop(lstate, lua_gettop(lstate) - item_format_table - 1);
	}

	/* pop the last key and format table */
	lua_pop(lstate, 2);
	return CHECK_PASSED;
}

int load_type_format_field(format_field_t * field, lua_State * lstate, int table_index) {
	const char * item_type_field = NULL;
	lua_getfield(lstate, table_index, "type");
	if(!lua_isstring(lstate, -1)) {
		exit_abt_safe("item format's type must be a string");
		return CHECK_FAILED;
	}

	/* map the item format type to a constant */
	item_type_field = lua_tostring(lstate, -1);
	if(ncs_strcmp(item_type_field, "flavour") == 0) {
		field->item_field = FLAVOUR_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "type") == 0) {
		field->item_field = TYPE_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "buy") == 0) {
		field->item_field = BUY_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "sell") == 0) {
		field->item_field = SELL_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "weight") == 0) {
		field->item_field = WEIGHT_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "attack") == 0) {
		field->item_field = ATK_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "defense") == 0) {
		field->item_field = DEF_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "range") == 0) {
		field->item_field = RANGE_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "job") == 0 ||
			  ncs_strcmp(item_type_field, "class") == 0) {
		field->item_field = JOB_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "gender") == 0) {
		field->item_field = GENDER_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "location") == 0) {
		field->item_field = LOC_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "weapon_level") == 0) {
		field->item_field = WEAPON_LEVEL_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "require_level") == 0) {
		field->item_field = LEVEL_REQUIRE_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "refinement") == 0) {
		field->item_field = REFINE_ABILITY_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "script") == 0) {
		field->item_field = SCRIPT_ITEM_FIELD;
	} else {
		exit_func_safe("invalid item format field %s", item_type_field);
		lua_pop(lstate, 1);
		return CHECK_FAILED;
	}
	lua_pop(lstate, 1);
	return CHECK_PASSED;
}

int write_item(format_t * format, item_t * item, char * script) {
	flavour_text_t flavour;

	/* search for the item's flavour text */
	flavour_text_id_search(format, &flavour, item->id);

	
}

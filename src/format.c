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

int init_format(format_t * format, lua_State * lstate, int format_index, int file_format, int server_type) {
	int table_index = 0;
	int item_type_id = 0;
	const char * item_type = NULL;

	/* check for null */
	if(exit_null_safe(2, format, lstate))
		return CHECK_FAILED;

	/* initialize the item rule array */
	memset(format->format_type_list, 0, sizeof(format_rule_t *) * ITEM_TYPE_SIZE);
	format->file_format = file_format;
	format->server_type = server_type;

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
			/* rathena swap item type for weapon and armor */
			if(server_type == MODE_RATHENA) {
				item_type_id = WEAPON_ITEM_TYPE_RA;
			} else {
				item_type_id = WEAPON_ITEM_TYPE;
			}
		else if(ncs_strcmp(item_type, "armor") == 0)
			/* rathena swap item type for weapon and armor */
			if(server_type == MODE_RATHENA) {
				item_type_id = ARMOR_ITEM_TYPE_RA;
			} else {
				item_type_id = ARMOR_ITEM_TYPE;
			}
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
	format_rule_t * format_rule_temp = NULL;
	format_rule_t * format_type_iter = NULL;

	/* check the item type id index */
	if(item_type_id < 0 || item_type_id > ITEM_TYPE_SIZE) {
		exit_func_safe("invalid format item type id %d", item_type_id);
		return CHECK_FAILED;
	}

	/* get the root of the format type's linked list */
	lua_pushnil(lstate);
	while(lua_next(lstate, type_table)) {
		format_rule_temp = calloc(1, sizeof(format_rule_t));

		/* check that each rule is a table */
		if(!lua_istable(lstate, -1)) {
			exit_abt_safe("item format's rules must be a table");
			return CHECK_FAILED;
		}

		/* load the item table and format */
		if(load_item_id(format_rule_temp, lstate, lua_gettop(lstate)) != CHECK_PASSED) {
			exit_abt_safe("failed to load item id in item format rule");
			return CHECK_FAILED;
		}
		if(load_type_format(format_rule_temp, lstate, lua_gettop(lstate)) != CHECK_PASSED) {
			exit_abt_safe("failed to load item format in item format rule");
			return CHECK_FAILED;
		}

		/* load item type specified filters */
		if(format->server_type != MODE_RATHENA) {
			switch(item_type_id) {
				case HEALING_ITEM_TYPE:			break;
				case USABLE_ITEM_TYPE:			break;
				case ETC_ITEM_TYPE:				break;
				case WEAPON_ITEM_TYPE:			load_weapon_type(format_rule_temp, lstate, lua_gettop(lstate)); break;
				case ARMOR_ITEM_TYPE:			break;
				case CARD_ITEM_TYPE:			break;
				case PET_EGG_ITEM_TYPE:			break;
				case PET_EQUIP_ITEM_TYPE:		break;
				case AMMO_ITEM_TYPE:			break;
				case DELAY_USABLE_ITEM_TYPE:	break;
				case DELAY_CONFIRM_ITEM_TYPE:	break;
				case SHADOW_EQUIP_ITEM_TYPE:	break;
			}
		} else {
			switch(item_type_id) {
				case WEAPON_ITEM_TYPE_RA:		load_weapon_type(format_rule_temp, lstate, lua_gettop(lstate)); break;
				case ARMOR_ITEM_TYPE_RA:		break;
			}
		}

		format_type_iter = (format->format_type_list[item_type_id] == NULL) ?
			(format->format_type_list[item_type_id] = format_rule_temp):
			(format_type_iter->next = format_rule_temp);
		lua_pop(lstate, lua_gettop(lstate) - type_table - 1);
	}
	return CHECK_PASSED;
}

int deit_format(format_t * format) {
	int i = 0;
	format_rule_t * format_rule_temp = NULL;
	format_rule_t * format_type_iter = NULL;
	format_field_t * format_field_temp = NULL;
	format_field_t * format_field_iter = NULL;

	for(i = 0; i < ITEM_TYPE_SIZE;i++) {
		format_type_iter = format->format_type_list[i];
		while(format_type_iter != NULL) {
			format_rule_temp = format_type_iter;
			format_type_iter = format_type_iter->next;
			/* free the item id range */
			if(format_rule_temp->item_id != NULL)
				freerange(format_rule_temp->item_id);
			/* free the format */
			format_field_iter = format_rule_temp->format;
			while(format_field_iter != NULL) {
				format_field_temp = format_field_iter;
				format_field_iter = format_field_iter->next;
				free(format_field_temp);
			}
			free(format_rule_temp);
		}
	}

	if(format->flavour_text != NULL) {
		sqlite3_finalize(format->flavour_text_id_search);
		sqlite3_close(format->flavour_text);
	}
	return CHECK_PASSED;
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

int load_item_id(format_rule_t * type, lua_State * lstate, int table_index) {
	int status = 0;
	int item_id_table = 0;

	/* get the item id field */
	lua_getfield(lstate, table_index, "item_id");
	item_id_table = lua_gettop(lstate);
	if(!lua_istable(lstate, -1) || lua_isnil(lstate, -1)) {
		exit_abt_safe("item rule's item_id value must be a table or item_id is missing");
		lua_pop(lstate, 1);
		return CHECK_FAILED;
	}

	/* item id can specified an array of tuples or a single tuple */
	status = load_item_id_re(type, lstate, item_id_table);
	lua_pop(lstate, 1);
	return status;
}

int load_item_id_re(format_rule_t * type, lua_State * lstate, int table_index) {
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

int load_type_format(format_rule_t * type, lua_State * lstate, int table_index) {
	int item_format_table = 0;
	format_field_t * temp = NULL;
	format_field_t * iter = NULL;

	/* get the item id field */
	lua_getfield(lstate, table_index, "format");
	item_format_table = lua_gettop(lstate);
	if(!lua_istable(lstate, -1) || lua_isnil(lstate, -1)) {
		exit_abt_safe("item rule's format value must be a table or format is missing");
		lua_pop(lstate, 1);
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
	lua_pop(lstate, 1);
	return CHECK_PASSED;
}

int load_type_format_field(format_field_t * field, lua_State * lstate, int table_index) {
	const char * item_type_field = NULL;
	lua_getfield(lstate, table_index, "type");
	if(!lua_isstring(lstate, -1) || lua_isnil(lstate, -1)) {
		exit_abt_safe("item format's type must be a string or is missing");
		lua_pop(lstate, 1);
		return CHECK_FAILED;
	}

	/* map the item format type to a constant */
	item_type_field = lua_tostring(lstate, -1);
	if(ncs_strcmp(item_type_field, "flavour") == 0) {
		field->field = FLAVOUR_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "type") == 0) {
		field->field = TYPE_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "buy") == 0) {
		field->field = BUY_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "sell") == 0) {
		field->field = SELL_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "weight") == 0) {
		field->field = WEIGHT_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "attack") == 0) {
		field->field = ATK_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "defense") == 0) {
		field->field = DEF_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "range") == 0) {
		field->field = RANGE_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "job") == 0 ||
			  ncs_strcmp(item_type_field, "class") == 0) {
		field->field = JOB_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "gender") == 0) {
		field->field = GENDER_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "location") == 0) {
		field->field = LOC_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "weapon_level") == 0) {
		field->field = WEAPON_LEVEL_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "require_level") == 0) {
		field->field = LEVEL_REQUIRE_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "refinement") == 0) {
		field->field = REFINE_ABILITY_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "script") == 0) {
		field->field = SCRIPT_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "view") == 0) {
		field->field = VIEW_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "upper") == 0) {
		field->field = UPPER_ITEM_FIELD;
	} else if(ncs_strcmp(item_type_field, "magical_attack") == 0) {
		field->field = MATK_ITEM_FIELD;
	} else {
		exit_func_safe("invalid item format field %s", item_type_field);
		lua_pop(lstate, 1);
		return CHECK_FAILED;
	}

	/* load the text */
	lua_getfield(lstate, table_index, "text");
	if(!lua_isnil(lstate, -1)) {
		if(!lua_isstring(lstate, -1)) {
			exit_abt_safe("item rule's text must be a string");
			return CHECK_FAILED;
		}
		strncopy(field->text, FMT_TEXT_SIZE, (const unsigned char *) lua_tostring(lstate, -1));
	}

	lua_pop(lstate, 2);
	return CHECK_PASSED;
}

int load_weapon_type(format_rule_t * rule, lua_State * lstate, int rule_table) {
	int weapon_table = 0;

	lua_getfield(lstate, rule_table, "weapon_type");
	weapon_table = lua_gettop(lstate);
	if(lua_isnil(lstate, -1)) {
		rule->weapon_filter = 0xFFFFFFFF;
		lua_pop(lstate, 1);
		return CHECK_PASSED;
	}

	if(!lua_istable(lstate, weapon_table)) {
		exit_abt_safe("weapon type must be a table in item rule");
		lua_pop(lstate, 1);
		return CHECK_FAILED;
	}

	rule->weapon_filter = 0;
	lua_pushnil(lstate);
	while(lua_next(lstate, weapon_table)) {
		if(!lua_isnumber(lstate, -1)) {
			exit_abt_safe("all values in weapon type table must be numbers");
			lua_pop(lstate, lua_gettop(lstate) - weapon_table + 1);
			return CHECK_FAILED;
		}
		rule->weapon_filter |= weapon_flag((int) lua_tointeger(lstate, -1));
		lua_pop(lstate, lua_gettop(lstate) - weapon_table - 1);
	}

	/* pop the weapon table and last key */
	lua_pop(lstate, 1);
	return CHECK_PASSED;
}

int write_item(FILE * file, format_t * format, item_t * item, char * script) {
	format_rule_t * item_rule = NULL;
	format_field_t * item_field = NULL;

	/* check item type is valid */
	if(item->type < 0 || item->type > ITEM_TYPE_SIZE) {
		exit_func_safe("invalid item type %d\n", item->type);
		return CHECK_FAILED;
	}

	/* find a matching item rule per item type  */
	item_rule = format->format_type_list[item->type];
	while(item_rule != NULL) {
		/* filter by item id */
		if(!searchrange(item_rule->item_id, item->id)) {
			item_rule = item_rule->next;
			continue;
		}
		/* filter by weapon type */
		if((item->type == WEAPON_ITEM_TYPE && format->server_type != MODE_RATHENA) ||
		   (item->type == WEAPON_ITEM_TYPE_RA && format->server_type == MODE_RATHENA)){
			if(item_rule->weapon_filter & weapon_flag(item->view)) {
				break;
			} else {
				item_rule = item_rule->next;
				continue;
			}
		}
		break;
	}

	/* check whether an item rule is found */
	if(item_rule == NULL) return CHECK_FAILED;

	/* write item based on format */
	item_field = item_rule->format;
	if(format->file_format & FORMAT_TXT)
		return write_item_text(file, format, item_field, item, script);

	return CHECK_PASSED;
}

int write_item_text(FILE * file, format_t * format, format_field_t * field, item_t * item, char * script) {
	char buffer[FMT_BUF_SIZE];
	int offset = 0;
	flavour_text_t flavour;

	/* write text format header */
	fprintf(file,"%d#\n", item->id);
	
	while(field != NULL) {
		switch(field->field) {
			case FLAVOUR_ITEM_FIELD: 		format_flavour_text(buffer, &offset, format, field, &flavour, item->id); 	break;
			case SCRIPT_ITEM_FIELD:			format_script(buffer, &offset, field, script); 								break;
			case TYPE_ITEM_FIELD:			format_type(buffer, &offset, field, item->type);							break;
			case BUY_ITEM_FIELD:			format_integer(buffer, &offset, field, item->buy); 							break;
			case SELL_ITEM_FIELD:			format_integer(buffer, &offset, field, item->sell); 						break;
			case WEIGHT_ITEM_FIELD: 		format_weight(buffer, &offset, field, item->weight); 						break;
			case ATK_ITEM_FIELD:			format_integer(buffer, &offset, field, item->atk); 							break;
			case DEF_ITEM_FIELD:			format_integer(buffer, &offset, field, item->def); 							break;
			case RANGE_ITEM_FIELD:			format_integer(buffer, &offset, field, item->range); 						break;
			case JOB_ITEM_FIELD:			format_job(buffer, &offset, field, item->job, item->upper, item->gender);	break;
			case GENDER_ITEM_FIELD:			format_gender(buffer, &offset, field, item->gender);						break;
			case LOC_ITEM_FIELD:			format_location(buffer, &offset, field, item->loc);							break;
			case WEAPON_LEVEL_ITEM_FIELD:	format_integer(buffer, &offset, field, item->wlv); 							break;
			case LEVEL_REQUIRE_ITEM_FIELD:	format_integer(buffer, &offset, field, item->elv_min); 						break;
			case REFINE_ABILITY_ITEM_FIELD:	format_refinement(buffer, &offset, field, item->refineable);				break;
			case VIEW_ITEM_FIELD:			format_view(buffer, &offset, field, item->view, item->type);				break;
			case UPPER_ITEM_FIELD:			format_upper(buffer, &offset, field, item->upper);							break;
			case MATK_ITEM_FIELD:			format_integer(buffer, &offset, field, item->matk); 						break;
			default: break;
		}
		field = field->next;
	}

	/* write the text format content and footer */
	fprintf(file, "%s#\n", buffer);
	return CHECK_PASSED;
}

int format_flavour_text(char * buffer, int * offset, format_t * format, format_field_t * field, flavour_text_t * flavour, int item_id) {
	int i = 0;
	char * flavor = NULL;			/* reference the item description name */
	int length = 0;					/* length of the item description name */
	char * string[FMT_LINE_SIZE];	/* parsed strings separated by a peroid */
	int count = 0;					/* number of parsed strings */

	/* search flavour text for item id */
	if(flavour_text_id_search(format, flavour, item_id) != CHECK_PASSED)
		return CHECK_FAILED;
	
	/* preprocess the flavour text buffer */
	flavor = flavour->identified_description_name;
	length = strlen(flavor);
	
	/* skip initial newline and space characters */
	for(i = 0; i < length; i++)
		if(isspace(flavor[i])) flavor[i] = '\0'; else break;

	/* set the initial string */
	string[count++] = &flavor[i];

	for(; i < length && flavor[i] != '\0'; i++) {
		/* convert period to a newline character */
		if(flavor[i] == '.' || (i + 1 < length && flavor[i + 1] == '\0')) {
			/* skip up to the last peroid */
			while(i + 1 < length && flavor[i + 1] == '.') i++;
			flavor[i] = '\0';
			/* skip up to the last initial whitespace */
			while(i + 1 < length && isspace(flavor[i + 1])) i++;
			/* set the next string */
			string[count++] = &flavor[i + 1];
			if(count > FMT_LINE_SIZE) {
				return exit_func_safe("description string count overflowed; only support up to %d strings", FMT_LINE_SIZE);
			}
		}
	}

	/* the last string is always nil */
	count--;

	/* write the flavour text strings into the buffer */
	for(i = 0; i < count; i++)
		*offset += sprintf(&buffer[*offset], "%s.\n", string[i]);
	
	return CHECK_PASSED;
}

int format_script(char * buffer, int * offset, format_field_t * field, char * script) {
	/* write the script into the buffer */
	if(script[0] != '\0')
		*offset += sprintf(&buffer[*offset], "%s", script);
	return CHECK_PASSED;
}

int format_integer(char * buffer, int * offset, format_field_t * field, int value) {
	/* write the the text and value */
	if(value > 0)
		*offset += (field->text[0] != '\0')?
			sprintf(&buffer[*offset], "%s %d\n", field->text, value):
			sprintf(&buffer[*offset], "%d\n", value);
	return CHECK_PASSED;
}

int format_refinement(char * buffer, int * offset, format_field_t * field, int value) {
	if(value == 0)
		if(field->text[0] != '\0')
			*offset += sprintf(&buffer[*offset], "%s\n", field->text);
	return CHECK_PASSED;
}

int format_gender(char * buffer, int * offset, format_field_t * field, int value) {
	char * gender = NULL;

	/* resolve the gender to a gender string */
	switch(value) {
		case 0: gender = "female"; break;
		case 1: gender = "male"; break;
		case 2: break;
		default: exit_func_safe("cannot interpret gender value %d", value);
	}

	/* unisex or invalid gender values are not interpreted */
	if(gender == NULL) return CHECK_FAILED;

	/* write the female or male only restriction */
	*offset += (field->text[0] != '\0')?
		sprintf(&buffer[*offset], "%s %s\n", field->text, gender):
		sprintf(&buffer[*offset], "%s\n", gender);
	return CHECK_PASSED;
}

int format_view(char * buffer, int * offset, format_field_t * field, int view, int type) {
	char * view_type = NULL;

	/* view is only interpreted for weapon and ammo type */
	if(type != WEAPON_ITEM_TYPE && type != WEAPON_ITEM_TYPE_RA && type != AMMO_ITEM_TYPE) 
		return CHECK_FAILED;

	/* map the weapon type constant to weapon type string */
	if(type == WEAPON_ITEM_TYPE || type == WEAPON_ITEM_TYPE_RA) {
		view_type = weapon_tbl(view);
		if(ncs_strcmp(view_type, "error") == 0) {
			exit_func_safe("failed to find weapon type for view %d on type %d", view, type);
			return CHECK_FAILED;
		}
	} else {
		view_type = ammo_tbl(view);
		if(ncs_strcmp(view_type, "error") == 0) {
			exit_func_safe("failed to find ammo type for view %d on type %d", view, type);
			return CHECK_FAILED;
		}
	}

	/* write the weapon type string */
	*offset += (field->text[0] != '\0')?
		sprintf(&buffer[*offset], "%s %s\n", field->text, view_type):
		sprintf(&buffer[*offset], "%s\n", view_type);
	return CHECK_PASSED;
}

int format_type(char * buffer, int * offset, format_field_t * field, int type) {
	char * item_type = NULL;

	/* map the item type constant to item type string */
	item_type = item_type_tbl(type);
	if(ncs_strcmp(item_type, "error") == 0) {
		exit_func_safe("failed to find item type for item type %d", type);
		return CHECK_FAILED;
	}

	/* write the item type string */
	*offset += (field->text[0] != '\0')?
		sprintf(&buffer[*offset], "%s %s\n", field->text, item_type):
		sprintf(&buffer[*offset], "%s\n", item_type);
	return CHECK_PASSED;
}

int format_job(char * buffer, int * offset, format_field_t * field, int job, int upper, int gender) {
	int initial_offset = 0;
	
	/* check the common classes */
	if(job == 0xFFFFFFFF) {
		*offset += sprintf(&buffer[*offset], "%s Every Job\n", field->text);
		return CHECK_PASSED;
	} else if(job == 0xFFFFFFFE) {
		*offset += sprintf(&buffer[*offset], "%s Every Job except Novice\n", field->text);
		return CHECK_PASSED;
	}

	/* write the text */
	*offset += sprintf(&buffer[*offset], "%s ", field->text);
	initial_offset = *offset;

	/* simplified the combination explosion */
	if(job & 0x00000001) *offset += sprintf(&buffer[*offset], "%sNovice", (initial_offset < *offset)?", ":"");
	if(job & 0x00000002) *offset += sprintf(&buffer[*offset], "%sSwordsman", (initial_offset < *offset)?", ":"");
	if(job & 0x00000080) *offset += sprintf(&buffer[*offset], "%sKnight", (initial_offset < *offset)?", ":"");
	if(job & 0x00004000) *offset += sprintf(&buffer[*offset], "%sCrusader", (initial_offset < *offset)?", ":"");
	if(job & 0x00000004) *offset += sprintf(&buffer[*offset], "%sMagician", (initial_offset < *offset)?", ":"");
	if(job & 0x00000200) *offset += sprintf(&buffer[*offset], "%sWizard", (initial_offset < *offset)?", ":"");
	if(job & 0x00010000) *offset += sprintf(&buffer[*offset], "%sSage", (initial_offset < *offset)?", ":"");
	if(job & 0x00000008) *offset += sprintf(&buffer[*offset], "%sArcher", (initial_offset < *offset)?", ":"");
	if(job & 0x00000800) *offset += sprintf(&buffer[*offset], "%sHunter", (initial_offset < *offset)?", ":"");
	if(job & 0x00080000 && gender == 0) *offset += sprintf(&buffer[*offset], "%sDancer", (initial_offset < *offset)?", ":"");
	if(job & 0x00080000 && gender == 1) *offset += sprintf(&buffer[*offset], "%sBard", (initial_offset < *offset)?", ":"");
	if(job & 0x00000010) *offset += sprintf(&buffer[*offset], "%sAcolyte", (initial_offset < *offset)?", ":"");
	if(job & 0x00000100) *offset += sprintf(&buffer[*offset], "%sPriest", (initial_offset < *offset)?", ":"");
	if(job & 0x00008000) *offset += sprintf(&buffer[*offset], "%sMonk", (initial_offset < *offset)?", ":"");
	if(job & 0x00000020) *offset += sprintf(&buffer[*offset], "%sMerchant", (initial_offset < *offset)?", ":"");
	if(job & 0x00000400) *offset += sprintf(&buffer[*offset], "%sBlacksmith", (initial_offset < *offset)?", ":"");
	if(job & 0x00040000) *offset += sprintf(&buffer[*offset], "%sAlchemist", (initial_offset < *offset)?", ":"");
	if(job & 0x00000040) *offset += sprintf(&buffer[*offset], "%sThief", (initial_offset < *offset)?", ":"");
	if(job & 0x00001000) *offset += sprintf(&buffer[*offset], "%sAssassin", (initial_offset < *offset)?", ":"");
	if(job & 0x00020000) *offset += sprintf(&buffer[*offset], "%sRogue", (initial_offset < *offset)?", ":"");
	if(job & 0x02000000) *offset += sprintf(&buffer[*offset], "%sNinja", (initial_offset < *offset)?", ":"");
	if(job & 0x20000000) *offset += sprintf(&buffer[*offset], "%sKagerou & Oboro", (initial_offset < *offset)?", ":"");
	if(job & 0x01000000) *offset += sprintf(&buffer[*offset], "%sGunslinger", (initial_offset < *offset)?", ":"");
	if(job & 0x40000000) *offset += sprintf(&buffer[*offset], "%sRebellion", (initial_offset < *offset)?", ":"");
	if(job & 0x00200000) *offset += sprintf(&buffer[*offset], "%sTaekwon", (initial_offset < *offset)?", ":"");
	if(job & 0x00400000) *offset += sprintf(&buffer[*offset], "%sStar Gladiator", (initial_offset < *offset)?", ":"");
	if(job & 0x00800000) *offset += sprintf(&buffer[*offset], "%sSoul Linker", (initial_offset < *offset)?", ":"");
	if(job & 0x04000000) *offset += sprintf(&buffer[*offset], "%sGangsi", (initial_offset < *offset)?", ":"");
	if(job & 0x08000000) *offset += sprintf(&buffer[*offset], "%sDeath Knight", (initial_offset < *offset)?", ":"");
	if(job & 0x10000000) *offset += sprintf(&buffer[*offset], "%sDark Collector", (initial_offset < *offset)?", ":"");
	*offset += sprintf(&buffer[*offset], "\n");
	return CHECK_PASSED;
}

int format_upper(char * buffer, int * offset, format_field_t * field, int upper) {
	int initial_offset = 0;

	/* write the text */
	*offset += sprintf(&buffer[*offset], "%s ", field->text);
	initial_offset = *offset;

	if(upper & 0x01) *offset += sprintf(&buffer[*offset], "%sFirst & Second Job", (initial_offset < *offset)?", ":"");
	if(upper & 0x02) *offset += sprintf(&buffer[*offset], "%sRebirth First & Second Job", (initial_offset < *offset)?", ":"");
	if(upper & 0x08) *offset += sprintf(&buffer[*offset], "%sThird Job", (initial_offset < *offset)?", ":"");
	if(upper & 0x10) *offset += sprintf(&buffer[*offset], "%sRebirth Third Job", (initial_offset < *offset)?", ":"");
	if(upper & 0x04) *offset += sprintf(&buffer[*offset], "%sSecond Baby Job", (initial_offset < *offset)?", ":"");
	if(upper & 0x20) *offset += sprintf(&buffer[*offset], "%sThird Baby Job", (initial_offset < *offset)?", ":"");

	*offset += sprintf(&buffer[*offset], "\n");
	return CHECK_PASSED;
}

int format_location(char * buffer, int * offset, format_field_t * field, int loc) {
	int initial_offset = 0;

	/* write the text */
	*offset += sprintf(&buffer[*offset], "%s ", field->text);
	initial_offset = *offset;

	if(loc & 0x000100) *offset += sprintf(&buffer[*offset], "%sUpper Headgear", (initial_offset < *offset)?", ":"");
	if(loc & 0x000200) *offset += sprintf(&buffer[*offset], "%sMiddle Headgear", (initial_offset < *offset)?", ":"");
	if(loc & 0x000001) *offset += sprintf(&buffer[*offset], "%sLower Headgear", (initial_offset < *offset)?", ":"");
	if(loc & 0x000010) *offset += sprintf(&buffer[*offset], "%sArmor", (initial_offset < *offset)?", ":"");
	if(loc & 0x000002) *offset += sprintf(&buffer[*offset], "%sWeapon", (initial_offset < *offset)?", ":"");
	if(loc & 0x000020) *offset += sprintf(&buffer[*offset], "%sShield", (initial_offset < *offset)?", ":"");
	if(loc & 0x000004) *offset += sprintf(&buffer[*offset], "%sGarment", (initial_offset < *offset)?", ":"");
	if(loc & 0x000040) *offset += sprintf(&buffer[*offset], "%sShoes", (initial_offset < *offset)?", ":"");
	if(loc & 0x000008) *offset += sprintf(&buffer[*offset], "%sRight Accessory", (initial_offset < *offset)?", ":"");
	if(loc & 0x000080) *offset += sprintf(&buffer[*offset], "%sLeft Accessory", (initial_offset < *offset)?", ":"");
	if(loc & 0x000400) *offset += sprintf(&buffer[*offset], "%sCostume Upper Headgear", (initial_offset < *offset)?", ":"");
	if(loc & 0x000800) *offset += sprintf(&buffer[*offset], "%sCostume Middle Headgear", (initial_offset < *offset)?", ":"");
	if(loc & 0x001000) *offset += sprintf(&buffer[*offset], "%sCostume Lower Headgear", (initial_offset < *offset)?", ":"");
	if(loc & 0x002000) *offset += sprintf(&buffer[*offset], "%sCostume Garment", (initial_offset < *offset)?", ":"");
	if(loc & 0x008000) *offset += sprintf(&buffer[*offset], "%sAmmo", (initial_offset < *offset)?", ":"");
	if(loc & 0x010000) *offset += sprintf(&buffer[*offset], "%sShadow Armor", (initial_offset < *offset)?", ":"");
	if(loc & 0x020000) *offset += sprintf(&buffer[*offset], "%sShadow Weapon", (initial_offset < *offset)?", ":"");
	if(loc & 0x040000) *offset += sprintf(&buffer[*offset], "%sShadow Shield", (initial_offset < *offset)?", ":"");
	if(loc & 0x080000) *offset += sprintf(&buffer[*offset], "%sShadow Shoes", (initial_offset < *offset)?", ":"");
	if(loc & 0x100000) *offset += sprintf(&buffer[*offset], "%sShadow Right Accessory", (initial_offset < *offset)?", ":"");
	if(loc & 0x200000) *offset += sprintf(&buffer[*offset], "%sShadow Left Accessory", (initial_offset < *offset)?", ":"");

	*offset += sprintf(&buffer[*offset], "\n");
	return CHECK_PASSED;
}

int format_weight(char * buffer, int * offset, format_field_t * field, int weight) {
	if(weight == 0) return CHECK_FAILED;	
	if(weight / 10 == 0) {
		*offset += (field->text[0] != '\0') ?
			sprintf(&buffer[*offset], "%s %.2f\n", field->text, ((double) weight) / 10):
			sprintf(&buffer[*offset], "%.2f\n", ((double) weight) / 10);
	} else {
		format_integer(buffer, offset, field, weight / 10);
	}
	return CHECK_PASSED;
}

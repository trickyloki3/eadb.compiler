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
	  (sqlite3_prepare_v2(format->flavour_text, flavour_text_id_sql, strlen
	  (flavour_text_id_sql), &format->flavour_text_id_search, NULL) != SQLITE_OK)) {
		/* print sqlite3 error before exiting */
		status = sqlite3_errcode(format->flavour_text);
		error = sqlite3_errmsg(format->flavour_text);
		exit_func_safe("sqlite3 code %d; %s", status, error);
		if(format->flavour_text != NULL) sqlite3_close(format->flavour_text);
		return CHECK_FAILED;
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

int init_format(format_t * format, lua_State * lstate, int format_index, int file_format, int server_type) {
	int item_type_table = 0;				/* index on stack to item type table */
	int item_format_table = 0;				/* index on stack to item format table */
	int item_rule_table = 0;				/* item format rule table for item type */
	/* item type name and constant */
	const char * item_type_name = NULL;
	int item_type_constant = 0;

	/* check for null references */	
	if(exit_null_safe(2, format, lstate)) 
		return CHECK_FAILED;

	/* set server type, file format, and format rules */
	format->file_format = file_format;
	format->server_type = server_type;
	memset(format->rule_list, 0, sizeof(format_rule_t *) * ITEM_TYPE_SIZE);

	/* get the item type and item format table */
	lua_getglobal(lstate, "item_type");
	if(lua_isnil(lstate, -1)) {
		exit_abt_safe("failed to find global item type table");
		lua_pop(lstate, 1);
		return CHECK_FAILED;
	}
	if(lua_get_field(lstate, format_index, "item_format", LUA_TTABLE))
		return CHECK_FAILED;
	item_type_table = lua_gettop(lstate) - 1;
	item_format_table = lua_gettop(lstate);

	/* iterate the item format table */
	lua_pushnil(lstate);
	while(lua_next(lstate, item_format_table)) {
		/* check that the key type is a string */
		if(lua_isnil(lstate, -2)) {
			exit_abt_safe("'item format' has invalid nil key");
			break;
		} else if(!lua_isstring(lstate, -2)) {
			exit_abt_safe("'item format' key values must be a string value");
			break;
		}
		item_type_name = lua_tostring(lstate, -2);

		/* check that the value type is a table */
		if(lua_isnil(lstate, -1)) {
			exit_func_safe("'item format' key '%s' has nil value", item_type_name);
			break;
		} else if(!lua_istable(lstate, -1)) {
			exit_func_safe("'item format' key '%s' must have a table value", item_type_name);
			break;
		}
		item_rule_table = lua_gettop(lstate);

		/* conver item type name to item type constant */
		if(lua_get_field(lstate, item_type_table, item_type_name, LUA_TNUMBER)) {
			exit_func_safe("failed to map item type string %s to constant", item_type_name);
			break;
		} else {
			item_type_constant = lua_tonumber(lstate, -1);
			lua_pop(lstate, 1);
		}

		/* load the item format linked list */
		if(init_format_type(format, lstate, item_rule_table, item_type_constant)) {
			exit_func_safe("failed to load item rule for '%s'", item_type_name);
			break;
		}

		/* remove element until the last key */
		lua_pop(lstate, lua_gettop(lstate) - item_format_table - 1);
	}
	
	/* remove all element until the configuration table */
	lua_pop(lstate, lua_gettop(lstate) - format_index);
	return CHECK_PASSED;
}

int init_format_type(format_t * format, lua_State * lstate, int rule_table, int item_type_constant) {
	/* I am a linked list monster; nom nom nom */
	format_rule_t * rule_temp = NULL;
	format_rule_t * rule_iter = NULL;
	int rule = 0;
	/* check the item type constant index */
	if(item_type_constant < 0 || item_type_constant > ITEM_TYPE_SIZE) {
		exit_func_safe("invalid format item type constant %d", item_type_constant);
		return CHECK_FAILED;
	}

	/* iterate the item rule table */
	lua_pushnil(lstate);
	while(lua_next(lstate, rule_table)) {
		/* check the that the value type is a table */
		if(lua_isnil(lstate, -1)) {
			exit_abt_safe("invalid item rule table with nil value");
			break;
		} else if(!lua_istable(lstate, -1)) {
			exit_abt_safe("item rule must be a table value");
			break;
		}
		rule = lua_gettop(lstate);

		/* create an item rule node and add to linked list */
		rule_temp = calloc(1, sizeof(format_rule_t));
		rule_iter = (format->rule_list[item_type_constant] == NULL) ?
					(format->rule_list[item_type_constant] = rule_temp):
					(rule_iter->next = rule_temp);

		/* load the item table and format */
		if(load_item_id(rule_temp, lstate, rule) != CHECK_PASSED) {
			exit_abt_safe("failed to load item id for item rule");
			break;
		}
		if(load_item_field(rule_temp, lstate, rule) != CHECK_PASSED) {
			exit_abt_safe("failed to load item format for item rule");
			break;
		}

		/* load special item rule filters */
		if(item_type_constant == WEAPON_ITEM_TYPE)
			if(load_weapon_type(rule_temp, lstate, rule))
				exit_abt_safe("failed to load weapon type filter");

		
		lua_pop(lstate, lua_gettop(lstate) - rule_table - 1);
	}

	/* remove all element until the item rule table */
	lua_pop(lstate, lua_gettop(lstate) - rule_table);
	return CHECK_PASSED;
}

int load_item_id(format_rule_t * type, lua_State * lstate, int rule) {
	int status = 0;
	int item_id_table = 0;

	/* get the item id table */
	if(lua_get_field(lstate, rule, "item_id", LUA_TTABLE))
		return CHECK_FAILED;
	item_id_table = lua_gettop(lstate);

	/* recursive add item id ranges */
	status = load_item_id_re(type, lstate, item_id_table);
	lua_pop(lstate, lua_gettop(lstate) - rule);
	return status;
}

int load_item_id_re(format_rule_t * type, lua_State * lstate, int item_id_table) {
	int phase = 0;
	int item_id_start = 0;
	int item_id_final = 0;
	range_t * left_range = NULL;
	range_t * right_range = NULL;

	/* item id can be a single tuple or a list of tuple */
	lua_pushnil(lstate);
	while(lua_next(lstate, item_id_table)) {
		if(phase == 0) {
			/* item id is either a list of item id tuples or a single item id tuple */
			if(lua_istable(lstate, -1)) {
				load_item_id_re(type, lstate, lua_gettop(lstate));
			} else {
				if(!lua_isnumber(lstate, -1)) {
					exit_abt_safe("invalid item id value");
					break;
				}
				item_id_start = lua_tonumber(lstate, -1);
				phase++;
			}
		} else {
			/* each tuple has two item id */
			if(!lua_isnumber(lstate, -1)) {
				exit_abt_safe("invalid item id value");
				break;
			}
			item_id_final = lua_tonumber(lstate, -1);

			/* default item id range if item id is -1 */
			if(item_id_start == -1 || item_id_final == -1) {
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
			
			lua_pop(lstate, lua_gettop(lstate) - item_id_table);
			break;
		}
		lua_pop(lstate, lua_gettop(lstate) - item_id_table - 1);
	}
	return CHECK_PASSED;
}

int load_item_field(format_rule_t * type, lua_State * lstate, int rule) {
	int item_field_table = 0;
	format_field_t * field_temp = NULL;
	format_field_t * field_iter = NULL;

	/* get the item id field */
	if(lua_get_field(lstate, rule, "format", LUA_TTABLE))
		return CHECK_FAILED;
	item_field_table = lua_gettop(lstate);
	
	/* iterate through the format table */
	lua_pushnil(lstate);
	while(lua_next(lstate, item_field_table)) {
		/* check the that the value type is a table */
		if(lua_isnil(lstate, -1)) {
			exit_abt_safe("invalid item field table with nil value");
			break;
		} else if(!lua_istable(lstate, -1)) {
			exit_abt_safe("item field must be a table value");
			break;
		}

		/* create the field node and add to linked list */
		field_temp = calloc(1, sizeof(format_field_t));
		field_iter = (type->format == NULL) ?
			   (type->format = field_temp):
			   (field_iter->next = field_temp);

		/* loads the ["type"] and ["text"] for each table value in ["format"] */
		if(load_item_field_re(field_temp, lstate, lua_gettop(lstate))) 
			break;

		/* pop everything except for the item field table and key */
		lua_pop(lstate, lua_gettop(lstate) - item_field_table - 1);
	}

	/* pop everything except for the item format table */
	lua_pop(lstate, lua_gettop(lstate) - item_field_table);
	return CHECK_PASSED;
}

int load_item_field_re(format_field_t * field, lua_State * lstate, int field_table) {
	size_t length = 0;
	const char * item_field_type = NULL;
	const char * item_field_text = NULL;
	int item_field_table = 0;

	/* get the item field type string */
	if(lua_get_field(lstate, field_table, "type", LUA_TSTRING))
		return CHECK_FAILED;
	item_field_type = lua_tostring(lstate, -1);

	/* get the item field type table */
	lua_getglobal(lstate, "item_field");
	if(lua_isnil(lstate, -1)) {
		exit_abt_safe("failed to find global item field table");
		lua_pop(lstate, lua_gettop(lstate) - field_table);
		return CHECK_FAILED;
	}
	item_field_table = lua_gettop(lstate);

	/* map the item field type string to constant */
	if(lua_get_field(lstate, item_field_table, item_field_type, LUA_TNUMBER)) {
		exit_func_safe("failed to map item field string %s to constant", item_field_type);
		return CHECK_FAILED;
	}
	field->field = lua_tointeger(lstate, -1);

	/* load the text [optional] */
	if(!lua_get_field(lstate, field_table, "text", LUA_TSTRING)) {
		item_field_text = lua_tostring(lstate, -1);
		strncopy(field->text, FMT_TEXT_SIZE, (const unsigned char *) item_field_text);
	}

	/* load the color [optional] */
	if(!lua_get_field(lstate, field_table, "color", LUA_TSTRING)) {
		item_field_text = lua_tolstring(lstate, -1, &length);

		/* validate the color */
		if(length != 6)
			exit_func_safe("'color' for %s type must consist of 6 hexadecimal characters", item_field_type);

		strncopy(field->color, FMT_COLOR_SIZE, (const unsigned char *) item_field_text);
	}	

	/* pop everything except the field table */
	lua_pop(lstate, lua_gettop(lstate) - field_table);
	return CHECK_PASSED;
}

int load_weapon_type(format_rule_t * rule, lua_State * lstate, int rule_table) {
	int weapon_type = 0;
	int weapon_type_table = 0;

	/* push the weapon type table */
	if(lua_get_field(lstate, rule_table, "weapon_type", LUA_TTABLE)) {
		rule->weapon_type = 0xFFFFFFFF;
		return CHECK_PASSED;
	}
	weapon_type_table = lua_gettop(lstate);
	
	/* initialize the weapon type filter */
	rule->weapon_type = 0;

	/* set each weapon type flag */
	lua_pushnil(lstate);
	while(lua_next(lstate, weapon_type_table)) {
		if(lua_isnil(lstate, -1)) {
			exit_abt_safe("invalid weapon type table with nil value");
			break;
		} else if(!lua_isnumber(lstate, -1)) {
			exit_abt_safe("weapon type table must have must be integer value");
			break;
		}
		weapon_type = lua_tointeger(lstate, -1);
		rule->weapon_type |= weapon_flag(weapon_type);
		lua_pop(lstate, lua_gettop(lstate) - weapon_type_table - 1);
	}

	/* pop the weapon table and last key */
	lua_pop(lstate, lua_gettop(lstate) - rule_table);
	return CHECK_PASSED;
}

int lua_get_field(lua_State * state, int table, const char * key, int expected_type) {
	int value_type = 0;
	if(exit_null_safe(2, state, key)) 
		return CHECK_FAILED;
	if(lua_gettop(state) < table) {
		exit_func_safe("invalid table index %d", table);
		return CHECK_FAILED;
	}

	/* push the value on the stack */
	lua_getfield(state, table, key);
	if(lua_isnil(state, -1)) {
		lua_pop(state, 1);
		return CHECK_FAILED;
	}

	/* check the type of the value */
	value_type = lua_type(state, -1);
	if(value_type != expected_type) {
		exit_func_safe("expected '%s' type but got '%s' type on key %s",
		lua_typename(state, expected_type), lua_typename(state, value_type), key);
		lua_pop(state, 1);
		return CHECK_FAILED;
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
		format_type_iter = format->rule_list[i];
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

int write_item(FILE * file, format_t * format, item_t * item, char * script) {
	format_rule_t * item_rule = NULL;
	format_field_t * item_field = NULL;

	/* rathena uses weapon(5) and armor(4), but eathena and hercules
	 * uses weapon(4) and  armor(5), normalize to one item type set */
	if(format->server_type == MODE_RATHENA) {
		if(item->type == WEAPON_ITEM_TYPE)
			item->type = ARMOR_ITEM_TYPE;
		else if(item->type == ARMOR_ITEM_TYPE)
			item->type = WEAPON_ITEM_TYPE;
	}

	/* check item type is valid */
	if(item->type < 0 || item->type > ITEM_TYPE_SIZE) {
		exit_func_safe("invalid item type %d", item->type);
		return CHECK_FAILED;
	}

	/* find a matching item rule per item type  */
	item_rule = format->rule_list[item->type];
	while(item_rule != NULL) {
		/* filter by item id */
		if(!searchrange(item_rule->item_id, item->id)) {
			item_rule = item_rule->next;
			continue;
		}
		/* filter by weapon type */
		if(item->type == WEAPON_ITEM_TYPE) {
			if(item_rule->weapon_type & weapon_flag(item->view)) break;
			item_rule = item_rule->next;
			continue;
		}
		break;
	}

	/* check whether an item rule is found */
	if(item_rule == NULL) return CHECK_FAILED;

	/* write item based on format */
	item_field = item_rule->format;
	if(format->file_format & FORMAT_TXT)
		return write_item_text(file, format, item_field, item, script);
	else if(format->file_format & FORMAT_LUA)
		return write_item_lua(file, format, item_field, item, script);
	else 
		exit_func_safe("invalid item format %d", format->file_format);

	return CHECK_PASSED;
}

int write_item_text(FILE * file, format_t * format, format_field_t * field, item_t * item, char * script) {
	char buffer[FMT_BUF_SIZE];
	int offset = 0;
	flavour_text_t flavour;
	buffer[0] = '\0';

	/* write text format header */
	fprintf(file,"%d#\n", item->id);
	
	while(field != NULL) {
		switch(field->field) {
			case FLAVOUR_ITEM_FIELD: 		format_flavour_text(buffer, &offset, format, field, &flavour, item->id); 			break;
			case SCRIPT_ITEM_FIELD:			format_script(buffer, &offset, field, script); 										break;
			case TYPE_ITEM_FIELD:			format_type(buffer, &offset, field, item->type);									break;
			case BUY_ITEM_FIELD:			format_integer(buffer, &offset, field, item->buy); 									break;
			case SELL_ITEM_FIELD:			format_integer(buffer, &offset, field, item->sell); 								break;
			case WEIGHT_ITEM_FIELD: 		format_weight(buffer, &offset, field, item->weight); 								break;
			case ATK_ITEM_FIELD:			format_integer(buffer, &offset, field, item->atk); 									break;
			case DEF_ITEM_FIELD:			format_integer(buffer, &offset, field, item->def); 									break;
			case RANGE_ITEM_FIELD:			format_integer(buffer, &offset, field, item->range); 								break;
			case JOB_ITEM_FIELD:			format_job(buffer, &offset, field, item->job, item->upper, item->gender);			break;
			case GENDER_ITEM_FIELD:			format_gender(buffer, &offset, field, item->gender);								break;
			case LOC_ITEM_FIELD:			format_location(buffer, &offset, field, item->loc);									break;
			case WEAPON_LEVEL_ITEM_FIELD:	format_integer(buffer, &offset, field, item->wlv); 									break;
			case LEVEL_REQUIRE_ITEM_FIELD:	format_integer(buffer, &offset, field, item->elv_min); 								break;
			case REFINE_ABILITY_ITEM_FIELD:	format_refinement(buffer, &offset, field, item->refineable, format->server_type);	break;
			case VIEW_ITEM_FIELD:			format_view(buffer, &offset, field, item->view, item->type);						break;
			case UPPER_ITEM_FIELD:			format_upper(buffer, &offset, field, item->upper, format->server_type);				break;
			case MATK_ITEM_FIELD:			format_integer(buffer, &offset, field, item->matk); 								break;
			case BINDONEQUIP_ITEM_FIELD:	format_boolean(buffer, &offset, field, item->bindonequip); 							break;
			case BUYINGSTORE_ITEM_FIELD:	format_boolean(buffer, &offset, field, item->buyingstore); 							break;
			case DELAY_ITEM_FIELD:			format_delay(buffer, &offset, field, item->delay); 									break;
			case TRADE_ITEM_FIELD:			format_trade(buffer, &offset, field, item->trade);									break;
			case STACK_ITEM_FIELD:			format_stack(buffer, &offset, field, item->stack);									break;
			default: break;
		}
		field = field->next;
	}

	/* write the text format content and footer */
	(offset > 0) ?
		fprintf(file, "%s#\n", buffer):
		fprintf(file, "#\n");

	return CHECK_PASSED;
}

int write_item_lua(FILE * file, format_t * format, format_field_t * field, item_t * item, char * script) {
	
	return CHECK_PASSED;
}

int format_flavour_text(char * buffer, int * offset, format_t * format, format_field_t * field, flavour_text_t * flavour, int item_id) {
	int i = 0;
	char * flavor = NULL;			/* reference the item description name */
	int length = 0;					/* length of the item description name */
	char * string[FMT_LINE_SIZE];	/* parsed strings separated by a peroid */
	int count = 0;					/* number of parsed strings */

	/* search flavour text using item id */
	if(flavour_text_id_search(format, flavour, item_id) != CHECK_PASSED) {
		/*fprintf(stderr,"[warn]: failed to search for flavour text for item id %d.\n", item_id);*/
		return CHECK_FAILED;
	}
	
	/* preprocess the flavour text buffer */
	flavor = flavour->identified_description_name;
	length = strlen(flavor);
	if(length == 0) {
		/*fprintf(stderr,"[warn]: empty flavour text for item id %d\n", item_id);*/
		return CHECK_FAILED;
	}
	
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
		format_field_integer(buffer, offset, field, value);
	return CHECK_PASSED;
}

int format_refinement(char * buffer, int * offset, format_field_t * field, int value, int server_type) {
	if(server_type != MODE_HERCULES) {
		if(value == 0)
			if(field->text[0] != '\0')
				*offset += sprintf(&buffer[*offset], "%s\n", field->text);
	} else {
		if(value != 1)
			if(field->text[0] != '\0')
				*offset += sprintf(&buffer[*offset], "%s\n", field->text);
	}
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
	format_field_string(buffer, offset, field, gender);
	return CHECK_PASSED;
}

int format_view(char * buffer, int * offset, format_field_t * field, int view, int type) {
	char * view_type = NULL;

	/* view is only interpreted for weapon and ammo type */
	if(type != WEAPON_ITEM_TYPE && type != AMMO_ITEM_TYPE) 
		return CHECK_FAILED;

	/* map the weapon type constant to weapon type string */
	if(type == WEAPON_ITEM_TYPE) {
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
	format_field_string(buffer, offset, field, view_type);
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
	format_field_string(buffer, offset, field, item_type);
	return CHECK_PASSED;
}

int format_job(char * buffer, int * offset, format_field_t * field, int job, int upper, int gender) {
	char job_list[BUF_SIZE];
	int job_offset = 0;
	job_list[0] = '\0';

	/* check the common classes */
	if(job == 0xFFFFFFFF) {
		format_field_string(buffer, offset, field, "Every Job");
		return CHECK_PASSED;
	} else if(job == 0xFFFFFFFE || job == 0x7FFFFFFE) {
		format_field_string(buffer, offset, field, "Every Job except Novice");
		return CHECK_PASSED;
	}

	/* simplified the combination explosion */
	if(job & 0x00000001) job_offset += sprintf(&job_list[job_offset], "%sNovice", (job_offset != 0)?", ":"");
	if(job & 0x00004082) {
		job_offset += sprintf(&job_list[job_offset], "%sSwordsman Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x00000002) job_offset += sprintf(&job_list[job_offset], "%sSwordsman", (job_offset != 0)?", ":"");
		if(job & 0x00000080) job_offset += sprintf(&job_list[job_offset], "%sKnight", (job_offset != 0)?", ":"");
		if(job & 0x00004000) job_offset += sprintf(&job_list[job_offset], "%sCrusader", (job_offset != 0)?", ":"");
	}

	if(job & 0x00010204) {
		job_offset += sprintf(&job_list[job_offset], "%sMagician Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x00000004) job_offset += sprintf(&job_list[job_offset], "%sMagician", (job_offset != 0)?", ":"");
		if(job & 0x00000200) job_offset += sprintf(&job_list[job_offset], "%sWizard", (job_offset != 0)?", ":"");
		if(job & 0x00010000) job_offset += sprintf(&job_list[job_offset], "%sSage", (job_offset != 0)?", ":"");
	}

	if(job & 0x00080808) {
		job_offset += sprintf(&job_list[job_offset], "%sArcher Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x00000008) job_offset += sprintf(&job_list[job_offset], "%sArcher", (job_offset != 0)?", ":"");
		if(job & 0x00000800) job_offset += sprintf(&job_list[job_offset], "%sHunter", (job_offset != 0)?", ":"");
		if(job & 0x00080000 && gender == 0) job_offset += sprintf(&job_list[job_offset], "%sDancer", (job_offset != 0)?", ":"");
		if(job & 0x00080000 && gender == 1) job_offset += sprintf(&job_list[job_offset], "%sBard", (job_offset != 0)?", ":"");
	}
	if(job & 0x00008110) {
		job_offset += sprintf(&job_list[job_offset], "%sAcolyte Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x00000010) job_offset += sprintf(&job_list[job_offset], "%sAcolyte", (job_offset != 0)?", ":"");
		if(job & 0x00000100) job_offset += sprintf(&job_list[job_offset], "%sPriest", (job_offset != 0)?", ":"");
		if(job & 0x00008000) job_offset += sprintf(&job_list[job_offset], "%sMonk", (job_offset != 0)?", ":"");
	}

	if(job & 0x00040420) {
		job_offset += sprintf(&job_list[job_offset], "%sMerchant Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x00000020) job_offset += sprintf(&job_list[job_offset], "%sMerchant", (job_offset != 0)?", ":"");
		if(job & 0x00000400) job_offset += sprintf(&job_list[job_offset], "%sBlacksmith", (job_offset != 0)?", ":"");
		if(job & 0x00040000) job_offset += sprintf(&job_list[job_offset], "%sAlchemist", (job_offset != 0)?", ":"");
	}

	if(job & 0x00021040) {
		job_offset += sprintf(&job_list[job_offset], "%sThief Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x00000040) job_offset += sprintf(&job_list[job_offset], "%sThief", (job_offset != 0)?", ":"");
		if(job & 0x00001000) job_offset += sprintf(&job_list[job_offset], "%sAssassin", (job_offset != 0)?", ":"");
		if(job & 0x00020000) job_offset += sprintf(&job_list[job_offset], "%sRogue", (job_offset != 0)?", ":"");
	}

	if(job & 0x22000000) {
		job_offset += sprintf(&job_list[job_offset], "%sNinja Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x02000000) job_offset += sprintf(&job_list[job_offset], "%sNinja", (job_offset != 0)?", ":"");
		if(job & 0x20000000) job_offset += sprintf(&job_list[job_offset], "%sKagerou & Oboro", (job_offset != 0)?", ":"");
	}

	if(job & 0x41000000) {
		job_offset += sprintf(&job_list[job_offset], "%sGunslinger Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x01000000) job_offset += sprintf(&job_list[job_offset], "%sGunslinger", (job_offset != 0)?", ":"");
		if(job & 0x40000000) job_offset += sprintf(&job_list[job_offset], "%sRebellion", (job_offset != 0)?", ":"");
	}

	if(job & 0x00E00000) {
		job_offset += sprintf(&job_list[job_offset], "%sTaekwon Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x00200000) job_offset += sprintf(&job_list[job_offset], "%sTaekwon", (job_offset != 0)?", ":"");
		if(job & 0x00400000) job_offset += sprintf(&job_list[job_offset], "%sStar Gladiator", (job_offset != 0)?", ":"");
		if(job & 0x00800000) job_offset += sprintf(&job_list[job_offset], "%sSoul Linker", (job_offset != 0)?", ":"");
	}

	if(job & 0x1C000000) {
		job_offset += sprintf(&job_list[job_offset], "%sGangsi Class", (job_offset != 0)?", ":"");
	} else {
		if(job & 0x04000000) job_offset += sprintf(&job_list[job_offset], "%sGangsi", (job_offset != 0)?", ":"");
		if(job & 0x08000000) job_offset += sprintf(&job_list[job_offset], "%sDeath Knight", (job_offset != 0)?", ":"");
		if(job & 0x10000000) job_offset += sprintf(&job_list[job_offset], "%sDark Collector", (job_offset != 0)?", ":"");
	}
	format_field_string(buffer, offset, field, job_list);
	return CHECK_PASSED;
}

int format_upper(char * buffer, int * offset, format_field_t * field, int upper, int server_type) {
	char tier_list[BUF_SIZE];
	int tier_offset = 0;
	tier_list[0] = '\0';

	/* check common groups */
	if((server_type == MODE_HERCULES || server_type == MODE_RATHENA)) {
		if(upper == 0x3F) {
			format_field_string(buffer, offset, field, "Every Tier");
			return CHECK_PASSED;
		} else if(upper == 0x1C) {
			format_field_string(buffer, offset, field, "Every Tier except Baby Tier");
			return CHECK_PASSED;
		}
	} else if(server_type == MODE_EATHENA) {
		if(upper == 0x07) {
			format_field_string(buffer, offset, field, "Every Tier");
			return CHECK_PASSED;
		} else if(upper == 0x03) {
			format_field_string(buffer, offset, field, "Every Tier except Baby Tier");
			return CHECK_PASSED;
		}
	}

	if(upper & 0x01) tier_offset += sprintf(&tier_list[tier_offset], "%sFirst & Second Job", (tier_offset != 0)?", ":"");
	if(upper & 0x02) tier_offset += sprintf(&tier_list[tier_offset], "%sRebirth First & Second Job", (tier_offset != 0)?", ":"");
	if(upper & 0x08) tier_offset += sprintf(&tier_list[tier_offset], "%sThird Job", (tier_offset != 0)?", ":"");
	if(upper & 0x10) tier_offset += sprintf(&tier_list[tier_offset], "%sRebirth Third Job", (tier_offset != 0)?", ":"");
	if(upper & 0x04) tier_offset += sprintf(&tier_list[tier_offset], "%sSecond Baby Job", (tier_offset != 0)?", ":"");
	if(upper & 0x20) tier_offset += sprintf(&tier_list[tier_offset], "%sThird Baby Job", (tier_offset != 0)?", ":"");
	format_field_string(buffer, offset, field, tier_list);
	return CHECK_PASSED;
}

int format_location(char * buffer, int * offset, format_field_t * field, int loc) {
	char loc_list[BUF_SIZE];
	int loc_offset = 0;
	loc_list[0] = '\0';

	if(loc & 0x000301) {
		loc_offset += sprintf(&loc_list[loc_offset], "%sHeadgear", (loc_offset != 0)?", ":"");
	} else {
		if(loc & 0x000100) loc_offset += sprintf(&loc_list[loc_offset], "%sUpper Headgear", (loc_offset != 0)?", ":"");
		if(loc & 0x000200) loc_offset += sprintf(&loc_list[loc_offset], "%sMiddle Headgear", (loc_offset != 0)?", ":"");
		if(loc & 0x000001) loc_offset += sprintf(&loc_list[loc_offset], "%sLower Headgear", (loc_offset != 0)?", ":"");
	}
	if(loc & 0x000010) loc_offset += sprintf(&loc_list[loc_offset], "%sArmor", (loc_offset != 0)?", ":"");
	if(loc & 0x000002) loc_offset += sprintf(&loc_list[loc_offset], "%sWeapon", (loc_offset != 0)?", ":"");
	if(loc & 0x000020) loc_offset += sprintf(&loc_list[loc_offset], "%sShield", (loc_offset != 0)?", ":"");
	if(loc & 0x000004) loc_offset += sprintf(&loc_list[loc_offset], "%sGarment", (loc_offset != 0)?", ":"");
	if(loc & 0x000040) loc_offset += sprintf(&loc_list[loc_offset], "%sShoes", (loc_offset != 0)?", ":"");
	if(loc & 0x000088) {
		loc_offset += sprintf(&loc_list[loc_offset], "%sAccessory", (loc_offset != 0)?", ":"");
	} else {
		if(loc & 0x000008) loc_offset += sprintf(&loc_list[loc_offset], "%sRight Accessory", (loc_offset != 0)?", ":"");
		if(loc & 0x000080) loc_offset += sprintf(&loc_list[loc_offset], "%sLeft Accessory", (loc_offset != 0)?", ":"");
	}
	if(loc & 0x001C00) {
		loc_offset += sprintf(&loc_list[loc_offset], "%sCostume Headgear", (loc_offset != 0)?", ":"");
	} else {
		if(loc & 0x000400) loc_offset += sprintf(&loc_list[loc_offset], "%sCostume Upper Headgear", (loc_offset != 0)?", ":"");
		if(loc & 0x000800) loc_offset += sprintf(&loc_list[loc_offset], "%sCostume Middle Headgear", (loc_offset != 0)?", ":"");
		if(loc & 0x001000) loc_offset += sprintf(&loc_list[loc_offset], "%sCostume Lower Headgear", (loc_offset != 0)?", ":"");
	}

	if(loc & 0x002000) loc_offset += sprintf(&loc_list[loc_offset], "%sCostume Garment", (loc_offset != 0)?", ":"");
	if(loc & 0x008000) loc_offset += sprintf(&loc_list[loc_offset], "%sAmmo", (loc_offset != 0)?", ":"");
	if(loc & 0x010000) loc_offset += sprintf(&loc_list[loc_offset], "%sShadow Armor", (loc_offset != 0)?", ":"");
	if(loc & 0x020000) loc_offset += sprintf(&loc_list[loc_offset], "%sShadow Weapon", (loc_offset != 0)?", ":"");
	if(loc & 0x040000) loc_offset += sprintf(&loc_list[loc_offset], "%sShadow Shield", (loc_offset != 0)?", ":"");
	if(loc & 0x080000) loc_offset += sprintf(&loc_list[loc_offset], "%sShadow Shoes", (loc_offset != 0)?", ":"");
	if(loc & 0x300000) {
		loc_offset += sprintf(&loc_list[loc_offset], "%sShadow Accessory", (loc_offset != 0)?", ":"");
	} else {
		if(loc & 0x100000) loc_offset += sprintf(&loc_list[loc_offset], "%sShadow Right Accessory", (loc_offset != 0)?", ":"");
		if(loc & 0x200000) loc_offset += sprintf(&loc_list[loc_offset], "%sShadow Left Accessory", (loc_offset != 0)?", ":"");
	}

	format_field_string(buffer, offset, field, loc_list);
	return CHECK_PASSED;
}

int format_weight(char * buffer, int * offset, format_field_t * field, int weight) {
	if(weight == 0) return CHECK_FAILED;	
	if(weight / 10 == 0) {
		format_field_double(buffer, offset, field, ((double) weight) / 10);
	} else {
		format_integer(buffer, offset, field, weight / 10);
	}
	return CHECK_PASSED;
}

int format_boolean(char * buffer, int * offset, format_field_t * field, int value) {
	if(value > 0)
		if(field->text[0] != '\0')
			*offset += sprintf(&buffer[*offset], "%s\n", field->text);
	return CHECK_PASSED;
}

int format_delay(char * buffer, int * offset, format_field_t * field, int delay) {
	char unit[BUF_SIZE];
	int unit_offset = 0;
	unit[0] = '\0';
	if((delay / 1000) == 0) {
		if(delay > 0) {
			unit_offset = sprintf(unit, "%d milliseconds", delay);
			format_field_integer(buffer, offset, field, delay);
		}
	} else {
		unit_offset = sprintf(unit, "%d seconds", delay / 1000);
		format_field_integer(buffer, offset, field, delay);
	}
	return CHECK_PASSED;
}

int format_trade(char * buffer, int * offset, format_field_t * field, int * trade) {
	char trade_list[BUF_SIZE];
	int trade_offset = 0;
	trade_list[0] = '\0';

	if(trade[TRADE_NODROP] > 0 || trade[TRADE_NOTRADE] > 0 || 
	   trade[TRADE_NOAUCTION] > 0 || trade[TRADE_NOSELLTONPC] > 0 || 
	   trade[TRADE_NOMAIL] > 0) {
		trade_offset = 0;
		trade_offset += sprintf(&trade_list[trade_offset], "Item cannot be ");
		if(trade[TRADE_NODROP] > 0) 		trade_offset += sprintf(&trade_list[trade_offset], "%sdropped", (trade_offset != 0)?", ":"");
		if(trade[TRADE_NOTRADE] > 0) 		trade_offset += sprintf(&trade_list[trade_offset], "%straded, vended", (trade_offset != 0)?", ":"");
		if(trade[TRADE_NOAUCTION] > 0) 		trade_offset += sprintf(&trade_list[trade_offset], "%sauctioned", (trade_offset != 0)?", ":"");
		if(trade[TRADE_NOSELLTONPC] > 0) 	trade_offset += sprintf(&trade_list[trade_offset], "%ssold", (trade_offset != 0)?", ":"");
		if(trade[TRADE_NOMAIL] > 0) 		trade_offset += sprintf(&trade_list[trade_offset], "%smailed", (trade_offset != 0)?", ":"");
		format_field_string(buffer, offset, field, trade_list);
	}


	if(trade[TRADE_NOCART] > 0 || trade[TRADE_NOSTORAGE] > 0 || trade[TRADE_NOGSTORAGE] > 0) {
		trade_offset = 0;
		trade_offset += sprintf(&trade_list[trade_offset], "Item cannot be stored in ");
		if(trade[TRADE_NOCART] > 0) 			trade_offset += sprintf(&trade_list[trade_offset], "%scart", (trade_offset != 0)?", ":"");
		if(trade[TRADE_NOSTORAGE] > 0) 			trade_offset += sprintf(&trade_list[trade_offset], "%sstorage", (trade_offset != 0)?", ":"");
		if(trade[TRADE_NOGSTORAGE] > 0) 		trade_offset += sprintf(&trade_list[trade_offset], "%sguild storage", (trade_offset != 0)?", ":"");
		format_field_string(buffer, offset, field, trade_list);	
	}

	if(trade[TRADE_PARTNEROVERRIDE] > 0)
		format_field_string(buffer, offset, field, "Item shared between partners.");
	return CHECK_PASSED;
}

int format_stack(char * buffer, int * offset, format_field_t * field, int * stack) {
	int type = stack[STACK_TYPE];
	int amount = stack[STACK_AMOUNT];
	int initial_offset = 0;

	/* zero type or amount means no stack restriction */
	if(amount == 0 || type == 0) return CHECK_FAILED;

	/* write the text */
	*offset += sprintf(&buffer[*offset], "Item Stack Restriction: ");
	if(field->color[0] != '\0') *offset += sprintf(&buffer[*offset], "^%s", field->color);
	initial_offset = *offset;
	if(type & 0x1) *offset += sprintf(&buffer[*offset], "%sinventory", (initial_offset < *offset)?", ":"");
	if(type & 0x2) *offset += sprintf(&buffer[*offset], "%scart", (initial_offset < *offset)?", ":"");
	if(type & 0x3) *offset += sprintf(&buffer[*offset], "%sstorage", (initial_offset < *offset)?", ":"");
	if(type & 0x4) *offset += sprintf(&buffer[*offset], "%sguild storage", (initial_offset < *offset)?", ":"");
	if(field->color[0] != '\0') {
		*offset += sprintf(&buffer[*offset], ".^%s\nItem Stack Limit: ^%s%d^%s\n", DEFAULT_COLOR, field->color, amount, DEFAULT_COLOR);
	} else {
		*offset += sprintf(&buffer[*offset], ".\nItem Stack Limit: %d\n", amount);
	}
	return CHECK_PASSED;
}

int format_field_string(char * buffer, int * offset, format_field_t * field, char * string) {
	if(field->text[0] != '\0' && field->color[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "%s^%s%s^%s\n", field->text, field->color, string, DEFAULT_COLOR);
	} else if(field->text[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "%s%s\n", field->text, string);
	} else if(field->color[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "^%s%s^%s\n", field->color, string, DEFAULT_COLOR);
	} else {
		*offset += sprintf(&buffer[*offset], "%s\n", string);
	}
	return CHECK_PASSED;
}

int format_field_double(char * buffer, int * offset, format_field_t * field, double value) {
	if(field->text[0] != '\0' && field->color[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "%s^%s%.2f^%s\n", field->text, field->color, value, DEFAULT_COLOR);
	} else if(field->text[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "%s%.2f\n", field->text, value);
	} else if(field->color[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "^%s%.2f^%s\n", field->color, value, DEFAULT_COLOR);
	} else {
		*offset += sprintf(&buffer[*offset], "%.2f\n", value);
	}
	return CHECK_PASSED;
}

int format_field_integer(char * buffer, int * offset, format_field_t * field, int value) {
	if(field->text[0] != '\0' && field->color[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "%s^%s%d^%s\n", field->text, field->color, value, DEFAULT_COLOR);
	} else if(field->text[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "%s%d\n", field->text, value);
	} else if(field->color[0] != '\0') {
		*offset += sprintf(&buffer[*offset], "^%s%d^%s\n", field->color, value, DEFAULT_COLOR);
	} else {
		*offset += sprintf(&buffer[*offset], "%d\n", value);
	}
	return CHECK_PASSED;
}

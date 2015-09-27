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
      case 0:   record->id = convert_integer(val, 10);                                  break;
      case 1:   strnload(record->aegis, MAX_NAME_SIZE, val);                            break;
      case 2:   strnload(record->eathena, MAX_NAME_SIZE, val);                          break;
      case 3:   record->type = convert_integer(val, 10);                                break;
      case 4:   record->buy = convert_integer(val, 10);                                 break;
      case 5:   record->sell = convert_integer(val, 10);                                break;
      case 6:   record->weight = convert_integer(val, 10);                              break;
      case 7:   convert_integer_delimit(val, ":", 2, &record->atk, &record->matk);      break;
      case 8:   record->def = convert_integer(val, 10);                                 break;
      case 9:   record->range = convert_integer(val, 10);                               break;
      case 10:  record->slots = convert_integer(val, 10);                               break;
      case 11:  record->job = convert_uinteger(val, 16);                                break;
      case 12:  record->upper = convert_integer(val, 10);                               break;
      case 13:  record->gender = convert_integer(val, 10);                              break;
      case 14:  record->loc = convert_integer(val, 10);                                 break;
      case 15:  record->wlv = convert_integer(val, 10);                                 break;
      case 16:  convert_integer_delimit(val, ":", 2, &record->elv, &record->elv_max);   break;
      case 17:  record->refineable = convert_integer(val, 10);                          break;
      case 18:  record->view = convert_integer(val, 10);                                break;
      case 19:  strnload(record->script, MAX_SCRIPT_SIZE, val);                         break;
      case 20:  strnload(record->onequip, MAX_SCRIPT_SIZE, val);                        break;
      case 21:  strnload(record->onunequip, MAX_SCRIPT_SIZE, val);                      break;
      default:  exit_func_safe("invalid column field %d in rathena item database.\n", col);
   }
   return 0;
}

int mob_ra_load(void * db, int row, int col, char * val) {
    mob_ra * record = &((mob_ra *) db)[row];
    switch(col) {
        case 0: record->id = convert_integer(val,10);               break;
        case 1: strnload(record->sprite, MAX_NAME_SIZE, val);       break;
        case 2: strnload(record->kro, MAX_NAME_SIZE, val);          break;
        case 3: strnload(record->iro, MAX_NAME_SIZE, val);          break;
        case 4: record->lv = convert_integer(val,10);               break;
        case 5: record->hp = convert_integer(val,10);               break;
        case 6: record->sp = convert_integer(val,10);               break;
        case 7: record->exp = convert_integer(val,10);              break;
        case 8: record->jexp = convert_integer(val,10);             break;
        case 9: record->range = convert_integer(val,10);            break;
        case 10: record->atk1 = convert_integer(val,10);            break;
        case 11: record->atk2 = convert_integer(val,10);            break;
        case 12: record->def = convert_integer(val,10);             break;
        case 13: record->mdef = convert_integer(val,10);            break;
        case 14: record->str = convert_integer(val,10);             break;
        case 15: record->agi = convert_integer(val,10);             break;
        case 16: record->vit = convert_integer(val,10);             break;
        case 17: record->intr = convert_integer(val,10);            break;
        case 18: record->dex = convert_integer(val,10);             break;
        case 19: record->luk = convert_integer(val,10);             break;
        case 20: record->range2 = convert_integer(val,10);          break;
        case 21: record->range3 = convert_integer(val,10);          break;
        case 22: record->scale = convert_integer(val,10);           break;
        case 23: record->race = convert_integer(val,10);            break;
        case 24: record->element = convert_integer(val,10);         break;
        case 25: record->mode = convert_uinteger(val,16);           break;
        case 26: record->speed = convert_integer(val,10);           break;
        case 27: record->adelay = convert_integer(val,10);          break;
        case 28: record->amotion = convert_integer(val,10);         break;
        case 29: record->dmotion = convert_integer(val,10);         break;
        case 30: record->mexp = convert_integer(val,10);            break;
        case 31: record->mvp1id = convert_integer(val,10);          break;
        case 32: record->mvp1per = convert_integer(val,10);         break;
        case 33: record->mvp2id = convert_integer(val,10);          break;
        case 34: record->mvp2per = convert_integer(val,10);         break;
        case 35: record->mvp3id = convert_integer(val,10);          break;
        case 36: record->mvp3per = convert_integer(val,10);         break;
        case 37: record->drop1id = convert_integer(val,10);         break;
        case 38: record->drop1per = convert_integer(val,10);        break;
        case 39: record->drop2id = convert_integer(val,10);         break;
        case 40: record->drop2per = convert_integer(val,10);        break;
        case 41: record->drop3id = convert_integer(val,10);         break;
        case 42: record->drop3per = convert_integer(val,10);        break;
        case 43: record->drop4id = convert_integer(val,10);         break;
        case 44: record->drop4per = convert_integer(val,10);        break;
        case 45: record->drop5id = convert_integer(val,10);         break;
        case 46: record->drop5per = convert_integer(val,10);        break;
        case 47: record->drop6id = convert_integer(val,10);         break;
        case 48: record->drop6per = convert_integer(val,10);        break;
        case 49: record->drop7id = convert_integer(val,10);         break;
        case 50: record->drop7per = convert_integer(val,10);        break;
        case 51: record->drop8id = convert_integer(val,10);         break;
        case 52: record->drop8per = convert_integer(val,10);        break;
        case 53: record->drop9id = convert_integer(val,10);         break;
        case 54: record->drop9per = convert_integer(val,10);        break;
        case 55: record->dropcardid = convert_integer(val,10);      break;
        case 56: record->dropcardper = convert_integer(val,10);     break;
        default: exit_func_safe("invalid column field %d in rathena mob database.\n", col);
    }
    return 0;
}

int skill_ra_load(void * db, int row, int col, char * val) {
    skill_ra * record = &((skill_ra *) db)[row];
    switch(col) {
        case 0: record->id = convert_integer(val,10);                       break;
        case 1: strnload(record->range.str, MAX_VARARG_SIZE, val);          break;
        case 2: record->hit = convert_integer(val,10);                      break;
        case 3: record->inf = convert_integer(val,10);                      break;
        case 4: strnload(record->element.str, MAX_VARARG_SIZE, val);        break;
        case 5: record->nk = convert_uinteger(val,16);                      break;
        case 6: strnload(record->splash.str, MAX_VARARG_SIZE, val);         break;
        case 7: record->maxlv = convert_integer(val,10);                    break;
        case 8: strnload(record->hit_amount.str, MAX_VARARG_SIZE, val);     break;
        case 9: strnload(record->cast_cancel, MAX_NAME_SIZE, val);          break;
        case 10: record->cast_def_reduce_rate = convert_integer(val,10);    break;
        case 11: record->inf2 = convert_uinteger(val,16);                   break;
        case 12: strnload(record->maxcount.str, MAX_VARARG_SIZE, val);      break;
        case 13: strnload(record->type, MAX_NAME_SIZE, val);                break;
        case 14: strnload(record->blow_count.str, MAX_VARARG_SIZE, val);    break;
        case 15: record->inf3 = convert_integer(val,16);                    break;
        case 16: strnload(record->name, MAX_NAME_SIZE, val);                break;
        case 17: strnload(record->desc, MAX_NAME_SIZE, val);                break;
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
        case 0: record->id = convert_integer(val, 10);              break;
        case 1: strnload(record->sprite, MAX_NAME_SIZE, val);       break;
        case 2: strnload(record->name, MAX_NAME_SIZE, val);         break;
        case 3: record->lv = convert_integer(val, 10);              break;
        case 4: record->hp = convert_integer(val, 10);              break;
        case 5: record->sp = convert_integer(val, 10);              break;
        case 6: record->range1 = convert_integer(val, 10);          break;
        case 7: record->atk1 = convert_integer(val, 10);            break;
        case 8: record->atk2 = convert_integer(val, 10);            break;
        case 9: record->def = convert_integer(val, 10);             break;
        case 10: record->mdef = convert_integer(val, 10);           break;
        case 11: record->str = convert_integer(val, 10);            break;
        case 12: record->agi = convert_integer(val, 10);            break;
        case 13: record->vit = convert_integer(val, 10);            break;
        case 14: record->intr = convert_integer(val, 10);           break;
        case 15: record->dex = convert_integer(val, 10);            break;
        case 16: record->luk = convert_integer(val, 10);            break;
        case 17: record->range2 = convert_integer(val, 10);         break;
        case 18: record->range3 = convert_integer(val, 10);         break;
        case 19: record->scale = convert_integer(val, 10);          break;
        case 20: record->race = convert_integer(val, 10);           break;
        case 21: record->element = convert_integer(val, 10);        break;
        case 22: record->speed = convert_integer(val, 10);          break;
        case 23: record->adelay = convert_integer(val, 10);         break;
        case 24: record->amotion = convert_integer(val, 10);        break;
        case 25: record->dmotion = convert_integer(val, 10);        break;
        default: exit_func_safe("invalid column field %d in rathena mercenary database.\n", col);
    }
    return 0;
}

int pet_ra_load(void * db, int row, int col, char * val) {
    pet_ra * record = &((pet_ra *) db)[row];
    switch(col) {
        case 0: record->mob_id = convert_integer(val,10);               break;
        case 1: strnload(record->name, MAX_NAME_SIZE, val);             break;
        case 2: strnload(record->jname, MAX_NAME_SIZE, val);            break;
        case 3: record->lure_id = convert_integer(val,10);              break;
        case 4: record->egg_id = convert_integer(val,10);               break;
        case 5: record->equip_id = convert_integer(val,10);             break;
        case 6: record->food_id = convert_integer(val,10);              break;
        case 7: record->fullness = convert_integer(val,10);             break;
        case 8: record->hungry_delay = convert_integer(val,10);         break;
        case 9: record->r_hungry = convert_integer(val,10);             break;
        case 10: record->r_full = convert_integer(val,10);              break;
        case 11: record->intimate = convert_integer(val,10);            break;
        case 12: record->die = convert_integer(val,10);                 break;
        case 13: record->capture = convert_integer(val,10);             break;
        case 14: record->speed = convert_integer(val,10);               break;
        case 15: record->s_performance = convert_integer(val,10);       break;
        case 16: record->talk_convert = convert_integer(val,10);        break;
        case 17: record->attack_rate = convert_integer(val,10);         break;
        case 18: record->defence_attack_rate = convert_integer(val,10); break;
        case 19: record->change_target_rate = convert_integer(val,10);  break;
        case 20: strnload(record->pet_script, MAX_SCRIPT_SIZE, val);    break;
        case 21: strnload(record->loyal_script, MAX_SCRIPT_SIZE, val);  break;
        default: exit_func_safe("invalid column field %d in rathena pet database.\n", col);
    }
    return 0;
}

int item_group_ra_load(void * db, int row, int col, char * val) {
    item_group_ra * record = &((item_group_ra *) db)[row];
    switch(col) {
        case 0: strnload(record->name, MAX_NAME_SIZE, val); break;
        case 1: record->item_id = convert_integer(val, 10); break;
        case 2: record->rate = convert_integer(val, 10);    break;
        default: exit_func_safe("invalid column field %d in rathena item group database.\n", col);
    }
    return 0;
}

int const_ra_load(void * db, int row, int col, char * val) {
    const_ra * record = &((const_ra *) db)[row];
    switch(col) {
        case 0: strnload(record->name, MAX_NAME_SIZE, val);                             break;
        case 1:
            /* constant can be represented as hexadecimal or decimal */
            record->value = (strlen(val) > 2 && val[0] == '0' && val[1] == 'x') ?
                convert_integer(val, 16):
                convert_integer(val, 10);                                               break;
        case 2: record->type = convert_integer(val, 10);                                break;
        default: exit_func_safe("invalid column field %d in rathena const database.\n", col);
    }
    return 0;
}

int package_ra_load(void * db, int row, int col, char * val) {
    package_ra * record = &((package_ra *) db)[row];
    switch(col) {
        case 0: strnload(record->group_name, MAX_NAME_SIZE, val);       break;
        case 1: strnload(record->item_name, MAX_NAME_SIZE, val);        break;
        case 2: record->rate = convert_integer(val, 10);                break;
        case 3: record->amount = convert_integer(val, 10);              break;
        case 4: record->random = convert_integer(val, 10);              break;
        case 5: record->announced = convert_integer(val, 10);           break;
        case 6: record->duration = convert_integer(val, 10);            break;
        case 7: record->guid = convert_integer(val, 10);                break;
        case 8: record->bound = convert_integer(val, 10);               break;
        case 9: record->named = convert_integer(val, 10);               break;
        default: exit_func_safe("invalid column field %d in rathena package database.\n", col);
    }
    return 0;
}

int combo_ra_load(void * db, int row, int col, char * val) {
    combo_ra * record = &((combo_ra *) db)[row];
    switch(col) {
        case 0: strnload(record->item_id.str, MAX_VARARG_SIZE, val);    break;
        case 1: strnload(record->script, MAX_SCRIPT_SIZE, val);         break;
        default: break;
    }
    return 0;
}

int ra_db_init(ra_db_t ** ra, const char * path) {
    char * error = NULL;
    ra_db_t * _ra = NULL;

    exit_null_safe(2, ra, path);

    _ra = calloc(1, sizeof(ra_db_t));
    if(NULL == _ra)
        goto failed;

    if(SQLITE_OK != sqlite3_open_v2(path, &_ra->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL)) {
        if(NULL != _ra->db)
            fprintf(stderr, "[load]: failed to create %s; %s.\n", path, sqlite3_errmsg(_ra->db));
        goto failed;
    }

    if( SQLITE_OK != sqlite3_exec(_ra->db,
        RA_ITEM_DELETE
        RA_ITEM_CREATE
        RA_MOB_DELETE
        RA_MOB_CREATE
        RA_SKILL_DELETE
        RA_SKILL_CREATE
        RA_PRODUCE_DELETE
        RA_PRODUCE_CREATE
        RA_MERC_DELETE
        RA_MERC_CREATE
        RA_PET_DELETE
        RA_PET_CREATE
        RA_CONST_DELETE
        RA_CONST_CREATE
        RA_ITEM_PACKAGE_DELETE
        RA_ITEM_PACKAGE_CREATE
        RA_ITEM_COMBO_DELETE
        RA_ITEM_COMBO_CREATE
        RA_ITEM_PACKAGE_META_DELETE
        RA_ITEM_PACKAGE_META_CREATE,
        NULL, NULL, &error)) {
        if(NULL != error) {
            fprintf(stderr, "[load]: failed to load schema %s; %s.\n", path, error);
            sqlite3_free(error);
        }
        goto failed;
    }

    if( SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_INSERT, strlen(RA_ITEM_INSERT), &_ra->item_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_MOB_INSERT, strlen(RA_MOB_INSERT), &_ra->mob_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_SKILL_INSERT, strlen(RA_SKILL_INSERT), &_ra->skill_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_PRODUCE_INSERT, strlen(RA_PRODUCE_INSERT), &_ra->produce_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_MERC_INSERT, strlen(RA_MERC_INSERT), &_ra->mercenary_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_PET_INSERT, strlen(RA_PET_INSERT), &_ra->pet_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_GROUP_INSERT, strlen(RA_ITEM_GROUP_INSERT), &_ra->item_group_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_CONST_INSERT, strlen(RA_CONST_INSERT), &_ra->const_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_COMBO_INSERT, strlen(RA_ITEM_COMBO_INSERT), &_ra->item_combo_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_CONST_SEARCH, strlen(RA_CONST_SEARCH), &_ra->const_ra_name_search, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_SEARCH_NAME, strlen(RA_ITEM_SEARCH_NAME), &_ra->item_ra_name_search, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_SEARCH_ID, strlen(RA_ITEM_SEARCH_ID), &_ra->item_ra_id_search, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_PACKAGE_INSERT, strlen(RA_ITEM_PACKAGE_INSERT), &_ra->item_package_ra_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_PACKAGE_META_INSERT, strlen(RA_ITEM_PACKAGE_META_INSERT), &_ra->item_package_meta_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ra->db, RA_ITEM_PACKAGE_QUERY, strlen(RA_ITEM_PACKAGE_QUERY), &_ra->item_package_query, NULL)) {
        fprintf(stderr, "[load]: failed prepare sql statments; %s.\n", sqlite3_errmsg(_ra->db));
        goto failed;
    }

    *ra = _ra;
    return CHECK_PASSED;

failed:
    ra_db_deit(&_ra);
    return CHECK_FAILED;
}

int ra_db_deit(ra_db_t ** ra) {
    ra_db_t * _ra = NULL;

    exit_null_safe(2, ra, *ra);

    _ra = *ra;
    if( SQLITE_OK != sqlite3_finalize(_ra->item_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->mob_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->skill_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->produce_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->mercenary_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->pet_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->item_group_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->const_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->item_combo_ra_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->const_ra_name_search) ||
        SQLITE_OK != sqlite3_finalize(_ra->item_ra_name_search) ||
        SQLITE_OK != sqlite3_finalize(_ra->item_ra_id_search) ||
        SQLITE_OK != sqlite3_finalize(_ra->item_package_meta_insert) ||
        SQLITE_OK != sqlite3_finalize(_ra->item_package_query) ||
        SQLITE_OK != sqlite3_close(_ra->db)) {
        exit(EXIT_FAILURE);
    }
    SAFE_FREE(_ra);
    *ra = NULL;
    return CHECK_PASSED;
}

int ra_db_exec(ra_db_t * db, char * sql) {
    char * error = NULL;

    if (SQLITE_OK != sqlite3_exec(db->db, sql, NULL, NULL, &error)) {
        if(NULL != error) {
            fprintf(stderr, "[load]: failed to execute sql query %s; %s.\n", sql, error);
            sqlite3_free(error);
        }
        return CHECK_FAILED;
    }

    return CHECK_PASSED;
}

int ra_db_item_load(ra_db_t * ra, const char * path) {
    native_t items;
    items.db = NULL;
    items.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_numeric, load_native_general, &items, &load_ra_native[0]) ||
        NULL == items.db ||
        0 >= items.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_item_load_record(items.db, items.size, ra->item_ra_sql_insert) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load item db; %s.\n", path);
        SAFE_FREE(items.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded item db; %s.\n", path);
    SAFE_FREE(items.db);
    return CHECK_PASSED;
}

int ra_db_item_load_record(item_ra * items, int size, sqlite3_stmt * sql) {
    int i = 0;
    item_ra * item = NULL;

    for(i = 0; i < size; i++) {
        item = &items[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
            SQLITE_OK != sqlite3_bind_int(sql, 1, item->id) ||
            SQLITE_OK != sqlite3_bind_text(sql, 2, item->aegis, strlen(item->aegis), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_text(sql, 3, item->eathena, strlen(item->eathena), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_int(sql, 4, item->type) ||
            SQLITE_OK != sqlite3_bind_int(sql, 5, item->buy) ||
            SQLITE_OK != sqlite3_bind_int(sql, 6, item->sell) ||
            SQLITE_OK != sqlite3_bind_int(sql, 7, item->weight) ||
            SQLITE_OK != sqlite3_bind_int(sql, 8, item->matk) ||
            SQLITE_OK != sqlite3_bind_int(sql, 9, item->atk) ||
            SQLITE_OK != sqlite3_bind_int(sql, 10, item->def) ||
            SQLITE_OK != sqlite3_bind_int(sql, 11, item->range) ||
            SQLITE_OK != sqlite3_bind_int(sql, 12, item->slots) ||
            SQLITE_OK != sqlite3_bind_int(sql, 13, item->job) ||
            SQLITE_OK != sqlite3_bind_int(sql, 14, item->upper) ||
            SQLITE_OK != sqlite3_bind_int(sql, 15, item->gender) ||
            SQLITE_OK != sqlite3_bind_int(sql, 16, item->loc) ||
            SQLITE_OK != sqlite3_bind_int(sql, 17, item->wlv) ||
            SQLITE_OK != sqlite3_bind_int(sql, 18, item->elv) ||
            SQLITE_OK != sqlite3_bind_int(sql,19, item->elv_max) ||
            SQLITE_OK != sqlite3_bind_int(sql, 20, item->refineable) ||
            SQLITE_OK != sqlite3_bind_int(sql, 21, item->view) ||
            SQLITE_OK != sqlite3_bind_text(sql, 22, item->script, strlen(item->script), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_text(sql, 23, item->onequip, strlen(item->onequip), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_text(sql, 24, item->onunequip, strlen(item->onunequip), SQLITE_STATIC) ||
            SQLITE_DONE != sqlite3_step(sql) ||
            SQLITE_OK != sqlite3_reset(sql)) {
            fprintf(stderr, "[load]: failed to add %s to item db.\n", item->aegis);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }

        }
    }
    return CHECK_PASSED;
}

int ra_db_mob_load(ra_db_t * ra, const char * path) {
    native_t mobs;
    mobs.db = NULL;
    mobs.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_numeric, load_native_general, &mobs, &load_ra_native[1]) ||
        NULL == mobs.db ||
        0 >= mobs.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_mob_load_record(mobs.db, mobs.size, ra->mob_ra_sql_insert) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load mob db; %s.\n", path);
        SAFE_FREE(mobs.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded mob db; %s.\n", path);
    SAFE_FREE(mobs.db);
    return CHECK_PASSED;
}

int ra_db_mob_load_record(mob_ra * mobs, int size, sqlite3_stmt * sql) {
    int i = 0;
    mob_ra * mob = NULL;

    for(i = 0; i < size; i++) {
        mob = &mobs[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
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
            fprintf(stderr, "[load]: failed to add %s to mob db.\n", mob->iro);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }

    return CHECK_PASSED;
}

int ra_db_skill_load(ra_db_t * ra, const char * path) {
    native_t skills;
    skills.db = NULL;
    skills.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_numeric, load_native_general, &skills, &load_ra_native[2]) ||
        NULL == skills.db ||
        0 >= skills.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_skill_load_record(skills.db, skills.size, ra->skill_ra_sql_insert) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load skill db; %s.\n", path);
        SAFE_FREE(skills.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded skill db; %s.\n", path);
    SAFE_FREE(skills.db);
    return CHECK_PASSED;
}

int ra_db_skill_load_record(skill_ra * skills, int size, sqlite3_stmt * sql) {
    int i = 0;
    skill_ra * skill = NULL;

    for(i = 0; i < size; i++) {
        skill = &skills[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
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
            SQLITE_OK != sqlite3_bind_int(sql, 16, skill->inf3) ||
            SQLITE_OK != sqlite3_bind_text(sql, 17, skill->name, strlen(skill->name), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_text(sql, 18, skill->desc, strlen(skill->desc), SQLITE_STATIC) ||
            SQLITE_DONE != sqlite3_step(sql) ||
            SQLITE_OK != sqlite3_reset(sql)) {
            fprintf(stderr, "[load]: failed to add %s to skill db.\n", skill->name);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }

    return CHECK_PASSED;
}

int ra_db_produce_load(ra_db_t * ra, const char * path) {
    native_t produces;
    produces.db = NULL;
    produces.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_numeric, load_native_general, &produces, &load_ra_native[3]) ||
        NULL == produces.db ||
        0 >= produces.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_produce_load_record(produces.db, produces.size, ra->produce_ra_sql_insert) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load produce db; %s.\n", path);
        SAFE_FREE(produces.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded produce db; %s.\n", path);
    SAFE_FREE(produces.db);
    return CHECK_PASSED;
}

int ra_db_produce_load_record(produce_ra * produces, int size, sqlite3_stmt * sql) {
    int i = 0;
    char buf[BUF_SIZE];
    produce_ra * produce = NULL;

    for(i = 0; i < size; i++) {
        produce = &produces[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
            SQLITE_OK != sqlite3_bind_int(sql, 1, produce->id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 2, produce->item_id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 3, produce->item_lv) ||
            SQLITE_OK != sqlite3_bind_int(sql, 4, produce->skill_id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 5, produce->skill_lv) ||
            !array_to_string(buf, produce->item_id_req) ||
            SQLITE_OK != sqlite3_bind_text(sql, 6, buf, strlen(buf), SQLITE_TRANSIENT) ||
            !array_to_string_cnt(buf, produce->item_amount_req, array_field_cnt(buf) + 1) ||
            SQLITE_OK != sqlite3_bind_text(sql, 7, buf, strlen(buf), SQLITE_TRANSIENT) ||
            SQLITE_DONE != sqlite3_step(sql) ||
            SQLITE_OK != sqlite3_reset(sql)) {
            fprintf(stderr, "[load]: failed to add %d to produce db.\n", produce->id);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }

    return CHECK_PASSED;
}

int ra_db_merc_load(ra_db_t * ra, const char * path) {
    native_t mercs;
    mercs.db = NULL;
    mercs.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_numeric, load_native_general, &mercs, &load_ra_native[4]) ||
        NULL == mercs.db ||
        0 >= mercs.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_merc_load_record(mercs.db, mercs.size, ra->mercenary_ra_sql_insert) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load mercenary db; %s.\n", path);
        SAFE_FREE(mercs.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded mercenary db; %s.\n", path);
    SAFE_FREE(mercs.db);
    return CHECK_PASSED;
}

int ra_db_merc_load_record(mercenary_ra * mercs, int size, sqlite3_stmt * sql) {
    int i = 0;
    mercenary_ra * merc = NULL;

    for(i = 0; i < size; i++) {
        merc = &mercs[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
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
            fprintf(stderr, "[load]: failed to add %s to mercenary db.\n", merc->name);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }
    return CHECK_PASSED;
}

int ra_db_pet_load(ra_db_t * ra, const char * path) {
    native_t pets;
    pets.db = NULL;
    pets.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_numeric, load_native_general, &pets, &load_ra_native[5]) ||
        NULL == pets.db ||
        0 >= pets.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_pet_load_record(pets.db, pets.size, ra->pet_ra_sql_insert) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load pet db; %s.\n", path);
        SAFE_FREE(pets.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded pet db; %s.\n", path);
    SAFE_FREE(pets.db);
    return CHECK_PASSED;
}


int ra_db_pet_load_record(pet_ra * pets, int size, sqlite3_stmt * sql) {
    int i = 0;
    pet_ra * pet = NULL;

    for(i = 0; i < size; i++) {
        pet = &pets[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
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
            fprintf(stderr, "[load]: failed to add %s to pet db.\n", pet->name);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }
    return CHECK_PASSED;
}

int ra_db_const_load(ra_db_t * ra, const char * path) {
    native_t constants;
    constants.db = NULL;
    constants.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_general, load_native_general, &constants, &load_ra_native[7]) ||
        NULL == constants.db ||
        0 >= constants.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_const_load_record(constants.db, constants.size, ra->const_ra_sql_insert) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load constant db; %s.\n", path);
        SAFE_FREE(constants.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded constant db; %s.\n", path);
    SAFE_FREE(constants.db);
    return CHECK_PASSED;
}

int ra_db_const_load_record(const_ra * constants, int size, sqlite3_stmt * sql) {
    int i = 0;
    const_ra * constant = NULL;

    for(i = 0; i < size; i++) {
        constant = &constants[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
            SQLITE_OK != sqlite3_bind_text(sql, 1, constant->name, strlen(constant->name), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_int(sql, 2, constant->value) ||
            SQLITE_OK != sqlite3_bind_int(sql, 3, constant->type) ||
            SQLITE_DONE != sqlite3_step(sql) ||
            SQLITE_OK != sqlite3_reset(sql)) {
            fprintf(stderr, "[load]: failed to add %s to constant db.\n", constant->name);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }

    return CHECK_PASSED;
}

int const_ra_name_search(sqlite3_stmt * sql, char * name, const_ra * constant) {
    if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
        SQLITE_OK != sqlite3_bind_text(sql, 1, name, strlen(name), SQLITE_STATIC) ||
        SQLITE_ROW != sqlite3_step(sql)) {
        fprintf(stderr, "FAILED %s\n", name);
        sqlite3_reset(sql);
        return CHECK_FAILED;
    }

    strncopy(constant->name, MAX_NAME_SIZE, sqlite3_column_text(sql, 0));
    constant->value = sqlite3_column_int(sql, 1);
    constant->type = sqlite3_column_int(sql, 2);

    sqlite3_reset(sql);
    return CHECK_PASSED;
}

int item_ra_name_search(sqlite3_stmt * sql, char * name, item_ra * item) {
    int len = strlen(name);

    if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
        SQLITE_OK != sqlite3_bind_text(sql, 1, name, len, SQLITE_STATIC) ||
        SQLITE_OK != sqlite3_bind_text(sql, 2, name, len, SQLITE_STATIC) ||
        SQLITE_ROW != sqlite3_step(sql)) {
        sqlite3_reset(sql);
        return CHECK_FAILED;
    }

    item->id = sqlite3_column_int(sql, 0);
    strncopy(item->eathena, MAX_NAME_SIZE, sqlite3_column_text(sql, 1));
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(sql, 2));

    sqlite3_reset(sql);
    return CHECK_PASSED;
}

int item_ra_id_search(sqlite3_stmt * sql, int item_id, item_ra * item) {
    if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
        SQLITE_OK != sqlite3_bind_int(sql, 1, item_id) ||
        SQLITE_ROW != sqlite3_step(sql)) {
        sqlite3_reset(sql);
        return CHECK_FAILED;
    }

    item->id = sqlite3_column_int(sql, 0);
    strncopy(item->eathena, MAX_NAME_SIZE, sqlite3_column_text(sql, 1));
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(sql, 2));
    item->type = sqlite3_column_int(sql, 3);
    sqlite3_reset(sql);
    return CHECK_PASSED;
}

int ra_db_item_group_load(ra_db_t * ra, const char * path) {
    native_t item_groups;
    item_groups.db = NULL;
    item_groups.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_alpha, load_native_general, &item_groups, &load_ra_native[6]) ||
        NULL == item_groups.db ||
        0 >= item_groups.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_item_group_load_record(item_groups.db, item_groups.size, ra->item_group_ra_sql_insert, ra->const_ra_name_search) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load item groups db; %s.\n", path);
        SAFE_FREE(item_groups.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded item groups db; %s.\n", path);
    SAFE_FREE(item_groups.db);
    return CHECK_PASSED;
}

int ra_db_item_group_load_record(item_group_ra * item_groups, int size, sqlite3_stmt * sql, sqlite3_stmt * search) {
    int i = 0;
    item_group_ra * item_group = NULL;
    const_ra constant;

    for(i = 0; i < size; i++) {
        item_group = &item_groups[i];
        memset(&constant, 0, sizeof(const_ra));
        if( const_ra_name_search(search, item_group->name, &constant) ||
            SQLITE_OK != sqlite3_clear_bindings(sql) ||
            SQLITE_OK != sqlite3_bind_int(sql, 1, constant.value) ||
            SQLITE_OK != sqlite3_bind_int(sql, 2, item_group->item_id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 3, item_group->rate) ||
            SQLITE_DONE != sqlite3_step(sql) ||
            SQLITE_OK != sqlite3_reset(sql)) {
            fprintf(stderr, "[load]: failed to add %s to item group db.\n", item_group->name);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }

    return CHECK_PASSED;
}

int ra_db_item_package_load(ra_db_t * ra, const char * path) {
    native_t item_packages;
    item_packages.db = NULL;
    item_packages.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_alpha, load_native_general, &item_packages, &load_ra_native[8]) ||
        NULL == item_packages.db ||
        0 >= item_packages.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_item_group_package_record(ra, item_packages.db, item_packages.size) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load item package db; %s.\n", path);
        SAFE_FREE(item_packages.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded item package db; %s.\n", path);
    SAFE_FREE(item_packages.db);
    return CHECK_PASSED;
}

int ra_db_item_group_package_record(ra_db_t * ra, package_ra * packages, int size) {
    int i = 0;
    package_ra * package = NULL;
    const_ra constant;              /* revolve group constant to group id */
    item_ra item;                   /* resolve item name to item id */

    for(i = 0; i < size; i++) {
        package = &packages[i];
        memset(&constant, 0, sizeof(const_ra));
        memset(&item, 0, sizeof(item_ra));
        if( const_ra_name_search(ra->const_ra_name_search, package->group_name, &constant) ||
            (item_ra_name_search(ra->item_ra_name_search, package->item_name, &item) &&
             item_ra_id_search(ra->item_ra_id_search, convert_integer(package->item_name, 10), &item)) ||
            SQLITE_OK != sqlite3_clear_bindings(ra->item_package_ra_sql_insert) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 1, constant.value) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 2, item.id) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 3, package->rate) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 4, package->amount) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 5, package->random) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 6, package->announced) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 7, package->duration) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 8, package->guid) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 9, package->bound) ||
            SQLITE_OK != sqlite3_bind_int(ra->item_package_ra_sql_insert, 10, package->named) ||
            SQLITE_DONE != sqlite3_step(ra->item_package_ra_sql_insert) ||
            SQLITE_OK != sqlite3_reset(ra->item_package_ra_sql_insert)) {
            fprintf(stderr, "[load]: failed to add %s to %s in item package db.\n", package->item_name, package->group_name);
            if (SQLITE_OK != sqlite3_reset(ra->item_package_ra_sql_insert)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }

    return CHECK_PASSED;
}

int ra_db_item_combo_load(ra_db_t * ra, const char * path) {
    native_t item_combos;
    item_combos.db = NULL;
    item_combos.size = 0;

    exit_null_safe(2, ra, path);

    if( load_native(path, trim_numeric, load_native_general, &item_combos, &load_ra_native[9]) ||
        NULL == item_combos.db ||
        0 >= item_combos.size ||
        ra_db_exec(ra, "BEGIN IMMEDIATE TRANSACTION;") ||
        ra_db_item_combo_load_record(ra, item_combos.db, item_combos.size) ||
        ra_db_exec(ra, "COMMIT TRANSACTION;")) {
        fprintf(stderr, "[load]: failed to load item combo db; %s.\n", path);
        SAFE_FREE(item_combos.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded item combo db; %s.\n", path);
    SAFE_FREE(item_combos.db);
    return CHECK_PASSED;
}

int ra_db_item_combo_load_record(ra_db_t * ra, combo_ra * combos, int size) {
    int i = 0;
    combo_ra * combo = NULL;

    /* item id */
    array_w item_array;
    int * item_list = NULL;

    /* item name */
    int j = 0;
    int off = 0;
    char buf[BUF_SIZE];
    item_ra item;

    for(i = 0; i < size; i++) {
        combo = &combos[i];

        /* convert the : delimited list of item ids */
        memset(&item_array, 0, sizeof(array_w));
        convert_integer_delimit_array(combo->item_id.str, ":", &item_array);
        item_list = item_array.array;

        if(item_list != NULL && item_array.size > 0) {
            /* resolve all item id to item name and concatenate the item names */
            for(j = 0, off = 0; j < item_array.size; j++) {
                memset(&item, 0, sizeof(item_ra));
                if(!item_ra_id_search(ra->item_ra_id_search, item_list[j], &item)) {
                    off += (off == 0)?
                        sprintf(&buf[off], "[%s", item.eathena):
                        sprintf(&buf[off], ", %s", item.eathena);
                }
            }
            off += sprintf(&buf[off], " Combo]");

            for(j = 0; j < item_array.size; j++) {
                if( SQLITE_OK != sqlite3_clear_bindings(ra->item_combo_ra_sql_insert) ||
                    SQLITE_OK != sqlite3_bind_int(ra->item_combo_ra_sql_insert, 1, item_list[j]) ||
                    SQLITE_OK != sqlite3_bind_text(ra->item_combo_ra_sql_insert, 2, combo->script, strlen(combo->script), SQLITE_STATIC) ||
                    SQLITE_OK != sqlite3_bind_text(ra->item_combo_ra_sql_insert, 3, buf, off, SQLITE_STATIC) ||
                    SQLITE_DONE != sqlite3_step(ra->item_combo_ra_sql_insert) ||
                    SQLITE_OK != sqlite3_reset(ra->item_combo_ra_sql_insert)) {
                    if (SQLITE_OK != sqlite3_reset(ra->item_combo_ra_sql_insert)) {
                        fprintf(stderr, "[load]: failed to add item id combo %d to item combo db.\n", item_list[j]);
                        fprintf(stderr, "[load]: failed to reset sql statement.\n");
                        return CHECK_FAILED;
                    }
                }
            }
            SAFE_FREE(item_list);
        }
    }

    return CHECK_PASSED;
}

int ra_db_item_package_meta(ra_db_t * ra) {
    int ret = 0;
    int cnt = 0;
    item_ra item;
    package_ra package;
    package_meta_ra meta;
    sqlite3_stmt * stmt = NULL;

    /* error on invalid reference */
    exit_null_safe(1, ra);

    /* setup the local stack buffers */
    memset(&item, 0, sizeof(item_ra));
    memset(&package, 0, sizeof(package_ra));
    memset(&meta, 0, sizeof(package_meta_ra));
    stmt = ra->item_package_query;

    /* query each item package record */
    ret = sqlite3_step(stmt);
    if(SQLITE_DONE == ret)
        return exit_abt_safe("item package table is empty");

    while(SQLITE_ROW == ret) {
        package.group_id = sqlite3_column_int(stmt, 0);
        package.item_id = sqlite3_column_int(stmt, 1);
        package.rate = sqlite3_column_int(stmt, 2);
        package.amount = sqlite3_column_int(stmt, 3);
        package.random = sqlite3_column_int(stmt, 4);
        package.announced = sqlite3_column_int(stmt, 5);
        package.duration = sqlite3_column_int(stmt, 6);
        package.guid = sqlite3_column_int(stmt, 7);
        package.bound = sqlite3_column_int(stmt, 8);
        package.named = sqlite3_column_int(stmt, 9);

        /* setup the package meta record */
        if(0 == cnt) {
            meta.group_id = package.group_id;
            meta.subgroup_id = package.random;
        } else if(meta.group_id != package.group_id ||
                  meta.subgroup_id != package.random) {
            /* write the current package meta record to database */
            if(ra_db_item_package_meta_insert(ra->item_package_meta_insert, &meta))
                return CHECK_FAILED;
            memset(&meta, 0, sizeof(package_meta_ra));
            meta.group_id = package.group_id;
            meta.subgroup_id = package.random;
        }

        /* item count */
        meta.item++;

        /* item type count */
        if(item_ra_id_search(ra->item_ra_id_search, package.item_id, &item))
            return exit_func_safe("item id %d does not exist", package.item_id);
        switch(item.type) {
            case 0: meta.heal++; break;
            case 2: meta.usable++; break;
            case 3: meta.etc++; break;
            case 4: meta.armor++; break;
            case 5: meta.weapon++; break;
            case 6: meta.card++; break;
            case 7: meta.pet++; break;
            case 8: meta.pet_equip++; break;
            case 10: meta.ammo++; break;
            case 11: meta.delay_usable++; break;
            case 12: meta.shadow++; break;
            case 18: meta.confirm_usable++; break;
            default:
                return exit_func_safe("invalid item type %d for item id %d", item.type, item.id);
        }

        /* item group count */
        if(package.bound > 0)
            meta.bind++;
        if(package.duration > 0)
            meta.rent++;

        ret = sqlite3_step(stmt);
        cnt++;
    }

    /* write the last package meta record to database */
    if(ra_db_item_package_meta_insert(ra->item_package_meta_insert, &meta))
        return CHECK_FAILED;

    fprintf(stdout, "[load]: created item package meta table.\n");
    return CHECK_PASSED;
}

int ra_db_item_package_meta_insert(sqlite3_stmt * stmt, package_meta_ra * meta) {
    if(SQLITE_OK != sqlite3_reset(stmt) ||
       SQLITE_OK != sqlite3_clear_bindings(stmt) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 1, meta->group_id) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 2, meta->subgroup_id) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 3, meta->item) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 4, meta->heal) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 5, meta->usable) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 6, meta->etc) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 7, meta->armor) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 8, meta->weapon) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 9, meta->card) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 10, meta->pet) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 11, meta->pet_equip) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 12, meta->ammo) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 13, meta->delay_usable) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 14, meta->shadow) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 15, meta->confirm_usable) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 16, meta->bind) ||
       SQLITE_OK != sqlite3_bind_int(stmt, 17, meta->rent) ||
       SQLITE_DONE != sqlite3_step(stmt))
        return exit_func_safe("failed to add item package meta fo"
        "r group %d subgroup %d", meta->group_id, meta->subgroup_id);
    return CHECK_PASSED;
}

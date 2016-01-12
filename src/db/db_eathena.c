/*
 *   file: db_eathena.c
 *   date: 02/28/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "db_eathena.h"
#include "libsort.h"

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
        case 0:   record->id = convert_integer(val, 10);                break;
        case 1:   strnload(record->aegis, MAX_NAME_SIZE, val);          break;
        case 2:   strnload(record->eathena, MAX_NAME_SIZE, val);        break;
        case 3:   record->type = convert_integer(val, 10);              break;
        case 4:   record->buy = convert_integer(val, 10);               break;
        case 5:   record->sell = convert_integer(val, 10);              break;
        case 6:   record->weight = convert_integer(val, 10);            break;
        case 7:   record->atk = convert_integer(val, 10);               break;
        case 8:   record->def = convert_integer(val, 10);               break;
        case 9:   record->range = convert_integer(val, 10);             break;
        case 10:  record->slots = convert_integer(val, 10);             break;
        case 11:  record->job = convert_uinteger(val, 16);              break;
        case 12:  record->upper = convert_integer(val, 10);             break;
        case 13:  record->gender = convert_integer(val, 10);            break;
        case 14:  record->loc = convert_integer(val, 10);               break;
        case 15:  record->wlv = convert_integer(val, 10);               break;
        case 16:  record->elv = convert_integer(val, 10);               break;
        case 17:  record->refineable = convert_integer(val, 10);        break;
        case 18:  record->view = convert_integer(val, 10);              break;
        case 19:  strnload(record->script, MAX_SCRIPT_SIZE, val);       break;
        case 20:  strnload(record->onequip, MAX_SCRIPT_SIZE, val);      break;
        case 21:  strnload(record->onunequip, MAX_SCRIPT_SIZE, val);    break;
        default:  exit_func_safe("invalid column field %d in eathena item database.\n", col);
    }
    return 0;
}

int mob_ea_load(void * db, int row, int col, char * val) {
    mob_ea * record = &((mob_ea *) db)[row];
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
        case 31: record->expper = convert_integer(val,10);          break;
        case 32: record->mvp1id = convert_integer(val,10);          break;
        case 33: record->mvp1per = convert_integer(val,10);         break;
        case 34: record->mvp2id = convert_integer(val,10);          break;
        case 35: record->mvp2per = convert_integer(val,10);         break;
        case 36: record->mvp3id = convert_integer(val,10);          break;
        case 37: record->mvp3per = convert_integer(val,10);         break;
        case 38: record->drop1id = convert_integer(val,10);         break;
        case 39: record->drop1per = convert_integer(val,10);        break;
        case 40: record->drop2id = convert_integer(val,10);         break;
        case 41: record->drop2per = convert_integer(val,10);        break;
        case 42: record->drop3id = convert_integer(val,10);         break;
        case 43: record->drop3per = convert_integer(val,10);        break;
        case 44: record->drop4id = convert_integer(val,10);         break;
        case 45: record->drop4per = convert_integer(val,10);        break;
        case 46: record->drop5id = convert_integer(val,10);         break;
        case 47: record->drop5per = convert_integer(val,10);        break;
        case 48: record->drop6id = convert_integer(val,10);         break;
        case 49: record->drop6per = convert_integer(val,10);        break;
        case 50: record->drop7id = convert_integer(val,10);         break;
        case 51: record->drop7per = convert_integer(val,10);        break;
        case 52: record->drop8id = convert_integer(val,10);         break;
        case 53: record->drop8per = convert_integer(val,10);        break;
        case 54: record->drop9id = convert_integer(val,10);         break;
        case 55: record->drop9per = convert_integer(val,10);        break;
        case 56: record->dropcardid = convert_integer(val,10);      break;
        case 57: record->dropcardper = convert_integer(val,10);     break;
        default: exit_func_safe("invalid column field %d in eathena mob database.\n", col);
    }
    return 0;
}

int skill_ea_load(void * db, int row, int col, char * val) {
    skill_ea * record = &((skill_ea *) db)[row];
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
        case 15: strnload(record->name, MAX_NAME_SIZE, val);                break;
        case 16: strnload(record->desc, MAX_NAME_SIZE, val);                break;
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
            record->item_id = convert_integer(val,10);                      break;
        case 1: record->item_lv = convert_integer(val,10);                  break;
        case 2: record->skill_id = convert_integer(val,10);                 break;
        case 3: record->skill_lv = convert_integer(val,10);                 break;
        default:
            (!alternate) ?
                (record->item_id_req[material_cnt] = convert_integer(val,10)):
                (record->item_amount_req[material_cnt++] = convert_integer(val,10));
            alternate = !alternate;
            record->ingredient_count = material_cnt;                        break;
    }
    return 0;
}

int mercenary_ea_load(void * db, int row, int col, char * val) {
    mercenary_ea * record = &((mercenary_ea *) db)[row];
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
        case 25: record->dmotion = convert_integer(val, 10);         break;
        default: exit_func_safe("invalid column field %d in eathena mercenary database.\n", col);
    }
    return 0;
}

int pet_ea_load(void * db, int row, int col, char * val) {
    pet_ea * record = &((pet_ea *) db)[row];
    switch(col) {
        case 0: record->mob_id = convert_integer(val,10);                   break;
        case 1: strnload(record->name, MAX_NAME_SIZE, val);                 break;
        case 2: strnload(record->jname, MAX_NAME_SIZE, val);                break;
        case 3: record->lure_id = convert_integer(val,10);                  break;
        case 4: record->egg_id = convert_integer(val,10);                   break;
        case 5: record->equip_id = convert_integer(val,10);                 break;
        case 6: record->food_id = convert_integer(val,10);                  break;
        case 7: record->fullness = convert_integer(val,10);                 break;
        case 8: record->hungry_delay = convert_integer(val,10);             break;
        case 9: record->r_hungry = convert_integer(val,10);                 break;
        case 10: record->r_full = convert_integer(val,10);                  break;
        case 11: record->intimate = convert_integer(val,10);                break;
        case 12: record->die = convert_integer(val,10);                     break;
        case 13: record->capture = convert_integer(val,10);                 break;
        case 14: record->speed = convert_integer(val,10);                   break;
        case 15: record->s_performance = convert_integer(val,10);           break;
        case 16: record->talk_convert = convert_integer(val,10);            break;
        case 17: record->attack_rate = convert_integer(val,10);             break;
        case 18: record->defence_attack_rate = convert_integer(val,10);     break;
        case 19: record->change_target_rate = convert_integer(val,10);      break;
        case 20: strnload(record->pet_script, MAX_SCRIPT_SIZE, val);        break;
        case 21: strnload(record->loyal_script, MAX_SCRIPT_SIZE, val);      break;
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
        case 0: strnload(record->name, MAX_NAME_SIZE, val);                             break;
        case 1:
            /* constant can be represented as hexadecimal or decimal */
            record->value = (strlen(val) > 2 && val[0] == '0' && val[1] == 'x') ?
                convert_integer(val, 16):
                convert_integer(val, 10);                                               break;
        case 2: record->type = convert_integer(val, 10);                                break;
        default: exit_func_safe("invalid column field %d in eathena const database.\n", col);
    }
    return 0;
}

int ea_db_init(ea_db_t ** ea, const char * path) {
    char * error = NULL;
    ea_db_t * _ea = NULL;

    exit_null_safe(2, ea, path);

    _ea = calloc(1, sizeof(ea_db_t));
    if(NULL == _ea)
        goto failed;

    if(SQLITE_OK != sqlite3_open_v2(path, &_ea->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL)) {
        if(NULL != _ea->db)
            fprintf(stderr, "[load]: failed to create %s; %s.\n", path, sqlite3_errmsg(_ea->db));
        goto failed;
    }

    if( SQLITE_OK != sqlite3_exec(_ea->db,
        EA_ITEM_DELETE
        EA_ITEM_CREATE
        EA_MOB_DELETE
        EA_MOB_CREATE
        EA_SKILL_DELETE
        EA_SKILL_CREATE
        EA_PRODUCE_DELETE
        EA_PRODUCE_CREATE
        EA_MERC_DELETE
        EA_MERC_CREATE
        EA_PET_DELETE
        EA_PET_CREATE
        EA_ITEM_GROUP_DELETE
        EA_ITEM_GROUP_CREATE
        EA_ITEM_GROUP_ENTRY_DELETE
        EA_ITEM_GROUP_ENTRY_CREATE
        EA_CONST_DELETE
        EA_CONST_CREATE,
        NULL, NULL, &error)) {
        if(NULL != error) {
            fprintf(stderr, "[load]: failed to load schema %s; %s.\n", path, error);
            sqlite3_free(error);
        }
        goto failed;
    }

    if( SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_ITEM_INSERT, strlen(EA_ITEM_INSERT), &_ea->item_ea_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_MOB_INSERT, strlen(EA_MOB_INSERT), &_ea->mob_ea_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_SKILL_INSERT, strlen(EA_SKILL_INSERT), &_ea->skill_ea_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_PRODUCE_INSERT, strlen(EA_PRODUCE_INSERT), &_ea->produce_ea_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_MERC_INSERT, strlen(EA_MERC_INSERT), &_ea->mercenary_ea_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_PET_INSERT, strlen(EA_PET_INSERT), &_ea->pet_ea_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_ITEM_GROUP_INSERT, strlen(EA_ITEM_GROUP_INSERT), &_ea->item_group_ea_sql_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_ITEM_GROUP_ENTRY_INSERT, strlen(EA_ITEM_GROUP_ENTRY_INSERT), &_ea->item_group_ea_sql_record_insert, NULL) ||
        SQLITE_OK != sqlite3_prepare_v2(_ea->db, EA_CONST_INSERT, strlen(EA_CONST_INSERT), &_ea->const_ea_sql_insert, NULL)) {
        fprintf(stderr, "[load]: failed prepare sql statments; %s.\n", sqlite3_errmsg(_ea->db));
        goto failed;
    }

    *ea = _ea;
    return CHECK_PASSED;

failed:
    ea_db_deit(&_ea);
    return CHECK_FAILED;
}

int ea_db_deit(ea_db_t ** ea) {
    ea_db_t * _ea = NULL;

    exit_null_safe(2, ea, *ea);

    _ea = *ea;
    if( SQLITE_OK != sqlite3_finalize(_ea->item_ea_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ea->mob_ea_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ea->skill_ea_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ea->produce_ea_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ea->mercenary_ea_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ea->pet_ea_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ea->item_group_ea_sql_insert) ||
        SQLITE_OK != sqlite3_finalize(_ea->const_ea_sql_insert) ||
        SQLITE_OK != sqlite3_close(_ea->db)) {
        exit(EXIT_FAILURE);
    }
    SAFE_FREE(_ea);
    *ea = NULL;
    return CHECK_PASSED;
}

int ea_db_exec(ea_db_t * db, char * sql) {
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

int ea_db_item_load(ea_db_t * ea, const char * path) {
    native_t items;
    items.db = NULL;
    items.size = 0;

    if( load_native(path, trim_numeric, load_native_general, &items, &load_ea_native[0]) ||
        NULL == items.db ||
        0 >= items.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;") ||
        ea_db_item_load_record(items.db, items.size, ea->item_ea_sql_insert) ||
        ea_db_exec(ea, "COMMIT TRANSACTION;")) {
        ea_db_exec(ea, "ROLLBACK TRANSACTION;");
        fprintf(stderr, "[load]: failed to load item db; %s.\n", path);
        SAFE_FREE(items.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded item db; %s.\n", path);
    SAFE_FREE(items.db);
    return CHECK_PASSED;
}

int ea_db_item_load_record(item_ea * items, int size, sqlite3_stmt * sql) {
    int i = 0;
    item_ea * item = NULL;

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
            SQLITE_OK != sqlite3_bind_int(sql, 8, item->atk) ||
            SQLITE_OK != sqlite3_bind_int(sql, 9, item->def) ||
            SQLITE_OK != sqlite3_bind_int(sql, 10, item->range) ||
            SQLITE_OK != sqlite3_bind_int(sql, 11, item->slots) ||
            SQLITE_OK != sqlite3_bind_int(sql, 12, item->job) ||
            SQLITE_OK != sqlite3_bind_int(sql, 13, item->upper) ||
            SQLITE_OK != sqlite3_bind_int(sql, 14, item->gender) ||
            SQLITE_OK != sqlite3_bind_int(sql, 15, item->loc) ||
            SQLITE_OK != sqlite3_bind_int(sql, 16, item->wlv) ||
            SQLITE_OK != sqlite3_bind_int(sql, 17, item->elv) ||
            SQLITE_OK != sqlite3_bind_int(sql, 18, item->refineable) ||
            SQLITE_OK != sqlite3_bind_int(sql, 19, item->view) ||
            SQLITE_OK != sqlite3_bind_text(sql, 20, item->script, strlen(item->script), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_text(sql, 21, item->onequip, strlen(item->onequip), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_text(sql, 22, item->onunequip, strlen(item->onunequip), SQLITE_STATIC) ||
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

int ea_db_mob_load(ea_db_t * ea, const char * path) {
    native_t mobs;
    mobs.db = NULL;
    mobs.size = 0;

    if( load_native(path, trim_numeric, load_native_general, &mobs, &load_ea_native[1]) ||
        NULL == mobs.db ||
        0 >= mobs.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;") ||
        ea_db_mob_load_record(mobs.db, mobs.size, ea->mob_ea_sql_insert) ||
        ea_db_exec(ea, "COMMIT TRANSACTION;")) {
        ea_db_exec(ea, "ROLLBACK TRANSACTION;");
        fprintf(stderr, "[load]: failed to load mob db; %s.\n", path);
        SAFE_FREE(mobs.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded mob db; %s.\n", path);
    SAFE_FREE(mobs.db);
    return CHECK_PASSED;
}

int ea_db_mob_load_record(mob_ea * mobs, int size, sqlite3_stmt * sql) {
    int i = 0;
    mob_ea * mob = NULL;
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
            SQLITE_OK != sqlite3_bind_int(sql, 32, mob->expper) ||
            SQLITE_OK != sqlite3_bind_int(sql, 33, mob->mvp1id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 34, mob->mvp1per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 35, mob->mvp2id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 36, mob->mvp2per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 37, mob->mvp3id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 38, mob->mvp3per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 39, mob->drop1id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 40, mob->drop1per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 41, mob->drop2id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 42, mob->drop2per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 43, mob->drop3id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 44, mob->drop3per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 45, mob->drop4id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 46, mob->drop4per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 47, mob->drop5id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 48, mob->drop5per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 49, mob->drop6id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 50, mob->drop6per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 51, mob->drop7id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 52, mob->drop7per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 53, mob->drop8id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 54, mob->drop8per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 55, mob->drop9id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 56, mob->drop9per) ||
            SQLITE_OK != sqlite3_bind_int(sql, 57, mob->dropcardid) ||
            SQLITE_OK != sqlite3_bind_int(sql, 58, mob->dropcardper) ||
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

int ea_db_skill_load(ea_db_t * ea, const char * path) {
    native_t skills;
    skills.db = NULL;
    skills.size = 0;

    if( load_native(path, trim_numeric, load_native_general, &skills, &load_ea_native[2]) ||
        NULL == skills.db ||
        0 >= skills.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;") ||
        ea_db_skill_load_record(skills.db, skills.size, ea->skill_ea_sql_insert) ||
        ea_db_exec(ea, "COMMIT TRANSACTION;")) {
        ea_db_exec(ea, "ROLLBACK TRANSACTION;");
        fprintf(stderr, "[load]: failed to load skill db; %s.\n", path);
        SAFE_FREE(skills.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded skill db; %s.\n", path);
    SAFE_FREE(skills.db);
    return CHECK_PASSED;
}

int ea_db_skill_load_record(skill_ea * skills, int size, sqlite3_stmt * sql) {
    int i = 0;
    skill_ea * skill = NULL;
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
            SQLITE_OK != sqlite3_bind_text(sql, 16, skill->name, strlen(skill->name), SQLITE_STATIC) ||
            SQLITE_OK != sqlite3_bind_text(sql, 17, skill->desc, strlen(skill->desc), SQLITE_STATIC) ||
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

int ea_db_produce_load(ea_db_t * ea, const char * path) {
    native_t produces;
    produces.db = NULL;
    produces.size = 0;

    if( load_native(path, trim_numeric, load_native_general, &produces, &load_ea_native[3]) ||
        NULL == produces.db ||
        0 >= produces.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;") ||
        ea_db_produce_load_record(produces.db, produces.size, ea->produce_ea_sql_insert) ||
        ea_db_exec(ea, "COMMIT TRANSACTION;")) {
        ea_db_exec(ea, "ROLLBACK TRANSACTION;");
        fprintf(stderr, "[load]: failed to load produce db; %s.\n", path);
        SAFE_FREE(produces.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded produce db; %s.\n", path);
    SAFE_FREE(produces.db);
    return CHECK_PASSED;
}

int ea_db_produce_load_record(produce_ea * produces, int size, sqlite3_stmt * sql) {
    int i = 0;
    char buf[BUF_SIZE];
    produce_ea * produce = NULL;

    for(i = 0; i < size; i++) {
        produce = &produces[i];
        if( SQLITE_OK != sqlite3_clear_bindings(sql) ||
            SQLITE_OK != sqlite3_bind_int(sql, 1, produce->item_id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 2, produce->item_lv) ||
            SQLITE_OK != sqlite3_bind_int(sql, 3, produce->skill_id) ||
            SQLITE_OK != sqlite3_bind_int(sql, 4, produce->skill_lv) ||
            !array_to_string(buf, produce->item_id_req) ||
            SQLITE_OK != sqlite3_bind_text(sql, 5, buf, strlen(buf), SQLITE_TRANSIENT) ||
            !array_to_string_cnt(buf, produce->item_amount_req, array_field_cnt(buf) + 1) ||
            SQLITE_OK != sqlite3_bind_text(sql, 6, buf, strlen(buf), SQLITE_TRANSIENT) ||
            SQLITE_DONE != sqlite3_step(sql) ||
            SQLITE_OK != sqlite3_reset(sql)) {
            fprintf(stderr, "[load]: failed to add %d to produce db.\n", produce->item_id);
            if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
            }
        }
    }

    return CHECK_PASSED;
}


int ea_db_merc_load(ea_db_t * ea, const char * path) {
    native_t mercs;
    mercs.db = NULL;
    mercs.size = 0;

    if( load_native(path, trim_numeric, load_native_general, &mercs, &load_ea_native[4]) ||
        NULL == mercs.db ||
        0 >= mercs.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;") ||
        ea_db_merc_load_record(mercs.db, mercs.size, ea->mercenary_ea_sql_insert) ||
        ea_db_exec(ea, "COMMIT TRANSACTION;")) {
        ea_db_exec(ea, "ROLLBACK TRANSACTION;");
        fprintf(stderr, "[load]: failed to load mercenary db; %s.\n", path);
        SAFE_FREE(mercs.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded mercenary db; %s.\n", path);
    SAFE_FREE(mercs.db);
    return CHECK_PASSED;
}

int ea_db_merc_load_record(mercenary_ea * mercs, int size, sqlite3_stmt * sql) {
    int i = 0;
    mercenary_ea * merc = NULL;

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

int ea_db_pet_load(ea_db_t * ea, const char * path) {
    native_t pets;
    pets.db = NULL;
    pets.size = 0;

    if( load_native(path, trim_numeric, load_native_general, &pets, &load_ea_native[5]) ||
        NULL == pets.db ||
        0 >= pets.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;") ||
        ea_db_pet_load_record(pets.db, pets.size, ea->pet_ea_sql_insert) ||
        ea_db_exec(ea, "COMMIT TRANSACTION;")) {
        ea_db_exec(ea, "ROLLBACK TRANSACTION;");
        fprintf(stderr, "[load]: failed to load pet db; %s.\n", path);
        SAFE_FREE(pets.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded pet db; %s.\n", path);
    SAFE_FREE(pets.db);
    return CHECK_PASSED;
}

int ea_db_pet_load_record(pet_ea * pets, int size, sqlite3_stmt * sql) {
    int i = 0;
    pet_ea * pet = NULL;

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

int ea_db_item_group_load(ea_db_t * ea, const char * path) {
    native_t item_groups;
    item_groups.db = NULL;
    item_groups.size = 0;
    item_group_ea tmp;

    if( load_native(path, trim_numeric, load_native_general, &item_groups, &load_ea_native[6]) ||
        NULL == item_groups.db ||
        0 >= item_groups.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;")) {
        goto failed;
    } else {
        quick_sort(item_groups.db, &tmp, item_groups.size, item_group_ea, group_id);
        if( ea_db_item_group_load_record(item_groups.db, item_groups.size, ea) ||
            ea_db_exec(ea, "COMMIT TRANSACTION;")) {
            ea_db_exec(ea, "ROLLBACK TRANSACTION;");
            goto failed;
        }
    }

    fprintf(stdout, "[load]: loaded item group db; %s.\n", path);
    SAFE_FREE(item_groups.db);
    return CHECK_PASSED;

failed:
    fprintf(stderr, "[load]: failed to load item group db; %s.\n", path);
    SAFE_FREE(item_groups.db);
    return CHECK_FAILED;
}

int ea_db_item_group_load_record(item_group_ea * item_groups, int size, ea_db_t * ea) {
    int i = 0;
    int group_off = 0;                  /* starting index for the current group id */
    int group_id = 0;                   /* current group id */
    int group_size = 0;                 /* current group size */
    item_group_ea * item_group = NULL;  /* iterater */
    sqlite3_stmt * stmt = NULL;

    stmt = ea->item_group_ea_sql_insert;

    /* set the initial group offset and id */
    group_off = i;
    group_id = item_groups[i].group_id;

    for(i = 0; ; i++) {
        item_group = &item_groups[i];

        /* insert the current group id before the next group id */
        if (i == size ||
            item_group->group_id != group_id) {
            group_size = i - group_off;

            /* insert the group id and size */
            if(SQLITE_OK != sqlite3_reset(stmt) ||
               SQLITE_OK != sqlite3_clear_bindings(stmt) ||
               SQLITE_OK != sqlite3_bind_int(stmt, 1, group_id) ||
               SQLITE_OK != sqlite3_bind_int(stmt, 2, group_size) ||
               SQLITE_DONE != sqlite3_step(stmt)) {
                fprintf(stderr, "[load]: failed to insert group id %d.\n", group_id);
                return CHECK_FAILED;
            }

            /* insert the group id records */
            if(ea_db_item_group_load_records(item_groups, group_off, i, ea->item_group_ea_sql_record_insert)) {
                fprintf(stderr, "[load]: failed to insert group id %d records.\n", group_id);
                return CHECK_FAILED;
            }

            /* set the next group offset and id */
            group_off = i;
            group_id = item_group->group_id;
        }

        /* handle the last case in the same loop therefore the
         * loop must break after the last case is completed. */
        if(i == size)
            break;
    }
    return CHECK_PASSED;
}

int ea_db_item_group_load_records(item_group_ea * item_groups, int off, int end, sqlite3_stmt * stmt) {
    int i = 0;
    item_group_ea * item_group = NULL;

    for(i = off; i < end; i++) {
        item_group = &item_groups[i];
        if( SQLITE_OK != sqlite3_reset(stmt) ||
            SQLITE_OK != sqlite3_clear_bindings(stmt) ||
            SQLITE_OK != sqlite3_bind_int(stmt, 1, item_group->group_id) ||
            SQLITE_OK != sqlite3_bind_int(stmt, 2, item_group->item_id) ||
            SQLITE_OK != sqlite3_bind_int(stmt, 3, item_group->rate) ||
            SQLITE_DONE != sqlite3_step(stmt)) {
            return CHECK_FAILED;
        }
    }
    return CHECK_PASSED;
}

int ea_db_const_load(ea_db_t * ea, const char * path) {
    native_t constants;
    constants.db = NULL;
    constants.size = 0;

    if( load_native(path, trim_general, load_native_general, &constants, &load_ea_native[7]) ||
        NULL == constants.db ||
        0 >= constants.size ||
        ea_db_exec(ea, "BEGIN IMMEDIATE TRANSACTION;") ||
        ea_db_const_load_record(constants.db, constants.size, ea->const_ea_sql_insert) ||
        ea_db_exec(ea, "COMMIT TRANSACTION;")) {
        ea_db_exec(ea, "ROLLBACK TRANSACTION;");
        fprintf(stderr, "[load]: failed to load constant db; %s.\n", path);
        SAFE_FREE(constants.db);
        return CHECK_FAILED;
    }
    fprintf(stdout, "[load]: loaded constant db; %s.\n", path);
    SAFE_FREE(constants.db);
    return CHECK_PASSED;
}

int ea_db_const_load_record(const_ea * constants, int size, sqlite3_stmt * sql) {
    int i = 0;
    const_ea * constant = NULL;
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

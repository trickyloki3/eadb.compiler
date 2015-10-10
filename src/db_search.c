/*
 *   file: db_search.c
 *   date: 03/08/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "db_search.h"

int init_db(sqlite3 ** db, const char * path) {
    sqlite3 * _db = NULL;

    exit_null_safe(2, db, path);

    if(SQLITE_OK != sqlite3_open_v2(path, &_db, SQLITE_OPEN_READONLY, NULL)) {
        if(NULL != _db) {
            fprintf(stderr, "[load]: failed to load %s; %s.\n", path, sqlite3_errmsg(_db));
            sqlite3_close(_db);
        } else {
            fprintf(stderr, "[load]: failed to load %s.\n", path);
        }
        return CHECK_FAILED;
    }

    *db = _db;
    return CHECK_PASSED;
}


int deit_db(sqlite3 ** db) {
    sqlite3 * _db = NULL;

    exit_null_safe(2, db, *db);

    _db = *db;
    if(sqlite3_close(_db)) {
        fprintf(stderr, "[load]: failed to unload database; %s.\n", sqlite3_errmsg(_db));
        return CHECK_FAILED;
    }

    *db = NULL;
    return CHECK_PASSED;
}

int init_db_load(db_t ** db, const char * re_path, const char * db_path, int server_type) {
    db_t * _db = NULL;

    exit_null_safe(3, db, re_path, db_path);

    _db = calloc(1, sizeof(db_t));
    if(NULL == _db)
        return CHECK_FAILED;

    _db->server_type = server_type;

    if(/* open resource and server databases */
       init_db(&_db->re, re_path) ||
       init_db(&_db->db, db_path) ||
       /* prepare resource database queries */
       init_db_prep_sql(_db->re, &_db->opt_name, RE_OPT_NAME_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->map_name, RE_MAP_NAME_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->map_id, RE_MAP_ID_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->bonus_name, RE_BONUS_NAME_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->status_name, RE_STATUS_ID_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->var_name, RE_VAR_NAME_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->block_name, RE_BLOCK_NAME_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->sprite_id, RE_SPR_ID_SEARCH) ||
       init_db_prep_sql(_db->re, &_db->usprite_id, RE_NSPR_ID_SEARCH))
        goto failed;

    /* prepare server database queries */
    switch(server_type) {
        case EATHENA:
            if(init_db_prep_sql(_db->db, &_db->item_db, EA_ITEM_ITERATE) ||
               init_db_prep_sql(_db->db, &_db->const_name, EA_CONST_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->const_id, EA_CONST_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->skill_name, EA_SKILL_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->skill_id, EA_SKILL_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_name, EA_ITEM_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_id, EA_ITEM_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->mob_id, EA_MOB_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->merc_id, EA_MERC_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->pet_id, EA_PET_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->produce_id, EA_PRODUCE_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_group_name, EA_ITEM_GROUP_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_group_id, EA_ITEM_GROUP_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_group_record, EA_ITEM_GROUP_ID_RECORD))
                goto failed;
            break;
        case RATHENA:
            if(init_db_prep_sql(_db->db, &_db->item_db, RA_ITEM_ITERATE) ||
               init_db_prep_sql(_db->db, &_db->const_name, RA_CONST_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->const_id, RA_CONST_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->skill_name, RA_SKILL_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->skill_id, RA_SKILL_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_name, RA_ITEM_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_id, RA_ITEM_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->mob_id, RA_MOB_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->merc_id, RA_MERC_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->pet_id, RA_PET_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->produce_id, RA_PRODUCE_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_group_name, RA_ITEM_GROUP_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_group_id, RA_ITEM_GROUP_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_group_record, RA_ITEM_GROUP_ID_RECORD) ||
               init_db_prep_sql(_db->db, &_db->item_combo, RA_ITEM_COMBO_ID_SEARCH))
                goto failed;
            break;
        case HERCULE:
            if(init_db_prep_sql(_db->db, &_db->item_db, HE_ITEM_ITERATE) ||
               init_db_prep_sql(_db->db, &_db->const_name, HE_CONST_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->const_id, HE_CONST_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->skill_name, HE_SKILL_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->skill_id, HE_SKILL_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_name, HE_ITEM_NAME_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_id, HE_ITEM_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->mob_id, HE_MOB_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->merc_id, HE_MERC_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->pet_id, HE_PET_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->produce_id, HE_PRODUCE_ID_SEARCH) ||
               init_db_prep_sql(_db->db, &_db->item_combo, HE_ITEM_COMBO_ID_SEARCH))
                goto failed;
            break;
        default:
            fprintf(stderr, "[fail]: server type (%d) is unidentified.\n", server_type);
            return CHECK_FAILED;
    }

    *db = _db;
    return CHECK_PASSED;

failed:
    deit_db_load(&_db);
    return CHECK_FAILED;
}

int deit_db_load(db_t ** db) {
    db_t * _db = NULL;

    exit_null_safe(2, db, *db);

    _db = *db;
    if((_db->item_combo && deit_db_prep_sql(_db->db, &_db->item_combo)) ||
       (_db->item_group_record && deit_db_prep_sql(_db->db, &_db->item_group_record)) ||
       (_db->item_group_id && deit_db_prep_sql(_db->db, &_db->item_group_id)) ||
       (_db->item_group_name && deit_db_prep_sql(_db->db, &_db->item_group_name)) ||
       (_db->produce_id && deit_db_prep_sql(_db->db, &_db->produce_id)) ||
       (_db->pet_id && deit_db_prep_sql(_db->db, &_db->pet_id)) ||
       (_db->merc_id && deit_db_prep_sql(_db->db, &_db->merc_id)) ||
       (_db->mob_id && deit_db_prep_sql(_db->db, &_db->mob_id)) ||
       (_db->item_id && deit_db_prep_sql(_db->db, &_db->item_id)) ||
       (_db->item_name && deit_db_prep_sql(_db->db, &_db->item_name)) ||
       (_db->skill_id && deit_db_prep_sql(_db->db, &_db->skill_id)) ||
       (_db->skill_name && deit_db_prep_sql(_db->db, &_db->skill_name)) ||
       (_db->const_id && deit_db_prep_sql(_db->db, &_db->const_id)) ||
       (_db->const_name && deit_db_prep_sql(_db->db, &_db->const_name)) ||
       (_db->item_db && deit_db_prep_sql(_db->db, &_db->item_db)) ||
       (_db->usprite_id && deit_db_prep_sql(_db->re, &_db->usprite_id)) ||
       (_db->sprite_id && deit_db_prep_sql(_db->re, &_db->sprite_id)) ||
       (_db->block_name && deit_db_prep_sql(_db->re, &_db->block_name)) ||
       (_db->var_name && deit_db_prep_sql(_db->re, &_db->var_name)) ||
       (_db->status_name && deit_db_prep_sql(_db->re, &_db->status_name)) ||
       (_db->bonus_name && deit_db_prep_sql(_db->re, &_db->bonus_name)) ||
       (_db->map_id && deit_db_prep_sql(_db->re, &_db->map_id)) ||
       (_db->map_name && deit_db_prep_sql(_db->re, &_db->map_name)) ||
       (_db->opt_name && deit_db_prep_sql(_db->re, &_db->opt_name)) ||
       (_db->re && deit_db(&_db->re)) ||
       (_db->db && deit_db(&_db->db)))
        return CHECK_FAILED;

    SAFE_FREE(_db);
    *db = NULL;
    return CHECK_PASSED;
}

int init_db_prep_sql(sqlite3 * db, sql_t ** sql, const char * query) {
    int len = 0;
    sql_t * _sql = NULL;
    const char * tail = NULL;

    exit_null_safe(3, db, sql, query);

    _sql = calloc(1, sizeof(sql_t));
    if(NULL == _sql)
        goto failed;

    _sql->query = convert_stringn(query, &len);
    if(NULL == _sql->query)
        goto failed;

    if(SQLITE_OK != sqlite3_prepare_v2(db, _sql->query, len + 1, &_sql->stmt, &tail))
        goto failed;

    *sql = _sql;
    return CHECK_PASSED;

failed:
    fprintf(stderr, "[load]: failed to compile sqlite3 query; %s.\n", query);

    if(NULL != tail)
        fprintf(stderr, "[load]: error occurred near; %s.\n", tail);

    if(NULL != _sql)
        deit_db_prep_sql(db, &_sql);

    return CHECK_FAILED;
}


int deit_db_prep_sql(sqlite3 * db, sql_t ** sql) {
    sql_t * _sql = NULL;

    exit_null_safe(3, db, sql, *sql);

    _sql = *sql;
    if(NULL != _sql->stmt)
        if(SQLITE_OK != sqlite3_finalize(_sql->stmt))
            fprintf(stderr, "[load]: failed to finalize sqlite3 quer"
            "y; %s.\n[load]: %s.\n", _sql->query, sqlite3_errmsg(db));
    SAFE_FREE(_sql->query);
    SAFE_FREE(_sql);
    *sql = NULL;
    return CHECK_PASSED;
}

int exec_db_query(sqlite3 * db, sql_t * sql, ...) {
    char * query = NULL;
    sqlite3_stmt * stmt = NULL;
    va_list args;

    exit_null_safe(2, db, sql);

    query = sql->query;
    stmt = sql->stmt;

    /* reset the statement and bind the string */
    if(SQLITE_OK != sqlite3_reset(stmt) ||
       SQLITE_OK != sqlite3_clear_bindings(stmt)) {
       fprintf(stderr, "[fail]: failed to reset statement; %s"
        ".\n[fail]: %s query.\n", sqlite3_errmsg(db), query);
    }


    /* bind the statements */
    va_start(args, sql);
    if(bind_db_query(db, stmt, args)) {
        fprintf(stderr, "[fail]: failed to bind %s.\n", query);
        va_end(args);
        return CHECK_FAILED;
    }
    va_end(args);

    /* execute the sql statement */
    switch(sqlite3_step(stmt)) {
        case SQLITE_DONE:
            /* query returned no results */
            return CHECK_FAILED;
        case SQLITE_ROW:
            /* query returned one or many results */
            return CHECK_PASSED;
        default:
            fprintf(stderr, "[fail]: failed to execute sql statement"
            "; %s.\n[fail]: %s query.\n", sqlite3_errmsg(db), query);
            return CHECK_FAILED;
    }
}

int bind_db_query(sqlite3 * db, sqlite3_stmt * stmt, va_list args) {
    int type = 0;
    int count = 0;

    /* get the total number of arguments;
     * adjusted by one because bind args
     * for sqlite3 start at 1 not 0. */
    count = va_arg(args, int) + 1;

    /* bind all the arguments */
    for(int i = 1; i < count; i++) {
        /* get the type of argument */
        type = va_arg(args, int);
        switch(type) {
            case BIND_STRING:
                if(bind_db_string(db, stmt, i, args))
                    return CHECK_FAILED;
#if defined(_WIN32) || defined(_WIN64)
                va_arg(args, const char *);
                va_arg(args, int);
#endif
                break;
            case BIND_NUMBER:
                if(bind_db_number(db, stmt, i, args))
                    return CHECK_FAILED;
#if defined(_WIN32) || defined(_WIN64)
                va_arg(args, int);
#endif
                break;
            default:
                return CHECK_FAILED;
        }
    }

    return CHECK_PASSED;
}

int bind_db_string(sqlite3 * db, sqlite3_stmt * stmt, int bind, va_list args) {
    int len = 0;
    const char * name = NULL;

    name = va_arg(args, const char *);
    len = va_arg(args, int);

    if(SQLITE_OK != sqlite3_bind_text(stmt, bind, name, len, SQLITE_STATIC)) {
        fprintf(stderr, "[fail]: failed to bind the name '%s' "
        "of length '%d'; %s.\n", name, len, sqlite3_errmsg(db));
        return CHECK_FAILED;
    }

    return CHECK_PASSED;
}

int bind_db_number(sqlite3 * db, sqlite3_stmt * stmt, int bind, va_list args) {
    int id = 0;

    id = va_arg(args, int);

    if(SQLITE_OK != sqlite3_bind_int(stmt, bind, id)) {
        fprintf(stderr, "[fail]: failed to bind the "
        "integer '%d'; %s.\n", id, sqlite3_errmsg(db));
        return CHECK_FAILED;
    }

    return CHECK_PASSED;
}

int opt_name(db_t * db, option_res * opt, const char * name, int len) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(3, db, opt, name);
    if(exec_db_query(db->re, db->opt_name, 1, BIND_STRING, name, len))
        return CHECK_FAILED;

    stmt = db->opt_name->stmt;
    strncopy(opt->option, MAX_NAME_SIZE, sqlite3_column_text(stmt, 0));
    strncopy(opt->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    opt->flag = sqlite3_column_int(stmt, 2);
    return CHECK_PASSED;
}

int map_name(db_t * db, map_res * map, const char * name, int len) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(3, db, map, name);

    if(exec_db_query(db->re, db->map_name, 1, BIND_STRING, name, len))
        return CHECK_FAILED;

    stmt = db->map_name->stmt;
    map->id = sqlite3_column_int(stmt, 0);
    strncopy(map->map, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    strncopy(map->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 2));
    return CHECK_PASSED;
}

int map_id(db_t * db, map_res * map, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, map);

    if (exec_db_query(db->re, db->map_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->map_id->stmt;
    map->id = sqlite3_column_int(stmt, 0);
    strncopy(map->map, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    strncopy(map->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 2));
    return CHECK_PASSED;
}

int bonus_name(db_t * db, bonus_res * bonus, const char * prefix, int plen, const char * name, int nlen) {
    int cnt = 0;
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(4, db, bonus, prefix, name);
    if(exec_db_query(db->re, db->bonus_name, 2, BIND_STRING, prefix, plen, BIND_STRING, name, nlen))
        return CHECK_FAILED;

    stmt = db->bonus_name->stmt;
    bonus->id = sqlite3_column_int(stmt, 0);
    bonus->flag = sqlite3_column_int(stmt, 1);
    bonus->attr = sqlite3_column_int(stmt, 2);
    strncopy(bonus->prefix, MAX_NAME_SIZE, sqlite3_column_text(stmt, 3));
    strncopy(bonus->bonus, MAX_NAME_SIZE, sqlite3_column_text(stmt, 4));
    strncopy(bonus->format, MAX_NAME_SIZE, sqlite3_column_text(stmt, 5));
    bonus->type_cnt = sqlite3_column_int(stmt, 6);
    if (convert_integer_delimit_static((const char *)sqlite3_column_text(stmt, 7), ":", bonus->type, MAX_BONUS, &cnt))
        return CHECK_FAILED;
    bonus->order_cnt = sqlite3_column_int(stmt, 8);
    if (convert_integer_delimit_static((const char *)sqlite3_column_text(stmt, 9), ":", bonus->order, MAX_BONUS, &cnt))
        return CHECK_FAILED;
    return CHECK_PASSED;
}

int status_id(db_t * db, status_res * status, int id) {
    int cnt = 0;
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, status);

    if(exec_db_query(db->re, db->status_name, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->status_name->stmt;
    status->id = sqlite3_column_int(stmt, 0);
    strncopy(status->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    status->val1 = sqlite3_column_int(stmt, 2);
    status->val2 = sqlite3_column_int(stmt, 3);
    status->val3 = sqlite3_column_int(stmt, 4);
    status->val4 = sqlite3_column_int(stmt, 5);
    strncopy(status->format, MAX_FORMAT_SIZE, sqlite3_column_text(stmt, 6));
    status->offset_count = sqlite3_column_int(stmt, 7);
    return convert_integer_delimit_static((const char *) sqlite3_column_text(stmt, 8), ":", status->offset, status->offset_count, &cnt);
}

int var_name(db_t * db, var_res * var, const char * name, int len) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(3, db, var, name);

    if(exec_db_query(db->re, db->var_name, 1, BIND_STRING, name, len))
        return CHECK_FAILED;

    stmt = db->var_name->stmt;

    var->id = sqlite3_column_int(stmt, 0);
    var->type = sqlite3_column_int(stmt, 1);
    var->flag = sqlite3_column_int(stmt, 2);
    var->min = sqlite3_column_int(stmt, 3);
    var->max = sqlite3_column_int(stmt, 4);
    strncopy(var->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 5));
    strncopy(var->desc, MAX_NAME_SIZE, sqlite3_column_text(stmt, 6));
    return CHECK_PASSED;
}

int block_name(db_t * db, block_res * block, const char * name, int len) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(3, db, block, name);

    if(exec_db_query(db->re, db->block_name, 1, BIND_STRING, name, len))
        return CHECK_FAILED;

    stmt = db->block_name->stmt;
    block->id = sqlite3_column_int(stmt, 0);
    strncopy(block->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    block->flag = sqlite3_column_int(stmt, 2);
    return CHECK_PASSED;
}

int sprite_id(db_t * db, nid_res * sprite, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, sprite);

    if(exec_db_query(db->re, db->sprite_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->sprite_id->stmt;
    sprite->id = sqlite3_column_int(stmt, 0);
    strncopy(sprite->res, MAX_RESNAME_SIZE, sqlite3_column_text(stmt, 1));
    return CHECK_PASSED;
}

int usprite_id(db_t * db, nid_res * usprite, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, usprite);

    if(exec_db_query(db->re, db->usprite_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->usprite_id->stmt;
    usprite->id = sqlite3_column_int(stmt, 0);
    strncopy(usprite->res, MAX_RESNAME_SIZE, sqlite3_column_text(stmt, 1));
    return CHECK_PASSED;
}

int item_iterate(db_t * db, item_t * item) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, item);

    /* reset the item struct */
    memset(item, 0, sizeof(item_t));

    stmt = db->item_db->stmt;
    switch(sqlite3_step(stmt)) {
        case SQLITE_ROW:
            /* read the next item */
            switch(db->server_type) {
                case EATHENA:
                    return item_eathena(stmt, item);
                case RATHENA:
                    return item_rathena(stmt, item);
                case HERCULE:
                    return item_hercule(stmt, item);
                default:
                    /* invalid server type */
                    fprintf(stderr, "[fail]: server type (%d) is unidentified.\n", db->server_type);
                    return CHECK_FAILED;
            }
        case SQLITE_DONE:
            /* read the last item */
            return CHECK_FAILED;
        default:
            /* failed to read items */
            fprintf(stderr, "[fail]: failed to execute "
            "item query; %s.\n", sqlite3_errmsg(db->db));
            return CHECK_FAILED;
    }
}

int item_eathena(sqlite3_stmt * stmt, item_t * item) {
    item->id = sqlite3_column_int(stmt, 0);
    strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    item->type = sqlite3_column_int(stmt, 2);
    item->buy = sqlite3_column_int(stmt, 3);
    item->sell = sqlite3_column_int(stmt, 4);
    if(item->sell <= 0) item->sell = item->buy / 2;
    if(item->buy <= 0) item->buy = item->sell * 2;
    item->weight = sqlite3_column_int(stmt, 5);
    item->atk = sqlite3_column_int(stmt, 6);
    item->matk = -1;
    item->def = sqlite3_column_int(stmt, 7);
    item->range = sqlite3_column_int(stmt, 8);
    item->slots = sqlite3_column_int(stmt, 9);
    item->job = sqlite3_column_int(stmt, 10);
    item->upper = sqlite3_column_int(stmt, 11);
    item->gender = sqlite3_column_int(stmt, 12);
    item->loc = sqlite3_column_int(stmt, 13);
    item->wlv = sqlite3_column_int(stmt, 14);
    item->elv_min = sqlite3_column_int(stmt, 15);
    item->elv_max = item->elv_min;
    item->refineable = sqlite3_column_int(stmt, 16);
    item->view = sqlite3_column_int(stmt, 17);
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 18));
    /* zero out hercules only */
    item->bindonequip = -1;
    item->buyingstore = -1;
    item->delay = -1;
    memset(item->trade, 0, sizeof(int) * 10);
    memset(item->stack, 0, sizeof(int) * 2);
    return CHECK_PASSED;
}

int item_rathena(sqlite3_stmt * stmt, item_t * item) {
    item->id = sqlite3_column_int(stmt, 0);
    strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    item->type = sqlite3_column_int(stmt, 2);
    item->buy = sqlite3_column_int(stmt, 3);
    item->sell = sqlite3_column_int(stmt, 4);
    if(item->sell <= 0) item->sell = item->buy / 2;
    if(item->buy <= 0) item->buy = item->sell * 2;
    item->weight = sqlite3_column_int(stmt, 5);
    item->matk = sqlite3_column_int(stmt, 6);
    item->atk = sqlite3_column_int(stmt, 7);
    item->def = sqlite3_column_int(stmt, 8);
    item->range = sqlite3_column_int(stmt, 9);
    item->slots = sqlite3_column_int(stmt, 10);
    item->job = sqlite3_column_int(stmt, 11);
    item->upper = sqlite3_column_int(stmt, 12);
    item->gender = sqlite3_column_int(stmt, 13);
    item->loc = sqlite3_column_int(stmt, 14);
    item->wlv = sqlite3_column_int(stmt, 15);
    item->elv_min = sqlite3_column_int(stmt, 16);
    item->elv_max = item->elv_min;
    item->refineable = sqlite3_column_int(stmt, 17);
    item->view = sqlite3_column_int(stmt, 18);
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 19));
    /* zero out hercules only */
    item->bindonequip = -1;
    item->buyingstore = -1;
    item->delay = -1;
    memset(item->trade, 0, sizeof(int) * 10);
    memset(item->stack, 0, sizeof(int) * 2);
    return CHECK_PASSED;
}

int item_hercule(sqlite3_stmt * stmt, item_t * item) {
    item->id = sqlite3_column_int(stmt, 0);
    strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    item->type = sqlite3_column_int(stmt, 2);
    item->buy = sqlite3_column_int(stmt, 3);
    item->sell = sqlite3_column_int(stmt, 4);
    if(item->sell <= 0) item->sell = item->buy / 2;
    if(item->buy <= 0) item->buy = item->sell * 2;
    item->weight = sqlite3_column_int(stmt, 5);
    item->matk = sqlite3_column_int(stmt, 6);
    item->atk = sqlite3_column_int(stmt, 7);
    item->def = sqlite3_column_int(stmt, 8);
    item->range = sqlite3_column_int(stmt, 9);
    item->slots = sqlite3_column_int(stmt, 10);
    item->job = sqlite3_column_int(stmt, 11);
    item->upper = sqlite3_column_int(stmt, 12);
    item->gender = sqlite3_column_int(stmt, 13);
    item->loc = sqlite3_column_int(stmt, 14);
    item->wlv = sqlite3_column_int(stmt, 15);
    item->elv_min = sqlite3_column_int(stmt, 16);
    item->elv_max = item->elv_min;
    item->refineable = sqlite3_column_int(stmt, 17);
    item->view = sqlite3_column_int(stmt, 18);
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 19));
    /* hercules only */
    item->bindonequip = sqlite3_column_int(stmt, 20);
    item->buyingstore = sqlite3_column_int(stmt, 21);
    item->delay = sqlite3_column_int(stmt, 22);
    item->trade[0] = sqlite3_column_int(stmt, 23);
    item->trade[1] = sqlite3_column_int(stmt, 24);
    item->trade[2] = sqlite3_column_int(stmt, 25);
    item->trade[3] = sqlite3_column_int(stmt, 26);
    item->trade[4] = sqlite3_column_int(stmt, 27);
    item->trade[5] = sqlite3_column_int(stmt, 28);
    item->trade[6] = sqlite3_column_int(stmt, 29);
    item->trade[7] = sqlite3_column_int(stmt, 30);
    item->trade[8] = sqlite3_column_int(stmt, 31);
    item->trade[9] = sqlite3_column_int(stmt, 32);
    item->stack[0] = sqlite3_column_int(stmt, 33);
    item->stack[1] = sqlite3_column_int(stmt, 34);
    return CHECK_PASSED;
}

int const_name(db_t * db, const_t * constant, const char * name, int len) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(3, db, constant, name);

    if(exec_db_query(db->db, db->const_name, 1, BIND_STRING, name, len))
        return CHECK_FAILED;

    stmt = db->const_name->stmt;
    strncopy(constant->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 0));
    constant->value = sqlite3_column_int(stmt, 1);
    constant->type = sqlite3_column_int(stmt, 2);
    return CHECK_PASSED;
}

int const_id(db_t * db, const_t * constant, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, constant);

    if(exec_db_query(db->db, db->const_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->const_id->stmt;
    strncopy(constant->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 0));
    constant->value = sqlite3_column_int(stmt, 1);
    constant->type = sqlite3_column_int(stmt, 2);
    return CHECK_PASSED;
}

int skill_name(db_t * db, skill_t * skill, const char * name, int len) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(3, db, skill, name);

    if(exec_db_query(db->db, db->skill_name, 1, BIND_STRING, name, len))
        return CHECK_FAILED;

    stmt = db->skill_name->stmt;
    skill->id = sqlite3_column_int(stmt, 0);
    skill->maxlv = sqlite3_column_int(stmt, 1);
    strncopy(skill->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 2));
    strncopy(skill->desc, MAX_NAME_SIZE, sqlite3_column_text(stmt, 3));
    return CHECK_PASSED;
}

int skill_id(db_t * db, skill_t * skill, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, skill);

    if(exec_db_query(db->db, db->skill_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->skill_id->stmt;
    skill->id = sqlite3_column_int(stmt, 0);
    skill->maxlv = sqlite3_column_int(stmt, 1);
    strncopy(skill->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 2));
    strncopy(skill->desc, MAX_NAME_SIZE, sqlite3_column_text(stmt, 3));
    return CHECK_PASSED;
}

int item_name(db_t * db, item_t * item, const char * name, int len) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(3, db, item, name);

    if(exec_db_query(db->db, db->item_name, 2, BIND_STRING, name, len, BIND_STRING, name, len))
        return CHECK_FAILED;

    stmt = db->item_name->stmt;
    item->id = sqlite3_column_int(stmt, 0);
    strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 2));
    return CHECK_PASSED;
}

int item_id(db_t * db, item_t * item, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, item);

    if(exec_db_query(db->db, db->item_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->item_id->stmt;
    item->id = sqlite3_column_int(stmt, 0);
    strncopy(item->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    strncopy(item->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 2));
    return CHECK_PASSED;
}

int mob_id(db_t * db, mob_t * mob, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, mob);

    if(exec_db_query(db->db, db->mob_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->mob_id->stmt;
    mob->id = sqlite3_column_int(stmt, 0);
    strncopy(mob->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    return CHECK_PASSED;
}

int merc_id(db_t * db, merc_t * merc, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, merc);

    if(exec_db_query(db->db, db->merc_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->merc_id->stmt;
    merc->id = sqlite3_column_int(stmt, 0);
    strncopy(merc->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    return CHECK_PASSED;
}

int pet_id(db_t * db, pet_t * pet, int id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, pet);

    if(exec_db_query(db->db, db->pet_id, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->pet_id->stmt;
    pet->mob_id = sqlite3_column_int(stmt, 0);
    strncopy(pet->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 1));
    strncopy(pet->pet_script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 2));
    strncopy(pet->loyal_script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 3));
    return CHECK_PASSED;
}

int produce_id(db_t * db, produce_t ** produces, int item_level) {
    sqlite3_stmt * stmt = NULL;
    produce_t * temp = NULL;
    produce_t * root = NULL;
    produce_t * iter = NULL;

    exit_null_safe(2, db, produces);

    if(exec_db_query(db->db, db->produce_id, 1, BIND_NUMBER, item_level))
        return CHECK_FAILED;

    stmt = db->produce_id->stmt;
    do {
        temp = calloc(1, sizeof(produce_t));

        temp->item_id = sqlite3_column_int(stmt, 0);
        temp->item_lv = sqlite3_column_int(stmt, 1);
        temp->skill_id = sqlite3_column_int(stmt, 2);
        temp->skill_lv = sqlite3_column_int(stmt, 3);
        if (convert_integer_delimit_static((const char *)sqlite3_column_text(stmt, 4), ":", temp->item_id_req, MAX_INGREDIENT, &temp->ingredient_count) ||
            convert_integer_delimit_static((const char *)sqlite3_column_text(stmt, 5), ":", temp->item_amount_req, MAX_INGREDIENT, &temp->ingredient_count))
            return CHECK_FAILED;
        temp->next = NULL;
        (root == NULL) ?
            (root = iter = temp) :
            (iter = iter->next = temp);
    } while(SQLITE_DONE != sqlite3_step(stmt));

    *produces = root;
    return CHECK_PASSED;
}

int produce_free(produce_t ** produces) {
    produce_t * temporary = NULL;
    produce_t * _produces = NULL;

    exit_null_safe(2, produces, *produces);

    _produces = *produces;
    while(_produces != NULL) {
        temporary = _produces;
        _produces = _produces->next;
        free(temporary);
    }
    *produces = NULL;
    return CHECK_PASSED;
}

int item_group_name(db_t * db, const_t * group_name, int group_id) {
    sqlite3_stmt * stmt = NULL;

    /* error on invalid reference */
    exit_null_safe(2, db, group_name);

    if(exec_db_query(db->db, db->item_group_name, 1, BIND_NUMBER, group_id))
        return CHECK_FAILED;

    stmt = db->item_group_name->stmt;
    strncopy(group_name->name, MAX_NAME_SIZE, sqlite3_column_text(stmt, 0));
    group_name->value = sqlite3_column_int(stmt, 1);
    group_name->type = sqlite3_column_int(stmt, 2);
    return CHECK_PASSED;
}

int item_group_id(db_t * db, item_group_t * item_group, int group_id, int subgroup_id) {
    int i = 0;
    sqlite3_stmt * stmt = NULL;
    item_group_meta_t meta;

    /* get the group size for the group id */
    exit_null_safe(2, db, item_group);

    /* get the item group metadata */
    switch(db->server_type) {
        case EATHENA:
            if(item_group_id_ea(db, item_group, group_id))
                return CHECK_FAILED;
            break;
        case RATHENA:
            memset(&meta, 0, sizeof(item_group_meta_t));
            if(item_group_id_ra(db, &meta, group_id, subgroup_id))
                return CHECK_FAILED;
            item_group->group_id = meta.group_id;
            item_group->size = meta.item;
            break;
        default:
            return exit_func_safe("%d server type is not supported", db->server_type);
    }

    /* error on empty item group */
    if(0 == item_group->size)
        return CHECK_FAILED;

    /* create array for item id and rate */
    item_group->item_id = malloc(item_group->size * sizeof(int));
    item_group->rate = malloc(item_group->size * sizeof(int));
    if(NULL == item_group->item_id ||
       NULL == item_group->rate) {
        item_group_free(item_group);
        return CHECK_FAILED;
    }

    /* read item id and rate from item group */
    switch(db->server_type) {
        case EATHENA:
            if(exec_db_query(db->db, db->item_group_record, 1, BIND_NUMBER, group_id)) {
                item_group_free(item_group);
                return CHECK_FAILED;
            }
            break;
        case RATHENA:
            if(exec_db_query(db->db, db->item_group_record, 2, BIND_NUMBER, group_id, BIND_NUMBER, subgroup_id)) {
                item_group_free(item_group);
                return CHECK_FAILED;
            }
            break;
        default:
            return exit_func_safe("%d server type is not supported", db->server_type);
    }

    stmt = db->item_group_record->stmt;
    do {
        item_group->item_id[i] = sqlite3_column_int(stmt, 1);
        item_group->rate[i] = sqlite3_column_int(stmt, 2);
        i++;
    } while(SQLITE_DONE != sqlite3_step(stmt));

    return CHECK_PASSED;
}

int item_group_free(item_group_t * item_group) {
    exit_null_safe(1, item_group);

    SAFE_FREE(item_group->item_id);
    SAFE_FREE(item_group->rate);
    return CHECK_PASSED;
}

int item_group_id_ea(db_t * db, item_group_t * meta, int group_id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, meta);

    if(exec_db_query(db->db, db->item_group_id, 1, BIND_NUMBER, group_id))
        return CHECK_FAILED;

    stmt = db->item_group_id->stmt;
    meta->group_id = sqlite3_column_int(stmt, 0);
    meta->size = sqlite3_column_int(stmt, 1);
    return CHECK_PASSED;
}

int item_group_id_ra(db_t * db, item_group_meta_t * meta, int group_id, int subgroup_id) {
    sqlite3_stmt * stmt = NULL;

    exit_null_safe(2, db, meta);

    if(exec_db_query(db->db, db->item_group_id, 2, BIND_NUMBER, group_id, BIND_NUMBER, subgroup_id))
        return CHECK_FAILED;

    stmt = db->item_group_id->stmt;
    meta->group_id = sqlite3_column_int(stmt, 0);
    meta->subgroup_id = sqlite3_column_int(stmt, 1);
    meta->item = sqlite3_column_int(stmt, 2);
    meta->heal = sqlite3_column_int(stmt, 3);
    meta->usable = sqlite3_column_int(stmt, 4);
    meta->etc = sqlite3_column_int(stmt, 5);
    meta->armor = sqlite3_column_int(stmt, 6);
    meta->weapon = sqlite3_column_int(stmt, 7);
    meta->card = sqlite3_column_int(stmt, 8);
    meta->pet = sqlite3_column_int(stmt, 9);
    meta->pet_equip = sqlite3_column_int(stmt, 10);
    meta->ammo = sqlite3_column_int(stmt, 11);
    meta->delay_usable = sqlite3_column_int(stmt, 12);
    meta->shadow = sqlite3_column_int(stmt, 13);
    meta->confirm_usable = sqlite3_column_int(stmt, 14);
    meta->bind = sqlite3_column_int(stmt, 15);
    meta->rent = sqlite3_column_int(stmt, 16);
    return CHECK_PASSED;
}


int item_combo_id(db_t * db, combo_t ** item_combos, int id) {
    sqlite3_stmt * stmt = NULL;
    combo_t * temp = NULL;
    combo_t * root = NULL;
    combo_t * iter = NULL;

    exit_null_safe(2, db, item_combos);

    if(exec_db_query(db->db, db->item_combo, 1, BIND_NUMBER, id))
        return CHECK_FAILED;

    stmt = db->item_combo->stmt;

    do {
        temp = calloc(1, sizeof(combo_t));
        strncopy(temp->script, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 0));
        strncopy(temp->group, MAX_SCRIPT_SIZE, sqlite3_column_text(stmt, 1));
        (root == NULL)?
            (root = iter = temp) :
            (iter = iter->next = temp);
    } while(SQLITE_DONE != sqlite3_step(stmt));

    *item_combos = root;
    return CHECK_PASSED;
}

int item_combo_free(combo_t ** item_combos) {
    combo_t * temporary = NULL;
    combo_t * _item_combos = NULL;

    exit_null_safe(2, item_combos, *item_combos);

    _item_combos = *item_combos;
    while(_item_combos != NULL) {
        temporary = _item_combos;
        _item_combos = _item_combos->next;
        free(temporary);
    }

    *item_combos = NULL;
    return CHECK_PASSED;
}

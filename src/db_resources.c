/*
 *   file: db_resources.c
 *   date: 03/07/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "db_resources.h"

native_config_t load_res_native[RESOURCE_DB_COUNT] = {
   { option_res_load, sentinel_newline, delimit_cvs, CHECK_QUOTE | SKIP_NEXT_WS | CHECK_FIELD_COUNT, OPTION_RES_FIELD_COUNT, sizeof(option_res) },
   { map_res_load, sentinel_newline, delimit_cvs_pound, SKIP_NEXT_WS | CHECK_FIELD_COUNT, MAP_RES_FIELD_COUNT, sizeof(map_res) },
   { bonus_res_load, sentinel_newline, delimit_cvs, CHECK_QUOTE | SKIP_NEXT_WS , 0, sizeof(bonus_res) },
   { status_res_load, sentinel_newline, delimit_cvs, CHECK_QUOTE | SKIP_NEXT_WS | CHECK_FIELD_COUNT, STATUS_RES_FIELD_COUNT, sizeof(status_res) },
   { var_res_load, sentinel_semicolon, delimit_cvs_semicolon, CHECK_QUOTE | SKIP_NEXT_WS | CHECK_FIELD_COUNT, VAR_RES_FIELD_COUNT, sizeof(var_res) },
   { block_res_load, sentinel_semicolon, delimit_cvs_semicolon, CHECK_QUOTE | SKIP_NEXT_WS | CHECK_FIELD_COUNT, BLOCK_RES_FIELD_COUNT, sizeof(block_res) },
   { nid_res_load, sentinel_newline, delimit_cvs_pound, SKIP_NEXT_WS | CHECK_FIELD_COUNT, NID_RES_FIELD_COUNT, sizeof(nid_res) },
   { nid_res_load, sentinel_newline, delimit_cvs_pound, SKIP_NEXT_WS | CHECK_FIELD_COUNT, NID_RES_FIELD_COUNT, sizeof(nid_res) }
};

int option_res_load(void * db, int row, int col, char * val) {
    option_res * record = &((option_res *) db)[row];
    switch(col) {
        case 0: strnload(record->option, MAX_NAME_SIZE, val); break;
        case 1: strnload(record->name, MAX_NAME_SIZE, val);   break;
        case 2: record->flag = convert_integer(val, 16);      break;
        default: exit_func_safe("invalid column field %d in option database", col);
    }
    return 0;
}

int map_res_load(void * db, int row, int col, char * val) {
    int i = 0;
    map_res * record = &((map_res *) db)[row];
    record->id = row;
    switch(col) {
        case 0:
            /* remove the file extension for the map name */
            for(i = strlen(val); i >= 0; i--) if(val[i] == '.') { val[i] = '\0'; break; }
            strnload(record->map, MAX_NAME_SIZE, val);         break;
        case 1: strnload(record->name, MAX_NAME_SIZE, val); break;
        default: exit_func_safe("invalid column field %d in map database", col);
    }
    return 0;
}

int nid_res_load(void * db, int row, int col, char * val) {
   nid_res * record = &((nid_res *) db)[row];
   switch(col) {
      case 0: record->id = convert_integer(val, 10);           break;
      case 1: strnload(record->res, MAX_RESNAME_SIZE, val);    break;
      default: exit_func_safe("invalid column field %d in numid or id resource database", col);
   }
   return 0;
}

int bonus_res_load(void * db, int row, int col, char * val) {
   /* loader is non-reentrant */
    static int type_cnt = 0;
   static int order_cnt = 0;
   static int var_state = 0;
   bonus_res * record = &((bonus_res *) db)[row];
   switch(col) {
      case 0:
         type_cnt = 0;
         order_cnt = 0;
         var_state = 0;
         record->id = convert_integer(val, 10);                break;
      case 1: record->flag = convert_integer(val, 16);         break;
      case 2: record->attr = convert_integer(val, 10);         break;
      case 3: strnload(record->prefix, MAX_NAME_SIZE, val);    break;
      case 4: strnload(record->bonus, MAX_NAME_SIZE, val);     break;
      case 5: strnload(record->format, MAX_NAME_SIZE, val);    break;
      case 6: record->type_cnt = convert_integer(val, 10);     break;
      default:
         if(var_state < record->type_cnt) {   /* read the types */
            record->type[type_cnt] = val[0];
            var_state++;
            type_cnt++;
         } else if(var_state == record->type_cnt) {   /* initialize order count */
            record->order_cnt = convert_integer(val, 10);
            var_state++;
         } else {                                     /* read the orders */
            record->order[order_cnt] = convert_integer(val, 10);
            order_cnt++;
         }
         break;
   }
   return 0;
}

int status_res_load(void * db, int row, int col, char * val) {
   /* loader is non-reentrant */
   static int split_cnt = 0;
   status_res * record = &((status_res *) db)[row];
   switch(col) {
      case 0:
         split_cnt = 0;
         record->scid = convert_integer(val,10);               break;
      case 1: strnload(record->scstr, MAX_NAME_SIZE, val);     break;
      case 2: record->type = convert_integer(val,10);          break;
      case 3: strnload(record->scfmt, MAX_FORMAT_SIZE, val);   break;
      case 4: strnload(record->scend, MAX_FORMAT_SIZE, val);   break;
      case 5: record->vcnt = convert_integer(val,10);          break;
      default:
         if(split_cnt < 4) {
            record->vmod[split_cnt] = (int) val[0];
            split_cnt++;
         } else {
            record->voff[split_cnt-4] = convert_integer(val,10);
            split_cnt++;
         }
         break;
   }
   return 0;
}

int var_res_load(void * db, int row, int col, char * val) {
   var_res * record = &((var_res *) db)[row];
   switch(col) {
      case 0: record->id = convert_integer(val, 10);      break;
      case 1: record->type = convert_integer(val, 16);    break;
      case 2: record->flag = convert_integer(val, 16);    break;
      case 3: record->min = convert_integer(val, 10);     break;
      case 4: record->max = convert_integer(val, 10);     break;
      case 5: strnload(record->name, MAX_NAME_SIZE, val); break;
      case 6: strnload(record->desc, MAX_NAME_SIZE, val); break;
      default: exit_func_safe("invalid column field %d in variable and function database", col);
   }
   return 0;
}

int block_res_load(void * db, int row, int col, char * val) {
   block_res * record = &((block_res *) db)[row];
   switch(col) {
      case 0: record->id = convert_integer(val, 10); break;
      case 1: strnload(record->name, MAX_NAME_SIZE, val); break;
      case 2: record->flag = convert_integer(val, 10); break;
      default:  return 0;
   }
   return 0;
}

int opt_db_init(opt_db_t ** opt, const char * path) {
   char * error = NULL;
   opt_db_t * _opt = NULL;

   exit_null_safe(2, opt, path);

   _opt = calloc(1, sizeof(opt_db_t));
   if(NULL == _opt)
      goto failed;

   if(SQLITE_OK != sqlite3_open_v2(path, &_opt->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL)) {
      if(NULL != _opt->db)
         fprintf(stderr, "[load]: failed to create %s; %s.\n", path, sqlite3_errmsg(_opt->db));
      goto failed;
   }

   if(SQLITE_OK != sqlite3_exec(_opt->db,
      RES_OPT_DELETE
      RES_OPT_CREATE
      RES_MAP_DELETE
      RES_MAP_CREATE
      RES_BNS_DELETE
      RES_BNS_CREATE
      RES_STA_DELETE
      RES_STA_CREATE
      RES_VAR_DELETE
      RES_VAR_CREATE
      RES_BLK_DELETE
      RES_BLK_CREATE
      RES_RID_DELETE
      RES_RID_CREATE
      RES_NID_DELETE
      RES_NID_CREATE,
      NULL, NULL, &error)) {
      if(NULL != error) {
         fprintf(stderr, "[load]: failed to load schema %s; %s.\n", path, error);
         sqlite3_free(error);
      }
      goto failed;
   }

   if(SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_OPT_INSERT, strlen(RES_OPT_INSERT), &_opt->option_sql_insert, NULL) ||
      SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_MAP_INSERT, strlen(RES_MAP_INSERT), &_opt->map_sql_insert, NULL) ||
      SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_BNS_INSERT, strlen(RES_BNS_INSERT), &_opt->bonus_sql_insert, NULL) ||
      SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_STA_INSERT, strlen(RES_STA_INSERT), &_opt->status_sql_insert, NULL) ||
      SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_VAR_INSERT, strlen(RES_VAR_INSERT), &_opt->var_sql_insert, NULL) ||
      SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_BLK_INSERT, strlen(RES_BLK_INSERT), &_opt->block_sql_insert, NULL) ||
      SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_RID_INSERT, strlen(RES_RID_INSERT), &_opt->id_res_sql_insert, NULL) ||
      SQLITE_OK != sqlite3_prepare_v2(_opt->db, RES_NID_INSERT, strlen(RES_NID_INSERT), &_opt->numid_res_sql_insert, NULL)) {
      fprintf(stderr, "[load]: failed prepare sql statments; %s.\n", sqlite3_errmsg(_opt->db));
      goto failed;
   }

   *opt = _opt;
   return CHECK_PASSED;

failed:
   opt_db_deit(&_opt);
   return CHECK_FAILED;
}

int opt_db_deit(opt_db_t ** opt) {
   opt_db_t * _opt = NULL;

   exit_null_safe(2, opt, *opt);

   _opt = *opt;
   if( SQLITE_OK != sqlite3_finalize(_opt->option_sql_insert) ||
       SQLITE_OK != sqlite3_finalize(_opt->map_sql_insert) ||
       SQLITE_OK != sqlite3_finalize(_opt->bonus_sql_insert) ||
       SQLITE_OK != sqlite3_finalize(_opt->status_sql_insert) ||
       SQLITE_OK != sqlite3_finalize(_opt->var_sql_insert) ||
       SQLITE_OK != sqlite3_finalize(_opt->block_sql_insert) ||
       SQLITE_OK != sqlite3_finalize(_opt->id_res_sql_insert) ||
       SQLITE_OK != sqlite3_finalize(_opt->numid_res_sql_insert) ||
       SQLITE_OK != sqlite3_close(_opt->db)) {
      exit(EXIT_FAILURE);
   }
   SAFE_FREE(_opt);
   *opt = NULL;
   return CHECK_PASSED;
}

int opt_db_exec(opt_db_t * db, char * sql) {
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

int opt_db_res_load(opt_db_t * opt, const char * path) {
   native_t options;
   options.db = NULL;
   options.size = 0;

   if(load_native(path, trim_alpha, load_native_general, &options, &load_res_native[0]) ||
      NULL == options.db ||
      0 >= options.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_res_load_record(options.db, options.size, opt->option_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load option db; %s.\n", path);
      SAFE_FREE(options.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded option db; %s.\n", path);
   SAFE_FREE(options.db);
   return CHECK_PASSED;
}

int opt_db_res_load_record(option_res * options, int size, sqlite3_stmt * sql) {
   int i = 0;
   option_res * option = NULL;

   for(i = 0; i < size; i++) {
      option = &options[i];
      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_text(sql, 1, option->option, strlen(option->option), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_text(sql, 2, option->name, strlen(option->name), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_int(sql, 3, option->flag) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to option db.\n", option->option);
          if (SQLITE_OK != sqlite3_reset(sql)) {
              fprintf(stderr, "[load]: failed to reset sql statement.\n");
              return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

int opt_db_map_load(opt_db_t * opt, const char * path) {
   native_t maps;
   maps.db = NULL;
   maps.size = 0;

   if(load_native(path, trim_general, load_native_general, &maps, &load_res_native[1]) ||
      NULL == maps.db ||
      0 >= maps.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_map_load_record(maps.db, maps.size, opt->map_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load map db; %s.\n", path);
      SAFE_FREE(maps.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded map db; %s.\n", path);
   SAFE_FREE(maps.db);
   return CHECK_PASSED;
}

int opt_db_map_load_record(map_res * maps, int size, sqlite3_stmt * sql) {
   int i = 0;
   map_res * map = NULL;

   for(i = 0; i < size; i++) {
      map = &maps[i];
      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_int(sql, 1, map->id) ||
         SQLITE_OK != sqlite3_bind_text(sql, 2, map->map, strlen(map->map), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_text(sql, 3, map->name, strlen(map->name), SQLITE_STATIC) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to map db.\n", map->map);
          if (SQLITE_OK != sqlite3_reset(sql)) {
                fprintf(stderr, "[load]: failed to reset sql statement.\n");
                return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

int opt_db_bns_load(opt_db_t * opt, const char * path) {
   native_t bonuses;
   bonuses.db = NULL;
   bonuses.size = 0;

   if(load_native(path, trim_numeric, load_native_general, &bonuses, &load_res_native[2]) ||
      NULL == bonuses.db ||
      0 >= bonuses.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_bns_load_record(bonuses.db, bonuses.size, opt->bonus_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load bonus db; %s.\n", path);
      SAFE_FREE(bonuses.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded bonus db; %s.\n", path);
   SAFE_FREE(bonuses.db);
   return CHECK_PASSED;
}

int opt_db_bns_load_record(bonus_res * bonuses, int size, sqlite3_stmt * sql) {
   int i = 0;
   char buf[BUF_SIZE];
   bonus_res * bonus = NULL;

   for(i = 0; i < size; i++) {
      bonus = &bonuses[i];

      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_int(sql, 1, bonus->id) ||
         SQLITE_OK != sqlite3_bind_int(sql, 2, bonus->flag) ||
         SQLITE_OK != sqlite3_bind_int(sql, 3, bonus->attr) ||
         SQLITE_OK != sqlite3_bind_text(sql, 4, bonus->prefix, strlen(bonus->prefix), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_text(sql, 5, bonus->bonus, strlen(bonus->bonus), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_text(sql, 6, bonus->format, strlen(bonus->format), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_int(sql, 7, bonus->type_cnt) ||
         NULL == array_to_string_cnt(buf, bonus->type, bonus->type_cnt) ||
         SQLITE_OK != sqlite3_bind_text(sql, 8, buf, strlen(buf), SQLITE_TRANSIENT) ||
         SQLITE_OK != sqlite3_bind_int(sql, 9, bonus->order_cnt) ||
         NULL == array_to_string_cnt(buf, bonus->order, bonus->order_cnt) ||
         SQLITE_OK != sqlite3_bind_text(sql, 10, buf, strlen(buf), SQLITE_TRANSIENT) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to bonus db.\n", bonus->bonus);
          if (SQLITE_OK != sqlite3_reset(sql)) {
              fprintf(stderr, "[load]: failed to reset sql statement.\n");
              return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

int opt_db_sta_load(opt_db_t * opt, const char * path) {
   native_t statuses;
   statuses.db = NULL;
   statuses.size = 0;

   if(load_native(path, trim_numeric, load_native_general, &statuses, &load_res_native[3]) ||
      NULL == statuses.db ||
      0 >= statuses.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_sta_load_record(statuses.db, statuses.size, opt->status_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load status db; %s.\n", path);
      SAFE_FREE(statuses.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded status db; %s.\n", path);
   SAFE_FREE(statuses.db);
   return CHECK_PASSED;
}

int opt_db_sta_load_record(status_res * statuses, int size, sqlite3_stmt * sql) {
   int i = 0;
   char buf[BUF_SIZE];
   status_res * status = NULL;

   for(i = 0; i < size; i++) {
      status = &statuses[i];

      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_int(sql, 1, status->scid) ||
         SQLITE_OK != sqlite3_bind_text(sql, 2, status->scstr, strlen(status->scstr), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_int(sql, 3, status->type) ||
         (status->scfmt != NULL && SQLITE_OK != sqlite3_bind_text(sql, 4, status->scfmt, strlen(status->scfmt), SQLITE_STATIC)) ||
         (status->scend != NULL && SQLITE_OK != sqlite3_bind_text(sql, 5, status->scend, strlen(status->scend), SQLITE_STATIC)) ||
         SQLITE_OK != sqlite3_bind_int(sql, 6, status->vcnt) ||
         NULL == array_to_string_cnt(buf, status->vmod, 4) ||
         SQLITE_OK != sqlite3_bind_text(sql, 7, buf, strlen(buf), SQLITE_TRANSIENT) ||
         NULL == array_to_string_cnt(buf, status->voff, 4) ||
         SQLITE_OK != sqlite3_bind_text(sql, 8, buf, strlen(buf), SQLITE_TRANSIENT) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to status db.\n", status->scstr);
          if (SQLITE_OK != sqlite3_reset(sql)) {
              fprintf(stderr, "[load]: failed to reset sql statement.\n");
              return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

int opt_db_var_load(opt_db_t * opt, const char * path) {
   native_t vars;
   vars.db = NULL;
   vars.size = 0;

   if(load_native(path, trim_numeric, load_native_general, &vars, &load_res_native[4]) ||
      NULL == vars.db ||
      0 >= vars.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_var_load_record(vars.db, vars.size, opt->var_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load variable db; %s.\n", path);
      SAFE_FREE(vars.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded variable db; %s.\n", path);
   SAFE_FREE(vars.db);
   return CHECK_PASSED;
}

int opt_db_var_load_record(var_res * vars, int size, sqlite3_stmt * sql) {
   int i = 0;
   var_res * var = NULL;

   for(i = 0; i < size; i++) {
      var = &vars[i];

      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_int(sql, 1, var->id) ||
         SQLITE_OK != sqlite3_bind_int(sql, 2, var->type) ||
         SQLITE_OK != sqlite3_bind_int(sql, 3, var->flag) ||
         SQLITE_OK != sqlite3_bind_int(sql, 4, var->min) ||
         SQLITE_OK != sqlite3_bind_int(sql, 5, var->max) ||
         SQLITE_OK != sqlite3_bind_text(sql, 6, var->name, strlen(var->name), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_text(sql, 7, var->desc, strlen(var->desc), SQLITE_STATIC) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to variable db.\n", var->name);
          if (SQLITE_OK != sqlite3_reset(sql)) {
              fprintf(stderr, "[load]: failed to reset sql statement.\n");
              return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

int opt_db_blk_load(opt_db_t * opt, const char * path) {
   native_t blocks;
   blocks.db = NULL;
   blocks.size = 0;

   if(load_native(path, trim_numeric, load_native_general, &blocks, &load_res_native[5]) ||
      NULL == blocks.db ||
      0 >= blocks.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_blk_load_record(blocks.db, blocks.size, opt->block_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load block db; %s.\n", path);
      SAFE_FREE(blocks.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded block db; %s.\n", path);
   SAFE_FREE(blocks.db);
   return CHECK_PASSED;
}

int opt_db_blk_load_record(block_res * blocks, int size, sqlite3_stmt * sql) {
   int i = 0;
   block_res * block = NULL;

   for(i = 0; i < size; i++) {
      block = &blocks[i];
      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_int(sql, 1, block->id) ||
         SQLITE_OK != sqlite3_bind_text(sql, 2, block->name, strlen(block->name), SQLITE_STATIC) ||
         SQLITE_OK != sqlite3_bind_int(sql, 3, block->flag) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to block db.\n", block->name);
          if (SQLITE_OK != sqlite3_reset(sql)) {
              fprintf(stderr, "[load]: failed to reset sql statement.\n");
              return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

int opt_db_rid_load(opt_db_t * opt, const char * path) {
   native_t rids;
   rids.db = NULL;
   rids.size = 0;

   if(load_native(path, trim_numeric, load_native_general, &rids, &load_res_native[6]) ||
      NULL == rids.db ||
      0 >= rids.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_rid_load_record(rids.db, rids.size, opt->id_res_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load rid db; %s.\n", path);
      SAFE_FREE(rids.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded rid db; %s.\n", path);
   SAFE_FREE(rids.db);
   return CHECK_PASSED;
}

int opt_db_rid_load_record(nid_res * rids, int size, sqlite3_stmt * sql) {
   int i = 0;
   nid_res * rid = NULL;

   for(i = 0; i < size; i++) {
      rid = &rids[i];
      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_int(sql, 1, rid->id) ||
         SQLITE_OK != sqlite3_bind_text(sql, 2, rid->res, strlen(rid->res), SQLITE_STATIC) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to rid db.\n", rid->res);
          if (SQLITE_OK != sqlite3_reset(sql)) {
              fprintf(stderr, "[load]: failed to reset sql statement.\n");
              return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

int opt_db_nid_load(opt_db_t * opt, const char * path) {
   native_t nids;
   nids.db = NULL;
   nids.size = 0;

   if(load_native(path, trim_numeric, load_native_general, &nids, &load_res_native[7]) ||
      NULL == nids.db ||
      0 >= nids.size ||
      opt_db_exec(opt, "BEGIN IMMEDIATE TRANSACTION;") ||
      opt_db_nid_load_record(nids.db, nids.size, opt->numid_res_sql_insert) ||
      opt_db_exec(opt, "COMMIT TRANSACTION;")) {
      opt_db_exec(opt, "ROLLBACK TRANSACTION;");
      fprintf(stderr, "[load]: failed to load nid db; %s.\n", path);
      SAFE_FREE(nids.db);
      return CHECK_FAILED;
   }
   fprintf(stdout, "[load]: loaded nid db; %s.\n", path);
   SAFE_FREE(nids.db);
   return CHECK_PASSED;
}


int opt_db_nid_load_record(nid_res * nids, int size, sqlite3_stmt * sql) {
   int i = 0;
   nid_res * nid = NULL;

   for(i = 0; i < size; i++) {
      nid = &nids[i];
      if(SQLITE_OK != sqlite3_clear_bindings(sql) ||
         SQLITE_OK != sqlite3_bind_int(sql,1,nid->id) ||
         SQLITE_OK != sqlite3_bind_text(sql,2,nid->res,strlen(nid->res),SQLITE_STATIC) ||
         SQLITE_DONE != sqlite3_step(sql) ||
         SQLITE_OK != sqlite3_reset(sql)) {
          fprintf(stderr, "[load]: failed to add %s to nid db.\n", nid->res);
          if (SQLITE_OK != sqlite3_reset(sql)) {
              fprintf(stderr, "[load]: failed to reset sql statement.\n");
              return CHECK_FAILED;
          }
      }
   }
   return CHECK_PASSED;
}

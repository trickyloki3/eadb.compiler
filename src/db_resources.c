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
   { block_res_load, sentinel_semicolon, delimit_cvs_semicolon, CHECK_QUOTE | SKIP_NEXT_WS | CHECK_FIELD_COUNT, BLOCK_RES_FIELD_COUNT, sizeof(block_res) }
};

int option_res_load(void * db, int row, int col, char * val) {
	option_res * record = &((option_res *) db)[row];
	switch(col) {
		case 0: strnload(record->option, MAX_NAME_SIZE, val); break;
		case 1: strnload(record->name, MAX_NAME_SIZE, val);   break;
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
			strnload(record->map, MAX_NAME_SIZE, val); 		break;
		case 1: strnload(record->name, MAX_NAME_SIZE, val); break;
		default: exit_func_safe("invalid column field %d in map database", col);
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
      case 0: record->tag = convert_integer(val, 10);    break;
      case 1: strnload(record->id, MAX_NAME_SIZE, val);  break;
      case 2: record->type = convert_integer(val, 16);   break;
      case 3: record->vflag = convert_integer(val, 16);  break;
      case 4: record->fflag = convert_integer(val, 16);  break;
      case 5: record->min = convert_integer(val, 10);    break;
      case 6: record->max = convert_integer(val, 10);    break;
      case 7: strnload(record->str, MAX_NAME_SIZE, val); break;
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

int load_resource_database(const char * resource_path) {
   int status = 0;

   native_t option_db;
   status = load_native("/root/Desktop/dev/eAdb.Compiler3/res/option_db.txt",
   trim_alpha, load_native_general, &option_db, &load_res_native[0]);
   if(status == CHECK_FAILED) {
      fprintf(stderr,"failed to load option database.\n");
      exit(EXIT_FAILURE);
   }
   free(option_db.db);

   native_t map_db;
   status = load_native("/root/Desktop/dev/misc/map.txt",
   trim_alpha, load_native_general, &map_db, &load_res_native[1]);
   if(status == CHECK_FAILED) {
      fprintf(stderr,"failed to load map database.\n");
      exit(EXIT_FAILURE);
   }
   free(map_db.db);

   native_t bonus_db;
   status = load_native("/root/Desktop/dev/eAdb.Compiler3/res/item_bonus.txt",
   trim_numeric, load_native_general, &bonus_db, &load_res_native[2]);
   if(status == CHECK_FAILED) {
      fprintf(stderr,"failed to load item bonus database.\n");
      exit(EXIT_FAILURE);
   }
   free(bonus_db.db);

   native_t status_db;
   status = load_native("/root/Desktop/dev/eAdb.Compiler3/res/status_db.txt",
   trim_numeric, load_native_general, &status_db, &load_res_native[3]);
   if(status == CHECK_FAILED) {
      fprintf(stderr,"failed to load item status database.\n");
      exit(EXIT_FAILURE);
   }
   free(status_db.db);

   native_t var_db;
   status = load_native("/root/Desktop/dev/eAdb.Compiler3/res/var_db.txt",
   trim_numeric, load_native_general, &var_db, &load_res_native[4]);
   if(status == CHECK_FAILED) {
      fprintf(stderr,"failed to load item variable and function database.\n");
      exit(EXIT_FAILURE);
   }
   free(var_db.db);

   native_t block_db;
   status = load_native("/root/Desktop/dev/eAdb.Compiler3/res/block_db.txt",
   trim_numeric, load_native_general, &block_db, &load_res_native[5]);
   if(status == CHECK_FAILED) {
      fprintf(stderr,"failed to load block database.\n");
      exit(EXIT_FAILURE);
   }
   free(block_db.db);

   return 0;
}

int default_resource_database(void) {
   db_res_t db;
   create_resource_database(&db, "/root/Desktop/dev/eAdb.Compiler3/resources.db");
   resource_option_sql_load(&db, "/root/Desktop/dev/eAdb.Compiler3/res/option_db.txt");
   resource_map_sql_load(&db, "/root/Desktop/dev/misc/map.txt");
   resource_bonus_sql_load(&db, "/root/Desktop/dev/eAdb.Compiler3/res/item_bonus.txt");
   resource_status_sql_load(&db, "/root/Desktop/dev/eAdb.Compiler3/res/status_db.txt");
   resource_var_sql_load(&db, "/root/Desktop/dev/eAdb.Compiler3/res/var_db.txt");
   resource_block_sql_load(&db, "/root/Desktop/dev/eAdb.Compiler3/res/block_db.txt");
   finalize_resource_database(&db);
   return CHECK_PASSED;
}


int create_resource_database(db_res_t * db, const char * path) {
   int status = 0;
   const char * error = NULL;
   char * sql_error = NULL;   
   if( /* open database connection specified by path */
      sqlite3_open(path, &db->db) != SQLITE_OK ||
      /* execute resource table creation script */
      sqlite3_exec(db->db, resource_database_sql, NULL, NULL, &sql_error) != SQLITE_OK ||
      /* prepare the insertion statements */
      sqlite3_prepare_v2(db->db, option_insert,    strlen(option_insert),  &db->option_sql_insert, NULL) != SQLITE_OK ||
      sqlite3_prepare_v2(db->db, map_insert,       strlen(map_insert),     &db->map_sql_insert,    NULL) != SQLITE_OK ||
      sqlite3_prepare_v2(db->db, bonus_insert,     strlen(bonus_insert),   &db->bonus_sql_insert,  NULL) != SQLITE_OK ||
      sqlite3_prepare_v2(db->db, status_insert,    strlen(status_insert),  &db->status_sql_insert, NULL) != SQLITE_OK ||
      sqlite3_prepare_v2(db->db, var_insert,       strlen(var_insert),     &db->var_sql_insert,    NULL) != SQLITE_OK ||
      sqlite3_prepare_v2(db->db, block_insert,     strlen(block_insert),   &db->block_sql_insert, NULL) != SQLITE_OK) {
      /* print sqlite3 error before exiting */
      status = sqlite3_errcode(db->db);
      error = sqlite3_errmsg(db->db);
      exit_func_safe("sqlite3 code %d; %s", status, (sql_error != NULL) ? sql_error : error);
      /* release the error string and database handle */
      if(sql_error != NULL) sqlite3_free(sql_error);
      if(db->db != NULL) sqlite3_close(db->db);
      return CHECK_FAILED;
   }
   return CHECK_PASSED;
}

int finalize_resource_database(db_res_t * db) {
   sqlite3_finalize(db->option_sql_insert);
   sqlite3_finalize(db->map_sql_insert);
   sqlite3_finalize(db->bonus_sql_insert);
   sqlite3_finalize(db->status_sql_insert);
   sqlite3_finalize(db->var_sql_insert);
   sqlite3_finalize(db->block_sql_insert);
   sqlite3_close(db->db);
   return CHECK_PASSED;
}

int resource_option_sql_load(db_res_t * db, const char * path) {
   int i = 0;
   native_t option_db;
   option_res * option_res_db = NULL;
   memset(&option_db, 0, sizeof(native_t));

   /* load the native database */
   if(load_native(path, trim_alpha, load_native_general, &option_db, &load_res_native[0]) == CHECK_FAILED) {
      exit_func_safe("failed to load resource option database at %s; invalid path", path);
      return CHECK_FAILED;
   }

   /* check the native database */
   if(option_db.db == NULL || option_db.size <= 0) {
      exit_func_safe("failed to load resource option database at %s; detected zero entries", path);
      return CHECK_FAILED;
   }

   /* load the native database into the sqlite3 resources database */
   sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
   for(i = 0, option_res_db = option_db.db; i < option_db.size; i++) {
      sqlite3_clear_bindings(db->option_sql_insert);
      sqlite3_bind_text(db->option_sql_insert, 1, option_res_db[i].option, strlen(option_res_db[i].option), SQLITE_STATIC);
      sqlite3_bind_text(db->option_sql_insert, 2, option_res_db[i].name,   strlen(option_res_db[i].name),   SQLITE_STATIC);
      sqlite3_step(db->option_sql_insert);
      sqlite3_reset(db->option_sql_insert);
   }
   sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
   free(option_db.db);
   return CHECK_PASSED;
}

int resource_map_sql_load(db_res_t * db, const char * path) {
   int i = 0;
   native_t map_db;
   map_res * map_res_db = NULL;
   memset(&map_db, 0, sizeof(native_t));

   /* load the native database */
   if(load_native(path, trim_alpha, load_native_general, &map_db, &load_res_native[1]) == CHECK_FAILED) {
      exit_func_safe("failed to load resource map database at %s; invalid path", path);
      return CHECK_FAILED;
   }

   /* check the native database */
   if(map_db.db == NULL || map_db.size <= 0) {
      exit_func_safe("failed to load resource map database at %s; detected zero entries", path);
      return CHECK_FAILED;
   }

   /* load the native database into the sqlite3 resources database */
   sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
   for(i = 0, map_res_db = map_db.db; i < map_db.size; i++) {
      sqlite3_clear_bindings(db->map_sql_insert);
      sqlite3_bind_int(db->map_sql_insert,   1, map_res_db[i].id);
      sqlite3_bind_text(db->map_sql_insert,  2, map_res_db[i].map,   strlen(map_res_db[i].map),    SQLITE_STATIC);
      sqlite3_bind_text(db->map_sql_insert,  3, map_res_db[i].name,  strlen(map_res_db[i].name),   SQLITE_STATIC);
      sqlite3_step(db->map_sql_insert);
      sqlite3_reset(db->map_sql_insert);
   }
   sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
   free(map_db.db);
   return CHECK_PASSED;
}

int resource_bonus_sql_load(db_res_t * db, const char * path) {
   int i = 0;
   char buf[BUF_SIZE];
   native_t bonus_db;
   bonus_res * bonus_res_db = NULL;
   memset(&bonus_db, 0, sizeof(native_t));

   /* load the native database */
   if(load_native(path, trim_numeric, load_native_general, &bonus_db, &load_res_native[2]) == CHECK_FAILED) {
      exit_func_safe("failed to load resource bonus database at %s; invalid path", path);
      return CHECK_FAILED;
   }

   /* check the native database */
   if(bonus_db.db == NULL || bonus_db.size <= 0) {
      exit_func_safe("failed to load resource bonus database at %s; detected zero entries", path);
      return CHECK_FAILED;
   }

   /* load the native database into the sqlite3 resources database */
   sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
   for(i = 0, bonus_res_db = bonus_db.db; i < bonus_db.size; i++) {
      sqlite3_clear_bindings(db->bonus_sql_insert);
      sqlite3_bind_int(db->bonus_sql_insert,    1, bonus_res_db[i].id);
      sqlite3_bind_int(db->bonus_sql_insert,    2, bonus_res_db[i].flag);
      sqlite3_bind_int(db->bonus_sql_insert,    3, bonus_res_db[i].attr);
      sqlite3_bind_text(db->bonus_sql_insert,   4, bonus_res_db[i].prefix, strlen(bonus_res_db[i].prefix), SQLITE_STATIC);
      sqlite3_bind_text(db->bonus_sql_insert,   5, bonus_res_db[i].bonus, strlen(bonus_res_db[i].bonus), SQLITE_STATIC);     
      sqlite3_bind_text(db->bonus_sql_insert,   6, bonus_res_db[i].format, strlen(bonus_res_db[i].format), SQLITE_STATIC);
      sqlite3_bind_int(db->bonus_sql_insert,    7, bonus_res_db[i].type_cnt);
      array_to_string_cnt(buf, bonus_res_db[i].type, bonus_res_db[i].type_cnt);
      sqlite3_bind_text(db->bonus_sql_insert,   8, buf,        strlen(buf), SQLITE_TRANSIENT);
      sqlite3_bind_int(db->bonus_sql_insert,    9, bonus_res_db[i].order_cnt);
      array_to_string_cnt(buf, bonus_res_db[i].order, bonus_res_db[i].order_cnt);
      sqlite3_bind_text(db->bonus_sql_insert,   10, buf,       strlen(buf), SQLITE_TRANSIENT);   
      sqlite3_step(db->bonus_sql_insert);
      sqlite3_reset(db->bonus_sql_insert);
   }
   sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);

   free(bonus_db.db);
   return CHECK_PASSED;
}

int resource_status_sql_load(db_res_t * db, const char * path) {
   int i = 0;
   char buf[BUF_SIZE];
   native_t status_db;
   status_res * status_res_db = NULL;
   memset(&status_db, 0, sizeof(native_t));

   /* load the native database */
   if(load_native(path, trim_numeric, load_native_general, &status_db, &load_res_native[3]) == CHECK_FAILED) {
      exit_func_safe("failed to load resource status database at %s; invalid path", path);
      return CHECK_FAILED;
   }

   /* check the native database */
   if(status_db.db == NULL || status_db.size <= 0) {
      exit_func_safe("failed to load resource status database at %s; detected zero entries", path);
      return CHECK_FAILED;
   }

   /* load the native database into the sqlite3 resources database */
   sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
   for(i = 0, status_res_db = status_db.db; i < status_db.size; i++) {
      sqlite3_clear_bindings(db->status_sql_insert);
      sqlite3_bind_int(db->status_sql_insert,      1, status_res_db[i].scid);
      sqlite3_bind_text(db->status_sql_insert,     2, status_res_db[i].scstr, strlen(status_res_db[i].scstr), SQLITE_STATIC);
      sqlite3_bind_int(db->status_sql_insert,      3, status_res_db[i].type);
      if(status_res_db[i].scfmt != NULL)
         sqlite3_bind_text(db->status_sql_insert,  4, status_res_db[i].scfmt, strlen(status_res_db[i].scfmt), SQLITE_STATIC);
      if(status_res_db[i].scend != NULL)
         sqlite3_bind_text(db->status_sql_insert,  5, status_res_db[i].scend, strlen(status_res_db[i].scend), SQLITE_STATIC);
      sqlite3_bind_int(db->status_sql_insert,      6, status_res_db[i].vcnt);
      array_to_string_cnt(buf, status_res_db[i].vmod, 4);
      sqlite3_bind_text(db->status_sql_insert,     7, buf, strlen(buf), SQLITE_TRANSIENT);
      array_to_string_cnt(buf, status_res_db[i].voff, 4);
      sqlite3_bind_text(db->status_sql_insert,     8, buf, strlen(buf), SQLITE_TRANSIENT);
      sqlite3_step(db->status_sql_insert);
      sqlite3_reset(db->status_sql_insert);
   }
   sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
   free(status_db.db);
   return CHECK_PASSED;
}

int resource_var_sql_load(db_res_t * db, const char * path) {
   int i = 0;
   native_t var_db;
   var_res * var_res_db = NULL;
   memset(&var_db, 0, sizeof(native_t));

   /* load the native database */
   if(load_native(path, trim_numeric, load_native_general, &var_db, &load_res_native[4]) == CHECK_FAILED) {
      exit_func_safe("failed to load resource variable and function database at %s; invalid path", path);
      return CHECK_FAILED;
   }

   /* check the native database */
   if(var_db.db == NULL || var_db.size <= 0) {
      exit_func_safe("failed to load resource variable and function database at %s; detected zero entries", path);
      return CHECK_FAILED;
   }

   /* load the native database into the sqlite3 resources database */
   sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
   for(i = 0, var_res_db = var_db.db; i < var_db.size; i++) {
      sqlite3_clear_bindings(db->var_sql_insert);
      sqlite3_bind_int(db->var_sql_insert,   1, var_res_db[i].tag);
      sqlite3_bind_text(db->var_sql_insert,  2, var_res_db[i].id,    strlen(var_res_db[i].id), SQLITE_STATIC);
      sqlite3_bind_int(db->var_sql_insert,   3, var_res_db[i].type);
      sqlite3_bind_int(db->var_sql_insert,   4, var_res_db[i].vflag);
      sqlite3_bind_int(db->var_sql_insert,   5, var_res_db[i].fflag);
      sqlite3_bind_int(db->var_sql_insert,   6, var_res_db[i].min);
      sqlite3_bind_int(db->var_sql_insert,   7, var_res_db[i].max);
      sqlite3_bind_text(db->var_sql_insert,  8, var_res_db[i].str,   strlen(var_res_db[i].str), SQLITE_STATIC);
      sqlite3_step(db->var_sql_insert);
      sqlite3_reset(db->var_sql_insert);
   }
   sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
   free(var_db.db);
   return CHECK_PASSED;
}

int resource_block_sql_load(db_res_t * db, const char * path) {
   int i = 0;
   native_t block_db;
   block_res * block_res_db = NULL;
   memset(&block_db, 0, sizeof(native_t));

   /* load the native database */
   if(load_native(path, trim_numeric, load_native_general, &block_db, &load_res_native[5]) == CHECK_FAILED) {
      exit_func_safe("failed to load resource block database at %s; invalid path", path);
      return CHECK_FAILED;
   }

   /* check the native database */
   if(block_db.db == NULL || block_db.size <= 0) {
      exit_func_safe("failed to load resource block database at %s; detected zero entries", path);
      return CHECK_FAILED;
   }

   /* load the native database into the sqlite3 resources database */
   sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
   for(i = 0, block_res_db = block_db.db; i < block_db.size; i++) {
      sqlite3_clear_bindings(db->block_sql_insert);
      sqlite3_bind_int(db->block_sql_insert,    1, block_res_db[i].id);
      sqlite3_bind_text(db->block_sql_insert,   2, block_res_db[i].name, strlen(block_res_db[i].name), SQLITE_STATIC);
      sqlite3_bind_int(db->block_sql_insert,    3, block_res_db[i].flag);
      sqlite3_step(db->block_sql_insert);
      sqlite3_reset(db->block_sql_insert);
   }
   sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
   free(block_db.db);
   return CHECK_PASSED;
}

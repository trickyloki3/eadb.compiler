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
         } else if(record->order == NULL) {   /* build the order  */
            record->order_cnt = convert_integer(val, 10); 
         } else {                             /* read the orders */
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

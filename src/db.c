/*
 *   file: db.c
 *   date: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "db.h"

load_cb_t * ea_item_load() {
	load_cb_t * interface = calloc(1, sizeof(load_cb_t));
	exit_null("failed to load interface", 1, interface);
	interface->load_column = ea_load_column;
	interface->is_row_sentinel = is_row_sentinel;
	interface->is_row_delimiter = is_row_delimiter;
	interface->flag = CHECK_BRACKET;
	interface->column_count = 22;
   interface->type_size = sizeof(ea_item_t);
   interface->dealloc = ea_item_dealloc;
	return interface;
}

load_cb_t * ra_item_load() {
	load_cb_t * interface = calloc(1, sizeof(load_cb_t));
	exit_null("failed to load interface", 1, interface);
	interface->load_column = ra_load_column;
	interface->is_row_sentinel = is_row_sentinel;
	interface->is_row_delimiter = is_row_delimiter;
	interface->flag = CHECK_BRACKET;
	interface->column_count = 22;
   interface->type_size = sizeof(ra_item_t);
   interface->dealloc = ra_item_dealloc;
	return interface;
}

load_cb_t * pet_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = pet_load_column;
   interface->is_row_sentinel = is_row_sentinel;
   interface->is_row_delimiter = is_row_delimiter;
   interface->flag = CHECK_BRACKET;
   interface->column_count = 22;
   interface->type_size = sizeof(pet_t);
   interface->dealloc = pet_dealloc;
   return interface;
}

load_cb_t * merc_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load mercenary interface", 1, interface);
   interface->load_column = merc_load_column;
   interface->is_row_sentinel = is_row_sentinel;
   interface->is_row_delimiter = is_row_delimiter;
   interface->flag = 0;
   interface->column_count = 26;
   interface->type_size = sizeof(merc_t);
   interface->dealloc = merc_dealloc;
   return interface;
}

load_cb_t * produce_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = produce_load_column;
   interface->is_row_sentinel = is_row_sentinel;
   interface->is_row_delimiter = is_row_delimiter;
   interface->flag = 0;
   interface->column_count = 0;
   interface->type_size = sizeof(produce_t);
   interface->dealloc = blank_dealloc;
   return interface;
}

load_cb_t * ra_produce_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = ra_produce_load_column;
   interface->is_row_sentinel = is_row_sentinel;
   interface->is_row_delimiter = is_row_delimiter;
   interface->flag = 0;
   interface->column_count = 0;
   interface->type_size = sizeof(ra_produce_t);
   interface->dealloc = blank_dealloc;
   return interface;
}

load_cb_t * skill_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = skill_load_column;
   interface->is_row_sentinel = is_row_sentinel_comment;
   interface->is_row_delimiter = is_row_delimiter_comment;
   interface->flag = 0;
   interface->column_count = 17;
   interface->type_size = sizeof(skill_t);
   interface->dealloc = skill_dealloc;
   return interface;
}

load_cb_t * ra_skill_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = ra_skill_load_column;
   interface->is_row_sentinel = is_row_sentinel_comment;
   interface->is_row_delimiter = is_row_delimiter_comment;
   interface->flag = 0;
   interface->column_count = 18;
   interface->type_size = sizeof(ra_skill_t);
   interface->dealloc = ra_skill_dealloc;
   return interface;
}

load_cb_t * mob_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = mob_load_column;
   interface->is_row_sentinel = is_row_sentinel;
   interface->is_row_delimiter = is_row_delimiter;
   interface->flag = 0;
   interface->column_count = 57;
   interface->type_size = sizeof(mob_t);
   interface->dealloc = mob_dealloc;
   return interface;
}

load_cb_t * ea_mob_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = ea_mob_load_column;
   interface->is_row_sentinel = is_row_sentinel;
   interface->is_row_delimiter = is_row_delimiter;
   interface->flag = 0;
   interface->column_count = 58;
   interface->type_size = sizeof(ea_mob_t);
   interface->dealloc = ea_mob_dealloc;
   return interface;
}

load_cb_t * block_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = block_load_column;
   interface->is_row_sentinel = is_row_sentinel_semicolon;
   interface->is_row_delimiter = is_row_delimiter_semicolon;
   interface->flag = 0;
   interface->column_count = 3;
   interface->type_size = sizeof(block_t);
   interface->dealloc = block_dealloc;
   return interface;
}

load_cb_t * var_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = var_load_column;
   interface->is_row_sentinel = is_row_sentinel_semicolon;
   interface->is_row_delimiter = is_row_delimiter_semicolon;
   interface->flag = 0;
   interface->column_count = 7;
   interface->type_size = sizeof(var_t);
   interface->dealloc = var_dealloc;
   return interface;
}

load_cb_t * status_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = status_load_column;
   interface->is_row_sentinel = is_row_sentinel_semicolon;
   interface->is_row_delimiter = is_row_delimiter_semicolon;
   interface->flag = CHECK_QUOTE;
   interface->column_count = 0;
   interface->type_size = sizeof(status_t);
   interface->dealloc = status_dealloc;
   return interface;
}

load_cb_t * bonus_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = bonus_load_column;
   interface->is_row_sentinel = is_row_sentinel_semicolon;
   interface->is_row_delimiter = is_row_delimiter_semicolon;
   interface->flag = CHECK_QUOTE;
   interface->column_count = 0;
   interface->type_size = sizeof(bonus_t);
   interface->dealloc = bonus_dealloc;
   return interface;
}

load_cb_t * const_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = const_load_column;
   interface->is_row_sentinel = is_row_sentinel;
   interface->is_row_delimiter = is_row_delimiter_whitespace;
   interface->flag = SKIP_NEXT_WS;
   interface->column_count = 0;
   interface->type_size = sizeof(const_t);
   interface->dealloc = const_dealloc;
   return interface;
}

load_cb_t * ea_item_group_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = ea_item_group_load_column;
   interface->is_row_sentinel = is_row_sentinel_whitespace;
   interface->is_row_delimiter = is_row_delimiter_semicolon;
   interface->flag = SKIP_NEXT_WS;
   interface->column_count = 0;
   interface->type_size = sizeof(ea_item_group_t);
   interface->dealloc = blank_dealloc;
   return interface;  
}

load_cb_t * ra_item_group_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = ra_item_group_load_column;
   interface->is_row_sentinel = is_row_sentinel_whitespace;
   interface->is_row_delimiter = is_row_delimiter_semicolon;
   interface->flag = SKIP_NEXT_WS;
   interface->column_count = 0;
   interface->type_size = sizeof(ra_item_group_t);
   interface->dealloc = ra_item_group_dealloc;
   return interface;  
}

load_cb_t * ra_item_package_load() {
   load_cb_t * interface = calloc(1, sizeof(load_cb_t));
   exit_null("failed to load interface", 1, interface);
   interface->load_column = ra_item_package_column;
   interface->is_row_sentinel = is_row_sentinel_whitespace;
   interface->is_row_delimiter = is_row_delimiter_semicolon;
   interface->flag = SKIP_NEXT_WS;
   interface->column_count = 0;
   interface->type_size = sizeof(ra_item_package_t);
   interface->dealloc = ra_item_package_dealloc;
   return interface;  
}

int ea_load_column(void * db, int row, int col, char * val) {
   ea_item_t * item_row = &((ea_item_t *) db)[row];
   switch(col) {
      case 0:   item_row->id = convert_integer(val, 10); break;
      case 1:   item_row->aegis = convert_string(val); break;
      case 2:   item_row->eathena = convert_string(val); break;
      case 3:   item_row->type = convert_integer(val, 10); break;
      case 4:   item_row->buy = convert_integer(val, 10); break;
      case 5:   item_row->sell = convert_integer(val, 10); break;
      case 6:   item_row->weight = convert_integer(val, 10); break;
      case 7:   item_row->atk = convert_integer(val, 10); break;
      case 8:   item_row->def = convert_integer(val, 10); break;
      case 9:   item_row->range = convert_integer(val, 10); break;
      case 10:  item_row->slots = convert_integer(val, 10); break;
      case 11:  item_row->job = convert_uinteger(val, 16); break;
      case 12:  item_row->upper = convert_integer(val, 10); break;
      case 13:  item_row->gender = convert_integer(val, 10); break;
      case 14:  item_row->loc = convert_integer(val, 10); break;
      case 15:  item_row->wlv = convert_integer(val, 10); break;
      case 16:  item_row->elv = convert_integer(val, 10); break;
      case 17:  item_row->refineable = convert_integer(val, 10); break;
      case 18:  item_row->view = convert_integer(val, 10); break;
      case 19:  item_row->script = convert_string(val); break;
      case 20:  item_row->onequip = convert_string(val); break;
      case 21:  item_row->onunequip = convert_string(val); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int ra_load_column(void * db, int row, int col, char * val) {
	ra_item_t * item_row = &((ra_item_t *) db)[row];
	switch(col) {
      case 0:   item_row->id = convert_integer(val, 10); break;
      case 1:   item_row->aegis = convert_string(val); break;
      case 2:   item_row->eathena = convert_string(val); break;
      case 3:   item_row->type = convert_integer(val, 10); break;
      case 4:   item_row->buy = convert_integer(val, 10); break;
      case 5:   item_row->sell = convert_integer(val, 10); break;
      case 6:   item_row->weight = convert_integer(val, 10); break;
      case 7:   convert_delimit_integer(val, ':', 2, 
      &item_row->atk, &item_row->matk); break;
      case 8:   item_row->def = convert_integer(val, 10); break;
      case 9:   item_row->range = convert_integer(val, 10); break;
      case 10:  item_row->slots = convert_integer(val, 10); break;
      case 11:  item_row->job = convert_uinteger(val, 16); break;
      case 12:  item_row->upper = convert_integer(val, 10); break;
      case 13:  item_row->gender = convert_integer(val, 10); break;
      case 14:  item_row->loc = convert_integer(val, 10); break;
      case 15:  item_row->wlv = convert_integer(val, 10); break;
      case 16:  item_row->elv = convert_integer(val, 10); break;
      case 17:  item_row->refineable = convert_integer(val, 10); break;
      case 18:  item_row->view = convert_integer(val, 10); break;
      case 19:  item_row->script = convert_string(val); break;
      case 20:  item_row->onequip = convert_string(val); break;
      case 21:  item_row->onunequip = convert_string(val); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int pet_load_column(void * db, int row, int col, char * val) {
   pet_t * pet_row = &((pet_t *) db)[row];
   switch(col) {
      case 0: pet_row->mob_id = convert_integer(val,10); break;
      case 1: pet_row->pet_name = convert_string(val); break;
      case 2: pet_row->pet_jname = convert_string(val); break;
      case 3: pet_row->lure_id = convert_integer(val,10); break;
      case 4: pet_row->egg_id = convert_integer(val,10); break;
      case 5: pet_row->equip_id = convert_integer(val,10); break;
      case 6: pet_row->food_id = convert_integer(val,10); break;
      case 7: pet_row->fullness = convert_integer(val,10); break;
      case 8: pet_row->hungry_delay = convert_integer(val,10); break;
      case 9: pet_row->r_hungry = convert_integer(val,10); break;
      case 10: pet_row->r_full = convert_integer(val,10); break;
      case 11: pet_row->intimate = convert_integer(val,10); break;
      case 12: pet_row->die = convert_integer(val,10); break;
      case 13: pet_row->capture = convert_integer(val,10); break;
      case 14: pet_row->speed = convert_integer(val,10); break;
      case 15: pet_row->s_performance = convert_integer(val,10); break;
      case 16: pet_row->talk_convert = convert_integer(val,10); break;
      case 17: pet_row->attack_rate = convert_integer(val,10); break;
      case 18: pet_row->defence_attack_rate = convert_integer(val,10); break;
      case 19: pet_row->change_target_rate = convert_integer(val,10); break;
      case 20: pet_row->pet_script = convert_string(val); break;
      case 21: pet_row->loyal_script = convert_string(val); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int merc_load_column(void * db, int row, int col, char * val) {
   merc_t * merc_row = &((merc_t *) db)[row];
   switch(col) {
      case 0: merc_row->id = convert_integer(val, 10); break;
      case 1: merc_row->sprite = convert_string(val); break;
      case 2: merc_row->name = convert_string(val); break;
      case 3: merc_row->lv = convert_integer(val, 10); break;
      case 4: merc_row->hp = convert_integer(val, 10); break;
      case 5: merc_row->sp = convert_integer(val, 10); break;
      case 6: merc_row->range1 = convert_integer(val, 10); break;
      case 7: merc_row->atk1 = convert_integer(val, 10); break;
      case 8: merc_row->atk2 = convert_integer(val, 10); break;
      case 9: merc_row->def = convert_integer(val, 10); break;
      case 10: merc_row->mdef = convert_integer(val, 10); break;
      case 11: merc_row->str = convert_integer(val, 10); break;
      case 12: merc_row->agi = convert_integer(val, 10); break;
      case 13: merc_row->vit = convert_integer(val, 10); break;
      case 14: merc_row->intr = convert_integer(val, 10); break;
      case 15: merc_row->dex = convert_integer(val, 10); break;
      case 16: merc_row->luk = convert_integer(val, 10); break;
      case 17: merc_row->range2 = convert_integer(val, 10); break;
      case 18: merc_row->range3 = convert_integer(val, 10); break;
      case 19: merc_row->scale = convert_integer(val, 10); break;
      case 20: merc_row->race = convert_integer(val, 10); break;
      case 21: merc_row->element = convert_integer(val, 10); break;
      case 22: merc_row->speed = convert_integer(val, 10); break;
      case 23: merc_row->adelay = convert_integer(val, 10); break;
      case 24: merc_row->amotion = convert_integer(val, 10); break;
      case 25: merc_row->dmotion = convert_integer(val, 10); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int produce_load_column(void * db, int row, int col, char * val) {
   static int material_cnt = 0;
   static int alternate = 0;
   produce_t * produce_row = &((produce_t *) db)[row];
   switch(col) {
      case 0: 
         material_cnt = 0;
         alternate = 0;
         produce_row->item_id = convert_integer(val,10);
         break;
      case 1: produce_row->item_lv = convert_integer(val,10); break;
      case 2: produce_row->req_skill = convert_integer(val,10); break;
      case 3: produce_row->req_skill_lv = convert_integer(val,10); break;
      default: 
         (!alternate) ?
            (produce_row->material[material_cnt] = convert_integer(val,10)):
            (produce_row->amount[material_cnt++] = convert_integer(val,10));
         alternate = !alternate;
         break;
   }
   return 0;
}

int ra_produce_load_column(void * db, int row, int col, char * val) {
   static int material_cnt = 0;
   static int alternate = 0;
   ra_produce_t * produce_row = &((ra_produce_t *) db)[row];
   switch(col) {
      case 0:
         material_cnt = 0;
         alternate = 0;
         produce_row->id = convert_integer(val,10);
         break;
      case 1: produce_row->item_id = convert_integer(val,10); break;
      case 2: produce_row->item_lv = convert_integer(val,10); break;
      case 3: produce_row->req_skill = convert_integer(val,10); break;
      case 4: produce_row->req_skill_lv = convert_integer(val,10); break;
      default: 
         (!alternate) ?
            (produce_row->material[material_cnt] = convert_integer(val,10)):
            (produce_row->amount[material_cnt++] = convert_integer(val,10));
         alternate = !alternate;
         break;
   }
   return 0;
}

int skill_load_column(void * db, int row, int col, char * val) {
   skill_t * skill_row = &((skill_t *) db)[row];
   switch(col) {
      case 0: skill_row->id = convert_integer(val,10); break;
      case 1: skill_row->range = convert_string(val); break;
      case 2: skill_row->hit = convert_integer(val,10); break;
      case 3: skill_row->inf = convert_integer(val,10); break;
      case 4: skill_row->element = convert_string(val); break;
      case 5: skill_row->nk = convert_uinteger(val,16); break;
      case 6: skill_row->splash = convert_string(val); break;
      case 7: skill_row->max = convert_integer(val,10); break;
      case 8: skill_row->hit_amount = convert_string(val); break;
      case 9: skill_row->cast_cancel = convert_string(val); break;
      case 10: skill_row->cast_def_reduce_rate = convert_integer(val,10); break;
      case 11: skill_row->inf2 = convert_uinteger(val,16); break;
      case 12: skill_row->maxcount = convert_string(val); break;
      case 13: skill_row->type = convert_string(val); break;
      case 14: skill_row->blow_count = convert_string(val); break;
      case 15: skill_row->name = convert_string(val); break;
      case 16: skill_row->desc = convert_string(val); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int ra_skill_load_column(void * db, int row, int col, char * val) {
   ra_skill_t * skill_row = &((ra_skill_t *) db)[row];
   switch(col) {
      case 0: skill_row->id = convert_integer(val,10); break;
      case 1: skill_row->range = convert_string(val); break;
      case 2: skill_row->hit = convert_integer(val,10); break;
      case 3: skill_row->inf = convert_integer(val,10); break;
      case 4: skill_row->element = convert_string(val); break;
      case 5: skill_row->nk = convert_uinteger(val,16); break;
      case 6: skill_row->splash = convert_string(val); break;
      case 7: skill_row->max = convert_integer(val,10); break;
      case 8: skill_row->hit_amount = convert_string(val); break;
      case 9: skill_row->cast_cancel = convert_string(val); break;
      case 10: skill_row->cast_def_reduce_rate = convert_integer(val,10); break;
      case 11: skill_row->inf2 = convert_uinteger(val,16); break;
      case 12: skill_row->maxcount = convert_string(val); break;
      case 13: skill_row->type = convert_string(val); break;
      case 14: skill_row->blow_count = convert_string(val); break;
      case 15: skill_row->inf3 = convert_uinteger(val,16); break;
      case 16: skill_row->name = convert_string(val); break;
      case 17: skill_row->desc = convert_string(val); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int mob_load_column(void * db, int row, int col, char * val) {
   mob_t * mob_row = &((mob_t *) db)[row];
   switch(col) {
      case 0: mob_row->id = convert_integer(val,10);            break;
      case 1: mob_row->sprite = convert_string(val);            break;
      case 2: mob_row->kro = convert_string(val);               break;
      case 3: mob_row->iro = convert_string(val);               break;
      case 4: mob_row->lv = convert_integer(val,10);            break;
      case 5: mob_row->hp = convert_integer(val,10);            break;
      case 6: mob_row->sp = convert_integer(val,10);            break;
      case 7: mob_row->exp = convert_integer(val,10);           break;
      case 8: mob_row->jexp = convert_integer(val,10);          break;
      case 9: mob_row->range = convert_integer(val,10);         break;
      case 10: mob_row->atk1 = convert_integer(val,10);         break;
      case 11: mob_row->atk2 = convert_integer(val,10);         break;
      case 12: mob_row->def = convert_integer(val,10);          break;
      case 13: mob_row->mdef = convert_integer(val,10);         break;
      case 14: mob_row->str = convert_integer(val,10);          break;
      case 15: mob_row->agi = convert_integer(val,10);          break;
      case 16: mob_row->vit = convert_integer(val,10);          break;
      case 17: mob_row->intr = convert_integer(val,10);         break;
      case 18: mob_row->dex = convert_integer(val,10);          break;
      case 19: mob_row->luk = convert_integer(val,10);          break;
      case 20: mob_row->range2 = convert_integer(val,10);       break;
      case 21: mob_row->range3 = convert_integer(val,10);       break;
      case 22: mob_row->scale = convert_integer(val,10);        break;
      case 23: mob_row->race = convert_integer(val,10);         break;
      case 24: mob_row->element = convert_integer(val,10);      break;
      case 25: mob_row->mode = convert_uinteger(val,16);        break;
      case 26: mob_row->speed = convert_integer(val,10);        break;
      case 27: mob_row->adelay = convert_integer(val,10);       break;
      case 28: mob_row->amotion = convert_integer(val,10);      break;
      case 29: mob_row->dmotion = convert_integer(val,10);      break;
      case 30: mob_row->mexp = convert_integer(val,10);         break;
      case 31: mob_row->mvp1id = convert_integer(val,10);       break;
      case 32: mob_row->mvp1per = convert_integer(val,10);      break;
      case 33: mob_row->mvp2id = convert_integer(val,10);       break;
      case 34: mob_row->mvp2per = convert_integer(val,10);      break;
      case 35: mob_row->mvp3id = convert_integer(val,10);       break;
      case 36: mob_row->mvp3per = convert_integer(val,10);      break;
      case 37: mob_row->drop1id = convert_integer(val,10);      break;
      case 38: mob_row->drop1per = convert_integer(val,10);     break;
      case 39: mob_row->drop2id = convert_integer(val,10);      break;
      case 40: mob_row->drop2per = convert_integer(val,10);     break;
      case 41: mob_row->drop3id = convert_integer(val,10);      break;
      case 42: mob_row->drop3per = convert_integer(val,10);     break;
      case 43: mob_row->drop4id = convert_integer(val,10);      break;
      case 44: mob_row->drop4per = convert_integer(val,10);     break;
      case 45: mob_row->drop5id = convert_integer(val,10);      break;
      case 46: mob_row->drop5per = convert_integer(val,10);     break;
      case 47: mob_row->drop6id = convert_integer(val,10);      break;
      case 48: mob_row->drop6per = convert_integer(val,10);     break;
      case 49: mob_row->drop7id = convert_integer(val,10);      break;
      case 50: mob_row->drop7per = convert_integer(val,10);     break;
      case 51: mob_row->drop8id = convert_integer(val,10);      break;
      case 52: mob_row->drop8per = convert_integer(val,10);     break;
      case 53: mob_row->drop9id = convert_integer(val,10);      break;
      case 54: mob_row->drop9per = convert_integer(val,10);     break;
      case 55: mob_row->dropcardid = convert_integer(val,10);   break;
      case 56: mob_row->dropcardper = convert_integer(val,10);  break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int ea_mob_load_column(void * db, int row, int col, char * val) {
   ea_mob_t * mob_row = &((ea_mob_t *) db)[row];
   switch(col) {
      case 0: mob_row->id = convert_integer(val,10);              break;
      case 1: mob_row->sprite = convert_string(val);              break;
      case 2: mob_row->kro = convert_string(val);                 break;
      case 3: mob_row->iro = convert_string(val);                 break;
      case 4: mob_row->lv = convert_integer(val,10);              break;
      case 5: mob_row->hp = convert_integer(val,10);              break;
      case 6: mob_row->sp = convert_integer(val,10);              break;
      case 7: mob_row->exp = convert_integer(val,10);             break;
      case 8: mob_row->jexp = convert_integer(val,10);            break;
      case 9: mob_row->range = convert_integer(val,10);           break;
      case 10: mob_row->atk1 = convert_integer(val,10);           break;
      case 11: mob_row->atk2 = convert_integer(val,10);           break;
      case 12: mob_row->def = convert_integer(val,10);            break;
      case 13: mob_row->mdef = convert_integer(val,10);           break;
      case 14: mob_row->str = convert_integer(val,10);            break;
      case 15: mob_row->agi = convert_integer(val,10);            break;
      case 16: mob_row->vit = convert_integer(val,10);            break;
      case 17: mob_row->intr = convert_integer(val,10);           break;
      case 18: mob_row->dex = convert_integer(val,10);            break;
      case 19: mob_row->luk = convert_integer(val,10);            break;
      case 20: mob_row->range2 = convert_integer(val,10);         break;
      case 21: mob_row->range3 = convert_integer(val,10);         break;
      case 22: mob_row->scale = convert_integer(val,10);          break;
      case 23: mob_row->race = convert_integer(val,10);           break;
      case 24: mob_row->element = convert_integer(val,10);        break;
      case 25: mob_row->mode = convert_uinteger(val,16);          break;
      case 26: mob_row->speed = convert_integer(val,10);          break;
      case 27: mob_row->adelay = convert_integer(val,10);         break;
      case 28: mob_row->amotion = convert_integer(val,10);        break;
      case 29: mob_row->dmotion = convert_integer(val,10);        break;
      case 30: mob_row->mexp = convert_integer(val,10);           break;
      case 31: mob_row->expper = convert_integer(val,10);         break;
      case 32: mob_row->mvp1id = convert_integer(val,10);         break;
      case 33: mob_row->mvp1per = convert_integer(val,10);        break;
      case 34: mob_row->mvp2id = convert_integer(val,10);         break;
      case 35: mob_row->mvp2per = convert_integer(val,10);        break;
      case 36: mob_row->mvp3id = convert_integer(val,10);         break;
      case 37: mob_row->mvp3per = convert_integer(val,10);        break;
      case 38: mob_row->drop1id = convert_integer(val,10);        break;
      case 39: mob_row->drop1per = convert_integer(val,10);       break;
      case 40: mob_row->drop2id = convert_integer(val,10);        break;
      case 41: mob_row->drop2per = convert_integer(val,10);       break;
      case 42: mob_row->drop3id = convert_integer(val,10);        break;
      case 43: mob_row->drop3per = convert_integer(val,10);       break;
      case 44: mob_row->drop4id = convert_integer(val,10);        break;
      case 45: mob_row->drop4per = convert_integer(val,10);       break;
      case 46: mob_row->drop5id = convert_integer(val,10);        break;
      case 47: mob_row->drop5per = convert_integer(val,10);       break;
      case 48: mob_row->drop6id = convert_integer(val,10);        break;
      case 49: mob_row->drop6per = convert_integer(val,10);       break;
      case 50: mob_row->drop7id = convert_integer(val,10);        break;
      case 51: mob_row->drop7per = convert_integer(val,10);       break;
      case 52: mob_row->drop8id = convert_integer(val,10);        break;
      case 53: mob_row->drop8per = convert_integer(val,10);       break;
      case 54: mob_row->drop9id = convert_integer(val,10);        break;
      case 55: mob_row->drop9per = convert_integer(val,10);       break;
      case 56: mob_row->dropcardid = convert_integer(val,10);     break;
      case 57: mob_row->dropcardper = convert_integer(val,10);    break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int block_load_column(void * db, int row, int col, char * val) {
   block_t * block_row = &((block_t *) db)[row];
   switch(col) {
      case 0: block_row->bk_id = convert_integer(val, 10); break;
      case 1: block_row->bk_kywd = convert_string(val); break;
      case 2: block_row->bk_flag = convert_integer(val, 10); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int var_load_column(void * db, int row, int col, char * val) {
   var_t * var_row = &((var_t *) db)[row];
   switch(col) {
      case 0: var_row->tag = convert_integer(val, 10); break;
      case 1: var_row->id = convert_string(val); break;
      case 2: var_row->type = convert_integer(val, 16); break;
      case 3: var_row->vflag = convert_integer(val, 16); break;
      case 4: var_row->fflag = convert_integer(val, 16); break;
      case 5: var_row->min = convert_integer(val, 10); break;
      case 6: var_row->max = convert_integer(val, 10); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}

int status_load_column(void * db, int row, int col, char * val) {
   static int split_cnt = 0;
   status_t * status_row = &((status_t *) db)[row];
   switch(col) {
      case 0: 
         split_cnt = 0;
         status_row->scid = convert_integer(val,10); 
         break;
      case 1: status_row->scstr = convert_string(val); break;
      case 2: status_row->type = convert_integer(val,10); break;
      case 3: status_row->scfmt = convert_string(val); break;
      case 4: status_row->scend = convert_string(val); break;
      case 5: status_row->vcnt = convert_integer(val,10); break;
      default: 
         if(split_cnt < 4) {
            status_row->vmod[split_cnt] = (int) val[0];
            split_cnt++;
         } else {
            status_row->voff[split_cnt-4] = convert_integer(val,10);
            split_cnt++;
         }
   }
   return 0;
}

int bonus_load_column(void * db, int row, int col, char * val) {
   static int type_cnt = 0;
   static int order_cnt = 0;
   static int var_state = 0;
   bonus_t * bonus_row = &((bonus_t *) db)[row];
   switch(col) {
      case 0: 
         type_cnt = 0;
         order_cnt = 0;
         var_state = 0;
         bonus_row->pref = convert_string(val); break;
      case 1: bonus_row->buff = convert_string(val); break;
      case 2: bonus_row->attr = convert_integer(val, 10); break;
      case 3: bonus_row->desc = convert_string(val); break;
      case 4: bonus_row->type_cnt = convert_integer(val, 10); 
              bonus_row->type = calloc(5, sizeof(int32_t));
              break;
      default: 
         if(var_state < bonus_row->type_cnt) {   /* read the types */
            bonus_row->type[type_cnt] = val[0];
            var_state++;
            type_cnt++;
         } else if(bonus_row->order == NULL) {   /* build the order  */
            bonus_row->order_cnt = convert_integer(val, 10); 
            bonus_row->order = calloc(5, sizeof(int32_t));
         } else {                                                 /* read the orders */
            bonus_row->order[order_cnt] = convert_integer(val, 10); 
            order_cnt++;
         }
         break;
   }
   return 0;
}

int const_load_column(void * db, int row, int col, char * val) {
   const_t * const_row = &((const_t *) db)[row];
   switch(col) {
      case 0: const_row->name = convert_string(val); break;
      case 1:  
         /* constant can be represented as hexadecimal or decimal */
         if(strlen(val) > 2 && val[0] == '0' && val[1] == 'x')
            const_row->value = convert_integer(val, 16);
         else
            const_row->value = convert_integer(val, 10);
         break;
      case 2: const_row->type = convert_integer(val, 10); break;
      default:  exit_abt("invalid column");
   }
   return 0;
}


int ea_item_group_load_column(void * db, int row, int col, char * val) {
   ea_item_group_t * ea_item_group = &((ea_item_group_t *) db)[row];
   switch(col) {
      case 0: ea_item_group->group_id = convert_integer(val, 10); break;
      case 1: ea_item_group->item_id = convert_integer(val, 10); break;
      case 2: ea_item_group->rate = convert_integer(val, 10); break;
      default: exit_abt("invalid column");
   }
   return 0;  
}

int ra_item_group_load_column(void * db, int row, int col, char * val) {
   ra_item_group_t * ra_item_group = &((ra_item_group_t *) db)[row];
   switch(col) {
      case 0: ra_item_group->group_name = convert_string(val); break;
      case 1: ra_item_group->item_id = convert_integer(val, 10); break;
      case 2: ra_item_group->rate = convert_integer(val, 10); break;
      default: printf("%d;%s\n", col, val); exit_abt("invalid column");
   }
   return 0;
}

int ra_item_package_column(void * db, int row, int col, char * val) {
   ra_item_package_t * ra_item_package = &((ra_item_package_t *) db)[row];
   switch(col) {
      case 0: ra_item_package->group_name = convert_string(val); break;
      case 1: ra_item_package->item_name = convert_string(val); break;
      case 2: ra_item_package->rate = convert_integer(val, 10); break;
      default: break;
   }
   return 0;
}

int is_row_sentinel(char buf) { return (buf == '\n' || buf == '\0'); }
int is_row_delimiter(char buf) { return (buf == ',' || buf == '\n' || buf == '\0'); }
int is_row_sentinel_whitespace(char buf) { return (buf == '\n' || buf == '\0' || isspace(buf)); }
int is_row_sentinel_comment(char buf) { return (buf == '\n' || buf == '\0' || buf == '/'); }
int is_row_delimiter_comment(char buf) { return (buf == ',' || buf == '\n' || buf == '\0' || buf == '/'); }
int is_row_sentinel_semicolon(char buf) { return (buf == '\n' || buf == '\0' || buf == ';'); }
int is_row_delimiter_semicolon(char buf) { return (buf == ',' || buf == '\n' || buf == '\0' || buf == ';'); }
int is_row_delimiter_whitespace(char buf) { return (buf == ',' || buf == '\n' || buf == '\0' || isspace(buf)); }

void const_dealloc(void * mem, int cnt) {
   int i = 0;
   const_t * db = (const_t *) mem;
   for(i = 0; i < cnt; i++)
      if(db[i].name != NULL) free(db[i].name);
   free(db);
}

void ea_item_dealloc(void * mem, int cnt) {
   int i = 0;
   ea_item_t * db = (ea_item_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].aegis != NULL) free(db[i].aegis);
      if(db[i].eathena != NULL) free(db[i].eathena);
      if(db[i].script != NULL) free(db[i].script);
      if(db[i].onequip != NULL) free(db[i].onequip);
      if(db[i].onunequip != NULL) free(db[i].onunequip);
   }
   free(db);
}

void ra_item_dealloc(void * mem, int cnt) {
   int i = 0;
   ra_item_t * db = (ra_item_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].aegis != NULL) free(db[i].aegis);
      if(db[i].eathena != NULL) free(db[i].eathena);
      if(db[i].script != NULL) free(db[i].script);
      if(db[i].onequip != NULL) free(db[i].onequip);
      if(db[i].onunequip != NULL) free(db[i].onunequip);
   }
   free(db);
}

void pet_dealloc(void * mem, int cnt) {
   int i = 0;
   pet_t * db = (pet_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].pet_name != NULL) free(db[i].pet_name);
      if(db[i].pet_jname != NULL) free(db[i].pet_jname);
      if(db[i].pet_script != NULL) free(db[i].pet_script);
      if(db[i].loyal_script != NULL) free(db[i].loyal_script);
   }
   free(db);
}

void merc_dealloc(void * mem, int cnt) {
   int i = 0;
   merc_t * db = (merc_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].sprite != NULL) free(db[i].sprite);
      if(db[i].name != NULL) free(db[i].name);
   }
   free(db);
}

void blank_dealloc(void * mem, int cnt) {
   free(mem);
}

void skill_dealloc(void * mem, int cnt) {
   int i = 0;
   skill_t * db = (skill_t *) mem;
   for(i = 0; i < cnt; i++) {
      free(db[i].range);
      free(db[i].element);
      free(db[i].splash);
      free(db[i].hit_amount);
      if(db[i].cast_cancel != NULL) free(db[i].cast_cancel);
      free(db[i].maxcount);
      if(db[i].type != NULL) free(db[i].type);
      free(db[i].blow_count);
      if(db[i].name != NULL) free(db[i].name);
      if(db[i].desc != NULL) free(db[i].desc);
   }
   free(db);
}

void ra_skill_dealloc(void * mem, int cnt) {
   int i = 0;
   ra_skill_t * db = (ra_skill_t *) mem;
   for(i = 0; i < cnt; i++) {
      free(db[i].range);
      free(db[i].element);
      free(db[i].splash);
      free(db[i].hit_amount);
      if(db[i].cast_cancel != NULL) free(db[i].cast_cancel);
      free(db[i].maxcount);
      if(db[i].type != NULL) free(db[i].type);
      free(db[i].blow_count);
      if(db[i].name != NULL) free(db[i].name);
      if(db[i].desc != NULL) free(db[i].desc);
   }
   free(db);
}

void ea_mob_dealloc(void * mem, int cnt) {
   int i = 0;
   ea_mob_t * db = (ea_mob_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].sprite != NULL) free(db[i].sprite);
      if(db[i].kro != NULL) free(db[i].kro);
      if(db[i].iro != NULL) free(db[i].iro);
   }
   free(db);
}

void mob_dealloc(void * mem, int cnt) {
   int i = 0;
   mob_t * db = (mob_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].sprite != NULL) free(db[i].sprite);
      if(db[i].kro != NULL) free(db[i].kro);
      if(db[i].iro != NULL) free(db[i].iro);
   }
   free(db);
}

void block_dealloc(void * mem, int cnt) {
   int i = 0;
   block_t * db = (block_t *) mem;
   for(i = 0; i < cnt; i++)
      if(db[i].bk_kywd != NULL) free(db[i].bk_kywd);
   free(db);
}

void var_dealloc(void * mem, int cnt) {
   int i = 0;
   var_t * db = (var_t *) mem;
   for(i = 0; i < cnt; i++)
      if(db[i].id != NULL) free(db[i].id);
   free(db);
}

void status_dealloc(void * mem, int cnt) {
   int i = 0;
   status_t * db = (status_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].scstr != NULL) free(db[i].scstr);
      if(db[i].scfmt != NULL) free(db[i].scfmt);
      if(db[i].scend != NULL) free(db[i].scend);
   }
   free(db);
}

void bonus_dealloc(void * mem, int cnt) {
   int i = 0;
   bonus_t * db = (bonus_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].pref != NULL) free(db[i].pref);
      if(db[i].buff != NULL) free(db[i].buff);
      if(db[i].desc != NULL) free(db[i].desc);
      if(db[i].type != NULL) free(db[i].type);
      if(db[i].order != NULL) free(db[i].order);
   }
   free(db);
}

void ra_item_group_dealloc(void * mem, int cnt) {
   int i = 0;
   ra_item_group_t * db = (ra_item_group_t *) mem;
   for(i = 0; i < cnt; i++)
      if(db[i].group_name != NULL) free(db[i].group_name);
   free(db);
}

void ra_item_package_dealloc(void * mem, int cnt) {
   int i = 0;
   ra_item_package_t * db = (ra_item_package_t *) mem;
   for(i = 0; i < cnt; i++) {
      if(db[i].group_name != NULL) free(db[i].group_name);
      if(db[i].item_name != NULL) free(db[i].item_name);
   }
   free(db);
}
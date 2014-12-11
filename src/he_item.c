/*
 *   file: he_item.c
 *   date: 12/9/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "db.h"
#include "libconfig.h"
void he_item_db_insert(struct lt_db_t *, he_item_t *);
void load_he_item(const char *, struct lt_db_t *);

int main(int argc, char * argv[]) {
	struct lt_db_t * db = init_db("athena.db", INITIALIZE_SKIP);
	load_he_item("../hedb/item_db.conf", db);
	deit_db(db);
	exit(EXIT_SUCCESS);
}

void load_he_item(const char * filename, struct lt_db_t * db) {
	int i = 0;
	int cnt = 0;
	int status = 0;
	/* error reporting */
	int offset = 0;
	char error[4096];
	/* libconfig */
	config_t * cfg = calloc(1, sizeof(config_t));
	config_setting_t * item_db = NULL;
	config_setting_t * item_row = NULL;
	config_setting_t * item_sub = NULL;
	/* item_db */
	he_item_t item;

	config_init(cfg);
	/* read item_db.conf */
	status = config_read_file(cfg, filename);
	if(status != CONFIG_TRUE) {
		offset = sprintf(error, "%s;%d;%s",
			config_error_file(cfg),
			config_error_line(cfg),
			config_error_text(cfg));
		error[offset] = '\0';
		/*exit_abt(error);*/
	}

	/* retrieve data */
	item_db = config_lookup(cfg, "item_db");
	if(config_setting_is_list(item_db)) {
		cnt = config_setting_length(item_db);
		sqlite3_exec(db->db, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, NULL);
		for(i = 0; i < cnt; i++) {
			item_row = config_setting_get_elem(item_db, i);
			memset(&item, 0, sizeof(he_item_t));

			config_setting_lookup_int(item_row, "Id", &item.id);
			config_setting_lookup_string(item_row, "AegisName", &item.aegis);
			config_setting_lookup_string(item_row, "Name", &item.name);
			config_setting_lookup_int(item_row, "Type", &item.type);
			config_setting_lookup_int(item_row, "Buy", &item.buy);
			config_setting_lookup_int(item_row, "Sell", &item.sell);
			config_setting_lookup_int(item_row, "Weight", &item.weight);
			config_setting_lookup_int(item_row, "Atk", &item.atk);
			config_setting_lookup_int(item_row, "Matk", &item.matk);
			config_setting_lookup_int(item_row, "Def", &item.def);
			config_setting_lookup_int(item_row, "Range", &item.range);
			config_setting_lookup_int(item_row, "Slots", &item.slots);
			config_setting_lookup_int(item_row, "Job", &item.job);
			config_setting_lookup_int(item_row, "Upper", &item.upper);
			config_setting_lookup_int(item_row, "Gender", &item.gender);
			config_setting_lookup_int(item_row, "Loc", &item.loc);
			config_setting_lookup_int(item_row, "WeaponLv", &item.weaponlv);
			item_sub = config_setting_get_member(item_row, "EquipLv");
			if(item_sub != NULL && config_setting_is_list(item_sub) == CONFIG_TRUE) {
				item.equiplv[EQUIP_MIN] = config_setting_get_int_elem(item_sub, EQUIP_MIN);
				item.equiplv[EQUIP_MAX] = config_setting_get_int_elem(item_sub, EQUIP_MAX);
			} else if(item_sub != NULL) {
				config_setting_lookup_int(item_sub, "EquipLv", &item.equiplv[EQUIP_MIN]);
				item.equiplv[EQUIP_MAX] = item.equiplv[EQUIP_MIN];
			}
			config_setting_lookup_bool(item_row, "Refine", &item.refine);
			config_setting_lookup_int(item_row, "View", &item.view);
			config_setting_lookup_bool(item_row, "BindOnEquip", &item.bindonequip);
			config_setting_lookup_bool(item_row, "BuyingStore", &item.buyingstore);
			config_setting_lookup_int(item_row, "Delay", &item.delay);
			item_sub = config_setting_get_member(item_row, "Trade");
			if(item_sub != NULL && config_setting_is_group(item_sub) == CONFIG_TRUE) {
				config_setting_lookup_int(item_row, "override", &item.trade[TRADE_OVERRIDE]);
				config_setting_lookup_bool(item_row, "nodrop", &item.trade[TRADE_NODROP]);
				config_setting_lookup_bool(item_row, "notrade", &item.trade[TRADE_NOTRADE]);
				config_setting_lookup_bool(item_row, "partneroverride", &item.trade[TRADE_PARTNEROVERRIDE]);
				config_setting_lookup_bool(item_row, "noselltonpc", &item.trade[TRADE_NOSELLTONPC]);
				config_setting_lookup_bool(item_row, "nocart", &item.trade[TRADE_NOCART]);
				config_setting_lookup_bool(item_row, "nostorage", &item.trade[TRADE_NOSTORAGE]);
				config_setting_lookup_bool(item_row, "nogstorage", &item.trade[TRADE_NOGSTORAGE]);
				config_setting_lookup_bool(item_row, "nomail", &item.trade[TRADE_NOMAIL]);
				config_setting_lookup_bool(item_row, "noauction", &item.trade[TRADE_NOAUCTION]);
			}
			item_sub = config_setting_get_member(item_row, "Nouse");
			if(item_sub != NULL && config_setting_is_group(item_sub) == CONFIG_TRUE) {
				config_setting_lookup_int(item_row, "override", &item.trade[NOUSE_OVERRIDE]);
				config_setting_lookup_bool(item_row, "sitting", &item.trade[NOUSE_SITTING]);
			}
			item_sub = config_setting_get_member(item_row, "Stack");
			if(item_sub != NULL && config_setting_is_list(item_sub) == CONFIG_TRUE) {
				item.equiplv[STACK_AMOUNT] = config_setting_get_int_elem(item_sub, STACK_AMOUNT);
				item.equiplv[STACK_TYPE] = config_setting_get_int_elem(item_sub, STACK_TYPE);
			}
			config_setting_lookup_string(item_row, "Script", &item.script);
			config_setting_lookup_string(item_row, "OnEquipScript", &item.onequipscript);
			config_setting_lookup_string(item_row, "OnUnequipScript", &item.onunequipscript);
			if(item.script == NULL) item.script = "";
			if(item.onequipscript == NULL) item.onequipscript = "";
			if(item.onunequipscript == NULL) item.onunequipscript = "";
			he_item_db_insert(db, &item);
		}
		sqlite3_exec(db->db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
	} else {
		/*exit_abt("item configuration file root setting.");*/
	}
	config_destroy(cfg);
	free(cfg);
}

void he_item_db_insert(struct lt_db_t * db, he_item_t * item) {
	int status = 0;
	sqlite3_clear_bindings(db->he_item_insert);
	sqlite3_bind_int(db->he_item_insert, 1, item->id);
	sqlite3_bind_text(db->he_item_insert, 2, item->aegis, strlen(item->aegis), SQLITE_STATIC);
	sqlite3_bind_text(db->he_item_insert, 3, item->name, strlen(item->name), SQLITE_STATIC);
	sqlite3_bind_int(db->he_item_insert, 4, item->type);
	sqlite3_bind_int(db->he_item_insert, 5, item->buy);
	sqlite3_bind_int(db->he_item_insert, 6, item->sell);
	sqlite3_bind_int(db->he_item_insert, 7, item->weight);
	sqlite3_bind_int(db->he_item_insert, 8, item->atk);
	sqlite3_bind_int(db->he_item_insert, 9, item->matk);
	sqlite3_bind_int(db->he_item_insert, 10, item->def);
	sqlite3_bind_int(db->he_item_insert, 11, item->range);
	sqlite3_bind_int(db->he_item_insert, 12, item->slots);
	sqlite3_bind_int(db->he_item_insert, 13, item->job);
	sqlite3_bind_int(db->he_item_insert, 14, item->upper);
	sqlite3_bind_int(db->he_item_insert, 15, item->gender);
	sqlite3_bind_int(db->he_item_insert, 16, item->loc);
	sqlite3_bind_int(db->he_item_insert, 17, item->weaponlv);
	sqlite3_bind_int(db->he_item_insert, 18, item->equiplv[EQUIP_MIN]);
	sqlite3_bind_int(db->he_item_insert, 19, item->equiplv[EQUIP_MAX]);
	sqlite3_bind_int(db->he_item_insert, 20, item->refine);
	sqlite3_bind_int(db->he_item_insert, 21, item->view);
	sqlite3_bind_int(db->he_item_insert, 22, item->bindonequip);
	sqlite3_bind_int(db->he_item_insert, 23, item->buyingstore);
	sqlite3_bind_int(db->he_item_insert, 24, item->delay);
	sqlite3_bind_int(db->he_item_insert, 25, item->trade[TRADE_OVERRIDE]);
	sqlite3_bind_int(db->he_item_insert, 26, item->trade[TRADE_NODROP]);
	sqlite3_bind_int(db->he_item_insert, 27, item->trade[TRADE_NOTRADE]);
	sqlite3_bind_int(db->he_item_insert, 28, item->trade[TRADE_PARTNEROVERRIDE]);
	sqlite3_bind_int(db->he_item_insert, 29, item->trade[TRADE_NOSELLTONPC]);
	sqlite3_bind_int(db->he_item_insert, 30, item->trade[TRADE_NOCART]);
	sqlite3_bind_int(db->he_item_insert, 31, item->trade[TRADE_NOSTORAGE]);
	sqlite3_bind_int(db->he_item_insert, 32, item->trade[TRADE_NOGSTORAGE]);
	sqlite3_bind_int(db->he_item_insert, 33, item->trade[TRADE_NOMAIL]);
	sqlite3_bind_int(db->he_item_insert, 34, item->trade[TRADE_NOAUCTION]);
	sqlite3_bind_int(db->he_item_insert, 35, item->nouse[NOUSE_OVERRIDE]);
	sqlite3_bind_int(db->he_item_insert, 36, item->nouse[NOUSE_SITTING]);
	sqlite3_bind_int(db->he_item_insert, 37, item->stack[STACK_AMOUNT]);
	sqlite3_bind_int(db->he_item_insert, 38, item->stack[STACK_TYPE]);
	sqlite3_bind_int(db->he_item_insert, 39, item->sprite);
	sqlite3_bind_text(db->he_item_insert, 40, item->script, strlen(item->script), SQLITE_STATIC);
	sqlite3_bind_text(db->he_item_insert, 41, item->onequipscript, strlen(item->onequipscript), SQLITE_STATIC);
	sqlite3_bind_text(db->he_item_insert, 42, item->onunequipscript, strlen(item->onunequipscript), SQLITE_STATIC);
	status = sqlite3_step(db->he_item_insert);
	/*if(status != SQLITE_DONE) exit_abt("fatal error inserting item.");*/
	sqlite3_reset(db->he_item_insert);
}
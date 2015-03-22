/*
 *   file: format.h
 *   date: 03/21/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef FORMAT_H
#define FORMAT_H
 	#include "util.h"
 	#include "range.h"
 	#include "setting.h"
 	#include "db_search.h"
 	#include "sqlite3.h"
	#include "lua.h"
	#include "lauxlib.h"
 	#include "limits.h"
 	/* flavour text */
 	typedef struct flavour_text_t {
 		int item_id;
 		char unidentified_display_name[MAX_NAME_SIZE];
 		char unidentified_resource_name[MAX_NAME_SIZE];
 		char unidentified_description_name[MAX_DESCRIPTION_SIZE];
 		char identified_display_name[MAX_NAME_SIZE];
 		char identified_resource_name[MAX_NAME_SIZE];
 		char identified_description_name[MAX_DESCRIPTION_SIZE];
 		int slot_count;
 		int class_num;
 	} flavour_text_t;
 	#define flavour_text_id_sql "SELECT * FROM flavour_text WHERE id = ?;"

 	/* item type */
 	#define HEALING_ITEM_TYPE 		0
 	#define USABLE_ITEM_TYPE 		2
 	#define ETC_ITEM_TYPE			3
 	#define WEAPON_ITEM_TYPE		4
 	#define ARMOR_ITEM_TYPE			5
 	#define CARD_ITEM_TYPE			6
 	#define PET_EGG_ITEM_TYPE		7
 	#define PET_EQUIP_ITEM_TYPE		8
 	#define AMMO_ITEM_TYPE			10
 	#define DELAY_USABLE_ITEM_TYPE	11
 	#define SHADOW_EQUIP_ITEM_TYPE	12	/* rathena only */
 	#define DELAY_CONFIRM_ITEM_TYPE	18
 	#define ITEM_TYPE_SIZE			20

 	/* item fields */
 	#define FLAVOUR_ITEM_FIELD			0
 	#define TYPE_ITEM_FIELD				1
 	#define BUY_ITEM_FIELD				2
 	#define SELL_ITEM_FIELD				3
 	#define WEIGHT_ITEM_FIELD			4
 	#define ATK_ITEM_FIELD				5
 	#define DEF_ITEM_FIELD				6
 	#define RANGE_ITEM_FIELD			7
 	#define JOB_ITEM_FIELD				8
 	#define GENDER_ITEM_FIELD			9
 	#define LOC_ITEM_FIELD				10
 	#define WEAPON_LEVEL_ITEM_FIELD 	11
 	#define LEVEL_REQUIRE_ITEM_FIELD 	12
 	#define REFINE_ABILITY_ITEM_FIELD	13
	#define SCRIPT_ITEM_FIELD			14

 	typedef struct format_field_t {
 		int item_field;
 		struct format_field_t * next;
 	} format_field_t;

 	typedef struct format_type_t {
 		range_t * item_id;
 		format_field_t * format;
 		struct format_type_t * next;
 	} format_type_t;

	typedef struct format_t {
		int file_format;
		/* flavour text database */
		sqlite3 * flavour_text;
		sqlite3_stmt * flavour_text_id_search;
		/* item format rules */
		format_type_t * format_type_list[ITEM_TYPE_SIZE];
	} format_t;

	/* item format rule functions */
	int init_format(format_t *, lua_State *, int, int);
	int init_format_type(format_t *, lua_State *, int, int);
	int deit_format(format_t *);

	/* auxiliary item format loading functions */
	int load_item_id(format_type_t *, lua_State *, int);
	int load_item_id_re(format_type_t *, lua_State *, int);
	int load_type_format(format_type_t *, lua_State *, int);
	int load_type_format_field(format_field_t *, lua_State *, int);

	/* flavour text database load and search functions */
	int init_flavour_db(format_t *, const char *);
	int flavour_text_id_search(format_t *, flavour_text_t *, int);

	/* write item fields */
	int write_item(format_t *, item_t *, char *);
#endif

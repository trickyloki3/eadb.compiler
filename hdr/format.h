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
 	#include "table.h"
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

 	/* supported file formats */
 	#define FORMAT_TXT		0x1
 	#define FORMAT_LUA		0x2

 	/* item type for eathena, rathena, and hercules */
 	#define HEALING_ITEM_TYPE 			0
 	#define USABLE_ITEM_TYPE 			2
 	#define ETC_ITEM_TYPE				3
 	#define WEAPON_ITEM_TYPE			4
 	#define ARMOR_ITEM_TYPE				5
 	#define CARD_ITEM_TYPE				6
 	#define PET_EGG_ITEM_TYPE			7
 	#define PET_EQUIP_ITEM_TYPE			8
 	#define AMMO_ITEM_TYPE				10
 	#define DELAY_USABLE_ITEM_TYPE		11
 	#define SHADOW_EQUIP_ITEM_TYPE		12
 	#define DELAY_CONFIRM_ITEM_TYPE		18
 	#define ITEM_TYPE_SIZE				20

 	/* item fields for eathena, rathena, and hercules */
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
	#define VIEW_ITEM_FIELD				15
	#define UPPER_ITEM_FIELD			16
	#define MATK_ITEM_FIELD				17
	#define BINDONEQUIP_ITEM_FIELD		18
	#define BUYINGSTORE_ITEM_FIELD		19
	#define DELAY_ITEM_FIELD			20
	#define TRADE_ITEM_FIELD			21
	#define STACK_ITEM_FIELD			22

 	/* weapon type for eathena, rathena, and hercules */
	#define FIST_WEAPON_TYPE			0x10000000
	#define DAGGER_WEAPON_TYPE			0x00000001
	#define H1_SWORD_WEAPON_TYPE		0x00000002
	#define H2_SWORD_WEAPON_TYPE		0x00000004
	#define H1_SPEAR_WEAPON_TYPE		0x00000008
	#define H2_SPEAR_WEAPON_TYPE		0x00000010
	#define H1_AXE_WEAPON_TYPE			0x00000020
	#define H2_AXE_WEAPON_TYPE			0x00000040
	#define MACE_WEAPON_TYPE			0x00000080
 	#define STAVE_WEAPON_TYPE			0x00000100
 	#define BOW_WEAPON_TYPE				0x00000200
 	#define KNUCKLE_WEAPON_TYPE			0x00000400
 	#define INSTRUMENT_WEAPON_TYPE		0x00000800
 	#define WHIP_WEAPON_TYPE			0x00001000
 	#define BOOK_WEAPON_TYPE			0x00002000
 	#define KATAR_WEAPON_TYPE			0x00004000
 	#define REVOLVER_WEAPON_TYPE		0x00008000
 	#define RIFLES_WEAPON_TYPE			0x00010000
 	#define GATLING_GUN_WEAPON_TYPE		0x00020000
 	#define SHOTGUN_WEAPON_TYPE			0x00040000
 	#define GRENADE_WEAPON_TYPE			0x00080000
 	#define SHURIKEN_WEAPON_TYPE		0x00100000
 	#define H2_STAVE_WEAPON_TYPE		0x00200000
 	#define DUAL_DAGGER_WEAPON_TYPE		0x00400000
 	#define DUAL_SWORD_WEAPON_TYPE		0x00800000
 	#define DUAL_AXE_WEAPON_TYPE		0x01000000
 	#define DAGGER_SWORD_WEAPON_TYPE	0x02000000
 	#define DAGGER_AXE_WEAPON_TYPE		0x04000000
 	#define SWORD_AXE_WEAPON_TYPE		0x08000000

 	/* item field specifies how the item field is written */
 	typedef struct format_field_t {
 		int field;						/* item field value (*_ITEM_FIELD macros) */
 		char text[FMT_TEXT_SIZE];		/* item field category text, i.e. "Weight:", "Job:", etc. */
 		struct format_field_t * next;	/* next item field to write */
 	} format_field_t;

 	/* item rule filters by item id, then by specific item type filters */
 	typedef struct format_rule_t {
 		range_t * item_id;				/* item id range for which format applies */
 		int weapon_type;				/* weapon type for which format applies (bitmask) */
 		format_field_t * format;
 		struct format_rule_t * next;	/* next item rule to test for match */
 	} format_rule_t;

 	/* high level structure for item rule list and flavour text database */
	typedef struct format_t {
		int server_type;									/* eAthena, rAthena, or Hercules */
		int file_format;									/* idnum2itemdesctabke.txt or itemInfo.lua */
		format_rule_t * rule_list[ITEM_TYPE_SIZE];			/* format rules for each item type */
		sqlite3 * flavour_text;								/* flavour text database */
		sqlite3_stmt * flavour_text_id_search;				/* search for flavour text by item id */
	} format_t;

	/* flavour text database load and search functions */
	int init_flavour_db(format_t *, const char *);
	int flavour_text_id_search(format_t *, flavour_text_t *, int);

	/* item format rule functions */
	int init_format(format_t *, lua_State *, int, int, int);
	int init_format_type(format_t *, lua_State *, int, int);
	int load_item_id(format_rule_t *, lua_State *, int);
	int load_item_id_re(format_rule_t *, lua_State *, int);
	int load_item_field(format_rule_t *, lua_State *, int);
	int load_item_field_re(format_field_t *, lua_State *, int);
	int load_weapon_type(format_rule_t *, lua_State *, int);
	int lua_get_field(lua_State *, int, const char *, int);
	int deit_format(format_t *);

	/* write item fields */
	int write_item(FILE *, format_t *, item_t *, char *);
	int write_item_text(FILE *, format_t *, format_field_t *, item_t *, char *);
	int format_flavour_text(char *, int *, format_t *, format_field_t *, flavour_text_t *, int);
	int format_script(char *, int *, format_field_t *, char *);
	int format_integer(char *, int *, format_field_t *, int);			/* non-interpreted values */
	int format_refinement(char *, int *, format_field_t *, int, int);	/* refinement is interpreted either refine (don't write text) or nonrefine (write text) */
	int format_gender(char *, int *, format_field_t *, int);			/* gender is interpreted female (0), male(1), unisex(2) */
	int format_view(char *, int *, format_field_t *, int, int);			/* view is interpreted only for weapon and ammo type */
	int format_type(char *, int *, format_field_t *, int);
	int format_job(char *, int *, format_field_t *, int, int, int);
	int format_upper(char *, int *, format_field_t *, int);
	int format_location(char *, int *, format_field_t *, int);
	int format_weight(char *, int * , format_field_t *, int);
	int format_boolean(char *, int * , format_field_t *, int);
	int format_delay(char *, int * , format_field_t *, int);
	int format_trade(char *, int * , format_field_t *, int *);
	int format_stack(char *, int * , format_field_t *, int *);
#endif

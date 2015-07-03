/*
 *   file: db_search.h
 *   date: 03/08/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef DB_SEARCH_H
#define DB_SEARCH_H
 	#include "db_eathena.h"
	#include "db_rathena.h"
	#include "db_hercules.h"
	#include "db_resources.h"

 	/* database modes */
 	#define MODE_EATHENA 	0x1
	#define MODE_RATHENA 	0x2
	#define MODE_HERCULES 	0x4

 	/* resource database */
 	#define option_res_name_sql		"SELECT * FROM option_res WHERE opt = ? COLLATE NOCASE;"
 	#define map_res_name_sql 		"SELECT * FROM map_res WHERE map = ?;"
	#define map_res_id_sql 			"SELECT * FROM map_res WHERE id = ?;"
 	#define bonus_search_sql 		"SELECT * FROM bonus_res WHERE pref = ? AND buff = ? COLLATE NOCASE;"
 	#define status_search_sql 		"SELECT * FROM status_res WHERE scid = ? AND scstr = ? COLLATE NOCASE;"
 	#define var_search_sql 			"SELECT * FROM var_res WHERE id = ? COLLATE NOCASE;"
 	#define block_res_key_sql 		"SELECT * FROM block_res WHERE key = ? COLLATE NOCASE;"
 	#define id_res_id_sql			"SELECT * FROM id_res WHERE id = ?;"
 	#define num_id_res_id_sql		"SELECT * FROM numid_res WHERE id = ?;"

 	/* constant database */
 	typedef struct const_t {
 		char name[MAX_NAME_SIZE];
		int value;
		int type;
 	} const_t;
 	#define	const_ea_name_search	"SELECT * FROM const_ea WHERE name = ? COLLATE NOCASE;"
 	#define	const_ra_name_search	"SELECT * FROM const_ra WHERE name = ? COLLATE NOCASE;"
 	#define	const_he_name_search	"SELECT * FROM const_he WHERE name = ? COLLATE NOCASE;"
 	#define	const_ea_id_search		"SELECT * FROM const_ea WHERE value = ?;"
 	#define	const_ra_id_search		"SELECT * FROM const_ra WHERE value = ?;"
 	#define	const_he_id_search		"SELECT * FROM const_he WHERE value = ?;"

 	/* skill database */
 	typedef struct skill_t {
 		int id;
 		int maxlv;
 		char name[MAX_NAME_SIZE];
 		char desc[MAX_NAME_SIZE];
 	} skill_t;
 	#define skill_ea_name_search 	"SELECT id, max, name, desc FROM skill_ea WHERE name = ? COLLATE NOCASE;"
 	#define skill_ra_name_search 	"SELECT id, max, name, desc FROM skill_ra WHERE name = ? COLLATE NOCASE;"
 	#define skill_he_name_search 	"SELECT id, max, name, desc FROM skill_he WHERE name = ? COLLATE NOCASE;"
 	#define skill_ea_id_search 		"SELECT id, max, name, desc FROM skill_ea WHERE id = ?;"
	#define skill_ra_id_search 		"SELECT id, max, name, desc FROM skill_ra WHERE id = ?;"
	#define skill_he_id_search 		"SELECT id, max, name, desc FROM skill_he WHERE id = ?;"

 	/* item database */
 	typedef struct item_t {
 		int id;
 		char name[MAX_NAME_SIZE];
 		int type;
		int buy;
		int sell;
		int weight;
		int atk;
		int def;
		int range;
		int slots;
		int job;
		int upper;
		int gender;
		int loc;
		int wlv;
		int elv_min;
		int elv_max;
		int refineable;
		int view;
		char script[MAX_SCRIPT_SIZE];
		/* rathena and hercules only */
		int matk;
		/* hercules only */
		int bindonequip;
		int buyingstore;
		int delay;
		int trade[TRADE_TOTAL];
		int stack[STACK_TOTAL];
 	} item_t;
 	
	#define item_ea_iterate			"SELECT id, eathena, type, buy, sell, weight, atk, def, range, slots, job, upper, gender, loc, wlv, elv, refineable, view, script from item_ea;"
 	#define item_ra_iterate			"SELECT id, eathena, type, buy, sell, weight, matk, atk, def, range, slots, job, upper, gender, loc, wlv, elv, refineable, view, script from item_ra;"
 	#define item_he_iterate			"SELECT Id, Name, Type, Buy, Sell, Weight, Matk, Atk, Def, Range, Slots, Job, Upper, Gender, Loc, WeaponLv, EquipLvMin, Refine, View, Script,"\
 									"BindOnEquip, BuyingStore, Delay, override, nodrop, notrade, partneroverride, noselltonpc, nocart, nostorage, nogstorage, nomail, noauction,"\
 									"StackAmount, StackType from item_he;"
 	#define item_ea_name_search 	"SELECT id, eathena, script FROM item_ea WHERE eathena = ? OR aegis = ? COLLATE NOCASE;"
	#define item_ra_name_search 	"SELECT id, eathena, script FROM item_ra WHERE eathena = ? OR aegis = ? COLLATE NOCASE;"
	#define item_he_name_search 	"SELECT Id, Name, Script FROM item_he WHERE Name = ? OR AegisName = ? COLLATE NOCASE;"
	#define item_ea_id_search 		"SELECT id, eathena, script FROM item_ea WHERE id = ?;"
	#define item_ra_id_search 		"SELECT id, eathena, script FROM item_ra WHERE id = ?;"
	#define item_he_id_search 		"SELECT Id, Name, Script FROM item_he WHERE id = ?;"

	/* mob database */
	typedef struct mob_t {
		int id;
		char name[MAX_NAME_SIZE];
	} mob_t;
	#define mob_ea_id_search 		"SELECT id, iro FROM mob_ea WHERE id = ?;"
	#define mob_ra_id_search 		"SELECT id, iro FROM mob_ra WHERE id = ?;"
	#define mob_he_id_search 		"SELECT id, iro FROM mob_he WHERE id = ?;"

	/* mercenary database */
	typedef struct merc_t {
		int id;
		char name[MAX_NAME_SIZE];
	} merc_t;
	#define merc_ea_id_search 		"SELECT id, name FROM mercenary_ea WHERE id = ?;"
	#define merc_ra_id_search 		"SELECT id, name FROM mercenary_ra WHERE id = ?;"
	#define merc_he_id_search 		"SELECT id, name FROM mercenary_he WHERE id = ?;"

	/* pet database */
	typedef struct pet_t {
		int mob_id;
		char pet_script[MAX_SCRIPT_SIZE];
		char loyal_script[MAX_SCRIPT_SIZE];
	} pet_t;
	#define pet_ea_id_search 		"SELECT pet_script, loyal_script FROM pet_ea WHERE mob_id = ?;"
	#define pet_ra_id_search 		"SELECT pet_script, loyal_script FROM pet_ra WHERE mob_id = ?;"
	#define pet_he_id_search 		"SELECT pet_script, loyal_script FROM pet_he WHERE mob_id = ?;"
	
	/* produce database */
	typedef struct produce_t {
		int id;
		int item_id;
		int item_lv;
		int skill_id;
		int skill_lv;
		int item_id_req[MAX_INGREDIENT];
		int item_amount_req[MAX_INGREDIENT];
		int ingredient_count;
		struct produce_t * next;
	} produce_t;
	#define produce_ea_id_search 	"SELECT item_id, item_lv, req_skill, req_skill_lv, material, amount FROM produce_ea WHERE item_lv = ?;"
	#define produce_ra_id_search 	"SELECT item_id, item_lv, req_skill, req_skill_lv, material, amount FROM produce_ra WHERE item_lv = ?;"
	#define produce_he_id_search 	"SELECT item_id, item_lv, req_skill, req_skill_lv, material, amount FROM produce_he WHERE item_lv = ?;"

	/* item group databases */
	typedef struct item_group_t {
		int group_id;
		int item_id;
		int rate;
	} item_group_t;
	#define ea_item_group_search 	"SELECT group_id, item_id, rate FROM item_group_ea WHERE group_id = ?;"
	#define ra_item_group_search 	"SELECT group_id, item_id, rate FROM item_group_ra WHERE group_id = ?;"

	/* item combo database */
	typedef struct combo_t {
		char script[MAX_SCRIPT_SIZE];
		char group[MAX_SCRIPT_SIZE];
		struct combo_t * next;
	} combo_t;
	#define item_combo_ra_id_search "SELECT script, combo_group FROM item_combo_ra WHERE id = ?;"
	#define item_combo_he_id_search "SELECT script, combo_group FROM item_combo_he WHERE id = ?;"

 	typedef struct db_search_t {
 		sqlite3 * athena;
 		sqlite3 * resource;
 		/* eathena, rathena, or hercules */
 		int mode;
 		/* resource database search */
 		sqlite3_stmt * option_res_name_search;
 		sqlite3_stmt * map_res_name_search;
 		sqlite3_stmt * map_res_id_search;
 		sqlite3_stmt * bonus_res_prefix_name_search;
 		sqlite3_stmt * status_res_id_name_search;
 		sqlite3_stmt * var_res_id_search;
 		sqlite3_stmt * block_res_key_search;
 		sqlite3_stmt * id_res_id_search;
 		sqlite3_stmt * num_id_res_id_search;
 		/* athena database search */
 		sqlite3_stmt * item_iterate;
 		sqlite3_stmt * const_db_name_search;
 		sqlite3_stmt * const_db_id_search;
 		sqlite3_stmt * skill_db_name_search;
 		sqlite3_stmt * skill_db_id_search;
 		sqlite3_stmt * item_db_name_search;
 		sqlite3_stmt * item_db_id_search;
 		sqlite3_stmt * mob_db_id_search;
 		sqlite3_stmt * merc_db_id_search;
 		sqlite3_stmt * pet_db_id_search;
 		sqlite3_stmt * produce_db_id_search;
 		sqlite3_stmt * item_group_id_search;
 		sqlite3_stmt * item_combo_id_search;
 	} db_search_t;

	ITEMC_API ITEML_API int init_db(db_search_t * db, int mode, const char * resource_path, const char * database_path);
	ITEMC_API ITEML_API int deit_db(db_search_t * db);
	ITEML_API int test_db(void);

 	/* resource database querying */
	ITEML_API int option_search_name(db_search_t * db, option_res * option, const char * name);
	ITEML_API int map_search_name(db_search_t * db, map_res * map, const char * name);
	ITEML_API int map_search_id(db_search_t * db, map_res * map, int id);
	ITEML_API int bonus_search_name(db_search_t * db, bonus_res * bonus, const char * prefix, const char * name);
	ITEML_API int status_search_id_name(db_search_t * db, status_res * status, int id, const char * name);
	ITEML_API int var_search_id(db_search_t * db, var_res * var, const char * id);
	ITEML_API int block_search_name(db_search_t * db, block_res * block, const char * name);
	ITEML_API int id_res_id_search(db_search_t * db, nid_res * nid, int id);
	ITEML_API int num_id_res_id_search(db_search_t * db, nid_res * nid, int id);

 	/* athena database querying */
	ITEMC_API ITEML_API int item_iterate(db_search_t * db, item_t * item);
	ITEML_API int const_db_search_name(db_search_t * db, const_t * search, const char * name);
	ITEML_API int const_db_search_id(db_search_t * db, const_t * search, int id);
	ITEML_API int skill_db_search_name(db_search_t * db, skill_t * search, const char * name);
	ITEML_API int skill_db_search_id(db_search_t * db, skill_t * search, int id);
	ITEML_API int item_db_search_name(db_search_t * db, item_t * search, const char * name);
	ITEML_API int item_db_search_id(db_search_t * db, item_t * search, int id);
	ITEML_API int mob_db_search_id(db_search_t * db, mob_t * search, int id);
	ITEML_API int merc_db_search_id(db_search_t * db, merc_t * search, int id);
	ITEML_API int pet_db_search_id(db_search_t * db, pet_t * search, int id);
	ITEML_API int produce_db_search_id(db_search_t * db, produce_t ** search, int id);
	ITEML_API void produce_free(produce_t * produce);
	ITEML_API int item_group_db_search_id(db_search_t * db, item_group_t * search, int id, char * buffer);
	ITEML_API int item_combo_db_search_id(db_search_t * db, combo_t ** search, int id);
	ITEML_API void combo_free(combo_t * combo);

 	/* debug */
	ITEML_API void print_stmt(void * data, const char * sql);
#endif

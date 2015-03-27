/*
 *   file: db_rathena.h
 *   date: 03/01/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef DB_RATHENA_H
#define DB_RATHENA_H
	#include "load.h"
 	#include "util.h"
	#include "setting.h"
 	#include "sqlite3.h"

 	/* rathena database type definitions 
	 * -----------------------------------------
	 * type name 	- 	type reference
	 * -----------------------------------------
	 * item_ra 			- 	rathena item database
	 * mob_ra 			- 	rathena mob database 
	 * skill_ra 		- 	rathena skill database 
	 * produce_ra		- 	rathena produce database
	 * mercenary_ra 	-	rathena mecenary database
	 * pet_ra			-	rathena pet database
	 * item_group_ra 	-	rathena item group database
	 * const_ra 		- 	rathena constant database
	 * package_ra 		- 	rathena package database
	 * combo_ra 		- 	rathena combo database
	 */

	/* database record counts */
	#define ITEM_RA_FIELD_COUNT 			22
	#define MOB_RA_FIELD_COUNT				57
	#define SKILL_RA_FIELD_COUNT			18
	#define MERCENARY_RA_FIELD_COUNT 		26
	#define PET_RA_FIELD_COUNT				22
	#define ITEM_GROUP_RA_FIELD_COUNT 		3
	#define ITEM_COMBO_FIELD_COUNT			2

	typedef struct {
		int id;
		char aegis[MAX_NAME_SIZE];
		char eathena[MAX_NAME_SIZE];
		int type;
		int buy;
		int sell;
		int weight;
		int atk;
		int matk;
		int def;
		int range;
		int slots;
		int job;
		int upper;
		int gender;
		int loc;
		int wlv;
		int elv;
		int elv_max;
		int refineable;
		int view;
		char script[MAX_SCRIPT_SIZE];
		char onequip[MAX_SCRIPT_SIZE];
		char onunequip[MAX_SCRIPT_SIZE];
	} item_ra;

	typedef struct {
		int id;
		char sprite[MAX_NAME_SIZE];
		char kro[MAX_NAME_SIZE];
		char iro[MAX_NAME_SIZE];
		int lv;
		int hp;
		int sp;
		int exp;
		int jexp;
		int range;
		int atk1;
		int atk2;
		int def;
		int mdef;
		int str;
		int agi;
		int vit;
		int intr;
		int dex;
		int luk;
		int range2;
		int range3;
		int scale;
		int race;
		int element;
		int mode;
		int speed;
		int adelay;
		int amotion;
		int dmotion;
		int mexp;
		int mvp1id;
		int mvp1per;
		int mvp2id;
		int mvp2per;
		int mvp3id;
		int mvp3per;
		int drop1id;
		int drop1per;
		int drop2id;
		int drop2per;
		int drop3id;
		int drop3per;
		int drop4id;
		int drop4per;
		int drop5id;
		int drop5per;
		int drop6id;
		int drop6per;
		int drop7id;
		int drop7per;
		int drop8id;
		int drop8per;
		int drop9id;
		int drop9per;
		int dropcardid;
		int dropcardper;
	} mob_ra;

	typedef struct {
		int id;
		varlist range;
		int hit;
		int inf;
		varlist element;
		int nk;
		varlist splash;
		int maxlv;
		varlist hit_amount;
		char cast_cancel[MAX_NAME_SIZE];		/* 'yes' or 'no' string */
		int cast_def_reduce_rate;
		int inf2;
		varlist maxcount;
		char type[MAX_NAME_SIZE];				/* 'none', 'weapon', 'magic', 'misc' */
		varlist blow_count;
		int inf3;
		char name[MAX_NAME_SIZE];
		char desc[MAX_NAME_SIZE];
	} skill_ra;

	typedef struct produce_ra {
		int id;
		int item_id;
		int item_lv;
		int skill_id;
		int skill_lv;
		int item_id_req[MAX_INGREDIENT];
		int item_amount_req[MAX_INGREDIENT];
		int ingredient_count;
	} produce_ra;

	typedef struct {
		int id;
		char sprite[MAX_NAME_SIZE];
		char name[MAX_NAME_SIZE];
		int lv;
		int hp;
		int sp;
		int range1;
		int atk1;
		int atk2;
		int def;
		int mdef;
		int str;
		int agi;
		int vit;
		int intr;
		int dex;
		int luk;
		int range2;
		int range3;
		int scale;
		int race;
		int element;
		int speed;
		int adelay;
		int amotion;
		int dmotion;
	} mercenary_ra;

	typedef struct {
		int mob_id;
		char name[MAX_NAME_SIZE];
		char jname[MAX_NAME_SIZE];
		int lure_id;
		int egg_id;
		int equip_id;
		int food_id;
		int fullness;
		int hungry_delay;
		int r_hungry;
		int r_full;
		int intimate;
		int die;
		int capture;
		int speed;
		int s_performance;
		int talk_convert;
		int attack_rate;
		int defence_attack_rate;
		int change_target_rate;
		char pet_script[MAX_SCRIPT_SIZE];
		char loyal_script[MAX_SCRIPT_SIZE];
	} pet_ra;

	typedef struct {
		int id;
		char name[MAX_NAME_SIZE];
		int item_id;
		int rate;
	} item_group_ra;

	typedef struct {
		char name[MAX_NAME_SIZE];
		int value;
		int type;
	} const_ra;

	typedef struct {
		int group_id;
		int item_id;
		char group_name[MAX_NAME_SIZE];
		char item_name[MAX_NAME_SIZE];
		int rate;
		int amount;
		int random;
		int announced;
		int duration;
		int guid;
		int bound;
		int named;
	} package_ra;

	typedef struct combo_ra {
		varlist item_id;
		char script[MAX_SCRIPT_SIZE];
		struct combo_ra * next;
	} combo_ra;

	/* rathena native database loading */
	extern native_config_t load_ra_native[RATHENA_DB_COUNT];
	int item_ra_load(void * db, int row, int col, char * val);
	int mob_ra_load(void * db, int row, int col, char * val);
	int skill_ra_load(void * db, int row, int col, char * val);
	int produce_ra_load(void * db, int row, int col, char * val);
	int mercenary_ra_load(void * db, int row, int col, char * val); 
	int pet_ra_load(void * db, int row, int col, char * val);
	int item_group_ra_load(void * db, int row, int col, char * val);
	int const_ra_load(void * db, int row, int col, char * val);
	int package_ra_load(void * db, int row, int col, char * val);
	int combo_ra_load(void * db, int row, int col, char * val);

	/* rathena sqlite3 database interface */
	#define rathena_database_sql	"DROP TABLE IF EXISTS item_ra;"																\
									"DROP TABLE IF EXISTS mob_ra;"																\
									"DROP TABLE IF EXISTS skill_ra;"															\
									"DROP TABLE IF EXISTS produce_ra;"															\
									"DROP TABLE IF EXISTS mercenary_ra;"														\
									"DROP TABLE IF EXISTS pet_ra;"																\
									"DROP TABLE IF EXISTS item_group_ra;"														\
									"DROP TABLE IF EXISTS const_ra;"															\
									"DROP TABLE IF EXISTS item_combo_ra;"														\
									""																							\
									"CREATE TABLE IF NOT EXISTS item_ra(" 														\
									"id INTEGER PRIMARY KEY, aegis TEXT, eathena TEXT," 										\
									"type INTEGER, buy INTEGER, sell INTEGER, weight INTEGER, " 								\
									"matk INTEGER, atk INTEGER, def INTEGER, range INTEGER, slots INTEGER, " 					\
									"job INTEGER, upper INTEGER, gender INTEGER, loc INTEGER, " 								\
									"wlv INTEGER, elv INTEGER, elv_max INTEGER, refineable INTEGER, view INTEGER, " 			\
									"script TEXT, onequip TEXT, onunequip TEXT);"												\
									""																							\
									"CREATE TABLE IF NOT EXISTS mob_ra(" 														\
									"id INTEGER PRIMARY KEY, sprite TEXT, kro TEXT, iro TEXT, " 								\
									"lv INTEGER, hp INTEGER, sp INTEGER, exp INTEGER, jexp INTEGER, " 							\
									"range INTEGER, atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " 					\
									"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, dex INTEGER, " 						\
									"luk INTEGER, range2 INTEGER, range3 INTEGER, scale INTEGER, race INTEGER, " 				\
									"element INTEGER, mode INTEGER, speed INTEGER, adelay INTEGER, amotion INTEGER, " 			\
									"dmotion INTEGER, mexp INTEGER, mvp1id INTEGER, mvp1per INTEGER, " 							\
									"mvp2id INTEGER, mvp2per INTEGER, mvp3id INTEGER, mvp3per INTEGER, drop1id INTEGER, " 		\
									"drop1per INTEGER, drop2id INTEGER, drop2per INTEGER, drop3id INTEGER, drop3per INTEGER, " 	\
									"drop4id INTEGER, drop4per INTEGER, drop5id INTEGER, drop5per INTEGER, drop6id INTEGER, " 	\
									"drop6per INTEGER, drop7id INTEGER, drop7per INTEGER, drop8id INTEGER, drop8per INTEGER, " 	\
									"drop9id INTEGER, drop9per INTEGER, dropcardid INTEGER, dropcardper INTEGER); "				\
									""																							\
									"CREATE TABLE IF NOT EXISTS skill_ra(" 														\
									"id INTEGER PRIMARY KEY, range TEXT, hit INTEGER, inf INTEGER, " 							\
									"element TEXT, nk INTEGER, splash TEXT, max INTEGER, hit_amount INTEGER, " 					\
									"cast_cancel TEXT, cast_def_reduce_rate INTEGER, inf2 INTEGER, " 							\
									"maxcount TEXT, type TEXT, blow_count TEXT, inf3 INTEGER, name TEXT, desc TEXT);"			\
									""																							\
									"CREATE TABLE IF NOT EXISTS produce_ra(" 													\
									"id INTEGER PRIMARY KEY, item_id INTEGER, item_lv INTEGER, " 								\
									"req_skill INTEGER, req_skill_lv INTEGER, material TEXT, amount TEXT);"						\
									""																							\
									"CREATE TABLE IF NOT EXISTS mercenary_ra(" 													\
									"id INTEGER PRIMARY KEY, sprite TEXT, name TEXT, " 											\
									"lv INTEGER, hp INTEGER, sp INTEGER, range1 INTEGER, " 										\
									"atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " 									\
									"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, " 									\
									"dex INTEGER, luk INTEGER, range2 INTEGER, range3 INTEGER, "								\
									"scale INTEGER, race INTEGER, element INTEGER, speed INTEGER, " 							\
									"adelay INTEGER, amotion INTEGER, dmotion INTEGER);"										\
									""																							\
									"CREATE TABLE IF NOT EXISTS pet_ra(" 														\
									"mob_id INTEGER PRIMARY KEY, pet_name TEXT, pet_jname TEXT, " 								\
									"lure_id INTEGER, egg_id INTEGER, equip_id INTEGER, " 										\
									"food_id INTEGER, fullness INTEGER, hungry_delay INTEGER, " 								\
									"r_hungry INTEGER, r_full INTEGER, intimate INTEGER, " 										\
									"die INTEGER, capture INTEGER, speed INTEGER, " 											\
									"s_performance INTEGER, talk_convert INTEGER, attack_rate INTEGER, " 						\
									"defence_attack_rate INTEGER, change_target_rate INTEGER, pet_script TEXT, " 				\
									"loyal_script TEXT);"																		\
									""																							\
									"CREATE TABLE IF NOT EXISTS item_group_ra(group_id INTEGER, item_id INTEGER, rate INTEGER);"\
									""																							\
									"CREATE TABLE IF NOT EXISTS const_ra(name TEXT, value INTEGER, type INTEGER);"				\
									""																							\
									"CREATE TABLE IF NOT EXISTS item_combo_ra(" 												\
									"id INTEGER, script TEXT, combo_group TEXT, PRIMARY KEY(id, script));"						



	#define item_ra_insert 			"INSERT INTO item_ra VALUES(?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 
	#define mob_ra_insert 			"INSERT INTO mob_ra VALUES(" 																\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?);"
	#define skill_ra_insert 		"INSERT INTO skill_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define produce_ra_insert 		"INSERT INTO produce_ra VALUES(?, ?, ?, ?, ?, ?, ?);"
	#define mercenary_ra_insert 	"INSERT INTO mercenary_ra VALUES(" 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 													\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 
	#define pet_ra_insert 			"INSERT INTO pet_ra VALUES(" 																\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," 															\
									"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define item_group_ra_insert 	"INSERT INTO item_group_ra VALUES(?, ?, ?);"
	#define const_ra_insert			"INSERT INTO const_ra VALUES(?, ?, ?);"
	#define item_combo_ra_insert 	"INSERT INTO item_combo_ra VALUES(?, ?, ?);"

	typedef struct db_ra_aux_t {
		sqlite3_stmt * const_ra_name_search;
		sqlite3_stmt * item_ra_name_search;
		sqlite3_stmt * item_ra_id_search;
	} db_ra_aux_t;

	typedef struct db_ra_t {
		sqlite3 * db;
		sqlite3_stmt * item_ra_sql_insert;
		sqlite3_stmt * mob_ra_sql_insert;
		sqlite3_stmt * skill_ra_sql_insert;
		sqlite3_stmt * produce_ra_sql_insert;
		sqlite3_stmt * mercenary_ra_sql_insert;
		sqlite3_stmt * pet_ra_sql_insert;
		sqlite3_stmt * item_group_ra_sql_insert;
		sqlite3_stmt * const_ra_sql_insert;
		sqlite3_stmt * item_combo_ra_sql_insert;
	} db_ra_t;

	/* database loading depends on the path of the database */
	int create_rathena_database(db_ra_t * db, const char * path);
	int finalize_rathena_database(db_ra_t * db);
	int item_ra_sql_load(db_ra_t * db, const char * path);
	int mob_ra_sql_load(db_ra_t * db, const char * path);
	int skill_ra_sql_load(db_ra_t * db, const char * path);
	int produce_ra_sql_load(db_ra_t * db, const char * path);
	int mercenary_ra_sql_load(db_ra_t * db, const char * path);
	int pet_ra_sql_load(db_ra_t * db, const char * path);
	int const_ra_sql_load(db_ra_t * db, const char * path);
	int item_group_ra_sql_load(db_ra_t * db, const char * path, db_ra_aux_t * db_search);
	int item_package_ra_sql_load(db_ra_t * db, const char * path, db_ra_aux_t * db_search);
	int item_combo_ra_sql_load(db_ra_t * db, const char * path, db_ra_aux_t * db_search);

	/* item group and combo require searching existing database */
	int init_ra_search(db_ra_t * db, db_ra_aux_t * db_search);
	int deit_ra_search(db_ra_aux_t * db_search);
	int const_ra_name_search(db_ra_aux_t * db_search, char * group_name, const_ra * const_name_search);
	int item_ra_name_search(db_ra_aux_t * db_search, char * item_name, item_ra * item_name_search);
	int item_ra_id_search(db_ra_aux_t * db_search, int item_id, item_ra * item_name_search);
#endif

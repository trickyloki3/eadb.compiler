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
		struct produce_ra * next;
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
		int group_id;
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

	/* rathena auxiliary */
	int load_rathena_database(const char * eathena_path);
#endif

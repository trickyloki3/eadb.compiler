/*
 *   file: db_eathena.h
 *   date: 02/28/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef DB_EATHENA_H
#define DB_EATHENA_H
 	#include "load.h"
 	#include "util.h"
	#include "setting.h"

	/* eathena database type definitions 
	 * -----------------------------------------
	 * type name 	- 	type reference
	 * -----------------------------------------
	 * item_ea 			- 	eathena item database
	 * mob_ea 			- 	eathena mob database 
	 * skill_ea 		- 	eathena skill database 
	 * produce_ea		- 	eathena produce database
	 * mercenary_ea 	-	eathena mecenary database
	 * pet_ea			-	eathena pet database
	 * item_group_ea 	-	eathena item group database
	 */

	/* database record counts */
	#define ITEM_EA_FIELD_COUNT 			22
	#define MOB_EA_FIELD_COUNT 				58
	#define SKILL_EA_FIELD_COUNT 			17
	#define MERCENARY_EA_FIELD_COUNT 		26
	#define PET_EA_FIELD_COUNT				22
	#define ITEM_GROUP_EA_FIELD_COUNT 		3
	
	/* default database filenames */	
	#define MOB_DB_FILENAME					"mob_db.txt"
	#define SKILL_DB_FILENAME				"skill_db.txt"
	#define PRODUCE_DB_FILENAME				"produce_db.txt"
	#define MERCENARY_DB_FILENAME			"mercenary_db.txt"
	#define PET_DB_FILENAME					"pet_db.txt"
	#define ITEM_DB_FILENAME				"item_db.txt"
	#define CONST_DB_FILENAME				"const.txt"

	/* default item group database filenames */
	#define ITEM_GROUP_BLUEBOX_FILENAME		"item_bluebox.txt"
	#define ITEM_GROUP_CARDALBUM_FILENAME	"item_cardalbum.txt"
	#define ITEM_GROUP_COOKIEBAG_FILENAME	"item_cookie_bag.txt"
	#define ITEM_GROUP_FINDINGORE_FILENAME	"item_findingore.txt"
	#define ITEM_GROUP_GIFTBOX_FILENAME		"item_giftbox.txt"
	#define ITEM_GROUP_MISC_FILENAME		"item_misc.txt"
	#define ITEM_GROUP_SCROLL_FILENAME		"item_scroll.txt"
	#define ITEM_GROUP_VIOLETBOX_FILENAME	"item_violetbox.txt"

	typedef struct {
		int id;
		char aegis[MAX_NAME_SIZE];
		char eathena[MAX_NAME_SIZE];
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
		int elv;
		int refineable;
		int view;
		char script[MAX_SCRIPT_SIZE];
		char onequip[MAX_SCRIPT_SIZE];
		char onunequip[MAX_SCRIPT_SIZE];
	} item_ea;

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
		int expper;
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
	} mob_ea;

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
		char name[MAX_NAME_SIZE];
		char desc[MAX_NAME_SIZE];
	} skill_ea;

	typedef struct  {
		int item_id;
		int item_lv;
		int skill_id;
		int skill_lv;
		int item_id_req[MAX_INGREDIENT];
		int item_amount_req[MAX_INGREDIENT];
		int ingredient_count;
	} produce_ea;

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
	} mercenary_ea;

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
	} pet_ea;

	typedef struct {
		int group_id;
		int item_id;
		int rate;
	} item_group_ea;

	typedef struct {
		char name[MAX_NAME_SIZE];
		int value;
		int type;
	} const_ea;

	/* eathena native database loading */
	extern native_config_t load_ea_native[EATHENA_DB_COUNT];
	int item_ea_load(void * db, int row, int col, char * val);
	int mob_ea_load(void * db, int row, int col, char * val);
	int skill_ea_load(void * db, int row, int col, char * val);
	int produce_ea_load(void * db, int row, int col, char * val);
	int mercenary_ea_load(void * db, int row, int col, char * val);
	int pet_ea_load(void * db, int row, int col, char * val);
	int item_group_ea_load(void * db, int row, int col, char * val);
	int const_ea_load(void * db, int row, int col, char * val);
	int load_eathena_database(const char * eathena_path);
#endif

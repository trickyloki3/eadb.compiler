/*
 *   file: db_hercules.h
 *   date: 03/01/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef DB_HERCULES_H
#define DB_HERCULES_H
 	#include "load.h"
 	#include "util.h"
 	#include "setting.h"
 	#include "libconfig.h"

 	/* hercules database type definitions 
	 * -----------------------------------------
	 * type name 	- 	type reference
	 * -----------------------------------------
	 * item_he 			- 	hercules item database
	 * mob_he 			- 	hercules mob database 
	 * skill_he 		- 	hercules skill database 
	 * produce_he		- 	hercules produce database
	 * mercenary_he 	-	hercules mecenary database
	 * pet_he			-	hercules pet database
	 * const_he 		- 	hercules constant database
	 */

 	/* database record counts */
 	#define MOB_HE_FIELD_COUNT				57
	#define SKILL_HE_FIELD_COUNT 			17
	#define MERCENARY_HE_FIELD_COUNT		26
	#define PET_HE_FIELD_COUNT				22

	/* constant to index equip array */
	#define EQUIP_MIN					0
	#define EQUIP_MAX					1
	#define EQUIP_TOTAL					2

	/* constant to index trade array */
	#define TRADE_OVERRIDE 				0
	#define TRADE_NODROP				1
	#define TRADE_NOTRADE				2
	#define TRADE_PARTNEROVERRIDE		3
	#define TRADE_NOSELLTONPC			4
	#define TRADE_NOCART				5
	#define TRADE_NOSTORAGE 			6
	#define TRADE_NOGSTORAGE			7
	#define TRADE_NOMAIL				8
	#define TRADE_NOAUCTION				9
	#define TRADE_TOTAL					10

	/* constant to index nouse array */
	#define NOUSE_OVERRIDE				0
	#define NOUSE_SITTING				1
	#define NOUSE_TOTAL					2

	/* constant to index stack array */
	#define STACK_AMOUNT				0
	#define STACK_TYPE					1
	#define STACK_TOTAL					2

	typedef struct {
		int id;
		const char * aegis;
		const char * name;
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
		int weaponlv;
		int equiplv[EQUIP_TOTAL];
		int refine;
		int view;
		int bindonequip;
		int buyingstore;
		int delay;
		int trade[TRADE_TOTAL];
		int nouse[NOUSE_TOTAL];
		int stack[STACK_TOTAL];
		int sprite;
		const char * script;
		const char * onequipscript;
		const char * onunequipscript;
	} item_he;

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
	} mob_he;

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
	} skill_he;

	typedef struct  {
		int item_id;
		int item_lv;
		int skill_id;
		int skill_lv;
		int item_id_req[MAX_INGREDIENT];
		int item_amount_req[MAX_INGREDIENT];
		int ingredient_count;
	} produce_he;

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
	} mercenary_he;

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
	} pet_he;

	typedef struct {
		char name[MAX_NAME_SIZE];
		int value;
		int type;
	} const_he;

	/* hercules shares the same database format as eathena and rathena
	 * but who knows what they'll change completely, better keep separate */
	int load_he_item(const char * file_name, native_t * native);

	/* hercules native database loading */
	extern native_config_t load_he_native[HERCULES_DB_COUNT];
	int mob_he_load(void * db, int row, int col, char * val);
	int skill_he_load(void * db, int row, int col, char * val);
	int produce_he_load(void * db, int row, int col, char * val);
	int mercenary_he_load(void * db, int row, int col, char * val);
	int pet_he_load(void * db, int row, int col, char * val);
	int const_he_load(void * db, int row, int col, char * val);

	int load_hercules_database(const char * hercules_path);
#endif
/*
 *   file: db.h
 *   date: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef DB_H
#define DB_H
	#include "load.h"
	#include "util.h"
	
 	/* general item container */
	typedef struct {
		int id;
		char * name;
		char * script;
	} ic_item_t;

	typedef struct {
		int id;
		char * aegis;
		char * eathena;
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
		char * script;
		char * onequip;
		char * onunequip;
	} ea_item_t; /* OK */

	typedef struct {
		int id;
		char * aegis;
		char * eathena;
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
		char * script;
		char * onequip;
		char * onunequip;
	} ra_item_t; /* OK */

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
	} he_item_t;

	typedef struct {
		int mob_id;
		char * pet_name;
		char * pet_jname;
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
		char * pet_script;
		char * loyal_script;
	} pet_t;

	typedef struct {
		int id;
		char * sprite;
		char * name;
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
	} merc_t;

	typedef struct is_produce_t {
		int item_id;
		int item_lv;
		int req_skill;
		int req_skill_lv;
		int req_cnt;
		int * material;
		int * amount;
		struct is_produce_t * next;
	} ic_produce_t;

	typedef struct {
		int item_id;
		int item_lv;
		int req_skill;
		int req_skill_lv;
		/* loader version */
		int material[32];
		int amount[32];
	} produce_t;

	typedef struct {
		int id;
		int item_id;
		int item_lv;
		int req_skill;
		int req_skill_lv;
		int material[32];
		int amount[32];
	} ra_produce_t;

	typedef struct {
		int id;
		int max;
		char * name;
		char * desc;
	} ic_skill_t;

	typedef struct {
		int id;
		char * range;			/* array of int delimit by : */
		int hit;
		int inf;
		char * element;			/* array of int delimit by : */
		int nk;
		char * splash;			/* array of int delimit by : */
		int max;
		char * hit_amount;		/* array of int delimit by : */
		char * cast_cancel;		/* 'yes' or 'no' string */
		int cast_def_reduce_rate;
		int inf2;
		char * maxcount;		/* array of int delimit by : */
		char * type;			/* 'none', 'weapon', 'magic', 'misc' */
		char * blow_count;		/* array of int delimit by : */
		char * name;
		char * desc;
	} skill_t;

	typedef struct {
		int id;
		char*  range;			/* array of int delimit by : */
		int hit;
		int inf;
		char * element;			/* array of int delimit by : */
		int nk;
		char * splash;			/* array of int delimit by : */
		int max;
		char * hit_amount;		/* array of int delimit by : */
		char * cast_cancel;		/* 'yes' or 'no' string */
		int cast_def_reduce_rate;
		int inf2;
		char * maxcount;		/* array of int delimit by : */
		char * type;			/* 'none', 'weapon', 'magic', 'misc' */
		char * blow_count;		/* array of int delimit by : */
		int inf3;
		char * name;
		char * desc;
	} ra_skill_t;

	typedef struct {
		int id;
		char * iro;
	} ic_mob_t;

	typedef struct {
		int id;
		char * sprite;
		char * kro;
		char * iro;
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
	} ea_mob_t;

	typedef struct {
		int id;
		char * sprite;
		char * kro;
		char * iro;
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
	} mob_t;

	typedef struct {
		/* for loading */
		int bk_id;
		char * bk_kywd;
		int bk_flag;
		/* for using */
		int id;
		char * keyword;
		int flag;
	} block_t;

	/* type flag */
	#define TYPE_FUNC 0x00000001
	#define TYPE_VARI 0x00000002

	/* variable flags */
	#define VARI_DIRECT 0x00000001

	/* function flags */
	#define FUNC_DIRECT     0x00000001  /* retrieve min and max directly */
	#define FUNC_PARAMS     0x00000002  /* retrieve min and max from handler */
	#define FUNC_GETSKILLLV 0x10000000  /* getskilllv */
	#define FUNC_RAND       0x20000000  /* rand */
	#define FUNC_POW        0x40000000  /* pow */

	typedef struct {
      int tag;         /* in-house identification */
      char * id;       /* identifier string */
      int type;        /* type of identifer and flags */
      int vflag;
      int fflag;
      int min;         /* help calculations */
      int max;
      char * str;
   } var_t;

	typedef struct {
		int scid;     /* status id */
		char * scstr; /* status string identifer */
		int type;
		char * scfmt; /* format of status string */
		char * scend; /* format of status end string */
		int vcnt;     /* value count */
		int vmod[4];  /* modifer for value */
		int voff[4];  /* offset translation stack */
		/* compiler version */
		int * vmod_ptr;
		int * voff_ptr;
	} status_t;

	typedef struct {
		int id;
		int flag;
		int attr;        /* bonus attributes */
		char * pref;     /* bonus prefix, i.e. bonus, bonus2, etc. */
		char * buff;     /* bonus buff, i.e. bStr, bAgi, etc. */
		char * desc;     /* bonus format string */
		int * type;      /* bonus argument type */
		int type_cnt;
		int * order;     /* bonus order for each type */
		int order_cnt;
	} bonus_t;

   	typedef struct {
		char * name;
		int value;
		int type;
	} const_t;

	typedef struct {
		int group_id;
		int item_id;
		int rate;
	} ea_item_group_t;

	typedef struct {
		int group_id;
		char * group_name;
		int item_id;
		int rate;
	} ra_item_group_t;

	typedef struct {
		int group_id;
		char * group_name;
		int item_id;
		char * item_name;
		int rate;
	} ra_item_package_t;

	typedef struct ra_item_combo_t {
		int item_id;                      /* use by api */
		char * item_id_list;	
		char * script;
		char * combo;
		struct ra_item_combo_t * next;    /* use by api */
	} ra_item_combo_t;

	load_cb_t * ea_item_load();
	load_cb_t * ra_item_load();
	load_cb_t * pet_load();
	load_cb_t * merc_load();
	load_cb_t * produce_load();
	load_cb_t * ra_produce_load();
	load_cb_t * skill_load();
	load_cb_t * ra_skill_load();
	load_cb_t * mob_load();
	load_cb_t * ea_mob_load();
	load_cb_t * block_load();
	load_cb_t * var_load();
	load_cb_t * status_load();
	load_cb_t * bonus_load();
	load_cb_t * const_load();
	load_cb_t * ea_item_group_load();
	load_cb_t * ra_item_group_load();
	load_cb_t * ra_item_package_load();
	load_cb_t * ra_item_combo_load();
	int ea_load_column(void *, int, int, char *);
	int ra_load_column(void *, int, int, char *);
	int pet_load_column(void *, int, int, char *);
	int merc_load_column(void *, int, int, char *);
	int produce_load_column(void *, int, int, char *);
	int ra_produce_load_column(void *, int, int, char *);
	int skill_load_column(void *, int, int, char *);
	int ra_skill_load_column(void *, int, int, char *);
	int mob_load_column(void *, int, int, char *);
	int ea_mob_load_column(void *, int, int, char *);
	int block_load_column(void *, int, int, char *);
	int var_load_column(void *, int, int, char *);
	int status_load_column(void *, int, int, char *);
	int bonus_load_column(void *, int, int, char *);
	int const_load_column(void *, int, int, char *);
	int ea_item_group_load_column(void *, int, int, char *);
	int ra_item_group_load_column(void *, int, int, char *);
	int ra_item_package_column(void *, int, int, char *);
	int ra_item_combo_colum(void *, int, int, char *);
	int is_row_sentinel(char);
	int is_row_sentinel_comment(char);
	int is_row_sentinel_semicolon(char);
	int is_row_sentinel_whitespace(char);
	int is_row_delimiter(char);
	int is_row_delimiter_comment(char);
	int is_row_delimiter_semicolon(char);
	int is_row_delimiter_whitespace(char);
	void const_dealloc(void *, int);
	void ea_item_dealloc(void *, int);
	void ra_item_dealloc(void *, int);
	void pet_dealloc(void *, int);
	void merc_dealloc(void *, int);
	void blank_dealloc(void *, int);
	void skill_dealloc(void *, int);
	void ra_skill_dealloc(void *, int);
	void ea_mob_dealloc(void *, int);
	void mob_dealloc(void *, int);
	void block_dealloc(void *, int);
	void var_dealloc(void *, int);
	void status_dealloc(void *, int);
	void bonus_dealloc(void *, int);
	void ra_item_group_dealloc(void *, int);
	void ra_item_package_dealloc(void *, int);
	void ra_item_combo_dealloc(void *, int);
#endif

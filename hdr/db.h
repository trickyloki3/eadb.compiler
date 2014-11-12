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

	typedef struct {
		int32_t mob_id;
		char * pet_name;
		char * pet_jname;
		int32_t lure_id;
		int32_t egg_id;
		int32_t equip_id;
		int32_t food_id;
		int32_t fullness;
		int32_t hungry_delay;
		int32_t r_hungry;
		int32_t r_full;
		int32_t intimate;
		int32_t die;
		int32_t capture;
		int32_t speed;
		int32_t s_performance;
		int32_t talk_convert;
		int32_t attack_rate;
		int32_t defence_attack_rate;
		int32_t change_target_rate;
		char * pet_script;
		char * loyal_script;
	} pet_t;

	typedef struct {
		int32_t id;
		char * sprite;
		char * name;
		int32_t lv;
		int32_t hp;
		int32_t sp;
		int32_t range1;
		int32_t atk1;
		int32_t atk2;
		int32_t def;
		int32_t mdef;
		int32_t str;
		int32_t agi;
		int32_t vit;
		int32_t intr;
		int32_t dex;
		int32_t luk;
		int32_t range2;
		int32_t range3;
		int32_t scale;
		int32_t race;
		int32_t element;
		int32_t speed;
		int32_t adelay;
		int32_t amotion;
		int32_t dmotion;
	} merc_t;

	typedef struct {
		int32_t item_id;
		int32_t item_lv;
		int32_t req_skill;
		int32_t req_skill_lv;
		int32_t material[32];
		int32_t amount[32];
	} produce_t;

	typedef struct {
		int32_t id;
		int32_t item_id;
		int32_t item_lv;
		int32_t req_skill;
		int32_t req_skill_lv;
		int32_t material[32];
		int32_t amount[32];
	} ra_produce_t;

	typedef struct {
		int32_t id;
		char * range;			/* array of int32_t delimit by : */
		int32_t hit;
		int32_t inf;
		char * element;		/* array of int32_t delimit by : */
		uint32_t nk;
		char * splash;			/* array of int32_t delimit by : */
		int32_t max;
		char * hit_amount;		/* array of int32_t delimit by : */
		char * cast_cancel;		/* 'yes' or 'no' string */
		int32_t cast_def_reduce_rate;
		uint32_t inf2;
		char * maxcount;		/* array of int32_t delimit by : */
		char * type;			/* 'none', 'weapon', 'magic', 'misc' */
		char * blow_count;		/* array of int32_t delimit by : */
		char * name;
		char * desc;
	} skill_t;

	typedef struct {
		int32_t id;
		char*  range;			/* array of int32_t delimit by : */
		int32_t hit;
		int32_t inf;
		char * element;		/* array of int32_t delimit by : */
		uint32_t nk;
		char * splash;		/* array of int32_t delimit by : */
		int32_t max;
		char * hit_amount;	/* array of int32_t delimit by : */
		char * cast_cancel;	/* 'yes' or 'no' string */
		int32_t cast_def_reduce_rate;
		uint32_t inf2;
		char * maxcount;		/* array of int32_t delimit by : */
		char * type;			/* 'none', 'weapon', 'magic', 'misc' */
		char * blow_count;	/* array of int32_t delimit by : */
		uint32_t inf3;
		char * name;
		char * desc;
	} ra_skill_t;

	typedef struct {
		int32_t id;
		char * sprite;
		char * kro;
		char * iro;
		int32_t lv;
		int32_t hp;
		int32_t sp;
		int32_t exp;
		int32_t jexp;
		int32_t range;
		int32_t atk1;
		int32_t atk2;
		int32_t def;
		int32_t mdef;
		int32_t str;
		int32_t agi;
		int32_t vit;
		int32_t intr;
		int32_t dex;
		int32_t luk;
		int32_t range2;
		int32_t range3;
		int32_t scale;
		int32_t race;
		int32_t element;
		uint32_t mode;
		int32_t speed;
		int32_t adelay;
		int32_t amotion;
		int32_t dmotion;
		int32_t mexp;
		int32_t expper;
		int32_t mvp1id;
		int32_t mvp1per;
		int32_t mvp2id;
		int32_t mvp2per;
		int32_t mvp3id;
		int32_t mvp3per;
		int32_t drop1id;
		int32_t drop1per;
		int32_t drop2id;
		int32_t drop2per;
		int32_t drop3id;
		int32_t drop3per;
		int32_t drop4id;
		int32_t drop4per;
		int32_t drop5id;
		int32_t drop5per;
		int32_t drop6id;
		int32_t drop6per;
		int32_t drop7id;
		int32_t drop7per;
		int32_t drop8id;
		int32_t drop8per;
		int32_t drop9id;
		int32_t drop9per;
		int32_t dropcardid;
		int32_t dropcardper;
	} ea_mob_t;

	typedef struct {
		int32_t id;
		char * sprite;
		char * kro;
		char * iro;
		int32_t lv;
		int32_t hp;
		int32_t sp;
		int32_t exp;
		int32_t jexp;
		int32_t range;
		int32_t atk1;
		int32_t atk2;
		int32_t def;
		int32_t mdef;
		int32_t str;
		int32_t agi;
		int32_t vit;
		int32_t intr;
		int32_t dex;
		int32_t luk;
		int32_t range2;
		int32_t range3;
		int32_t scale;
		int32_t race;
		int32_t element;
		uint32_t mode;
		int32_t speed;
		int32_t adelay;
		int32_t amotion;
		int32_t dmotion;
		int32_t mexp;
		int32_t mvp1id;
		int32_t mvp1per;
		int32_t mvp2id;
		int32_t mvp2per;
		int32_t mvp3id;
		int32_t mvp3per;
		int32_t drop1id;
		int32_t drop1per;
		int32_t drop2id;
		int32_t drop2per;
		int32_t drop3id;
		int32_t drop3per;
		int32_t drop4id;
		int32_t drop4per;
		int32_t drop5id;
		int32_t drop5per;
		int32_t drop6id;
		int32_t drop6per;
		int32_t drop7id;
		int32_t drop7per;
		int32_t drop8id;
		int32_t drop8per;
		int32_t drop9id;
		int32_t drop9per;
		int32_t dropcardid;
		int32_t dropcardper;
	} mob_t;

	typedef struct {
		int32_t bk_id;
		char * bk_kywd;
		int32_t bk_flag;
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
      int32_t tag;         /* in-house identification */
      char * id;           /* identifier string */
      int32_t type;        /* type of identifer and flags */
      int32_t vflag;
      int32_t fflag;
      int32_t min;         /* help calculations */
      int32_t max;
   } var_t;

	typedef struct {
		int32_t scid;     /* status id */
		char * scstr;     /* status string identifer */
		int32_t type;
		char * scfmt;     /* format of status string */
		char * scend;     /* format of status end string */
		int32_t vcnt;     /* value count */
		int32_t vmod[4];  /* modifer for value */
		int32_t voff[4];  /* offset translation stack */
	} status_t;

	typedef struct {
		char * pref;         /* bonus prefix, i.e. bonus, bonus2, etc. */
		char * buff;         /* bonus buff, i.e. bStr, bAgi, etc. */
		int32_t attr;        /* bonus attributes */
		char * desc;         /* bonus format string */
		int32_t * type;      /* bonus argument type */
		int32_t type_cnt;
		int32_t * order;     /* bonus order for each type */
		int32_t order_cnt;
	} bonus_t;

   	typedef struct {
		char * name;
		int32_t value;
		int32_t type;
	} const_t;

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
	int is_row_sentinel(char);
	int is_row_sentinel_comment(char);
	int is_row_sentinel_semicolon(char);
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
#endif

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

    /* database record counts */
    #define ITEM_RA_FIELD_COUNT             22
    #define MOB_RA_FIELD_COUNT              57
    #define SKILL_RA_FIELD_COUNT            18
    #define MERCENARY_RA_FIELD_COUNT        26
    #define PET_RA_FIELD_COUNT              22
    #define ITEM_GROUP_RA_FIELD_COUNT       3
    #define ITEM_COMBO_FIELD_COUNT          2

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

    #define RA_ITEM_SEARCH_NAME     "SELECT id, eathena, script FROM item_ra WHERE eathena = ? OR aegis = ? COLLATE NOCASE;"
    #define RA_ITEM_SEARCH_ID       "SELECT id, eathena, script, type FROM item_ra WHERE id = ? COLLATE NOCASE;"
    #define RA_ITEM_INSERT          "INSERT INTO item_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RA_ITEM_CREATE          "CREATE TABLE IF NOT EXISTS item_ra("\
                                    "   id INTEGER PRIMARY KEY,"\
                                    "   aegis TEXT,"\
                                    "   eathena TEXT,"\
                                    "   type INTEGER,"\
                                    "   buy INTEGER,"\
                                    "   sell INTEGER,"\
                                    "   weight INTEGER,"\
                                    "   matk INTEGER,"\
                                    "   atk INTEGER,"\
                                    "   def INTEGER,"\
                                    "   range INTEGER,"\
                                    "   slots INTEGER,"\
                                    "   job INTEGER,"\
                                    "   upper INTEGER,"\
                                    "   gender INTEGER,"\
                                    "   loc INTEGER, "\
                                    "   wlv INTEGER,"\
                                    "   elv INTEGER,"\
                                    "   elv_max INTEGER,"\
                                    "   refineable INTEGER,"\
                                    "   view INTEGER,"\
                                    "   script TEXT,"\
                                    "   onequip TEXT,"\
                                    "   onunequip TEXT"\
                                    ");"
    #define  RA_ITEM_DELETE         "DROP TABLE IF EXISTS item_ra;"

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

    #define RA_MOB_INSERT           "INSERT INTO mob_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RA_MOB_CREATE           "CREATE TABLE IF NOT EXISTS mob_ra("\
                                    "   id INTEGER PRIMARY KEY,"\
                                    "   sprite TEXT,"\
                                    "   kro TEXT,"\
                                    "   iro TEXT,"\
                                    "   lv INTEGER,"\
                                    "   hp INTEGER,"\
                                    "   sp INTEGER,"\
                                    "   exp INTEGER,"\
                                    "   jexp INTEGER,"\
                                    "   range INTEGER,"\
                                    "   atk1 INTEGER,"\
                                    "   atk2 INTEGER,"\
                                    "   def INTEGER,"\
                                    "   mdef INTEGER,"\
                                    "   str INTEGER,"\
                                    "   agi INTEGER,"\
                                    "   vit INTEGER,"\
                                    "   intr INTEGER,"\
                                    "   dex INTEGER,"\
                                    "   luk INTEGER,"\
                                    "   range2 INTEGER,"\
                                    "   range3 INTEGER,"\
                                    "   scale INTEGER,"\
                                    "   race INTEGER,"\
                                    "   element INTEGER,"\
                                    "   mode INTEGER,"\
                                    "   speed INTEGER,"\
                                    "   adelay INTEGER,"\
                                    "   amotion INTEGER,"\
                                    "   dmotion INTEGER,"\
                                    "   mexp INTEGER,"\
                                    "   mvp1id INTEGER,"\
                                    "   mvp1per INTEGER,"\
                                    "   mvp2id INTEGER,"\
                                    "   mvp2per INTEGER,"\
                                    "   mvp3id INTEGER,"\
                                    "   mvp3per INTEGER,"\
                                    "   drop1id INTEGER,"\
                                    "   drop1per INTEGER,"\
                                    "   drop2id INTEGER,"\
                                    "   drop2per INTEGER,"\
                                    "   drop3id INTEGER,"\
                                    "   drop3per INTEGER,"\
                                    "   drop4id INTEGER,"\
                                    "   drop4per INTEGER,"\
                                    "   drop5id INTEGER,"\
                                    "   drop5per INTEGER,"\
                                    "   drop6id INTEGER,"\
                                    "   drop6per INTEGER,"\
                                    "   drop7id INTEGER,"\
                                    "   drop7per INTEGER,"\
                                    "   drop8id INTEGER,"\
                                    "   drop8per INTEGER,"\
                                    "   drop9id INTEGER,"\
                                    "   drop9per INTEGER,"\
                                    "   dropcardid INTEGER,"\
                                    "   dropcardper INTEGER"\
                                    ");"
    #define RA_MOB_DELETE           "DROP TABLE IF EXISTS mob_ra;"

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
        char cast_cancel[MAX_NAME_SIZE];        /* 'yes' or 'no' string */
        int cast_def_reduce_rate;
        int inf2;
        varlist maxcount;
        char type[MAX_NAME_SIZE];               /* 'none', 'weapon', 'magic', 'misc' */
        varlist blow_count;
        int inf3;
        char name[MAX_NAME_SIZE];
        char desc[MAX_NAME_SIZE];
    } skill_ra;

    #define RA_SKILL_INSERT         "INSERT INTO skill_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RA_SKILL_CREATE         "CREATE TABLE IF NOT EXISTS skill_ra("\
                                    "   id INTEGER PRIMARY KEY,"\
                                    "   range TEXT,"\
                                    "   hit INTEGER,"\
                                    "   inf INTEGER,"\
                                    "   element TEXT,"\
                                    "   nk INTEGER,"\
                                    "   splash TEXT,"\
                                    "   max INTEGER,"\
                                    "   hit_amount INTEGER,"\
                                    "   cast_cancel TEXT,"\
                                    "   cast_def_reduce_rate INTEGER,"\
                                    "   inf2 INTEGER,"\
                                    "   maxcount TEXT,"\
                                    "   type TEXT,"\
                                    "   blow_count TEXT,"\
                                    "   inf3 INTEGER,"\
                                    "   name TEXT,"\
                                    "   desc TEXT"\
                                    ");"
    #define RA_SKILL_DELETE         "DROP TABLE IF EXISTS skill_ra;"

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

    #define RA_PRODUCE_INSERT       "INSERT INTO produce_ra VALUES(?, ?, ?, ?, ?, ?, ?);"
    #define RA_PRODUCE_CREATE       "CREATE TABLE IF NOT EXISTS produce_ra("\
                                    "   id INTEGER PRIMARY KEY,"\
                                    "   item_id INTEGER,"\
                                    "   item_lv INTEGER,"\
                                    "   req_skill INTEGER,"\
                                    "   req_skill_lv INTEGER,"\
                                    "   material TEXT,"\
                                    "   amount TEXT"\
                                    ");"
    #define RA_PRODUCE_DELETE       "DROP TABLE IF EXISTS produce_ra;"

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

    #define RA_MERC_INSERT          "INSERT INTO mercenary_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RA_MERC_CREATE          "CREATE TABLE IF NOT EXISTS mercenary_ra("\
                                    "   id INTEGER PRIMARY KEY,"\
                                    "   sprite TEXT,"\
                                    "   name TEXT,"\
                                    "   lv INTEGER,"\
                                    "   hp INTEGER,"\
                                    "   sp INTEGER,"\
                                    "   range1 INTEGER,"\
                                    "   atk1 INTEGER,"\
                                    "   atk2 INTEGER,"\
                                    "   def INTEGER,"\
                                    "   mdef INTEGER,"\
                                    "   str INTEGER,"\
                                    "   agi INTEGER,"\
                                    "   vit INTEGER,"\
                                    "   intr INTEGER,"\
                                    "   dex INTEGER,"\
                                    "   luk INTEGER,"\
                                    "   range2 INTEGER,"\
                                    "   range3 INTEGER,"\
                                    "   scale INTEGER,"\
                                    "   race INTEGER,"\
                                    "   element INTEGER,"\
                                    "   speed INTEGER,"\
                                    "   adelay INTEGER,"\
                                    "   amotion INTEGER,"\
                                    "   dmotion INTEGER"\
                                    ");"
    #define RA_MERC_DELETE          "DROP TABLE IF EXISTS mercenary_ra;"

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

    #define RA_PET_INSERT           "INSERT INTO pet_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RA_PET_CREATE           "CREATE TABLE IF NOT EXISTS pet_ra("\
                                    "   mob_id INTEGER PRIMARY KEY,"\
                                    "   pet_name TEXT,"\
                                    "   pet_jname TEXT,"\
                                    "   lure_id INTEGER,"\
                                    "   egg_id INTEGER,"\
                                    "   equip_id INTEGER,"\
                                    "   food_id INTEGER,"\
                                    "   fullness INTEGER,"\
                                    "   hungry_delay INTEGER,"\
                                    "   r_hungry INTEGER,"\
                                    "   r_full INTEGER,"\
                                    "   intimate INTEGER,"\
                                    "   die INTEGER,"\
                                    "   capture INTEGER,"\
                                    "   speed INTEGER,"\
                                    "   s_performance INTEGER,"\
                                    "   talk_convert INTEGER,"\
                                    "   attack_rate INTEGER,"\
                                    "   defence_attack_rate INTEGER,"\
                                    "   change_target_rate INTEGER,"\
                                    "   pet_script TEXT,"\
                                    "   loyal_script TEXT"\
                                    ");"
    #define RA_PET_DELETE           "DROP TABLE IF EXISTS pet_ra;"

    typedef struct {
        char name[MAX_NAME_SIZE];
        int value;
        int type;
    } const_ra;

    #define RA_CONST_SEARCH         "SELECT * FROM const_ra WHERE name = ? COLLATE NOCASE;"
    #define RA_CONST_INSERT         "INSERT INTO const_ra VALUES(?, ?, ?);"
    #define RA_CONST_CREATE         "CREATE TABLE IF NOT EXISTS const_ra("\
                                    "   name TEXT,"\
                                    "   value INTEGER,"\
                                    "   type INTEGER"\
                                    ");"
    #define RA_CONST_DELETE         "DROP TABLE IF EXISTS const_ra;"

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

    typedef struct {
        int id;
        char name[MAX_NAME_SIZE];
        int item_id;
        int rate;
    } item_group_ra;

    #define RA_ITEM_GROUP_INSERT    "INSERT INTO item_package_ra VALUES(?, ?, ?, 0, 1, 0, 0, 0, 0, 0);"
    #define RA_ITEM_PACKAGE_INSERT  "INSERT INTO item_package_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RA_ITEM_PACKAGE_CREATE  "CREATE TABLE IF NOT EXISTS item_package_ra("\
                                    "   group_id INTEGER,"\
                                    "   item_id INTEGER,"\
                                    "   rate INTEGER,"\
                                    "   amount INTEGER,"\
                                    "   random INTEGER,"\
                                    "   announced INTEGER,"\
                                    "   duration INTEGER,"\
                                    "   guid INTEGER,"\
                                    "   bound INTEGER,"\
                                    "   named INTEGER"\
                                    ");"
    #define RA_ITEM_PACKAGE_DELETE  "DROP TABLE IF EXISTS item_package_ra;"

    typedef struct combo_ra {
        varlist item_id;
        char script[MAX_SCRIPT_SIZE];
        struct combo_ra * next;
    } combo_ra;

    #define RA_ITEM_COMBO_INSERT    "INSERT INTO item_combo_ra VALUES(?, ?, ?);"
    #define RA_ITEM_COMBO_CREATE    "CREATE TABLE IF NOT EXISTS item_combo_ra("\
                                    "   id INTEGER,"\
                                    "   script TEXT,"\
                                    "   combo_group TEXT"\
                                    ");"
    #define RA_ITEM_COMBO_DELETE    "DROP TABLE IF EXISTS item_combo_ra;"

    typedef struct {
        int group_id;
        int subgroup_id;
        int item;                   /* item count */
        /* item type count */
        int heal;                   /* 0 */
        int usable;                 /* 2 */
        int etc;                    /* 3 */
        int armor;                  /* 4 */
        int weapon;                 /* 5 */
        int card;                   /* 6 */
        int pet;                    /* 7 */
        int pet_equip;              /* 8 */
        int ammo;                   /* 10 */
        int delay_usable;           /* 11 */
        int shadow;                 /* 12 */
        int confirm_usable;         /* 18 */
        /* item group count */
        int bind;                   /* bind count */
        int rent;                   /* rent count */
    } package_meta_ra;

    #define RA_ITEM_PACKAGE_META_CREATE "CREATE TABLE IF NOT EXISTS item_package_meta_ra("\
                                        "group_id INTEGER,"\
                                        "subgroup_id INTEGER,"\
                                        "item INTEGER,"\
                                        "heal INTEGER,"\
                                        "usable INTEGER,"\
                                        "etc INTEGER,"\
                                        "armor INTEGER,"\
                                        "weapon INTEGER,"\
                                        "card INTEGER,"\
                                        "pet INTEGER,"\
                                        "pet_equip INTEGER,"\
                                        "ammo INTEGER,"\
                                        "delay_usable INTEGER,"\
                                        "shadow INTEGER,"\
                                        "confirm_usable INTEGER,"\
                                        "bind INTEGER,"\
                                        "rent INTEGER,"\
                                        "PRIMARY KEY(group_id, subgroup_id));"
    #define RA_ITEM_PACKAGE_META_DELETE "DROP TABLE IF EXISTS item_package_meta_ra;"
    #define RA_ITEM_PACKAGE_META_INSERT "INSERT INTO item_package_meta_ra VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RA_ITEM_PACKAGE_QUERY       "SELECT * FROM item_package_ra ORDER BY group_id, random;"

    typedef struct ra_db_t {
        sqlite3 * db;
        /* insert */
        sqlite3_stmt * item_ra_sql_insert;
        sqlite3_stmt * mob_ra_sql_insert;
        sqlite3_stmt * skill_ra_sql_insert;
        sqlite3_stmt * produce_ra_sql_insert;
        sqlite3_stmt * mercenary_ra_sql_insert;
        sqlite3_stmt * pet_ra_sql_insert;
        sqlite3_stmt * item_group_ra_sql_insert;
        sqlite3_stmt * const_ra_sql_insert;
        sqlite3_stmt * item_combo_ra_sql_insert;
        sqlite3_stmt * item_package_ra_sql_insert;
        /* search */
        sqlite3_stmt * const_ra_name_search;
        sqlite3_stmt * item_ra_name_search;
        sqlite3_stmt * item_ra_id_search;
        /* meta */
        sqlite3_stmt * item_package_meta_insert;
        sqlite3_stmt * item_package_query;
    } ra_db_t;

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

    /* convert csv databases to sqlite3 */
    int ra_db_init(ra_db_t **, const char *);
    int ra_db_deit(ra_db_t **);
    int ra_db_exec(ra_db_t *, char *);
    int ra_db_item_load(ra_db_t *, const char *);
    int ra_db_item_load_record(item_ra *, int, sqlite3_stmt *);
    int ra_db_mob_load(ra_db_t *, const char *);
    int ra_db_mob_load_record(mob_ra *, int, sqlite3_stmt *);
    int ra_db_skill_load(ra_db_t *, const char *);
    int ra_db_skill_load_record(skill_ra *, int, sqlite3_stmt *);
    int ra_db_produce_load(ra_db_t *, const char *);
    int ra_db_produce_load_record(produce_ra *, int, sqlite3_stmt *);
    int ra_db_merc_load(ra_db_t *, const char *);
    int ra_db_merc_load_record(mercenary_ra *, int, sqlite3_stmt *);
    int ra_db_pet_load(ra_db_t *, const char *);
    int ra_db_pet_load_record(pet_ra *, int, sqlite3_stmt *);
    int ra_db_const_load(ra_db_t *, const char *);
    int ra_db_const_load_record(const_ra *, int, sqlite3_stmt *);
    int const_ra_name_search(sqlite3_stmt *, char *, const_ra *);
    int item_ra_name_search(sqlite3_stmt *, char *, item_ra *);
    int item_ra_id_search(sqlite3_stmt *, int, item_ra *);
    int ra_db_item_group_load(ra_db_t *, const char *);
    int ra_db_item_group_load_record(item_group_ra *, int, sqlite3_stmt *, sqlite3_stmt *);
    int ra_db_item_package_load(ra_db_t *, const char *);
    int ra_db_item_group_package_record(ra_db_t *, package_ra *, int);
    int ra_db_item_combo_load(ra_db_t *, const char *);
    int ra_db_item_combo_load_record(ra_db_t *, combo_ra *, int);

    /* metadata tables for item groups */
    int ra_db_item_package_meta(ra_db_t *);
    int ra_db_item_package_meta_insert(sqlite3_stmt *, package_meta_ra *);
#endif

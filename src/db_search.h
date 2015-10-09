/*
 *   file: db_search.h
 *   date: 03/08/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef DB_SEARCH_H
#define DB_SEARCH_H
    #include "util.h"
    #include "setting.h"
    #include "sqlite3.h"

    /* database modes */
    #define EATHENA    0x1
    #define RATHENA    0x2
    #define HERCULE    0x4
    #define DB_MODE    0x7

#if 0
    #include "db_eathena.h"
    #include "db_rathena.h"
    #include "db_resources.h"
    #include "db_hercules.h"
#else
    typedef struct option_res {
        char option[MAX_NAME_SIZE];
        char name[MAX_NAME_SIZE];
        int flag;
    } option_res;

    typedef struct map_res {
        int id;
        char map[MAX_NAME_SIZE];
        char name[MAX_NAME_SIZE];
    } map_res;

    typedef struct {
        int id;
        int flag;
        int attr;
        char prefix[MAX_NAME_SIZE];
        char bonus[MAX_NAME_SIZE];
        char format[MAX_NAME_SIZE];
        int type[MAX_BONUS];
        int type_cnt;
        int order[MAX_BONUS];
        int order_cnt;
    } bonus_res;

    typedef struct {
        int scid;                       /* status id */
        char scstr[MAX_NAME_SIZE];      /* status string identifer */
        int type;
        char scfmt[MAX_FORMAT_SIZE];    /* format of status string */
        char scend[MAX_FORMAT_SIZE];    /* format of status end string */
        int vcnt;                       /* value count */
        int vmod[4];                    /* modifer for value */
        int voff[4];                    /* offset translation stack */
    } status_res;

    typedef struct {
        int id;
        int type;
        int flag;
        int min;
        int max;
        char name[MAX_NAME_SIZE];
        char desc[MAX_NAME_SIZE];
    } var_res;

    #define VARI_FLAG       0x1
    #define FUNC_FLAG       0x2
    #define FUNC_CONST_FLAG 0x4
    #define ITEM_TYPE_FLAG  0x1

   typedef struct block_res {
        int id;
        char name[MAX_NAME_SIZE];
        int flag;
    } block_res;

    typedef struct nid_res {
        int id;
        char res[MAX_RESNAME_SIZE];
    } nid_res;
#endif
    typedef struct const_t {
        char name[MAX_NAME_SIZE];
        int value;
        int type;
    } const_t;

    typedef struct skill_t {
        int id;
        int maxlv;
        char name[MAX_NAME_SIZE];
        char desc[MAX_NAME_SIZE];
    } skill_t;

    /* constant to index equip array */
    #define EQUIP_MIN                   0
    #define EQUIP_MAX                   1
    #define EQUIP_TOTAL                 2

    /* constant to index trade array */
    #define TRADE_OVERRIDE              0
    #define TRADE_NODROP                1
    #define TRADE_NOTRADE               2
    #define TRADE_PARTNEROVERRIDE       3
    #define TRADE_NOSELLTONPC           4
    #define TRADE_NOCART                5
    #define TRADE_NOSTORAGE             6
    #define TRADE_NOGSTORAGE            7
    #define TRADE_NOMAIL                8
    #define TRADE_NOAUCTION             9
    #define TRADE_TOTAL                 10

    /* constant to index nouse array */
    #define NOUSE_OVERRIDE              0
    #define NOUSE_SITTING               1
    #define NOUSE_TOTAL                 2

    /* constant to index stack array */
    #define STACK_AMOUNT                0
    #define STACK_TYPE                  1
    #define STACK_TOTAL                 2

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
        int trade[10];
        int stack[2];
    } item_t;

    typedef struct mob_t {
        int id;
        char name[MAX_NAME_SIZE];
    } mob_t;

    typedef struct merc_t {
        int id;
        char name[MAX_NAME_SIZE];
    } merc_t;

    typedef struct pet_t {
        int mob_id;
        char name[MAX_NAME_SIZE];
        char pet_script[MAX_SCRIPT_SIZE];
        char loyal_script[MAX_SCRIPT_SIZE];
    } pet_t;

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

    typedef struct item_group_t {
        int group_id;
        int * item_id;
        int * rate;
        int size;
    } item_group_t;

    typedef struct combo_t {
        char script[MAX_SCRIPT_SIZE];
        char group[MAX_SCRIPT_SIZE];
        struct combo_t * next;
    } combo_t;

    typedef struct item_group_meta_t {
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
    } item_group_meta_t;

    /* resource database queries */
    #define RE_OPT_NAME_SEARCH              "SELECT * FROM option_res WHERE option = ? COLLATE NOCASE;"
    #define RE_MAP_NAME_SEARCH              "SELECT * FROM map_res WHERE map = ?;"
    #define RE_MAP_ID_SEARCH                "SELECT * FROM map_res WHERE id = ?;"
    #define RE_BONUS_NAME_SEARCH            "SELECT * FROM bonus_res WHERE pref = ? AND buff = ? COLLATE NOCASE;"
    #define RE_STATUS_NAME_SEARCH           "SELECT * FROM status_res WHERE scid = ? COLLATE NOCASE;"
    #define RE_VAR_NAME_SEARCH              "SELECT * FROM var_res WHERE name = ? COLLATE NOCASE;"
    #define RE_BLOCK_NAME_SEARCH            "SELECT * FROM block_res WHERE key = ? COLLATE NOCASE;"
    #define RE_SPR_ID_SEARCH                "SELECT * FROM id_res WHERE id = ?;"
    #define RE_NSPR_ID_SEARCH               "SELECT * FROM numid_res WHERE id = ?;"

    /* server item database queries */
    #define EA_ITEM_ITERATE                 "SELECT id, eathena, type, buy, sell, weight, atk, def, range, slots, job, upper, gender, loc, wlv, elv, refineable, view, script from item_ea;"
    #define RA_ITEM_ITERATE                 "SELECT id, eathena, type, buy, sell, weight, matk, atk, def, range, slots, job, upper, gender, loc, wlv, elv, refineable, view, script from item_ra;"
    #define HE_ITEM_ITERATE                 "SELECT Id, Name, Type, Buy, Sell, Weight, Matk, Atk, Def, Range, Slots, Job, Upper, Gender, Loc, WeaponLv, EquipLvMin, Refine, View, Script,"\
                                            "BindOnEquip, BuyingStore, Delay, override, nodrop, notrade, partneroverride, noselltonpc, nocart, nostorage, nogstorage, nomail, noauction,"\
                                            "StackAmount, StackType from item_he;"

    /* server database queries */
    #define EA_CONST_NAME_SEARCH            "SELECT * FROM const_ea WHERE name = ? COLLATE NOCASE;"
    #define RA_CONST_NAME_SEARCH            "SELECT * FROM const_ra WHERE name = ? COLLATE NOCASE;"
    #define HE_CONST_NAME_SEARCH            "SELECT * FROM const_he WHERE name = ? COLLATE NOCASE;"
    #define EA_CONST_ID_SEARCH              "SELECT * FROM const_ea WHERE value = ?;"
    #define RA_CONST_ID_SEARCH              "SELECT * FROM const_ra WHERE value = ?;"
    #define HE_CONST_ID_SEARCH              "SELECT * FROM const_he WHERE value = ?;"
    #define EA_SKILL_NAME_SEARCH            "SELECT id, max, name, desc FROM skill_ea WHERE name = ? COLLATE NOCASE;"
    #define RA_SKILL_NAME_SEARCH            "SELECT id, max, name, desc FROM skill_ra WHERE name = ? COLLATE NOCASE;"
    #define HE_SKILL_NAME_SEARCH            "SELECT id, max, name, desc FROM skill_he WHERE name = ? COLLATE NOCASE;"
    #define EA_SKILL_ID_SEARCH              "SELECT id, max, name, desc FROM skill_ea WHERE id = ?;"
    #define RA_SKILL_ID_SEARCH              "SELECT id, max, name, desc FROM skill_ra WHERE id = ?;"
    #define HE_SKILL_ID_SEARCH              "SELECT id, max, name, desc FROM skill_he WHERE id = ?;"
    #define EA_ITEM_NAME_SEARCH             "SELECT id, eathena, script FROM item_ea WHERE eathena = ? OR aegis = ? COLLATE NOCASE;"
    #define RA_ITEM_NAME_SEARCH             "SELECT id, eathena, script FROM item_ra WHERE eathena = ? OR aegis = ? COLLATE NOCASE;"
    #define HE_ITEM_NAME_SEARCH             "SELECT Id, Name, Script FROM item_he WHERE Name = ? OR AegisName = ? COLLATE NOCASE;"
    #define EA_ITEM_ID_SEARCH               "SELECT id, eathena, script FROM item_ea WHERE id = ?;"
    #define RA_ITEM_ID_SEARCH               "SELECT id, eathena, script FROM item_ra WHERE id = ?;"
    #define HE_ITEM_ID_SEARCH               "SELECT Id, Name, Script FROM item_he WHERE id = ?;"
    #define EA_MOB_ID_SEARCH                "SELECT id, iro FROM mob_ea WHERE id = ?;"
    #define RA_MOB_ID_SEARCH                "SELECT id, iro FROM mob_ra WHERE id = ?;"
    #define HE_MOB_ID_SEARCH                "SELECT id, iro FROM mob_he WHERE id = ?;"
    #define EA_MERC_ID_SEARCH               "SELECT id, name FROM mercenary_ea WHERE id = ?;"
    #define RA_MERC_ID_SEARCH               "SELECT id, name FROM mercenary_ra WHERE id = ?;"
    #define HE_MERC_ID_SEARCH               "SELECT id, name FROM mercenary_he WHERE id = ?;"
    #define EA_PET_ID_SEARCH                "SELECT mob_id, pet_jname, pet_script, loyal_script FROM pet_ea WHERE mob_id = ?;"
    #define RA_PET_ID_SEARCH                "SELECT mob_id, pet_jname, pet_script, loyal_script FROM pet_ra WHERE mob_id = ?;"
    #define HE_PET_ID_SEARCH                "SELECT mob_id, pet_jname, pet_script, loyal_script FROM pet_he WHERE mob_id = ?;"
    #define EA_PRODUCE_ID_SEARCH            "SELECT item_id, item_lv, req_skill, req_skill_lv, material, amount FROM produce_ea WHERE item_lv = ?;"
    #define RA_PRODUCE_ID_SEARCH            "SELECT item_id, item_lv, req_skill, req_skill_lv, material, amount FROM produce_ra WHERE item_lv = ?;"
    #define HE_PRODUCE_ID_SEARCH            "SELECT item_id, item_lv, req_skill, req_skill_lv, material, amount FROM produce_he WHERE item_lv = ?;"
    #define RA_ITEM_COMBO_ID_SEARCH         "SELECT script, combo_group FROM item_combo_ra WHERE id = ?;"
    #define HE_ITEM_COMBO_ID_SEARCH         "SELECT script, combo_group FROM item_combo_he WHERE id = ?;"
    #define EA_ITEM_GROUP_NAME_SEARCH       "SELECT * FROM const_ea WHERE value = ? and name like 'IG_%'"
    #define RA_ITEM_GROUP_NAME_SEARCH       "SELECT * FROM const_ra WHERE value = ? and name like 'IG_%'"
    #define EA_ITEM_GROUP_ID_SEARCH         "SELECT * FROM item_group_ea WHERE group_id = ?;"
    #define RA_ITEM_GROUP_ID_SEARCH         "SELECT * FROM item_package_meta_ra WHERE group_id = ? AND subgroup_id = ?;"
    #define EA_ITEM_GROUP_ID_RECORD         "SELECT * FROM item_group_record_ea WHERE group_id = ? ORDER BY item_id;"
    #define RA_ITEM_GROUP_ID_RECORD         "SELECT * FROM item_package_ra WHERE group_id = ? and random = ? ORDER BY item_id;"

    typedef struct sql_t {
        char * query;
        sqlite3_stmt * stmt;
    } sql_t;

    typedef struct db_t {
        int server_type;
        sqlite3 * re;
        sqlite3 * db;
        /* resource database search */
        sql_t * opt_name;
        sql_t * map_name;
        sql_t * map_id;
        sql_t * bonus_name;
        sql_t * status_name;
        sql_t * var_name;
        sql_t * block_name;
        sql_t * sprite_id;
        sql_t * usprite_id;
        /* server database search */
        sql_t * const_name;
        sql_t * const_id;
        sql_t * skill_name;
        sql_t * skill_id;
        sql_t * item_name;
        sql_t * item_id;
        sql_t * mob_id;
        sql_t * merc_id;
        sql_t * pet_id;
        sql_t * produce_id;
        sql_t * item_group_name;
        sql_t * item_group_id;          /* query item group metadata */
        sql_t * item_group_record;      /* query item group records */
        sql_t * item_combo;
        /* server item database iterate */
        sql_t * item_db;
    } db_t;

    /* load resource and server sqlite3 databases */
    int init_db_load(db_t **, const char *, const char *, int);
    int deit_db_load(db_t **);

    /* load sqlite3 databases */
    int init_db(sqlite3 **, const char *);
    int deit_db(sqlite3 **);

    /* prepare sqlite3 statements */
    int init_db_prep_sql(sqlite3 *, sql_t **, const char *);
    int deit_db_prep_sql(sqlite3 *, sql_t **);

    /* reset, bind, and execute sqlite3 statements */
    #define BIND_STRING 0x1
    #define BIND_NUMBER 0x2
    int exec_db_query(sqlite3 *, sql_t *, ...);
    int bind_db_query(sqlite3 *, sqlite3_stmt *, va_list);
    int bind_db_string(sqlite3 *, sqlite3_stmt *, int, va_list);
    int bind_db_number(sqlite3 *, sqlite3_stmt *, int, va_list);

    /* resource sqlite3 database queries */
    int opt_name(db_t *, option_res *, const char *, int);
    int map_name(db_t *, map_res *, const char *, int);
    int map_id(db_t *, map_res *, int);
    int bonus_name(db_t *, bonus_res *, const char *, int, const char *, int);
    int status_id(db_t *, status_res *, int);
    int var_name(db_t *, var_res *, const char *, int);
    int block_name(db_t *, block_res *, const char *, int);
    int sprite_id(db_t *, nid_res *, int);
    int usprite_id(db_t *, nid_res *, int);

    /* server item database */
    int item_iterate(db_t *, item_t *);
    int item_eathena(sqlite3_stmt *, item_t *);
    int item_rathena(sqlite3_stmt *, item_t *);
    int item_hercule(sqlite3_stmt *, item_t *);

    /* server databse search functions */
    int const_name(db_t *, const_t *, const char *, int);
    int const_id(db_t *, const_t *, int);
    int skill_name(db_t *, skill_t *, const char *, int);
    int skill_id(db_t *, skill_t *, int);
    int item_name(db_t *, item_t *, const char *, int);
    int item_id(db_t *, item_t *, int);
    int mob_id(db_t *, mob_t *, int);
    int merc_id(db_t *, merc_t *, int);
    int pet_id(db_t *, pet_t *, int);
    int produce_id(db_t *, produce_t **, int);
    int produce_free(produce_t **);
    int item_group_name(db_t *, const_t *, int);
    int item_group_id(db_t *, item_group_t *, int, int);
    int item_group_free(item_group_t *);
    int item_group_id_ea(db_t *, item_group_t *, int);
    int item_group_id_ra(db_t *, item_group_meta_t *, int, int);
    int item_combo_id(db_t *, combo_t **, int);
    int item_combo_free(combo_t **);
#endif

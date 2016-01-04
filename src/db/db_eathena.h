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
     #include "sqlite3.h"

    /* database record counts */
    #define ITEM_EA_FIELD_COUNT              22
    #define MOB_EA_FIELD_COUNT               58
    #define SKILL_EA_FIELD_COUNT             17
    #define MERCENARY_EA_FIELD_COUNT         26
    #define PET_EA_FIELD_COUNT               22
    #define ITEM_GROUP_EA_FIELD_COUNT        3

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

    #define EA_ITEM_DELETE          "DROP TABLE IF EXISTS item_ea;"
    #define EA_ITEM_INSERT          "INSERT INTO item_ea VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define EA_ITEM_CREATE          "CREATE TABLE IF NOT EXISTS item_ea("\
                                    "    id INTEGER PRIMARY KEY,"\
                                    "    aegis TEXT,"\
                                    "    eathena TEXT,"\
                                    "    type INTEGER,"\
                                    "    buy INTEGER,"\
                                    "    sell INTEGER,"\
                                    "    weight INTEGER,"\
                                    "    atk INTEGER,"\
                                    "    def INTEGER,"\
                                    "    range INTEGER,"\
                                    "    slots INTEGER,"\
                                    "    job INTEGER,"\
                                    "    upper INTEGER,"\
                                    "    gender INTEGER,"\
                                    "    loc INTEGER,"\
                                    "    wlv INTEGER,"\
                                    "    elv INTEGER,"\
                                    "    refineable INTEGER,"\
                                    "    view INTEGER,"\
                                    "    script TEXT,"\
                                    "    onequip TEXT,"\
                                    "    onunequip TEXT"\
                                    ");"

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

    #define EA_MOB_DELETE           "DROP TABLE IF EXISTS mob_ea;"
    #define EA_MOB_INSERT           "INSERT INTO mob_ea VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define EA_MOB_CREATE           "CREATE TABLE IF NOT EXISTS mob_ea("\
                                    "    id INTEGER PRIMARY KEY,"\
                                    "    sprite TEXT,"\
                                    "    kro TEXT,"\
                                    "    iro TEXT,"\
                                    "    lv INTEGER,"\
                                    "    hp INTEGER,"\
                                    "    sp INTEGER,"\
                                    "    exp INTEGER,"\
                                    "    jexp INTEGER,"\
                                    "    range INTEGER,"\
                                    "    atk1 INTEGER,"\
                                    "    atk2 INTEGER,"\
                                    "    def INTEGER,"\
                                    "    mdef INTEGER,"\
                                    "    str INTEGER,"\
                                    "    agi INTEGER,"\
                                    "    vit INTEGER,"\
                                    "    intr INTEGER,"\
                                    "    dex INTEGER,"\
                                    "    luk INTEGER,"\
                                    "    range2 INTEGER,"\
                                    "    range3 INTEGER,"\
                                    "    scale INTEGER,"\
                                    "    race INTEGER,"\
                                    "    element INTEGER,"\
                                    "    mode INTEGER,"\
                                    "    speed INTEGER,"\
                                    "    adelay INTEGER,"\
                                    "    amotion INTEGER,"\
                                    "    dmotion INTEGER,"\
                                    "    mexp INTEGER,"\
                                    "    expper INTEGER,"\
                                    "    mvp1id INTEGER,"\
                                    "    mvp1per INTEGER,"\
                                    "    mvp2id INTEGER,"\
                                    "    mvp2per INTEGER,"\
                                    "    mvp3id INTEGER,"\
                                    "    mvp3per INTEGER,"\
                                    "    drop1id INTEGER,"\
                                    "    drop1per INTEGER,"\
                                    "    drop2id INTEGER,"\
                                    "    drop2per INTEGER,"\
                                    "    drop3id INTEGER,"\
                                    "    drop3per INTEGER,"\
                                    "    drop4id INTEGER,"\
                                    "    drop4per INTEGER,"\
                                    "    drop5id INTEGER,"\
                                    "    drop5per INTEGER,"\
                                    "    drop6id INTEGER,"\
                                    "    drop6per INTEGER,"\
                                    "    drop7id INTEGER,"\
                                    "    drop7per INTEGER,"\
                                    "    drop8id INTEGER,"\
                                    "    drop8per INTEGER,"\
                                    "    drop9id INTEGER,"\
                                    "    drop9per INTEGER,"\
                                    "    dropcardid INTEGER,"\
                                    "    dropcardper INTEGER"\
                                    ");"

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
        char type[MAX_NAME_SIZE];                /* 'none',"
        "'weapon', 'magic', 'misc' */
        varlist blow_count;
        char name[MAX_NAME_SIZE];
        char desc[MAX_NAME_SIZE];
    } skill_ea;

    #define EA_SKILL_DELETE         "DROP TABLE IF EXISTS skill_ea;"
    #define EA_SKILL_INSERT         "INSERT INTO skill_ea VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define EA_SKILL_CREATE         "CREATE TABLE IF NOT EXISTS skill_ea("\
                                    "    id INTEGER PRIMARY KEY,"\
                                    "    range TEXT,"\
                                    "    hit INTEGER,"\
                                    "    inf INTEGER,"\
                                    "    element TEXT,"\
                                    "    nk INTEGER,"\
                                    "    splash TEXT,"\
                                    "    max INTEGER,"\
                                    "    hit_amount INTEGER,"\
                                    "    cast_cancel TEXT,"\
                                    "    cast_def_reduce_rate INTEGER,"\
                                    "    inf2 INTEGER,"\
                                    "    maxcount TEXT,"\
                                    "    type TEXT,"\
                                    "    blow_count TEXT,"\
                                    "    name TEXT,"\
                                    "    desc TEXT"\
                                    ");"

    typedef struct  {
        int item_id;
        int item_lv;
        int skill_id;
        int skill_lv;
        int item_id_req[MAX_INGREDIENT];
        int item_amount_req[MAX_INGREDIENT];
        int ingredient_count;
    } produce_ea;

    #define EA_PRODUCE_DELETE       "DROP TABLE IF EXISTS produce_ea;"
    #define EA_PRODUCE_INSERT       "INSERT INTO produce_ea VALUES(?, ?, ?, ?, ?, ?);"
    #define EA_PRODUCE_CREATE       "CREATE TABLE IF NOT EXISTS produce_ea("\
                                    "    item_id INTEGER PRIMARY KEY,"\
                                    "    item_lv INTEGER,"\
                                    "    req_skill INTEGER,"\
                                    "    req_skill_lv INTEGER,"\
                                    "    material TEXT,"\
                                    "    amount TEXT"\
                                    ");"

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

    #define EA_MERC_DELETE          "DROP TABLE IF EXISTS mercenary_ea;"
    #define EA_MERC_INSERT          "INSERT INTO mercenary_ea VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define EA_MERC_CREATE          "CREATE TABLE IF NOT EXISTS mercenary_ea("\
                                    "    id INTEGER PRIMARY KEY,"\
                                    "    sprite TEXT,"\
                                    "    name TEXT,"\
                                    "    lv INTEGER,"\
                                    "    hp INTEGER,"\
                                    "    sp INTEGER,"\
                                    "    range1 INTEGER,"\
                                    "    atk1 INTEGER,"\
                                    "    atk2 INTEGER,"\
                                    "    def INTEGER,"\
                                    "    mdef INTEGER,"\
                                    "    str INTEGER,"\
                                    "    agi INTEGER,"\
                                    "    vit INTEGER,"\
                                    "    intr INTEGER,"\
                                    "    dex INTEGER,"\
                                    "    luk INTEGER,"\
                                    "    range2 INTEGER,"\
                                    "    range3 INTEGER,"\
                                    "    scale INTEGER,"\
                                    "    race INTEGER,"\
                                    "    element INTEGER,"\
                                    "    speed INTEGER,"\
                                    "    adelay INTEGER,"\
                                    "    amotion INTEGER,"\
                                    "    dmotion INTEGER"\
                                    ");"

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

    #define EA_PET_DELETE           "DROP TABLE IF EXISTS pet_ea;"
    #define EA_PET_INSERT           "INSERT INTO pet_ea VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define EA_PET_CREATE           "CREATE TABLE IF NOT EXISTS pet_ea("\
                                    "    mob_id INTEGER PRIMARY KEY,"\
                                    "    pet_name TEXT,"\
                                    "    pet_jname TEXT,"\
                                    "    lure_id INTEGER,"\
                                    "    egg_id INTEGER,"\
                                    "    equip_id INTEGER,"\
                                    "    food_id INTEGER,"\
                                    "    fullness INTEGER,"\
                                    "    hungry_delay INTEGER,"\
                                    "    r_hungry INTEGER,"\
                                    "    r_full INTEGER,"\
                                    "    intimate INTEGER,"\
                                    "    die INTEGER,"\
                                    "    capture INTEGER,"\
                                    "    speed INTEGER,"\
                                    "    s_performance INTEGER,"\
                                    "    talk_convert INTEGER,"\
                                    "    attack_rate INTEGER,"\
                                    "    defence_attack_rate INTEGER,"\
                                    "    change_target_rate INTEGER,"\
                                    "    pet_script TEXT,"\
                                    "    loyal_script TEXT"\
                                    ");"

    typedef struct {
        int group_id;
        int item_id;
        int rate;
    } item_group_ea;

    #define EA_ITEM_GROUP_DELETE        "DROP TABLE IF EXISTS item_group_ea;"
    #define EA_ITEM_GROUP_INSERT        "INSERT INTO item_group_ea VALUES(?, ?);"
    #define EA_ITEM_GROUP_CREATE        "CREATE TABLE IF NOT EXISTS item_group_ea("\
                                        "    group_id INTEGER,"\
                                        "    size INTEGER,"\
                                        "    PRIMARY KEY(group_id)"\
                                        ");"
    #define EA_ITEM_GROUP_ENTRY_DELETE  "DROP TABLE IF EXISTS item_group_record_ea;"
    #define EA_ITEM_GROUP_ENTRY_INSERT  "INSERT INTO item_group_record_ea VALUES(?, ?, ?);"
    #define EA_ITEM_GROUP_ENTRY_CREATE  "CREATE TABLE IF NOT EXISTS item_group_record_ea("\
                                        "    group_id INTEGER,"\
                                        "    item_id INTEGER,"\
                                        "    rate INTEGER"\
                                        ");"

    void swap_item_group(void *, size_t, size_t);
    int less_item_group(void *, size_t, size_t);
    int same_item_group(void *, size_t, size_t);
    void read_item_group(void *, size_t);
    extern swap_t qsort_item_group;

    typedef struct {
        char name[MAX_NAME_SIZE];
        int value;
        int type;
    } const_ea;

    #define EA_CONST_DELETE            "DROP TABLE IF EXISTS const_ea;"
    #define EA_CONST_INSERT            "INSERT INTO const_ea VALUES(?, ?, ?);"
    #define EA_CONST_CREATE            "CREATE TABLE IF NOT EXISTS const_ea(name TEXT, value INTEGER, type INTEGER);"

    typedef struct ea_db_t {
        sqlite3 * db;
        sqlite3_stmt * item_ea_sql_insert;
        sqlite3_stmt * mob_ea_sql_insert;
        sqlite3_stmt * skill_ea_sql_insert;
        sqlite3_stmt * produce_ea_sql_insert;
        sqlite3_stmt * mercenary_ea_sql_insert;
        sqlite3_stmt * pet_ea_sql_insert;
        sqlite3_stmt * item_group_ea_sql_insert;
        sqlite3_stmt * item_group_ea_sql_record_insert;
        sqlite3_stmt * const_ea_sql_insert;
    } ea_db_t;

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

    /* convert csv databases to sqlite3 */
    int ea_db_init(ea_db_t **, const char *);
    int ea_db_deit(ea_db_t **);
    int ea_db_exec(ea_db_t *, char *);
    int ea_db_item_load(ea_db_t *, const char *);
    int ea_db_item_load_record(item_ea *, int, sqlite3_stmt *);
    int ea_db_mob_load(ea_db_t *, const char *);
    int ea_db_mob_load_record(mob_ea *, int, sqlite3_stmt *);
    int ea_db_skill_load(ea_db_t *, const char *);
    int ea_db_skill_load_record(skill_ea *, int, sqlite3_stmt *);
    int ea_db_produce_load(ea_db_t *, const char *);
    int ea_db_produce_load_record(produce_ea *, int, sqlite3_stmt *);
    int ea_db_merc_load(ea_db_t *, const char *);
    int ea_db_merc_load_record(mercenary_ea *, int, sqlite3_stmt *);
    int ea_db_pet_load(ea_db_t *, const char *);
    int ea_db_pet_load_record(pet_ea *, int, sqlite3_stmt *);
    int ea_db_item_group_load(ea_db_t *, const char *);
    int ea_db_item_group_load_record(item_group_ea *, int, ea_db_t *);
    int ea_db_item_group_load_records(item_group_ea *, int, int, sqlite3_stmt *);
    int ea_db_const_load(ea_db_t *, const char *);
    int ea_db_const_load_record(const_ea *, int, sqlite3_stmt *);
#endif

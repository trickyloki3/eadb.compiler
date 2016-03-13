#ifndef DB_RESOURCES_H
#define DB_RESOURCES_H
     #include "load.h"
     #include "util.h"
     #include "setting.h"
     #include "sqlite3.h"

    /* database record counts */
    #define OPTION_RES_FIELD_COUNT  3
    #define MAP_RES_FIELD_COUNT     2
    #define VAR_RES_FIELD_COUNT     7
    #define BLOCK_RES_FIELD_COUNT   3
    #define NID_RES_FIELD_COUNT     2

    /* option table maps option constants to a user-friendly string */
    typedef struct option_res {
        char option[MAX_NAME_SIZE];
        char name[MAX_NAME_SIZE];
        int flag;
    } option_res;

    #define RES_OPT_DELETE          "DROP TABLE IF EXISTS option_res;"
    #define RES_OPT_INSERT          "INSERT INTO option_res VALUES(?, ?, ?);"
    #define RES_OPT_CREATE          "CREATE TABLE IF NOT EXISTS option_res("\
                                    "    option TEXT,"\
                                    "    name TEXT,"\
                                    "    flag INTEGER"\
                                    ");"


    /* map table maps the filename of the map to the name of the map */
    typedef struct map_res {
        int id;
        char map[MAX_NAME_SIZE];
        char name[MAX_NAME_SIZE];
    } map_res;

    #define RES_MAP_DELETE          "DROP TABLE IF EXISTS map_res;"
    #define RES_MAP_INSERT          "INSERT INTO map_res VALUES(?, ?, ?);"
    #define RES_MAP_CREATE          "CREATE TABLE IF NOT EXISTS map_res("\
                                    "    id INTEGER,"\
                                    "    map TEXT,"\
                                    "    name TEXT"\
                                    ");"

    /* bonus table consist of a set of template strings and argument types for translating bonuses. */
    typedef struct {
        int id;
        int attr;
        char prefix[MAX_NAME_SIZE];
        char bonus[MAX_NAME_SIZE];
        char format[MAX_NAME_SIZE];
        int type[MAX_BONUS];
        int type_cnt;
        int order[MAX_BONUS];
        int order_cnt;
    } bonus_res;

    #define RES_BNS_DELETE          "DROP TABLE IF EXISTS bonus_res;"
    #define RES_BNS_INSERT          "INSERT INTO bonus_res VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RES_BNS_CREATE          "CREATE TABLE IF NOT EXISTS bonus_res("\
                                    "    id INTEGER,"\
                                    "    attr INTEGER,"\
                                    "    pref TEXT,"\
                                    "    buff TEXT,"\
                                    "    desc TEXT,"\
                                    "    type_param TEXT,"\
                                    "    type_cnt INTEGER,"\
                                    "    order_param TEXT,"\
                                    "    order_cnt INTEGER"\
                                    ");"

    /* status table consist of a set of template strings and argument types for translating statuses */
    typedef struct {
        int id;
        char name[MAX_NAME_SIZE];
        int val1;
        int val2;
        int val3;
        int val4;
        char format[MAX_FORMAT_SIZE];
        int offset_count;
        int offset[MAX_VARARG_COUNT];
    } status_res;

    #define RES_STA_DELETE          "DROP TABLE IF EXISTS status_res;"
    #define RES_STA_INSERT          "INSERT INTO status_res VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);"
    #define RES_STA_CREATE          "CREATE TABLE IF NOT EXISTS status_res("\
                                    "id INTEGER, "\
                                    "name TEXT, "\
                                    "val1 INTEGER, "\
                                    "val2 INTEGER, "\
                                    "val3 INTEGER, "\
                                    "val4 INTEGER, "\
                                    "format TEXT, "\
                                    "offset_count INTEGER, "\
                                    "offset TEXT"\
                                    ");"

    /* variable table contain specific information on the script engine's special variables */
    typedef struct {
        int id;
        int type;
        int flag;
        int min;
        int max;
        char name[MAX_NAME_SIZE];
        char desc[MAX_NAME_SIZE];
    } var_res;

    #define RES_VAR_DELETE          "DROP TABLE IF EXISTS var_res;"
    #define RES_VAR_INSERT          "INSERT INTO var_res VALUES(?, ?, ?, ?, ?, ?, ?);"
    #define RES_VAR_CREATE          "CREATE TABLE IF NOT EXISTS var_res("\
                                    "id INTEGER,"\
                                    "type INTEGER, "\
                                    "flag INTEGER, "\
                                    "min INTEGER, "\
                                    "max INTEGER, "\
                                    "name TEXT, "\
                                    "desc TEXT"\
                                    ");"

   /* block table contains a list of keywords */
   typedef struct block_res {
        int id;
        char name[MAX_NAME_SIZE];
        int flag;
    } block_res;

    #define RES_BLK_DELETE          "DROP TABLE IF EXISTS block_res;"
    #define RES_BLK_INSERT          "INSERT INTO block_res VALUES(?, ?, ?);"
    #define RES_BLK_CREATE          "CREATE TABLE IF NOT EXISTS block_res("\
                                    "    id INTEGER,"\
                                    "    key TEXT,"\
                                    "    flag INTEGER"\
                                    ");"

    /* sprite table contains item id mapped to a resource filename */
    typedef struct nid_res {
        int id;
        char res[MAX_RESNAME_SIZE];
    } nid_res;

    #define RES_RID_DELETE          "DROP TABLE IF EXISTS id_res;"
    #define RES_RID_INSERT          "INSERT INTO id_res VALUES(?, ?);"
    #define RES_RID_CREATE          "CREATE TABLE IF NOT EXISTS id_res("\
                                    "    id INTEGER,"\
                                    "    res TEXT"\
                                    ");"

    #define RES_NID_DELETE          "DROP TABLE IF EXISTS numid_res;"
    #define RES_NID_CREATE          "CREATE TABLE IF NOT EXISTS numid_res("\
                                    "    id INTEGER,"\
                                    "    res TEXT"\
                                    ");"
    #define RES_NID_INSERT          "INSERT INTO numid_res VALUES(?, ?);"

    typedef struct opt_db_t {
        sqlite3 * db;
        sqlite3_stmt * option_sql_insert;
        sqlite3_stmt * map_sql_insert;
        sqlite3_stmt * bonus_sql_insert;
        sqlite3_stmt * status_sql_insert;
        sqlite3_stmt * var_sql_insert;
        sqlite3_stmt * block_sql_insert;
        sqlite3_stmt * id_res_sql_insert;
        sqlite3_stmt * numid_res_sql_insert;
    } opt_db_t;

    /* resource database loading */
    extern native_config_t load_res_native[RESOURCE_DB_COUNT];
    int option_res_load(void *, int, int, char *);
    int map_res_load(void *, int, int, char *);
    int bonus_res_load(void *, int, int, char *);
    int status_res_load(void *, int, int, char *);
    int var_res_load(void *, int, int, char *);
    int block_res_load(void *, int, int, char *);
    int nid_res_load(void *, int, int, char *);

    int opt_db_init(opt_db_t **, const char *);
    int opt_db_deit(opt_db_t **);
    int opt_db_exec(opt_db_t *, char *);
    int opt_db_res_load(opt_db_t *, const char *);
    int opt_db_res_load_record(option_res *, int, sqlite3_stmt *);
    int opt_db_map_load(opt_db_t *, const char *);
    int opt_db_map_load_record(map_res *, int, sqlite3_stmt *);
    int opt_db_bns_load(opt_db_t *, const char *);
    int opt_db_bns_load_record(bonus_res *, int, sqlite3_stmt *);
    int opt_db_sta_load(opt_db_t *, const char *);
    int opt_db_sta_load_record(status_res *, int, sqlite3_stmt *);
    int opt_db_var_load(opt_db_t *, const char *);
    int opt_db_var_load_record(var_res *, int, sqlite3_stmt *);
    int opt_db_blk_load(opt_db_t *, const char *);
    int opt_db_blk_load_record(block_res *, int, sqlite3_stmt *);
    int opt_db_rid_load(opt_db_t *, const char *);
    int opt_db_rid_load_record(nid_res *, int, sqlite3_stmt *);
    int opt_db_nid_load(opt_db_t *, const char *);
    int opt_db_nid_load_record(nid_res *, int, sqlite3_stmt *);
#endif

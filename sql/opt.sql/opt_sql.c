#include "windows.h"
#include "Shlwapi.h"
#include <db_resources.h>

#define OPT_DB_NAME             "opt.db"
#define OPTION_DB_PATH          "option_db.txt"
#define MAP_DB_PATH             "mapnametable.txt"
#define BONUS_DB_PATH           "item_bonus.txt"
#define STATUS_DB_PATH          "status_db.txt"
#define VARIABLE_DB_PATH        "var_db.txt"
#define BLOCK_DB_PATH           "block_db.txt"
#define RID_DB_PATH             "idnum2itemresnametable.txt"
#define NID_DB_PATH             "num2itemresnametable.txt"

int main(int argc, char * argv[]) {
    LPSTR out = NULL;
    LPSTR path = NULL;
    CHAR db_path[MAX_PATH];
    opt_db_t * db = NULL;

    if (1 >= argc || getopts(argc, argv) || !(path = opt['p' % 26])) {
        fprintf(stderr, "ra.sql.exe -p <rathena path> -o <output path>\n");
        exit(EXIT_FAILURE);
    }

    out = opt['o' % 26];
    if (NULL != out) {
        if (!SetCurrentDirectory(out)) {
            fprintf(stderr, "ra.sql: invalid output path '%s'.\n", out);
            exit(EXIT_FAILURE);
        }
    }

    if( opt_db_init(&db, OPT_DB_NAME) ||
        NULL == PathCombine(db_path, path, OPTION_DB_PATH) ||
        opt_db_res_load(db, db_path) ||
        NULL == PathCombine(db_path, path, MAP_DB_PATH) ||
        opt_db_map_load(db, db_path) ||
        NULL == PathCombine(db_path, path, BONUS_DB_PATH) ||
        opt_db_bns_load(db, db_path) ||
        NULL == PathCombine(db_path, path, STATUS_DB_PATH) ||
        opt_db_sta_load(db, db_path) ||
        NULL == PathCombine(db_path, path, VARIABLE_DB_PATH) ||
        opt_db_var_load(db, db_path) ||
        NULL == PathCombine(db_path, path, BLOCK_DB_PATH) ||
        opt_db_blk_load(db, db_path) ||
        NULL == PathCombine(db_path, path, RID_DB_PATH) ||
        opt_db_rid_load(db, db_path) ||
        NULL == PathCombine(db_path, path, NID_DB_PATH) ||
        opt_db_nid_load(db, db_path)) {
        opt_db_deit(&db);
        exit(EXIT_FAILURE);
    }
    opt_db_deit(&db);
	exit(EXIT_SUCCESS);
}
#include "windows.h"
#include "Shlwapi.h"
#include <db_eathena.h>

#define DIR_SEP                 "\\"
#define EA_DB_NAME              "ea.db"
#define ITEM_DB_PATH            "db"DIR_SEP"item_db.txt"
#define MOB_DB_PATH             "db"DIR_SEP"mob_db.txt"
#define SKILL_DB_PATH           "db"DIR_SEP"skill_db.txt"
#define PRODUCE_DB_PATH         "db"DIR_SEP"produce_db.txt"
#define MERCENARY_DB_PATH       "db"DIR_SEP"mercenary_db.txt"
#define PET_DB_PATH             "db"DIR_SEP"pet_db.txt"
#define CONST_PATH              "db"DIR_SEP"const.txt"
#define ITEM_GROUP_BLUEBOX      "db"DIR_SEP"item_bluebox.txt"
#define ITEM_GROUP_VIOLETBOX    "db"DIR_SEP"item_violetbox.txt"
#define ITEM_GROUP_CARDALBUM    "db"DIR_SEP"item_cardalbum.txt"
#define ITEM_GROUP_COOKIEBAG    "db"DIR_SEP"item_cookie_bag.txt"
#define ITEM_GROUP_FINDINGORE   "db"DIR_SEP"item_findingore.txt"
#define ITEM_GROUP_GIFTBOX      "db"DIR_SEP"item_giftbox.txt"
#define ITEM_GROUP_SCROLL       "db"DIR_SEP"item_scroll.txt"
#define ITEM_GROUP_MISC         "db"DIR_SEP"item_misc.txt"

int main(int argc, char * argv[]) {
    DWORD dwPathSize = 0;
    LPSTR lpPathCwd = NULL;
    LPSTR lpPathBase = NULL;
    LPSTR lpPath = NULL;
    ea_db_t * lpAthena = NULL;

    /* parse the command line arguments */
    if (1 >= argc || getopts(argc, argv)) {
        fprintf(stderr, "ra.sql.exe -p <rathena-path> -o <output-path>\n");
        exit(EXIT_FAILURE);
    }

    /* set the base directory and calloc the database path buffer */
    lpPathBase = getopt('p');
    if(NULL == lpPathBase) {
        fprintf(stderr, "ra.sql.exe -p <rathena-path> -o <output-path>\n");
        exit(EXIT_FAILURE);
    }


    /* set the working directory to write the output database */
    lpPathCwd = getopt('o');
    if(NULL != lpPathCwd) {
        if(!SetCurrentDirectory(lpPathCwd)) {
            fprintf(stderr, "[fail]: failed to set working directory to '%s'.\n", lpPathCwd);
            exit(EXIT_FAILURE);
        }
        lpPathCwd = convert_string(lpPathCwd);
    } else {
        dwPathSize = GetCurrentDirectory(0, NULL);
        lpPathCwd = calloc(dwPathSize, sizeof(TCHAR));
        if(NULL != lpPathCwd)
            GetCurrentDirectory(dwPathSize, lpPathCwd);
    }

    if(NULL != lpPathCwd) {
        fprintf(stdout, "[info]: set working directory to '%s'.\n", lpPathCwd);
    } else {
        fprintf(stdout, "[info]: failed to set working directory.\n");
        goto failed;
    }

    lpPath = calloc(MAX_PATH, sizeof(TCHAR));
    if(NULL == lpPath) {
        fprintf(stderr, "[fail]: failed to create the database path bufffer.\n");
        goto failed;
    }

    /* create the eathena database */
    if(ea_db_init(&lpAthena, EA_DB_NAME)) {
        fprintf(stdout, "[info]: failed to create '%s'.\n", EA_DB_NAME);
        return CHECK_PASSED;
    }

    /* load each eathena table */
    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_DB_PATH))
        ea_db_item_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, MOB_DB_PATH))
        ea_db_mob_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, SKILL_DB_PATH))
        ea_db_skill_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, PRODUCE_DB_PATH))
        ea_db_produce_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, MERCENARY_DB_PATH))
        ea_db_merc_load(lpAthena, lpPath);

    if (NULL != PathCombine(lpPath, lpPathBase, PET_DB_PATH))
        ea_db_pet_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_BLUEBOX))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_VIOLETBOX))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_CARDALBUM))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_COOKIEBAG))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_FINDINGORE))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_GIFTBOX))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_SCROLL))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, ITEM_GROUP_MISC))
        ea_db_item_group_load(lpAthena, lpPath);

    if(NULL != PathCombine(lpPath, lpPathBase, CONST_PATH))
        ea_db_const_load(lpAthena, lpPath);

    ea_db_deit(&lpAthena);

    SAFE_FREE(lpPath);
    SAFE_FREE(lpPathCwd);
    exit(EXIT_SUCCESS);

failed:
    SAFE_FREE(lpPath);
    SAFE_FREE(lpPathCwd);
    return CHECK_FAILED;
}

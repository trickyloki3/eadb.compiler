#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "db_eathena.h"
#include "db_rathena.h"
#ifdef WITH_HERCULES
#include "db_hercules.h"
#endif
#include "db_resources.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define PATH_MAX MAX_PATH
#endif

int main(int argc, char * argv[]) {
    int i = 0;
    int len = 0;
    char * pgm = NULL;

    /* output and server path */
    int output_len = 0;
    int server_len = 0;
    char * output_path = NULL;
    char * server_path = NULL;
    char * server_mode = NULL;

    /* database */
    ra_db_t * rathena = NULL;
    ea_db_t * eathena = NULL;
#ifdef WITH_HERCULES
    herc_db_t * hercule = NULL;
#endif
    opt_db_t * resource = NULL;
    /* parse the command line arguments */
    if(parse_argv(argv, argc))
        exit(EXIT_FAILURE);

    /* check path and output are specified */
    if(NULL == get_argv('p') ||
       NULL == get_argv('o') ||
       NULL == get_argv('m')) {
        len = (int) strlen(argv[0]);
        for(i = len - 2; i >= 0; i--)
            if(argv[0][i] == '/' ||
               argv[0][i] == '\\') {
                pgm = &argv[0][i + 1];
                break;
            }

        fprintf(stderr,
            "usage: %s  -p    <server-path>  -o      <output-path> -m <mode>\n"
            "usage: %s --path <server-path> --output <output-path> -m <mode>\n"
            "usage: valid modes are [eathena | rathena | hercules | resource]\n"
            "usage: (windows) %s -p C:\\git\\server -o C:\\db\n"
            "usage: (linux  ) %s -p /usr/git/server -o /usr/db\n"
            "usage: (osx    ) %s -p /usr/git/server -o /usr/db\n",
            pgm, pgm, pgm, pgm, pgm);
        exit(EXIT_FAILURE);
    }

    /* resolve relative to absolute path */
    output_path = calloc(PATH_MAX, sizeof(char));
    if(NULL == output_path)
        exit(EXIT_FAILURE);

    server_path = calloc(PATH_MAX, sizeof(char));
    if(NULL == server_path)
        exit(EXIT_FAILURE);

    server_mode = get_argv('m');

#if defined(_WIN32) || defined(_WIN64)
    if (0 == GetFullPathName(get_argv('p'), PATH_MAX, server_path, NULL)) {
#else
    if (NULL == realpath(get_argv('p'), server_path)) {
#endif
        fprintf(stderr, "failed to resolve server path %s", get_argv('p'));
        goto clean;
    }

#if defined(_WIN32) || defined(_WIN64)
    if (0 == GetFullPathName(get_argv('o'), PATH_MAX, output_path, NULL)) {
#else
    if(NULL == realpath(get_argv('o'), output_path)) {
#endif
        fprintf(stderr, "failed to resolve output path %s", get_argv('o'));
        goto clean;
    }

    output_len = (int) strlen(output_path);
    server_len = (int) strlen(server_path);

#ifdef WITH_HERCULES
    /* load the server's databases */
    if(0 == ncs_strcmp(server_mode, "hercules")) {
        if( path_concat(output_path, output_len, PATH_MAX, "hercule.db") ||
            herc_db_init(&hercule, output_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/re/item_db.conf") ||
            herc_load_item_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/re/mob_db.txt") ||
            herc_load_mob_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/re/skill_db.txt") ||
            herc_load_skill_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/produce_db.txt") ||
            herc_load_produce_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/mercenary_db.txt") ||
            herc_load_merc_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/pet_db.txt") ||
            herc_load_pet_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/const.txt") ||
            herc_load_const_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/re/item_combo_db.txt") ||
            herc_load_combo_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/re/item_group.conf") ||
            herc_load_item_group_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/re/item_chain.conf") ||
            herc_load_item_chain_db(hercule, server_path) ||
            path_concat(server_path, server_len, PATH_MAX, "db/re/item_packages.conf") ||
            herc_load_item_package_db(hercule, server_path)) {
            fprintf(stderr, "failed to load hercules databases.\n");
            goto clean;
        }
    } else
#endif
    if(0 == ncs_strcmp(server_mode, "rathena")) {
        if(path_concat(output_path, output_len, PATH_MAX, "rathena.db") ||
           ra_db_init(&rathena, output_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_db.txt") ||
           ra_db_item_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/mob_db.txt") ||
           ra_db_mob_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/skill_db.txt") ||
           ra_db_skill_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/produce_db.txt") ||
           ra_db_produce_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/mercenary_db.txt") ||
           ra_db_merc_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/pet_db.txt") ||
           ra_db_pet_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/const.txt") ||
           ra_db_const_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_bluebox.txt") ||
           ra_db_item_group_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_cardalbum.txt") ||
           ra_db_item_group_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_findingore.txt") ||
           ra_db_item_group_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_giftbox.txt") ||
           ra_db_item_group_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_misc.txt") ||
           ra_db_item_group_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_violetbox.txt") ||
           ra_db_item_group_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_package.txt") ||
           ra_db_item_package_load(rathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/re/item_combo_db.txt") ||
           ra_db_item_combo_load(rathena, server_path) ||
           ra_db_item_package_meta(rathena)) {
            fprintf(stderr, "failed to load rathena databases.\n");
            goto clean;
        }
    } else if(0 == ncs_strcmp(server_mode, "eathena")) {
        if(path_concat(output_path, output_len, PATH_MAX, "eathena.db") ||
           ea_db_init(&eathena, output_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_db.txt") ||
           ea_db_item_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/mob_db.txt") ||
           ea_db_mob_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/skill_db.txt") ||
           ea_db_skill_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/produce_db.txt") ||
           ea_db_produce_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/mercenary_db.txt") ||
           ea_db_merc_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/pet_db.txt") ||
           ea_db_pet_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/const.txt") ||
           ea_db_const_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_bluebox.txt") ||
           ea_db_item_group_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_violetbox.txt") ||
           ea_db_item_group_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_cardalbum.txt") ||
           ea_db_item_group_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_cookie_bag.txt") ||
           ea_db_item_group_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_findingore.txt") ||
           ea_db_item_group_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_giftbox.txt") ||
           ea_db_item_group_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_scroll.txt") ||
           ea_db_item_group_load(eathena, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "db/item_misc.txt") ||
           ea_db_item_group_load(eathena, server_path)) {
            fprintf(stderr, "failed to load eathena databases.\n");
            goto clean;
        }
    } else if(0 == ncs_strcmp(server_mode, "resource")) {
        if(path_concat(output_path, output_len, PATH_MAX, "resource.db") ||
           opt_db_init(&resource, output_path) ||
           path_concat(server_path, server_len, PATH_MAX, "option_db.txt") ||
           opt_db_res_load(resource, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "bonus_db.txt") ||
           opt_db_bns_load(resource, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "status_db.txt") ||
           opt_db_sta_load(resource, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "var_db.txt") ||
           opt_db_var_load(resource, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "block_db.txt") ||
           opt_db_blk_load(resource, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "client/mapnametable.txt") ||
           opt_db_map_load(resource, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "client/idnum2itemresnametable.txt") ||
           opt_db_rid_load(resource, server_path) ||
           path_concat(server_path, server_len, PATH_MAX, "client/num2itemresnametable.txt") ||
           opt_db_nid_load(resource, server_path)) {
            fprintf(stderr, "failed to load resource databases.\n");
            goto clean;
        }
    } else {
        fprintf(stderr, "'%s' is an invalid mode.\n", server_mode);
    }

clean:
    if(eathena) ea_db_deit(&eathena);
    if(rathena) ra_db_deit(&rathena);
#ifdef WITH_HERCULES
    if(hercule) herc_db_deit(&hercule);
#endif
    if(resource) opt_db_deit(&resource);
    SAFE_FREE(output_path);
    SAFE_FREE(server_path);
    return 0;
}

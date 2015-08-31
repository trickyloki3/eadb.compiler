#include "db_search.h"
#include "db_eathena.h"
#include "db_rathena.h"
#include "db_hercules.h"
#define EA_PATH "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\ea.db"
#define RA_PATH "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\ra.db"
#define HE_PATH "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\herc.db"
#define RE_PATH "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\opt.db"



int test_produce_id_search_ea(db_t * db, produce_ea * produces, int size) {
    int i = 0;
    int bytes = 0;
    produce_ea * produce = NULL;
    produce_t * result;

    for (i = 0; i < size; i++) {
        produce = &produces[i];
        if (produce_id(db, &result, produce->item_lv) ||
            produce->item_lv != result[0].item_lv) {
            fprintf(stderr, "%-*s\r", bytes, "");
            produce_free(&result);
            return CHECK_FAILED;
        }
        
        bytes = fprintf(stderr, "[test]: testing produce_id %d / %d ...\r", i, size);
        produce_free(&result);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_produce_id(db_t * db, const char * path, int server_type) {
    native_t produces;
    produces.db = NULL;
    produces.size = 0;
    switch (server_type) {
        case EATHENA:
            if (load_native(path, trim_numeric, load_native_general, &produces, &load_ea_native[3]) ||
                NULL == produces.db ||
                0 >= produces.size) {
                fprintf(stderr, "[load]: failed to load produce db; %s.\n", path);
                SAFE_FREE(produces.db);
                return CHECK_FAILED;
            }
            test_produce_id_search_ea(db, produces.db, produces.size) ?
                fprintf(stderr, "[test]: failed produce_id test.\n") :
                fprintf(stderr, "[test]: passed produce_id test.\n");
            break;
    }
    
    SAFE_FREE(produces.db);
    return CHECK_PASSED;
}

int test_pet_id_search_ea(db_t * db, pet_ea * pets, int size) {
    int i = 0;
    int bytes = 0;
    pet_ea * pet = NULL;
    pet_t result;

    for (i = 0; i < size; i++) {
        pet = &pets[i];
        if (pet_id(db, &result, pet->mob_id) ||
            pet->mob_id != result.mob_id) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing pet_t %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_pet_id(db_t * db, const char * path, int server_type) {
    native_t pets;
    pets.db = NULL;
    pets.size = 0;
    switch (server_type) {
        case EATHENA:
            if (load_native(path, trim_numeric, load_native_general, &pets, &load_ea_native[5]) ||
                NULL == pets.db ||
                0 >= pets.size) {
                fprintf(stderr, "[load]: failed to load pet db; %s.\n", path);
                SAFE_FREE(pets.db);
                return CHECK_FAILED;
            }
            test_pet_id_search_ea(db, pets.db, pets.size) ?
                fprintf(stderr, "[test]: failed pet_id test.\n") :
                fprintf(stderr, "[test]: passed pet_id test.\n");
            break;
    }
    
    SAFE_FREE(pets.db);
    return CHECK_PASSED;
}

int test_merc_id_search_ea(db_t * db, mercenary_ea * mercs, int size) {
    int i = 0;
    int bytes = 0;
    mercenary_ea * merc = NULL;
    merc_t result;

    for (i = 0; i < size; i++) {
        merc = &mercs[i];
        if (merc_id(db, &result, merc->id) ||
            merc->id != result.id) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing merc_t %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_merc_id(db_t * db, const char * path, int server_type) {
    native_t mercs;
    mercs.db = NULL;
    mercs.size = 0;
    switch (server_type) {
        case EATHENA:
            if (load_native(path, trim_numeric, load_native_general, &mercs, &load_ea_native[4]) ||
                NULL == mercs.db ||
                0 >= mercs.size) {
                fprintf(stderr, "[load]: failed to load mercenary db; %s.\n", path);
                SAFE_FREE(mercs.db);
                return CHECK_FAILED;
            }
            test_merc_id_search_ea(db, mercs.db, mercs.size) ?
                fprintf(stderr, "[test]: failed merc_id test.\n") :
                fprintf(stderr, "[test]: passed merc_id test.\n");
            break;
    }
    
    SAFE_FREE(mercs.db);
    return CHECK_PASSED;
}

int test_mob_id_search_ea(db_t * db, mob_ea * mobs, int size) {
    int i = 0;
    int bytes = 0;
    mob_ea * mob = NULL;
    mob_t result;

    for (i = 0; i < size; i++) {
        mob = &mobs[i];
        if (mob_id(db, &result, mob->id) ||
            mob->id != result.id) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing mob_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_mob_id(db_t * db, const char * path, int server_type) {
    native_t mobs;
    mobs.db = NULL;
    mobs.size = 0;
    switch (server_type) {
        case EATHENA:
            if (load_native(path, trim_numeric, load_native_general, &mobs, &load_ea_native[1]) ||
                NULL == mobs.db ||
                0 >= mobs.size) {
                fprintf(stderr, "[load]: failed to load mob db; %s.\n", path);
                SAFE_FREE(mobs.db);
                return CHECK_FAILED;
            }

            test_mob_id_search_ea(db, mobs.db, mobs.size) ?
                fprintf(stderr, "[test]: failed mob_id test.\n") :
                fprintf(stderr, "[test]: passed mob_id test.\n");

            break;
    }

    SAFE_FREE(mobs.db);
    return CHECK_PASSED;
}

int test_item_id_search_ea(db_t * db, item_ea * items, int size) {
    int i = 0;
    int bytes = 0;
    item_ea * item = NULL;
    item_t result;

    for (i = 0; i < size; i++) {
        item = &items[i];
        if (item_id(db, &result, item->id) ||
            item->id !=  result.id) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing item_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_item_name_search_ea(db_t * db, item_ea * items, int size) {
    int i = 0;
    int bytes = 0;
    item_ea * item = NULL;
    item_t result;

    for (i = 0; i < size; i++) {
        item = &items[i];
        if (item_name(db, &result, item->aegis, strlen(item->aegis)) ||
            0 != strcmp(item->eathena, result.name)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing item_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_item_name(db_t * db, const char * path, int server_type) {
    native_t items;
    items.db = NULL;
    items.size = 0;
    switch (server_type) {
        case EATHENA:
            if (load_native(path, trim_numeric, load_native_general, &items, &load_ea_native[0]) ||
                NULL == items.db ||
                0 >= items.size) {
                fprintf(stderr, "[load]: failed to load item db; %s.\n", path);
                SAFE_FREE(items.db);
                return CHECK_FAILED;
            }
            test_item_name_search_ea(db, items.db, items.size) ?
                fprintf(stderr, "[test]: failed item_name test.\n") :
                fprintf(stderr, "[test]: passed item_name test.\n");
            test_item_id_search_ea(db, items.db, items.size) ?
                fprintf(stderr, "[test]: failed item_id test.\n") :
                fprintf(stderr, "[test]: passed item_id test.\n");
            break;
    }
    
    SAFE_FREE(items.db);
    return CHECK_PASSED;
}

int test_skill_id_search_ea(db_t * db, skill_ea * skills, int size) {
    int i = 0;
    int bytes = 0;
    skill_ea * skill = NULL;
    skill_t result;

    for (i = 0; i < size; i++) {
        skill = &skills[i];
        if (skill_id(db, &result, skill->id) ||
            skill->id != result.id) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing skill_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_skill_name_search_ea(db_t * db, skill_ea * skills, int size) {
    int i = 0;
    int bytes = 0;
    skill_ea * skill = NULL;
    skill_t result;

    for (i = 0; i < size; i++) {
        skill = &skills[i];
        if (skill_name(db, &result, skill->name, strlen(skill->name)) ||
            0 != strcmp(skill->name, result.name)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing skill_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_skill_name(db_t * db, const char * path, int server_type) {
    native_t skills;
    skills.db = NULL;
    skills.size = 0;

    switch (server_type) {
        case EATHENA:
            if (load_native(path, trim_numeric, load_native_general, &skills, &load_ea_native[2]) ||
                NULL == skills.db ||
                0 >= skills.size) {
                fprintf(stderr, "[load]: failed to load skill db; %s.\n", path);
                SAFE_FREE(skills.db);
                return CHECK_FAILED;
            }
            test_skill_name_search_ea(db, skills.db, skills.size) ?
                fprintf(stderr, "[test]: failed skill_name test.\n") :
                fprintf(stderr, "[test]: passed skill_name test.\n");
            test_skill_id_search_ea(db, skills.db, skills.size) ?
                fprintf(stderr, "[test]: failed skill_id test.\n") :
                fprintf(stderr, "[test]: passed skill_id test.\n");
            break;
    }
    
    SAFE_FREE(skills.db);
    return CHECK_PASSED;
}

int test_const_id_search_he(db_t * db, const_he * constants, int size) {
    int i = 0;
    int bytes = 0;
    const_he * constant = NULL;
    const_t result;

    for (i = 0; i < size; i++) {
        constant = &constants[i];
        if (const_id(db, &result, constant->value) ||
            constant->value != result.value) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing const_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_const_id_search_ra(db_t * db, const_ra * constants, int size) {
    int i = 0;
    int bytes = 0;
    const_ra * constant = NULL;
    const_t result;

    for (i = 0; i < size; i++) {
        constant = &constants[i];
        if (const_id(db, &result, constant->value) ||
            constant->value != result.value) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing const_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_const_id_search_ea(db_t * db, const_ea * constants, int size) {
    int i = 0;
    int bytes = 0;
    const_ea * constant = NULL;
    const_t result;

    for (i = 0; i < size; i++) {
        constant = &constants[i];
        if (const_id(db, &result, constant->value) ||
            constant->value != result.value) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing const_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_const_name_search_he(db_t * db, const_he * constants, int size) {
    int i = 0;
    int bytes = 0;
    const_he * constant = NULL;
    const_t result;

    for (i = 0; i < size; i++) {
        constant = &constants[i];
        if (const_name(db, &result, constant->name, strlen(constant->name)) ||
            0 != strcmp(constant->name, result.name)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing const_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_const_name_search_ra(db_t * db, const_ra * constants, int size) {
    int i = 0;
    int bytes = 0;
    const_ra * constant = NULL;
    const_t result;

    for (i = 0; i < size; i++) {
        constant = &constants[i];
        if (const_name(db, &result, constant->name, strlen(constant->name)) ||
            0 != strcmp(constant->name, result.name)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing const_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_const_name_search_ea(db_t * db, const_ea * constants, int size) {
    int i = 0;
    int bytes = 0;
    const_ea * constant = NULL;
    const_t result;

    for (i = 0; i < size; i++) {
        constant = &constants[i];
        if (const_name(db, &result, constant->name, strlen(constant->name)) ||
            0 != strcmp(constant->name, result.name)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing const_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_const_name(db_t * db, const char * path, int server_type) {
    native_t constants;
    constants.db = NULL;
    constants.size = 0;

    switch (server_type) {
        case EATHENA:
            if (load_native(path, trim_general, load_native_general, &constants, &load_ea_native[7]) ||
                NULL == constants.db ||
                0 >= constants.size) {
                fprintf(stderr, "[load]: failed to load constant db; %s.\n", path);
                SAFE_FREE(constants.db);
                return CHECK_FAILED;
            }
            test_const_name_search_ea(db, constants.db, constants.size) ?
                fprintf(stderr, "[test]: failed const_name test.\n") :
                fprintf(stderr, "[test]: passed const_name test.\n");
            test_const_id_search_ea(db, constants.db, constants.size) ?
                fprintf(stderr, "[test]: failed const_id test.\n") :
                fprintf(stderr, "[test]: passed const_id test.\n");
            break;
        case RATHENA:
            if (load_native(path, trim_general, load_native_general, &constants, &load_ra_native[7]) ||
                NULL == constants.db ||
                0 >= constants.size) {
                fprintf(stderr, "[load]: failed to load constant db; %s.\n", path);
                SAFE_FREE(constants.db);
                return CHECK_FAILED;
            }
            test_const_name_search_ra(db, constants.db, constants.size) ?
                fprintf(stderr, "[test]: failed const_name test.\n") :
                fprintf(stderr, "[test]: passed const_name test.\n");
            test_const_id_search_ra(db, constants.db, constants.size) ?
                fprintf(stderr, "[test]: failed const_id test.\n") :
                fprintf(stderr, "[test]: passed const_id test.\n");
            break;
        case HERCULE:
            if (load_native(path, trim_general, load_native_general, &constants, &load_he_native[5]) ||
                NULL == constants.db ||
                0 >= constants.size) {
                fprintf(stderr, "[test]: failed to load constants db; %s.\n", path);
                return CHECK_FAILED;
            }
            test_const_name_search_he(db, constants.db, constants.size) ?
                fprintf(stderr, "[test]: failed const_name test.\n") :
                fprintf(stderr, "[test]: passed const_name test.\n");
            test_const_id_search_he(db, constants.db, constants.size) ?
                fprintf(stderr, "[test]: failed const_id test.\n") :
                fprintf(stderr, "[test]: passed const_id test.\n");
            break;
    }
    
    SAFE_FREE(constants.db);
    return CHECK_PASSED;
}

int test_usprite_id_search(db_t * db, nid_res * sprites, int size) {
    int i = 0;
    int bytes = 0;
    nid_res * sprite = NULL;
    nid_res result;
    for (i = 0; i < size; i++) {
        sprite = &sprites[i];
        if (usprite_id(db, &result, sprite->id) ||
            0 != strcmp(sprite->res, result.res)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            fprintf(stderr, "[test]: failed to search item id %d ...\n", sprite->id);
        }

        bytes = fprintf(stderr, "[test]: testing usprite_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_usprite_test(db_t * db, const char * path) {
    native_t nids;
    nids.db = NULL;
    nids.size = 0;

    if (load_native(path, trim_numeric, load_native_general, &nids, &load_res_native[7]) ||
        NULL == nids.db ||
        0 >= nids.size) {
        fprintf(stderr, "[load]: failed to load nid db; %s.\n", path);
        SAFE_FREE(nids.db);
        return CHECK_FAILED;
    }

    test_usprite_id_search(db, nids.db, nids.size) ?
        fprintf(stderr, "[test]: failed sprite_id test.\n") :
        fprintf(stderr, "[test]: passed sprite_id test.\n");

    SAFE_FREE(nids.db);
    return CHECK_PASSED;
}

int test_sprite_id_search(db_t * db, nid_res * sprites, int size) {
    int i = 0;
    int bytes = 0;
    nid_res * sprite = NULL;
    nid_res result;
    for (i = 0; i < size; i++) {
        sprite = &sprites[i];
        if (sprite_id(db, &result, sprite->id) ||
            0 != strcmp(sprite->res, result.res)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            fprintf(stderr, "[test]: failed to search item id %d ...\n", sprite->id);
        }

        bytes = fprintf(stderr, "[test]: testing sprite_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_sprite_id(db_t * db, const char * path) {
    native_t rids;
   rids.db = NULL;
   rids.size = 0;

   if(load_native(path, trim_numeric, load_native_general, &rids, &load_res_native[6]) ||
      NULL == rids.db ||
      0 >= rids.size) {
      fprintf(stderr, "[load]: failed to load rid db; %s.\n", path);
      SAFE_FREE(rids.db);
      return CHECK_FAILED;
   }
   
   test_sprite_id_search(db, rids.db, rids.size) ?
        fprintf(stderr, "[test]: failed sprite_id test.\n") :
        fprintf(stderr, "[test]: passed sprite_id test.\n");

   SAFE_FREE(rids.db);
   return CHECK_PASSED;
}

int test_block_name_search(db_t * db, block_res * blocks, int size) {
    int i = 0;
    int bytes = 0;
    block_res * block = NULL;
    block_res result;
    for (i = 0; i < size; i++) {
        block = &blocks[i];
        if (block_name(db, &result, block->name, strlen(block->name)) ||
            0 != strcmp(block->name, result.name)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }

        bytes = fprintf(stderr, "[test]: testing block_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_block_name(db_t * db, const char * path) {
    native_t blocks;
    blocks.db = NULL;
    blocks.size = 0;

    if (load_native(path, trim_numeric, load_native_general, &blocks, &load_res_native[5]) ||
        NULL == blocks.db ||
        0 >= blocks.size) {
        fprintf(stderr, "[load]: failed to load block db; %s.\n", path);
        SAFE_FREE(blocks.db);
        return CHECK_FAILED;
    }
   
    test_block_name_search(db, blocks.db, blocks.size) ?
        fprintf(stderr, "[test]: failed block_name test.\n") :
        fprintf(stderr, "[test]: passed block_name test.\n");

    SAFE_FREE(blocks.db);
    return CHECK_PASSED;
}

int test_var_name_search(db_t * db, var_res * vars, int size) {
    int i = 0;
    int bytes = 0;
    var_res * var = NULL;
    var_res result;
    for (i = 0; i < size; i++) {
        var = &vars[i];
        if (var_name(db, &result, var->id, strlen(var->id)) ||
            0 != strcmp(var->id, result.id)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }

        bytes = fprintf(stderr, "[test]: testing status_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_var_name(db_t * db, const char * path) {
    native_t vars;
    vars.db = NULL;
    vars.size = 0;

    if (load_native(path, trim_numeric, load_native_general, &vars, &load_res_native[4]) ||
        NULL == vars.db ||
        0 >= vars.size) {
        fprintf(stderr, "[load]: failed to load variable db; %s.\n", path);
        SAFE_FREE(vars.db);
        return CHECK_FAILED;
    }
    
    test_var_name_search(db, vars.db, vars.size) ?
        fprintf(stderr, "[test]: failed var_name test.\n") :
        fprintf(stderr, "[test]: passed var_name test.\n");

    SAFE_FREE(vars.db);
    return CHECK_PASSED;
}

int test_status_name_search(db_t * db, status_res * statuses, int size) {
    int i = 0;
    int bytes = 0;
    status_res * status = NULL;
    status_res result;
    for (i = 0; i < size; i++) {
        status = &statuses[i];
        if (status_name(db, &result, status->scid, status->scstr, strlen(status->scstr)) ||
            0 != strcmp(status->scstr, result.scstr)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }

        bytes = fprintf(stderr, "[test]: testing status_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_status_name(db_t * db, const char * path) {
    native_t statuses;
    statuses.db = NULL;
    statuses.size = 0;

    if (load_native(path, trim_numeric, load_native_general, &statuses, &load_res_native[3]) ||
        NULL == statuses.db ||
        0 >= statuses.size) {
        fprintf(stderr, "[load]: failed to load status db; %s.\n", path);
        SAFE_FREE(statuses.db);
        return CHECK_FAILED;
    }
    
    test_status_name_search(db, statuses.db, statuses.size)?
        fprintf(stderr, "[test]: failed status_name test.\n") :
        fprintf(stderr, "[test]: passed status_name test.\n");

    SAFE_FREE(statuses.db);
    return CHECK_PASSED;
}

int test_bonus_name_search(db_t * db, bonus_res * bonuses, int size) {
    int i = 0;
    int bytes = 0;
    bonus_res * bonus = NULL;
    bonus_res result;
    for (i = 0; i < size; i++) {
        bonus = &bonuses[i];
        if (bonus_name(db, &result, bonus->prefix, strlen(bonus->prefix), bonus->bonus, strlen(bonus->bonus)) ||
            0 != strcmp(bonus->bonus, result.bonus)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing bonus_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_bonus_name(db_t * db, const char * path) {
    native_t bonuses;
    bonuses.db = NULL;
    bonuses.size = 0;

    if (load_native(path, trim_numeric, load_native_general, &bonuses, &load_res_native[2]) ||
        NULL == bonuses.db ||
        0 >= bonuses.size) {
        fprintf(stderr, "[load]: failed to load bonus db; %s.\n", path);
        SAFE_FREE(bonuses.db);
        return CHECK_FAILED;
    }
    
    test_bonus_name_search(db, bonuses.db, bonuses.size)?
        fprintf(stderr, "[test]: failed bonus_name test.\n") :
        fprintf(stderr, "[test]: passed bonus_name test.\n");

    SAFE_FREE(bonuses.db);
    return CHECK_PASSED;
}


int test_map_id_search(db_t * db, map_res * maps, int size) {
    int i = 0;
    int bytes = 0;
    map_res * map;
    map_res result;

    for (i = 0; i < size; i++) {
        map = &maps[i];
        if (map_id(db, &result, map->id) ||
            0 != strcmp(map->map, result.map)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing map_id %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_map_name_search(db_t * db, map_res * maps, int size) {
    int i = 0;
    int bytes = 0;
    map_res * map;
    map_res result;

    for (i = 0; i < size; i++) {
        map = &maps[i];
        if (map_name(db, &result, map->map, strlen(map->map)) ||
            0 != strcmp(map->map, result.map)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing map_name %d / %d ...\r", i, size);
    }
    fprintf(stderr, "%-*s\r", bytes, "");
    return CHECK_PASSED;
}

int test_map_name(db_t * db, const char * path) {
    native_t maps;
    maps.db = NULL;
    maps.size = 0;

    if (load_native(path, trim_general, load_native_general, &maps, &load_res_native[1]) ||
        NULL == maps.db ||
        0 >= maps.size) {
        fprintf(stderr, "[load]: failed to load map db; %s.\n", path);
        SAFE_FREE(maps.db);
        return CHECK_FAILED;
    }

    (test_map_name_search(db, maps.db, maps.size)) ?
        fprintf(stderr, "[test]: failed map_name test.\n") :
        fprintf(stderr, "[test]: passed map_name test.\n");

    (test_map_id_search(db, maps.db, maps.size)) ?
        fprintf(stderr, "[test]: failed map_id test.\n") :
        fprintf(stderr, "[test]: passed map_id test.\n");

    SAFE_FREE(maps.db);
    return CHECK_PASSED;
}

int test_opt_name_search(db_t * db, option_res * options, int size) {
    int i = 0;
    int bytes = 0;
    option_res * option;
    option_res result;

    for (i = 0; i < size; i++) {
        option = &options[i];
        if (opt_name(db, &result, option->option, strlen(option->option)) ||
            0 != strcmp(option->option, result.option)) {
            fprintf(stderr, "%-*s\r", bytes, "");
            return CHECK_FAILED;
        }
        bytes = fprintf(stderr, "[test]: testing opt_name %d / %d ...\r", i, size);
    }

    fprintf(stderr, "%-*s\r", bytes, "");

    return CHECK_PASSED;
}

int test_opt_name(db_t * db, const char * path) {
    native_t options;
    options.db = NULL;
    options.size = 0;

    if (load_native(path, trim_alpha, load_native_general, &options, &load_res_native[0]) ||
        NULL == options.db ||
        0 >= options.size) {
        fprintf(stderr, "[load]: failed to load option db; %s.\n", path);
        SAFE_FREE(options.db);
        return CHECK_FAILED;
    }

    (test_opt_name_search(db, options.db, options.size)) ?
        fprintf(stderr, "[test]: failed opt_name test.\n") :
        fprintf(stderr, "[test]: passed opt_name test.\n");

    SAFE_FREE(options.db);
    return CHECK_PASSED;
}

int main(int argc, char * argv[]) {
    db_t * db = NULL;
    init_db_load(&db, RE_PATH, EA_PATH, EATHENA);
    /* test resource databases */
    /*test_opt_name(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\option_db.txt");
    test_map_name(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\mapnametable.txt");
    test_bonus_name(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\item_bonus.txt");
    test_status_name(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\status_db.txt");
    test_var_name(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\var_db.txt");
    test_block_name(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\block_db.txt");
    test_sprite_id(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\idnum2itemresnametable.txt");
    test_usprite_test(db, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\num2itemresnametable.txt");*/

    /* test eathena databases */
    /*test_const_name(db, "G:\\Git\\eathena\\db\\const.txt", EATHENA);
    test_skill_name(db, "G:\\Git\\eathena\\db\\skill_db.txt", EATHENA);
    test_item_name(db, "G:\\Git\\eathena\\db\\item_db.txt", EATHENA);
    test_mob_id(db, "G:\\Git\\eathena\\db\\mob_db.txt", EATHENA);
    test_merc_id(db, "G:\\Git\\eathena\\db\\mercenary_db.txt", EATHENA);
    test_pet_id(db, "G:\\Git\\eathena\\db\\pet_db.txt", EATHENA);
    test_produce_id(db, "G:\\Git\\eathena\\db\\produce_db.txt", EATHENA);*/
    item_group_t item_group;
    item_group_id(db, &item_group, 1);
    item_group_free(&item_group);
    deit_db_load(&db);

    //init_db_load(&db, RE_PATH, RA_PATH, RATHENA);
    ///* test rathena databases */
    //test_const_name(db, "G:\\Git\\rathena\\db\\const.txt", RATHENA);
    //deit_db_load(&db);

    //init_db_load(&db, RE_PATH, HE_PATH, HERCULE);
    ///* test hercule databases */
    //test_const_name(db, "G:\\Git\\Hercules\\db\\const.txt", HERCULE);
    //deit_db_load(&db);
    return 0;
}

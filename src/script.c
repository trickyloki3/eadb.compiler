/*
 *   file: script.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "script.h"

void block_init(block_r ** block, int size) {
    exit_null("block is null", 1, block);
    *block = calloc(size, sizeof(block_r));
}

void block_deinit(block_r * block, int size) {
    int i = 0;
    int j = 0;
    for(i = 0; i < size; i++) {
        /* buffer contains the stack of all strings */
        if(block[i].arg != NULL)
            free(block[i].arg);

        /* block information from res/block_db.txt is
         * store for any purpose, mostly minimization */
        if(block[i].type != NULL) {
            if(block[i].type->keyword != NULL) 
                free(block[i].type->keyword);
            free(block[i].type);
        }

        /* set blocks may contain variable and function
         * calls in the expression, we'll free all the
         * strings of the variable and functions here */
        if(block[i].set_dep != NULL)
            free(block[i].set_dep);

        /* logic trees are generated from conditional blocks
         * and inherited by enclosing scope of the conditional
         * block; the function checks null, so this is safe to
         * call on NULL logic trees. */
        deepfreenamerange(block[i].logic_tree);

        /* the result calculated from bonus integer arguments are
         * kept for post-integer analysis on writing the final string. */
        for(j = 0; j < BONUS_SIZE; j++)
            if(block[i].result[j] != NULL)
                node_free(block[i].result[j]);

        /* bonus blocks retain their bonus data return by sqlite 
         * for purpose of minimization and handling bonus argument. */
        if(block[i].bonus.pref != NULL) free(block[i].bonus.pref);
        if(block[i].bonus.buff != NULL) free(block[i].bonus.buff);
        if(block[i].bonus.desc != NULL) free(block[i].bonus.desc);
        if(block[i].bonus.type != NULL) free(block[i].bonus.type);
        if(block[i].bonus.order != NULL) free(block[i].bonus.order);
    }
    memset(block, 0, sizeof(block_r) * size);
}

void block_finalize(block_r * block, int size) {
    /* high level function call that might have
     * addition statements, this is to make the 
     * calling script more consistent. */
    free(block);
}

int script_lexical(token_r * token, char * buffer) {
    /* script data */
    char script[BUF_SIZE];
    int script_len = 0;

    /* lexical data */
    int i = 0;
    char * script_buf = NULL;   /* script's buffer */
    char ** script_ptr = NULL;  /* script's string */
    int script_buf_cnt = 0;     /* script's buffer index */
    int script_ptr_cnt = 0;     /* script's string index */
    int comment_level = 0;

    /* check paramaters for NULL references */
    exit_null("token is null", 1, token);
    exit_null("script is null", 1, script);

    /* clear all the white space */
    memset(script, 0, BUF_SIZE);
    for(i = 0; buffer[i] != '\0'; i++)
        script[i] = (isspace(buffer[i])) ? ' ' : buffer[i];

    script_len = strlen(script);
    if(script_len <= 0 || script_len > BUF_SIZE) 
        return SCRIPT_FAILED;
    /* initialize token structure */
    memset(token, 0, sizeof(token_r));
    script_buf = token->script;
    script_ptr = token->script_ptr;

    /* load the token structure */
    for(i = 0; i < script_len; i++) {
        /* skipped multiline comments embedded in script */
        if(i+1 < script_len && script[i] == '/' && script[i+1] == '*')
            comment_level++;
        else if(i+1 < script_len && script[i] == '*' && script[i+1] == '/') {
            comment_level--;
            i += 2;
        }
        if(comment_level) 
            continue;

        /* tokenize operators, identifiers, and syntax */
        if(ATHENA_SCRIPT_IDENTIFER(script[i]) || ATHENA_SCRIPT_SYMBOL(script[i])) {
            /* add identifier tokens */
            script_ptr[script_ptr_cnt++] = &script_buf[script_buf_cnt];
            while(ATHENA_SCRIPT_IDENTIFER(script[i]) || ATHENA_SCRIPT_SYMBOL(script[i]))
            script_buf[script_buf_cnt++] = script[i++];
            script_buf[script_buf_cnt++] = '\0';
        }

        /* skip space / single quote / double quote / slash characters */
        if(isspace(script[i]) || script[i] == '\'' || script[i] == '"' || script[i] == '\\')
            continue;

        /* abort the lexical analysis when \0 encounter */
        if(script[i] == '\0') break;

        /* add the operator or syntax */
        script_ptr[script_ptr_cnt++] = &script_buf[script_buf_cnt];
        script_buf[script_buf_cnt++] = script[i];
        script_buf[script_buf_cnt++] = '\0';
    }

   /* set the total number of tokens */
   token->script_cnt = script_ptr_cnt;
   return SCRIPT_PASSED;
}

int script_analysis(token_r * token, block_r * block, int * block_cnt, int item_id, int flag, int block_dep) {   
    /* local references */
    int i, j;
    int bracket_level = 0;
    char ** script_ptr = NULL;
    int script_cnt = 0;
    char err[BUF_SIZE];

    /* sqlite related variables */
    block_t info;
    int status = 0;

    /* parse analysis variables */
    int block_size = 0;
    block_r * block_init = 0;

    /* expand block variables */
    int ret_value = 0;
    int str_script = 0;
    int dep_block = 0;
    int new_block_cnt = 0;
    token_r * new_token = NULL;
    block_r * new_block = NULL;
    memset(&info, 0, sizeof(block_t));

    /* check paramaters for NULL references */
    exit_null("token is null", 1, token);
    exit_null(build_buffer(err, "block is null; %d",item_id), 1, block);
    exit_null("block_cnt is null", 1, block_cnt);

    /* initialize the script tokens for processing */
    script_ptr = token->script_ptr;
    script_cnt = token->script_cnt;
    if(script_cnt <= 0) return SCRIPT_FAILED;

    /* initialize the block set for loading */
    memset(block, 0, sizeof(block_r) * BLOCK_SIZE);
    block_size = block_dep;
    *block_cnt = 0;

    /* process the tokens and parse blocks */
    for(i = 0; i < script_cnt; i++) {
        if(script_ptr[i][0] == '{')
            bracket_level++;
        else if(script_ptr[i][0] == '}')
            bracket_level--;
        
        if(block_size >= BLOCK_SIZE) 
            exit_abt("BLOCK_SIZE overflow; increase size in script.h");

        /* check if token is a script keyword */
        status = block_keyword_search(global_db, &info, script_ptr[i]);
        if(!status) {
            /* initialize the block */
            block_init = &block[block_size];
            block_init->block_id = block_size;
            block_init->arg = calloc(BUF_SIZE, sizeof(char));
            block_init->item_id = item_id;
            block_init->ptr_cnt = 0;
            block_init->type = calloc(1, sizeof(block_t));
            *(block_init->type) = info;
            block_init->type->keyword = convert_string(info.keyword);
            block_init->link = block_dep-1;  /* -1 to indicate top-level */
            /* set the first argument pointer */
            block_init->ptr[0] = block_init->arg;
            block_init->ptr_cnt++;

            /* retrieve all the arguments of the block */
            switch(block[block_size].type->id) {
                case 26: /* parse if blocks */
                    /* parse the condition ( <condition> ) */
                    if(script_ptr[i+1][0] != '(') return SCRIPT_FAILED;
                    script_parse(token, &i, block_init, '\0', ')', 0x01);
                    if(script_ptr[i][0] != ')') return SCRIPT_FAILED;

                    /* compound or simple statement */
                    (script_ptr[i+1][0] == '{') ?
                        script_parse(token, &i, block_init, '\0', '}', 0x01):
                        script_parse(token, &i, block_init, '\0', ';', 0x03);
                    break;
                case 27: /* parse else blocks */
                    /* link with the an else if; continue linking until else*/
                    if(strcmp(script_ptr[i+1],"if") == 0) {
                        /* skip the 'if' */
                        i += 1;

                        /* continue linking until else */
                        block_init->link = dep_block;

                        /* parse the condition ( <condition> ) */
                        if(script_ptr[i+1][0] != '(') return SCRIPT_FAILED;
                        script_parse(token, &i, block_init, '\0', ')', 0x01);
                        if(script_ptr[i][0] != ')') return SCRIPT_FAILED;

                        /* compound or simple statement */
                        (script_ptr[i+1][0] == '{') ?
                            script_parse(token, &i, block_init, '\0', '}', 0x01):
                            script_parse(token, &i, block_init, '\0', ';', 0x03);
                    } else {
                        /* discontinue linking after else */
                        block_init->link = dep_block;

                        /* compound or simple statement */
                        (script_ptr[i+1][0] == '{') ?
                            script_parse(token, &i, block_init, '\0', '}', 0x01):
                            script_parse(token, &i, block_init, '\0', ';', 0x03);
                    }
                    break;
                default: /* parse all blocks */
                    script_parse(token, &i, block_init, ',', ';', 0x00);
                    break;
            }

            /* argument count is typically off by one */
            block_init->ptr_cnt--;

            /* load the next block */
            dep_block = block_size;
            block_size++;

            /* script nesting blocks are organize into a hierarchy by linking */
            if(strcmp(block_init->type->keyword,"if") == 0 || 
               strcmp(block_init->type->keyword,"else") == 0 ||
               strcmp(block_init->type->keyword,"autobonus") == 0 ||
               strcmp(block_init->type->keyword,"autobonus2") == 0 ||
               strcmp(block_init->type->keyword,"autobonus3") == 0 ||
               strcmp(block_init->type->keyword,"bonus_script") == 0) {
                /* lexical analysis the nested script */
                new_token = calloc(1, sizeof(token_r));
                /* set the index to the script for the block
                   script for if is usually in ptr[1];
                   script for else can be either ptr[1] or ptr[0];
                   script always ptr_cnt - 1 after ptr_cnt is normalize */
                str_script = (strcmp(block_init->type->keyword,"autobonus") == 0 ||
                              strcmp(block_init->type->keyword,"autobonus2") == 0 ||
                              strcmp(block_init->type->keyword,"autobonus3") == 0 ||
                              strcmp(block_init->type->keyword,"bonus_script") == 0)?
                              0:block_init->ptr_cnt - 1;

                /* lexical; select flag depending on simple or compound statement */
                ret_value = (block_init->ptr[str_script][0] == '{') ?
                    script_lexical(new_token, block_init->ptr[str_script]):
                    script_lexical(new_token, block_init->ptr[str_script]);
                if(ret_value == SCRIPT_FAILED) return SCRIPT_FAILED;
                
                /* allocate a new set of block, which will be merge
                 * with top level set of blocks */
                new_block = calloc(BLOCK_SIZE, sizeof(block_r));
                ret_value = script_analysis(new_token, new_block, &new_block_cnt, item_id, 0x00, block_size);
                if(ret_value == SCRIPT_FAILED) return SCRIPT_FAILED;
                
                /* link to dependency block and append to global block */
                for(j = 0; j < new_block_cnt; j++) {
                    block[block_size] = new_block[block_size];
                    block_size++;
                }

                free(new_block);  /* this frees only the block array, not the blocks themselves */
                free(new_token);
            }
        }
   }

   /* failed to parse any blocks */
   if(block_size <= 0) return SCRIPT_PASSED;
   if(info.keyword != NULL) free(info.keyword);

   /* readjust size when recursively parsing */
   *block_cnt = block_size - block_dep;
   return SCRIPT_PASSED;
}

int script_parse(token_r * token, int * position, block_r * block, char delimit, char end, int flag) {
    /* token reading data */
    int i = 0;
    int j = 0;
    int len = 0;
    char ** script_ptr = NULL;
    int script_cnt = 0;
    int bracket_level = 0;
    int subexpr_level = 0;

    /* block parsing data */
    char * arg = NULL;
    char ** ptr = NULL;
    int arg_cnt = 0;
    int ptr_cnt = 0;

    /* check paramaters */
    exit_null("token is null", 1, token);
    exit_null("position is null", 1, position);
    exit_null("block is null", 1, block);

    /* initialize token set */
    script_ptr = token->script_ptr;
    script_cnt = token->script_cnt;

    /* initialize block */
    arg = block->arg;
    ptr = block->ptr;
    arg_cnt = block->arg_cnt;
    ptr_cnt = block->ptr_cnt;
    if(*position < 0 || *position >= script_cnt) 
        return SCRIPT_FAILED;

    /* parse the block */
    for(i = *position + 1; i < script_cnt; i++) {
        /* indicate bracket level or subexpression level*/
        switch(script_ptr[i][0]) {
            case '{': bracket_level++; break;
            case '}': bracket_level--; break;
            case '(': subexpr_level++; break;
            case ')': subexpr_level--; break;
            default: break;
        }

        /* parsing top-level script; delimit exit only at top-level 
        flag 4 allows us to bypass any leveling */
        if((!bracket_level && !subexpr_level) || flag & 4) {
                /* delimiter, parse another argument */
                if(script_ptr[i][0] == delimit) {
                    if(arg_cnt > 0 && arg[arg_cnt-1] == ' ') {
                        arg[arg_cnt-1] = '\0';
                    ptr[ptr_cnt++] = &arg[arg_cnt];
                } else {
                    arg[arg_cnt++] = '\0';
                    ptr[ptr_cnt++] = &arg[arg_cnt];
                }
                continue;
            /* semicolon ends the block */
            } else if(script_ptr[i][0] == end) {
                /* include last token */
                if(flag & 0x01) {
                    /* write the argument */
                    len = strlen(script_ptr[i]);
                    for(j = 0; j < len; j++) 
                        arg[arg_cnt++] = script_ptr[i][j];

                    /* don't add space after athena symbol prefixes */
                    if(!ATHENA_SCRIPT_SYMBOL(script_ptr[i][0]))
                        arg[arg_cnt++] = ' ';
                }

                if(arg_cnt > 0 && arg[arg_cnt-1] == ' ') {
                    arg[arg_cnt-1] = '\0';
                    ptr[ptr_cnt++] = &arg[arg_cnt];
                } else {
                    arg[arg_cnt++] = '\0';
                    ptr[ptr_cnt++] = &arg[arg_cnt];
                }
                break;
            }
        }

        /* skip the comma at top level */
        if(script_ptr[i][0] == ',' && !bracket_level && !subexpr_level && !(flag & 0x02)) 
            continue;

        /* write the argument */
        len = strlen(script_ptr[i]);
        for(j = 0; j < len; j++) 
            arg[arg_cnt++] = script_ptr[i][j];

        /* don't add space after athena symbol prefixes */
        if(!ATHENA_SCRIPT_SYMBOL(script_ptr[i][0]))
            arg[arg_cnt++] = ' ';
    }

    block->arg_cnt = arg_cnt;
    block->ptr_cnt = ptr_cnt;
    *position = i;
    return SCRIPT_PASSED;
}

int script_dependencies(block_r * block, int block_cnt) {
   int i, j, k;

    /* super ad-hoc and vague dependency matching;
       note-to-self: don't even try re-reading this */
    for(i = 0; i < block_cnt; i++)
        /* constant (28) for set block is in block_db.txt */
        if(block[i].type->id == 28)
            /* link this block as dependency where ever it is reference */
            for(j = i + 1; j < block_cnt; j++)
                /* check all the argument each block; start at 1 for set blocks, 0 may be same var */
                for(k = ((block[j].type->id == 28)?1:0); k < block[j].ptr_cnt; k++) {
                    /* else blocks, may be chained; if ptr_cnt > 2, check the condition */
                    if(block[j].type->id == 27 && k == ((block[j].ptr_cnt >= 2)?1:0)) break;
    
                    /* attempt to find a dependency */
                    if(strstr(block[j].ptr[k], block[i].ptr[0]) != NULL)
                      block[j].depd[block[j].depd_cnt++] = &block[i];
    
                    /* for if blocks, only check the conditional */
                    if(block[j].type->id == 26 && !k) break;
                }

    return SCRIPT_PASSED;
}

int script_translate(block_r * block, int size) {
    int i;
    int link = 0;
    logic_node_t * temp = NULL;

    exit_null("block is null", 1, block);
    if(size < 0 || size >= BLOCK_SIZE) exit_abt("invalid block size.");

    /* translate each block; multiplex to handler */
    for(i = 0; i < size; i++) {
        /* IMPORTANT
         * blocks are stored in an array and are linked
         * by its index during the parsing phase, i.e.
         * block[5] might be linked to block[0].
         * 
         * blocks will always link to a block above it,
         * that is, for an index x or block[x], the link
         * will always be less than x, block[x].link < x.
         * 
         * script_translate processes each block starting
         * from 0 to the last block meaning that parent 
         * block will always be process before the child
         * blocks.
         *
         * Since linking in the parsing phase is only by
         * index, any data that needs to be process in
         * the parent blocks needs to be passed down to
         * the child block, i.e. child block inherits
         * from parent.
        */
        
        /* not the first block and links to another block */
        if(i > 0 && block[i].link >= 0) {
            link = block[i].link;
            /* linked the logic tree */
            if(block[link].logic_tree != NULL)
                block[i].logic_tree = copy_deep_any_tree(block[link].logic_tree);
        }

        /* translate each block using a specific handle;
         * block numbers are define in the block_db.txt;
         * for example, 0 = bonus, 1 = bonus2, etc. */
        switch(block[i].type->id) {
            case 0: translate_bonus(&block[i], 0x01); break;                                                        /* bonus */
            case 1: translate_bonus(&block[i], 0x02); break;                                                        /* bonus2 */
            case 2: translate_bonus(&block[i], 0x04); break;                                                        /* bonus3 */
            case 3: translate_bonus(&block[i], 0x08); break;                                                        /* bonus4 */
            case 4: translate_bonus(&block[i], 0x10); break;                                                        /* bonus5 */
            case 5: translate_autobonus(&block[i], 0x01); break;                                                    /* autobonus */
            case 6: translate_autobonus(&block[i], 0x02); break;                                                    /* autobonus2 */
            case 7: translate_autobonus(&block[i], 0x04); break;                                                    /* autobonus3 */
            case 8: translate_heal(&block[i], block[i].type->id); break;                                            /* heal */
            case 9: translate_heal(&block[i], block[i].type->id); break;                                            /* percentheal */
            case 10: translate_heal(&block[i], block[i].type->id); break;                                           /* itemheal */
            case 11: translate_skill_block(&block[i], block[i].type->id); break;                                    /* skill */
            case 12: translate_skill_block(&block[i], block[i].type->id); break;                                    /* itemskill */
            case 13: translate_skill_block(&block[i], block[i].type->id); break;                                    /* unitskilluseid */
            case 14: translate_status(&block[i], block[i].type->id); break;                                         /* sc_start */
            case 15: translate_status(&block[i], block[i].type->id); break;                                         /* sc_start4 */
            case 16: translate_status_end(&block[i]); break;                                                        /* sc_end */
            case 17: translate_getitem(&block[i], block[i].type->id); break;                                        /* getitem */
            case 18: translate_rentitem(&block[i], block[i].type->id); break;                                       /* rentitem */
            case 19: translate_delitem(&block[i], block[i].type->id); break;                                        /* delitem */
            case 20: translate_getrandgroup(&block[i], block[i].type->id); break;                                   /* getrandgroupitem */
            case 23: translate_write(&block[i], "Set new font.", 1); break;                                         /* setfont */
            case 24: translate_bstore(&block[i], block[i].type->id); break;                                         /* buyingstore */
            case 25: translate_bstore(&block[i], block[i].type->id); break;                                         /* searchstores */
            case 26: evaluate_expression(&block[i], block[i].ptr[0], 1, EVALUATE_FLAG_KEEP_LOGIC_TREE | EVALUATE_FLAG_EXPR_BOOL); break;        /* if */
            case 27: /* invert the linked logic tree; only the top needs to be inverted */
                        if(block[i].logic_tree != NULL) {
                            temp = block[i].logic_tree;
                            block[i].logic_tree = inverse_logic_tree(block[i].logic_tree);
                            block[i].logic_tree->stack = temp->stack;
                            freenamerange(temp);
                        }

                        /* add the else if condition onto the stack */
                        if(block[i].ptr_cnt > 1)
                            evaluate_expression(&block[i], block[i].ptr[0], 1, EVALUATE_FLAG_KEEP_LOGIC_TREE | EVALUATE_FLAG_EXPR_BOOL);
                        break;                                                                                      /* else */
            case 28: evaluate_expression(&block[i], block[i].ptr[1], 1, 
                     EVALUATE_FLAG_KEEP_LOGIC_TREE|EVALUATE_FLAG_WRITE_FORMULA|
                     EVALUATE_FLAG_KEEP_TEMP_TREE|EVALUATE_FLAG_EXPR_BOOL); break;      /* set */
            case 30: translate_write(&block[i], "Send a message through the announcement system.", 1); break;       /* announce */
            case 31: translate_misc(&block[i], "callfunc"); break;                                                  /* callfunc */
            case 33: translate_misc(&block[i], "warp"); break;                                                      /* warp */
            case 34: translate_pet_egg(&block[i], block[i].type->id); break;                                        /* pet */
            case 35: translate_write(&block[i], "Hatch a pet egg.", 1); break;                                      /* bpet */
            case 36: translate_hire_merc(&block[i], block[i].type->id); break;                                      /* mercenary_create */
            case 37: translate_heal(&block[i], block[i].type->id); break;                                           /* mercenary_heal */
            case 38: translate_status(&block[i], block[i].type->id); break;                                         /* mercenary_sc_status */
            case 39: translate_produce(&block[i], block[i].type->id); break;                                        /* produce */
            case 40: translate_produce(&block[i], block[i].type->id); break;                                        /* cooking */
            case 41: translate_produce(&block[i], block[i].type->id); break;                                        /* makerune */
            case 42: translate_getexp(&block[i], block[i].type->id); break;                                         /* getguildexp */
            case 43: translate_getexp(&block[i], block[i].type->id); break;                                         /* getexp */
            case 44: translate_misc(&block[i], "monster"); break;                                                   /* monster */
            case 45: translate_write(&block[i], "Evolve homunculus when requirements are met.", 1); break;          /* homevolution */
            case 46: translate_write(&block[i], "Change to Summer Outfit when worn.", 1); break;                    /* setoption */
            case 47: translate_write(&block[i], "Summon a creature to mount. [Work for all classes].", 1); break;   /* setmounting */
            case 48: translate_setfalcon(&block[i]); break;                                                         /* setfalcon */
            case 49: translate_getrandgroup(&block[i], block[i].type->id); break;                                   /* getgroupitem */
            case 50: translate_write(&block[i], "Reset all status points.", 1); break;                              /* resetstatus */
            case 51: translate_bonus_script(&block[i]); break;                                                      /* bonus_script */
            case 52: translate_write(&block[i], "Play another background song.", 1); break;                         /* playbgm */
            case 53: translate_transform(&block[i]); break;                                                         /* transform */
            case 54: translate_status(&block[i], block[i].type->id); break;                                         /* sc_start2 */
            default: break;
        }
    }
    
    return SCRIPT_PASSED;
}

/* for each non-nullified block, attempt to minimize with
 * blocks on the same enclosing block and bonus id */
int script_bonus(block_r * block, int block_cnt) {
    int i = 0;
    int j = 0;
    int k = 0;

    /* cache current and block pointer */
    int cur_blk_type = 0;
    int cur_blk_flag = 0;
    block_r * cur_blk;      /* attempt to minimize current block */
    block_r * cmp_blk;      /* compare current block with this block */

    /* hold the aggregated strings into these buffers,
     * should be ok to put these massive buffers on stack */
    int total[BONUS_SIZE];              /* total strings aggregated */
    int offset[BONUS_SIZE];             /* length of string */
    char buffer[BONUS_SIZE][BUF_SIZE];  /* aggregate buffer holding strings */
    char aux[BUF_SIZE];     /* =.= */   /* formatting buffer */

    for(i = 0; i < block_cnt; i++) {
        memset(total, 0, sizeof(int) * BONUS_SIZE);
        memset(offset, 0, sizeof(int) * BONUS_SIZE);
        cur_blk = &block[i];
        cur_blk_type = cur_blk->type->id;
        cur_blk_flag = cur_blk->bonus.flag;

        /* check block is a bonus block and 
         * not nullified and can be minimized */
        if(cur_blk_type > BONUS_ID_MAX ||
           cur_blk_type == BLOCK_NULLIFIED ||
           !(cur_blk_flag & 0x000C0000))
            continue;

        for(j = i + 1; j < block_cnt; j++) {
            cmp_blk = &block[j];

            /* bonus must be in same enclosing 
             * block and bonus id must match. */
            if( cur_blk->bonus.id != cmp_blk->bonus.id || 
                cur_blk->link != cmp_blk->link) 
                continue;

            /* MINIZ - indicates that arguments must be checked 
             * MINIS - indicates that only one single argument
             *         for the bonus and also an aggregate. */
            if(cur_blk_flag & BONUS_FLAG_MINIZ) {
                /* check all specified arguments in item_bonus.txt 
                 * all arguments are process from left to right in
                 * translate_bonus, therefore indexing the same
                 * block->eng[i] yields the same argument order. 
                 * bonus block supports only up to 5 arguments. */
                if( (cur_blk_flag & BONUS_FLAG_EQ_1 &&
                    ncs_strcmp(cur_blk->eng[0], cmp_blk->eng[0]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_2 &&
                    ncs_strcmp(cur_blk->eng[1], cmp_blk->eng[1]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_3 &&
                    ncs_strcmp(cur_blk->eng[2], cmp_blk->eng[2]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_4 &&
                    ncs_strcmp(cur_blk->eng[3], cmp_blk->eng[3]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_5 &&
                    ncs_strcmp(cur_blk->eng[4], cmp_blk->eng[4]) != 0)) 
                    continue;
            } else if(cur_blk_flag * BONUS_FLAG_MINIS) {
                /* single argument for bonus is also an aggregate */
            } else {
                /* BONUS_FLAG_MINIS or BONUS_FLAG_MINIZ must be set */
                fprintf(stderr, "[warn]: bonus flag must be either MINIS or MINIZ\n"
                                "[warn]: check item_bonus.txt on bonus id %d\n"
                                "[warn]: abort current bonus on item id %d\n",
                                cur_blk->bonus.id, cur_blk->item_id);
                break;
            }

            /* bonus blocks that are exactly the same should throw 
             * an error when NODUP is set; both NODUP and STACK are 
             * mutually exclusive. Supports only one aggregate 
             * variable at the moment. */
            if(cur_blk_flag & BONUS_FLAG_NODUP) {
                /* check that the aggregate fields are not the same */
                if( (cur_blk_flag & BONUS_FLAG_AG_1 &&
                    ncs_strcmp(cur_blk->eng[0], cmp_blk->eng[0]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_2 &&
                    ncs_strcmp(cur_blk->eng[1], cmp_blk->eng[1]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_3 &&
                    ncs_strcmp(cur_blk->eng[2], cmp_blk->eng[2]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_4 &&
                    ncs_strcmp(cur_blk->eng[3], cmp_blk->eng[3]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_5 &&
                    ncs_strcmp(cur_blk->eng[4], cmp_blk->eng[4]) == 0)) {
                    /* throw error and nullified duplicate block */
                    fprintf(stderr,
                        "[error]: bonus duplicated on item id %d.\n"
                        "[error]: %s in block %d and %s in block %d.\n", 
                        cur_blk->item_id, 
                        cur_blk->bonus.buff, i,
                        cmp_blk->bonus.buff, j);
                    block[j].type->id = BLOCK_NULLIFIED;
                    continue;
                }
            } else if(cur_blk_flag & BONUS_FLAG_STACK) {
                /* stack only arguments with a result node from translate_bonus,
                 * i.e. stack only arguments with integer values. 
                 * Indexing the block->result also uses the same left to right
                 * ordering, thereforce is safe. */
                if(cur_blk_flag & BONUS_FLAG_ST_1)
                    minimize_stack(cur_blk->result[0], cmp_blk->result[0]);
                if(cur_blk_flag & BONUS_FLAG_ST_2)
                    minimize_stack(cur_blk->result[1], cmp_blk->result[1]);
                if(cur_blk_flag & BONUS_FLAG_ST_3)
                    minimize_stack(cur_blk->result[2], cmp_blk->result[2]);
                if(cur_blk_flag & BONUS_FLAG_ST_4)
                    minimize_stack(cur_blk->result[3], cmp_blk->result[3]);
                if(cur_blk_flag & BONUS_FLAG_ST_5)
                    minimize_stack(cur_blk->result[4], cmp_blk->result[4]);
            } else {
                /* BONUS_FLAG_NODUP or BONUS_FLAG_STACK must be set */
                fprintf(stderr, "[warn]: bonus flag must be either STACK or NODUP\n"
                                "[warn]: check item_bonus.txt on bonus id %d\n"
                                "[warn]: abort current bonus on item id %d\n",
                                cur_blk->bonus.id, cur_blk->item_id);
                break;
            }

            /* aggregating strings and stacking integers
             * are mutually exclusive; aggregate strings
             * here */
            if(!(cur_blk_flag & BONUS_FLAG_STACK)) {
                if(cur_blk_flag & BONUS_FLAG_AG_1) {
                    offset[0] += (offset[0] <= 0) ?
                        sprintf(buffer[0] + offset[0],"%s, %s", cur_blk->eng[0], cmp_blk->eng[0]):
                        sprintf(buffer[0] + offset[0],", %s", cmp_blk->eng[0]);
                    total[0]++;
                }
                if(cur_blk_flag & BONUS_FLAG_AG_2) {
                    offset[1] += (offset[1] <= 0) ?
                        sprintf(buffer[1] + offset[1],"%s, %s", cur_blk->eng[1], cmp_blk->eng[1]):
                        sprintf(buffer[1] + offset[1],", %s", cmp_blk->eng[1]);
                    total[1]++;
                }
                if(cur_blk_flag & BONUS_FLAG_AG_3) {
                    offset[2] += (offset[2] <= 0) ?
                        sprintf(buffer[2] + offset[2],"%s, %s", cur_blk->eng[2], cmp_blk->eng[2]):
                        sprintf(buffer[2] + offset[2],", %s", cmp_blk->eng[2]);
                    total[2]++;
                }
                if(cur_blk_flag & BONUS_FLAG_AG_4) {
                    offset[3] += (offset[3] <= 0) ?
                        sprintf(buffer[3] + offset[3],"%s, %s", cur_blk->eng[3], cmp_blk->eng[3]):
                        sprintf(buffer[3] + offset[3],", %s", cmp_blk->eng[3]);
                    total[3]++;
                }
                if(cur_blk_flag & BONUS_FLAG_AG_5) {
                    offset[4] += (offset[4] <= 0) ?
                        sprintf(buffer[4] + offset[4],"%s, %s", cur_blk->eng[4], cmp_blk->eng[4]):
                        sprintf(buffer[4] + offset[4],", %s", cmp_blk->eng[4]);
                    total[4]++;
                }
            }

            /* nullified the minimized block */
            block[j].type->id = BLOCK_NULLIFIED;

            /*fprintf(stderr, 
                "block_index: %d | %d\n"
                "bonus_id: %d | %d - %s\n"
                "bonus_link: %d | %d - %s\n\n",
                i, j,
                cur_blk->bonus.id, cmp_blk->bonus.id, 
                (cur_blk->bonus.id == cmp_blk->bonus.id) ? 
                "matched" : "nonmatched",
                cur_blk->link, cmp_blk->link,
                (cur_blk->link == cmp_blk->link) ? 
                "matched" : "nonmatched");*/
        }

        /* substitute the aggregate string into the argument translation 
         * and fix the english x and y or x, y, and z formats */
        for(j = 0; j < BONUS_SIZE; j++)
            if(offset[j] > 0) {
                /* locate the last argument delimit by comma */
                for(k = strlen(buffer[j]) - 1; k >= 0; k--)
                    if(buffer[j][k] == ',')
                        break;
                buffer[j][k] = '\0';

                /* write the new argument with the 'and' inserted */
                k = (total[j] > 1) ?
                    sprintf(aux, "%s, and %s", buffer[j], &buffer[j][k + 2]):
                    sprintf(aux, "%s and %s", buffer[j], &buffer[j][k + 2]);
                aux[k] = '\0';
                translate_overwrite(cur_blk, aux, j);
            }
    }

    /* generate the final item description for bonus blocks */
    for(i = 0; i < block_cnt; i++)
        if(block[i].type->id >= 0 && block[i].type->id <= 4)
            translate_bonus_desc(block[i].result, &block[i], &block[i].bonus);

    return SCRIPT_PASSED;
}

int script_extend(block_r * block, char * expr) {
    int offset = 0;
    token_r token;
    /* lexical analysis on expression and return failed if failed */
    if(script_lexical(&token, expr) == SCRIPT_FAILED)
        exit_buf("failed to perform lexical analysis on '%s' in item %d", expr, block->item_id);
    /* check that tokens exist */
    if(token.script_cnt <= 0)
        exit_buf("zero tokens on '%s' in item %d", expr, block->item_id);
    /* parse the callfunc paramaters */
    block->ptr_cnt++; /* last pointer still contains a argument */
    script_parse(&token, &offset, block, ',',')',4);
    block->ptr_cnt--; /* off by one */
    return SCRIPT_PASSED;
}

char * script_compile_raw(char * script, int item_id, FILE * dbg) {
    token_r token_list;
    block_r * block_list = NULL;
    int block_cnt = 0;
    char buffer[BUF_SIZE];
    int offset = 0;

    /* only lexical, parser, and translator are likely to fail, 
     * however, the other functions may fail in the future. */
    block_init(&block_list, BLOCK_SIZE);
    if(script_lexical(&token_list, script) == SCRIPT_PASSED) {
        if(script_analysis(&token_list, block_list, &block_cnt, item_id, 0x01, 0) == SCRIPT_PASSED) {
            if(script_dependencies(block_list, block_cnt) == SCRIPT_PASSED) {
                if(script_translate(block_list, block_cnt) == SCRIPT_PASSED) {
                    if(script_bonus(block_list, block_cnt) == SCRIPT_PASSED) {
                        if(script_generate(block_list, block_cnt, buffer, &offset) == SCRIPT_PASSED) {
                            if(dbg != NULL) block_debug_dump_all(block_list, block_cnt, dbg);
                        } else {
                            fprintf(stderr,"%s;%d; generator failed; invalid blocks in %s "
                                "for item id %d\n", __func__, __LINE__, script, item_id);        
                        }
                    } else {
                        fprintf(stderr,"%s;%d; optimizer failed; invalid bonuses in %s "
                            "for item id %d\n", __func__, __LINE__, script, item_id);    
                    }
                } else {
                    fprintf(stderr,"%s;%d; translator failed; invalid blocks in %s "
                        "for item id %d\n", __func__, __LINE__, script, item_id);
                }
            } else {
                fprintf(stderr,"%s;%d; dependency matching failed; invalid set blocks in %s "
                    "for item id %d\n", __func__, __LINE__, script, item_id);
            }        
        } else {
            fprintf(stderr,"%s;%d; parser failed; invalid grammer in %s "
                "for item id %d\n", __func__, __LINE__, script, item_id);
        }
    } else {
        fprintf(stderr,"%s;%d; lexer failed; invalid tokens in %s "
            "for item id %d\n", __func__, __LINE__, script, item_id);
    }
    block_deinit(block_list, BLOCK_SIZE);
    block_finalize(block_list, BLOCK_SIZE);
    return (offset > 0) ? convert_string(buffer) : NULL;
}

int translate_getitem(block_r * block, int handler) {
    int i = 0;
    char buf[BUF_SIZE];
    int offset = 1;
    /* borrow algorithm from evaluate_function */
    token_r token_list;
    int sub_level = 0;
    node_t * resultOne = NULL;
    node_t * resultTwo = NULL;

    /* support parenthesize getitem =.= */
    if(block->ptr[0][0] == '(') {
        memset(&token_list, 0, sizeof(token_r));
        if(script_lexical(&token_list, block->ptr[0]) != SCRIPT_PASSED)
            exit_buf("failed to perform lexical analysis on %s in item %d", block->ptr[0], block->item_id);
        /* chop off the parenthesis on both open by +- index */
        for(i = 1; i < token_list.script_cnt - 1; i++) {
            switch(token_list.script_ptr[i][0]) {
                case '(': sub_level++; break;
                case ')': sub_level--; break;
                default: break;
            }
            /* found the top level comma position */
            if(!sub_level && token_list.script_ptr[i][0] == ',') break;
        }

        resultOne = evaluate_expression_recursive(block, token_list.script_ptr, 1, i, block->logic_tree, 0);
        resultTwo = evaluate_expression_recursive(block, token_list.script_ptr, i + 1, token_list.script_cnt - 1, block->logic_tree, 0);
        offset = sprintf(buf, "%d", minrange(resultOne->range));
        buf[offset] = '\0';
        translate_write(block, buf, 1);

        offset = sprintf(buf, "%d", minrange(resultTwo->range));
        buf[offset] = '\0';
        translate_write(block, buf, 1);

        if(resultOne != NULL) node_free(resultOne);
        if(resultTwo != NULL) node_free(resultTwo);
    } else {
        /* evaulate the expression because callfunc is called for some of the blocks */
        evaluate_expression(block, block->ptr[0], 1, 0);
        evaluate_expression(block, block->ptr[1], 1, 0);
    }

    /* special note; first condition is special case handling of values on stack */
    if(block->flag & 0x01) {
        offset = sprintf(buf,(block->flag & 0x02)?"Randomly Select\n":"Collect Items & Equipment");
        for(i = block->offset; i < block->ptr_cnt; i++) {
            translate_item(block, block->ptr[i]);
            offset += sprintf(buf + offset,"=> %s\n", block->eng[block->eng_cnt - 1]);
        }
    } else {
        translate_item(block, block->eng[0]);
        if(block->eng_cnt < 4)
            offset = sprintf(buf, "Retrieve %s %s.", block->eng[1], block->eng[2]);
        else
            offset = sprintf(buf, "Retrieve %s %s.", block->eng[2], block->eng[3]);
    }

    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_rentitem(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    translate_item(block, block->ptr[0]);
    translate_time(block, block->ptr[1]);
    offset = sprintf(buf,"Rent %s for %s.", block->eng[0], block->eng[2]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_delitem(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    translate_item(block, block->ptr[0]);
    offset = sprintf(buf,"Remove %s.", block->eng[0]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_getrandgroup(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];

    /* getrandgroupitem for has weir call syntax */
    if(handler == 20) {
        if(block->ptr_cnt == 1) {
            script_extend(block, block->ptr[0]);
            translate_const(block, block->ptr[1], 0x20);
            evaluate_expression(block, block->ptr[2], 1, 0);
            offset = sprintf(buf,"Collect %s item%s from %s.", block->eng[1], (convert_integer(block->eng[1], 10) > 1)?"s":"", block->eng[0]);
        } else if(block->ptr_cnt == 2) {
            evaluate_expression(block, block->ptr[0], 1, 0);
            evaluate_expression(block, block->ptr[1], 1, 0);
            translate_const(block, block->eng[0], 0x20);
            offset = sprintf(buf,"Collect %s item%s from %s.", block->eng[1], (convert_integer(block->eng[2], 10) > 1)?"s":"", block->eng[2]);
        }
    } else if(handler == 49) {
        script_extend(block, block->ptr[0]);
        evaluate_expression(block, block->ptr[1], 1, 0);
        translate_const(block, block->eng[0], 0x20);
        offset = sprintf(buf,"Collect 1 item from %s.", block->eng[1]);
    } else {
        exit_abt("invalid code path");
    }

    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_bstore(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    evaluate_expression(block, block->ptr[0], 1, 0);
    offset = (handler == 25) ?
        sprintf(buf,"Allows up to %s search for stores.", block->eng[0]):
        sprintf(buf,"Open buying store with %s slots.", block->eng[0]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_hire_merc(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    translate_id(block, block->ptr[0], 0x04);
    translate_time(block, block->ptr[1]);
    offset = sprintf(buf,"Hire %s for %s.", block->eng[0], block->eng[2]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_pet_egg(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    translate_id(block, block->ptr[0], 0x01);
    offset = sprintf(buf,"Egg containing %s.", block->eng[0]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_getexp(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * result = NULL;
    result = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA);
    if(result != NULL) {
        if(result->cond_cnt > 0)
            offset = sprintf(buf,"Gain %s (%s) %s experience.", block->eng[0], result->expr, (handler == 43)?"player":"guild");
        else
            offset = sprintf(buf,"Gain %s %s experience.", block->eng[0], (handler == 43)?"player":"guild");
        buf[offset] = '\0';
        translate_write(block, buf, 1);
        node_free(result);
    } else {
        exit_buf("failed to process getguildexp for item %d", block->item_id);
    }
    return SCRIPT_PASSED;
}

int translate_transform(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    /* support both mob id and name */
    if(isdigit(block->ptr[0][0])) {
        translate_id(block, block->ptr[0], 0x01);
        translate_time(block, block->ptr[1]); 
        offset = sprintf(buf, "Transform into a %s for %s.", block->eng[0], block->eng[2]);
    } else {
        translate_write(block, block->ptr[0], 1);
        translate_time(block, block->ptr[1]);
        offset = sprintf(buf, "Transform into a %s for %s.", block->eng[0], block->eng[2]);
    }
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_skill_block(block_r * block, int handler) {
    char buf[BUF_SIZE];     /* description */
    char aux[BUF_SIZE];     /* formula */
    node_t * result = NULL; /* formula expression */
    int offset = 1;

    /* skip GID for unitskilluseid */
    offset = (handler == 13)?1:0;
    /* translate skill and level */
    translate_skill(block, block->ptr[offset]);
    /* check whether level has a formula or not */
    result = evaluate_expression(block, block->ptr[offset+1], 1, 
        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA); 
        exit_null("result is null", 1, result);
    /* enable for skill block and cast for others;
     * add support for formula on level. */
    offset += (handler == 11) ?
        sprintf(buf,"Enable %s [Lv. %s]",block->eng[0], formula(aux, block->eng[1], result)):
        sprintf(buf,"Cast %s [Lv. %s]",block->eng[0], formula(aux, block->eng[1], result));
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    if(result != NULL) node_free(result);
    return SCRIPT_PASSED;
}

int translate_heal(block_r * block, int handler) {
    char buf[BUF_SIZE];
    int offset = 1;

    evaluate_expression(block, block->ptr[0], 1, 0);
    evaluate_expression(block, block->ptr[1], 1, 0);
    if(!CHECK_ZERO(block->eng[0])) 
        offset += sprintf(buf,"%s %sHP by %s%s.",
            (CHECK_NEGATIVE(block->eng[0])?"Drain":"Recover"), 
            (handler == 37)?"Mercenary ":"",
            block->eng[0],
            (handler == 9)?"%":"");
    if(!CHECK_ZERO(block->eng[1])) 
        offset += sprintf(buf + offset - 1,"%s%s SP by %s%s.",
            (!CHECK_ZERO(block->eng[0])?" ":""),
            (CHECK_NEGATIVE(block->eng[1])?"Drain":"Recover"), 
            block->eng[1],
            (handler == 9)?"%":"");
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

/* the final item description is deferred until script bonus */
int translate_bonus(block_r * block, int flag) {
    int i, j;
    /* keep expression information to improve translation quality;
     * only integer type bonus arguments keep their node; these
     * nodes also keep track of variables use to calculate final
     * result. */
    memset(&block->bonus, 0, sizeof(bonus_t));
    memset(block->result, 0, sizeof(node_t *) * BONUS_SIZE);
    exit_null("bonus is null", 1, block);

    /* search the bonus database for the buff identifier */
    if(flag & 0x01)
        if(bonus_name_search(global_db, &block->bonus, "bonus", block->ptr[0]))
            exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x02)
        if(bonus_name_search(global_db, &block->bonus, "bonus2", block->ptr[0]))
            exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x04)
        if(bonus_name_search(global_db, &block->bonus, "bonus3", block->ptr[0]))
            exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x08)
        if(bonus_name_search(global_db, &block->bonus, "bonus4", block->ptr[0]))
            exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x10)
        if(bonus_name_search(global_db, &block->bonus, "bonus5", block->ptr[0]))
            exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));

    /* translate each argument individually 
        i = subscript the bonus type array
        j = offset and subscript the block argument array (ptr)
        j = 0; is the buff identifier
        j = 1; is where the arguments start
    */
    for(i = 0, j = 1; i < block->bonus.type_cnt; i++, j++) {
        switch(block->bonus.type[i]) {
            case 'n': block->result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);     break; /* Integer Value */
            case 'p': block->result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);     break; /* Integer Percentage */
            case 'r': translate_const(block, block->ptr[j], 0x01);                                                                              break; /* Race */
            case 'l': translate_const(block, block->ptr[j], 0x02);                                                                              break; /* Element */
            case 'w': translate_splash(block, block->ptr[j]);                                                                                   break; /* Splash */
            case 'z': block->eng_cnt++;                                                                                                         break; /* Meaningless */
            case 'e': translate_const(block, block->ptr[j], 0x04);                                                                              break; /* Effect */
            case 'q': block->result[i] = evaluate_expression(block, block->ptr[j], 100, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);   break; /* Integer Percentage / 100 */
            case 'k': translate_skill(block, block->ptr[j]);                                                                                    break; /* Skill */
            case 's': translate_const(block, block->ptr[j], 0x08);                                                                              break; /* Size */
            case 'c': translate_id(block, block->ptr[j], 0x01);                                                                                 break; /* Monster Class & Job ID * Monster ID */
            case 'o': block->result[i] = evaluate_expression(block, block->ptr[j], 10, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);    break; /* Integer Percentage / 10 */
            case 'm': translate_item(block, block->ptr[j]);                                                                                     break; /* Item ID */
            case 'x': block->result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);     break; /* Level */
            case 'g': translate_tbl(block, block->ptr[j], 0x01);                                                                                break; /* Regen */
            case 'a': block->result[i] = evaluate_expression(block, block->ptr[j], 1000, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);  break; /* Millisecond */
            case 'h': block->result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);     break; /* SP Gain Bool */
            case 'v': translate_tbl(block, block->ptr[j], 0x04);                                                                                break; /* Cast Self, Enemy */
            case 't': translate_trigger(block, block->ptr[j], 1);                                                                               break; /* Trigger */
            case 'y': translate_item(block, block->ptr[j]);                                                                                     break; /* Item Group */
            case 'd': translate_trigger(block, block->ptr[j], 2);                                                                               break; /* Triger ATF */
            case 'f': block->result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);     break; /* Cell */
            case 'b': translate_tbl(block, block->ptr[j], 0x08);                                                                                break; /* Flag Bitfield */
            case 'i': translate_tbl(block, block->ptr[j], 0x10);                                                                                break; /* Weapon Type */
            case 'j': translate_const(block, block->ptr[j], 0x10);                                                                              break; /* Class Group */
            default: break;
        }
    }

    /* the number of arguments must match the number of translations */
    if(block->ptr_cnt-1 != block->eng_cnt) 
        exit_buf("failed to translate block on item %d", block[0].item_id);
    return SCRIPT_PASSED;
}

int translate_const(block_r * block, char * expr, int flag) {
    int tbl_index = 0;
    char * tbl_str = NULL;
    char buffer[BUF_SIZE];

    const_t const_info;
    memset(&const_info, 0, sizeof(const_t));
    exit_null("null paramater", 2, block, expr);

    if(isdigit(expr[0])) {
        tbl_index = convert_integer(expr,10);
    } else {
        if(const_keyword_search(global_db, &const_info, expr, global_mode))
            exit_abt(build_buffer(global_err, "failed to search constant %s for %d", expr, block->item_id));
        tbl_index = const_info.value;
    }

    /* look up the translation from table */
    if(flag & 0x01)
        tbl_str = race_tbl(tbl_index);
    if(flag & 0x02)
        tbl_str = ele_tbl(tbl_index);
    if(flag & 0x04)
        tbl_str = eff_tbl(tbl_index);
    if(flag & 0x08)
        tbl_str = size_tbl(tbl_index);
    if(flag & 0x10) 
        tbl_str = class_tbl(tbl_index);
    if(flag & 0x20) {
        if(global_mode == EATHENA) {
            ea_item_group_search(global_db, tbl_index, global_mode, buffer);
            tbl_str = buffer;
        } else if(global_mode == RATHENA) {
            ra_item_group_search(global_db, tbl_index, global_mode, buffer);
            tbl_str = buffer;
        } else {
            tbl_str = itemgrp_tbl(tbl_index);
        }
    }
 
    /* indicate unresolve item group; there are many I am too lazy to add */
    if(strcmp(tbl_str,"error") == 0)
        printf("warning: failed to map constant %d in item %d on flag %d\n", tbl_index, block->item_id, flag);

    /* check for invalid index */
    if(tbl_str == NULL)
        exit_abt(build_buffer(global_err, "failed to resolve constant %s for %d", expr, block->item_id));
 
    /* write the tbl_str */
    translate_write(block, tbl_str, 0x01);
    if(const_info.name != NULL) free(const_info.name);
    return SCRIPT_PASSED;
}

int translate_skill(block_r * block, char * expr) {
    int offset = 0;
    char buf[BUF_SIZE];
    ic_skill_t skill_info;
    memset(&skill_info, 0, sizeof(ic_skill_t)); 
    exit_null("null paramater", 2, block, expr);

    if(((isdigit(expr[0]))) ?
        skill_name_search_id(global_db, &skill_info, convert_integer(expr, 10), global_mode):
        skill_name_search(global_db, &skill_info, expr, global_mode))
        exit_abt(build_buffer(global_err, "failed to resolve skill %s for %d", expr, block->item_id));

    /* write the skill name */
    offset = sprintf(buf, "%s", skill_info.desc);
    buf[offset] = '\0';
    translate_write(block, buf, 0x01);
    if(skill_info.name != NULL) free(skill_info.name);
    if(skill_info.desc != NULL) free(skill_info.desc);
    return SCRIPT_PASSED;
}

int translate_tbl(block_r * block, char * expr, int flag) {
    char * id_str = NULL;
    int id = 0;
    exit_null("null paramater", 2, block, expr);
    /* lookup the id */
    id = convert_integer(expr, 10);
    if(flag & 0x01)
        id_str = regen_tbl(id);
    if(flag & 0x02)
        id_str = spgd_tbl(id);
    if(flag & 0x04)
        id_str = castgt_tbl(id);
    if(flag & 0x08)
        id_str = flagtgt_tbl(id);
    if(flag & 0x10)
        id_str = weapon_tbl(id);
    /* check for invalid index */
    if(id_str == NULL || strcmp(id_str,"error") == 0)
        exit_abt(build_buffer(global_err, "failed to resolve constant %s for %d", expr, block->item_id));
    /* write the translation */
    translate_write(block, id_str, 0x01);
    return SCRIPT_PASSED;
}

int translate_splash(block_r * block, char * expr) {
   int splash = 0;
   char buf[256];
   exit_null("null paramater", 2, block, expr);
   /* Convert the splash to status; adding diagonal block on both side */
   splash = convert_integer(expr, 10);
   splash = (splash * 2) + 1;
   sprintf(buf,"%d x %d", splash, splash);
   translate_write(block, buf, 0x01);
   return SCRIPT_PASSED;
}

int translate_trigger(block_r * block, char * expr, int type) {
    int trigger = 0;
    node_t * result = NULL;
    char trigger_buf[BUF_SIZE];
    memset(trigger_buf, 0, BUF_SIZE);
    exit_null("null paramater", 2, block, expr);
    /* evalute the trigger to bit-mask value */
    result = evaluate_expression(block, expr, 0, EVALUATE_FLAG_KEEP_NODE);
    if(result == NULL) exit_abt("invalid result");

    trigger = result->max;
    node_free(result);
    switch(type) {
        case 0x01: 
            /* Meelee or range attack */
            if(trigger & BF_SHORT && trigger & BF_LONG) {
                strcat(trigger_buf," [Meelee Damage]");
                strcat(trigger_buf," [Range Damage]");
            } else if(trigger&BF_SHORT) {
                strcat(trigger_buf," [Meelee Damage]");
            } else if(trigger&BF_LONG) {
                strcat(trigger_buf," [Range Damage]");
            } else {
                /* default (nothing?) */
            }
            
            /* Skill attacks */
            if(trigger&BF_WEAPON || trigger&BF_MAGIC || trigger&BF_MISC) {
                if(trigger&BF_WEAPON) strcat(trigger_buf," [Weapon Skill]");
                if(trigger&BF_MAGIC) strcat(trigger_buf," [Magic Skill]");
                if(trigger&BF_MISC) strcat(trigger_buf," [Misc Skill]");
            } else {
                strcat(trigger_buf," [Weapon Skill]");
            }
            break;
        case 0x02: 
            /* Decide whether to inflict enemy (default) or self or both */
            if(trigger&ATF_SELF && trigger&ATF_TARGET) {
                strcat(trigger_buf," on self and enemy");
            } else if(trigger&ATF_SELF) {
                strcat(trigger_buf," on self.");
            } else {
                strcat(trigger_buf," on enemy.");
            }
            
            if(trigger&ATF_SHORT) {
                strcat(trigger_buf," [Meelee Damage]");
            } else if(trigger&ATF_LONG) {
                strcat(trigger_buf," [Range Damage]");
            }
            
            /* ATF_SKILL is not a very bitwise friendly hex, thus special case */
            if(trigger & ATF_SKILL) {
                strcat(trigger_buf," [Skill Damage]");
                trigger -= ATF_SKILL;
            }
            
            if(trigger&ATF_WEAPON || trigger&ATF_MAGIC || trigger&ATF_MISC) {
                if(trigger&ATF_WEAPON) strcat(trigger_buf," [Weapon Attack]");
                if(trigger&ATF_MAGIC) strcat(trigger_buf," [Magic Attack]");
                if(trigger&ATF_MISC) strcat(trigger_buf," [Misc Attack]");
            }
            break;
        default: exit_abt("invalid flag");
    }
    translate_write(block, trigger_buf, 0x01);
    return SCRIPT_PASSED;
}

int translate_time(block_r * block, char * expr) {
    int tick_min = 0;
    int tick_max = 0;
    char buf[BUF_SIZE];
    char aux[BUF_SIZE];
    int offset = 0;
    node_t * result = NULL;
    exit_null("null paramater", 2, block, expr);
    /* evalute the expression and convert to time string */
    result = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
    tick_min = minrange(result->range);
    tick_max = maxrange(result->range);
    if((tick_min / 86400000) > 1) {                      /* Convert seconds to days */
        tick_min /= 86400000;
        tick_max /= 86400000;
        offset = (tick_min == tick_max) ? 
                    sprintf(buf,"%d days",tick_min):
                    sprintf(buf,"%d ~ %d days", tick_min, tick_max);
    } else if((tick_min / 3600000) > 1) {                /* Convert seconds to hours */
        tick_min /= 3600000;
        tick_max /= 3600000;
        offset = (tick_min == tick_max) ? 
                    sprintf(buf,"%d hours",tick_min):
                    sprintf(buf,"%d ~ %d hours", tick_min, tick_max);
    } else if((tick_min / 60000) > 1) {                  /* Convert seconds to minutes */
        tick_min /= 60000;
        tick_max /= 60000;
        offset = (tick_min == tick_max) ? 
                    sprintf(buf,"%d minutes",tick_min):
                    sprintf(buf,"%d ~ %d minutes", tick_min, tick_max);
    } else {                                             /* Convert to seconds */
        tick_min /= 1000;
        tick_max /= 1000;
        offset = (tick_min == tick_max) ? 
                    sprintf(buf,"%d seconds",tick_min):
                    sprintf(buf,"%d ~ %d seconds", tick_min, tick_max);
    }
    buf[offset] = '\0';
    translate_write(block, formula(aux, buf, result), 1);
    if(result != NULL) node_free(result);
    return SCRIPT_PASSED;
}

int translate_id(block_r * block, char * expr, int flag) {
    int id = 0;
    pet_t pet;
    merc_t merc;
    ic_mob_t mob;
    ic_item_t item;
    memset(&pet, 0, sizeof(pet_t));
    memset(&merc, 0, sizeof(merc_t));
    memset(&mob, 0, sizeof(ic_mob_t));
    memset(&item, 0, sizeof(ic_item_t));
    exit_null("null paramater", 2, block, expr);
    id = convert_integer(expr, 10);

    if(flag & 0x01 && !mob_id_search(global_db, &mob, id, global_mode)) {
        translate_write(block, mob.iro, 0x01);
        if(mob.iro != NULL) free(mob.iro);
        return SCRIPT_PASSED;
    }

    if(flag & 0x02 && !item_name_id_search(global_db, &item, id, global_mode)) {
        translate_write(block, item.name, 0x01);
        if(item.name != NULL) free(item.name);
        if(item.script != NULL) free(item.script);
        return SCRIPT_PASSED;
    }

    if(flag & 0x04 && !merc_id_search(global_db, &merc, id, global_mode)) {
        translate_write(block, merc.name, 0x01);
        if(merc.name != NULL) free(merc.name);
        return SCRIPT_PASSED;
    }

    if(flag & 0x08 && !pet_id_search(global_db, &pet, id, global_mode)) {
        translate_write(block, pet.pet_name, 0x01);
        if(pet.pet_name != NULL) free(pet.pet_name);
        return SCRIPT_PASSED;
    }

    /* last resort const searching */
    if(translate_const(block, expr, 0xFF) == SCRIPT_PASSED)
        return SCRIPT_PASSED;

    exit_abt(build_buffer(global_err, "failed to resolve id %s for %d", expr, block->item_id));
    return SCRIPT_FAILED;
}

int translate_item(block_r * block, char * expr) {
    ic_item_t item;
    exit_null("null paramater", 2, block, expr);

    /* search the item id if literal */
    if(isdigit(expr[0]))
        return translate_id(block, expr, 0x02);
    /* search the const id if identifer */
    else if(isalpha(expr[0])) {
        memset(&item, 0, sizeof(ic_item_t));
        if(item_name_search(global_db, &item, expr, global_mode))
            return translate_const(block, expr, 0x20);
        translate_write(block, item.name, 0x01);
        if(item.name != NULL) free(item.name);
        if(item.script != NULL) free(item.script);
    } else {
        exit_abt(build_buffer(global_err, "failed to resolve item or const id %s for %d", expr, block->item_id));
    }
    return SCRIPT_PASSED;
}

int translate_autobonus(block_r * block, int flag) {
    int offset = 0;
    char buf[BUF_SIZE];
    char aux[BUF_SIZE];
    node_t * rate = NULL;
    exit_null("null paramater", 1, block);
 
    /* translate for autobonus and autobonus2 */
    if(flag & 0x01 || flag & 0x02) {
        /* translate the rate (out of 100%), duration (seconds), and flag (BF_TRIGGER) */
        rate = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
        translate_bonus_float_percentage(block, rate, &offset, 100);
        translate_time(block, block->ptr[2]);
        if(block->ptr_cnt > 3) translate_trigger(block, block->ptr[3], 1);
        offset = sprintf(buf,"Add %s chance to armor break for %s when attacked.%s",
            formula(aux, block->eng[1], rate), block->eng[3], (block->eng[4] != NULL)?block->eng[4]:"");
    }
 
    if(flag & 0x04) {
        /* translate the rate (out of 100%), duration (seconds), and skill */
        rate = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
        translate_bonus_float_percentage(block, rate, &offset, 100);
        translate_time(block, block->ptr[2]);
        if(block->ptr_cnt > 3) translate_skill(block, block->ptr[3]);
        offset = sprintf(buf,"Add %s chance to limit break for %s when using skill %s.",
            formula(aux, block->eng[1], rate), block->eng[3], (block->eng[4] != NULL)?block->eng[4]:"");
    }
    buf[offset] = '\0';
    
    translate_write(block, buf, 1);
    if(rate != NULL) node_free(rate);
    return SCRIPT_PASSED;
}

int translate_misc(block_r * block, char * expr) {
    int type;
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * result = NULL;
    
    if(ncs_strcmp(expr,"warp") == 0) {
        if(ncs_strcmp(block->ptr[0],"bif_fild01") == 0)
            offset = sprintf(buf,"Warp to Bifrost Field.");
        else if(ncs_strcmp(block->ptr[0],"malangdo") == 0)
            offset = sprintf(buf,"Warp to Malangdo.");
        else if(ncs_strcmp(block->ptr[0],"malaya") == 0)
            offset = sprintf(buf,"Warp to Malaya.");
        else if(ncs_strcmp(block->ptr[0],"pvp_n_room") == 0)
            offset = sprintf(buf,"Warp to PVP Room.");
        else if(ncs_strcmp(block->ptr[0],"yuno_fild09") == 0)
            offset = sprintf(buf,"Warp to Yuno Field 9.");
        else if(ncs_strcmp(block->ptr[0],"niflheim") == 0)
            offset = sprintf(buf,"Warp to Niflheim.");
        else {
            exit_buf("unsupported warp map %s in %d", block->ptr[0], block->item_id);
        }
        buf[offset] = '\0';
        translate_write(block, buf, 1);
    }
    
    if(ncs_strcmp(expr,"monster") == 0) {
        result = evaluate_expression(block, block->ptr[4], 1, EVALUATE_FLAG_KEEP_NODE);
        type = result->max;
        if(type < 0) type *= -1;
        if(type >= 1324 && type <= 1363)
            offset = sprintf(buf,"Summon a treasure box.");
        else
            switch(type) {
                case 1: offset = sprintf(buf,"Summon a random monster."); break;
                case 2: offset = sprintf(buf,"Summon a random poring monster."); break;
                case 3: offset = sprintf(buf,"Summon a random MVP monster."); break;
                case 4: offset = sprintf(buf,"Summon a random monster."); break;
                case 1904: offset = sprintf(buf,"Summon a bomb poring."); break;
                default: exit_buf("unsupported monster type %d in %d", type, block->item_id); break;
            }
        buf[offset] = '\0';
        translate_write(block, buf, 1);
    }

    if(ncs_strcmp(expr,"callfunc") == 0) {
        if(ncs_strcmp(block->ptr[0],"F_CashCity") == 0) {
            result = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE);
            type = result->max;
            switch(type) {
                case 1: offset = sprintf(buf,"Teleport to any town, city, etc."); break;
                case 2: offset = sprintf(buf,"Teleport to Juno, Lighthalzen, Einbroch, or Hugel."); break;
                case 3: offset = sprintf(buf,"Teleport to Rachel or Veins."); break;
                case 4: offset = sprintf(buf,"Teleport to Ayothaya, Amatsu, Louyang, or Kunlun."); break;
                case 5: offset = sprintf(buf,"Teleport to any town, city, etc."); break;
                default: break;
            }
        } else if(ncs_strcmp(block->ptr[0],"F_CashStore") == 0) {
            offset = sprintf(buf,"Emergency Kafra service dispatch.");  
        } else if(ncs_strcmp(block->ptr[0],"F_CashTele") == 0) {
            result = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE);
            type = result->max;
            switch(type) {
                case 1: offset = sprintf(buf,"Teleport to the Save Point, Prontera, Geffen, Al De Baran or Izlude."); break;
                case 2: offset = sprintf(buf,"Teleport to the Save Point, Payon, Alberta, Morocc or Comodo Island."); break;
                case 3: offset = sprintf(buf,"Teleport to the Save Point, Dragon City, Gonryun, Amatsu or Ayothaya."); break;
                case 4: offset = sprintf(buf,"Teleport to the Save Point, Lutie, Umbala, or Niflheim."); break;
                case 5: offset = sprintf(buf,"Teleport to the Save Point, Juno, Einbroch, Lighthalzen, or Hugel."); break;
                case 6: offset = sprintf(buf,"Teleport to the Save Point, Rachel, or Veins."); break;
                default: break;
            }
        } else if(ncs_strcmp(block->ptr[0],"F_CashDungeon") == 0) {
            offset = sprintf(buf,"Teleport directly to a dungeon of choice.");
        } else if(ncs_strcmp(block->ptr[0],"F_CashPartyCall") == 0) {
            offset = sprintf(buf,"Allows to teleport many people at once to random areas when used by party master. All the party members in the same map will move to the location where the party leader is sent.");
        } else if(ncs_strcmp(block->ptr[0],"F_CashReset") == 0) {
            offset = sprintf(buf,"Resets the Skill Tree and gives the corresponding number of Skill Points. This item can only be used in town, and the character must carry 0 weight, and can not be equipped with a Pushcart, Falcon, or PecoPeco. Can not be used by Novice Class.");
        } else if(ncs_strcmp(block->ptr[0],"F_Snowball") == 0) {
            offset = sprintf(buf,"Random effect of Restore, Endure, or Wing of Butterfly.");
        } else if(ncs_strcmp(block->ptr[0],"F_CashSeigeTele") == 0) {
            offset = sprintf(buf,"Teleport to a specific castle in any guild grounds.");
        } else if(ncs_strcmp(block->ptr[0],"F_Cat_Hard_Biscuit") == 0) {
            offset = sprintf(buf,"An unforgetable taste! May recover HP or SP.");
        } else if(ncs_strcmp(block->ptr[0],"F_Rice_Weevil_Bug") == 0) {
            offset = sprintf(buf,"An unforgetable taste! May recover HP or SP.");
        } else if(ncs_strcmp(block->ptr[0],"SetPalete") == 0) {
            offset = sprintf(buf,"Set palete to %s", block->ptr[1]);
        } else {
            exit_buf("unsupported callfunc function %s in item %d", block->ptr[0], block->item_id);
        }
        buf[offset] = '\0';
        translate_write(block, buf, 1);
    }
    if(result != NULL) node_free(result);
    return SCRIPT_PASSED;
}

int translate_produce(block_r * block, int handler) {
    int j;
    int item_lv = 0;
    int offset = 0;
    int recipe_offset = 1;
    ic_produce_t * produce = NULL;
    ic_produce_t * iter = NULL;
    char buf[BUF_SIZE];
    char num[BUF_SIZE];
    node_t * result = NULL;
    memset(buf,0,BUF_SIZE);
    memset(num,0,BUF_SIZE);
 
    /* evaluate the item level */
    result = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    item_lv = result->max;
 
    /* Write the header information */
    if(handler != 41) {
        switch(item_lv) {
            case 1: offset = sprintf(buf,"Use to craft level 1 weapons.\n"); break;                     /* lv1 weapons */
            case 2: offset = sprintf(buf,"Use to craft level 2 weapons.\n"); break;                     /* lv2 weapons */
            case 3: offset = sprintf(buf,"Use to craft level 3 weapons.\n"); break;                     /* lv3 weapons */
            case 11: offset = sprintf(buf,"Use to cook recipes with rank 5 success rate.\n"); break;    /* cooking sets */
            case 12: offset = sprintf(buf,"Use to cook recipes with rank 4 success rate.\n"); break;    
            case 13: offset = sprintf(buf,"Use to cook recipes with rank 3 success rate.\n"); break;    
            case 14: offset = sprintf(buf,"Use to cook recipes with rank 2 success rate.\n"); break;    
            case 15: offset = sprintf(buf,"Use to cook recipes with rank 1 success rate.\n"); break;    
            case 21: offset = sprintf(buf,"Use to manufacture metals.\n"); break;                       /* metals */
            default: return SCRIPT_FAILED;
        }
    } else {
       offset = sprintf(buf,"Use to make rune stones with additional %d%% success rate.\n",item_lv);
       item_lv = 24;
    }
 
    /* Write out all the recipes */
    prod_lv_search(global_db, &produce, item_lv, global_mode);
    iter = produce;
    while(iter != NULL) {
            sprintf(num,"%d",iter->item_id);
            translate_id(block, num, 0x02);
            offset += sprintf(buf + offset,"Recipe: %s\n", block->eng[recipe_offset++]);
            for(j = 0; j < iter->req_cnt; j++) {
                sprintf(num,"%d",iter->material[j]);
                translate_id(block, num, 0x02);
                offset += sprintf(buf + offset," - %s [%d]\n", block->eng[recipe_offset++], iter->amount[j]);
            }
        iter = iter->next;
    }
    free_prod(produce);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    if(result != NULL) node_free(result);
    return SCRIPT_PASSED;
}

int translate_status(block_r * block, int handler) {
    int i;
    char buf[BUF_SIZE];
    char aux[BUF_SIZE];
    int off = 0;
    char * item_script = NULL;

    ic_item_t item;
    status_t status;
    const_t const_info;
    node_t * result[BONUS_SIZE];
    node_t * rate = NULL;
    exit_null("block paramater", 1, block);
    memset(&item, 0, sizeof(ic_item_t));
    memset(&status, 0, sizeof(status_t));
    memset(&const_info, 0, sizeof(const_t));
    memset(&result, 0, sizeof(node_t *) * BONUS_SIZE);

    if(const_keyword_search(global_db, &const_info, block->ptr[0], global_mode))
        exit_buf("failed to search const %s in item %d", block->ptr[0], block->item_id);
    if(status_id_search(global_db, &status, const_info.value, block->ptr[0])) {
        if(global_mode == HERCULES) {
            /* Hercules decided to change the status constants by + or - 1; what the fuck? */
            if(status_id_search(global_db, &status, const_info.value - 1, block->ptr[0]))
                if(status_id_search(global_db, &status, const_info.value + 1, block->ptr[0]))
                    exit_buf("failed to search status id %d name %s in item %d", const_info.value, block->ptr[0], block->item_id);
        } else {
            exit_buf("failed to search status id %d name %s in item %d", const_info.value, block->ptr[0], block->item_id);
        }
    }
    /* evaluate the duration of the status */
    translate_time(block, block->ptr[1]);

    if(ncs_strcmp(status.scstr, "sc_itemscript") != 0) {
        /* evaluate the val1, val2, ... , valN */
        for(i = 0; i < status.vcnt; i++) {
            switch(status.vmod_ptr[i]) {
                case 'n': 
                        result[i] = evaluate_expression(block, block->ptr[2+i], 1, 
                        EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);  
                        break; /* Integer Value */
                case 'm': 
                        result[i] = evaluate_expression(block, block->ptr[2+i], 1, 
                        EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);  
                        break; /* Integer Value */
                case 'p': 
                        result[i] = evaluate_expression(block, block->ptr[2+i], 1, 
                        EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
                        break; /* Integer Percentage */
                case 'e': 
                        translate_const(block, block->ptr[2+i], 0x02);                                           
                        break; /* Effect */
                case 'u': 
                        evaluate_expression(block, block->ptr[2+i], -1, EVALUATE_FLAG_WRITE_FORMULA); 
                        break; /* Integer Value */
                default: exit_buf("invalid bonus type %d in item %d", status.vmod_ptr[i], block->item_id);
            }
        }

        /* I like things unnecessarily complicated =.=; this can be improved */
        switch(status.vcnt) {
            case 0: 
                off += sprintf(buf + off,"%s",status.scfmt);
                break;
            case 1: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff_ptr[0]], result[0], status.vmod_ptr[0], status.voff_ptr[0]));
                break;
            case 2: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff_ptr[0]], result[0], status.vmod_ptr[0], status.voff_ptr[0]), 
                status_formula(aux, block->eng[status.voff_ptr[1]], result[1], status.vmod_ptr[1], status.voff_ptr[1]));
                break;
            case 3: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff_ptr[0]], result[0], status.vmod_ptr[0], status.voff_ptr[0]), 
                status_formula(aux, block->eng[status.voff_ptr[1]], result[1], status.vmod_ptr[1], status.voff_ptr[1]), 
                status_formula(aux, block->eng[status.voff_ptr[2]], result[2], status.vmod_ptr[2], status.voff_ptr[2]));
                break;
            case 4: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff_ptr[0]], result[0], status.vmod_ptr[0], status.voff_ptr[0]), 
                status_formula(aux, block->eng[status.voff_ptr[1]], result[1], status.vmod_ptr[1], status.voff_ptr[1]), 
                status_formula(aux, block->eng[status.voff_ptr[2]], result[2], status.vmod_ptr[2], status.voff_ptr[2]), 
                status_formula(aux, block->eng[status.voff_ptr[3]], result[3], status.vmod_ptr[3], status.voff_ptr[3]));
                break;
            default: exit_abt("invalid status offset");
        }

        /* write the duration */
        off += sprintf(buf + off, " for %s",block->eng[1]);
        if(block->ptr_cnt == 5) {
            rate = evaluate_expression(block, block->ptr[3], 100, EVALUATE_FLAG_DEBUG_DUMP|EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA); /* rate */
            if(rate->min != 0 && rate->max != 0)
                off += sprintf(buf + off," with %s%% chance",block->eng[block->eng_cnt - 1]);
            node_free(rate);
        }
        off += sprintf(buf + off,".");
        buf[off] = '\0';
    } else {
        /* translate an item script within an item script */
        if(item_name_id_search(global_db, &item, convert_integer(block->ptr[2], 10), global_mode))
            exit_buf("failed to search item id %d in item %s (sc_itemscript)", block->item_id, block->ptr[2]);
        item_script = script_compile(item.script, item.id);
        off += sprintf(buf + off, "Status Effect [Duration: %s]\n%s", block->eng[1], item_script);
        buf[off] = '\0';
        free(item_script);
        if(item.name != NULL) free(item.name);
        if(item.script != NULL) free(item.script);
    }

    /* write the translation only the buffer is filled with something */
    if(off <= 0) exit_buf("failed to translate status %s of item %d", block->ptr[0], block->item_id);
    
    for(i = 0; i < BONUS_SIZE; i++)
        if(result[i] != NULL) node_free(result[i]);
    translate_write(block, buf, 1);
    if(const_info.name != NULL) free(const_info.name);
    if(status.scstr != NULL) free(status.scstr);
    if(status.scfmt != NULL) free(status.scfmt);
    if(status.scend != NULL) free(status.scend);
    if(status.vmod_ptr != NULL) free(status.vmod_ptr);
    if(status.voff_ptr != NULL) free(status.voff_ptr);
    return SCRIPT_PASSED;
}

char * status_formula(char * aux, char * eng, node_t * result, int type, int blank) {
    int offset = 0;
    if(blank < 0) return "";
    if(result == NULL) return eng;
    switch(type) {
        case 'n': check_node_range(result, aux); break;
        case 'p': check_node_range_precentage(result, aux); break;
        default: offset = sprintf(aux, "%s", eng); aux[offset] = '\0'; break;
    }
    return formula(aux, aux, result);
}

int translate_status_end(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    status_t status;
    const_t const_info;
    memset(&status, 0, sizeof(status_t));
    memset(&const_info, 0, sizeof(const_t));
    exit_null("block paramater", 1, block);
    if(const_keyword_search(global_db, &const_info, block->ptr[0], global_mode))
        exit_buf("failed to search const %s in item %d", block->ptr[0], block->item_id);
    if(status_id_search(global_db, &status, const_info.value, block->ptr[0]))
        exit_buf("failed to search status id %d name %s in item %d", const_info.value, block->ptr[0], block->item_id);
    translate_write(block, status.scend, 1);
    offset = sprintf(buf,"Cures %s status effect.", status.scend);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    if(const_info.name != NULL) free(const_info.name);
    if(status.scstr != NULL) free(status.scstr);
    if(status.scfmt != NULL) free(status.scfmt);
    if(status.scend != NULL) free(status.scend);
    if(status.vmod_ptr != NULL) free(status.vmod_ptr);
    if(status.voff_ptr != NULL) free(status.voff_ptr);
    return SCRIPT_PASSED;
}

int translate_bonus_script(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * duration = NULL;
    node_t * type = NULL;
    node_t * flag = NULL;
    exit_null("block is null", 1, block);
    duration = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE); /* duration (in seconds) */
    if(duration->min != duration->max) exit_buf("unsupported expression; duration dependence; %d item id.", block->item_id);
    offset += sprintf(buf + offset,"Unstackable bonus lasting for %d seconds ", duration->min);
    if(block->ptr_cnt >= 3) {
        type = evaluate_expression(block, block->ptr[2], 1, EVALUATE_FLAG_KEEP_NODE); /* flag */
        if(type->min != type->max) exit_buf("unsupported expression; type dependence; %d item id.", block->item_id);
        if(type->min > 0) {
            offset += sprintf(buf + offset, "[");
            if(type->min & 1) offset += sprintf(buf + offset, "%s", "Remove when dead");
            if(type->min - 1 > 0) offset += sprintf(buf + offset, " | ");
            if(type->min & 2) offset += sprintf(buf + offset, "%s", "Removable by dispell");
            if(type->min - 2 > 0) offset += sprintf(buf + offset, " | ");
            if(type->min & 4) offset += sprintf(buf + offset, "%s", "Removable by Clearance");
            if(type->min - 4 > 0) offset += sprintf(buf + offset, " | ");
            if(type->min & 8) offset += sprintf(buf + offset, "%s", "Remove when logout");
            offset += sprintf(buf + offset, "] ");
        }
    }
    if(block->ptr_cnt >= 4) {
        flag = evaluate_expression(block, block->ptr[3], 1, EVALUATE_FLAG_KEEP_NODE); /* type */
        if(flag->min != flag->max) exit_buf("unsupported expression; flag dependence; %d item id.", block->item_id);
        switch(flag->min) {
            case 1: offset += sprintf(buf + offset, "[Buff] "); break;
            case 2: offset += sprintf(buf + offset, "[Debuff] "); break;
            default: break;
        }
    }

    translate_write(block, buf, 1);
    if(duration != NULL) node_free(duration);
    if(type != NULL) node_free(type);
    if(flag != NULL) node_free(flag);
    return SCRIPT_PASSED;
}

int translate_setfalcon(block_r * block) {
    node_t * result = NULL;
    exit_null("block is null", 1, block);
    result = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(result->max == 0) {
        translate_write(block, "Recall a falcon.", 1);
    } else {
        translate_write(block, "Summon a falcon.", 1);
    }
    node_free(result);
    return SCRIPT_PASSED;
}

int translate_write(block_r * block, char * desc, int flag) {
    int length = 0;
    exit_null("null paramater", 2, block, desc);
    /* write the translation on the buffer and assign pointer to the start */
    block->eng[block->eng_cnt] = &block->arg[block->arg_cnt];
    /* select the proper format to write the argument */
    if(flag & 0x01) length = sprintf(&block->arg[block->arg_cnt],"%s",desc);
    /* check that the write was successful */
    if(length <= 0)
        exit_abt(build_buffer(global_err, "failed to write translation (%s) on item %d.\n", desc, block->item_id));
    /* extend the buffer to the next length */
    block->arg_cnt += length + 1;
    block->eng_cnt++;
    return SCRIPT_PASSED;
}

int translate_overwrite(block_r * block, char * desc, int position) {
    int length = 0;
    exit_null("null paramater", 2, block, desc);
    /* write the translation on the buffer and assign pointer to the start */
    block->eng[position] = &block->arg[block->arg_cnt];
    /* select the proper format to write the argument */
    length = sprintf(&block->arg[block->arg_cnt],"%s",desc);
    /* check that the write was successful */
    if(length <= 0)
        exit_abt(build_buffer(global_err, "failed to write translation (%s) on item %d.\n", desc, block->item_id));
    /* extend the buffer to the next length */
    block->arg_cnt += length + 1;
    return SCRIPT_PASSED;   
}

int formula_write(block_r * block, char * formula) {
    int len = 0;
    exit_null("block is null", 1, block);
    exit_null("formula is null", 1, formula);
    block->exp[block->exp_cnt] = &block->arg[block->arg_cnt];
    len = sprintf(&block->arg[block->arg_cnt],"%s", formula);
    if(len <= 0)
        exit_abt(build_buffer(global_err, "failed to write formula (%s) on item %d.\n", formula, block->item_id));
    block->arg_cnt += len + 1;
    block->exp_cnt++;
    return SCRIPT_PASSED;
}

int dep_write(dep_t * dep, char * vf) {
    int i = 0;
    int len = 0;
    exit_null("dep is null", 1, dep);
    exit_null("vf is null", 1, vf);

    for(i = 0; i < dep->buf_ptr_cnt; i++)
        if(ncs_strcmp(&dep->buf[dep->buf_ptr[i]], vf) == 0)
            return SCRIPT_FAILED;

    dep->buf_ptr[dep->buf_ptr_cnt] = dep->cnt;
    len = sprintf(&dep->buf[dep->cnt],"%s", vf);
    if(len <= 0)
        exit_abt(build_buffer(global_err, "failed to write variable or function node (%s).\n", vf));
    dep->cnt += len + 1;
    dep->buf_ptr_cnt++;
    return SCRIPT_PASSED;   
}

void block_debug_dump_all(block_r * block, int size, FILE * stm) {
   /* display block type and argument */
    int i, j;
    for(i = 0; i < size; i++) {
        fprintf(stm," --- %d ---\n", block[i].item_id);
        fprintf(stm,"Block Address: %p\n", (void *) &block[i]);
        fprintf(stm,"Block ID: %d\n", block[i].block_id);
        fprintf(stm,"Block Link: %d\n", block[i].link);
        fprintf(stm,"Block Type: %s\n", block[i].type->keyword);
        /* argument stack */
        fprintf(stm,"Block Argument Count: %d\n", block[i].ptr_cnt);
        for(j = 0; j < block[i].ptr_cnt; j++)
             fprintf(stm,"Block Argument %d: %s\n", j, block[i].ptr[j]);
        /* translation stack */
        fprintf(stm,"Block Translation Count: %d\n", block[i].eng_cnt);
        for(j = 0; j < block[i].eng_cnt; j++)
           fprintf(stm,"Block Translation %d: %s\n", j, block[i].eng[j]);
        /* formula stack */
        fprintf(stm,"Block Formula Count: %d\n", block[i].exp_cnt);
        for(j = 0; j < block[i].exp_cnt; j++)
           fprintf(stm,"Block Formula %d: %s\n", j, block[i].exp[j]);
        /* final description string */
        if(block[i].desc != NULL) fprintf(stm,"Translation: %s\n", block[i].desc);
        /* dump the buffer until the expected size */
        fprintf(stm,"Block Buffer Size: %d\n", block[i].arg_cnt);
        fprintf(stm,"Buffer: ");
        for(j = 0; j < 255; j++) {
            if(block[i].arg_cnt == j) {
                fprintf(stm,">%c<", block[i].arg[j]);
                break;
            } else
                fprintf(stm,"%c", block[i].arg[j]);
        }
        fprintf(stm,"%c\n", block[i].arg[j]);
        /* dump the set block linking */
        if(block[i].depd_cnt > 0) {
            fprintf(stm,"Block Dependence: ");
            block_debug_dump_set_link(block[i].depd, block[i].depd_cnt, block[i].block_id, 0, stm);
        } else {
            fprintf(stm,"Block Dependence: N/A\n");
        }
        /* dump the conditional logic tree */
        if(block[i].logic_tree != NULL) {
            fprintf(stm, "Block Logic Tree:\n");
            deepdmpnamerange(block[i].logic_tree, stm, "logic tree on stack");
        }
    }
}

void block_debug_dump_set_link(struct block_r ** block, int block_cnt, int block_id, int depth, FILE * stm) {
    int i = 0;
    int j = 0;
    fprintf(stm,"\n");
    if(block_cnt > 0) {
        for(i = 0; i < block_cnt; i++) {
            for(j = 0; j < depth; j++) fprintf(stm,"\t");
            fprintf(stm,"%d -> %d ", block_id, block[i]->block_id);
            block_debug_dump_set_link(block[i]->depd, block[i]->depd_cnt, block[i]->block_id, depth + 1,stm);
        }
    } else {
        for(j = 0; j < depth; j++) fprintf(stm,"\t");
        fprintf(stm,"%d ROOT\n", block_id);
    }
}

node_t * evaluate_expression(block_r * block, char * expr, int modifier, int flag) {
    int i = 0;
    int offset = 0;
    int length = 0;
    node_t * root_node;
    token_r expr_token;
    logic_node_t * temp = NULL;
    dep_t * set_dep;

    /* lexical analysis on expression and return failed if failed */
    if(script_lexical(&expr_token, expr) == SCRIPT_FAILED) return NULL;
    if(expr_token.script_cnt <= 0) return NULL;

    /* evaluate the expression */
    root_node = evaluate_expression_recursive(block, expr_token.script_ptr, 0, expr_token.script_cnt, block->logic_tree, flag);
    if(root_node == NULL) exit_buf("failed to evaluate %s in item %d", expr, block->item_id);

    /* write the result into argument translation */
    if(modifier != 0) {
        block->eng[block->eng_cnt] = &block->arg[block->arg_cnt];
        if(root_node->min == root_node->max) {
            if((root_node->min / modifier) == 0 && root_node->min > 0)
                length = sprintf(&block->arg[block->arg_cnt],"%.2f",((double)(root_node->min) / modifier));
            else
                length = sprintf(&block->arg[block->arg_cnt],"%d",(root_node->min) / modifier);
        } else {
            if(((root_node->min / modifier) == 0 && root_node->min > 0) || 
               ((root_node->max / modifier) == 0 && root_node->max > 0))
                length = sprintf(&block->arg[block->arg_cnt],"%.2f ~ %.2f",
                    ((double)(root_node->min) / modifier), ((double)(root_node->max) / modifier));
            else
                length = sprintf(&block->arg[block->arg_cnt],"%d ~ %d",(root_node->min) / modifier, (root_node->max) / modifier);
        }
        if(length <= 0) return NULL;

        /* write the modifier in to the expression */
        if(EVALUATE_FLAG_WRITE_FORMULA & flag && modifier > 1 && root_node->cond_cnt > 0) {
            root_node->expr_cnt = sprintf(root_node->expr,"%s / %d", root_node->expr, modifier);
            root_node->expr[root_node->expr_cnt] = '\0';
        }

        /* extend the buffer to the next length */
        block->arg_cnt += length + 1;
        block->eng_cnt++;
    }

    /* save the rootnode of the set block */
    if(block->type->id == 28) {
        block->set_min = root_node->min;
        block->set_max = root_node->max;
        block->set_cond_cnt = root_node->cond_cnt;
        if(root_node->cond_cnt > 0) {
            offset = sprintf(block->set_expr,"%s", root_node->expr);
            block->set_expr[offset] = '\0';
        }
        if(root_node->dep != NULL && root_node->dep->buf_ptr_cnt > 0) {
            set_dep = calloc(1, sizeof(dep_t));
            for(i = 0; i < root_node->dep->buf_ptr_cnt; i++)
                dep_write(set_dep, &root_node->dep->buf[root_node->dep->buf_ptr[i]]);
            block->set_dep = set_dep;
        }
        block->set_complexity = root_node->complexity;
    }

    /* simplified all complex expression to variables */
    if(root_node->complexity > 0 && !(flag & EVALUATE_FLAG_EXPR_BOOL)) {
        /* this could definitely look cleaner; oh well 
         * simply lists all the variable and functions */
        root_node->expr_cnt = sprintf(root_node->expr,"based on %s", 
            &root_node->dep->buf[root_node->dep->buf_ptr[0]]);
        root_node->expr[root_node->expr_cnt++] = '\0';
        for(i = 1; i < root_node->dep->buf_ptr_cnt - 1; i++) {
            root_node->expr_cnt += sprintf(&root_node->expr[root_node->expr_cnt], 
                ", %s", &root_node->dep->buf[root_node->dep->buf_ptr[i]]);
            root_node->expr[root_node->expr_cnt] = '\0';
        }
        if(i < root_node->dep->buf_ptr_cnt) {
            root_node->expr_cnt += sprintf(&root_node->expr[root_node->expr_cnt], 
                ", %s", &root_node->dep->buf[root_node->dep->buf_ptr[i]]);
            root_node->expr[root_node->expr_cnt] = '\0';
        }
    }

    /* write the formula into the block and only write formula if dependencies exist */
    if(flag & EVALUATE_FLAG_WRITE_FORMULA && root_node->cond_cnt > 0) {
        formula_write(block, root_node->expr);
        /*printf("[%d] %s\n", root_node->cond_cnt, root_node->expr);*/
    }

    /* keep a copy of the logic tree and free if not */
    if(flag & EVALUATE_FLAG_KEEP_LOGIC_TREE)
        if(root_node->cond != NULL) {
            if(block->logic_tree == NULL) {
                block->logic_tree = copy_deep_any_tree(root_node->cond);
            } else {
                /* add the new tree to the stack */
                temp = block->logic_tree;
                block->logic_tree = copy_deep_any_tree(root_node->cond);
                block->logic_tree->stack = temp;
            }
        }

    /* all expressions should return a root node */
    if(root_node == NULL) exit_buf("failed to evaluate expression %s in item %d", expr, block->item_id);

    /* free the root node */
    if(!(flag & EVALUATE_FLAG_KEEP_NODE)) {
        node_free(root_node);
        root_node = NULL;
    }

    return root_node;
}

node_t * evaluate_expression_recursive(block_r * block, char ** expr, int start, int end, logic_node_t * logic_tree, int flag) {
    int i = 0;
    int j = 0;
    int k = 0;
    int len = 0;
    int temp_int = 0;

    int expr_inx_open = 0;
    int expr_inx_close = 0;
    int expr_sub_level = 0;

    int op_cnt = 0;
    const_t const_info;
    var_t var_info;
    dep_t * dep = calloc(1, sizeof(dep_t));
    dep_t * set_dep = NULL;
    range_t * temp_range = NULL;
    range_t * temp_range2 = NULL;
    /* operator precedence tree */
    int op_pred[PRED_LEVEL_MAX][PRED_LEVEL_PER] = {
        {'*'          ,'/'          ,'\0' ,'\0'         ,'\0'},
        {'+'          ,'-'          ,'\0' ,'\0'         ,'\0'},
        {'<'          ,'<' + '='    ,'>'  ,'>' + '='    ,'\0'},
        {'&'          ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'|'          ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'=' + '='    ,'!' + '='    ,'\0' ,'\0'         ,'\0'},
        {'&' + '&'    ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'|' + '|'    ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {':'          ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'?'          ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'='          ,'\0'         ,'\0' ,'\0'         ,'\0'}
    };

    node_t * root_node = NULL;
    node_t * iter_node = NULL;
    node_t * temp_node = NULL;
    
    /* initialize the root node */
    root_node = calloc(1, sizeof(node_t));
    iter_node = root_node;

    for(i = start; i < end; i++) {
        /* handle sub-expression by calling evaluate_expression_recursive */
        if(expr[i][0] == '(') {
            expr_inx_open = 0;
            expr_inx_close = 0;
            expr_sub_level = 0;

            for(expr_inx_open = i; i < end; i++) {
                if(expr[i][0] == '(') expr_sub_level++;
                if(expr[i][0] == ')') expr_sub_level--;
                if(!expr_sub_level && expr[i][0] == ')') {
                    expr_inx_close = i;
                    break;
                }
            }

            if(!expr_inx_close || expr_sub_level)
                exit_buf("failed interpreting item %d; unmatch parentheses.\n", block->item_id);
            /* ? operator require setting the EVALUATE_FLAG_EXPR_BOOL */            
            if(i + 1 < end && expr[i+1][0] == '?')
                temp_node = evaluate_expression_recursive(block, expr, expr_inx_open + 1, expr_inx_close, logic_tree, flag | EVALUATE_FLAG_EXPR_BOOL);
            else
                temp_node = evaluate_expression_recursive(block, expr, expr_inx_open + 1, expr_inx_close, logic_tree, flag);
            temp_node->type = NODE_TYPE_SUB;    /* subexpression value is now a special operand */
            op_cnt++;
            for(j = 0; j < temp_node->dep->buf_ptr_cnt; j++)
                dep_write(dep, &temp_node->dep->buf[temp_node->dep->buf_ptr[j]]);
            root_node->complexity += temp_node->complexity;
            free(temp_node->dep);
        /* handle single or dual operators and prefix operators */
        } else if(ATHENA_SCRIPT_OPERATOR(expr[i][0])) {
            /* create an operator node */
            temp_node = calloc(1, sizeof(node_t));
            temp_node->op = expr[i][0];
            if(op_cnt) {
                /* dual operators detected merge the operators to one */
                if((i + 1) < end && ATHENA_SCRIPT_OPERATOR(expr[i + 1][0]) && !ATHENA_SCRIPT_UNARY(expr[i + 1][0])) {
                    temp_node->op += expr[i + 1][0];
                    i++;
                }
                op_cnt = 0;
                temp_node->type = NODE_TYPE_OPERATOR;
            } else if(!op_cnt) {
                /* detect only one operand, assume prefix next expression */
                temp_node->type = NODE_TYPE_UNARY;
            }

        /* handle numeric literal operand */
        } else if(isdigit(expr[i][0])) {
            len = strlen(expr[i]);

            /* check if entirely numeric */
            for(j = 0; j < len; j++)
                if(!isdigit(expr[i][j]))
                    exit_buf("failed interpreting item %d; invalid operand %s.\n", block->item_id, expr[i]);

            /* create an operand node */
            temp_node = calloc(1, sizeof(node_t));
            temp_node->type = NODE_TYPE_OPERAND;
            temp_node->min = temp_node->max = convert_integer(expr[i],10);
            op_cnt++;
        /* handler constant, variable, or function call */
        } else if(ATHENA_SCRIPT_IDENTIFER(expr[i][0]) || ATHENA_SCRIPT_SYMBOL(expr[i][0])) {
            temp_node = calloc(1, sizeof(node_t));
            temp_node->id = expr[i];
            
            /* handle variable or function */
            memset(&const_info, 0, sizeof(const_t));
            memset(&var_info, 0, sizeof(var_t));
            if(!var_keyword_search(global_db, &var_info, expr[i])) {
                temp_node->cond_cnt = 1;
                temp_node->var_id = var_info.tag;

                /* handle function call */
                if(var_info.type & TYPE_FUNC) {
                    expr_inx_open = 0;
                    expr_inx_close = 0;
                    expr_sub_level = 0;

                    /* unique identifiers must be constructed when matching functions
                     * that have paramaters that modified their meaning; such as 
                     * readparam(bStr) and readparam(bAgi), but since only 2 matters,
                     * I hard code them here. 
                     * ACTUALLY, some function calls modifies the id in evaluate_function,
                     * check out var_db table. The implementation is currently all over the place. */
                    if(ncs_strcmp(expr[i],"readparam") == 0 ||
                        ncs_strcmp(expr[i],"getskilllv") == 0 ||
                        ncs_strcmp(expr[i],"checkoption") == 0)
                        temp_node->id = expr[i+2];

                    for(expr_inx_open = i; i < end; i++) {
                        if(expr[i][0] == '(') expr_sub_level++;
                        if(expr[i][0] == ')') expr_sub_level--;
                        if(!expr_sub_level && expr[i][0] == ')') {
                            expr_inx_close = i;
                            break;
                        }
                    }

                    if(!expr_inx_close || expr_sub_level)
                        exit_buf("failed interpreting item %d; unmatch parentheses.\n", block->item_id);

                    /* create function node */
                    temp_node->type = NODE_TYPE_FUNCTION;

                    /* static function value */
                    if(var_info.fflag & FUNC_DIRECT) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                    /* dynamic function value */
                    } else if(var_info.fflag & FUNC_PARAMS) {
                        temp_node->dep = dep;
                        evaluate_function(block, expr, expr[expr_inx_open], expr_inx_open + 2, expr_inx_close,
                                                &(temp_node->min), &(temp_node->max), temp_node);
                        root_node->complexity++;
                    }

                /* handle variable */
                } else if(var_info.type & TYPE_VARI) {
                    /* create function node */
                    temp_node->type = NODE_TYPE_VARIABLE;

                    /* static variable value */
                    if(var_info.vflag & VARI_DIRECT) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                    }
                }

                if(var_info.id != NULL) free(var_info.id);
                if(var_info.str != NULL) free(var_info.str);
            /* handle constant */
            } else if(!const_keyword_search(global_db, &const_info, expr[i], global_mode)) {
                /* create constant node */
                temp_node->type = NODE_TYPE_CONSTANT;
                temp_node->min = temp_node->max = const_info.value;
                if(const_info.name != NULL) free(const_info.name);
            /* handle set'd variable */
            } else {
                /* create set block node */
                temp_node->type = NODE_TYPE_LOCAL; /* assume string literal unless resolved in for loop */
                temp_node->min = temp_node->max = 0;
                set_dep = NULL;
                /* loop through all the dependencies and apply the last matching min and max */
                for(j = 0; j < block->depd_cnt; j++) {
                    if(strcmp(block->depd[j]->ptr[0], expr[i]) == 0) {
                        temp_node->min = block->depd[j]->set_min;
                        temp_node->max = block->depd[j]->set_max;
                        temp_node->cond_cnt = block->depd[j]->set_cond_cnt;
                        temp_int = block->depd[j]->set_complexity;
                        if(temp_node->cond_cnt > 0)
                            temp_node->expr_cnt = sprintf(temp_node->expr,"%s",block->depd[j]->set_expr);
                        if(block->depd[j]->logic_tree != NULL) {
                            if(temp_node != NULL) freenamerange(temp_node->cond);
                            temp_node->cond = copy_any_tree(block->depd[j]->logic_tree);
                        }
                        set_dep = block->depd[j]->set_dep;
                        /* aggregate all the possible set values */
                        if(temp_node->range == NULL) {
                            temp_node->range = mkrange(INIT_OPERATOR, temp_node->min, temp_node->max, DONT_CARE);
                        } else {
                            temp_range = mkrange(INIT_OPERATOR, temp_node->min, temp_node->max, DONT_CARE);
                            temp_range2 = temp_node->range;
                            temp_node->range = orrange(temp_range, temp_range2);
                            freerange(temp_range);
                            freerange(temp_range2);
                            temp_range = NULL;
                            temp_range2 = NULL;
                            temp_node->min = minrange(temp_node->range);
                            temp_node->max = maxrange(temp_node->range);
                        }
                    }
                }
                /* retrieve the dependencies */
                if(set_dep != NULL)
                    for(k = 0; k < set_dep->buf_ptr_cnt; k++)
                        dep_write(dep, &set_dep->buf[set_dep->buf_ptr[k]]);

                /* inherit the complexity count */
                root_node->complexity += temp_int;
            }
            op_cnt++;
        
        /* catch everything else */
        } else {
            exit_buf("failed interpreting item %d; invalid token %s.\n", block->item_id, expr[i]);
        }

        if(op_cnt > 1)
            exit_buf("failed interpreting item %d; operator without operand.\n", block->item_id, expr[i]);

        if(temp_node != NULL) {
            /* generate range for everything but subexpression and operators */
            if(temp_node->type & 0x3e && temp_node->range == NULL)
                temp_node->range = mkrange(INIT_OPERATOR, temp_node->min, temp_node->max, DONT_CARE);

            temp_node->dep = dep;
            /* add node to linked list */
            iter_node->next = temp_node;
            iter_node->list = temp_node;
            temp_node->prev = iter_node;
            iter_node = temp_node;
            temp_node = NULL;
        }
    }

    /* complete the head doubly linked list */
    root_node->prev = iter_node;
    iter_node->next = root_node;
    iter_node->list = root_node;

    /* check that at least one node was added */
    if(root_node == iter_node)
        exit_buf("failed interpreting item %d; detected zero nodes.\n", block->item_id, expr[i]);

    /* check that the list is completely linked */
    iter_node = root_node;
    do {
        if(iter_node->next == NULL || iter_node->prev == NULL)
            exit_buf("failed interpreting item %d; invalid node linking.\n", block->item_id, expr[i]);
        iter_node = iter_node->next;
    } while(iter_node != root_node);

    /* structre the tree unary operators */
    iter_node = root_node;
    do {
        /* assume all unary operator have same precedences */
        if(iter_node->type == NODE_TYPE_UNARY) {
            /* link to parent */
            iter_node->next->parent = iter_node;

            /* unary operator (forward) expansion */
            iter_node->left = iter_node->next;
            iter_node->right = iter_node->next;
            iter_node->next->next->prev = iter_node;
            iter_node->next = iter_node->next->next;
        }
        iter_node = iter_node->next;
    } while(iter_node != root_node);

    /* structure the tree binary operators*/
    for(i = 0; i < PRED_LEVEL_MAX; i++) {
        iter_node = root_node;
        do {
            if(iter_node->type == NODE_TYPE_OPERATOR) {
                for(j = 0; j < PRED_LEVEL_PER; j++) {
                    if(iter_node->op == op_pred[i][j] && op_pred[i][j] != '\0') {
                        /* link to parent */
                        iter_node->prev->parent = iter_node;
                        iter_node->next->parent = iter_node;

                        /* binary operator vertical expansion */
                        iter_node->left = iter_node->prev;
                        iter_node->right = iter_node->next;
                        iter_node->prev->prev->next = iter_node;
                        iter_node->next->next->prev = iter_node;
                        iter_node->next = iter_node->next->next;
                        iter_node->prev = iter_node->prev->prev;
                    }
                }
            }
            iter_node = iter_node->next;
        } while(iter_node != root_node);
    }

    /* evaluate the infix expression with pre-order traversal */
    evaluate_node(root_node->next, node_dbg, logic_tree, flag, &root_node->complexity);

    /* setup the subexpression or expression node */
    strncpy(root_node->expr, root_node->next->expr, strlen(root_node->next->expr));
    root_node->min = root_node->next->min;
    root_node->max = root_node->next->max;
    root_node->range = copyrange(root_node->next->range);
    root_node->cond_cnt = root_node->next->cond_cnt;
    root_node->dep = dep;
    
    /* simple function and variable parenthesis is meaningless, i.e. (getrefine()) */
    if(root_node->next->type == NODE_TYPE_OPERATOR)
        root_node->type = NODE_TYPE_SUB;
    else
        root_node->type = root_node->next->type;

    /* copy the logical tree if it exist */
    if(root_node->cond == NULL && root_node->next->cond != NULL) 
        root_node->cond = copy_any_tree(root_node->next->cond);
    
    /* free all the other nodes beside the root */
    iter_node = root_node->list;
    do {
        if(iter_node != root_node) {
            temp_node = iter_node;
            iter_node = iter_node->list;
            freenamerange(temp_node->cond);
            freerange(temp_node->range);
            free(temp_node);
        }
    } while(iter_node != root_node);
    return root_node;
}

int evaluate_function(block_r * block, char ** expr, char * func, int start, int end, int * min, int * max, node_t * node) {
    int i = 0;
    int index = 0;

    /* parsing variables */
    int subexpr_level = 0;
    /*char buffer[BUF_SIZE];*/

    /* operand variables */
    node_t * resultOne = NULL;
    node_t * resultTwo = NULL;

    /* token variable to handle special argument */
    token_r token;
    ic_skill_t skill;
    ic_item_t item;
    const_t const_info;
    ea_item_group_t ea_item_group;
    memset(&skill, 0, sizeof(ic_skill_t));
    memset(&item, 0, sizeof(ic_item_t));
    memset(&const_info, 0, sizeof(const_t));
    memset(&ea_item_group, 0, sizeof(ea_item_group_t));

    if(ncs_strcmp(func,"getskilllv") == 0) {
        /* search by skill id or skill constant for max level */
        if(skill_name_search(global_db, &skill, expr[start], global_mode) && isdigit(expr[start][0]))
            if(skill_name_search_id(global_db, &skill, convert_integer(expr[start], 10), global_mode))
                exit_abt(build_buffer(global_err, "failed to search for skill %s in %d", expr[start], block->item_id));

        /* write the skill name */
        node->expr_cnt = sprintf(node->expr,"%s Level", skill.desc);
        node->expr[node->expr_cnt] = '\0';

        /* skill level minimum is 0 (not learnt) or maximum level */
        *min = 0;
        *max = skill.max;
    } else if(ncs_strcmp(func,"pow") == 0) {
        /* figure out the top level comma position */
        for(i = start; i < end; i++) {
            switch(expr[i][0]) {
                case '(': subexpr_level++; break;
                case ')': subexpr_level--; break;
                default: break;
            }
            /* found the top level comma position */
            if(!subexpr_level && expr[i][0] == ',') break;
        }

        /* maths; evaluate operand one and two, pow them both.*/
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        *min = (int) pow(resultOne->min, resultTwo->min);
        *max = (int) pow(resultOne->max, resultTwo->max);

        node->expr_cnt = sprintf(node->expr,"(%s)^%s",resultOne->expr, resultTwo->expr);
        node->expr[node->expr_cnt] = '\0';
    } else if(ncs_strcmp(func,"min") == 0) {
        /* figure out the top level comma position */
        for(i = start; i < end; i++) {
            switch(expr[i][0]) {
                case '(': subexpr_level++; break;
                case ')': subexpr_level--; break;
                default: break;
            }
            /* found the top level comma position */
            if(!subexpr_level && expr[i][0] == ',') break;
        }

        /* maths; evaluate operand one and two, pow them both.*/
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        *min = (int) (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
        *max = (int) (resultOne->max < resultTwo->max)?resultOne->max:resultTwo->max;
        node->expr_cnt = sprintf(node->expr,"Minimum value of (%s) and (%s)",resultOne->expr, resultTwo->expr);
        node->expr[node->expr_cnt] = '\0';
    } else if(ncs_strcmp(func,"max") == 0) {
        /* figure out the top level comma position */
        for(i = start; i < end; i++) {
            switch(expr[i][0]) {
                case '(': subexpr_level++; break;
                case ')': subexpr_level--; break;
                default: break;
            }
            /* found the top level comma position */
            if(!subexpr_level && expr[i][0] == ',') break;
        }

        /* maths; evaluate operand one and two, pow them both.*/
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        *min = (int) (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
        *max = (int) (resultOne->max < resultTwo->max)?resultOne->max:resultTwo->max;
        node->expr_cnt = sprintf(node->expr,"Maximum value of (%s) and (%s)",resultOne->expr, resultTwo->expr);
        node->expr[node->expr_cnt] = '\0';
    } else if(ncs_strcmp(func,"rand") == 0) {
        /* figure out the top level comma position */
        for(i = start; i < end; i++) {
            switch(expr[i][0]) {
                case '(': subexpr_level++; break;
                case ')': subexpr_level--; break;
                default: break;
            }
            /* found the top level comma position */
            if(!subexpr_level && expr[i][0] == ',') break;
        }

        /* maths; evaluate operand one and two, pow them both.*/
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        if(i + 1 < end) {
            resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
            *min = (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
            *max = (resultOne->max > resultTwo->max)?resultOne->max:resultTwo->max;
            node->expr_cnt = sprintf(node->expr,"random %s ~ %s",resultOne->expr, resultTwo->expr);
            node->expr[node->expr_cnt] = '\0';
        } else {
            *min = resultOne->min;
            *max = resultOne->max;
            node->expr_cnt = sprintf(node->expr,"random %s",resultOne->expr);
            node->expr[node->expr_cnt] = '\0';
        }
    /* callfunc calls global functions; just enumerate .. */
    } else if(ncs_strcmp(func,"callfunc") == 0) {
        if(ncs_strcmp(expr[start], "F_Rand") == 0) {
            /* indicate multiset */
            block->flag = 0x01 | 0x02;
            block->offset = block->ptr_cnt;

            /* load the extended series onto the special buffer */
            for(i = 0; i < end + 1; i++) /* +1 include ) for parsing below */
                token.script_ptr[i] = expr[i];
            token.script_cnt = end + 1;

            /* parse the callfunc paramaters */
            script_parse(&token, &start, block, ',',')',4);
            block->ptr_cnt--;

            *min = 0;
            *max = 0;
        } else if(ncs_strcmp(expr[start], "F_RandMes") == 0) {
            /* indicate multiset */
            block->flag = 0x01 | 0x02;
            block->offset = block->ptr_cnt + 1;

            /* load the extended series onto the special buffer */
            for(i = 0; i < end + 1; i++) /* +1 include ) for parsing below */
                token.script_ptr[i] = expr[i];
            token.script_cnt = end + 1;

            /* parse the callfunc paramaters */
            script_parse(&token, &start, block, ',',')',4);
            block->ptr_cnt--;

            *min = 0;
            *max = 0;
        }
    } else if(ncs_strcmp(func,"strcharinfo") == 0) {
        /* note; hercules uses a set of constants named PC_*; might need to resolve to integer via const_db. */
        resultOne = evaluate_expression(block, expr[start], 1, EVALUATE_FLAG_KEEP_NODE);
        if(resultOne->min != resultOne->max) {
            fprintf(stderr,"warning[%d]: evaluate_function; strcharinfo must evaluate to constant integer; ", block->item_id);
            dmprange(resultOne->range, stderr, "warning: evaluate_function; ");
            exit(EXIT_FAILURE);
        }
        switch(resultOne->min) {
            case 0: argument_write(node, "Character"); break;
            case 1: argument_write(node, "Party"); break;
            case 2: argument_write(node, "Guild"); break;
            case 3: argument_write(node, "Map"); break;
            default: 
                fprintf(stderr, "warning[%d]: evaluate_function; strcharinfo; invalid constant %d\n", block->item_id, resultOne->min);
                exit(EXIT_FAILURE);
        }
    } else if(ncs_strcmp(func,"countitem") == 0) {
        /* note; countitem also accept item names, but this is not common; might need to resolve to via item_db or use as is */
        resultOne = evaluate_expression(block, expr[start], 1, EVALUATE_FLAG_KEEP_NODE);
        if(resultOne->min != resultOne->max) {
            fprintf(stderr,"warning[%d]: evaluate_function; countitem must evaluate to constant integer; ", block->item_id);
            dmprange(resultOne->range, stderr, "warning: evaluate_function; ");
            exit(EXIT_FAILURE);
        }

        if(item_name_id_search(global_db, &item, resultOne->min, global_mode))
            exit_abt(build_buffer(global_err, "failed to search for item %d in %d", resultOne->min, block->item_id));
        argument_write(node, item.name);
        *min = 0;
        *max = 2147483647;
    } else if(ncs_strcmp(func,"gettime") == 0) {
        resultOne = evaluate_expression(block, expr[start], 1, EVALUATE_FLAG_KEEP_NODE);
        if(resultOne->min != resultOne->max) {
            fprintf(stderr,"warning[%d]: evaluate_function; gettime must evaluate to constant integer; ", block->item_id);
            dmprange(resultOne->range, stderr, "warning: evaluate_function; ");
            exit(EXIT_FAILURE);
        }
        switch(resultOne->min) {
            case 1: argument_write(node, "Seconds"); node->id = "gettime_second"; break;
            case 2: argument_write(node, "Minutes"); node->id = "gettime_minute"; break;
            case 3: argument_write(node, "Hours"); node->id = "gettime_hour"; break;
            case 4: argument_write(node, "Weeks"); node->id = "gettime_week"; break;
            case 5: argument_write(node, "Day"); node->id = "gettime_day"; break;
            case 6: argument_write(node, "Month"); node->id = "gettime_month"; break;
            case 7: argument_write(node, "Year"); node->id = "gettime_year"; break;
            case 8: argument_write(node, "Day of Year"); node->id = "gettime_dayyear"; break;
            default: 
                fprintf(stderr, "warning[%d]: evaluate_function; gettime; invalid constant %d\n", block->item_id, resultOne->min);
                exit(EXIT_FAILURE);
                break;
        }
        *min = 0;
        *max = 2147483647;
    } else if(ncs_strcmp(func,"getiteminfo") == 0) {
        for(i = start; i < end; i++)
            if(expr[i][0] == ',')
                index = i;

        resultOne = evaluate_expression_recursive(block, expr, start, index, block->logic_tree, 0);
        resultTwo = evaluate_expression_recursive(block, expr, index + 1, index + 2, block->logic_tree, 0);
        argument_write(node, resultOne->cond->args);
        switch(resultTwo->min) {
            case 0: argument_write(node, "Buy Price");
            case 1: argument_write(node, "Sell Price");
            case 2: argument_write(node, "Item Type");
            case 3: argument_write(node, "Max Drop Chance");
            case 4: argument_write(node, "Sex");
            case 5: argument_write(node, "Equip");
            case 6: argument_write(node, "Weight");
            case 7: argument_write(node, "Attack");
            case 8: argument_write(node, "Defense");
            case 9: argument_write(node, "Range");
            case 10: argument_write(node, "Slot");
            case 11: argument_write(node, "Weapon Type");
            case 12: argument_write(node, "Requirement Level");
            case 13: argument_write(node, "Weapon Level");
            case 14: argument_write(node, "View ID");
            case 15: argument_write(node, "Magical Attack");
            default: break;
        }

        *min = 0;
        *max = 2147483647;
    } else if(ncs_strcmp(func,"getequipid") == 0) {
        /* note; getequipid also accept integers; might need to resolve to via item_db or use as is */
        if(const_keyword_search(global_db, &const_info, expr[start], global_mode))
            exit_abt(build_buffer(global_err, "failed to search for const %s in %d", expr[start], block->item_id));
        switch(const_info.value) {
            case 1: argument_write(node, "Upper Headgear"); break;
            case 2: argument_write(node, "Armor"); break;
            case 3: argument_write(node, "Left Hand"); break;
            case 4: argument_write(node, "Right Hand"); break;
            case 5: argument_write(node, "Garment"); break;
            case 6: argument_write(node, "Shoes"); break;
            case 7: argument_write(node, "Left Accessory"); break;
            case 8: argument_write(node, "Right Accessory"); break;
            case 9: argument_write(node, "Middle Headgear"); break;
            case 10: argument_write(node, "Lower Headgear"); break;
            case 11: argument_write(node, "Lower Costume Headgear"); break;
            case 12: argument_write(node, "Middle Costume Headgear"); break;
            case 13: argument_write(node, "Upper Costume Headgear"); break;
            case 14: argument_write(node, "Costume Garment"); break;
            case 15: argument_write(node, "Shadow Armor"); break;
            case 16: argument_write(node, "Shadow Weapon"); break;
            case 17: argument_write(node, "Shadow Shield"); break;
            case 18: argument_write(node, "Shadow Shoes"); break;
            case 19: argument_write(node, "Shadow Left Accessory"); break;
            case 20: argument_write(node, "Shadow Right Accessory"); break;
        }
        *min = 0;
        *max = 2147483647;
    } else if(ncs_strcmp(func,"isequipped") == 0) {
        for(i = start; i < end; i++) {
            if(expr[i][0] != ',') {
                if(item_name_id_search(global_db, &item, convert_integer(expr[i], 10), global_mode))
                    exit_abt(build_buffer(global_err, "failed to search for item %s in %d", expr[i], block->item_id));
                argument_write(node, item.name);
            }
        }
    } else if(ncs_strcmp(func,"groupranditem") == 0) {
        /*if(const_keyword_search(global_db, &const_info, expr[start], global_mode))
            exit_abt(build_buffer(global_err, "failed to search for const %s in %d", expr[start], block->item_id));
        ea_item_group_search(global_db, &ea_item_group, const_info.value, global_mode, buffer);
        block->flag |= 0x04;*/
        translate_write(block, expr[start], 1);
    } else {
        exit_buf("failed to search function handler for %s in item %d", func, block->item_id);
    }

    /* borrow deps from results one and two */
    if(resultOne != NULL)
        for(i = 0; i < resultOne->dep->buf_ptr_cnt; i++)
            dep_write(node->dep, &resultOne->dep->buf[resultOne->dep->buf_ptr[i]]);
    if(resultTwo != NULL)
        for(i = 0; i < resultTwo->dep->buf_ptr_cnt; i++)
            dep_write(node->dep, &resultTwo->dep->buf[resultTwo->dep->buf_ptr[i]]);

    if(resultOne != NULL) node_free(resultOne);
    if(resultTwo != NULL) node_free(resultTwo);
    if(skill.name != NULL) free(skill.name);
    if(skill.desc != NULL) free(skill.desc);
    if(item.name != NULL) free(item.name);
    if(item.script != NULL) free(item.script);
    if(const_info.name != NULL) free(const_info.name);
    return SCRIPT_PASSED;
}

void evaluate_node(node_t * node, FILE * stm, logic_node_t * logic_tree, int flag, int * complexity) {
    int i = 0;
    range_t * temp = NULL;
    range_t * result = NULL;
    logic_node_t * new_tree = NULL;

    /* support unary and binary operators */
    if(node->left == node->right) {
        /* unary operator */
        if(node->left != NULL) evaluate_node(node->left, stm, logic_tree, flag, complexity);
    } else {
        /* support ?: flip the logic tree for one side */
        if(node->type == NODE_TYPE_OPERATOR && node->op == ':') {
            if(logic_tree != NULL) {
                new_tree = inverse_logic_tree(logic_tree);
                /* use the 'true' logic tree for left; 'false' logic tree for right */
                if(node->left != NULL) evaluate_node(node->left, stm, logic_tree, flag, complexity);
                if(node->right != NULL) evaluate_node(node->right, stm, new_tree, flag, complexity);
                freenamerange(new_tree);
            } else{
                evaluate_node(node->left, stm, logic_tree, flag, complexity);
                evaluate_node(node->right, stm, logic_tree, flag, complexity);
            }
        /* normal pre-order traversal */
        } else {
            if(node->left != NULL) evaluate_node(node->left, stm, logic_tree, flag, complexity);
            /* support ?: add logic tree on stack */
            if(node->type == NODE_TYPE_OPERATOR && node->op == '?') {
                /* check that logic tree exist for left side */
                if(node->left->cond != NULL) {
                    new_tree = node->left->cond;
                    new_tree->stack = logic_tree;
                    /* keep the logic tree of ? conditional */
                    if(EVALUATE_FLAG_KEEP_TEMP_TREE & flag)
                        node->cond = copy_any_tree(new_tree);
                    if(node->right != NULL) evaluate_node(node->right, stm, new_tree, flag, complexity);
                } else {
                    if(node->right != NULL) evaluate_node(node->right, stm, logic_tree, flag, complexity);
                }
            } else {
                if(node->right != NULL) evaluate_node(node->right, stm, logic_tree, flag, complexity);
            }
        }
    }

    /* handle variable and function nodes; generate condition node */
    if(node->type & (NODE_TYPE_FUNCTION | NODE_TYPE_VARIABLE)) {
        if(node->id == NULL) {
            fprintf(stderr,"evalute_node; null id for variable or function.\n");
            exit(EXIT_FAILURE);
        }
        /* check logic tree for dependency */
        if(logic_tree != NULL)
            if(node->type == NODE_TYPE_FUNCTION || node->type == NODE_TYPE_VARIABLE) {
                result = search_tree_dependency(logic_tree, node->id, node->range);
                if(result != NULL) {
                    temp = node->range;
                    node->min = minrange(result);
                    node->max = maxrange(result);
                    node->range = result;
                    node->range->id_min = node->min;
                    node->range->id_max = node->max;
                    freerange(temp);
                }
            }
        node->cond = make_cond(node->id, node->range, NULL);

        /* inherit logic node's function argument stack */
        if(node->cond != NULL && node->args_cnt > 0) {
            /* copy the buffers */
            for(i = 0; i < node->args_cnt; i++)
                node->cond->args[i] = node->args[i];
            /* set the pointers at proper offsets */
            for(i = 0; i < node->args_ptr_cnt; i++)
                node->cond->args_ptr[i] = node->args_ptr[i];
            /* copy the integer statistics */
            node->cond->args_cnt = node->args_cnt;
            node->cond->args_ptr_cnt = node->args_ptr_cnt;
        }
    }

    /* handle unary operator nodes */
    if(node->type & NODE_TYPE_UNARY) {
        if(node->left == NULL) {
            fprintf(stderr,"evaluate_node; single node is null.\n");
            exit(EXIT_FAILURE);
        }

        if(node->left->range == NULL) {
            fprintf(stderr,"evaluate_node; single node operand has null range.\n");
            exit(EXIT_FAILURE);
        }

        switch(node->op) {
            case '-': node->range = negaterange(node->left->range); break;
            case '!': node->range = notrange(node->left->range); break;
        }

        if(node->left->cond != NULL)
            node->cond = make_cond(node->left->cond->name, node->range, node->left->cond);

        /* extract the local min and max from the range */
        node->min = minrange(node->range);
        node->max = maxrange(node->range);
        node->cond_cnt += node->left->cond_cnt;
    }

    /* handle operators nodes */
    if(node->type & NODE_TYPE_OPERATOR) {
        /* operators must have two non-null operands */
        if(node->left == NULL || node->right == NULL) {
            fprintf(stderr,"evaluate_node; left or right node is null.\n");
            exit(EXIT_FAILURE);
        }

        /* sub node inherit range, operator calculate range, operands have nodes created */
        if(node->left->range == NULL || node->right->range == NULL) {
            fprintf(stderr,"evaluate_node; left or right operand has null range.\n");
            exit(EXIT_FAILURE);
        }

        /* calculate range for operator node */
        switch(node->op) {
            case '*': node->range = calcrange(node->op, node->left->range, node->right->range); node_inherit_cond(node); break;
            case '/': node->range = calcrange(node->op, node->left->range, node->right->range); node_inherit_cond(node); break;
            case '+': node->range = calcrange(node->op, node->left->range, node->right->range); node_inherit_cond(node); break;
            case '-': node->range = calcrange(node->op, node->left->range, node->right->range); node_inherit_cond(node); break;
            case '<' + '=': 
            case '<': 
            case '>' + '=': 
            case '>': 
            case '!' + '=': 
            case '=' + '=': 
                /* typically, scripts use boolean expressions like 100 + 600 * (getrefine() > 5), which can be interpreted
                 * wrong for non-boolean expression, which should result in either 0 or 1 rather than range. */
                if(flag & EVALUATE_FLAG_EXPR_BOOL)
                    node->range = calcrange(node->op, node->left->range, node->right->range);
                else
                    node->range = mkrange(INIT_OPERATOR, 0, 1, 0);
                node_inherit_cond(node);
                (*complexity)++;
                break;
            case '&': 
                node->range = calcrange(node->op, node->left->range, node->right->range); 
                node_inherit_cond(node);
                break;
            case '|': 
                node->range = calcrange(node->op, node->left->range, node->right->range); 
                node_inherit_cond(node); 
                break;
            case '&' + '&': 
                if(flag & EVALUATE_FLAG_EXPR_BOOL) {
                    node->range = calcrange(node->op, node->left->range, node->right->range);
                    if(node->left->cond != NULL && node->right->cond != NULL)
                        node->cond = new_cond(LOGIC_NODE_AND, node->left->cond, node->right->cond);
                } else {
                    node->range = mkrange(INIT_OPERATOR, 0, 1, 0);
                }
                (*complexity)++;
                break;
            case '|' + '|': 
                if(flag & EVALUATE_FLAG_EXPR_BOOL) {
                    node->range = calcrange(node->op, node->left->range, node->right->range);
                    if(node->left->cond != NULL && node->right->cond != NULL)
                        node->cond = new_cond(LOGIC_NODE_OR, node->left->cond, node->right->cond);
                } else {
                    node->range = mkrange(INIT_OPERATOR, 0, 1, 0);  
                }
                (*complexity)++;
                break;
            case ':': 
                node->range = orrange(node->left->range, node->right->range); 
                (*complexity)++;
                break;
            case '?': 
                node->range = copyrange(node->right->range); /* right node is : operator */ 
                (*complexity)++;
                break;
            default: 
                fprintf(stderr,"evaluate_node; supported operator %c\n", node->op);
                break;            
        }

        /* extract the local min and max from the range */
        node->min = minrange(node->range);
        node->max = maxrange(node->range);

        /* running count of all the variable, function, and set block */
        node->cond_cnt = node->left->cond_cnt + node->right->cond_cnt;
    }

    /* write expression and print node */
    if(flag & EVALUATE_FLAG_WRITE_FORMULA)
        (node->type == NODE_TYPE_OPERATOR)?
            node_expr_append(node->left, node->right, node):
            node_expr_append(NULL, NULL, node);
    node_dmp(node, stm);
}

/* function must be called AFTER node->range is calc */
void node_inherit_cond(node_t * node) {
    /* only inherit if only ONE condition exist;
     * if condition exist on either end, then
     * the condition can't be interpreted. */
    if(node->left->cond != NULL && node->right->cond == NULL) {
        if(node->right->id != NULL)
            node->left->cond->args_str = convert_string(node->right->id);
        node->cond = make_cond(node->left->cond->name, node->range, node->left->cond);
    } else if(node->left->cond == NULL && node->right->cond != NULL) {
        node->cond = make_cond(node->right->cond->name, node->range, node->right->cond);
        if(node->left->id != NULL)
            node->right->cond->args_str = convert_string(node->left->id);
    }
}

void node_dmp(node_t * node, FILE * stm) {
    if(stm != NULL) {
        fprintf(stm," -- Node %p --\n", (void *) node);
        switch(node->type) {
            case NODE_TYPE_OPERATOR:   
                switch(node->op) {
                     case '<' + '=':   fprintf(stm,"     Type: Operator <=\n"); break;
                     case '>' + '=':   fprintf(stm,"     Type: Operator >=\n"); break;
                     case '!' + '=':   fprintf(stm,"     Type: Operator !=\n"); break;
                     case '=' + '=':   fprintf(stm,"     Type: Operator ==\n"); break;
                     case '&' + '&':   fprintf(stm,"     Type: Operator &&\n"); break;
                     case '|' + '|':   fprintf(stm,"     Type: Operator ||\n"); break;
                     default:          fprintf(stm,"     Type: Operator %c\n", node->op); break;
                 } 
                 break;
            case NODE_TYPE_UNARY: fprintf(stm,"     Type: Operator %c\n", node->op); break;
            case NODE_TYPE_OPERAND: fprintf(stm,"     Type: Literal %d:%d\n", node->min, node->max); break;
            case NODE_TYPE_FUNCTION: fprintf(stm,"     Type: Function %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_VARIABLE: fprintf(stm,"     Type: Variable %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_LOCAL: fprintf(stm,"     Type: Local %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_CONSTANT: fprintf(stm,"     Type: Constant %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_SUB: fprintf(stm,"     Type: Subexpression %s; %d:%d\n", node->expr, node->min, node->max); break;
            default: fprintf(stm,"     Type: %d\n", node->op); break;
        }
        /*fprintf(stm,"  Minimum: %d\n", node->min);
        fprintf(stm,"  Maximum: %d\n", node->max);*/
        fprintf(stm,"  Arg_Cnt: %d\n", node->args_cnt);
        fprintf(stm,"  Arg_Ptr: %d\n", node->args_ptr_cnt);
        /*fprintf(stm,"   Parent: %p\n", (void *) node->parent);
        fprintf(stm,"     Left: %p\n", (void *) node->left);
        fprintf(stm,"    Right: %p\n", (void *) node->right);      
        fprintf(stm,"     Next: %p\n", (void *) node->next);
        fprintf(stm,"     Prev: %p\n", (void *) node->prev);*/
        fprintf(stm," Cond Cnt: %d\n", node->cond_cnt);
        /*fprintf(stm," Expr Len: %d\n", node->expr_cnt);*/
        fprintf(stm,"     Expr: %s\n", node->expr);
        dmprange(node->range, stm, "range; ");
        dmpnamerange(node->cond, stdout, 0);
    }
}

/* generate human readable string of the expression, writable solution. */
void node_expr_append(node_t * left, node_t * right, node_t * dest) {
    int i = 0;
    char temp[BUF_SIZE];
    var_t var_info;

    /* dest is a leave node */
    if(left == NULL && right == NULL) {
        switch(dest->type) {
            case NODE_TYPE_UNARY:
                sprintf(dest->expr,"%c %s", dest->op, dest->left->expr);
                break;
            case NODE_TYPE_OPERAND:
                sprintf(dest->expr,"%d", dest->max);   
                break;
            case NODE_TYPE_FUNCTION:
                if(dest->expr_cnt <= 0) {
                    memset(&var_info, 0, sizeof(var_t));
                    if(var_keyword_search(global_db, &var_info, dest->id)) {
                        printf("warning: failed to resolve function name %s\n", dest->id);
                        sprintf(dest->expr,"%s", dest->id);
                    } else {
                        sprintf(dest->expr,"%s", var_info.str);
                    }
                    if(var_info.id != NULL) free(var_info.id);
                    if(var_info.str != NULL) free(var_info.str);
                }
                dep_write(dest->dep, dest->expr);
                break;
            case NODE_TYPE_VARIABLE:   
                memset(&var_info, 0, sizeof(var_t));
                if(var_keyword_search(global_db, &var_info, dest->id)) {
                    printf("warning: failed to resolve variable name %s\n", dest->id);
                    sprintf(dest->expr,"%s", dest->id);
                } else {
                    sprintf(dest->expr,"%s", var_info.str);
                }
                dep_write(dest->dep, dest->expr);
                if(var_info.id != NULL) free(var_info.id);
                if(var_info.str != NULL) free(var_info.str);
                break;
            case NODE_TYPE_LOCAL:
                if(dest->expr_cnt > 0) {
                    sprintf(dest->expr,"%s", dest->expr);
                } else {
                    /* some set block don't have function or variable */
                    if(dest->min == dest->max)
                        sprintf(dest->expr,"%d", dest->max);
                    else
                        sprintf(dest->expr,"%d~%d", dest->min, dest->max);
                }
                break;
            case NODE_TYPE_CONSTANT:   
                sprintf(dest->expr,"%s", dest->id);
                /*printf("Constant: %s %d\n", dest->id, dest->var_id);*/
                break;
            case NODE_TYPE_SUB:
                dest->expr_cnt = sprintf(temp,"(%s)", dest->expr);
                temp[dest->expr_cnt] = '\0';
                sprintf(dest->expr,"%s", temp);
                /*printf("Subexpression: %s\n", dest->expr);*/
                break;
            default: exit_buf("invalid node type %d", dest->type);
        }
    /* dest is a operand node */
    } else if(left != NULL && right != NULL) {
        switch(dest->type) {
            case NODE_TYPE_OPERATOR:
                switch(dest->op) {
                    case '?': 
                        if(dest->dep->buf_ptr_cnt > 0) {
                            dest->expr_cnt = sprintf(dest->expr,"based on %s", &dest->dep->buf[dest->dep->buf_ptr[0]]);
                            dest->expr[dest->expr_cnt++] = '\0';
                            for(i = 1; i < dest->dep->buf_ptr_cnt - 1; i++) {
                                dest->expr_cnt += sprintf(&dest->expr[dest->expr_cnt], ", %s", &dest->dep->buf[dest->dep->buf_ptr[i]]);
                                dest->expr[dest->expr_cnt] = '\0';
                            }
                            if(i < dest->dep->buf_ptr_cnt) {
                                dest->expr_cnt += sprintf(&dest->expr[dest->expr_cnt], ", %s", &dest->dep->buf[dest->dep->buf_ptr[i]]);
                                dest->expr[dest->expr_cnt] = '\0';
                            }
                        } else {
                            sprintf(dest->expr,"%s %c %s", left->expr, dest->op, right->expr);
                        }
                        break;
                    case '>': sprintf(dest->expr,"%s > %s", left->expr, right->expr); break;
                    case '<': sprintf(dest->expr,"%s < %s", left->expr, right->expr); break;
                    case '!' + '=': sprintf(dest->expr,"%s != %s", left->expr, right->expr); break;
                    case '=' + '=': sprintf(dest->expr,"%s == %s", left->expr, right->expr); break;
                    case '<' + '=': sprintf(dest->expr,"%s <= %s", left->expr, right->expr); break;
                    case '>' + '=': sprintf(dest->expr,"%s >= %s", left->expr, right->expr); break;
                    case '&' + '&': sprintf(dest->expr,"%s and %s", left->expr, right->expr); break;
                    case '|' + '|': sprintf(dest->expr,"%s or %s", left->expr, right->expr); break;
                    default: sprintf(dest->expr,"%s %c %s", left->expr, dest->op, right->expr); break;
                }
                break;
            default: exit_buf("invalid node type %d", dest->type);
        }
    }
}

void node_free(node_t * node) {
    if(node != NULL) {
        freenamerange(node->cond);
        freerange(node->range);
        free(node->dep);
        free(node);
    }
}

/* write into function argument */
void argument_write(node_t * node, char * desc) {
   int offset = node->args_cnt;                       /* current offset on stack */
   node->args_ptr[node->args_ptr_cnt++] = offset;     /* record current offset on stack */
   offset += sprintf(node->args + offset, "%s", desc);/* write new string onto stack */
   node->args[offset] = '\0';                         /* null terminate new string */
   node->args_cnt = offset + 1;                       /* set new offset on stack */
}

int translate_bonus_desc(node_t ** result, block_r * block, bonus_t * bonus) {
    int i = 0;
    int arg_cnt = bonus->type_cnt;
    int offset = 0;            /* current buffer offset */
    char buffer[BUF_SIZE];  /* buffer for handling modifier and qualifiers */
    int order[BONUS_SIZE];
    char aux[BONUS_SIZE][BUF_SIZE]; /* FORMULA Q.Q */

    /* item bonus integrity check */
    if(bonus->type_cnt != bonus->order_cnt || bonus->type_cnt > BONUS_SIZE)
        exit_abt(build_buffer(global_err,"warning[%d]: bonus(%s %s) argument and order count mismatch.\n", 
            block->item_id, bonus->pref, bonus->buff));

    /* order contains stack pointer constants that indicate the string on the stack to write to template */
    for(i = 0; i < bonus->type_cnt; i++)
        order[i] = bonus->order[i];

    /* perform post-integer analysis to determine qualifier or modifier use for translation;
     * specific qualifier or modifier is selected base on the <attr> field in item_bonus.txt;
     * these handlers will normalize the integer to positive and given modifier. */
    switch(bonus->attr) {
        case 1: translate_bonus_percentage2(block, result[1], &order[0], "Induce %s damage", "Reduce %s damage taken", "Receive %s more damage"); break;
        case 2: translate_bonus_percentage2(block, result[1], &order[0], "Add %s", "Add %s", "Reduce %s"); break;
        case 3: translate_bonus_percentage2(block, result[0], &order[0], "Increase %s", "Increase %s", "Decrease %s"); break;
        case 4: block->eng[0][0] = toupper(block->eng[0][0]); break;
        case 5: translate_bonus_integer(block, result[0], &order[0]); break;
        case 6: translate_bonus_percentage(block, result[0], &order[0]); break;
        case 7: translate_bonus_integer2(block, result[0], &order[0], "Regain %s", "Regain %s", "Drain %s"); break;
        case 8: translate_bonus_integer2(block, result[1], &order[0], "Regain %s", "Regain %s", "Drain %s"); break;
        case 9: translate_bonus_float(block, result[1], &order[0], 100); break;
        case 10: translate_bonus_percentage2(block, result[1], &order[0], "Induce %s damage", "Receive %s", "Reduce %s"); break;
        case 11: translate_bonus_percentage(block, result[1], &order[0]); break;
        case 12:
            translate_bonus_float_percentage2(block, result[1], &order[1], "Add %s", "Add %s", "Reduce %s", 10); 
            translate_bonus_percentage(block, result[2], &order[2]); 
            break;
        case 13:
            translate_bonus_integer(block, result[3], &order[2]);
            translate_bonus_float_percentage(block, result[1], &order[0], 100);
            translate_bonus_float(block, result[2], &order[3], 1000);
            /*translate_bonus_float(block, result[3], &order[2], 1000);*/
            break;
        case 14: translate_bonus_percentage2(block, result[1], &order[0], "Increase %s", "Increase %s", "Decrease %s"); break;
        case 15: translate_bonus_percentage2(block, result[0], &order[0], "Add %s", "Add %s", "Reduce %s"); break;
        case 16: translate_bonus_integer2(block, result[0], &order[0], "Add %s", "Add %s", "Add %s"); break;
        case 17: translate_bonus_float_percentage2(block, result[0], &order[0], "Add %s", "Add %s", "Reduce %s", 100); break;
        case 18: translate_bonus_integer(block, result[1], &order[0]); break;
        case 19: translate_bonus_percentage2(block, result[1], &order[0], "Add %s", "Reduce %s", "Receive %s"); break;
        case 20: translate_bonus_percentage2(block, result[2], &order[0], "Add %s", "Add %s", "Reduce %s"); break;
        case 21: translate_bonus_percentage2(block, result[1], &order[0], "Reduce %s", "Reduce %s", "Add %s"); break;
        case 22: 
            /* missing a modifier function for integer based */
            if((result[1]->min / 1000) != 0 && (result[1]->max / 1000) != 0) {
                result[1]->min /= 1000;
                result[1]->max /= 1000;
                translate_bonus_integer2(block, result[1], &order[0], "Add %s seconds", "Add %s seconds", "Reduce %s seconds"); 
            } else {
                translate_bonus_integer2(block, result[1], &order[0], "Add %s millisecond", "Add %s millisecond", "Reduce %s millisecond"); 
            }
            break;
        case 23: /* special case */
            if(result[1]->min == 0) {
                translate_bonus_integer2(block, result[0], &order[0], "Gain %s", "Gain %s", "Drain %s"); break;
            } else {
                translate_bonus_integer2(block, result[0], &order[0], "Drain %s", "Drain %s", "Gain %s"); break;
            }
            arg_cnt = 1;
            break;
        case 24: 
            translate_bonus_float_percentage2(block, result[0], &order[0], "Add %s", "Add %s", "Reduce %s", 10); 
            translate_bonus_percentage(block, result[1], &order[1]); 
            break;
        case 25:
            if(result[2]->min == 0) {
                translate_bonus_float_percentage2(block, result[0], &order[0], "Add %s", "Add %s", "Reduce %s", 10); 
                translate_bonus_percentage2(block, result[1], &order[1], "gain %s", "gain %s", "drain %s");
            } else {
                translate_bonus_float_percentage2(block, result[0], &order[0], "Add %s", "Add %s", "Reduce %s", 10); 
                translate_bonus_percentage2(block, result[1], &order[1], "drain %s", "drain %s", "gain %s");
            }
            arg_cnt = 2;
            break;
        case 26: translate_bonus_float_percentage2(block, result[1], &order[0], "Add %s", "Add %s", "Reduce %s", 100); break;
        case 27: translate_bonus_float_percentage2(block, result[2], &order[0], "Add %s", "Add %s", "Reduce %s", 10); break;
        case 28: translate_bonus_float_percentage2(block, result[2], &order[0], "Add %s", "Add %s", "Reduce %s", 100); break;
        case 29: translate_bonus_float(block, result[3], &order[0], 10); break;
        case 30: /* future debugggin print purposes */
        default: break;
    }

    /* write the final translation using the template */
    switch(arg_cnt) {
        case 1: 
            translate_bonus_template(buffer, &offset, bonus->desc, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]));
            break;
        case 2: 
            translate_bonus_template(buffer, &offset, bonus->desc, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]])); 
            break;
        case 3: 
            translate_bonus_template(buffer, &offset, bonus->desc, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]]), 
                formula(aux[2], block->eng[order[2]], result[bonus->order[2]])); 
            break;
        case 4: 
            translate_bonus_template(buffer, &offset, bonus->desc, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]]), 
                formula(aux[2], block->eng[order[2]], result[bonus->order[2]]), 
                formula(aux[3], block->eng[order[3]], result[bonus->order[3]])); 
            break;
        case 5: 
            translate_bonus_template(buffer, &offset, bonus->desc, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]]), 
                formula(aux[2], block->eng[order[2]], result[bonus->order[2]]), 
                formula(aux[3], block->eng[order[3]], result[bonus->order[3]]), 
                formula(aux[4], block->eng[order[4]], result[bonus->order[4]])); 
            break;
    }
    translate_write(block, buffer, 1);

    /* debugging purposes */
    switch(bonus->attr) {
        default: /*printf("[%d]%s\n", block->item_id, buffer);*/ break;
    }

    return SCRIPT_PASSED;
}

char * formula(char * buf, char * eng, node_t * result) {
    int len = 0;
    if(result != NULL && result->cond_cnt > 0) {
        if(result->expr[0] == '(' && result->expr[strlen(result->expr) - 1] == ')')
            len = sprintf(buf, "%s %s", eng, result->expr);
        else
            len = sprintf(buf, "%s (%s)", eng, result->expr);
        buf[len] = '\0';
        return buf;
    }
    return eng;
}

char * translate_bonus_template(char * buffer, int * offset, char * template, ...) {
    va_list bonus_args;
    va_start(bonus_args, template);
    (*offset) = vsprintf(buffer, template, bonus_args);
    buffer[(*offset)] = '\0';
    va_end(bonus_args);
    return buffer;
}

void translate_bonus_integer(block_r * block, node_t * result, int * order) {
    int offset = 0;
    char buffer[BUF_SIZE];
    char range[BUF_SIZE];
    offset = sprintf(buffer,"%s", check_node_range(result, range));
    buffer[offset] = '\0';
    translate_write(block, buffer, 1);
    *order = block->eng_cnt - 1;
}

void translate_bonus_integer2(block_r * block, node_t * result, int * order, char * bipolar, char * positive, char * negative) {
    int polarity = 0;
    int offset = 0;
    char buffer[BUF_SIZE];
    char range[BUF_SIZE];

    /* calculate polarity */
    polarity = check_node_affinity(result);
    switch(polarity) {
        case 0: offset = sprintf(buffer, bipolar, check_node_range(result, range)); break;
        case 1: offset = sprintf(buffer, positive, check_node_range(result, range)); break;
        case 2: 
            if(result->min < 0) result->min *= -1;
            if(result->max < 0) result->max *= -1;
            offset = sprintf(buffer, negative, check_node_range(result, range)); 
            break;
    }
    buffer[offset] = '\0';
    /* write new translation onto translation stack */
    translate_write(block, buffer, 1);
    /* change ordering so template writes new string */
    *order = block->eng_cnt - 1;
}

void translate_bonus_float(block_r * block, node_t * result, int * order, int modifier) {
    int offset = 0;
    char buffer[BUF_SIZE];
    char range[BUF_SIZE];
    offset = sprintf(buffer,"%s", check_node_range_float(result, range, modifier));
    buffer[offset] = '\0';
    translate_write(block, buffer, 1);
    *order = block->eng_cnt - 1;
}

void translate_bonus_float_percentage(block_r * block, node_t * result, int * order, int modifier) {
    int offset = 0;
    char buffer[BUF_SIZE];
    char range[BUF_SIZE];
    offset = sprintf(buffer,"%s", check_node_range_float_percentage(result, range, modifier));
    buffer[offset] = '\0';
    translate_write(block, buffer, 1);
    *order = block->eng_cnt - 1;
}

void translate_bonus_percentage(block_r * block, node_t * result, int * order) {
    int offset = 0;
    char buffer[BUF_SIZE];
    char range[BUF_SIZE];
    offset = sprintf(buffer,"%s", check_node_range_precentage(result, range));
    buffer[offset] = '\0';
    translate_write(block, buffer, 1);
    *order = block->eng_cnt - 1;
}

void translate_bonus_percentage2(block_r * block, node_t * result, int * order, char * bipolar, char * positive, char * negative) {
    int polarity = 0;
    int offset = 0;
    char buffer[BUF_SIZE];
    char range[BUF_SIZE];

    /* calculate polarity */
    polarity = check_node_affinity(result);
    switch(polarity) {
        case 0: offset = sprintf(buffer, bipolar, check_node_range_precentage(result, range)); break;
        case 1: offset = sprintf(buffer, positive, check_node_range_precentage(result, range)); break;
        case 2: 
            if(result->min < 0) result->min *= -1;
            if(result->max < 0) result->max *= -1;
            offset = sprintf(buffer, negative, check_node_range_precentage(result, range)); 
            break;
    }
    buffer[offset] = '\0';
    /* write new translation onto translation stack */
    translate_write(block, buffer, 1);
    /* change ordering so template writes new string */
    *order = block->eng_cnt - 1;
}

void translate_bonus_float_percentage2(block_r * block, node_t * result, int * order, char * bipolar, char * positive, char * negative, int modifier) {
    int polarity = 0;
    int offset = 0;
    char buffer[BUF_SIZE];
    char range[BUF_SIZE];

    /* calculate polarity */
    polarity = check_node_affinity(result);
    switch(polarity) {
        case 0: offset = sprintf(buffer, bipolar, check_node_range_float_percentage(result, range, modifier)); break;
        case 1: offset = sprintf(buffer, positive, check_node_range_float_percentage(result, range, modifier)); break;
        case 2: 
            if(result->min < 0) result->min *= -1;
            if(result->max < 0) result->max *= -1;
            offset = sprintf(buffer, negative, check_node_range_float_percentage(result, range, modifier)); 
            break;
    }
    buffer[offset] = '\0';
    /* write new translation onto translation stack */
    translate_write(block, buffer, 1);
    /* change ordering so template writes new string */
    *order = block->eng_cnt - 1;
}

char * check_node_range(node_t * node, char * buffer) {
    int offset = 0;
    offset = (node->min != node->max) ?
        sprintf(buffer, "%+d ~ %+d", node->min, node->max) :
        sprintf(buffer, "%+d", node->min);
    buffer[offset] = '\0';
    return buffer;
}

char * check_node_range_float(node_t * node, char * buffer, int modifier) {
    int offset = 0;
    if((node->min / modifier) < 1 && (node->max / modifier) < 1)
        offset = (node->min != node->max) ?
        sprintf(buffer, "%+.2f ~ %+.2f", ((double) node->min / modifier), ((double) node->max / modifier)) :
        sprintf(buffer, "%+.2f", ((double) node->min / modifier));
    else
        offset = (node->min != node->max) ?
        sprintf(buffer, "%+.0f ~ %+.0f", ((double) node->min / modifier), ((double) node->max / modifier)) :
        sprintf(buffer, "%+.0f", ((double) node->min / modifier));
    buffer[offset] = '\0';
    return buffer;
}

char * check_node_range_precentage(node_t * node, char * buffer) {
    int offset = 0;
    offset = (node->min != node->max) ?
        sprintf(buffer, "%+d%% ~ %+d%%", node->min, node->max) :
        sprintf(buffer, "%+d%%", node->min);
    buffer[offset] = '\0';
    return buffer;
}

char * check_node_range_float_percentage(node_t * node, char * buffer, int modifier) {
    int offset = 0;
    if((node->min / modifier) < 1 && (node->max / modifier) < 1)
        offset = (node->min != node->max) ?
        sprintf(buffer, "%+.2f%% ~ %+.2f%%", ((double) node->min / modifier), ((double) node->max / modifier)) :
        sprintf(buffer, "%+.2f%%", ((double) node->min / modifier));
    else
        offset = (node->min != node->max) ?
        sprintf(buffer, "%+.0f%% ~ %+.0f%%", ((double) node->min / modifier), ((double) node->max / modifier)) :
        sprintf(buffer, "%+.0f%%", ((double) node->min / modifier));
    buffer[offset] = '\0';
    return buffer;
}

/* 0; both; 1 positive; 2; negative*/
int check_node_affinity(node_t * node) {
    int affinity = 0;
    if(node == NULL)
        exit_abt(build_buffer(global_err, "check_node_affinity on node->NULL.\n"));
    if(node->min <= 0 && node->max <= 0) affinity = 2;
    else if(node->min >= 0 && node->max >= 0) affinity = 1;
    else if(node->min < 0 && node->max > 0) affinity = 0;
    return affinity;
}

int script_generate(block_r * block, int block_cnt, char * buffer, int * offset) {
    int i = 0;
    int j = 0;
    int k = 0;
    int nest = 0;
    int sc_end_cnt = 0;
    char buf[BUF_SIZE];

    for(i = 0; i < block_cnt; i++) {
        /* calculate linkage nest */
        nest = script_linkage_count(block, i);
        buf[0] = '\0';
        if(nest > 0) {
            k = 0;
            buf[k++] = ' ';
            for(j = 0; j < nest && k < BUF_SIZE; j++, k++) {
                if(j + 1 >= nest) {
                    buf[k++] = '-';
                    buf[k++] = '>';
                    buf[k] = ' ';
                } else {
                    buf[k++] = '-';
                    buf[k++] = '-';
                    buf[k] = '-';
                }
            }
            buf[k] = '\0';
        }
        switch(block[i].type->id) {
            case BLOCK_NULLIFIED: break; /* nullified block mean they're deleted */
            case 16: /* merge all sc_end into one block */
                /* figure out how many sc_end blocks there are */
                for(j = i; j < block_cnt; j++)
                    if(block[i].type->id == 16)
                        sc_end_cnt++;
                *offset += sprintf(buffer + *offset, "Cures ");
                /* if there exist only one sc_end, then assume the current one is the only one */
                if(sc_end_cnt == 1) {
                    *offset += sprintf(buffer + *offset, "%s", block[i].eng[0]);
                } else {
                    *offset += sprintf(buffer + *offset, "%s", block[i].eng[0]);
                    for(j = i + 1; j < block_cnt; j++) {
                        /* steal the status effect */
                        if(block[j].type->id == 16) {
                            if(j < block_cnt - 1) {
                                *offset += sprintf(buffer + *offset, ", %s", block[j].eng[0]);
                            } else {
                                *offset += sprintf(buffer + *offset, ", and %s", block[j].eng[0]);
                            }
                            /* nullified the block from being read */
                            block[j].type->id = BLOCK_NULLIFIED;
                        }
                    }
                }
                *offset += sprintf(buffer + *offset, " status effects.\n");
                
                break;
            /* blocks that might have other blocks linked to it */
            case 26: /* if */
            case 27: /* else */
                if(block[i].logic_tree != NULL)
                    script_generate_cond(block[i].logic_tree, stdout, buf, buffer, offset);
                break;
            case 28: /* set */
                /* special case for zeny */
                if(ncs_strcmp(block->ptr[0], "zeny") == 0)
                    *offset += sprintf(buffer + *offset, "%sCurrent %s\n", buf, block[i].exp[0]);
                break;
            /* ignore these blocks, since they have no interpretation */
            case 21: /* skilleffect */
            case 22: /* specialeffect2 */
            case 29: /* input */
            case 32: /* end */
                break;
            /* general block writing */
            default:
                if(block[i].desc != NULL)
                    *offset += sprintf(buffer + *offset, "%s%s\n", buf, block[i].desc);
                else {
                    if(block[i].eng_cnt > 0 && block[i].eng[block[i].eng_cnt - 1] != NULL)
                        *offset += sprintf(buffer + *offset, "%s%s\n", buf, block[i].eng[block[i].eng_cnt - 1]);
                }
        }
    }
    buffer[*offset] = '\0';
    return SCRIPT_PASSED;
}

int script_linkage_count(block_r * block, int start) {
    int nest = 0;
    int iter = start;
    /* count the number of links until 'if' or 'root' (-1) */
    while(block[iter].link >= 0) {
        if(block[iter].type->id != 27)
            nest++;
        iter = block[iter].link;
    }
    return nest;
}

/* primary multiplexer for translating logic tree */
void script_generate_cond(logic_node_t * tree, FILE * stm, char * prefix, char * buffer, int * offset) {
    char buf[BUF_SIZE];
    int off = 0;
    memset(buf, 0, BUF_SIZE);

    switch(tree->type) {
        /* each OR node means that multiple conditions */
        case LOGIC_NODE_OR: 
            if(tree->left != NULL) script_generate_cond(tree->left, stm, prefix, buffer, offset);
            if(tree->right != NULL) script_generate_cond(tree->right, stm, prefix, buffer, offset);
            break;
        /* write each OR's children (non-OR node) as separate condition */
        case LOGIC_NODE_AND: 
            script_generate_and_chain(tree, buf, &off);
            *offset += sprintf(buffer + *offset, "%s[%s]\n", prefix, buf);
            break;
        case LOGIC_NODE_COND: 
            script_generate_cond_node(tree, buf, &off);
            *offset += sprintf(buffer + *offset, "%s[%s]\n", prefix, buf);
            break;
        default: break;
    }
}

/* recursively chain all cond nodes into one buffer */
void script_generate_and_chain(logic_node_t * tree, char * buf, int * offset) {
    if(tree->left != NULL)
        switch(tree->left->type) {
            case LOGIC_NODE_AND: script_generate_and_chain(tree->left, buf, offset); break;
            case LOGIC_NODE_COND: script_generate_cond_node(tree->left, buf, offset); break;
            default: break;
        }

    if(tree->right != NULL)
        switch(tree->right->type) {
            case LOGIC_NODE_AND: script_generate_and_chain(tree->right, buf, offset); break;
            case LOGIC_NODE_COND: script_generate_cond_node(tree->right, buf, offset); break;
            default: break;
        }
}

void script_generate_cond_node(logic_node_t * tree, char * buf, int * offset) {
    int i = 0;
    int val_min = 0;
    int val_max = 0;
    int var_id = 0;
    char * val_str = NULL;
    var_t var;
    ic_skill_t skill;
    ic_item_t item;
    memset(&var, 0, sizeof(var_t));
    memset(&skill, 0, sizeof(ic_skill_t));
    memset(&item, 0, sizeof(ic_item_t));
    /* extract the value bounds */
    val_min = minrange(tree->range);
    val_max = maxrange(tree->range);

    /* if the buffer is not initially empty then add an 'and' in the buffer */
    if(*offset > 0) {
        *offset += sprintf(buf + *offset," and ");
        buf[*offset] = '\0';
    }

    /* resolve the condition by using variable and function database */
    if(!var_keyword_search(global_db, &var, tree->name)) {
        var_id = var.tag;
        switch(var_id) {
            /* simple dependencies */
            case 1: script_generate_cond_generic(buf, offset, val_min, val_max, "Refine Rate"); break; /* getrefine */
            case 2: script_generate_cond_generic(buf, offset, val_min, val_max, "Refine Rate"); break; /* getequiprefinerycnt */
            case 5: script_generate_cond_generic(buf, offset, val_min, val_max, "Random Roll"); break; /* rand */
            case 17: script_generate_cond_generic(buf, offset, val_min, val_max, "Job Level"); break; /* job level */
            case 18: script_generate_cond_generic(buf, offset, val_min, val_max, "Base Level"); break; /* base level */            
            case 23: script_generate_cond_generic(buf, offset, val_min, val_max, "HP"); break; /* HP */
            case 24: script_generate_cond_generic(buf, offset, val_min, val_max, "Zeny"); break; /* Zeny */
            case 28: *offset += sprintf(buf + *offset,"Marriage Partner Online"); buf[*offset] = '\0'; break; /* getpartnerid */
            case 32: *offset += sprintf(buf + *offset,"Rented Falcon"); buf[*offset] = '\0'; break; /* checkfalcon */
            case 33: *offset += sprintf(buf + *offset,"Rented Madogear"); buf[*offset] = '\0'; break; /* checkmadogear */
            case 34: *offset += sprintf(buf + *offset,"Rebirthed"); buf[*offset] = '\0'; break; /* upper */
            case 35: script_generate_cond_generic(buf, offset, val_min, val_max, "Strength"); break; /* strength */
            case 36: script_generate_cond_generic(buf, offset, val_min, val_max, "Agility"); break; /* agility */
            case 37: script_generate_cond_generic(buf, offset, val_min, val_max, "Vitality"); break; /* vitality */
            case 38: script_generate_cond_generic(buf, offset, val_min, val_max, "Luck"); break; /* luck */
            case 39: script_generate_cond_generic(buf, offset, val_min, val_max, "Dexterity"); break; /* dexterity */
            case 40: script_generate_cond_generic(buf, offset, val_min, val_max, "Intelligence"); break; /* intelligence */
            case 41: *offset += sprintf(buf + *offset,"Non-riding Wug"); buf[*offset] = '\0'; break; /* option_wug */
            case 42: *offset += sprintf(buf + *offset,"Riding Wug"); buf[*offset] = '\0'; break; /* option_wugrider */
            case 43: *offset += sprintf(buf + *offset,"Falcon Skill"); buf[*offset] = '\0'; break; /* HT_FALCON */
            /* complex dependencies */
            case 20: /* baseclass */
                if(val_min == val_max) {
                    *offset += sprintf(buf + *offset,"Base Class is %s", job_tbl(val_min)); buf[*offset] = '\0'; break; /* baseclass */
                } else {
                    script_generate_class_generic(buf, offset, tree->range, "Base Class is not ");
                }
                break;
            case 21: /* basejob */
                if(val_min == val_max) {
                    *offset += sprintf(buf + *offset,"Base Job is %s", job_tbl(val_min)); buf[*offset] = '\0'; break; /* baseclass */
                } else {
                    script_generate_class_generic(buf, offset, tree->range, "Base Job is not ");
                }
                break;
            case 22: /* class */
                if(val_min == val_max) {
                    *offset += sprintf(buf + *offset,"Current Class is %s", job_tbl(val_min)); buf[*offset] = '\0'; break; /* class */
                } else {
                    script_generate_class_generic(buf, offset, tree->range, "Class is not ");
                }
                break;
            case 29: /* strcharinfo */
                if(tree->args_str == NULL) {
                    fprintf(stderr,"warning: strcharinfo does not have comparsion value; strcharinfo() == ? (missing).\n");
                    exit(EXIT_FAILURE);
                }
                if(ncs_strcmp(tree->args_str, "job3_arch02") == 0)
                    val_str = "Archbishop Job Change Map";
                else if(ncs_strcmp(tree->args_str, "job3_rang02") == 0)
                    val_str = "Ranger Job Change Map";
                else if(ncs_strcmp(tree->args_str, "job3_war02") == 0)
                    val_str = "Warlock Job Change Map";
                else if(ncs_strcmp(tree->args_str, "job3_rune02") == 0)
                    val_str = "Rune Knight Job Change Map";
                *offset += sprintf(buf + *offset,"%s is %s", &tree->args[tree->args_ptr[0]], val_str); buf[*offset] = '\0'; 
                buf[*offset] = '\0';
                break;
            case 44: /* gettime month */
            case 45: /* gettime day */
                if(val_min == val_max) {
                    *offset += sprintf(buf + *offset,"%d %s", val_min, &tree->args[tree->args_ptr[0]]); buf[*offset] = '\0'; 
                    buf[*offset] = '\0';
                } else {
                    *offset += sprintf(buf + *offset,"%d~%d %s", val_min, val_max, &tree->args[tree->args_ptr[0]]); buf[*offset] = '\0';  
                    buf[*offset] = '\0';
                }
                break;
            case 30: /* countitem */
                *offset += sprintf(buf + *offset,"Inventory has %d~%d %s", val_min, val_max, &tree->args[tree->args_ptr[0]]); 
                buf[*offset] = '\0';  
                break;
            case 9: /* getequipid */
                /* getequipid must be == or != with an item id value, otherwise error */
                if(val_min == val_max) {
                    if(item_name_id_search(global_db, &item, val_min, global_mode))
                        exit_abt(build_buffer(global_err, "failed to search item id %d", val_min));
                    *offset += sprintf(buf + *offset,"%s equips %s", &tree->args[tree->args_ptr[0]], item.name); 
                    buf[*offset] = '\0'; 
                }
                break;
            case 13: /* isequipped */
                *offset += sprintf(buf + *offset,"Equipped with ");
                for(i = 0; i < tree->args_ptr_cnt; i++) {
                    *offset += sprintf(buf + *offset,"%s, ", &tree->args[tree->args_ptr[i]]);
                }
                buf[*offset - 2] = '\0'; 
                break;
            case 8: /* getiteminfo */
                if(ncs_strcmp(&tree->args[tree->args_ptr[1]], "Weapon Type") == 0) {
                    *offset += sprintf(buf + *offset,"%s's %s is %s", 
                        &tree->args[tree->args_ptr[0]], 
                        &tree->args[tree->args_ptr[1]],
                        weapon_tbl(val_min)); 
                    buf[*offset] = '\0';
                } else {
                    *offset += sprintf(buf + *offset,"%s's %s is %d", 
                        &tree->args[tree->args_ptr[0]], 
                        &tree->args[tree->args_ptr[1]],
                        val_min); 
                    buf[*offset] = '\0';
                }
                break;
            case 48: /* checkmount */
                switch(val_min) {
                    case 0: *offset += sprintf(buf + *offset,"Not Mounted"); buf[*offset] = '\0'; break;
                    case 1: *offset += sprintf(buf + *offset,"Peco Mounted"); buf[*offset] = '\0'; break;
                    case 2: *offset += sprintf(buf + *offset,"Wug Mounted"); buf[*offset] = '\0'; break;
                    case 3: *offset += sprintf(buf + *offset,"Mado Mounted"); buf[*offset] = '\0'; break;
                    case 4: *offset += sprintf(buf + *offset,"Dragon Mounted"); buf[*offset] = '\0'; break;
                    case 5: *offset += sprintf(buf + *offset,"Dragon Mounted"); buf[*offset] = '\0'; break;
                    case 6: *offset += sprintf(buf + *offset,"Dragon Mounted"); buf[*offset] = '\0'; break;
                    case 7: *offset += sprintf(buf + *offset,"Dragon Mounted"); buf[*offset] = '\0'; break;
                    case 8: *offset += sprintf(buf + *offset,"Dragon Mounted"); buf[*offset] = '\0'; break;
                }
                break;
            default: exit_buf("invalid logic node;%d;%s", var.tag, tree->name); break;
        }
        if(var.id != NULL) free(var.id);
        if(var.str != NULL) free(var.str);
        if(item.name != NULL) free(item.name);
        if(item.script != NULL) free(item.script);
        return;
    }

    /* resolve the condition by using skill database */
    if(!skill_name_search(global_db, &skill, tree->name, global_mode)) {
        if(val_min != val_max)
            *offset = sprintf(buf + *offset,"%s Lv.%d ~ Lv.%d", skill.desc, val_min, val_max);
        else
            *offset = sprintf(buf + *offset,"%s Lv.%d", skill.desc, val_max);
        buf[*offset] = '\0';
        if(skill.name != NULL) free(skill.name);
        if(skill.desc != NULL) free(skill.desc);
        return;
    } else {
        exit_abt(build_buffer(global_err, "failed to search skill %s", tree->name));
    }

    dmprange(tree->range, stdout, "nim");
}

void script_generate_class_generic(char * buf, int * offset, range_t * range, char * template) {
    int i = 0;
    range_t * itrrange = NULL;
    range_t * negrange = NULL;

    /* assume list of not equal class, because otherwise 
     * 'true' class have separate class node */
    negrange = notrange(range);
    itrrange = negrange;
    *offset += sprintf(buf + *offset,"%s",template); 
    buf[*offset] = '\0';
    while(itrrange != NULL) {
        for(i = itrrange->min; i <= itrrange->max; i++) {
            *offset += sprintf(buf + *offset,"%s, ", job_tbl(i)); 
            buf[*offset] = '\0';
        }
        itrrange = itrrange->next;
    }
    freerange(negrange);
    buf[(*offset) - 2] = '\0';
    (*offset) -= 2;
}

void script_generate_cond_generic(char * buf, int * offset, int val_min, int val_max, char * template) {
    *offset += (val_min != val_max) ?
        sprintf(buf + *offset, "%s %d ~ %d", template, val_min, val_max) :
        sprintf(buf + *offset, "%s %d", template, val_min);
    buf[*offset] = '\0';
}

int minimize_stack(node_t * left, node_t * right) {
    return SCRIPT_PASSED;
}
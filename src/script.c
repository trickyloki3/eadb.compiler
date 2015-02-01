/*
 *   file: script.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "script.h"

int iter_item_db(int mode, struct ic_db_t * db, ic_item_t * item) {
    int status = 0;
    switch(mode) {
        case EATHENA:
            status = sqlite3_step(db->ea_item_iterate);
            if(status == SQLITE_ROW) {
                item->id = sqlite3_column_int(db->ea_item_iterate, 0);
                strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->ea_item_iterate, 2));
                strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->ea_item_iterate, 19));
            }
            break;
        case RATHENA:
            status = sqlite3_step(db->ra_item_iterate);
            if(status == SQLITE_ROW) {
                item->id = sqlite3_column_int(db->ra_item_iterate, 0);
                strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->ra_item_iterate, 2));
                strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->ra_item_iterate, 20));
            }
            break;
        case HERCULES:
            status = sqlite3_step(db->he_item_iterate);
            if(status == SQLITE_ROW) {
                item->id = sqlite3_column_int(db->he_item_iterate, 0);
                strncopy(item->name, BUF_SIZE, sqlite3_column_text(db->he_item_iterate, 2));
                strncopy(item->script, BUF_SIZE, sqlite3_column_text(db->he_item_iterate, 39));
            }
            break;
        default: return 0;
    }
    return status;
}

int list_forward(block_list_t * list) {
    int off = 0;
    char buf[BUF_SIZE];
    block_r * iter = NULL;

    /* check null paramaters */
    if(exit_null_safe(2, list, list->head))
        return SCRIPT_FAILED;

    /* print the id of each block */
    iter = list->head;
    do {
        /* add the block id to the buffer */
        off += (off <= 0) ?
            sprintf(&buf[off], "%p ", (void *) iter):
            sprintf(&buf[off], "-> %p ", (void *) iter);
        buf[off] = '\0';
        iter = iter->next;
    } while(iter != list->head);

    /* print the buffer list */
    printf("[warn]:  forward: %s\n", buf);
    return SCRIPT_PASSED;
}

int list_backward(block_list_t * list) {
    int off = 0;
    char buf[BUF_SIZE];
    block_r * iter = NULL;

    /* check null paramaters */
    if(exit_null_safe(2, list, list->tail))
        return SCRIPT_FAILED;

    /* print the id of each block */
    iter = list->tail;
    do {
        /* add the block id to the buffer */
        off += (off <= 0) ?
            sprintf(&buf[off], "%p ", (void *) iter):
            sprintf(&buf[off], "-> %p ", (void *) iter);
        buf[off] = '\0';
        iter = iter->prev;
    } while(iter != list->tail);

    /* print the buffer list */
    printf("[warn]: backward: %s\n", buf);
    return SCRIPT_PASSED;   
}

void list_init_head(block_list_t * list) {
    list->head = list->tail = &list->root;
    list->head->next = list->head->prev = list->head;
    list->head->item_id = -1;
}

int list_append_tail(block_list_t * list, block_r * block) {
    /* check null paramater */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* initialize the head node */
    if(list->head == NULL)
        list_init_head(list);
    /* add to the end of the list */
    list->head->prev = block;
    list->tail->next = block;
    block->prev = list->tail;
    block->next = list->head;
    list->tail = block;
    return SCRIPT_PASSED;
}

int list_append_head(block_list_t * list, block_r * block) {
    /* check null paramater */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* initialize the head node */
    if(list->head == NULL)
        list_init_head(list);
    /* add to the front of the list */
    block->prev = list->head;
    block->next = list->head->next;
    list->head->next->prev = block;
    list->head->next = block;
    return SCRIPT_PASSED;
}

int list_add(block_list_t * list, block_r * predecessor, block_r * block) {
    /* check null paramaters */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* initialize the head node */
    if(list->head == NULL)
        list_init_head(list);
    if(predecessor == list->head) {
        /* case 1: add to front of list */
        list_append_head(list, block);
    } else if(predecessor == list->tail) {
        /* case 2: add to end of list */
        list_append_tail(list, block);
    } else {
        /* case 3: add to middle of list */
        block->prev = predecessor;
        block->next = predecessor->next;
        predecessor->next->prev = block;
        predecessor->next = block;
    }
    return SCRIPT_PASSED;
}

int list_rem(block_list_t * list, block_r * block) {
    /* check null paramaters */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* head node cannot be removed */
    if(block == list->head)
        return SCRIPT_FAILED;
    block->prev->next = block->next;
    block->next->prev = block->prev;
    if(block == list->tail)
        list->tail = block->prev;
    block->next = block->prev = NULL;
    return SCRIPT_PASSED;
}

int list_pop_head(block_list_t * list, block_r ** block) {
    /* check null paramaters */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* check if empty */
    if(list->head == list->tail)
        return SCRIPT_FAILED;
    *block = list->head->next;
    list_rem(list, list->head->next);
    return SCRIPT_PASSED;
}

int list_pop_tail(block_list_t * list, block_r ** block) {
    /* check null paramaters */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* check if empty */
    if(list->head == list->tail)
        return SCRIPT_FAILED;
    *block = list->tail;
    list_rem(list, list->tail);
    return SCRIPT_PASSED;
}

int list_check(block_list_t * list, block_r * block) {
    block_r * iter;
    /* check null paramaters */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* check if empty */
    if(list->head == list->tail)
        return SCRIPT_FAILED;
    /* search O(n) the list */
    iter = list->head->next;
    while(iter != list->head) {
        if(block == iter)
            return SCRIPT_PASSED;
        iter = iter->next;
    }
    return SCRIPT_FAILED;
}

int script_block_alloc(script_t * script, block_r ** block) {
    /* check null paramaters */
    if(exit_null_safe(2, script, block))
        return SCRIPT_FAILED;
    /* check free list for a block */
    if(list_pop_head(&script->free, block)) {
        /* allocate a new block and add to alloc list */
        *block = calloc(1, sizeof(block_r));        
    }

    if(list_append_tail(&script->block, *block)) {
        exit_abt_safe("failed to add block from alloc list");
        return SCRIPT_FAILED;
    }    
    return (*block != NULL) ? SCRIPT_PASSED : SCRIPT_FAILED;
}

int script_block_dealloc(script_t * script, block_r ** block) {
    /* check null paramaters */
    if(exit_null_safe(3, script, block, *block))
        return SCRIPT_FAILED;
    /* check block in alloc list */
    if(list_check(&script->block, *block)) {
        exit_func_safe("block %p does not exist in alloc list", (void *) *block);
        return SCRIPT_FAILED;
    }
    /* remove from alloc list */
    if(list_rem(&script->block, *block)) {
        exit_abt_safe("failed to remove block from alloc list");
        return SCRIPT_FAILED;
    }
    /* add to dealloc list */
    if(list_append_tail(&script->free, *block)) {
        exit_abt_safe("failed to add block from dealloc list");
        return SCRIPT_FAILED;
    }
    /* reset to null */
    *block = NULL;
    return SCRIPT_PASSED;
}

int script_block_reset(script_t * script) {
    block_r * block = NULL;
    /* check null paramaters */
    if(exit_null_safe(1, script))
        return SCRIPT_FAILED;
    /* rem from alloc list and add to dealloc list */
    while(!list_pop_head(&script->block, &block)) {
        script_block_release(block);
        if(list_append_tail(&script->free, block)) {
            exit_abt_safe("failed to add block from dealloc list");
            return SCRIPT_FAILED;
        }
    }
    return SCRIPT_PASSED;
}

int script_block_release(block_r * block) {
    int i = 0;

    /* logic trees are generated from conditional blocks and
     * inherited by enclosing scope of the conditional block. */
    deepfreenamerange(block->logic_tree);
    block->logic_tree = NULL;

    /* the result calculated from bonus integer arguments are
     * kept for post-integer analysis on writing the final string. */
    for(i = 0; i < BONUS_SIZE; i++)
        if(block->result[i] != NULL) {
            node_free(block->result[i]);
            block->result[i] = NULL;
        }

    /* invalid the bonus */
    block->bonus.id = -1;
    block->bonus.type_cnt = 0;
    block->bonus.order_cnt = 0;

    /* free the set node */
    if(block->type == 28) {
        node_free(block->set_node);
        block->set_node = NULL;
    }

    /* reset stack */
    block->item_id = 0;
    block->type = 0;
    block->arg_cnt = 0;
    block->ptr_cnt = 0;
    block->eng_cnt = 0;
    block->link = NULL;
    block->set = NULL;
    block->flag = 0;
    block->offset = 0;
    return SCRIPT_PASSED;
}

int script_block_finalize(script_t * script) {
    block_r * block = NULL;
    /* check null paramaters */
    if(exit_null_safe(1, script))
        return SCRIPT_FAILED;
    script_block_reset(script);
    /* rem from dealloc list and free all blocks */
    while(!list_pop_head(&script->free, &block))
        free(block);    
    return SCRIPT_PASSED;
}

int script_block_dump(script_t * script, FILE * stm) {
    int i = 0;
    int off = 0;
    char buf[BUF_SIZE];
    block_r * iter = NULL;

    /* check null paramaters */
    if(exit_null_safe(2, script, stm))
        return SCRIPT_FAILED;

    /* print the id of each block */
    iter = script->block.head->next;
    if(iter == script->block.head) return SCRIPT_FAILED;
    do {
        /* add the block id to the buffer */
        off = sprintf(&buf[off], 
            " --- %d ---\n"
            " Block Addr: %p\n"
            " Block Link: %p\n"
            " Block Set Link: %p\n"
            " Block Type: %s\n"
            " Block Paramater: %d\n", 
            iter->item_id, 
            (void *) iter,
            (void *) iter->link,
            (void *) iter->set,
            block_tbl(iter->type),
            iter->ptr_cnt);
        for(i = 0; i < iter->ptr_cnt; i++)
            off += sprintf(&buf[off], " Block Paramater[%d]: %s\n", i, iter->ptr[i]);
        for(i = 0; i < iter->eng_cnt; i++)
            off += sprintf(&buf[off], " Block Translation[%d]: %s\n", i, iter->eng[i]);
        /*if(iter->type == 28)
            off += sprintf(&buf[off], " Set Formula[%d]: %s\n", iter->set_node->expr_cnt, iter->set_node->expr);*/
        buf[off] = '\0';
        fprintf(stm, "%s", buf);
        /* dump the logic trees */
        if(iter->logic_tree != NULL) {
            fprintf(stm," Block Logic Tree: ");
            dmpnamerange(iter->logic_tree, stm, 0);
        }
        for(i = 0; i < BONUS_SIZE; i++) {
            if(iter->result[i] != NULL) {
                fprintf(stm," Paramater %d Logic Tree: ", i);
                dmpnamerange(iter->result[i]->cond, stm, 0);
            }
        }
        fprintf(stm, "\n");
        iter = iter->next;
        off = 0;
    } while(iter != script->block.head);
    return SCRIPT_PASSED;
}

int split_paramater_list(token_r * token, int * position, char * paramater) {
    /* perform lexical on paramater */
    return (script_lexical(token, paramater) || split_paramater(
            token->script_ptr, 1, token->script_cnt - 1, position)) ?
            SCRIPT_FAILED : SCRIPT_PASSED;
}

int split_paramater(char ** expr, int start, int end, int * pos) {
    int i = 0;
    int sub_level = 0;
    for(i = start; i < end; i++) {
        switch(expr[i][0]) {
            case '(': sub_level++; break;
            case ')': sub_level--; break;
            default: break;
        }
        /* found the top level comma position */
        if(!sub_level && expr[i][0] == ',') {
            *pos = i;
            return SCRIPT_PASSED;
        }
    }
    return SCRIPT_FAILED;
}

int script_lexical(token_r * token, char * script) {
    int i = 0;
    int id = 0;                 /* indicate previous token is an identifer */
    int len = 0;                /* length of script */
    char * buf = NULL;          /* token buffer */
    char ** ptr = NULL;         /* token string pointers */
    int buf_cnt = 0;            /* track the token buffer index */
    int ptr_cnt = 0;            /* track the script buffer index */
    int comment_level = 0;      /* counter for multiline comment level */
    char * swap = NULL;         /* temporary value to swap string pointers */

    /* check null paramaters */
    if(exit_null_safe(2, token, script))
        return SCRIPT_FAILED;

    /* check script length */
    len = strlen(script);
    if(len >= BUF_SIZE) {
        exit_func_safe("script %s exceed buffer size of %d", script, BUF_SIZE);
        return SCRIPT_FAILED;
    }

    /* remove whitespace and newline */
    for(i = 0; i < len; i++)
        script[i] = (isspace(script[i])) ? ' ' : script[i];

    /* dereference token structure */
    buf = token->script;
    ptr = token->script_ptr;

    /* tokenize operators, identifiers, and syntax */
    for(i = 0; i < len; i++) {
        /* skipped multiline comments embedded in script */
        if(i+1 < len && script[i] == '/' && script[i+1] == '*')
            comment_level++;
        else if(i+1 < len && script[i] == '*' && script[i+1] == '/') {
            comment_level--;
            i += 2;
        }
        if(comment_level) continue;

        /* read an identifier and continue */
        if(ATHENA_SCRIPT_IDENTIFER(script[i]) || ATHENA_SCRIPT_SYMBOL(script[i])) {
            /* add identifier tokens */
            ptr[ptr_cnt++] = &buf[buf_cnt];
            while(ATHENA_SCRIPT_IDENTIFER(script[i]) || ATHENA_SCRIPT_SYMBOL(script[i]))
                buf[buf_cnt++] = script[i++];
            buf[buf_cnt++] = '\0';
            /* lexer skips a character with continue */
            i--;
            id = 1;
            continue;
        }

        /* skip space / single quote / double quote / slash characters */
        if(isspace(script[i]) || script[i] == '\'' || script[i] == '"' || script[i] == '\\')
            continue;

        /* replace assignment operator with set block */
        if( id && /* last token must be an identifier */
            (i+1 < len && script[i+1] != '=') &&
            (i-1 > 0 && script[i-1] != '=') &&
            script[i] == '=') {
            /* convert = into a set block */
            ptr[ptr_cnt] = &buf[buf_cnt];
            buf_cnt += sprintf(ptr[ptr_cnt], "set");
            buf[buf_cnt++] = '\0';

            /* swap identifier with the set identifier */
            swap = ptr[ptr_cnt];
            ptr[ptr_cnt] = ptr[ptr_cnt - 1];
            ptr[ptr_cnt - 1] = swap;
            ptr_cnt++;

            /* add the , operator or syntax */
            ptr[ptr_cnt++] = &buf[buf_cnt];
            buf[buf_cnt++] = ',';
            buf[buf_cnt++] = '\0';
        /* add the operator or syntax */
        } else {
            ptr[ptr_cnt++] = &buf[buf_cnt];
            buf[buf_cnt++] = script[i];
            buf[buf_cnt++] = '\0';
        }
        id = 0;
    }

    /* check that the script was tokenize */
    if(ptr_cnt <= 0) {
        exit_func_safe("script %s failed to tokenize with %d tokens", script, ptr_cnt);
        return SCRIPT_FAILED;
    }

    /* set the total number of tokens */
    token->script_cnt = ptr_cnt;
    return SCRIPT_PASSED;
}

int script_analysis(script_t * script, token_r * token_list, block_r * parent, block_r ** var) {
    int i = 0;
    int ret = 0;
    char ** token = NULL;
    int token_cnt = 0;
    block_db_t block_type;      /* block id in block_db.txt */
    block_r * block = NULL;     /* current block being parsed */
    block_r * link = parent;    /* link blocks to if, else-if, else, and for */
    block_r * set = (var != NULL) ? *var : NULL;

    /* check null paramaters */
    if(exit_null_safe(2, script, token_list))
        return SCRIPT_FAILED;

    /* dereference the token paramaters */
    token = token_list->script_ptr;
    token_cnt = token_list->script_cnt;

    /* check empty token list */
    if(token_cnt <= 0) {
        exit_abt_safe("failed to analysis empty token list");
        return SCRIPT_FAILED;
    }

    /* parse tokens to build blocks */
    for(i = 0; i < token_cnt; i++) {
        if(token[i][0] == '{' || token[i][0] == '}') continue;
        if(!block_keyword_search(script->db, &block_type, token[i])) {
            /* allocate and initialize a block */
            if(script_block_alloc(script, &block)) 
                return SCRIPT_FAILED;
            block->item_id = script->item_id;
            block->type = block_type.id;

            /* duplicate database references to prevent massive code rewrite */
            block->db = script->db;
            block->mode = script->mode;

            /* link to parent or link else-if and else with if blocks */
            link = (link != parent && link->type != 27) ? parent : link;
            block->link = link;

            /* link the end of the current set list; blocks will not 
             * search set blocks defined after it, but might within
             * loop, but I don't know. */
            block->set = set;

            /* parser does not set the first pointer */
            block->ptr[0] = block->arg;
            block->ptr_cnt++;

            /* retrieve all the arguments of the block */
            switch(block->type) {
                case 26: /* parse if blocks */
                    /* else-if and else will link this block */
                    link = block;
                case 62: /* parse for blocks */
                    /* parse the condition ( <condition> ) */
                    if(token[i+1][0] != '(') return SCRIPT_FAILED;
                    ret = script_parse(token_list, &i, block, '\0', ')', 0x01);
                    if(ret == SCRIPT_FAILED || token[i][0] != ')') 
                        return SCRIPT_FAILED;

                    /* compound or simple statement */
                    if((token[i+1][0] == '{') ?
                        script_parse(token_list, &i, block, '\0', '}', 0x01):
                        script_parse(token_list, &i, block, '\0', ';', 0x03)
                        == SCRIPT_FAILED)
                        return SCRIPT_FAILED;

                    /* extend the block list by using subscript */
                    if(script_extend_block(script, block->ptr[1], block, &set))
                        return SCRIPT_FAILED;
                    break;
                case 27: /* parse else blocks */
                    /* link else-if and else blocks */
                    block->link = link;

                    if(strcmp(token[i+1],"if") == 0) {
                        /* skip the 'if' */
                        i += 1;

                        /* parse the condition ( <condition> ) */
                        if(token[i+1][0] != '(') return SCRIPT_FAILED;
                        ret = script_parse(token_list, &i, block, '\0', ')', 0x01);
                        if(ret == SCRIPT_FAILED || token[i][0] != ')') 
                            return SCRIPT_FAILED;

                        /* compound or simple statement */
                        if((token[i+1][0] == '{') ?
                            script_parse(token_list, &i, block, '\0', '}', 0x01):
                            script_parse(token_list, &i, block, '\0', ';', 0x03) 
                            == SCRIPT_FAILED) 
                            return SCRIPT_FAILED;

                        /* extend the block list by using subscript */
                        if(script_extend_block(script, block->ptr[1], block, &set))
                            return SCRIPT_FAILED;

                        /* else if continues the linking */
                        link = block;
                    } else {
                        /* compound or simple statement */
                        if((token[i+1][0] == '{') ?
                            script_parse(token_list, &i, block, '\0', '}', 0x01):
                            script_parse(token_list, &i, block, '\0', ';', 0x03)
                            == SCRIPT_FAILED)
                            return SCRIPT_FAILED;

                        /* extend the block list by using subscript */
                        if(script_extend_block(script, block->ptr[0], block, &set))
                            return SCRIPT_FAILED;

                        /* else ends the local linking */
                        link = parent;
                    }
                    break;
                case 5:     /* autobonus */
                case 6:     /* autobonus2 */
                case 7:     /* autobonus3 */
                case 51:    /* bonus_script */
                    /* parse the blocks before extending */
                    if(script_parse(token_list, &i, block, ',', ';', 0x00))
                        return SCRIPT_FAILED;

                    /* extend the block list by using subscript */
                    if(script_extend_block(script, block->ptr[0], block, &set))
                        return SCRIPT_FAILED;
                    break;
                case 28:
                    /* variables have no enclosing scope within same script */
                    /* add new set block as the tail */
                    if(var != NULL) *var = block;
                    set = block;
                default: /* parse all blocks */
                    if(script_parse(token_list, &i, block, ',', ';', 0x00))
                        return SCRIPT_FAILED;
                    break;
            }
            /* parsing is off-by-one */
            block->ptr_cnt--;
        }
    }

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

    /* check null paramaters */
    if(exit_null_safe(3, token, position, block))
        return SCRIPT_FAILED;

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
         * flag 4 allows us to bypass any leveling */
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
        if(!ATHENA_SCRIPT_SYMBOL(script_ptr[i][0]) && script_ptr[i][0] != '=')
            arg[arg_cnt++] = ' ';
    }

    block->arg_cnt = arg_cnt;
    block->ptr_cnt = ptr_cnt;
    *position = i;
    return SCRIPT_PASSED;
}

int script_extend_block(script_t * script, char * subscript, block_r * parent, block_r ** var) {
    token_r token;
    /* lex subscript */
    if(script_lexical(&token, subscript)) {
        exit_func_safe("failed to lex on '%s' in item %d", subscript, script->item_id);
        return SCRIPT_FAILED;
    }

    /* check empty token list */
    if(token.script_cnt <= 0) {
        exit_func_safe("zero tokens on '%s' in item %d", subscript, script->item_id);
        return SCRIPT_FAILED;
    }

    /* indirect-recurisve analysis on subscript */
    if(script_analysis(script, &token, parent, var)) {
        exit_func_safe("failed to parse %s\n", subscript);
        return SCRIPT_FAILED;
    }
    return SCRIPT_PASSED;
}

int script_extend_paramater(block_r * block, char * expr) {
    int offset = 0;
    token_r token;

    /* lex the expression */
    if(script_lexical(&token, expr) == SCRIPT_FAILED) {
        exit_func_safe("failed to lex on '%s' in item %d", expr, block->item_id);
        return SCRIPT_FAILED;
    }

    /* check empty token list */
    if(token.script_cnt <= 0) {
        exit_func_safe("zero tokens on '%s' in item %d", expr, block->item_id);
        return SCRIPT_FAILED;
    }

    /* parse the callfunc paramaters */
    block->ptr_cnt++; /* last pointer still contains a argument */
    if(script_parse(&token, &offset, block, ',',')',4)) {
        exit_func_safe("failed to parsed on '%s' in item %d", expr, block->item_id);
        return SCRIPT_FAILED;
    }
    block->ptr_cnt--;
    return SCRIPT_PASSED;
}

int script_translate(script_t * script) {
    int ret = 0;
    block_r * iter = NULL;
    block_r * end = NULL;
    logic_node_t * logic_tree = NULL;

    /* check null paramaters */
    if(exit_null_safe(1, script))
        return SCRIPT_FAILED;

    /* translate each block */
    iter = script->block.head->next;
    end = script->block.head;
    while(iter != end) {
        /* linked child block inherits logic tree from parent block */
        if(iter->link != NULL && iter->link->logic_tree != NULL)
            iter->logic_tree = copy_deep_any_tree(iter->link->logic_tree);

        /* translate each block using a specific handle;
         * block numbers are define in the block_db.txt;
         * for example, 0 = bonus, 1 = bonus2, etc. */
        switch(iter->type) {
            case 0: ret = translate_bonus(iter, "bonus"); break;                                                    /* bonus */
            case 1: ret = translate_bonus(iter, "bonus2"); break;                                                   /* bonus2 */
            case 2: ret = translate_bonus(iter, "bonus3"); break;                                                   /* bonus3 */
            case 3: ret = translate_bonus(iter, "bonus4"); break;                                                   /* bonus4 */
            case 4: ret = translate_bonus(iter, "bonus5"); break;                                                   /* bonus5 */
            case 5: ret = translate_autobonus(iter, 0x01); break;                                                   /* autobonus */
            case 6: ret = translate_autobonus(iter, 0x02); break;                                                   /* autobonus2 */
            case 7: ret = translate_autobonus(iter, 0x04); break;                                                   /* autobonus3 */
            case 8: ret = translate_heal(iter, iter->type); break;                                                  /* heal */
            case 9: ret = translate_heal(iter, iter->type); break;                                                  /* percentheal */
            case 10: ret = translate_heal(iter, iter->type); break;                                                 /* itemheal */
            case 11: ret = translate_skill_block(iter, iter->type); break;                                          /* skill */
            case 12: ret = translate_skill_block(iter, iter->type); break;                                          /* itemskill */
            case 13: ret = translate_skill_block(iter, iter->type); break;                                          /* unitskilluseid */
            case 14: ret = translate_status(iter, iter->type); break;                                               /* sc_start */
            case 15: ret = translate_status(iter, iter->type); break;                                               /* sc_start4 */
            case 16: ret = translate_status_end(iter); break;                                                       /* sc_end */
            case 17: ret = translate_getitem(iter, iter->type); break;                                              /* getitem */
            case 18: ret = translate_rentitem(iter, iter->type); break;                                             /* rentitem */
            case 19: ret = translate_delitem(iter, iter->type); break;                                              /* delitem */
            case 20: ret = translate_getrandgroup(iter, iter->type); break;                                         /* getrandgroupitem */
            case 23: ret = translate_write(iter, "Set new font.", 1); break;                                        /* setfont */
            case 24: ret = translate_bstore(iter, iter->type); break;                                               /* buyingstore */
            case 25: ret = translate_bstore(iter, iter->type); break;                                               /* searchstores */
            case 30: ret = translate_write(iter, "Send a message through the announcement system.", 1); break;      /* announce */
            case 31: ret = translate_misc(iter, "callfunc"); break;                                                 /* callfunc */
            case 33: ret = translate_misc(iter, "warp"); break;                                                     /* warp */
            case 34: ret = translate_pet_egg(iter, iter->type); break;                                              /* pet */
            case 35: ret = translate_write(iter, "Hatch a pet egg.", 1); break;                                     /* bpet */
            case 36: ret = translate_hire_merc(iter, iter->type); break;                                            /* mercenary_create */
            case 37: ret = translate_heal(iter, iter->type); break;                                                 /* mercenary_heal */
            case 38: ret = translate_status(iter, iter->type); break;                                               /* mercenary_sc_status */
            case 39: ret = translate_produce(iter, iter->type); break;                                              /* produce */
            case 40: ret = translate_produce(iter, iter->type); break;                                              /* cooking */
            case 41: ret = translate_produce(iter, iter->type); break;                                              /* makerune */
            case 42: ret = translate_getexp(iter, iter->type); break;                                               /* getguildexp */
            case 43: ret = translate_getexp(iter, iter->type); break;                                               /* getexp */
            case 44: ret = translate_misc(iter, "monster"); break;                                                  /* monster */
            case 45: ret = translate_write(iter, "Evolve homunculus when requirements are met.", 1); break;         /* homevolution */
            case 46: ret = translate_write(iter, "Change to Summer Outfit when worn.", 1); break;                   /* setoption */
            case 47: ret = translate_write(iter, "Summon a creature to mount. [Work for all classes].", 1); break;  /* setmounting */
            case 48: ret = translate_setfalcon(iter); break;                                                        /* setfalcon */
            case 49: ret = translate_getrandgroup(iter, iter->type); break;                                         /* getgroupitem */
            case 50: ret = translate_write(iter, "Reset all status points.", 1); break;                             /* resetstatus */
            case 51: ret = translate_bonus_script(iter); break;                                                     /* bonus_script */
            case 52: ret = translate_write(iter, "Play another background song.", 1); break;                        /* playbgm */
            case 53: ret = translate_transform(iter); break;                                                        /* transform */
            case 54: ret = translate_status(iter, iter->type); break;                                               /* sc_start2 */
            case 55: ret = translate_petloot(iter); break;                                                          /* petloot */
            case 56: ret = translate_petrecovery(iter); break;                                                      /* petrecovery */
            case 57: ret = translate_petskillbonus(iter); break;                                                    /* petskillbonus */
            case 58: ret = translate_petskillattack(iter); break;                                                   /* petskillattack */
            case 59: ret = translate_petskillattack2(iter); break;                                                  /* petskillattack2 */
            case 60: ret = translate_petskillsupport(iter); break;                                                  /* petskillsupport */
            case 61: ret = translate_petheal(iter); break;                                                          /* petheal */
            /* non-simple structures */
            case 26: evaluate_expression(iter, iter->ptr[0], 1,
                     EVALUATE_FLAG_KEEP_LOGIC_TREE | EVALUATE_FLAG_EXPR_BOOL); 
                     break;                                                                                         /* if */
            case 27: /* invert the linked logic tree; only the top needs to be inverted */
                    if(iter->logic_tree != NULL) {
                        logic_tree = iter->logic_tree;
                        iter->logic_tree = inverse_logic_tree(iter->logic_tree);
                        iter->logic_tree->stack = logic_tree->stack;
                        freenamerange(logic_tree);
                    }

                    /* add the else if condition onto the stack */
                    if(iter->ptr_cnt > 1)
                        evaluate_expression(iter, iter->ptr[0], 1, EVALUATE_FLAG_KEEP_LOGIC_TREE | EVALUATE_FLAG_EXPR_BOOL);
                    break;                                                                                          /* else */
            case 28: iter->set_node = evaluate_expression(iter, iter->ptr[1], 1, 
                     EVALUATE_FLAG_KEEP_NODE| EVALUATE_FLAG_KEEP_LOGIC_TREE|
                     EVALUATE_FLAG_WRITE_FORMULA|EVALUATE_FLAG_KEEP_TEMP_TREE|
                     EVALUATE_FLAG_EXPR_BOOL); 
                     break;                                                                                         /* set */
            case 62: break;
            default: return SCRIPT_FAILED;
        }

        /* any failed translation indicate block cannot be translated */
        if(ret) return SCRIPT_FAILED;

        /* process next block */
        iter = iter->next;
    }
    return SCRIPT_PASSED;
}

/* for each non-nullified block, attempt to minimize with
 * blocks on the same enclosing block and bonus id */
int script_bonus(script_t * script) {
    int i = 0;
    int j = 0;
    int k = 0;

    block_r * iter = NULL;
    block_r * iter_sub = NULL;

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

    iter = script->block.head->next;
    if(iter == script->block.head) return SCRIPT_FAILED;
    for(; iter != script->block.head; iter = iter->next) {
        memset(total, 0, sizeof(int) * BONUS_SIZE);
        memset(offset, 0, sizeof(int) * BONUS_SIZE);
        cur_blk = iter;
        cur_blk_type = cur_blk->type;
        cur_blk_flag = cur_blk->bonus.flag;

        /* check block is a bonus block and 
         * not nullified and can be minimized */
        if(cur_blk_type > BONUS_ID_MAX ||
           cur_blk_type == BLOCK_NULLIFIED ||
           !(cur_blk_flag & 0x000C0000))
            continue;

        iter_sub = iter->next;
        for(; iter_sub != script->block.head; iter_sub = iter_sub->next) {
            cmp_blk = iter_sub;
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
            } else if(cur_blk_flag & BONUS_FLAG_MINIS) {
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
                        "[warn]: bonus duplicated on item id %d; in block "
                        "%d and %d; see dump.txt.\n", 
                        cur_blk->item_id, i, j);
                    iter_sub->type = BLOCK_NULLIFIED;
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
            iter_sub->type = BLOCK_NULLIFIED;
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
    iter = script->block.head->next;
    while(iter != script->block.head) {
        if(iter->type >= 0 && iter->type <= 4)
            if(translate_bonus_desc(iter->result, iter, &iter->bonus))
                return SCRIPT_FAILED;
        iter = iter->next;
    }
    
    return SCRIPT_PASSED;
}

int script_generate(script_t * script, char * buffer, int * offset) {
    int i = 0;
    int j = 0;
    int k = 0;
    int nest = 0;
    int sc_end_cnt = 0;
    char buf[BUF_SIZE];
    block_r * iter = NULL;
    block_r * iter_sub = NULL;

    if(exit_null_safe(3, script, buffer, offset))
        return SCRIPT_FAILED;

    iter = script->block.head->next;
    for(; iter != script->block.head; iter = iter->next) {
        /* calculate linkage nest */
        nest = script_linkage_count(iter, i);
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
        switch(iter->type) {
            case BLOCK_NULLIFIED: break; /* nullified block mean they're deleted */
            case 16: /* merge all sc_end into one block */
                /* figure out how many sc_end blocks there are */
                iter_sub = iter;
                for(; iter_sub != script->block.head; iter_sub = iter_sub->next)
                    if(iter_sub->type == 16)
                        sc_end_cnt++;
                    
                *offset += sprintf(buffer + *offset, "Cures ");
                /* if there exist only one sc_end, then assume the current one is the only one */
                if(sc_end_cnt == 1) {
                    *offset += sprintf(buffer + *offset, "%s", iter->eng[0]);
                } else {
                    *offset += sprintf(buffer + *offset, "%s", iter->eng[0]);
                    iter_sub = iter->next;
                    for(; iter_sub != script->block.head; iter_sub = iter_sub->next) {
                        /* steal the status effect */
                        if(iter_sub->type == 16) {
                            if(iter_sub->next == script->block.head) {
                                *offset += sprintf(buffer + *offset, ", %s", iter_sub->eng[0]);
                            } else {
                                *offset += sprintf(buffer + *offset, ", and %s", iter_sub->eng[0]);
                            }
                            /* nullified the block from being read */
                            iter_sub->type = BLOCK_NULLIFIED;
                        }
                    }
                }
                *offset += sprintf(buffer + *offset, " status effects.\n");
                
                break;
            /* blocks that might have other blocks linked to it */
            case 26: /* if */
            case 27: /* else */
                if(iter->logic_tree != NULL)
                    if(script_generate_cond(iter->logic_tree, stdout, buf, buffer, offset))
                        return SCRIPT_FAILED;
                break;
            case 28: /* set */
                /* special case for zeny */
                if(ncs_strcmp(iter->ptr[0], "zeny") == 0)
                    *offset += sprintf(buffer + *offset, "%sCurrent %s\n", buf, iter->eng[0]);
                break;
            /* ignore these blocks, since they have no interpretation */
            case 21: /* skilleffect */
            case 22: /* specialeffect2 */
            case 29: /* input */
            case 32: /* end */
                break;
            /* general block writing */
            default:        
                if(iter->eng_cnt > 0 && iter->eng[iter->eng_cnt - 1] != NULL)
                    *offset += sprintf(buffer + *offset, "%s%s\n", buf, iter->eng[iter->eng_cnt - 1]);
        }
    }
    buffer[*offset] = '\0';
    return SCRIPT_PASSED;
}

int script_generate_combo(int item_id, char * buffer, int * offset) {
    char * item_desc = NULL;
    ra_item_combo_t * item_combo_list = NULL;
    ra_item_combo_t * item_combo_iter = NULL;
    /* search the item id for combos */
    if(!ra_item_combo_search_id(global_db, &item_combo_list, item_id)) {
        item_combo_iter = item_combo_list;
        while(item_combo_iter != NULL) {
            *offset += sprintf(buffer + *offset, "%s\n", item_combo_iter->combo);
            /* compile each script write to buffer */
            item_desc = script_compile(item_combo_iter->script, -1);
            if(item_desc != NULL) {
                *offset += sprintf(buffer + *offset, "%s", item_desc);
                free(item_desc);
            }
            item_combo_iter = item_combo_iter->next;
        }
        buffer[*offset] = '\0';
        /* free the list of item combo */
        if(item_combo_list != NULL)
            free_combo(item_combo_list);
    }
    return SCRIPT_PASSED;
}

char * script_compile_raw(char * subscript, int item_id, FILE * dbg) {
    char buffer[BUF_SIZE];
    int offset = 0;
    script_t script;
    memset(&script, 0, sizeof(script_t));
    script.mode = global_mode;
    script.db = global_db;
    script.item_id = item_id;
    list_init_head(&script.block);
    /* compilation process per script */
    if(!script_lexical(&script.token, subscript))
        if(!script_analysis(&script, &script.token, NULL, NULL))
            if(!script_translate(&script))
                if(!script_bonus(&script))
                    if(!script_generate(&script, buffer, &offset))
                        if(!script_generate_combo(script.item_id, buffer, &offset))
                            if(dbg != NULL)
                                script_block_dump(&script, dbg);
    script_block_reset(&script);
    script_block_finalize(&script);
    return (offset <= 0) ? NULL : convert_string(buffer);
}

int translate_getitem(block_r * block, int handler) {
    int i = 0;
    char buf[BUF_SIZE];
    int offset = 0;
    token_r token;
    node_t * item_id = NULL;
    node_t * item_amount = NULL;

    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;

    /* support parenthesize getitem =.= */
    if(block->ptr[0][0] == '(') {
        if(split_paramater_list(&token, &i, block->ptr[0])) {
            exit_func_safe("failed to perform lexical analysis "
                "on %s in item %d", block->ptr[0], block->item_id);
            return SCRIPT_FAILED;
        }

        /* evaluate item_id and item_amount paramater for getitem */
        item_id = evaluate_expression_recursive(block, token.script_ptr, 1, i, block->logic_tree, 0);
        item_amount = evaluate_expression_recursive(block, token.script_ptr, i + 1, token.script_cnt - 1, block->logic_tree, 0);
        if(item_id == NULL || item_amount == NULL) {
            if(item_id != NULL) node_free(item_id);
            if(item_amount != NULL) node_free(item_amount);
            return SCRIPT_FAILED;
        }
        
        /* push the result on the translation stack */
        offset = sprintf(buf, "%d", minrange(item_id->range));
        buf[offset] = '\0';
        translate_write(block, buf, 1);

        offset = sprintf(buf, "%d", minrange(item_amount->range));
        buf[offset] = '\0';
        translate_write(block, buf, 1);
    } else {
        /* evaulate the expression because callfunc is called for some of the blocks */
        item_id = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
        item_amount = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE);
        if(item_id == NULL || item_amount == NULL) {
            if(item_id != NULL) node_free(item_id);
            if(item_amount != NULL) node_free(item_amount);
            return SCRIPT_FAILED;
        }
    }
    node_free(item_id);
    node_free(item_amount);

    /* special note; first condition is special case handling of values on stack */
    if(block->flag & 0x01) {
        offset += sprintf(buf,(block->flag & 0x02) ? "Randomly Select\n" : "Collect Items & Equipment");
        for(i = block->offset; i < block->ptr_cnt; i++) {
            if(translate_item(block, block->ptr[i]))
                return SCRIPT_FAILED;
            offset += sprintf(buf + offset,"=> %s\n", block->eng[block->eng_cnt - 1]);
        }
    } else {
        if(translate_item(block, block->eng[0]))
            return SCRIPT_FAILED;
        offset = (block->eng_cnt < 4) ?
            sprintf(buf, "Retrieve %s %s.", block->eng[1], block->eng[2]):
            sprintf(buf, "Retrieve %s %s.", block->eng[2], block->eng[3]);
    }
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_rentitem(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_item(block, block->ptr[0]) ||
       translate_time(block, block->ptr[1]))
        return SCRIPT_FAILED;
    offset = sprintf(buf,"Rent %s for %s.", block->eng[0], block->eng[2]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_delitem(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_item(block, block->ptr[0]))
        return SCRIPT_FAILED;
    offset = sprintf(buf,"Remove %s.", block->eng[0]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_getrandgroup(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * group_id = NULL;
    node_t * group_amount = NULL;
    
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;

    if(handler == 20) {
        if(block->ptr_cnt == 1) {
            /* getrandgroupitem specifies only group id */
            if(script_extend_paramater(block, block->ptr[0]) || 
               translate_const(block, block->ptr[1], 0x20)) 
                return SCRIPT_FAILED;
            group_id = evaluate_expression(block, block->ptr[2], 1, EVALUATE_FLAG_KEEP_NODE);
            if(group_id == NULL) return SCRIPT_FAILED;
            offset = sprintf(buf,"Collect %s item%s from %s.", block->eng[1], 
                (convert_integer(block->eng[1], 10) > 1)?"s":"", block->eng[0]);
        } else if(block->ptr_cnt == 2) {
            /* getrandgroupitem specifies only group id and amount */
            group_id = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
            group_amount = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE);
            if(group_id == NULL || group_amount == NULL) {
                if(group_id != NULL) node_free(group_id);
                if(group_amount != NULL) node_free(group_amount);
                return SCRIPT_FAILED;
            }
            if(translate_const(block, block->eng[0], 0x20))
                return SCRIPT_FAILED;
            offset = sprintf(buf,"Collect %s item%s from %s.", block->eng[1], 
                (convert_integer(block->eng[2], 10) > 1)?"s":"", block->eng[2]);
        }
    } else if(handler == 49) {
        if(script_extend_paramater(block, block->ptr[0])) 
            return SCRIPT_FAILED;
        group_id = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE);
        if(group_id == NULL) return SCRIPT_FAILED;
        if(translate_const(block, block->eng[0], 0x20))
            return SCRIPT_FAILED;
        offset = sprintf(buf,"Collect 1 item from %s.", block->eng[1]);
    } else {
        exit_func_safe("non-implemented getrandgroup in item %d", block->item_id);
        return SCRIPT_FAILED;
    }
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    if(group_id != NULL) node_free(group_id);
    if(group_amount != NULL) node_free(group_amount);
    return SCRIPT_PASSED;
}

int translate_bstore(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * slots = NULL;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    slots = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(slots == NULL) return SCRIPT_FAILED;
    offset = (handler == 25) ?
        sprintf(buf,"Allows up to %s search for stores.", block->eng[0]):
        sprintf(buf,"Open buying store with %s slots.", block->eng[0]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    node_free(slots);
    return SCRIPT_PASSED;
}

int translate_hire_merc(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_id(block, block->ptr[0], 0x04) ||
       translate_time(block, block->ptr[1]))
        return SCRIPT_FAILED;
    offset = sprintf(buf,"Hire %s for %s.", block->eng[0], block->eng[2]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_pet_egg(block_r * block, int handler) {
    int i = 0;
    int len = 0;
    int offset = 0;
    char buf[BUF_SIZE];
    ic_pet_t pet;
    node_t * pet_id = NULL;
    char * pet_script = NULL;
    char * loyal_script = NULL;
    int statement_count = 0;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;

    /* translate mob id to mob name */
    if(translate_id(block, block->ptr[0], 0x01))
        return SCRIPT_FAILED;
    offset = sprintf(buf,"Egg containing %s.", block->eng[0]);
    buf[offset] = '\0';
    
    /* convert the integer expression into constant integer result,
     * i.e. pet_id evaluated correctly and min and max are equal */
    pet_id = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(pet_id == NULL || pet_id->min != pet_id->max) {
        if(pet_id != NULL) node_free(pet_id);
        exit_func_safe("failed to search for pet id or pet id is not constant %d ~ %d "
            "in item %d\n", pet_id->min, pet_id->max, block->item_id);
        return SCRIPT_FAILED;
    }

    /* search for pet and loyal script using pet id */
    memset(&pet, 0, sizeof(ic_pet_t));
    if(pet_id_search(block->db, &pet, pet_id->min, block->mode)) {
        node_free(pet_id);
        /*exit_func_safe("failed to search for pet id %d in item %d", pet_id->min, block->item_id);*/
        return SCRIPT_FAILED;
    }

    /* check that pet script is not empty */
    statement_count = 0;
    len = strlen(pet.pet_script);
    for(i = 0; i < len; i++)
        if(pet.pet_script[i] == ';')
            statement_count++;
    if(statement_count > 0) {
        pet_script = script_compile(pet.pet_script, block->item_id);
        if(pet_script == NULL) {
            node_free(pet_id);
            exit_func_safe("failed to compile pet script %s in item %d", pet.pet_script, block->item_id);
            return SCRIPT_FAILED;
        }
        /* write the pet script */
        offset += sprintf(buf + offset, "\n[Pet Bonus]\n%s", pet_script);
        buf[offset] = '\0';
    }

    /* check that loyal script is not empty */
    statement_count = 0;
    len = strlen(pet.loyal_script);
    for(i = 0; i < len; i++)
        if(pet.loyal_script[i] == ';')
            statement_count++;
    if(statement_count > 0) {
        /* compile the loyal script */
        loyal_script = script_compile(pet.loyal_script, block->item_id);
        if(loyal_script == NULL) {
            node_free(pet_id);
            exit_func_safe("failed to compile loyal script %s in item %d", pet.loyal_script, block->item_id);
            return SCRIPT_FAILED;
        }
            
        /* write the loyal script */
        offset += sprintf(buf + offset, "\n[Loyal Bonus]\n%s", loyal_script);   
        buf[offset] = '\0';
    }

    /* write the monster pet name, pet script and loyal script */
    translate_write(block, buf, 1);

    /* free the scripts and result node */
    if(pet_id != NULL) node_free(pet_id);
    if(pet_script != NULL) free(pet_script);
    if(loyal_script != NULL) free(loyal_script);
    return SCRIPT_PASSED;
}

int translate_getexp(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * exp_amount = NULL;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    exp_amount = evaluate_expression(block, block->ptr[0], 1, 
        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA);
    if(exp_amount != NULL) {
        offset = sprintf(buf,"Gain %s %s experience.", 
                 status_formula(&exp_amount->stack[exp_amount->
                    stack_cnt], block->eng[1], exp_amount, 'n', 1),
                 (handler == 43)?"player":"guild");
        buf[offset] = '\0';
        translate_write(block, buf, 1);
        node_free(exp_amount);
    } else {
        return SCRIPT_FAILED;
    }
    return SCRIPT_PASSED;
}

int translate_transform(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    /* support both mob id and name */
    if(isdigit(block->ptr[0][0])) {
        if(translate_id(block, block->ptr[0], 0x01) ||
           translate_time(block, block->ptr[1]))
            return SCRIPT_FAILED;
        offset = sprintf(buf, "Transform into a %s for %s.", block->eng[0], block->eng[2]);
    } else {
        if(translate_write(block, block->ptr[0], 1) ||
           translate_time(block, block->ptr[1]))
            return SCRIPT_FAILED;
        offset = sprintf(buf, "Transform into a %s for %s.", block->eng[0], block->eng[2]);
    }
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_skill_block(block_r * block, int handler) {
    char buf[BUF_SIZE];     /* description */
    char aux[BUF_SIZE];     /* formula */
    node_t * level = NULL; /* formula expression */
    int offset = 1;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    /* skip GID for unitskilluseid */
    offset = (handler == 13)?1:0;
    /* translate skill and level */
    if(translate_skill(block, block->ptr[offset]))
        return SCRIPT_FAILED;
    /* check whether level has a formula or not */
    level = evaluate_expression(block, block->ptr[offset+1], 1, 
        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA);
    if(level == NULL) return SCRIPT_FAILED;
    /* enable for skill block and cast for others;
     * add support for formula on level. */
    offset += (handler == 11) ?
        sprintf(buf,"Enable %s [Lv. %s]",block->eng[0], formula(aux, block->eng[1], level)):
        sprintf(buf,"Cast %s [Lv. %s]",block->eng[0], formula(aux, block->eng[1], level));
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    if(level != NULL) node_free(level);
    return SCRIPT_PASSED;
}

int translate_heal(block_r * block, int handler) {
    char buf[BUF_SIZE];
    /*char aux[BUF_SIZE];*/
    int offset = 1;
    node_t * hp = NULL;
    node_t * sp = NULL;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    hp = evaluate_expression(block, block->ptr[0], 1, 
        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA);
    sp = evaluate_expression(block, block->ptr[1], 1, 
        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA);
    if(hp == NULL || sp == NULL) {
        if(hp != NULL) node_free(hp);
        if(sp != NULL) node_free(sp);
        return SCRIPT_FAILED;
    }
    if(!CHECK_ZERO(block->eng[0])) 
        offset += sprintf(buf,"%s %sHP by %s%s.",
            (CHECK_NEGATIVE(block->eng[0])?"Drain":"Recover"), 
            (handler == 37)?"Mercenary ":"",
            /*formula(aux, block->eng[0], hp),*/
            block->eng[0],
            (handler == 9)?"%":"");
    if(!CHECK_ZERO(block->eng[1])) 
        offset += sprintf(buf + offset - 1,"%s%s SP by %s%s.",
            (!CHECK_ZERO(block->eng[0])?" ":""),
            (CHECK_NEGATIVE(block->eng[1])?"Drain":"Recover"), 
            /*formula(aux, block->eng[1], sp),*/
            block->eng[1],
            (handler == 9)?"%":"");
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    node_free(hp);
    node_free(sp);
    return SCRIPT_PASSED;
}

int translate_bonus(block_r * block, char * bonus) {
    int i = 0;
    int j = 0;
    int ret = 0;
    static int flag = EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA|EVALUATE_FLAG_KEEP_LOGIC_TREE;
    /* check null paramater */
    if(exit_null_safe(2, block, bonus))
        return SCRIPT_FAILED;

    /* bonus information and integer results are kept for minimizing and post processing */
    memset(block->result, 0, sizeof(node_t *) * BONUS_SIZE);
    if(bonus_name_search(block->db, &block->bonus, bonus, block->ptr[0])) {
        exit_func_safe("failed to search for bonus %s for item %d", block->ptr[0], block->item_id);
        return SCRIPT_FAILED;
    }

    /* translate each argument individually 
        i = subscript the bonus type array
        j = offset and subscript the block argument array (ptr)
        j = 0; is the buff identifier
        j = 1; is where the arguments start
    */
    for(i = 0, j = 1; i < block->bonus.type_cnt; i++, j++) {
        switch(block->bonus.type[i]) {
            case 'n': block->result[i] = evaluate_expression(block, block->ptr[j], 1, flag);    break; /* Integer Value */
            case 'p': block->result[i] = evaluate_expression(block, block->ptr[j], 1, flag);    break; /* Integer Percentage */
            case 'r': ret = translate_const(block, block->ptr[j], 0x01);                        break; /* Race */
            case 'l': ret = translate_const(block, block->ptr[j], 0x02);                        break; /* Element */
            case 'w': ret = translate_splash(block, block->ptr[j]);                             break; /* Splash */
            case 'z': block->eng_cnt++;                                                         break; /* Meaningless */
            case 'e': ret = translate_const(block, block->ptr[j], 0x04);                        break; /* Effect */
            case 'q': block->result[i] = evaluate_expression(block, block->ptr[j], 100, flag);  break; /* Integer Percentage / 100 */
            case 'k': ret = translate_skill(block, block->ptr[j]);                              break; /* Skill */
            case 's': ret = translate_const(block, block->ptr[j], 0x08);                        break; /* Size */
            case 'c': ret = translate_id(block, block->ptr[j], 0x01);                           break; /* Monster Class & Job ID * Monster ID */
            case 'o': block->result[i] = evaluate_expression(block, block->ptr[j], 10, flag);   break; /* Integer Percentage / 10 */
            case 'm': ret = translate_item(block, block->ptr[j]);                               break; /* Item ID */
            case 'x': block->result[i] = evaluate_expression(block, block->ptr[j], 1, flag);    break; /* Level */
            case 'g': ret = translate_tbl(block, block->ptr[j], 0x01);                          break; /* Regen */
            case 'a': block->result[i] = evaluate_expression(block, block->ptr[j], 1000, flag); break; /* Millisecond */
            case 'h': block->result[i] = evaluate_expression(block, block->ptr[j], 1, flag);    break; /* SP Gain Bool */
            case 'v': ret = translate_tbl(block, block->ptr[j], 0x04);                          break; /* Cast Self, Enemy */
            case 't': ret = translate_trigger(block, block->ptr[j], 1);                         break; /* Trigger */
            case 'y': ret = translate_item(block, block->ptr[j]);                               break; /* Item Group */
            case 'd': ret = translate_trigger(block, block->ptr[j], 2);                         break; /* Triger ATF */
            case 'f': block->result[i] = evaluate_expression(block, block->ptr[j], 1, flag);    break; /* Cell */
            case 'b': ret = translate_tbl(block, block->ptr[j], 0x08);                          break; /* Flag Bitfield */
            case 'i': ret = translate_tbl(block, block->ptr[j], 0x10);                          break; /* Weapon Type */
            case 'j': ret = translate_const(block, block->ptr[j], 0x10);                        break; /* Class Group */
            default: break;
        }

        switch(block->bonus.type[i]) {
            /* check for error on integer arguments */
            case 'n': case 'p': case 'q': case 'o':
            case 'x': case 'a': case 'h': case 'f':
                if(block->result[i] == NULL)
                    return SCRIPT_FAILED;
            default:
                if(ret == SCRIPT_FAILED)
                    return SCRIPT_FAILED;
        }
    }
    return SCRIPT_PASSED;
}

int translate_const(block_r * block, char * expr, int flag) {
    int tbl_index = 0;
    char * tbl_str = NULL;
    /*char buffer[BUF_SIZE];*/
    ic_const_t const_info;
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;

    if(isdigit(expr[0])) {
        tbl_index = convert_integer(expr,10);
    } else {
        if(const_keyword_search(block->db, &const_info, expr, block->mode)) {
            exit_func_safe("failed to search constant %s for %d", expr, block->item_id);
            return SCRIPT_FAILED;
        }
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
        /*if(block->mode == EATHENA) {
            if(ea_item_group_search(block->db, tbl_index, block->mode, buffer))
                return SCRIPT_FAILED;
            tbl_str = buffer;
        } else if(block->mode == RATHENA) {
            if(ra_item_group_search(block->db, tbl_index, block->mode, buffer))
                return SCRIPT_FAILED;
            tbl_str = buffer;
        } else {
            tbl_str = itemgrp_tbl(tbl_index);
        }*/
        tbl_str = "a box"; /* lol */
    }
 
    /* indicate unresolve item group; there are many I am too lazy to add */
    if(strcmp(tbl_str,"error") == 0)
        fprintf(stderr, "[warn]: failed to map constant %d in "
            "item %d on flag %d\n", tbl_index, block->item_id, flag);

    /* check for invalid index */
    if(tbl_str == NULL) {
        exit_func_safe("failed to resolve constant %s for %d", expr, block->item_id);
        return SCRIPT_FAILED;
    }
    translate_write(block, tbl_str, 0x01);
    return SCRIPT_PASSED;
}

int translate_skill(block_r * block, char * expr) {
    int offset = 0;
    char buf[BUF_SIZE];
    ic_skill_t skill_info;
    memset(&skill_info, 0, sizeof(ic_skill_t));
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;
    if(((isdigit(expr[0]))) ?
        skill_name_search_id(block->db, &skill_info, convert_integer(expr, 10), block->mode):
        skill_name_search(block->db, &skill_info, expr, block->mode)) {
        exit_func_safe("failed to resolve skill %s for %d", expr, block->item_id);
    }
    offset = sprintf(buf, "%s", skill_info.desc);
    buf[offset] = '\0';
    translate_write(block, buf, 0x01);
    return SCRIPT_PASSED;
}

int translate_tbl(block_r * block, char * expr, int flag) {
    char * id_str = NULL;
    int id = 0;
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;
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
    if(id_str == NULL || strcmp(id_str,"error") == 0) {
        exit_func_safe("failed to resolve constant %s for %d", expr, block->item_id);
        return SCRIPT_FAILED;
    }
    translate_write(block, id_str, 0x01);
    return SCRIPT_PASSED;
}

int translate_splash(block_r * block, char * expr) {
   int splash = 0;
   char buf[BUF_SIZE];
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;
   /* convert the splash to status; adding diagonal block on both side */
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
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;

    /* evalute the trigger to bit-mask value */
    result = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(result == NULL) {
        exit_func_safe("failed to evaluate '%s' for item %d\n", expr, block->item_id);
        return SCRIPT_FAILED;
    } else {
        trigger = result->max;
        node_free(result);
    }

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
                strcat(trigger_buf," on self");
            } else {
                strcat(trigger_buf," on enemy");
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
        default: return SCRIPT_FAILED;
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
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;

    /* evalute the expression and convert to time string */
    result = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
    if(result == NULL) return SCRIPT_FAILED;

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
    ic_merc_t merc;
    ic_mob_t mob;
    ic_item_t item;
    memset(&merc, 0, sizeof(ic_merc_t));
    memset(&mob, 0, sizeof(ic_mob_t));
    memset(&item, 0, sizeof(ic_item_t));

    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;

    id = convert_integer(expr, 10);
    if(flag & 0x01 && !mob_id_search(block->db, &mob, id, block->mode)) {
        translate_write(block, mob.iro, 0x01);
        return SCRIPT_PASSED;
    }

    if(flag & 0x02 && !item_name_id_search(block->db, &item, id, block->mode)) {
        translate_write(block, item.name, 0x01);
        return SCRIPT_PASSED;
    }

    if(flag & 0x04 && !merc_id_search(block->db, &merc, id, block->mode)) {
        translate_write(block, merc.name, 0x01);
        return SCRIPT_PASSED;
    }
    /* last resort assuming that it could be a constant */
    if(translate_const(block, expr, 0xFF) == SCRIPT_PASSED)
        return SCRIPT_PASSED;
    exit_func_safe("failed to resolve id %s for %d", expr, block->item_id);
    return SCRIPT_FAILED;
}

int translate_item(block_r * block, char * expr) {
    ic_item_t item;
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;
    /* search the item id if literal */
    if(isdigit(expr[0]))
        return translate_id(block, expr, 0x02);
    /* search the const id if identifer */
    else if(isalpha(expr[0])) {
        memset(&item, 0, sizeof(ic_item_t));
        if(item_name_search(block->db, &item, expr, block->mode))
            return translate_const(block, expr, 0x20);
        translate_write(block, item.name, 0x01);
    } else {
        exit_func_safe("failed to resolve item or const id %s for %d", expr, block->item_id);
        return SCRIPT_FAILED;
    }
    return SCRIPT_PASSED;
}

int translate_autobonus(block_r * block, int flag) {
    int offset = 0;
    char buf[BUF_SIZE];
    char aux[BUF_SIZE];
    node_t * rate = NULL;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    rate = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
    if(rate == NULL) return SCRIPT_FAILED;
    translate_bonus_float_percentage(block, rate, &offset, 100);
    if(translate_time(block, block->ptr[2]) == SCRIPT_FAILED)
        return SCRIPT_FAILED;
    /* translate for autobonus and autobonus2 */
    if(flag & 0x01 || flag & 0x02) {
        if(block->ptr_cnt > 3) 
            if(translate_trigger(block, block->ptr[3], 1) == SCRIPT_FAILED)
                return SCRIPT_FAILED;
        offset = sprintf(buf,"Add %s chance to autobonus for %s when attacked.%s",
            formula(aux, block->eng[1], rate), block->eng[3], (block->eng[5] != NULL)?block->eng[5]:"");
    }
 
    if(flag & 0x04) {
        if(block->ptr_cnt > 3) 
            if(translate_skill(block, block->ptr[3]) == SCRIPT_FAILED)
                return SCRIPT_FAILED;
        offset = sprintf(buf,"Add %s chance to autobonus for %s when using skill %s.",
            formula(aux, block->eng[1], rate), block->eng[3], (block->eng[5] != NULL)?block->eng[5]:"");
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
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;
    
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
            exit_func_safe("unsupported warp map %s in %d", block->ptr[0], block->item_id);
            return SCRIPT_FAILED;
        }
        buf[offset] = '\0';
        translate_write(block, buf, 1);
    }
    
    if(ncs_strcmp(expr,"monster") == 0) {
        result = evaluate_expression(block, block->ptr[4], 1, EVALUATE_FLAG_KEEP_NODE);
        if(result == NULL) return SCRIPT_FAILED;
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
                default: 
                    exit_func_safe("unsupported monster type %d in %d", type, block->item_id); 
                    return SCRIPT_FAILED;
            }
        buf[offset] = '\0';
        translate_write(block, buf, 1);
    }

    if(ncs_strcmp(expr,"callfunc") == 0) {
        if(ncs_strcmp(block->ptr[0],"F_CashCity") == 0) {
            result = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE);
            if(result == NULL) return SCRIPT_FAILED;
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
            if(result == NULL) return SCRIPT_FAILED;
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
            exit_func_safe("unsupported callfunc function %s in item %d", block->ptr[0], block->item_id);
            return SCRIPT_FAILED;
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
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;

    /* evaluate the item level */
    result = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(result == NULL) return SCRIPT_FAILED;
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
    if(prod_lv_search(block->db, &produce, item_lv, block->mode)) {
        node_free(result);
        return SCRIPT_FAILED;
    }

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
    int ret_value = 0;
    ic_item_t item;
    ic_status_t status;
    ic_const_t const_info;
    node_t * result[BONUS_SIZE];
    node_t * rate = NULL;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    memset(&item, 0, sizeof(ic_item_t));
    memset(&status, 0, sizeof(ic_status_t));
    memset(&result, 0, sizeof(node_t *) * BONUS_SIZE);

    if(const_keyword_search(block->db, &const_info, block->ptr[0], block->mode)) {
        exit_func_safe("failed to search const %s in item %d", block->ptr[0], block->item_id);
        return SCRIPT_FAILED;
    }

    if(status_id_search(block->db, &status, const_info.value, block->ptr[0])) {
        if(block->mode == HERCULES) {
            /* Hercules decided to change the status constants by + or - 1; what the fuck? */
            if(status_id_search(block->db, &status, const_info.value - 1, block->ptr[0]))
                if(status_id_search(block->db, &status, const_info.value + 1, block->ptr[0])) {
                    exit_func_safe("failed to search status id %d name %s in item %d", 
                        const_info.value, block->ptr[0], block->item_id);
                    return SCRIPT_FAILED;
                }
        } else {
            exit_func_safe("failed to search status id %d name %s in item %d", 
                const_info.value, block->ptr[0], block->item_id);
            return SCRIPT_FAILED;
        }
    }

    /* evaluate the duration of the status */
    if(translate_time(block, block->ptr[1]))
        return SCRIPT_FAILED;

    if(ncs_strcmp(status.scstr, "sc_itemscript") != 0) {
        /* evaluate the val1, val2, ... , valN */
        for(i = 0; i < status.vcnt; i++) {
            switch(status.vmod[i]) {
                case 'n': 
                        result[i] = evaluate_expression(block, block->ptr[2+i], 1, 
                        EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);  
                        if(result[i] == NULL) goto status_failed;
                        break; /* Integer Value */
                case 'm': 
                        result[i] = evaluate_expression(block, block->ptr[2+i], 1, 
                        EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);  
                        if(result[i] == NULL) goto status_failed;
                        break; /* Integer Value */
                case 'p': 
                        result[i] = evaluate_expression(block, block->ptr[2+i], 1, 
                        EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
                        if(result[i] == NULL) goto status_failed;
                        break; /* Integer Percentage */
                case 'e': 
                        if(translate_const(block, block->ptr[2+i], 0x02) == SCRIPT_FAILED)
                            goto status_failed;
                        break; /* Effect */
                case 'u': 
                        ret_value = block->eng_cnt;
                        evaluate_expression(block, block->ptr[2+i], -1, EVALUATE_FLAG_WRITE_FORMULA); 
                        if(ret_value + 1 != block->eng_cnt)
                            goto status_failed;
                        break; /* Integer Value */
                default: 
                    exit_func_safe("invalid bonus type %d in item %d", status.vmod[i], block->item_id);
                    goto status_failed;
            }
        }

        /* I like things unnecessarily complicated =.=; this can be improved */
        switch(status.vcnt) {
            case 0: 
                off += sprintf(buf + off,"%s",status.scfmt);
                break;
            case 1: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff[0]], result[0], status.vmod[0], status.voff[0]));
                break;
            case 2: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff[0]], result[0], status.vmod[0], status.voff[0]), 
                status_formula(aux, block->eng[status.voff[1]], result[1], status.vmod[1], status.voff[1]));
                break;
            case 3: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff[0]], result[0], status.vmod[0], status.voff[0]), 
                status_formula(aux, block->eng[status.voff[1]], result[1], status.vmod[1], status.voff[1]), 
                status_formula(aux, block->eng[status.voff[2]], result[2], status.vmod[2], status.voff[2]));
                break;
            case 4: 
                off += sprintf(buf + off,status.scfmt, 
                status_formula(aux, block->eng[status.voff[0]], result[0], status.vmod[0], status.voff[0]), 
                status_formula(aux, block->eng[status.voff[1]], result[1], status.vmod[1], status.voff[1]), 
                status_formula(aux, block->eng[status.voff[2]], result[2], status.vmod[2], status.voff[2]), 
                status_formula(aux, block->eng[status.voff[3]], result[3], status.vmod[3], status.voff[3]));
                break;
            default: return SCRIPT_FAILED;
        }

        /* write the duration */
        off += sprintf(buf + off, " for %s",block->eng[1]);
        if(block->ptr_cnt == 5) {
            rate = evaluate_expression(block, block->ptr[3], 100, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA); /* rate */
            if(rate == NULL) return SCRIPT_FAILED;
            if(rate->min != 0 && rate->max != 0)
                off += sprintf(buf + off," with %s%% chance",block->eng[block->eng_cnt - 1]);
            node_free(rate);
        }
        off += sprintf(buf + off,".");
        buf[off] = '\0';
    } else {
        /* translate an item script within an item script */
        if(item_name_id_search(block->db, &item, convert_integer(block->ptr[2], 10), block->mode)) {
            exit_func_safe("failed to search item id %d in item %s (sc_itemscript)", block->item_id, block->ptr[2]);
            return SCRIPT_FAILED;
        }
        item_script = script_compile(item.script, item.id);
        if(item_script == NULL) return SCRIPT_FAILED;
        off += sprintf(buf + off, "Status Effect [Duration: %s]\n%s", block->eng[1], item_script);
        buf[off] = '\0';
        free(item_script);
        
    }

    /* write the translation only the buffer is filled with something */
    if(off <= 0) {
        exit_func_safe("failed to translate status %s of item %d", block->ptr[0], block->item_id);
        return SCRIPT_FAILED;
    }
    for(i = 0; i < BONUS_SIZE; i++)
        if(result[i] != NULL) node_free(result[i]);
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;

    status_failed:
        for(i = 0; i < BONUS_SIZE; i++)
            if(result[i] != NULL) node_free(result[i]);
        return SCRIPT_FAILED;
}

int translate_status_end(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    ic_status_t status;
    ic_const_t const_info;
    memset(&status, 0, sizeof(ic_status_t));
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;

    if(const_keyword_search(block->db, &const_info, block->ptr[0], block->mode)) {
        exit_func_safe("failed to search const %s in "
            "item %d", block->ptr[0], block->item_id);
        return SCRIPT_FAILED;
    }

    if(status_id_search(block->db, &status, const_info.value, block->ptr[0])) {
        exit_func_safe("failed to search status id %d name %s in "
            "item %d", const_info.value, block->ptr[0], block->item_id);
        return SCRIPT_FAILED;
    }
    translate_write(block, status.scend, 1);
    offset = sprintf(buf,"Cures %s status effect.", status.scend);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_bonus_script(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * duration = NULL;
    node_t * type = NULL;
    node_t * flag = NULL;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    duration = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE);
    if(duration == NULL) return SCRIPT_FAILED;
    if(duration->min != duration->max) {
        node_free(duration);
        exit_func_safe("unsupported expression; duration dependence; %d item id", block->item_id);
        return SCRIPT_FAILED;
    }
    offset += sprintf(buf + offset,"Unstackable bonus lasting for %d seconds ", duration->min);
    if(block->ptr_cnt >= 3) {
        type = evaluate_expression(block, block->ptr[2], 1, EVALUATE_FLAG_KEEP_NODE); /* flag */
        if(type == NULL) {
            node_free(duration);
            return SCRIPT_FAILED;
        }
        if(type->min != type->max) {
            exit_func_safe("unsupported expression; type dependence; %d item id.", block->item_id);
            node_free(duration);
            node_free(type);
            return SCRIPT_FAILED;
        }
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
        if(flag == NULL) {
            node_free(duration);
            node_free(type);
            return SCRIPT_FAILED;
        }
        if(flag->min != flag->max) {
            exit_func_safe("unsupported expression; flag dependence; %d item id", block->item_id);
            node_free(duration);
            node_free(type);
            node_free(flag);
            return SCRIPT_FAILED;
        }
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
    int ret = 0;
    node_t * result = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    result = evaluate_argument(block, block->ptr[0]);
    ret = (result != NULL) ? SCRIPT_PASSED : SCRIPT_FAILED;
    if(ret == SCRIPT_PASSED) {
        (result->max == 0) ?
            translate_write(block, "Recall a falcon.", 1):
            translate_write(block, "Summon a falcon.", 1);
    }
    if(result != NULL) node_free(result);
    return ret;
}

int translate_petloot(block_r * block) {
    int ret = 0;
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * capacity = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    capacity = evaluate_argument(block, block->ptr[0]);
    ret = (capacity != NULL) ? SCRIPT_PASSED : SCRIPT_FAILED;
    if(ret == SCRIPT_PASSED) {
        offset += sprintf(buf,"Pet can loot and hold upto %s items.\n"
            "Use pet performance to retrieve the items.", 
            status_formula(&capacity->stack[capacity->stack_cnt], block->eng[0], capacity, 'n', 1));
        buf[offset] = '\0';
        translate_write(block, buf, 1);
    }
    if(capacity != NULL) node_free(capacity);
    return ret;
}

int translate_petrecovery(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * delay = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_status_end(block) == SCRIPT_FAILED)
        return SCRIPT_FAILED;
    /* evaluate the delay expression */
    delay = evaluate_argument(block, block->ptr[1]);
    if(delay == NULL) return SCRIPT_FAILED;
    offset += sprintf(buf, "Pet cures %s every %d seconds.", block->eng[0], delay->min);
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    node_free(delay);
    return SCRIPT_PASSED;
}

/* require refactoring */
int translate_petskillbonus(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    char aux[BUF_SIZE];
    char aux2[BUF_SIZE];
    node_t * duration = NULL;
    node_t * delay = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_bonus(block, "bonus") == SCRIPT_FAILED)
        return SCRIPT_FAILED;
    duration = evaluate_argument(block, block->ptr[2]);
    delay = evaluate_argument(block, block->ptr[3]);
    if(duration == NULL || delay == NULL) {
        if(duration != NULL) node_free(duration);
        if(delay != NULL) node_free(delay);
        return SCRIPT_FAILED;
    }
    offset += sprintf(buf, "Additional pet bonus lasting %s seconds for every %s seconds:\n -> ",
        formula(aux, block->eng[1], duration),
        formula(aux2, block->eng[2], delay));
    if(block->bonus.id == 8)
        translate_bonus_template(buf + offset, &offset, block->bonus.format, 
            formula(aux, block->eng[0], block->result[0]),
            formula(aux, block->eng[0], block->result[0]));    
    else if(block->bonus.id == 9)
        translate_bonus_template(buf + offset, &offset, block->bonus.format, 
            formula(aux, block->eng[0], block->result[0]),
            formula(aux, block->eng[0], block->result[0]),
            formula(aux, block->eng[0], block->result[0]));    
    else
        translate_bonus_template(buf + offset, &offset, block->bonus.format, 
            formula(aux, block->eng[0], block->result[0]));
    node_free(duration);
    node_free(delay);
    translate_write(block, buf, 1);
    return SCRIPT_PASSED;
}

int translate_petskillattack(block_r * block) {
    int ret = 0;
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * level = NULL;
    node_t * rate = NULL;
    node_t * brate = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_skill(block, block->ptr[0]) == SCRIPT_FAILED)
        return SCRIPT_FAILED;
    level = evaluate_argument(block, block->ptr[1]);
    rate = evaluate_argument(block, block->ptr[2]);
    brate = evaluate_argument(block, block->ptr[3]);
    ret = (level != NULL && rate != NULL && brate != NULL) ? SCRIPT_PASSED : SCRIPT_FAILED;
    if(ret == SCRIPT_PASSED) {
        offset = sprintf(buf, "Add %s (%s on max intimacy) chance of casting %s [Lv. %s].",
            status_formula(&rate->stack[rate->stack_cnt], block->eng[2], rate, 'p', 1),
            status_formula(&brate->stack[brate->stack_cnt], block->eng[3], brate, 'p', 1),
            block->eng[0],
            formula(&level->stack[level->stack_cnt], block->eng[1], level));
        translate_write(block, buf, 1);
    }
    if(level != NULL) node_free(level);
    if(rate != NULL) node_free(rate);
    if(brate != NULL) node_free(brate);
    return ret;
}

int translate_petskillattack2(block_r * block) {
    int ret = 0;
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * damage = NULL;
    node_t * hit = NULL;
    node_t * rate = NULL;
    node_t * brate = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_skill(block, block->ptr[0]) == SCRIPT_FAILED)
        return SCRIPT_FAILED;
    damage = evaluate_argument(block, block->ptr[1]);
    hit = evaluate_argument(block, block->ptr[2]);
    rate = evaluate_argument(block, block->ptr[3]);
    brate = evaluate_argument(block, block->ptr[4]);
    ret = (damage != NULL && hit != NULL && rate != NULL && brate != NULL) ? SCRIPT_PASSED : SCRIPT_FAILED;
    if(ret == SCRIPT_PASSED) {
        offset = sprintf(buf, "Add %s (%s on max intimacy) chance of casting %s for %s damage x %s hits.",
            status_formula(&rate->stack[rate->stack_cnt], block->eng[3], rate, 'p', 1),
            status_formula(&brate->stack[brate->stack_cnt], block->eng[4], brate, 'p', 1),
            block->eng[0],
            status_formula(&damage->stack[damage->stack_cnt], block->eng[1], damage, 'n', 1),
            status_formula(&hit->stack[hit->stack_cnt], block->eng[2], hit, 'n', 1));
        translate_write(block, buf, 1);
    }
    if(damage != NULL) node_free(damage);
    if(hit != NULL) node_free(hit);
    if(rate != NULL) node_free(rate);
    if(brate != NULL) node_free(brate);
    return ret;
}

int translate_petskillsupport(block_r * block) {
    int ret = 0;
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * level = NULL;
    node_t * delay = NULL;
    node_t * hp = NULL;
    node_t * sp = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_skill(block, block->ptr[0]) == SCRIPT_FAILED)
        return SCRIPT_FAILED;
    level = evaluate_argument(block, block->ptr[1]);
    delay = evaluate_argument(block, block->ptr[2]);
    hp = evaluate_argument(block, block->ptr[3]);
    sp = evaluate_argument(block, block->ptr[4]);
    ret = (level != NULL && delay != NULL && hp != NULL && sp != NULL) ? SCRIPT_PASSED : SCRIPT_FAILED;
    if(ret == SCRIPT_PASSED) {
        offset = sprintf(buf, "Cast %s [Lv. %s] when pet owner's hp is below %s and sp is below %s for every %s seconds.",
            block->eng[0],
            status_formula(&level->stack[level->stack_cnt], block->eng[1], level, 'n', 1),
            status_formula(&hp->stack[hp->stack_cnt], block->eng[3], hp, 'p', 1),
            status_formula(&sp->stack[sp->stack_cnt], block->eng[4], sp, 'p', 1),
            status_formula(&delay->stack[delay->stack_cnt], block->eng[2], delay, 'n', 1));
        translate_write(block, buf, 1);
    }
    if(level != NULL) node_free(level);
    if(delay != NULL) node_free(delay);
    if(hp != NULL) node_free(hp);
    if(sp != NULL) node_free(sp);
    return ret;
}

int translate_petheal(block_r * block) {
    int ret = 0;
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * level = NULL;
    node_t * delay = NULL;
    node_t * hp = NULL;
    node_t * sp = NULL;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    level = evaluate_argument(block, block->ptr[0]);
    delay = evaluate_argument(block, block->ptr[1]);
    hp = evaluate_argument(block, block->ptr[2]);
    sp = evaluate_argument(block, block->ptr[3]);
    ret = (level != NULL && delay != NULL && hp != NULL && sp != NULL) ? SCRIPT_PASSED : SCRIPT_FAILED;
    if(ret == SCRIPT_PASSED) {
        offset = sprintf(buf, "Cast Heal [Lv. %s] when pet owner's hp is below %s and sp is below %s for every %s seconds.",
            status_formula(&level->stack[level->stack_cnt], block->eng[0], level, 'n', 1),
            status_formula(&hp->stack[hp->stack_cnt], block->eng[2], hp, 'p', 1),
            status_formula(&sp->stack[sp->stack_cnt], block->eng[3], sp, 'p', 1),
            status_formula(&delay->stack[delay->stack_cnt], block->eng[1], delay, 'n', 1));
        translate_write(block, buf, 1);
    }
    if(level != NULL) node_free(level);
    if(delay != NULL) node_free(delay);
    if(hp != NULL) node_free(hp);
    if(sp != NULL) node_free(sp);
    return ret;
}

int translate_write(block_r * block, char * desc, int flag) {
    int length = 0;
    if(exit_null_safe(2, block, desc))
        return SCRIPT_FAILED;

    /* write the translation on the buffer and assign pointer to the start */
    block->eng[block->eng_cnt] = &block->arg[block->arg_cnt];
    /* select the proper format to write the argument */
    if(flag & 0x01) length = sprintf(&block->arg[block->arg_cnt],"%s",desc);
    block->arg[block->arg_cnt + length] = '\0';
    /* check that the write was successful */
    if(length <= 0) {
        exit_func_safe("failed to write translation (%s) on item %d", desc, block->item_id);
        return SCRIPT_FAILED;
    }
    /* extend the buffer to the next length */
    block->arg_cnt += length + 1;
    block->eng_cnt++;
    return SCRIPT_PASSED;
}

int translate_overwrite(block_r * block, char * desc, int position) {
    int length = 0;
    if(exit_null_safe(2, block, desc))
        return SCRIPT_FAILED;
    /* write the translation on the buffer and assign pointer to the start */
    block->eng[position] = &block->arg[block->arg_cnt];
    /* select the proper format to write the argument */
    length = sprintf(&block->arg[block->arg_cnt],"%s",desc);
    block->arg[block->arg_cnt + length] = '\0';
    /* check that the write was successful */
    if(length <= 0) {
        exit_func_safe("failed to write translation (%s) on item %d", desc, block->item_id);
        return SCRIPT_FAILED;
    }
    /* extend the buffer to the next length */
    block->arg_cnt += length + 1;
    return SCRIPT_PASSED;   
}

char * formula(char * buf, char * eng, node_t * result) {
    int len = 0;
    if(buf == NULL || eng == NULL || result == NULL)
        return eng;
    if(result != NULL && result->cond_cnt > 0) {
        
        len = (result->formula[0] == '(' && result->formula[strlen(result->formula) - 1] == ')') ?
            sprintf(buf, "%s %s", eng, result->formula):
            sprintf(buf, "%s (%s)", eng, result->formula);
        buf[len] = '\0';
        return buf;
    }
    return eng;
}

int formula_write(block_r * block, char * formula) {
    int len = 0;
    if(exit_null_safe(2, block, formula))
        return SCRIPT_FAILED;
    len = sprintf(&block->arg[block->arg_cnt],"%s", formula);
    if(len <= 0) {
        exit_func_safe("failed to write formula (%s) on item %d", formula, block->item_id);
        return SCRIPT_FAILED;
    }
    block->arg_cnt += len + 1;
    return SCRIPT_PASSED;
}

char * status_formula(char * aux, char * eng, node_t * result, int type, int blank) {
    int offset = 0;
    if(aux == NULL || eng == NULL || blank < 0)
        return "";
    if(result == NULL) 
        return eng;
    switch(type) {
        case 'n': check_node_range(result, aux); break;
        case 'p': check_node_range_precentage(result, aux); break;
        default: offset = sprintf(aux, "%s", eng); aux[offset] = '\0'; break;
    }
    return formula(aux, aux, result);
}

void argument_write(node_t * node, char * desc) {
    if(exit_null_safe(2, node, desc)) return;   
    int offset = node->stack_cnt;                               /* current offset on stack */
    node->aug_ptr[node->aug_ptr_cnt++] = &node->stack[offset];  /* record current offset on stack */
    offset += sprintf(node->stack + offset, "%s", desc);        /* write new string onto stack */
    node->stack[offset] = '\0';                                 /* null terminate new string */
    node->stack_cnt = offset + 1;                               /* set new offset on stack */
}

void id_write(node_t * node, char * fmt, ...) {
    if(exit_null_safe(2, node, fmt)) return;   
    va_list expr_list;
    va_start(expr_list, fmt);
    int offset = node->stack_cnt;                               /* current offset on stack */
    char * temp = &node->stack[offset];                         /* record current offset on stack */
    offset += vsprintf(node->stack + offset, fmt, expr_list);   /* write new string onto stack */
    node->stack[offset] = '\0';                                 /* null terminate new string */
    va_end(expr_list);
    node->stack_cnt = offset + 1;                               /* set new offset on stack */
    node->id = temp;
}

void expression_write(node_t * node, char * fmt, ...) {
    if(exit_null_safe(2, node, fmt)) return;   
    va_list expr_list;
    va_start(expr_list, fmt);
    int offset = node->stack_cnt;                               /* current offset on stack */
    char * temp = &node->stack[offset];                         /* record current offset on stack */
    offset += vsprintf(node->stack + offset, fmt, expr_list);   /* write new string onto stack */
    node->stack[offset] = '\0';                                 /* null terminate new string */
    va_end(expr_list);
    node->stack_cnt = offset + 1;                               /* set new offset on stack */
    node->formula = temp;
}

node_t * evaluate_argument(block_r * block, char * expr) {
    node_t * result = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(result == NULL) {
        exit_func_safe("failed to evaluate expression %s in item %d", expr, block->item_id);
        return NULL;
    }
    return result;
}

node_t * evaluate_expression(block_r * block, char * expr, int modifier, int flag) {
    int length = 0;
    node_t * root_node = NULL;
    token_r expr_token;
    logic_node_t * temp = NULL;

    /* check null paramaters */
    if(exit_null_safe(2, block, expr))
        return NULL;

    /* check division by zero */
    if(modifier == 0) {
        exit_func_safe("modifier is zero will cause division by zero in item %d", block->item_id);
        return NULL;
    }

    /* tokenize the expression */
    if(script_lexical(&expr_token, expr)) {
        exit_func_safe("failed to lex '%s' in item %d", expr, block->item_id);
        return NULL;
    }
    if(expr_token.script_cnt <= 0) {
        exit_func_safe("empty token list '%s' in item %d", expr, block->item_id);
        return NULL;
    }

    /* evaluate the expression */
    root_node = evaluate_expression_recursive(block, expr_token.script_ptr, 
                0, expr_token.script_cnt, block->logic_tree, flag);
    if(root_node == NULL) {
        exit_func_safe("failed to evaluate '%s' in item %d", expr, block->item_id);
        return NULL;
    }

    /* write the integer values on the stack */
    block->eng[block->eng_cnt] = &block->arg[block->arg_cnt];
    if(root_node->min == root_node->max) {
        /* single value with adjusting up to 2 decimal places */
        length = ((root_node->min / modifier) == 0 && root_node->min > 0) ?
            sprintf(&block->arg[block->arg_cnt],"%.2f",((double)(root_node->min) / modifier)):
            sprintf(&block->arg[block->arg_cnt],"%d",(root_node->min) / modifier);
    } else {
        /* multiple value with adjusting up to 2 decimal places */
        length = (((root_node->min / modifier) == 0 && root_node->min > 0) || ((root_node->max / modifier) == 0 && root_node->max > 0)) ?
            sprintf(&block->arg[block->arg_cnt],"%.2f ~ %.2f", ((double)(root_node->min) / modifier), ((double)(root_node->max) / modifier)):
            sprintf(&block->arg[block->arg_cnt],"%d ~ %d",(root_node->min) / modifier, (root_node->max) / modifier);
    }

    /* advance the translation stack */
    block->arg_cnt += length + 1;
    block->eng_cnt++;

    if(EVALUATE_FLAG_WRITE_FORMULA & flag && root_node->formula == NULL) {
        fprintf(stderr,"%s\n", expr);
    }

    /* write the modifier in the formula expression */
    if(EVALUATE_FLAG_WRITE_FORMULA & flag && modifier > 1 && root_node->cond_cnt > 0)
        expression_write(root_node, "%s / %d", root_node->formula, modifier);

    /* write the formula into the block and only write formula if dependencies exist */
    if(EVALUATE_FLAG_WRITE_FORMULA & flag && root_node->cond_cnt > 0)
        formula_write(block, root_node->formula);

    /* keep logic tree in node */
    if(EVALUATE_FLAG_KEEP_LOGIC_TREE & flag)
        if(root_node->cond != NULL) {
            if(block->logic_tree == NULL) {
                block->logic_tree = copy_deep_any_tree(root_node->cond);
            } else {
                /* new logic trees are stack onto of one another */
                temp = block->logic_tree;
                block->logic_tree = copy_deep_any_tree(root_node->cond);
                block->logic_tree->stack = temp;
            }
        }

    /* keep root node */
    if(!(EVALUATE_FLAG_KEEP_NODE & flag)) {
        node_free(root_node);
        root_node = NULL;
    }

    return root_node;
}

node_t * evaluate_expression_recursive(block_r * block, char ** expr, int start, int end, logic_node_t * logic_tree, int flag) {
    int i = 0;
    int j = 0;
    int len = 0;
    int ret = 0;
    int temp = 0;
    int expr_inx_open = 0;
    int expr_inx_close = 0;
    int expr_sub_level = 0;
    int op_cnt = 0;
    block_r * set_iter = NULL;
    ic_const_t const_info;
    ic_var_t var_info;

    /* operator precedence tree */
    static int op_pred[PRED_LEVEL_MAX][PRED_LEVEL_PER] = {
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

    /* linked list builder */
    node_t * root_node = NULL;
    node_t * iter_node = NULL;
    node_t * temp_node = NULL;
    
    /* initialize the root node */
    root_node = calloc(1, sizeof(node_t));
    iter_node = root_node;
    for(i = start; i < end; i++) {
        /* mark end of free list so that fail_expression does not overflow */
        iter_node->list = NULL;

        /* handle sub-expression by calling evaluate_expression_recursive recursively*/
        if(expr[i][0] == '(') {
            /* find the ending parenthesis */
            expr_inx_open = expr_inx_close = expr_sub_level = 0;
            for(expr_inx_open = i; i < end; i++) {
                if(expr[i][0] == '(') expr_sub_level++;
                if(expr[i][0] == ')') expr_sub_level--;
                if(!expr_sub_level && expr[i][0] == ')') {
                    expr_inx_close = i;
                    break;
                }
            }
            if(!expr_inx_close || expr_sub_level) {
                exit_func_safe("unmatch parenthesis in item %d", block->item_id);
                goto failed_expression;
            }

            /* ? operator require setting the EVALUATE_FLAG_EXPR_BOOL to prevent recursive 
             * evaluate expression to free the logic tree use by the 2nd and 3rd expression. */            
            temp_node = (i + 1 < end && expr[i+1][0] == '?')?
                evaluate_expression_recursive(block, expr, expr_inx_open + 1, expr_inx_close, logic_tree, flag | EVALUATE_FLAG_EXPR_BOOL):
                evaluate_expression_recursive(block, expr, expr_inx_open + 1, expr_inx_close, logic_tree, flag);
            if(temp_node == NULL) {
                exit_func_safe("fail to evaluate subexpression in item %d", block->item_id);
                goto failed_expression;
            }
            /* subexpression value is now a special operand */
            temp_node->type = NODE_TYPE_SUB;
            op_cnt++;
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
            /* check if entirely numeric */
            len = strlen(expr[i]);
            for(j = 0; j < len; j++)
                if(!isdigit(expr[i][j])) {
                    exit_func_safe("invalid numeric operand '%s' in item %d", expr[i], block->item_id);
                    goto failed_expression;
                }
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
            memset(&var_info, 0, sizeof(ic_var_t));
            if(!var_keyword_search(block->db, &var_info, expr[i])) {
                temp_node->cond_cnt = 1;
                id_write(temp_node, "%s", var_info.str);
                /* handle function call */
                if(var_info.type & TYPE_FUNC) {
                    /* find the ending parenthesis */
                    expr_inx_open = expr_inx_close = expr_sub_level = 0;
                    for(expr_inx_open = i; i < end; i++) {
                        if(expr[i][0] == '(') expr_sub_level++;
                        if(expr[i][0] == ')') expr_sub_level--;
                        if(!expr_sub_level && expr[i][0] == ')') {
                            expr_inx_close = i;
                            break;
                        }
                    }
                    if(!expr_inx_close || expr_sub_level) {
                        exit_func_safe("unmatch parenthesis in item %d", block->item_id);
                        goto failed_expression;
                    }

                    /* create function node */
                    temp_node->type = NODE_TYPE_FUNCTION;

                    /* static function value */
                    if(var_info.fflag & FUNC_DIRECT) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                    /* dynamic function value */
                    } else if(var_info.fflag & FUNC_PARAMS) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                        if(evaluate_function(block, expr, expr[expr_inx_open], 
                            expr_inx_open + 2, expr_inx_close, &(temp_node->min), 
                            &(temp_node->max), temp_node) == SCRIPT_FAILED) {
                            exit_func_safe("failed to evaluate function '%s' in item %d", expr, block->item_id);
                            goto failed_expression;
                        }
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
            /* handle constant */
            } else if(!const_keyword_search(block->db, &const_info, expr[i], block->mode)) {
                /* create constant node */
                temp_node->type = NODE_TYPE_CONSTANT;
                temp_node->min = temp_node->max = const_info.value;
            /* handle set'd variable */
            } else {
                /* create set block node */
                temp_node->type = NODE_TYPE_LOCAL; /* assume string literal unless resolved in for loop */
                temp_node->min = temp_node->max = 0;
                /* search for the set block in the link list */
                set_iter = block->set;
                while(set_iter != NULL) {
                    if(ncs_strcmp(set_iter->ptr[0], expr[i]) == 0) {
                        /* inherit from the set block */
                        temp_node->min = set_iter->set_node->min;
                        temp_node->max = set_iter->set_node->max;
                        if(set_iter->set_node->range != NULL)
                            temp_node->range = copyrange(set_iter->set_node->range);
                        if(set_iter->set_node->cond != NULL)
                            temp_node->cond = copy_deep_any_tree(set_iter->set_node->cond);
                        temp_node->cond_cnt = set_iter->set_node->cond_cnt;
                        expression_write(temp_node, "%s", set_iter->set_node->formula);
                        break;
                    }
                    set_iter = set_iter->set;
                }
            }
            op_cnt++;
        /* catch everything else */
        } else {
            exit_func_safe("invalid token '%s' in item %s", expr[i], block->item_id);
            goto failed_expression;
        }

        /* check operands are connected by opreator */
        if(op_cnt > 1) {
            exit_func_safe("operand '%s' without operator in item %d", expr[i], block->item_id);
            goto failed_expression;
        }

        if(temp_node != NULL) {
            /* generate range for everything but subexpression and operators */
            if(temp_node->type & 0x3e && temp_node->range == NULL)
                temp_node->range = mkrange(INIT_OPERATOR, temp_node->min, temp_node->max, DONT_CARE);

            /* add node to linked list */
            iter_node->next = temp_node;
            iter_node->list = temp_node;
            temp_node->prev = iter_node;
            iter_node = temp_node;
            iter_node->list = NULL;
            temp_node = NULL;
        } else {
            exit_func_safe("failed to generate node in item %s", block->item_id);
            goto failed_expression;
        }
    }

    /* complete the head doubly linked list */
    root_node->prev = iter_node;
    iter_node->next = root_node;
    iter_node->list = root_node;

    /* check that at least one node was added */
    if(root_node == iter_node) {
        exit_func_safe("detect zero node in expression in item %d", block->item_id);
        goto failed_expression;
    }

    /* structre the tree unary operators */
    iter_node = root_node;
    do {
        /* assume all unary operator have same precedences */
        if(iter_node->type == NODE_TYPE_UNARY) {
            /* unary operator (forward) expansion */
            iter_node->left = iter_node->next;
            iter_node->right = iter_node->next;
            iter_node->next->next->prev = iter_node;
            iter_node->next = iter_node->next->next;
        }
        iter_node = iter_node->next;
    } while(iter_node != root_node);

    /* structure the tree binary operators */
    for(i = 0; i < PRED_LEVEL_MAX; i++) {
        iter_node = root_node;
        do {
            if(iter_node->type == NODE_TYPE_OPERATOR) {
                for(j = 0; j < PRED_LEVEL_PER; j++) {
                    if(iter_node->op == op_pred[i][j] && op_pred[i][j] != '\0') {
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
    ret = evaluate_node(root_node->next, NULL, logic_tree, flag, &temp);

    /* the actual result is in root_node->next so we need 
     * to copy any value we want to return using the root node */
    if(ret == SCRIPT_PASSED) {
        /* simple function and variable parenthesis is meaningless, i.e. (getrefine()) */
        root_node->type = (root_node->next->type == NODE_TYPE_OPERATOR) ?
            NODE_TYPE_SUB : root_node->next->type;        
        root_node->min = root_node->next->min;
        root_node->max = root_node->next->max;
        root_node->range = copyrange(root_node->next->range);
        /* copy the logical tree if it exist */
        if(root_node->cond == NULL && root_node->next->cond != NULL) 
            root_node->cond = copy_any_tree(root_node->next->cond);
        root_node->cond_cnt = root_node->next->cond_cnt;
        expression_write(root_node, "%s", root_node->next->formula);
    } else {
        exit_func_safe("failed to evaluate node tree in item %d", block->item_id);
        goto failed_expression;
    }

    /* free all nodes except the head */
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

    /* script has failed; clean up everything */
    failed_expression:
        iter_node = root_node;
        while(iter_node != NULL) {
            temp_node = iter_node;
            iter_node = iter_node->list;
            freenamerange(temp_node->cond);
            freerange(temp_node->range);
            free(temp_node);
        }
        return NULL;
}

int evaluate_function(block_r * block, char ** expr, char * func, int start, int end, int * min, int * max, node_t * node) {
    int i = 0;
    int index = 0;

    /* operand variables */
    node_t * resultOne = NULL;
    node_t * resultTwo = NULL;

    /* token variable to handle special argument */
    token_r token;
    ic_skill_t skill;
    ic_item_t item;
    ic_const_t const_info;
    /*ea_item_group_t ea_item_group;*/
    option_t option;

    if(ncs_strcmp(func,"getskilllv") == 0) {
        /* search by skill id or skill constant for max level */
        memset(&skill, 0, sizeof(ic_skill_t));
        if(skill_name_search(block->db, &skill, expr[start], block->mode))
            if(isdigit(expr[start][0]) && skill_name_search_id(block->db, 
            &skill, convert_integer(expr[start], 10), block->mode)) {
                exit_func_safe("failed to search for skill %s in %d", expr[start], block->item_id);
                return SCRIPT_FAILED;
            }
        /* write the skill name */
        argument_write(node, skill.desc);
        expression_write(node, "%s Level", skill.desc);
        id_write(node, "%s Level", skill.desc);
        /* skill level minimum is 0 (not learnt) or maximum level */
        *min = 0;
        *max = skill.max;
    } else if(ncs_strcmp(func,"pow") == 0) {
        split_paramater(expr, start, end, &i);
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        if(resultOne == NULL || resultTwo == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d", expr, block->item_id);
            if(resultOne != NULL) node_free(resultOne);
            if(resultTwo != NULL) node_free(resultTwo);
            return SCRIPT_FAILED;
        } else {
            *min = (int) pow(resultOne->min, resultTwo->min);
            *max = (int) pow(resultOne->max, resultTwo->max);
            expression_write(node, "(%s)^%s", resultOne->formula, resultTwo->formula);
        }
    } else if(ncs_strcmp(func,"min") == 0) {
        split_paramater(expr, start, end, &i);
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        if(resultOne == NULL || resultTwo == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            if(resultOne != NULL) node_free(resultOne);
            if(resultTwo != NULL) node_free(resultTwo);
            return SCRIPT_FAILED;
        } else {
            *min = (int) (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
            *max = (int) (resultOne->max < resultTwo->max)?resultOne->max:resultTwo->max;
            expression_write(node, "Minimum(%s,%s)", resultOne->formula, resultTwo->formula);
        }
    } else if(ncs_strcmp(func,"max") == 0) {
        split_paramater(expr, start, end, &i);
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        if(resultOne == NULL || resultTwo == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d", expr, block->item_id);
            if(resultOne != NULL) node_free(resultOne);
            if(resultTwo != NULL) node_free(resultTwo);
            return SCRIPT_FAILED;
        } else {
            *min = (int) (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
            *max = (int) (resultOne->max < resultTwo->max)?resultOne->max:resultTwo->max;
            expression_write(node, "Maximum(%s,%s)", resultOne->formula, resultTwo->formula);
        }
    } else if(ncs_strcmp(func,"rand") == 0) {
        /* rand may have one or two argument */
        if(split_paramater(expr, start, end, &i)) {
            i = end; /* q.q obscure arcane */
            resultOne = evaluate_expression_recursive(block, expr, start, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        } else {
            resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        }
        if(resultOne == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        if(i + 1 < end) {
            resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
            if(resultTwo == NULL) {
                node_free(resultOne);
                exit_func_safe("failed to evaluate function '%s' in item id %d", expr, block->item_id);
                return SCRIPT_FAILED;
            } else {
                *min = (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
                *max = (resultOne->max > resultTwo->max)?resultOne->max:resultTwo->max;
                expression_write(node, "%s", "Random");
            }
        } else {
            *min = resultOne->min;
            *max = resultOne->max;
            expression_write(node, "%s", "Random");
        }
    /* callfunc calls global functions; just enumerate .. */
    } else if(ncs_strcmp(func,"callfunc") == 0) {
        block->flag = 0x01 | 0x02;
        /* the paramaters begin at a certain offset */
        if(ncs_strcmp(expr[start], "F_Rand") == 0) {
            block->offset = block->ptr_cnt;
        } else if(ncs_strcmp(expr[start], "F_RandMes") == 0) {
            block->offset = block->ptr_cnt + 1;
        } else {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }

        /* load the extended series onto the special buffer */
        for(i = 0; i < end + 1; i++) /* +1 include ) for parsing below */
            token.script_ptr[i] = expr[i];
        token.script_cnt = end + 1;

        /* parse the callfunc paramaters */
        if(script_parse(&token, &start, block, ',',')',4) == SCRIPT_FAILED) {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        block->ptr_cnt--;

        *min = *max = 0;
    } else if(ncs_strcmp(func,"strcharinfo") == 0) {
        /* note; hercules uses a set of constants named PC_*; might need to resolve to integer via const_db. */
        resultOne = evaluate_expression(block, expr[start], 1, EVALUATE_FLAG_KEEP_NODE);
        if(resultOne == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        if(resultOne->min != resultOne->max) {
            exit_func_safe("%s must be a constant expression in item %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        switch(resultOne->min) {
            case 0: argument_write(node, "Character"); break;
            case 1: argument_write(node, "Party"); break;
            case 2: argument_write(node, "Guild"); break;
            case 3: argument_write(node, "Map"); break;
            default: 
                exit_func_safe("invalid '%s' argument in item %d\n", expr, block->item_id);
                return SCRIPT_FAILED;
        }
    } else if(ncs_strcmp(func,"countitem") == 0) {
        resultOne = evaluate_expression(block, expr[start], 1, EVALUATE_FLAG_KEEP_NODE);
        if(resultOne == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        if(resultOne->min != resultOne->max) {
            exit_func_safe("%s must be a constant expression in item %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        memset(&item, 0, sizeof(ic_item_t));
        if(item_name_id_search(block->db, &item, resultOne->min, block->mode)) {
            exit_func_safe("failed to search for item %d in %d", resultOne->min, block->item_id);
            return SCRIPT_FAILED;
        }
        /* write the item name on the argument stack */
        argument_write(node, item.name);
        *min = 0;
        *max = 2147483647;
    } else if(ncs_strcmp(func,"gettime") == 0) {
        resultOne = evaluate_expression(block, expr[start], 1, EVALUATE_FLAG_KEEP_NODE);
        if(resultOne == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        if(resultOne->min != resultOne->max) {
            exit_func_safe("%s must be a constant expression in item %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        switch(resultOne->min) {
            case 1: argument_write(node, "Seconds"); break;
            case 2: argument_write(node, "Minutes"); break;
            case 3: argument_write(node, "Hours"); break;
            case 4: argument_write(node, "Weeks"); break;
            case 5: argument_write(node, "Day"); break;
            case 6: argument_write(node, "Month"); break;
            case 7: argument_write(node, "Year"); break;
            case 8: argument_write(node, "Day of Year"); break;
            default: 
                exit_func_safe("invalid '%s' argument in item %d\n", expr, block->item_id);
                return SCRIPT_FAILED;
        }
        id_write(node, "%s", node->aug_ptr[node->aug_ptr_cnt - 1]);
    } else if(ncs_strcmp(func,"getiteminfo") == 0) {
        for(i = start; i < end; i++)
            if(expr[i][0] == ',')
                index = i;
        resultOne = evaluate_expression_recursive(block, expr, start, index, block->logic_tree, 0);
        resultTwo = evaluate_expression_recursive(block, expr, index + 1, index + 2, block->logic_tree, 0);
        if(resultOne == NULL || resultTwo == NULL) {
            if(resultOne != NULL) node_free(resultOne);
            if(resultTwo != NULL) node_free(resultTwo);
            return SCRIPT_FAILED;
        }
        /*argument_write(node, resultOne->cond->args);*/
        switch(resultTwo->min) {
            case 0: argument_write(node, "Buy Price"); break;
            case 1: argument_write(node, "Sell Price"); break;
            case 2: argument_write(node, "Item Type"); break;
            case 3: argument_write(node, "Max Drop Chance"); break;
            case 4: argument_write(node, "Sex"); break;
            case 5: argument_write(node, "Equip"); break;
            case 6: argument_write(node, "Weight"); break;
            case 7: argument_write(node, "Attack"); break;
            case 8: argument_write(node, "Defense"); break;
            case 9: argument_write(node, "Range"); break;
            case 10: argument_write(node, "Slot"); break;
            case 11: argument_write(node, "Weapon Type"); break;
            case 12: argument_write(node, "Requirement Level"); break;
            case 13: argument_write(node, "Weapon Level"); break;
            case 14: argument_write(node, "View ID"); break;
            case 15: argument_write(node, "Magical Attack"); break;
            default: 
                exit_func_safe("invalid '%s' argument in item %d\n", expr, block->item_id);
                return SCRIPT_FAILED;
        }
        id_write(node, "%s", node->aug_ptr[node->aug_ptr_cnt - 1]);
    } else if(ncs_strcmp(func,"getequipid") == 0) {
        if(const_keyword_search(block->db, &const_info, expr[start], block->mode)) {
            exit_func_safe("failed to search for const '%s' in %d", expr[start], block->item_id);
            return SCRIPT_FAILED;
        }
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
        id_write(node, "%s", node->aug_ptr[node->aug_ptr_cnt - 1]);
    } else if(ncs_strcmp(func,"isequipped") == 0) {
        for(i = start; i < end; i++) {
            if(expr[i][0] != ',') {
                memset(&item, 0, sizeof(ic_item_t));
                if(item_name_id_search(block->db, &item, convert_integer(expr[i], 10), block->mode)) {
                    exit_func_safe("failed to search for item %s in %d", expr[i], block->item_id);
                    return SCRIPT_FAILED;
                }
                argument_write(node, item.name);
            }
        }
    } else if(ncs_strcmp(func,"groupranditem") == 0) {
        /*memset(&ea_item_group, 0, sizeof(ea_item_group_t));
        if(const_keyword_search(block->db, &const_info, expr[start], block->mode))
            exit_abt(build_buffer(global_err, "failed to search for const %s in %d", expr[start], block->item_id));
        ea_item_group_search(block->db, &ea_item_group, const_info.value, block->mode, buffer);
        block->flag |= 0x04;*/
        translate_write(block, expr[start], 1);
    } else if(ncs_strcmp(func,"getequiprefinerycnt") == 0) {
        resultOne = evaluate_expression_recursive(block, expr, start, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        if(resultOne == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        switch(resultOne->min) {
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
            default: argument_write(node, "Error"); break;
        }
        id_write(node, "%s's %s", node->aug_ptr[node->aug_ptr_cnt - 1], node->id);
    } else if(ncs_strcmp(func,"readparam") == 0) {
        if(const_keyword_search(block->db, &const_info, expr[start], block->mode)) {
            exit_func_safe("failed to search for const '%s' in %d", expr[start], block->item_id);
            return SCRIPT_FAILED;
        }
        /* values correspond to the const.txt for rAthena */
        switch(const_info.value) {
            case 6: argument_write(node, "Maximum HP"); break;
            case 8: argument_write(node, "Maximum SP"); break;
            case 13: argument_write(node, "Strength"); break;
            case 14: argument_write(node, "Agility"); break;
            case 15: argument_write(node, "Vitality"); break;
            case 16: argument_write(node, "Intelligence"); break;
            case 17: argument_write(node, "Dexterity"); break;
            case 18: argument_write(node, "Luck"); break;
            case 41: argument_write(node, "Base Attack"); break;
            case 42: argument_write(node, "Weapon Attack"); break;
            case 45: argument_write(node, "Base Defense"); break;
            case 46: argument_write(node, "Armor Defense"); break;
            case 47: argument_write(node, "Base Magical Defense"); break;
            case 48: argument_write(node, "Armor Magical Defense"); break;
            case 49: argument_write(node, "Hit"); break;
            case 50: argument_write(node, "Base Flee"); break;
            case 51: argument_write(node, "Armor Flee"); break;
            case 52: argument_write(node, "Critical Rate"); break;
            case 53: argument_write(node, "Attack Speed"); break;
            case 59: argument_write(node, "Fame"); break;
            case 60: argument_write(node, "Unbreakable"); break;
            default: 
                exit_func_safe("invalid constant value for %s -> %d in item "
                    "%d\n", const_info.name, const_info.value, block->item_id);
                return SCRIPT_FAILED;
        }
        id_write(node, "%s", node->aug_ptr[node->aug_ptr_cnt - 1]);
    } else if(ncs_strcmp(func,"checkoption") == 0) {
        /* constants are not defined in const.txt, but specified 
         * in the script command. See option_db.txt for mappings.*/
        memset(&option, 0, sizeof(option_t));
        if(ra_option_search_str(block->db, &option, expr[start])) {
            exit_func_safe("failed to search for option '%s' in %d", expr[start], block->item_id);
            return SCRIPT_FAILED;
        }
        argument_write(node, option.name);
        id_write(node,"%s", option.name);
    } else {
        exit_func_safe("failed to search function handler for '%s' in item %d", func, block->item_id);
        return SCRIPT_FAILED;
    }

    if(resultOne != NULL) node_free(resultOne);
    if(resultTwo != NULL) node_free(resultTwo);
    return SCRIPT_PASSED;
}

int evaluate_node(node_t * node, FILE * stm, logic_node_t * logic_tree, int flag, int * complexity) {
    range_t * temp = NULL;
    range_t * result = NULL;
    logic_node_t * new_tree = NULL;

    /* support unary and binary operators */
    if(node->left == node->right) {
        /* unary operator */
        if(node->left != NULL) 
            if(evaluate_node(node->left, stm, logic_tree, flag, complexity))
                return SCRIPT_FAILED;
    } else {
        /* support ?: flip the logic tree for one side */
        if(node->type == NODE_TYPE_OPERATOR && node->op == ':') {
            if(logic_tree != NULL) {
                new_tree = inverse_logic_tree(logic_tree);
                /* use the 'true' logic tree for left; 'false' logic tree for right */
                if(node->left != NULL) 
                    if(evaluate_node(node->left, stm, logic_tree, flag, complexity))
                        return SCRIPT_FAILED;
                if(node->right != NULL) 
                    if(evaluate_node(node->right, stm, new_tree, flag, complexity))
                        return SCRIPT_FAILED;
                freenamerange(new_tree);
            } else{
                if(evaluate_node(node->left, stm, logic_tree, flag, complexity) ||
                   evaluate_node(node->right, stm, logic_tree, flag, complexity))
                    return SCRIPT_FAILED;
            }
        /* normal pre-order traversal */
        } else {
            if(node->left != NULL) 
                if(evaluate_node(node->left, stm, logic_tree, flag, complexity))
                    return SCRIPT_FAILED;
            /* support ?: add logic tree on stack */
            if(node->type == NODE_TYPE_OPERATOR && node->op == '?') {
                /* check that logic tree exist for left side */
                if(node->left->cond != NULL) {
                    new_tree = node->left->cond;
                    new_tree->stack = logic_tree;
                    /* keep the logic tree of ? conditional */
                    if(EVALUATE_FLAG_KEEP_TEMP_TREE & flag)
                        node->cond = copy_any_tree(new_tree);
                    if(node->right != NULL) 
                        if(evaluate_node(node->right, stm, new_tree, flag, complexity))
                            return SCRIPT_FAILED;
                } else {
                    if(node->right != NULL) 
                        if(evaluate_node(node->right, stm, logic_tree, flag, complexity))
                            return SCRIPT_FAILED;
                }
            } else {
                if(node->right != NULL) 
                    if(evaluate_node(node->right, stm, logic_tree, flag, complexity))
                        return SCRIPT_FAILED;
            }
        }
    }

    /* handle variable and function nodes; generate condition node */
    if(node->type & (NODE_TYPE_FUNCTION | NODE_TYPE_VARIABLE)) {
        if(node->id == NULL) {
            exit_abt_safe("invalid node id");
            return SCRIPT_FAILED;
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
    }

    /* handle unary operator nodes */
    if(node->type & NODE_TYPE_UNARY) {
        if(node->left == NULL) {
            exit_abt_safe("invalid unary operator operand");
            return SCRIPT_FAILED;
        }

        if(node->left->range == NULL) {
            exit_abt_safe("invalid unary operator operand without range");
            return SCRIPT_FAILED;
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

        /* running count of all the variable, function, and set block */
        node->cond_cnt += node->left->cond_cnt;
    }

    /* handle operators nodes */
    if(node->type & NODE_TYPE_OPERATOR) {
        /* operators must have two non-null operands */
        if(node->left == NULL || node->right == NULL) {
            fprintf(stderr,"evaluate_node; left or right node is null.\n");
            return SCRIPT_FAILED;
        }

        /* sub node inherit range, operator calculate range, operands have nodes created */
        if(node->left->range == NULL || node->right->range == NULL) {
            fprintf(stderr,"evaluate_node; left or right operand has null range.\n");
            return SCRIPT_FAILED;
        }

        /* calculate range for operator node */
        switch(node->op) {
            case '*': 
            case '/': 
            case '+': 
            case '-': 
                node->range = calcrange(node->op, node->left->range, node->right->range); 
                node_inherit_cond(node); 
                break;
            case '<' + '=': 
            case '<': 
            case '>' + '=': 
            case '>': 
            case '!' + '=': 
            case '=' + '=': 
                /* typically, scripts use boolean expressions like 100 + 600 * (getrefine() > 5), which can be interpreted
                 * wrong for non-boolean expression, which should result in either 0 or 1 rather than range. */
                node->range = (flag & EVALUATE_FLAG_EXPR_BOOL)?
                    calcrange(node->op, node->left->range, node->right->range):
                    mkrange(INIT_OPERATOR, 0, 1, 0);
                node_inherit_cond(node);
                (*complexity)++;
                break;
            case '&': 
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
                return SCRIPT_FAILED;
        }

        /* extract the local min and max from the range */
        node->min = minrange(node->range);
        node->max = maxrange(node->range);

        /* running count of all the variable, function, and set block */
        node->cond_cnt = node->left->cond_cnt + node->right->cond_cnt;
    }

    /* write expression and print node */
    if(EVALUATE_FLAG_WRITE_FORMULA & flag)
        (node->type == NODE_TYPE_OPERATOR)?
            node_expr_append(node->left, node->right, node):
            node_expr_append(NULL, NULL, node);

    /* useful for debugging the nodes */
    node_dmp(node, stm);
    return SCRIPT_PASSED;
}

/* function must be called AFTER node->range is calc */
void node_inherit_cond(node_t * node) {
    if(exit_null_safe(1, node)) return;
    /* only inherit if only ONE condition exist; if condition 
     * exist on either end, then the condition can't be interpreted. */
    if(node->left->cond != NULL && node->right->cond == NULL) {
        node->cond = make_cond(node->left->cond->name, node->range, node->left->cond);
    } else if(node->left->cond == NULL && node->right->cond != NULL) {
        node->cond = make_cond(node->right->cond->name, node->range, node->right->cond);
    }
}

void node_expr_append(node_t * left, node_t * right, node_t * dest) {
    if(exit_null_safe(1, dest)) return;

    /* constructs the formula by resolving variables to names */
    if(left == NULL && right == NULL) {
        /* dest is a leave node */
        switch(dest->type) {
            case NODE_TYPE_UNARY: expression_write(dest, "%c%s", dest->op, dest->left->formula); break;
            case NODE_TYPE_OPERAND: expression_write(dest, "%d", dest->max); break;
            case NODE_TYPE_FUNCTION: expression_write(dest, "%s", dest->id); break;
            case NODE_TYPE_VARIABLE: expression_write(dest, "%s", dest->id); break;
            case NODE_TYPE_LOCAL: /* already inherited when resolving set block */ break;
            case NODE_TYPE_CONSTANT: expression_write(dest, "%s", dest->id); break;
            case NODE_TYPE_SUB: expression_write(dest, "%s", dest->formula); break;
            default: exit_func_safe("invalid node type %d", dest->type); break;
        }
    } else if(left != NULL && right != NULL) {
        /* dest is a operand node */
        switch(dest->type) {
            case NODE_TYPE_OPERATOR:
                switch(dest->op) {
                    case '?': expression_write(dest,"%s %c %s", left->formula, dest->op, right->formula); break;
                    case '>': expression_write(dest,"%s > %s", left->formula, right->formula); break;
                    case '<': expression_write(dest,"%s < %s", left->formula, right->formula); break;
                    case '!' + '=': expression_write(dest,"%s != %s", left->formula, right->formula); break;
                    case '=' + '=': expression_write(dest,"%s == %s", left->formula, right->formula); break;
                    case '<' + '=': expression_write(dest,"%s <= %s", left->formula, right->formula); break;
                    case '>' + '=': expression_write(dest,"%s >= %s", left->formula, right->formula); break;
                    case '&' + '&': expression_write(dest,"%s and %s", left->formula, right->formula); break;
                    case '|' + '|': expression_write(dest,"%s or %s", left->formula, right->formula); break;
                    default: expression_write(dest,"%s %c %s", left->formula, dest->op, right->formula); break;
                }
                break;
            default: exit_func_safe("invalid node type %d", dest->type); break;
        }
    }
}

void node_dmp(node_t * node, FILE * stm) {
    if(node != NULL && stm != NULL) {
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
            case NODE_TYPE_SUB: fprintf(stm,"     Type: Subexpression %s; %d:%d\n", node->formula, node->min, node->max); break;
            default: fprintf(stm,"     Type: %d\n", node->op); break;
        }
        fprintf(stm,"Stack_Cnt: %d\n", node->stack_cnt);
        fprintf(stm,"  Arg_Ptr: %d\n", node->aug_ptr_cnt);
        fprintf(stm," Cond Cnt: %d\n", node->cond_cnt);
        fprintf(stm,"     Expr: %s\n", node->formula);
        dmprange(node->range, stm, "range; ");
        dmpnamerange(node->cond, stdout, 0);
    }
}

void node_free(node_t * node) {
    if(node != NULL) {
        if(node->cond != NULL) freenamerange(node->cond);
        if(node->range != NULL) freerange(node->range);
        free(node);
    }
}

int translate_bonus_desc(node_t ** result, block_r * block, ic_bonus_t * bonus) {
    int i = 0;
    int arg_cnt = bonus->type_cnt;
    int offset = 0;                 /* current buffer offset */
    char buffer[BUF_SIZE];          /* buffer for handling modifier and qualifiers */
    int order[BONUS_SIZE];
    char aux[BONUS_SIZE][BUF_SIZE]; /* FORMULA Q.Q */

    /* item bonus integrity check */
    if(bonus->type_cnt != bonus->order_cnt || bonus->type_cnt > BONUS_SIZE) {
        exit_func_safe("argument and order count mismatch in item %d\n", block->item_id);
        return SCRIPT_FAILED;
    }

    /* order contains stack pointer constants that indicate 
     * the string on the stack to write to template */
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
            translate_bonus_float_percentage2(block, result[1], &order[0], "Add %s", "Add %s", "Reduce %s", 10); 
            translate_bonus_percentage(block, result[2], &order[1]); 
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
        case 30: translate_bonus_float_percentage2(block, result[2], &order[0], "%s", "%s", "%s", 100); break; /* inefficient */
        default: break;
    }

    /* write the final translation using the template */
    switch(arg_cnt) {
        case 1: 
            translate_bonus_template(buffer, &offset, bonus->format, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]));
            break;
        case 2: 
            translate_bonus_template(buffer, &offset, bonus->format, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]])); 
            break;
        case 3: 
            translate_bonus_template(buffer, &offset, bonus->format, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]]), 
                formula(aux[2], block->eng[order[2]], result[bonus->order[2]])); 
            break;
        case 4: 
            translate_bonus_template(buffer, &offset, bonus->format, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]]), 
                formula(aux[2], block->eng[order[2]], result[bonus->order[2]]), 
                formula(aux[3], block->eng[order[3]], result[bonus->order[3]])); 
            break;
        case 5: 
            translate_bonus_template(buffer, &offset, bonus->format, 
                formula(aux[0], block->eng[order[0]], result[bonus->order[0]]), 
                formula(aux[1], block->eng[order[1]], result[bonus->order[1]]), 
                formula(aux[2], block->eng[order[2]], result[bonus->order[2]]), 
                formula(aux[3], block->eng[order[3]], result[bonus->order[3]]), 
                formula(aux[4], block->eng[order[4]], result[bonus->order[4]])); 
            break;
    }
    translate_write(block, buffer, 1);
    return SCRIPT_PASSED;
}

char * translate_bonus_template(char * buffer, int * offset, char * template, ...) {
    va_list bonus_args;
    va_start(bonus_args, template);
    (*offset) += vsprintf(buffer, template, bonus_args);
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
        case -1: return;
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
        case -1: return;
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
        case -1: return;
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
    if(exit_null_safe(1, node))
        return SCRIPT_FAILED;
    if(node->min <= 0 && node->max <= 0) affinity = 2;
    else if(node->min >= 0 && node->max >= 0) affinity = 1;
    else if(node->min < 0 && node->max > 0) affinity = 0;
    return affinity;
}

int script_linkage_count(block_r * block, int start) {
    int nest = 0;
    block_r * iter = block->link;
    /* calculate the nesting */
    while(iter != NULL) {
        if(iter->type != 27) nest++;
        iter = iter->link;
    }
    return nest;
}

/* primary multiplexer for translating logic tree */
int script_generate_cond(logic_node_t * tree, FILE * stm, char * prefix, char * buffer, int * offset) {
    char buf[BUF_SIZE];
    int off = 0;
    memset(buf, 0, BUF_SIZE);

    switch(tree->type) {
        /* each OR node means that multiple conditions */
        case LOGIC_NODE_OR: 
            if(tree->left != NULL) 
                if(script_generate_cond(tree->left, stm, prefix, buffer, offset) == SCRIPT_FAILED)
                    return SCRIPT_FAILED;
            if(tree->right != NULL) 
                if(script_generate_cond(tree->right, stm, prefix, buffer, offset) == SCRIPT_FAILED)
                    return SCRIPT_FAILED;
            break;
        /* write each OR's children (non-OR node) as separate condition */
        case LOGIC_NODE_AND: 
            if(script_generate_and_chain(tree, buf, &off) == SCRIPT_FAILED)
                return SCRIPT_FAILED;
            *offset += sprintf(buffer + *offset, "%s[%s]\n", prefix, buf);
            break;
        case LOGIC_NODE_COND: 
            if(script_generate_cond_node(tree, buf, &off) == SCRIPT_FAILED)
                return SCRIPT_FAILED;
            *offset += sprintf(buffer + *offset, "%s[%s]\n", prefix, buf);
            break;
        default: break;
    }
    return SCRIPT_PASSED;
}

/* recursively chain all cond nodes into one buffer */
int script_generate_and_chain(logic_node_t * tree, char * buf, int * offset) {
    int ret_value = 0;
    if(tree->left != NULL)
        switch(tree->left->type) {
            case LOGIC_NODE_AND: ret_value = script_generate_and_chain(tree->left, buf, offset); break;
            case LOGIC_NODE_COND: ret_value = script_generate_cond_node(tree->left, buf, offset); break;
            default: break;
        }
    if(ret_value == SCRIPT_FAILED) return SCRIPT_FAILED;

    if(tree->right != NULL)
        switch(tree->right->type) {
            case LOGIC_NODE_AND: ret_value = script_generate_and_chain(tree->right, buf, offset); break;
            case LOGIC_NODE_COND: ret_value = script_generate_cond_node(tree->right, buf, offset); break;
            default: break;
        }
    return ret_value;
}

int script_generate_cond_node(logic_node_t * tree, char * buf, int * offset) {
    return SCRIPT_PASSED;
}

int script_generate_class_generic(char * buf, int * offset, range_t * range, char * template) {
    int i = 0;
    range_t * itrrange = NULL;
    range_t * negrange = NULL;

    if(exit_null_safe(4, buf, offset, range, template))
        return SCRIPT_FAILED;

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
    return SCRIPT_PASSED;
}

int script_generate_cond_generic(char * buf, int * offset, int val_min, int val_max, char * template) {
    if(exit_null_safe(3, buf, offset, template))
        return SCRIPT_FAILED;
    *offset += (val_min != val_max) ?
        sprintf(buf + *offset, "%s %d ~ %d", template, val_min, val_max) :
        sprintf(buf + *offset, "%s %d", template, val_min);
    buf[*offset] = '\0';
    return SCRIPT_PASSED;
}

int minimize_stack(node_t * left, node_t * right) {
    return SCRIPT_PASSED;
}
/*
 *   file: script.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "script.h"

FILE * node_dbg = NULL;

int block_init(block_r ** block) {
    block_r * _block = NULL;

    exit_null_safe(1, block);

    _block = calloc(1, sizeof(block_r));
    if(NULL == _block)
        return CHECK_FAILED;

    _block->next = _block;
    _block->prev = _block;
    _block->free = 1;

    *block = _block;
    return CHECK_PASSED;
}

int block_append(block_r * parent, block_r * child) {
    exit_null_safe(2, parent, child);

    parent->next->prev = child->prev;
    child->prev->next = parent->next;
    parent->next = child;
    child->prev = parent;

    return CHECK_PASSED;
}

int block_remove(block_r * block) {
    exit_null_safe(1, block);

    block->prev->next = block->next;
    block->next->prev = block->prev;
    block->next = block;
    block->prev = block;

    return CHECK_PASSED;
}

int block_reset(block_r * block) {
    exit_null_safe(1, block);

    /* reset block and item id */
    block->item_id = 0;
    block->name = NULL;
    block->type = 0;

    /* reset block stack */
    block->arg_cnt = 0;
    block->ptr_cnt = 0;
    block->eng_cnt = 0;

    /* reset block linking */
    block->link = NULL;
    block->set = NULL;

    /* free set block's node */
    if(NULL != block->set_node) {
        node_free(block->set_node);
        block->set_node = NULL;
    }

    /* logic trees are generated for if, else, and for block */
    if(NULL != block->logic_tree) {
        deepfreenamerange(block->logic_tree);
        block->logic_tree = NULL;
    }

    block->free = 1;
    return CHECK_PASSED;
}

int block_deit(block_r ** block) {
    block_r * _block = NULL;

    exit_null_safe(2, block, *block);

    _block = *block;
    free(_block);
    *block = NULL;

    return CHECK_PASSED;
}

int block_stack_push(block_r * block, int type, const char * str) {
    int ret = 0;
    int cnt = 0;
    int off = 0;
    int len = 0;
    char * buf = NULL;

    /* check for infidels */
    exit_null_safe(2, block, str);

    /* get buffer state */
    off = block->arg_cnt;
    buf = &block->arg[off];

    /* check buffer size */
    len = strlen(str) + 1;
    if ((off + len) >= BUF_SIZE)
        return exit_func_safe("buffer overflow in item %d", block->item_id);

    /* get translated or argument string pointer */
    switch (type) {
        case TYPE_PTR:
            cnt = block->ptr_cnt;
            if (cnt >= PTR_SIZE)
                return exit_func_safe("exceed translated string array"
                " size %d in item %d", PTR_SIZE, block->item_id);
            block->ptr[cnt] = buf;
            block->ptr_cnt++;
            break;
        case TYPE_ENG:
            cnt = block->eng_cnt;
            if (cnt >= PTR_SIZE)
                return exit_func_safe("exceed argument string array"
                " size %d in item %d", PTR_SIZE, block->item_id);
            block->eng[cnt] = buf;
            block->eng_cnt++;
            break;
        default:
            return exit_func_safe("invalid type %d in item %d", type, block->item_id);
    }

    /* update buffer state */
    ret = sprintf(buf, "%s", str);
    if (ret + 1 != len)
        return exit_func_safe("failed to write buffer in item %d", block->item_id);

    /* length of string + null character + 1 to pointer pass the null character */
    block->arg_cnt += len;
    return SCRIPT_PASSED;
}

int block_stack_pop(block_r * block, int type) {
    int cnt = 0;
    int len = 0;
    char * buf = NULL;
    switch (type) {
        case TYPE_PTR:
            cnt = --block->ptr_cnt;
            if (cnt < 0)
                return exit_func_safe("empty buffer in item %d", block->item_id);
            buf = block->ptr[cnt];
            block->ptr[cnt] = NULL;
            break;
        case TYPE_ENG:
            cnt = --block->eng_cnt;
            if (cnt < 0)
                return exit_func_safe("empty buffer in item %d", block->item_id);
            buf = block->eng[cnt];
            block->eng[cnt] = NULL;
            break;
        default:
            return exit_func_safe("invalid type %d in item %d", type, block->item_id);
    }

    /* update buffer state */
    len = strlen(buf) + 1;
    block->arg_cnt -= len;

    /* removing the last string at block->eng[0] or block->ptr[0]
     * causes the arg_cnt to be -1, which corrupts the block->type. */
    if (block->arg_cnt < 0)
        block->arg_cnt = 0;
    return SCRIPT_PASSED;
}

int block_stack_reset(block_r * block, int type) {
    int i = 0;
    int cnt = 0;

    /* error on invalid references */
    exit_null_safe(1, block);

    /* get the number of elements on the stack */
    switch(type) {
        case TYPE_PTR: cnt = block->ptr_cnt; break;
        case TYPE_ENG: cnt = block->eng_cnt; break;
        default:
            return exit_func_safe("invalid stack type %d in item %d", type, block->item_id);
    }

    /* pop each element from the stack */
    for(i = 0; i < cnt; i++)
        block_stack_pop(block, type);

    return CHECK_PASSED;
}

int block_stack_formula(block_r * block, int index, node_t * node, char ** out) {
    int ret = 0;
    char * buffer = NULL;
    char * prefix = NULL;
    char * formula = NULL;
    int buffer_size = 0;
    int prefix_len = 0;
    int formula_len = 0;

    exit_null_safe(2, block, node);

    /* check output pointer */
    if (out == NULL || *out != NULL)
        return exit_func_safe("unsafe output pointer %p in item %d", (void *)out, block->item_id);

    /* check index */
    if (index >= block->eng_cnt)
        return exit_func_safe("invalid index in translate array for item %d", block->item_id);

    /* check null */
    prefix = block->eng[index];
    if (prefix == NULL)
        return exit_func_safe("invalid translated string at index %d for item %d", index, block->item_id);

    /* check empty string */
    prefix_len = strlen(prefix);
    if (prefix_len <= 0)
        return exit_func_safe("empty translated string at index %d for item %d", index, block->item_id);

    formula = node->formula;
    formula_len = (node->formula != NULL) ? strlen(formula) : 0;
    if (formula_len <= 0 || node->cond_cnt <= 0) {
        /* no formula */
        buffer = convert_string(prefix);
        /* formula must contain at least one function or variable */
    } else if (node->cond_cnt > 0) {
        /* include formula */
        buffer_size = prefix_len + formula_len + 5; /* pad with 10 bytes for formatting with syntax and '\0' */
        buffer = calloc(buffer_size, sizeof(char));
        if (buffer == NULL)
            return exit_func_safe("out of memory in item %d", block->item_id);

        /* check whether formula is wrap in parenthesis */
        ret = (formula[0] == '(' && formula[formula_len - 1] == ')') ?
            sprintf(buffer, "%s %s", prefix, formula) :
            sprintf(buffer, "%s (%s)", prefix, formula);
    }

    *out = buffer;
    return SCRIPT_PASSED;
}

int block_stack_dump(block_r * block, FILE * stream) {
    int i = 0;
    block_r * iter = NULL;

    iter = block;
    do {
        /* dump the used block only */
        if(iter->free == 0) {
            fprintf(stream,
                "     item id: %d\n"
                "  block addr: %p\n"
                "  block name: %s [%d]\n"
                "  block link: %p\n"
                "   block set: %p\n"
                "   arg stack: %d\n"
                "   eng stack: %d\n",
                iter->item_id,
                iter,
                iter->name,
                iter->type,
                iter->link,
                iter->set,
                iter->ptr_cnt,
                iter->eng_cnt);
            /* dump the stack */
            for(i = 0; i < iter->ptr_cnt; i++)
                fprintf(stream, "  arg[%5d]: %s\n", i, iter->ptr[i]);
            for(i = 0; i < iter->eng_cnt; i++)
                fprintf(stream, "  eng[%5d]: %s\n", i, iter->eng[i]);
            fprintf(stream, "         buf: ");
            for(i = 0; i < iter->arg_cnt; i++)
                (iter->arg[i] == '\0') ?
                    putc(iter->arg[i], stream):
                    putc(iter->arg[i], stream);
            putc('\n', stream);

            /* dump the logic tree */
            if(iter->logic_tree != NULL) {
                fprintf(stream," logic tree: ");
                dmpnamerange(iter->logic_tree, stream, 0);
            }
            fprintf(stream, "------------;\n");
        }
        iter = iter->next;
    } while(iter != block);

    return CHECK_PASSED;
}

int script_block_new(script_t * script, block_r ** block) {
    block_r * _block = NULL;

    exit_null_safe(2, script, block);

    if(script->blocks->prev->free) {
        /* tail of the list is a free block */
        _block = script->blocks->prev;
    } else {
        /* create a new block and append to block list */
        if(block_init(&_block) ||
           block_append(_block, script->blocks))
            return CHECK_FAILED;
    }

    /* the root block is either set to a new block or
     * a free block from the tail of the block list */
    script->blocks = _block;
    script->blocks->free = 0;

    *block = script->blocks;
    return CHECK_PASSED;
}

int script_block_free(script_t * script, block_r ** block) {
    block_r * _block = NULL;

    exit_null_safe(3, script, block, *block);

    _block = *block;

    /* special case when freeing the root block */
    if(script->blocks == _block) {
        /* free blocks are on the tail and used
         * blocks are on the head, we will move
         * the root block to a used block */
        script->blocks = script->blocks->next;
    }

    /* reset and remove the block
     * then append to the tail of
     * the block list */
    if( block_reset(_block) ||
        block_remove(_block) ||
        block_append(_block, script->blocks))
        return CHECK_FAILED;

    *block = NULL;
    return CHECK_PASSED;
}

int script_block_free_all(script_t * script) {
    block_r * iter = NULL;

    exit_null_safe(1, script);

    /* iterate from the root block until
     * the root block or the free blocks */
    iter = script->blocks;
    do {
        if(block_reset(iter))
            return CHECK_FAILED;
        iter = iter->next;
    } while(iter != script->blocks && !iter->free);

    return CHECK_PASSED;
}

int script_block_release(script_t * script) {
    block_r * iter = NULL;
    block_r * temp = NULL;

    exit_null_safe(1, script);

    if(script_block_free_all(script))
        return CHECK_FAILED;

    iter = script->blocks->next;
    while(iter != script->blocks) {
        temp = iter;
        iter = iter->next;
        /* remove the block from the
         * list and free the memory */
        if(block_remove(temp) ||
           block_deit(&temp))
            return CHECK_FAILED;
    }

    /* free the root block */
    return block_deit(&script->blocks);
}

int script_map_init(script_t * script, const char * map_path) {
    exit_null_safe(2, script, map_path);

    script->map = luaL_newstate();
    if(NULL == script->map ||
       luaL_loadfile(script->map, map_path) ||
       lua_pcall(script->map, 0, 0, 0))
        return exit_func_safe("failed to load mapping tables from %s", map_path);

    return CHECK_PASSED;
}

int script_map_id(block_r * block, char * map, int id, char ** val) {
    int ret = 0;
    int index = 0;
    lua_State * maps = NULL;

    exit_null_safe(3, block, map, val);

    maps = block->script->map;

    /* push table */
    lua_getglobal(maps, map);
    if(!lua_istable(maps, -1)) {
        exit_func_safe("%s is not a valid map", map);
        goto failed;
    }

    /* push key */
    index = lua_gettop(maps);
    lua_pushinteger(maps, id);
    lua_gettable(maps, index);
    if(!lua_isstring(maps, -1))
        goto failed;

    /* table[key] */
    *val = convert_string(lua_tostring(maps, -1));

clean:
    /* reset the lua stack */
    lua_settop(maps, 0);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int script_map_deit(script_t * script) {
    exit_null_safe(1, script);

    if(NULL != script->map)
        lua_close(script->map);

    return CHECK_PASSED;
}

int script_init(script_t ** script, const char * rdb_path, const char * sdb_path, const char * map_path, int mode) {
    script_t * _script = NULL;

    /* error on invalid reference */
    exit_null_safe(3, script, rdb_path, sdb_path);

    /* error on invalid server mode */
    if(!(mode & DB_MODE))
        return CHECK_FAILED;

    /* construct script struct */
    _script = calloc(1, sizeof(script_t));
    if(NULL == _script)
        return CHECK_FAILED;

    /* set the mode */
    _script->mode = mode;

    /* initialize resource and server databases and mapping tables */
    if (block_init(&_script->blocks) ||
        init_db_load(&_script->db, rdb_path, sdb_path, _script->mode) ||
        script_map_init(_script, map_path)) {
        script_deit(&_script);
        return CHECK_FAILED;
    }

    *script = _script;
    return CHECK_PASSED;
}

int script_deit(script_t ** script) {
    script_t * _script = NULL;

    exit_null_safe(2, script, *script);

    _script = *script;
    /* release all mappings */
    script_map_deit(_script);
    /* release all databases */
    if(_script->db)
        deit_db_load(&_script->db);
    /* release all blocks */
    script_block_release(_script);
    /* release script struct */
    SAFE_FREE(_script);
    *script = NULL;

    return CHECK_PASSED;
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
    exit_null_safe(2, token, script);

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
        if(SCRIPT_STRING(script[i]) || SCRIPT_SYMBOL(script[i])) {
            /* add identifier tokens */
            ptr[ptr_cnt++] = &buf[buf_cnt];
            while(SCRIPT_STRING(script[i]) || SCRIPT_SYMBOL(script[i]))
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
    if(ptr_cnt <= 0)
        return SCRIPT_FAILED;

    /* set the total number of tokens */
    token->script_cnt = ptr_cnt;
    return SCRIPT_PASSED;
}

int script_analysis(script_t * script, token_r * token_list, block_r * parent, block_r ** var) {
    int i = 0;
    int j = 0;
    int len = 0;
    int ret = 0;
    int block_cnt = 0;
    char ** token = NULL;
    int token_cnt = 0;
    block_res block_type;                           /* block id in block_db.txt */
    block_r * block = NULL;                         /* current block being parsed */
    block_r * link = parent;                        /* link blocks to if, else-if, else, and for */
    block_r * set = (var != NULL) ? *var : NULL;
    block_r * iterable_set_block = NULL;
    block_r * direction_set_block = NULL;
    char subscript[BUF_SIZE];

    /* check null paramaters */
    exit_null_safe(2, script, token_list);

    /* dereference the token paramaters */
    token = token_list->script_ptr;
    token_cnt = token_list->script_cnt;

    /* check empty token list */
    if(token_cnt <= 0)
        return SCRIPT_FAILED;

    /* parse tokens to build blocks */
    for(i = 0; i < token_cnt; i++) {
        if(token[i][0] == '{' || token[i][0] == '}') continue;
        if(!block_name(script->db, &block_type, token[i], strlen(token[i]))) {
            /* count the number of blocks parsed */
            block_cnt++;

            /* allocate and initialize a block */
            if(script_block_new(script, &block))
                return SCRIPT_FAILED;

            block->name = token[i];
            block->item_id = script->item.id;
            block->type = block_type.id;

            /* copy the references from script to block
             * to avoid massive rewrite for passing the
             * references. */
            block->script = script;

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

                    /* parse the condition ( <condition> ) */
                    if( token[i+1][0] != '(' ||
                        script_parse(token_list, &i, block, '\0', ')', FLAG_PARSE_LAST_TOKEN) ||
                        token[i][0] != ')')
                        return SCRIPT_FAILED;

                    /* compound or simple statement and perform
                     * script_analysis to create create the new
                     * blocks */
                    if((token[i+1][0] == '{') ?
                        script_parse(token_list, &i, block, '\0', '}', FLAG_PARSE_LAST_TOKEN):
                        script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA) ||
                        script_analysis_(script, block->ptr[1], block, &set))
                        return SCRIPT_FAILED;
                    break;
                case 27: /* parse else blocks */
                    /* link else-if and else blocks */
                    block->link = link;

                    if(strcmp(token[i + 1], "if") == 0) {
                        /* skip the 'if' */
                        i += 1;

                        /* parse the condition ( <condition> ) */
                        if(token[i+1][0] != '(' ||
                           script_parse(token_list, &i, block, '\0', ')', FLAG_PARSE_LAST_TOKEN) ||
                           token[i][0] != ')')
                            return SCRIPT_FAILED;

                        /* compound or simple statement and perform
                         * script_analysis to create create the new
                         * blocks */
                        if((token[i+1][0] == '{') ?
                            script_parse(token_list, &i, block, '\0', '}', FLAG_PARSE_LAST_TOKEN):
                            script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA) ||
                            script_analysis_(script, block->ptr[1], block, &set))
                            return SCRIPT_FAILED;

                        /* else if continues the linking */
                        link = block;
                    } else {
                        /* compound or simple statement */
                        if((token[i+1][0] == '{') ?
                            script_parse(token_list, &i, block, '\0', '}', FLAG_PARSE_LAST_TOKEN):
                            script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA)
                            == SCRIPT_FAILED)
                            return SCRIPT_FAILED;

                        /* extend the block list by using subscript */
                        if(script_analysis_(script, block->ptr[0], block, &set))
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
                    if(script_parse(token_list, &i, block, ',', ';', FLAG_PARSE_NORMAL))
                        return SCRIPT_FAILED;

                    /* extend the block list by using subscript */
                    if(script_analysis_(script, block->ptr[0], block, &set))
                        return SCRIPT_FAILED;
                    break;
                case 62: /* parse for blocks */
                    /* skip the starting parenthesis */
                    if(token[i+1][0] != '(') {
                        exit_func_safe("missing starting parenthesis for 'for' loop in "
                        "item id %d current token is %s", script->item.id, token[i+1]);
                        return SCRIPT_FAILED;
                    } else {
                        i++;
                    }

                    /* parse the for-loop initialization */
                    if(script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_ALL_FLAGS))
                        return SCRIPT_FAILED;

                    /* check whether the initialization is empty */
                    if(check_loop_expression(script, block->ptr[0], ";")) {
                        exit_func_safe("missing initialization expression"
                        " in for-loop for item id %d", script->item.id);
                        return SCRIPT_FAILED;
                    }

                    /* parse the for-loop condition */
                    if(script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_ALL_FLAGS))
                        return SCRIPT_FAILED;

                    /* check whether the initialization is empty */
                    if(check_loop_expression(script, block->ptr[1], ";")) {
                        exit_func_safe("missing conditional expression"
                        " in for-loop for item id %d", script->item.id);
                        return SCRIPT_FAILED;
                    }

                    /* parse the variant to know whether it goes up or down */
                    if(script_parse(token_list, &i, block, '\0', ')', FLAG_PARSE_ALL_FLAGS))
                        return SCRIPT_FAILED;

                    if(check_loop_expression(script, block->ptr[2], ")")) {
                        exit_func_safe("missing variant expression"
                        " in for-loop for item id %d", script->item.id);
                        return SCRIPT_FAILED;
                    }

                    /* compound or simple statement */
                    if((token[i+1][0] == '{') ?
                        script_parse(token_list, &i, block, '\0', '}', FLAG_PARSE_LAST_TOKEN):
                        script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA))
                        return SCRIPT_FAILED;

                    /* check that the compound or simple statement is not empty */
                    if(check_loop_expression(script, block->ptr[3], ";"))
                        return exit_func_safe("missing block expression"
                        " in for-loop for item id %d", script->item.id);

                    /* remove the semicolon in the condition */
                    len = strlen(block->ptr[1]);
                    for(j = len; j >= 0; j--)
                        if(block->ptr[1][j] == ';') {
                            block->ptr[1][j] = '\0';
                            break;
                        }

                    /* remove the parenthesis in the variant */
                    len = strlen(block->ptr[2]);
                    for(j = len; j >= 0; j--)
                        if(block->ptr[2][j] == ')') {
                            block->ptr[2][j] = ';';
                            break;
                        }

                    /* perform script_analysis on the init-expr of for */
                    if(script_analysis_(script, block->ptr[0], parent, &set)) {
                        return exit_func_safe("failed to create iterable set bl"
                        "ock '%s' in item id %d", block->ptr[0], script->item.id);
                    } else {
                        /* the root block always points to the last block used */
                        iterable_set_block = script->blocks;
                    }

                    /* perform script_analysis on the cond-expr of for */
                    if(script_analysis_(script, block->ptr[2], parent, &set)) {
                        return exit_func_safe("failed to create direction set bl"
                        "ock '%s' in item id %d", block->ptr[2], script->item.id);
                    } else {
                        /* the root block always points to the last block used */
                        direction_set_block = script->blocks;
                    }

                    /* rearranging the condition and set block in this format is a design choice
                     * for building the initial condition tree over the entire integer range and
                     * then depending on the direction, adjust the range from the set block to
                     * produce the best 'guess' of the iterable range. */
                    sprintf(subscript, "(%s) ? (%s) : 0", block->ptr[1], iterable_set_block->ptr[1]);
                    if( block_stack_pop(iterable_set_block, TYPE_PTR) ||
                        block_stack_push(iterable_set_block, TYPE_PTR, subscript))
                        return SCRIPT_FAILED;
                    iterable_set_block->flag |= EVALUATE_FLAG_ITERABLE_SET;

                    /* cheap hack to evaluate a 0 (increasing) or 1 (decreasing) */
                    sprintf(subscript, "(%s) < (%s)", direction_set_block->ptr[0], direction_set_block->ptr[1]);
                    if( block_stack_pop(direction_set_block, TYPE_PTR) ||
                        block_stack_push(direction_set_block, TYPE_PTR, subscript))
                        return SCRIPT_FAILED;
                    direction_set_block->flag |= EVALUATE_FLAG_VARIANT_SET;

                    /* perform script_analysis on the statements of for */
                    if(script_analysis_(script, block->ptr[3], parent, &set))
                        return exit_func_safe("failed to create if block "
                        "'%s' in item id %d", subscript, script->item.id);
                    break;
                case 28:
                    /* add new set block as the tail */
                    if(var != NULL) *var = block;
                    set = block;
                default: /* parse all blocks */
                    if(script_parse(token_list, &i, block, ',', ';', FLAG_PARSE_NORMAL))
                        return SCRIPT_FAILED;
                    break;
            }
            /* parsing is off-by-one */
            block->ptr_cnt--;
        }
    }

    return SCRIPT_PASSED;
}

int script_analysis_(script_t * script, char * subscript, block_r * parent, block_r ** var) {
    token_r * token = NULL;

    exit_null_safe(2, script, subscript);

    token = calloc(1, sizeof(token_r));
    if(NULL == token)
        return CHECK_FAILED;

    if( script_lexical(token, subscript) ||
        script_analysis(script, token, parent, var)) {
        SAFE_FREE(token);
        return SCRIPT_FAILED;
    }

    SAFE_FREE(token);
    return SCRIPT_PASSED;
}

int check_loop_expression(script_t * script, char * expr, char * end) {
    token_r token;
    if(script_lexical(&token, expr)) {
        exit_func_safe("failed to tokenize '%s' for item id %d", expr, script->item.id);
        return SCRIPT_FAILED;
    }

    /* check whether ; semicolon exist */
    if(token.script_cnt <= 0) {
        exit_func_safe("missing '%s' in for loop for item id %d", end, script->item.id);
        return SCRIPT_FAILED;
    }

    /* check whether ; is the first token */
    return (strcmp(token.script_ptr[0], end) == 0) ? SCRIPT_FAILED : SCRIPT_PASSED;
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
    exit_null_safe(3, token, position, block);

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
        /* indicate bracket level or subexpression level */
        switch(script_ptr[i][0]) {
            case '{': bracket_level++; break;
            case '}': bracket_level--; break;
            case '(': subexpr_level++; break;
            case ')': subexpr_level--; break;
            default: break;
        }

        /* parsing top-level script; delimit exit only at top-level
         * flag 4 allows us to bypass any leveling */
        if((!bracket_level && !subexpr_level) || flag & FLAG_PARSE_SKIP_LEVEL) {
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
                if(flag & FLAG_PARSE_LAST_TOKEN) {
                    /* write the argument */
                    len = strlen(script_ptr[i]);
                    for(j = 0; j < len; j++)
                        arg[arg_cnt++] = script_ptr[i][j];

                    /* don't add space after athena symbol prefixes */
                    if(!SCRIPT_SYMBOL(script_ptr[i][0]))
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
        if(script_ptr[i][0] == ',' && !bracket_level && !subexpr_level && !(flag & FLAG_PARSE_KEEP_COMMA))
            continue;

        /* write the argument */
        len = strlen(script_ptr[i]);
        for (j = 0; j < len; j++)
            arg[arg_cnt++] = script_ptr[i][j];

        /* don't add space after athena symbol prefixes */
        if (!SCRIPT_SYMBOL(script_ptr[i][0]) && script_ptr[i][0] != '=')
            arg[arg_cnt++] = ' ';
    }

    block->arg_cnt = arg_cnt;
    block->ptr_cnt = ptr_cnt;
    *position = i;
    return SCRIPT_PASSED;
}

int script_translate(script_t * script) {
    int ret = 0;
    block_r * iter = NULL;
    block_r * end = NULL;
    block_r * set = NULL;
    node_t * node = NULL;
    logic_node_t * logic_tree = NULL;

    /* handling iterable set ranges */
    range_t * iterable_range;
    range_t * temp_range;
    int val_min = 0;
    int val_max = 0;

    /* check null paramaters */
    exit_null_safe(1, script);

    /* translate each block */
    iter = script->blocks;
    do {
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
            case 8: ret = translate_heal(iter); break;                                                              /* heal */
            case 9: ret = translate_heal(iter); break;                                                              /* percentheal */
            case 10: ret = translate_heal(iter); break;                                                             /* itemheal */
            case 11: ret = translate_skill(iter); break;                                                            /* skill */
            case 12: ret = translate_itemskill(iter); break;                                                        /* itemskill */
            case 13: ret = translate_skill_block(iter, iter->type); break;                                          /* unitskilluseid */
            case 14: ret = translate_status(iter); break;                                                           /* sc_start */
            case 15: ret = exit_func_safe("maintenance"); break;                                                    /* sc_start4 */
            case 16: ret = translate_status_end(iter); break;                                                       /* sc_end */
            case 17: ret = translate_getitem(iter); break;                                                          /* getitem */
            case 18: ret = translate_rentitem(iter); break;                                                         /* rentitem */
            case 19: ret = translate_delitem(iter); break;                                                          /* delitem */
            case 20: ret = translate_getrandgroup(iter, iter->type); break;                                         /* getrandgroupitem */
            case 23: ret = translate_write(iter, "Set new font.", 1); break;                                        /* setfont */
            case 24: ret = translate_buyingstore(iter); break;                                                      /* buyingstore */
            case 25: ret = translate_searchstore(iter); break;                                                      /* searchstores */
            case 30: ret = translate_write(iter, "Send a message through the announcement system.", 1); break;      /* announce */
            case 31: ret = translate_misc(iter, "callfunc"); break;                                                 /* callfunc */
            case 33: ret = translate_misc(iter, "warp"); break;                                                     /* warp */
            case 35: ret = translate_write(iter, "Hatch a pet egg.", 1); break;                                     /* bpet */
            case 34: ret = translate_pet_egg(iter); break;                                                          /* pet */
            case 36: ret = translate_hire_merc(iter, iter->type); break;                                            /* mercenary_create */
            case 37: ret = translate_heal(iter); break;                                                             /* mercenary_heal */
            case 38: ret = exit_func_safe("maintenance"); break;                                                    /* mercenary_sc_status */
            case 39: ret = translate_produce(iter, iter->type); break;                                              /* produce */
            case 40: ret = translate_produce(iter, iter->type); break;                                              /* cooking */
            case 41: ret = exit_func_safe("maintenance"); break;                                                    /* makerune */
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
            case 54: ret = exit_func_safe("maintenance"); break;                                                    /* sc_start2 */
            case 55: ret = translate_petloot(iter); break;                                                          /* petloot */
            case 56: ret = translate_petrecovery(iter); break;                                                      /* petrecovery */
            case 57: ret = translate_petskillbonus(iter); break;                                                    /* petskillbonus */
            case 58: ret = translate_petskillattack(iter); break;                                                   /* petskillattack */
            case 59: ret = translate_petskillattack2(iter); break;                                                  /* petskillattack2 */
            case 60: ret = translate_petskillsupport(iter); break;                                                  /* petskillsupport */
            case 61: ret = translate_petheal(iter); break;                                                          /* petheal */
            /* non-simple structures */
            case 26:
                node = evaluate_expression(iter, iter->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_KEEP_LOGIC_TREE | EVALUATE_FLAG_EXPR_BOOL);
                if(NULL == node)
                    return CHECK_FAILED;
                node_free(node);
                break;                                                                                              /* if */
            case 27: /* invert the linked logic tree; only the top needs to be inverted */
                if(iter->logic_tree != NULL) {
                    logic_tree = iter->logic_tree;
                    iter->logic_tree = inverse_logic_tree(iter->logic_tree);
                    iter->logic_tree->stack = logic_tree->stack;
                    freenamerange(logic_tree);
                }

                /* add the else if condition onto the stack */
                if(iter->ptr_cnt > 1) {
                    node = evaluate_expression(iter, iter->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_KEEP_LOGIC_TREE | EVALUATE_FLAG_EXPR_BOOL);
                    if(NULL == node)
                        return CHECK_FAILED;
                    node_free(node);
                }
               break;                                                                                              /* else */
            case 28:
                if(iter->flag & EVALUATE_FLAG_ITERABLE_SET) {
                    iter->set_node = evaluate_expression(iter, iter->ptr[1], 1,
                        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_KEEP_LOGIC_TREE |
                        EVALUATE_FLAG_WRITE_FORMULA | EVALUATE_FLAG_KEEP_TEMP_TREE |
                        EVALUATE_FLAG_EXPR_BOOL | EVALUATE_FLAG_ITERABLE_SET);
                } else if(iter->flag & EVALUATE_FLAG_VARIANT_SET) {
                    iter->set_node = evaluate_expression(iter, iter->ptr[1], 1,
                        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_KEEP_LOGIC_TREE |
                        EVALUATE_FLAG_WRITE_FORMULA | EVALUATE_FLAG_KEEP_TEMP_TREE |
                        EVALUATE_FLAG_VARIANT_SET);
                } else {
                    iter->set_node = evaluate_expression(iter, iter->ptr[1], 1,
                        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_KEEP_LOGIC_TREE |
                        EVALUATE_FLAG_WRITE_FORMULA | EVALUATE_FLAG_KEEP_TEMP_TREE |
                        EVALUATE_FLAG_EXPR_BOOL );
                }

                /* special thanks to 'iterable' ranges for making set block 100x more complex! */
                if(iter->set_node == NULL) {
                    exit_func_safe("failed to evaluate set block expression "
                    "'%s' on item id %d", iter->ptr[1], iter->item_id);
                    return SCRIPT_FAILED;
                }

                /* calculate a reasonable iterable range */
                if(iter->flag & EVALUATE_FLAG_VARIANT_SET) {
                    set = iter->set;   /* the parent is the non-variant */

                    /* check that the non-variant set block is linked */
                    if(set == NULL) {
                        exit_func_safe("failed to search non-variant set block "
                        "for '%s' in item %d", iter->ptr[0], iter->item_id);
                        return SCRIPT_FAILED;
                    }

                    /* check that the condition was successfully analysis */
                    if(set->logic_tree == NULL) {
                        exit_func_safe("failed to build logic tree for set "
                        "expression '%s' int item %d", set->ptr[1], set->item_id);
                    }

                    /* search for the range of the set variable */
                    iterable_range = search_tree_dependency_or(set->logic_tree, set->ptr[0], set->set_node->range);
                    if(iterable_range == NULL) {
                        exit_func_safe("failed to search iterable set block range '%s'"
                        " from logic tree in item %d", set->ptr[0], set->item_id);
                        dmpnamerange(set->logic_tree, stderr, 0);
                        return SCRIPT_FAILED;
                    } else {
                        val_min = RANGE_MIN(iterable_range, set->set_node->range);
                        val_max = RANGE_MAX(iterable_range, set->set_node->range);
                        temp_range = iterable_range;
                        if(IS_SUBSET_OF(set->set_node->range, iterable_range)) {
                            switch(iter->set_node->range->min) {
                                case -1: iterable_range = mkrange(INIT_OPERATOR, set->set_node->range->min, val_max, DONT_CARE); break;
                                case  0: iterable_range = andrange(iterable_range, set->set_node->range); break;
                                case  1: iterable_range = mkrange(INIT_OPERATOR, val_min, set->set_node->range->max, DONT_CARE); break;
                                default: /* variant must evaluate to -1, 0, and 1 */
                                    exit_func_safe("invalid variant value %d in item "
                                    "id %d", iter->set_node->range->min, iter->item_id);
                                    break;
                            }
                        } else {
                            iterable_range = mkrange(INIT_OPERATOR, val_min, val_max, DONT_CARE); break;
                        }
                        freerange(temp_range);

                        /* update the set block range to be iterable range */
                        temp_range = iter->set_node->range;
                        iter->set_node->min = minrange(iterable_range);
                        iter->set_node->max = maxrange(iterable_range);
                        iter->set_node->range = iterable_range;
                        iter->set_node->range->id_min = set->set_node->min;
                        iter->set_node->range->id_max = set->set_node->max;
                        freerange(temp_range);
                    }
                }
                break;                                                                                              /* set */
            case 62: break;
            case 21: /* skilleffect */
            case 22: /* specialeffect2 */
            case 29: /* input */
            case 32: /* end */
               break;
            default: return SCRIPT_FAILED;
        }

        /* script cannot be translated */
        if(ret)
            return SCRIPT_FAILED;

        iter = iter->next;
    } while(iter != script->blocks && !iter->free);

    return SCRIPT_PASSED;
}

int script_generate(script_t * script, char * buffer, int * offset) {
    block_r * iter = NULL;

    exit_null_safe(3, script, buffer, offset);

    iter = script->blocks;
    do {
        switch(iter->type) {
            case 26: /* if */
            case 27: /* else */
                /* traverse logic tree */
                break;
            case 28: /* set */
                /* special case for zeny */
                break;
            case 21: /* skilleffect */
            case 22: /* specialeffect2 */
            case 29: /* input */
            case 32: /* end */
                break;
            default:
                if(iter->eng_cnt != 1)
                    return CHECK_FAILED;
                *offset += sprintf(&buffer[*offset], "%s", iter->eng[0]);
        }
        iter = iter->next;
    } while (iter != script->blocks && !iter->free);
    return SCRIPT_PASSED;
}

int script_combo(int item_id, char * buffer, int * offset, db_t * db, int mode) {
    //char * item_desc = NULL;
    //combo_t * item_combo_list = NULL;
    //combo_t * item_combo_iter = NULL;

    ///* eathena does not have item combos */
    //if(EATHENA == mode)
    //    return CHECK_PASSED;

    ///* search the item id for combos */
    //if(!item_combo_id(db, &item_combo_list, item_id)) {
    //    item_combo_iter = item_combo_list;
    //    while(item_combo_iter != NULL) {
    //        *offset += sprintf(buffer + *offset, "%s\n", item_combo_iter->group);
    //        /* compile each script write to buffer */
    //        item_desc = script_compile(item_combo_iter->script, item_id, db, mode);
    //        if(item_desc != NULL) {
    //            *offset += sprintf(buffer + *offset, "%s", item_desc);
    //            free(item_desc);
    //        }
    //        item_combo_iter = item_combo_iter->next;
    //    }
    //    buffer[*offset] = '\0';
    //    /* free the list of item combo */
    //    if(item_combo_list != NULL)
    //        item_combo_free(&item_combo_list);
    //}
    return SCRIPT_PASSED;
}

int script_recursive(db_t * db, int mode, lua_State * map, char * subscript, char ** value) {
    int ret = 0;
    script_t * script = NULL;

    /* error on invalid references */
    exit_null_safe(4, db, map, subscript, value);

    /* build a "sub-script" (lol) context */
    script = calloc(1, sizeof(script_t));
    if(NULL == script)
        return CHECK_FAILED;

    /* set the database and mapping references */
    script->db = db;
    script->mode = mode;
    script->map = map;

    /* allocate block list */
    if(block_init(&script->blocks))
        goto failed;

    /* set the initial buffer state */
    script->offset = 0;
    script->buffer[0] = '\0';


    /* compile the item script */
    if( script_lexical(&script->token, subscript) ||
        script_analysis(script, &script->token, NULL, NULL) ||
        script_translate(script) ||
        script_generate(script, script->buffer, &script->offset))
        goto failed;

    /* copy the translated script to output */
    *value = convert_string(script->buffer);

clean:
    /* free the block list */
    script_block_free_all(script);
    /* free the sub-script context */
    SAFE_FREE(script);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

/* interpret the function call syntax by parsing each argument that
 * is delimited by a comma and  writing each  argument's expression
 * onto the block's ptr stack
 *
 * if the expression is "(0, 1)" then
 *      block->ptr[n + 0] = "0"
 *      block->ptr[n + 1] = "1"
 *      *argc = 2;
 * i.e. the arguments will be pushed onto the block->ptr stack
 * and the number of arguments pushed will be returned by argc
 */
int stack_ptr_call(block_r * block, char * expr, int * argc) {
    int len = 0;
    int err = CHECK_PASSED;
    token_r * token = NULL;

    /* check for null arguments */
    exit_null_safe(3, block, expr, argc);

    /* check for empty expression */
    len = strlen(expr);
    if(0 >= len)
        return CHECK_FAILED;

    /* check for invalid syntax */
    if(expr[0] != '(' && expr[len] != ')')
        return CHECK_FAILED;

    /* tokenize the expression */
    token = calloc(1, sizeof(token_r));
    if(NULL == token)
        return CHECK_FAILED;

    if( script_lexical(token, expr) ||
        0 >= token->script_cnt ||
        stack_ptr_call_(block, token, argc))
        err = CHECK_FAILED;

    SAFE_FREE(token);
    return err;
}

int stack_ptr_call_(block_r * block, token_r * token, int * argc) {
    int pos = 0;
    int top = 0;

    /* check for null arguments */
    exit_null_safe(3, block, token, argc);

    /* remove the ending parenthesis */
    token->script_cnt--;

    /* prior to calling script_parse,  the block->ptr stack must be
     * initialized by setting the top of block->ptr to the point to
     * the end of the block->arg buffer */
    top = block->ptr_cnt;
    block->ptr[top] = &block->arg[block->arg_cnt];
    block->ptr_cnt++;

    /* script_parse will push the arguments to the block->ptr stack
     * but script_parse just works but totally full of problems =.=
     * bandaids must be used. */
    if(script_parse(token, &pos, block, ',', '\0', FLAG_PARSE_NORMAL))
        return CHECK_FAILED;

    /* script_parse will not set the NULL for the lasted argument */
    block->arg[block->arg_cnt - 1] = '\0';

    /* number of arguments pushed onto block->ptr can be calculated
     * by taking  the difference of the  previous and current stack
     * size. */
    *argc = block->ptr_cnt - top;

    return CHECK_PASSED;
}

/* evaluate the expression for a single or multiple item id
 * and convert each item id to item name and write the item
 * name onto the block->eng stack
 *
 * if the expression is "1101" then
 *      block->eng[n + 0] = "sword"
 *      *argc = 1;
 *
 * if the expression is "1101 + 2" then
 *      block->eng[n + 0] = "sword" (1101)
 *      block->eng[n + 1] = "sword" (1102)
 *      block->eng[n + 2] = "sword" (1103)
 *
 * but to prevent stack overflow on the block->eng stack;
 * if more than 5 items, then the list will cut off with
 * a note to indicate that not all items are listed.
 */
int stack_eng_item(block_r * block, char * expr, int * argc) {
    int i = 0;
    int len = 0;                /* length of the expression */
    int top = 0;                /* top of the block->eng stack */
    int cnt = 0;                /* total number of arguments pushed */
    int ret = CHECK_PASSED;
    item_t * item = NULL;
    node_t * node = NULL;
    range_t * iter = NULL;

    /* check for null arguments */
    exit_null_safe(3, block, expr, argc);

    /* check for empty expression */
    len = strlen(expr);
    if(0 >= len)
        return CHECK_FAILED;

    item = calloc(1, sizeof(item_t));
    if(NULL == item)
        return CHECK_FAILED;

    top = block->eng_cnt;

    /* if the expression begins with a letter or under
     * score then the expression might be an item name */
    if(isalpha(expr[0]) || expr[0] == '_') {
        if(!item_name(block->script->db, item, expr, len)) {
            if(block_stack_push(block, TYPE_ENG, item->name))
                ret = CHECK_FAILED;
            goto clean;
        }
    }

    /* evaluate the expression for a single or multiple item id */
    node = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == node) {
        ret = CHECK_FAILED;
    } else {
        /* map each item id to name and write
         * each item name to block->eng stack */
        iter = node->range;
        while(iter != NULL) {
            for(i = iter->min; i <= iter->max && cnt < MAX_ITEM_LIST; i++) {
                if( item_id(block->script->db, item, i) ||
                    block_stack_push(block, TYPE_ENG, item->name)) {
                    /* invalid item id or block->eng stacked overflow */
                    ret = CHECK_FAILED;
                    goto return_type;
                }
                cnt++;
            }
            /* stop pushing item names onto the stack */
            if(cnt >= MAX_ITEM_LIST)
                break;
            iter = iter->next;
        }
    }

clean:
    *argc = block->eng_cnt - top;
    SAFE_FREE(item);
    node_free(node);
    return ret;

return_type:
    /* check whether the node has a return type of item id
     * if so, then write that formula instead of item name */
    if(node->return_type & ITEM_TYPE_FLAG &&
       node->formula != NULL)
        ret = block_stack_push(block, TYPE_ENG, node->formula);
    goto clean;
}

int stack_eng_skill(block_r * block, char * expr, int * argc) {
    int i = 0;
    int len = 0;
    int top = 0;
    int ret = CHECK_PASSED;
    skill_t * skill = NULL;
    node_t * node = NULL;
    range_t * iter = NULL;

    /* check for null arguments */
    exit_null_safe(3, block, expr, argc);

    /* check for empty expression */
    len = strlen(expr);
    if(0 >= len)
        return CHECK_FAILED;

    skill = calloc(1, sizeof(skill_t));
    if(NULL == skill)
        return CHECK_FAILED;

    top = block->eng_cnt;

    /* if the expression begins with a letter or under
     * score then the expression might be an skill name */
    if(isalpha(expr[0]) || expr[0] == '_') {
        if(!skill_name(block->script->db, skill, expr, len)) {
            if(block_stack_push(block, TYPE_ENG, skill->desc))
                ret = CHECK_FAILED;
            goto clean;
        }
    }

    /* evaluate the expression for a single or multiple skill id */
    node = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == node) {
        ret = CHECK_FAILED;
    } else {
        /* map each skill id to name and write
         * each skill name to block->eng stack */
        iter = node->range;
        while(iter != NULL) {
            for(i = iter->min; i <= iter->max; i++) {
                if( skill_id(block->script->db, skill, i) ||
                    block_stack_push(block, TYPE_ENG, skill->desc)) {
                    /* invalid skill id or block->eng stacked overflow */
                    ret = CHECK_FAILED;
                    goto clean;
                }
            }
            iter = iter->next;
        }
    }

clean:
    *argc = block->eng_cnt - top;
    SAFE_FREE(skill);
    node_free(node);
    return ret;
}

int stack_eng_grid(block_r * block, char * expr) {
    int ret = 0;
    int off = 0;
    int splash_min = 0;
    int splash_max = 0;
    char buf[256];
    node_t * grid = NULL;

    exit_null_safe(2, block, expr);

    /* evaluate the grid expression*/
    grid = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == grid)
        return CHECK_FAILED;

    /* calculate the splash grid */
    if(grid->min != grid->max) {
        splash_min = grid->min * 2 + 1;
        splash_max = grid->max * 2 + 1;
    } else {
        splash_min = grid->min * 2 + 1;
    }

    if(splash_min > 0 && splash_max > 0)
        off += sprintf(buf, "%d x %d to %d x %d", splash_min, splash_min, splash_min, splash_min);
    else if(splash_min > 0)
        off += sprintf(buf, "%d x %d", splash_min, splash_min);
    else if(splash_max > 0)
        off += sprintf(buf, "%d x %d", splash_max, splash_max);

    if(stack_aux_formula(block, grid, buf))
        goto failed;

clean:
    node_free(grid);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

/* evaluate the expression and write the integer range
 * onto the block->eng stack along with any dependency
 *
 * if the expression is "getrefine() + 10" then
 *      block->eng[n + 0] = "10 ~ 25 (refine rate)"
 */
int stack_eng_int(block_r * block, char * expr, int modifier) {
    int flag = 0;
    node_t * node = NULL;

    exit_null_safe(2, block, expr);

    /* push the integer result on the stack */
    flag = EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA | EVALUATE_FLAG_WRITE_STACK;
    node = evaluate_expression(block, expr, modifier, flag);
    if (node == NULL)
        return CHECK_FAILED;

    node_free(node);
    return CHECK_PASSED;
}

/* evaluate an expression and select the prefix
 * depending on whether the result is positive,
 * negative, or both.
 *
 * if the expression is "10 - getrefine()" then
 *      block->eng[n + 0] = "positive 0 ~ 10 or"
 *                          "negative 0 ~ 5"
 *      (since 10 - getrefine() = -5 to 10)
 *      *argc = 1;
 *
 * if the expression is "10 " then
 *      block->eng[n + 0] = "positive 10"
 *      *argc = 1;
 *
 * if the expression is "-10 " then
 *      block->eng[n + 0] = "negative 10"
 *      *argc = 1;
 */
int stack_eng_int_signed(block_r * block, char * expr, int modifier, const char * pos, const char * neg) {
    int ret = 0;
    int min = 0;
    int max = 0;
    int len = 0;
    char * buf = NULL;
    node_t * node = NULL;

    /* error on invalid reference */
    exit_null_safe(4, block, expr, pos, neg);

    len = strlen(pos) + strlen(neg) + 128;
    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    /* push the integer result on the stack */
    node = evaluate_expression(block, expr, modifier, EVALUATE_FLAG_KEEP_NODE);
    if (node == NULL)
        goto failed;

    min = node->min;
    max = node->max;

    /* failed on the odd case */
    if(0 == min && 0 == max)
        goto failed;

    /* write the positive and negative ranges */
    if(min > 0 && max > 0) {
        (min == max) ?
            sprintf(buf, "%s %d", pos, min):
            sprintf(buf, "%s %d ~ %d", pos, min, max);
    } else if(min < 0 && max > 0) {
        sprintf(buf, "%s 0 ~ %d or %s 0 ~ %d", neg, min * -1, pos, max);
    } else if(min > 0 && max < 0) {
        sprintf(buf, "%s 0 ~ %d or %s 0 ~ %d", pos, min, neg, max * -1);
    } else {
        (min == max) ?
            sprintf(buf, "%s %d", neg, min):
            sprintf(buf, "%s %d ~ %d", neg, min, max);
    }

    /* write buffer with formula */
    if(stack_aux_formula(block, node, buf))
        goto failed;

clean:
    node_free(node);
    SAFE_FREE(buf);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

/* evaluate the expression for a single or multiple item id
 * and convert each item id to item name and write the item
 * name onto the block->eng stack
 *
 * if the expression is "1101" then
 *      block->eng[n + 0] = "sword"
 *      *argc = 1;
 */
int stack_eng_time(block_r * block, char * expr, int modifier) {
    node_t * time = NULL;
    int tick_min = 0;
    int tick_max = 0;
    int time_unit = 0;
    char * time_suffix = NULL;

    /* maximum 32-bit integer require up to 11 characters plus -
     * enough padding room for additional suffix and formatting
     * characters. */
    int len = 0;
    char buf[64];

    /* check null */
    exit_null_safe(2, block, expr);

    /* evaluate the expression and convert to time string */
    time = evaluate_expression(block, expr, modifier, EVALUATE_FLAG_KEEP_NODE);
    if (time == NULL)
        return SCRIPT_FAILED;

    /* get the minimum and maximum of the time expression */
    tick_min = time->min;
    tick_max = time->max;

    /* get the closest time metric that can divide the total number of milliseconds */
    if (tick_min / 86400000 > 1) {
        time_suffix = "day";
        time_unit = 86400000;
    }
    else if (tick_min / 3600000 > 1) {
        time_suffix = "hour";
        time_unit = 3600000;
    }
    else if (tick_min / 60000 > 1) {
        time_suffix = "minute";
        time_unit = 60000;
    }
    else {
        time_suffix = "second";
        time_unit = 1000;
    }

    /* convert millisecond to time metric */
    tick_min /= time_unit;
    tick_max /= time_unit;

    /* write time string to buffer */
    len = (tick_min == tick_max) ?
        sprintf(buf, "%d %s%s", tick_min, time_suffix, tick_min > 1 ? "s" : "") :
        sprintf(buf, "%d ~ %d %ss", tick_min, tick_max, time_suffix);

    /* check overflow but sprintf can crash in sprintf */
    if (len > 64) {
        exit_func_safe("buffer overflow in item %d", block->item_id);
        goto failed;
    }

    /* write buffer with formula */
    if(stack_aux_formula(block, time, buf))
        goto failed;

    node_free(time);
    return CHECK_PASSED;

failed:
    node_free(time);
    return CHECK_FAILED;
}

/* evaluate the expression into an item level constant
 * which used to search  for all item recipes with the
 * same item level.
 *
 * if the expression is "21" then
 * (depend on the produce table)
 *      block->eng[n + 1] = Recipe for Flame Heart
 *      block->eng[n + 2] = 10 Red Blood
 *      block->eng[n + 3] = Recipe for Mystic Frozen
 *      block->eng[n + 4] = 10 Crystal Blue
 *      block->eng[n + 5] = Recipe for Rough Wind
 *      block->eng[n + 6] = 10 Wind of Verdure
 *      block->eng[n + 7] = Recipe for Great Nature
 *      block->eng[n + 8] = 10 Green Live
 *      block->eng[n + 9] = Recipe for Iron
 *      block->eng[n + 10] = 1 Iron Ore
 *      block->eng[n + 11] = Recipe for Steel
 *      block->eng[n + 12] = 5 Iron
 *      block->eng[n + 13] = 1 Coal
 *      block->eng[n + 14] = Recipe for Star Crumb
 *      block->eng[n + 15] = 10 Star Dust
 *      *argc = 16;
 */
int stack_eng_produce(block_r * block, char * expr, int * argc) {
    int i = 0;
    int ret = 0;
    int top = 0;
    item_t * item;
    node_t * item_level = 0;
    produce_t * produce = NULL;
    produce_t * produces = NULL;
    char buf[MAX_NAME_SIZE + 32];

    exit_null_safe(3, block, expr, argc);

    item = calloc(1, sizeof(item_t));
    if(NULL == item)
        return CHECK_FAILED;

    top = block->eng_cnt;

    item_level = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == item_level)
        return CHECK_FAILED;

    /* error on invalid constant; multiple values supported */
    if(item_level->min != item_level->max)
        goto failed;

    /* write each item recipe on the stack */
    if(produce_id(block->script->db, &produces, item_level->min))
        goto failed;

    /* write the produce header */
    switch(item_level->min) {
        case 1: ret = block_stack_push(block, TYPE_ENG, "Use to craft level 1 weapons.\n"); break;                     /* lv1 weapons */
        case 2: ret = block_stack_push(block, TYPE_ENG, "Use to craft level 2 weapons.\n"); break;                     /* lv2 weapons */
        case 3: ret = block_stack_push(block, TYPE_ENG, "Use to craft level 3 weapons.\n"); break;                     /* lv3 weapons */
        case 11: ret = block_stack_push(block, TYPE_ENG, "Use to cook recipes with rank 5 success rate.\n"); break;    /* cooking sets */
        case 12: ret = block_stack_push(block, TYPE_ENG, "Use to cook recipes with rank 4 success rate.\n"); break;
        case 13: ret = block_stack_push(block, TYPE_ENG, "Use to cook recipes with rank 3 success rate.\n"); break;
        case 14: ret = block_stack_push(block, TYPE_ENG, "Use to cook recipes with rank 2 success rate.\n"); break;
        case 15: ret = block_stack_push(block, TYPE_ENG, "Use to cook recipes with rank 1 success rate.\n"); break;
        case 21: ret = block_stack_push(block, TYPE_ENG, "Use to manufacture metals.\n"); break;                       /* metals */
        default:
            exit_func_safe("unsupported item level %d in item %d", item_level->min, block->item_id);
            goto failed;
    }
    if(ret)
        goto failed;

    produce = produces;
    while(produce != NULL) {
        if(item_id(block->script->db, item, produce->item_id))
            goto failed;

        sprintf(buf, "Recipe for %s", item->name);

        if(block_stack_push(block, TYPE_ENG, buf))
            goto failed;

        for(i = 0; i < produce->ingredient_count; i++) {
            if(item_id(block->script->db, item, produce->item_id_req[i]))
                goto failed;

            sprintf(buf, "%d %s", produce->item_amount_req[i], item->name);

            if(block_stack_push(block, TYPE_ENG, buf))
                goto failed;
        }
        produce = produce->next;
    }

clean:
    *argc = block->eng_cnt - top;
    produce_free(&produces);
    node_free(item_level);
    SAFE_FREE(item);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

/* evaluate the expression into an id
 * and use the id as key for the maps
 * (see athena_db.txt for all maps)
 *
 * if the expression is "1" and flag MAP_AMMO_FLAG then
 *      block->eng[n + 1] = ammo_type[i]
 *      *argc = 1;
 */
int stack_eng_map(block_r * block, char * expr, int flag, int * argc) {
    int i = 0;
    int ret = 0;
    int top = 0;
    char * value = NULL;
    node_t * id = NULL;
    range_t * iter = NULL;

    exit_null_safe(3, block, expr, argc);

    if (0 == flag)
        return CHECK_FAILED;


    top = block->eng_cnt;

    id = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == id)
        return CHECK_FAILED;

    /* iterate the linked list of ranges */
    iter = id->range;
    while(NULL != iter) {
        /* iterate the values within a range */
        for(i = iter->min; i <= iter->max; i++) {
            if(MAP_AMMO_FLAG & flag && !script_map_id(block, "ammo_type", i, &value))
                goto found;
            if(MAP_CAST_FLAG & flag && !script_map_id(block, "cast_flag", i, &value))
                goto found;
            if(MAP_CLASS_FLAG & flag && !script_map_id(block, "class_type", i, &value))
                goto found;
            if(MAP_EFFECT_FLAG & flag && !script_map_id(block, "effect_type", i, &value))
                goto found;
            if(MAP_ELEMENT_FLAG & flag && !script_map_id(block, "element_type", i, &value))
                goto found;
            if(MAP_LOCATION_FLAG & flag && !script_map_id(block, "item_location", i, &value))
                goto found;
            if(MAP_ITEM_FLAG & flag && !script_map_id(block, "item_type", i, &value))
                goto found;
            if(MAP_JOB_FLAG & flag && !script_map_id(block, "job_type", i, &value))
                goto found;
            if(MAP_RACE_FLAG & flag && !script_map_id(block, "race_type", i, &value))
                goto found;
            if(MAP_READPARAM_FLAG & flag && !script_map_id(block, "readparam", i, &value))
                goto found;
            if(MAP_REGEN_FLAG & flag && !script_map_id(block, "regen_flag", i, &value))
                goto found;
            if(MAP_SEARCHSTORE_FLAG & flag && !script_map_id(block, "search_store", i, &value))
                goto found;
            if(MAP_SIZE_FLAG & flag && !script_map_id(block, "size_type", i, &value))
                goto found;
            if(MAP_SP_FLAG & flag && !script_map_id(block, "sp_flag", i, &value))
                goto found;
            if(MAP_TARGET_FLAG & flag && !script_map_id(block, "target_flag", i, &value))
                goto found;
            if(MAP_WEAPON_FLAG & flag && !script_map_id(block, "weapon_type", i, &value))
                goto found;
            if(MAP_REFINE_FLAG & flag && !script_map_id(block, "refine_location", i, &value))
                goto found;
            if(MAP_ITEM_INFO_FLAG & flag && !script_map_id(block, "item_info", i, &value))
                goto found;

            /* failed to find resolve the id */
            if(!(flag & MAP_NO_ERROR))
                exit_func_safe("failed to map %d (%s) on flag "
                "%d in item %d", i, expr, flag, block->item_id);
            goto failed;
found:
            /* write the mapped value on the block->eng stack */
            if(block_stack_push(block, TYPE_ENG, value))
                goto failed;
            SAFE_FREE(value);
        }
        iter = iter->next;
    }

clean:
    *argc = block->eng_cnt - top;
    SAFE_FREE(value);
    node_free(id);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

/* evaluate the expression into an id
 * and use the id as key for the data
 * base to get the name
 *
 * if the expression is "1101" and flag DB_ITEM_ID then
 *      block->eng[n + 1] = "sword"
 *      *argc = 1;
 */
int stack_eng_db(block_r * block, char * expr, int flag, int * argc) {
    int i = 0;
    int ret = 0;
    int top = 0;
    node_t * id = NULL;
    range_t * iter = NULL;

    skill_t * skill = NULL;
    item_t * item = NULL;
    mob_t * mob = NULL;
    merc_t * merc = NULL;
    pet_t * pet = NULL;
    map_res * map = NULL;

    exit_null_safe(3, block, expr, argc);

    if (0 == flag)
        return CHECK_FAILED;

    top = block->eng_cnt;

    id = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == id)
        return CHECK_FAILED;

    /* iterate the linked list of ranges */
    iter = id->range;
    while(NULL != iter) {
        /* iterate the values within a range */
        for(i = iter->min; i <= iter->max; i++) {
            if(DB_SKILL_ID & flag) {
                skill = calloc(1, sizeof(skill_t));
                if(NULL == skill)
                    goto failed;

                if(skill_id(block->script->db, skill, i) ||
                   block_stack_push(block, TYPE_ENG, skill->desc))
                    goto failed;

                SAFE_FREE(skill);
                continue;
            }
            if(DB_ITEM_ID & flag) {
                item = calloc(1, sizeof(item_t));
                if(NULL == item)
                    goto failed;

                if(item_id(block->script->db, item, i) ||
                   block_stack_push(block, TYPE_ENG, item->name))
                    goto failed;

                SAFE_FREE(item);
                continue;
            }
            if(DB_MOB_ID & flag) {
                mob = calloc(1, sizeof(mob_t));
                if(NULL == mob)
                    goto failed;

                if(mob_id(block->script->db, mob, i) ||
                   block_stack_push(block, TYPE_ENG, mob->name))
                    goto failed;

                SAFE_FREE(mob);
                continue;
            }
            if(DB_MERC_ID & flag) {
                merc = calloc(1, sizeof(merc_t));
                if(NULL == merc)
                    goto failed;

                if(merc_id(block->script->db, merc, i) ||
                   block_stack_push(block, TYPE_ENG, merc->name))
                    goto failed;

                SAFE_FREE(merc);
                continue;
            }
            if(DB_PET_ID & flag) {
                pet = calloc(1, sizeof(pet_t));
                if(NULL == pet)
                    goto failed;

                if(pet_id(block->script->db, pet, i) ||
                   block_stack_push(block, TYPE_ENG, pet->name))
                    goto failed;

                SAFE_FREE(pet);
                continue;
            }
            if(DB_MAP_ID & flag) {
                map = calloc(1, sizeof(map_res));
                if(NULL == map)
                    goto failed;

                if(map_id(block->script->db, map, i) ||
                   block_stack_push(block, TYPE_ENG, map->name))
                    goto failed;

                SAFE_FREE(map);
                continue;
            }

            /* failed to find resolve the id */
            exit_func_safe("failed to map %d (%s) on flag "
            "%d in item %d", i, expr, flag, block->item_id);
            goto failed;
        }
        iter = iter->next;
    }

clean:
    *argc = block->eng_cnt - top;
    SAFE_FREE(skill);
    SAFE_FREE(item);
    SAFE_FREE(mob);
    SAFE_FREE(merc);
    SAFE_FREE(pet);
    SAFE_FREE(map);
    node_free(id);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

int stack_eng_item_group_name(block_r * block, char * expr, int * argc) {
    int ret = 0;
    int top = 0;
    node_t * group_id = NULL;
    item_group_t item_group;
    char buf[64];

    exit_null_safe(3, block, expr, argc);

    top = block->eng_cnt;

    /* group id must evaluate to a integer */
    group_id = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == group_id ||
       group_id->min != group_id->max ||
       item_group_id(block->script->db, &item_group, group_id->min) ||
       0 >= item_group.size)
        goto failed;

    sprintf(buf, "group %d (%d items)", group_id->min, item_group.size);

    if(block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    *argc = block->eng_cnt - top;
    node_free(group_id);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

int stack_eng_trigger_bt(block_r * block, char * expr) {
    int ret = 0;
    int off = 0;
    int val = 0;
    char buf[256];
    node_t * trigger = NULL;

    /* error on invalid references */
    exit_null_safe(2, block, expr);

    trigger = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == trigger)
        goto failed;

    /* trigger must be a constant */
    if(trigger->min != trigger->max)
        goto failed;

    val = trigger->min;

    /* write using the format:
     *  trigger on
     *  [meelee and range]
     *  [weapon and magic or misc]
     *  [normal attacks or skills]
     */

    off += sprintf(&buf[off], "on ");

    /* trigger range (inclusive) */
    if(BF_RANGEMASK & val) {
        if((BF_LONG | BF_SHORT) & val)
            off += sprintf(&buf[off], "meelee and range ");
        else if(BF_LONG & val)
            off += sprintf(&buf[off], "range ");
        else if(BF_SHORT & val)
            off += sprintf(&buf[off], "meelee ");
        else
            /* unsupported bit */
            goto failed;
    } else {
        /* default to meelee and range */
        off += sprintf(&buf[off], "meelee and range ");
    }

    /* trigger type (exclusive?) */
    if(BF_WEAPONMASK & val) {
        if(BF_WEAPON & val)
            off += sprintf(&buf[off], "weapon ");
        else if((BF_MAGIC | BF_MISC) & val)
            off += sprintf(&buf[off], "magic and misc");
        else if(BF_MAGIC & val)
            off += sprintf(&buf[off], "magic ");
        else if(BF_MISC & val)
            off += sprintf(&buf[off], "misc ");
        else
            /* unsupported bit */
            goto failed;
    } else {
        /* default to weapon */
        off += sprintf(&buf[off], "weapon ");
    }

    /* trigger method (exclusive) */
    if(BF_SKILLMASK & val) {
        if(BF_SKILL & val)
            off += sprintf(&buf[off], "skills");
        else if(BF_NORMAL & val)
            off += sprintf(&buf[off], "attack");
        else
            /* unsupported bit */
            goto failed;
    } else {
        /* default depends on trigger type */
        if(BF_MISC & val || BF_MAGIC & val)
            off += sprintf(&buf[off], "skills");
        else if(BF_WEAPON & val)
            off += sprintf(&buf[off], "attack");
    }

    if(stack_aux_formula(block, trigger, buf))
        goto failed;

clean:
    node_free(trigger);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

int stack_eng_trigger_atf(block_r * block, char * expr) {
    int ret = 0;
    int off = 0;
    int val = 0;
    char buf[256];
    node_t * trigger = NULL;

    /* error on invalid references */
    exit_null_safe(2, block, expr);

    trigger = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == trigger) {
        goto failed;
    }

    /* trigger must be a constant */
    if(trigger->min != trigger->max) {
        goto failed;
    }

    val = trigger->min;

    /* write using the format:
     *  trigger on
     *  [meelee and range]
     *  [weapon or magic and misc]
     *  [on self or target ]
     */

    off += sprintf(&buf[off], "on ");

    /* trigger range (inclusive) */
    if((ATF_LONG | ATF_SHORT) & val)
        off += sprintf(&buf[off], "meelee and range ");
    else if(ATF_LONG & val)
        off += sprintf(&buf[off], "range ");
    else if(ATF_SHORT & val)
        off += sprintf(&buf[off], "meelee ");
    else
        /* default range is both */
        off += sprintf(&buf[off], "meelee and range ");

    /* trigger type (exclusive?) */
    if(ATF_WEAPON & val)
        off += sprintf(&buf[off], "weapon ");
    else if((ATF_MAGIC | ATF_MISC) & val)
        off += sprintf(&buf[off], "magic and misc ");
    else if(ATF_MAGIC & val)
        off += sprintf(&buf[off], "magic ");
    else if(ATF_MISC & val)
        off += sprintf(&buf[off], "misc ");
    else
        /* default is weapon */
        off += sprintf(&buf[off], "weapon ");

    /* trigger target */
    if(ATF_SELF & val)
        off += sprintf(&buf[off], "on self");
    else if(ATF_TARGET & val)
        off += sprintf(&buf[off], "on target");
    else
        /* default is target */
        off += sprintf(&buf[off], "on target");

    if(stack_aux_formula(block, trigger, buf))
        goto failed;

clean:
    node_free(trigger);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

int stack_eng_script(block_r * block, char * script) {
    int i = 0;
    int ret = 0;
    int len = 0;
    int cnt = 0;
    char * buf = NULL;

    /* error on invalid reference */
    exit_null_safe(2, block, script);

    /* error on empty script */
    len = strlen(script);
    if(0 >= len)
        return CHECK_FAILED;

    /* error on zero statements */
    for(i = 0; i < len; i++)
        if(';' == script[i])
            cnt++;
    if(0 >= cnt)
        return CHECK_FAILED;

    /* compile and push the script on the stack */
    if(script_recursive(block->script->db, block->script->mode, block->script->map, script, &buf) ||
       block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    SAFE_FREE(buf);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;

}

/* append the formula expression onto the top of block->eng stack
 *
 * printf("%s (%s)", block->eng[block->eng_cnt - 1], node->formula);
 */
int stack_aux_formula(block_r * block, node_t * node, char * buf) {
    char * formula = NULL;

    /* get time expression formula */
    if (block_stack_push(block, TYPE_ENG, buf) ||
        block_stack_formula(block, block->eng_cnt - 1, node, &formula))
        goto failed;

    /* pop results and write final translated string */
    if (block_stack_pop(block, TYPE_ENG) ||
        block_stack_push(block, TYPE_ENG, formula))
        goto failed;

    SAFE_FREE(formula);
    return CHECK_PASSED;

failed:
    SAFE_FREE(formula);
    return CHECK_FAILED;
}

/* translate getitem and delitem formats
 *      getitem <item_id>, <item_amount>, {<account_id>}
 *      delitem <item_id>, <item_amount>, {<account_id>}
 * the total length of all the  strings pushed on
 * to the block->eng stack is return through size
 * which is use to calculate the size of a buffer
 * by the caller.
 */
int translate_id_amount(block_r * block, int * argc, int * size, const char * func) {
    int _argc = 0;
    int _size = 0;

    /* check for null references */
    exit_null_safe(4, block, argc, size, func);

    /* check for function call syntax, i.e. (a, b, c) */
    _argc = block->ptr_cnt;
    if(1 == _argc && '(' == block->ptr[0][0])
       /* parse the argument list */
        if(stack_ptr_call(block, block->ptr[0], &_argc))
            return CHECK_FAILED;
    /* check for item id and amount argument */
    if(2 > _argc)
        return exit_func_safe("%s is missing item id "
        "or amount in item %d", func, block->item_id);

    /* evaluate the item id and amount expression */
    _size = block->arg_cnt;
    if( stack_eng_item(block, block->ptr[block->ptr_cnt - 2], &_argc) ||
        stack_eng_int(block, block->ptr[block->ptr_cnt - 1], 1))
        return CHECK_FAILED;

    /* return the total length and count of
     * strings push to the block->eng stack */
    *size = block->arg_cnt - _size;
    *argc = _argc;
    return CHECK_PASSED;
}

int translate_getitem(block_r * block) {
    int i = 0;
    int ret = 0;
    int off = 0;
    int argc = 0;
    int size = 0;
    char * buf = NULL;

    exit_null_safe(1, block);

    if(translate_id_amount(block, &argc, &size, "getitem"))
        return CHECK_FAILED;

    /* size only account for item name and amount
     * 128 bytes must be added for getitem format */
    buf = calloc(size + 128, sizeof(char));
    if (buf == NULL)
        return CHECK_FAILED;

    off += sprintf(&buf[off], "Add %s %s%s",
        block->eng[block->eng_cnt - 1],
        block->eng[0],
        (argc > 1) ? ", " : "");
    for (i = 1; i < argc; i++)
        off += (i + 1 == argc) ?
            sprintf(&buf[off], "and %s", block->eng[i]) :
            sprintf(&buf[off], "%s, ", block->eng[i]);
    off += sprintf(&buf[off], " into your inventory.");

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_delitem(block_r * block) {
    int i = 0;
    int ret = 0;
    int off = 0;
    int argc = 0;
    int size = 0;
    char * buf = NULL;

    exit_null_safe(1, block);

    if(translate_id_amount(block, &argc, &size, "getitem"))
        return CHECK_FAILED;

    /* size only account for item name and amount
     * 128 bytes must be added for getitem format */
    buf = calloc(size + 128, sizeof(char));
    if (buf == NULL)
        return CHECK_FAILED;

    off += sprintf(&buf[off], "Remove %s %s%s",
        block->eng[block->eng_cnt - 1],
        block->eng[0],
        (argc > 1) ? ", " : "");
    for (i = 1; i < argc; i++)
        off += (i + 1 == argc) ?
            sprintf(&buf[off], "and %s", block->eng[i]) :
            sprintf(&buf[off], "%s, ", block->eng[i]);
    off += sprintf(&buf[off], " from your inventory.");

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_rentitem(block_r * block) {
    int i = 0;
    int ret = 0;
    int off = 0;
    int argc = 0;
    int size = 0;
    char * buf = NULL;

    /* check for null references */
    exit_null_safe(1, block);

    /* check for function call syntax, i.e. (a, b, c) */
    argc = block->ptr_cnt;
    if(1 == argc && '(' == block->ptr[0][0])
       /* parse the argument list */
        if(stack_ptr_call(block, block->ptr[0], &argc))
            return CHECK_FAILED;
    /* check for item id and time argument */
    if(2 > argc)
        return exit_func_safe("rentitem is missing "
        "item id or time in item %d", block->item_id);

    size = block->arg_cnt;
    if( stack_eng_item(block, block->ptr[block->ptr_cnt - 2], &argc) ||
        stack_eng_time(block, block->ptr[block->ptr_cnt - 1], 1))
        return CHECK_FAILED;

    /* return the total length and count of
     * strings push to the block->eng stack */
    size = block->arg_cnt - size;

    /* size only accounts for item names and time
     * 128 bytes must be added for delitem format */
    buf = calloc(size + 128, sizeof(char));
    if (buf == NULL)
        return CHECK_FAILED;

    off += sprintf(&buf[off], "Rent %s %s%s",
        aeiou(block->eng[0][0]) ? "a" : "an",
        block->eng[0],
        (argc > 1) ? ", " : "");
    for (i = 1; i < argc; i++)
        off += (i + 1 == argc) ?
            sprintf(&buf[off], "and %s", block->eng[i]) :
            sprintf(&buf[off], "%s, ", block->eng[i]);
    off += sprintf(&buf[off], " for %s.", block->eng[argc]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return SCRIPT_PASSED;
}

int translate_heal(block_r * block) {
    int i = 0;
    int ret = 0;
    int len = 0;
    int off = 0;
    char * buf = NULL;

    /* error on null references */
    exit_null_safe(1, block);

    /* error on invalid argument count */
    if(2 > block->ptr_cnt)
        return exit_func_safe("missing hp or sp argument for %s in item %d", block->name, block->item_id);

    len = block->arg_cnt;
    stack_eng_int_signed(block, block->ptr[0], 1, "Recover HP by", "Drain HP by");
    stack_eng_int_signed(block, block->ptr[1], 1, "Recover SP by", "Drain SP by");
    if(block->eng_cnt == 0)
        return CHECK_FAILED;

    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    for(i = 0; i < block->eng_cnt; i++)
        off += sprintf(&buf[off], "%s.", block->eng[i]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_produce(block_r * block, int handler) {
    int i = 0;
    int ret = 0;
    int len = 0;
    int off = 0;
    char * buf = NULL;
    int arg_off = 0;
    int arg_cnt = 0;

    /* error on invalid references */
    exit_null_safe(1, block);

    /* error on invalid argument */
    if(1 > block->ptr_cnt)
        return exit_func_safe("missing item level argument"
        " for %s in item %d", block->name, block->item_id);

    /* get total number of items pushed onto block->eng
     * stack and the length bytes written to the buffer */
    len = block->arg_cnt;
    arg_off = block->eng_cnt;
    if(stack_eng_produce(block, block->ptr[0], &arg_cnt))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    /* write the produce recipes */
    for(i = arg_off; i < arg_cnt; i++)
        off += sprintf(&buf[off], "%s\n", block->eng[i]);

    for(i = arg_off; i < arg_cnt; i++)
        block_stack_pop(block, TYPE_ENG);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf)) {
        SAFE_FREE(buf);
        return CHECK_FAILED;
    }

    SAFE_FREE(buf);
    return CHECK_PASSED;
}

int translate_status(block_r * block) {
    int i = 0;
    int ret = 0;
    int len = 0;
    int off = 0;
    int argc = 0;
    char * buf = NULL;
    node_t * effect = NULL;
    status_res status;

    if(block->ptr_cnt < 3)
        return exit_func_safe("missing effect id, time, or values "
        "argument for %s in item %d", block->name, block->item_id);

    /* evaluate the status constant */
    effect = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == effect)
        return CHECK_FAILED;

    /* error on invalid constant
       multiple values supported */
    if(effect->min != effect->max)
        goto failed;

    len = block->arg_cnt;
    if(status_id(block->script->db, &status, effect->min)) {
        printf("unimplemented status %d\n", effect->min);
        goto failed;
    }

    /* translate the tick */
    if (stack_eng_time(block, block->ptr[1], 1))
        goto failed;

    /* handle special case for sc_itemscript */
    if(status.scid == 289) {
        /* get item script from item id */
        /* compile the item script recursively */
        exit_abt_safe("not implemented");
        goto failed;
    } else {
        /* translate the extra arguments */
        for(int i = 0; i < status.vcnt; i++) {
            /* status argument types are different from bonus argument types */
            switch(status.vmod[i]) {
                case 'n': ret = stack_eng_int(block, block->ptr[2 + i], 1);                         break;    /* integer */
                case 'm': ret = stack_eng_int(block, block->ptr[2 + i], 1);                         break;    /* skill level */
                case 'p': ret = stack_eng_int(block, block->ptr[2 + i], 1);                         break;    /* integer percentage */
                case 'e': ret = stack_eng_map(block, block->ptr[2 + i], MAP_EFFECT_FLAG, &argc);    break;    /* effect */
                case 'u': ret = stack_eng_int(block, block->ptr[2 + i], -1);                        break;    /* regen */
                default:
                    exit_func_safe("unsupported status argment type "
                    "%c in item %d", status.vmod[i], block->item_id);
                    goto failed;
            }
            if(ret)
                goto failed;
        }
    }
    len = (block->arg_cnt - len);
    len += strlen(status.scfmt) + 128;

    /* write the format and duration */
    buf = calloc(len + 128, sizeof(char));
    if(NULL == buf)
        goto failed;

    switch(status.vcnt) {
        case 0: off += sprintf(&buf[off], status.scfmt); break;
        case 1: off += sprintf(&buf[off], status.scfmt, block->eng[status.voff[0]]); break;
        default:
            exit_func_safe("unsupport status argument coun"
            "t %d in item %d", status.vcnt, block->item_id);
    }

    /* write the duration */
    off += sprintf(&buf[off], " for %s.", block->eng[0]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    SAFE_FREE(buf);
    node_free(effect);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

int translate_status_end(block_r * block) {
    int ret = 0;
    int len = 0;
    char * buf = NULL;
    node_t * effect = NULL;
    status_res status;

    if(block->ptr_cnt < 1)
        return exit_func_safe("missing effect id argument"
        " for %s in item %d", block->name, block->item_id);

    effect = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == effect)
        return CHECK_FAILED;

    /* error on invalid constant;
     * multiple values supported */
    if(effect->min != effect->max)
        goto failed;

    if(status_id(block->script->db, &status, effect->min))
        goto failed;

    len = strlen(status.scend);
    if(0 >= len)
        goto failed;

    buf = calloc(len + 32, sizeof(char));
    if(NULL == buf)
        goto failed;

    sprintf(buf, "Cures %s.", status.scend);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    SAFE_FREE(buf);
    node_free(effect);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

int translate_pet_egg(block_r * block) {
    int i = 0;
    int ret = 0;
    int off = 0;
    int len = 0;
    node_t * id = 0;
    pet_t pet;
    char * buf = NULL;

    /* error on invalid references */
    exit_null_safe(1, block);

    /* error on invalid argument count */
    if(block->ptr_cnt < 1)
        return exit_func_safe("missing pet id argument "
        "for %s in item %d", block->name, block->item_id);

    /* evaluate for pet id */
    id = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == id)
        goto failed;

    /* error on invalid constant;
     * multiple values supported */
    if(id->min != id->max)
        goto failed;

    /* search for the pet in the database and
     * translate both script on block->eng stack */
    len = block->arg_cnt;
    if(pet_id(block->script->db, &pet, id->min) ||
       stack_eng_script(block, pet.pet_script) ||
       stack_eng_script(block, pet.loyal_script))
        goto failed;
    len = block->arg_cnt - len;

    buf = calloc(len + 128, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    off += sprintf(&buf[off], "Egg containing %s.\n", pet.name);
    if(NULL != block->eng[0])
        off += sprintf(&buf[off], "\n[Normal Bonus]\n%s", block->eng[0]);
    if(NULL != block->eng[1])
        off += sprintf(&buf[off], "\n[Loyal Bonus]\n%s", block->eng[1]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    SAFE_FREE(buf);
    node_free(id);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;

}

int translate_bonus(block_r * block, char * prefix) {
    int i = 0;
    int j = 0;
    int ret = 0;
    int cnt = 0;
    int len = 0;
    int blen = 0;
    int plen = 0;
    char * buf = NULL;
    bonus_res * bonus = NULL;

    /* error on invalid references
     * or error on empty arguments */
    exit_null_safe(2, block, prefix);

    if (1 > block->ptr_cnt)
        return CHECK_FAILED;

    /* error on empty string */
    blen = strlen(block->ptr[0]);
    plen = strlen(prefix);
    if(0 >= blen || 0 >= plen)
        return SCRIPT_FAILED;

    /* search for bonus information */
    bonus = calloc(1, sizeof(bonus_res));
    if(NULL == bonus)
        return SCRIPT_FAILED;

    if(bonus_name(block->script->db, bonus, prefix, plen, block->ptr[0], blen)) {
        exit_func_safe("unsupported %s %s for item "
        "%d", prefix, block->ptr[0], block->item_id);
        goto failed;
    }

    len = block->arg_cnt;

    /* translate each bonus argument by argument type */
    for(i = 0, j = 1; i < bonus->type_cnt; i++, j++) {

        /* push the argument on the block->eng stack */
        switch(bonus->type[i]) {
            case 'n': ret = stack_eng_int(block, block->ptr[j], 1);                                     break; /* Integer Value */
            case 'p': ret = stack_eng_int(block, block->ptr[j], 1);                                     break; /* Integer Percentage */
            case 'r': ret = stack_eng_map(block, block->ptr[j], MAP_RACE_FLAG, &cnt);                   break; /* Race */
            case 'l': ret = stack_eng_map(block, block->ptr[j], MAP_ELEMENT_FLAG, &cnt);                break; /* Element */
            case 'w': ret = stack_eng_grid(block, block->ptr[j]);                                       break; /* Splash */
            case 'z':                                                                                   break; /* Meaningless */
            case 'e': ret = stack_eng_map(block, block->ptr[j], MAP_EFFECT_FLAG, &cnt);                 break; /* Effect */
            case 'q': ret = stack_eng_int(block, block->ptr[j], 100);                                   break; /* Integer Percentage / 100 */
            case 'k': ret = stack_eng_skill(block, block->ptr[j], &cnt);                                break; /* Skill */
            case 's': ret = stack_eng_map(block, block->ptr[j], MAP_SIZE_FLAG, &cnt);                   break; /* Size */
            case 'c': ret = stack_eng_db(block, block->ptr[j], DB_MOB_ID, &cnt);                        break; /* Monster Class & Job ID * Monster ID */
            case 'o': ret = stack_eng_int(block, block->ptr[j], 10);                                    break; /* Integer Percentage / 10 */
            case 'm': ret = stack_eng_db(block, block->ptr[j], DB_ITEM_ID, &cnt);                       break; /* Item ID */
            case 'x': ret = stack_eng_int(block, block->ptr[j], 1);                                     break; /* Level */
            case 'g': ret = stack_eng_map(block, block->ptr[j], MAP_REGEN_FLAG, &cnt);                  break; /* Regen */
            case 'a': ret = stack_eng_int(block, block->ptr[j], 1000);                                  break; /* Millisecond */
            case 'h': ret = stack_eng_int(block, block->ptr[j], 1);                                     break; /* SP Gain Bool */
            case 'v': ret = stack_eng_map(block, block->ptr[j], MAP_CAST_FLAG, &cnt);                   break; /* Cast Self, Enemy */
            case 't': ret = stack_eng_trigger_bt(block, block->ptr[j]);                                 break; /* Trigger BT */
            case 'y': ret = (stack_eng_item_group_name(block, block->ptr[j], &cnt)
                            && stack_eng_db(block, block->ptr[j], DB_ITEM_ID, &cnt));                   break; /* Item Group */
            case 'd': ret = stack_eng_trigger_atf(block, block->ptr[j]);                                break; /* Triger ATF */
            case 'f': ret = stack_eng_int(block, block->ptr[j], 1);                                     break; /* Cell */
            case 'b': ret = stack_eng_map(block, block->ptr[j], MAP_TARGET_FLAG, &cnt);                 break; /* Flag Bitfield */
            case 'i': ret = stack_eng_map(block, block->ptr[j], MAP_WEAPON_FLAG, &cnt);                 break; /* Weapon Type */
            case 'j': ret = (stack_eng_map(block, block->ptr[j], MAP_CLASS_FLAG | MAP_NO_ERROR, &cnt)
                             && stack_eng_db(block, block->ptr[j], DB_MOB_ID, &cnt));                  break; /* Class Group & Monster */
            default: break;
        }

        /* failed to push values onto the stack */
        if(ret)
            goto failed;

        /* stack_eng_map and stack_eng_db may push
         * multiple values on the  stack, which is
         * not supported */
        if(cnt > 1)
            goto failed;
    }

    /* write the bonus format */
    len = (block->arg_cnt - len) + strlen(bonus->format) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        goto failed;

    switch(bonus->type_cnt) {
        case 1:
            sprintf(buf,
                bonus->format,
                block->eng[bonus->order[0]]);
            break;
        case 2:
            sprintf(buf,
                bonus->format,
                block->eng[bonus->order[0]],
                block->eng[bonus->order[1]]);
            break;
        case 3:
            sprintf(buf,
                bonus->format,
                block->eng[bonus->order[0]],
                block->eng[bonus->order[1]],
                block->eng[bonus->order[2]]);
            break;
        case 4:
            sprintf(buf,
                bonus->format,
                block->eng[bonus->order[0]],
                block->eng[bonus->order[1]],
                block->eng[bonus->order[2]],
                block->eng[bonus->order[3]]);
            break;
        case 5:
            sprintf(buf,
                bonus->format,
                block->eng[bonus->order[0]],
                block->eng[bonus->order[1]],
                block->eng[bonus->order[2]],
                block->eng[bonus->order[3]],
                block->eng[bonus->order[4]]);
            break;
        default:
            return exit_func_safe("unsupport bonus argument count"
            " %d on %s in item %d", bonus->type_cnt, bonus->bonus,
            block->item_id);
    }

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    SAFE_FREE(buf);
    SAFE_FREE(bonus);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int translate_skill(block_r * block) {
    int ret = 0;
    int len = 0;
    int cnt = 0;
    int temp = 0;
    char * buf = NULL;
    node_t * flag = NULL;

    /* error on invalid references
     * and error on empty argument */
    exit_null_safe(1, block);

    if(block->ptr_cnt < 2)
        return CHECK_FAILED;

    /* translate skill name and level */
    len = block->arg_cnt;
    if(stack_eng_skill(block, block->ptr[0], &cnt) ||
       cnt > 1 || /* skill id must be constant */
       stack_eng_int(block, block->ptr[1], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    /* translate skill flag */
    if(block->ptr_cnt == 3) {
        flag = evaluate_expression(block, block->ptr[2], 1, EVALUATE_FLAG_KEEP_NODE);
        if(NULL == flag)
            return CHECK_FAILED;

        /* flag must be a constant */
        if(flag->min != flag->max)
            goto failed;

        temp = flag->min;
    } else {
        /* default is 1 */
        temp = 1;
    }

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        goto failed;

    switch(temp) {
        case 0:
            sprintf(buf, "Enable skill %s level %s.", block->eng[0], block->eng[1]);
            break;
        case 1:
            sprintf(buf, "Temporarily enable skill %s level %s.", block->eng[0], block->eng[1]);
            break;
        case 2:
            sprintf(buf, "Enable skill %s level %s.\nOr add %s levels "
            "to the skill.", block->eng[0], block->eng[1], block->eng[1]);
            break;
        default:
            break;
    }

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    node_free(flag);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int translate_itemskill(block_r * block) {
    int i = 0;
    int ret = 0;
    int cnt = 0;
    int off = 0;
    int len = 0;
    char * buf = 0;

    /* error on invalid reference and empty argument */
    exit_null_safe(1, block);

    if(block->ptr_cnt < 2)
        return CHECK_FAILED;

    /* get skill name and level */
    len = block->arg_cnt;
    if(stack_eng_skill(block, block->ptr[0], &cnt) ||
       cnt > 1 || /* skill id must be a constant */
       stack_eng_int(block, block->ptr[1], 1))
        goto failed;
    len = (block->arg_cnt - len) + 32;

    /* write the item skill */
    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        goto failed;

    sprintf(buf, "Cast %s level %s.", block->eng[0], block->eng[1]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        goto failed;

clean:
    SAFE_FREE(buf);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int translate_petloot(block_r * block) {
    int ret = 0;
    int len = 0;
    char * buf = NULL;

    /* error on invalid reference and empty argument */
    exit_null_safe(1, block);

    if(block->ptr_cnt < 1)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if(stack_eng_int(block, block->ptr[0], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "Pet can loot and hold upto %s items.\n"
    "Use pet performance to transfer items to inventory.",
    block->eng[0]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_petheal(block_r * block) {
    int ret = 0;
    int len = 0;
    int off = 0;
    char * buf = NULL;

    /* error on invalid reference and empty argument */
    exit_null_safe(1, block);

    if(block->ptr_cnt < 4)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if( stack_eng_int(block, block->ptr[0], 1) ||
        stack_eng_int(block, block->ptr[1], 1) ||
        stack_eng_int(block, block->ptr[2], 1) ||
        stack_eng_int(block, block->ptr[3], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "[HP < %s and SP < %s]\nCast Heal [Lv. %s] for every %s "
    "seconds.", block->eng[2], block->eng[3], block->eng[0], block->eng[1]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_petrecovery(block_r * block) {
    int ret = 0;
    int len = 0;
    char * buf = NULL;

    exit_null_safe(1, block);

    if(block->ptr_cnt < 2)
        return CHECK_FAILED;

    len  = block->arg_cnt;
    if(translate_status_end(block) ||
       stack_eng_int(block, block->ptr[0], 1))
        return CHECK_FAILED;
    len  = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    len = strlen(block->eng[0]);
    block->eng[0][len - 1] = '\0';

    sprintf(buf, "%s every %s seconds.", block->eng[0], block->eng[1]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_petskillbonus(block_r * block) {
    int ret = 0;
    int len = 0;
    char * buf = NULL;

    /* error on invalid reference and empty argument */
    exit_null_safe(1, block);

    if(block->ptr_cnt < 4)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if( translate_bonus(block, "bonus") ||
        stack_eng_int(block, block->ptr[2], 1) ||
        stack_eng_int(block, block->ptr[3], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "Pet skill bonus is activated for %s seconds with a del"
    "ay of %s seconds.\n%s", block->eng[1], block->eng[2], block->eng[0]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_petskillattack(block_r * block) {
    int ret = 0;
    int len = 0;
    int off = 0;
    int argc = 0;
    char * buf = NULL;

    /* error on invalid reference and empty argument */
    exit_null_safe(1, block);

    if(block->ptr_cnt < 4)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if( stack_eng_skill(block, block->ptr[0], &argc) || /* skill name */
        stack_eng_int(block, block->ptr[1], 1) ||       /* skill level */
        stack_eng_int(block, block->ptr[2], 1) ||       /* normal rate */
        stack_eng_int(block, block->ptr[3], 1))         /* loyalty rate */
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "Add a %s (%s on loyalty) chance of casting %s level"
    " %s.", block->eng[2], block->eng[3], block->eng[0], block->eng[1]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_petskillattack2(block_r * block) {
    int ret = 0;
    int len = 0;
    int off = 0;
    int argc = 0;
    char * buf = NULL;

    /* error on invalid reference and empty argument */
    exit_null_safe(1, block);
    if(block->ptr_cnt < 5)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if( stack_eng_skill(block, block->ptr[0], &argc) || /* skill name */
        stack_eng_int(block, block->ptr[1], 1) ||       /* damage */
        stack_eng_int(block, block->ptr[2], 1) ||       /* number of attacks */
        stack_eng_int(block, block->ptr[3], 1) ||       /* normal rate */
        stack_eng_int(block, block->ptr[4], 1))         /* loyalty rate */
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "Add a %s (%s on loyalty) chance of casting %s for %s damage x %s hi"
    "ts.", block->eng[3], block->eng[4], block->eng[0], block->eng[1], block->eng[2]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return CHECK_PASSED;
}

int translate_petskillsupport(block_r * block) {
    int ret = 0;
    int len = 0;
    int cnt = 0;
    int off = 0;
    char * buf = NULL;

    /* error on invalid reference and empty argument */
    exit_null_safe(1, block);

    if(block->ptr_cnt < 4)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if( stack_eng_skill(block, block->ptr[0], &cnt) ||
        stack_eng_int(block, block->ptr[1], 1) ||
        stack_eng_int(block, block->ptr[2], 1) ||
        stack_eng_int(block, block->ptr[3], 1) ||
        stack_eng_int(block, block->ptr[4], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "[HP < %s and SP < %s]\nCast %s [Lv. %s] for every %s seconds.",
    block->eng[3], block->eng[4], block->eng[0], block->eng[1], block->eng[2]);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_getexp(block_r * block, int handler) {
    int ret = 0;
    int len = 0;
    char * buf = NULL;

    exit_null_safe(1, block);

    if(block->ptr_cnt < 1)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if(stack_eng_int(block, block->ptr[0], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 32;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "Gain %s %s experience.", block->eng[0], (handler == 43) ? "base" : "guild");

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_autobonus(block_r * block, int flag) {
    int ret = 0;
    int len = 0;
    int off = 0;
    char * buf = NULL;
    char * script = NULL;

    exit_null_safe(1, block);

    if(block->ptr_cnt < 3)
        return CHECK_FAILED;

    len = block->arg_cnt;

    if (script_recursive(block->script->db, block->script->mode, block->script->map, block->ptr[0], &script)) {
        script = convert_string("No bonus script.");
        block_stack_push(block, TYPE_ENG, script);
    }

    if(stack_eng_int(block, block->ptr[1], 10) ||
       stack_eng_time(block, block->ptr[2], 1))
        return CHECK_FAILED;


    if(block->ptr_cnt > 3) {
       if(stack_eng_trigger_bt(block, block->ptr[3]))
            return CHECK_FAILED;
    } else {
        /* evaluates to default bf flags */
        if(stack_eng_trigger_bt(block, "0"))
            return CHECK_FAILED;
    }

    len = (block->arg_cnt - len) + strlen(script) + 256;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "Add %s chance to activate %s for %s.\n%s", block->eng[0], block->eng[2], block->eng[1], script);

    if(block_stack_reset(block, TYPE_ENG) ||
       block_stack_push(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    SAFE_FREE(script);
    return ret;
}

int translate_searchstore(block_r * block) {

    /* check for null references */
    exit_null_safe(1, block);

    /* check empty block->ptr stack */
    if(1 > block->ptr_cnt)
        return exit_func_safe("searchstore is missing "
        "amount or effect in item %d", block->item_id);


    return SCRIPT_FAILED;
}

int translate_buyingstore(block_r * block) {
    return SCRIPT_FAILED;
}

/* implemented only for rathena and hercules
 * but the implementations are different */
int translate_getrandgroup(block_r * block, int handler) {
    /*int offset = 0;
    char buf[BUF_SIZE];
    node_t * group_id = NULL;
    node_t * group_amount = NULL;

    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;

    if(handler == 20) {
        if(block->ptr_cnt == 1) {
            if(script_extend_paramater(block, block->ptr[0]) ||
               translate_const(block, block->ptr[1], 0x20))
                return SCRIPT_FAILED;
            group_id = evaluate_expression(block, block->ptr[2], 1, EVALUATE_FLAG_KEEP_NODE);
            if(group_id == NULL) return SCRIPT_FAILED;
            offset = sprintf(buf,"Collect %s item%s from %s.", block->eng[1],
                (convert_integer(block->eng[1], 10) > 1)?"s":"", block->eng[0]);
        } else if(block->ptr_cnt == 2) {
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
                (convert_integer(block->eng[1], 10) > 1)?"s":"", block->eng[2]);
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
    if(group_amount != NULL) node_free(group_amount);*/
    return exit_func_safe("maintenance");
}

int translate_bstore(block_r * block, int handler) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * slots = NULL;
    /* check null paramater */
    exit_null_safe(1, block);

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
    /*int offset = 0;
    char buf[BUF_SIZE];
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(translate_id(block, block->ptr[0], 0x04) ||
        stack_eng_time(block, block->ptr[1], 1))
        return SCRIPT_FAILED;
    offset = sprintf(buf,"Hire %s for %s.", block->eng[0], block->eng[2]);
    buf[offset] = '\0';
    translate_write(block, buf, 1);*/
    return exit_abt_safe("maintenance");
}

int translate_transform(block_r * block) {
    /*int offset = 0;
    char buf[BUF_SIZE];
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    if(isdigit(block->ptr[0][0])) {
        if(translate_id(block, block->ptr[0], 0x01) ||
            stack_eng_time(block, block->ptr[1], 1))
            return SCRIPT_FAILED;
        offset = sprintf(buf, "Transform into a %s for %s.", block->eng[0], block->eng[2]);
    } else {
        if(translate_write(block, block->ptr[0], 1) ||
            stack_eng_time(block, block->ptr[1], 1))
            return SCRIPT_FAILED;
        offset = sprintf(buf, "Transform into a %s for %s.", block->eng[0], block->eng[2]);
    }
    buf[offset] = '\0';
    translate_write(block, buf, 1);*/
    return exit_abt_safe("maintenance");
}

int translate_skill_block(block_r * block, int handler) {
    /*char buf[BUF_SIZE];
    char aux[BUF_SIZE];
    node_t * level = NULL;
    int offset = 1;
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    offset = (handler == 13)?1:0;
    if(translate_skill(block, block->ptr[offset]))
        return SCRIPT_FAILED;
    level = evaluate_expression(block, block->ptr[offset+1], 1,
        EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA);
    if(level == NULL) return SCRIPT_FAILED;
    offset += (handler == 11) ?
        sprintf(buf,"Enable %s [Lv. %s]",block->eng[0], formula(aux, block->eng[1], level)):
        sprintf(buf,"Cast %s [Lv. %s]",block->eng[0], formula(aux, block->eng[1], level));
    buf[offset] = '\0';
    translate_write(block, buf, 1);
    if(level != NULL) node_free(level);*/
    return exit_abt_safe("maintenance");
}

int translate_misc(block_r * block, char * expr) {
    int type;
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * result = NULL;
    /* check null paramater */
    exit_null_safe(2, block, expr);

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

int translate_bonus_script(block_r * block) {
    int offset = 0;
    char buf[BUF_SIZE];
    node_t * duration = NULL;
    node_t * type = NULL;
    node_t * flag = NULL;
    /* check null paramater */
    exit_null_safe(1, block);

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
    /*int ret = 0;
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
    if(result != NULL) node_free(result);*/
    return exit_abt_safe("maintenance");
}

int translate_write(block_r * block, char * desc, int flag) {
    return block_stack_push(block, TYPE_ENG, desc);
}

int translate_overwrite(block_r * block, char * desc, int position) {
    int length = 0;

    exit_null_safe(2, block, desc);

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

/* lowest level function for evaluating expressions
 * stack_eng_* and evaluate_function_* are both one
 * level above */
node_t * evaluate_expression(block_r * block, char * expr, int modifier, int flag) {
    int i = 0;
    node_t * result = NULL;
    token_r tokens;

    /* check null */
    if (NULL == block || expr == NULL)
        return NULL;

    /* check division by zero */
    if (modifier == 0) {
        exit_func_safe("modifier is zero in item %d", block->item_id);
        return NULL;
    }

    /* tokenize the expression */
    if (script_lexical(&tokens, expr) || tokens.script_cnt <= 0) {
        exit_func_safe("failed to tokenize '%s' in item %d", expr, block->item_id);
        return NULL;
    }

    /* evaluate expression */
    result = evaluate_expression_recursive(block, tokens.script_ptr, 0, tokens.script_cnt, block->logic_tree, flag);
    if (result == NULL) {
        exit_func_safe("failed to evaluate '%s' in item %d", expr, block->item_id);
        return NULL;
    }

    /* post expression handling of node */
    return evaluate_expression_(block, result, modifier, flag);
}

/* handle specific flags before returning to the caller */
node_t * evaluate_expression_(block_r * block, node_t * root_node, int modifier, int flag) {
    int min = 0;
    int max = 0;
    int len = 0;
    char buf[64];
    char * formula = NULL;
    logic_node_t * temp = NULL;

    /* check division by zero */
    if (modifier == 0) {
        exit_func_safe("modifier is zero in item %d", block->item_id);
        return NULL;
    }

    /* get the min and max value */
    min = root_node->min;
    max = root_node->max;

    /* check whether min and max can be divided by the modifier */
    if (min == max) {
        /* write a single value */
        len = (min > 0 && (min / modifier) == 0) ?
            sprintf(buf, "%.2f", (double) min / modifier) :
            sprintf(buf, "%d", min / modifier);
    } else {
        /* write multiple values */
        len = (min > 0 && (min / modifier) == 0 || max > 0 && (max / modifier) == 0) ?
            sprintf(buf, "%.2f ~ %.2f", (double) min / modifier, (double) max / modifier) :
            sprintf(buf, "%d ~ %d", min / modifier, max / modifier);
    }

    /* write formula if dependencies exist and only when flag is set */
    if (EVALUATE_FLAG_WRITE_FORMULA & flag && root_node->cond_cnt > 0) {
        if(block_stack_push(block, TYPE_ENG, buf) ||
           block_stack_formula(block, block->eng_cnt - 1, root_node, &formula) ||
           block_stack_pop(block, TYPE_ENG) ||
           block_stack_push(block, TYPE_ENG, formula))
            goto failed;
        SAFE_FREE(formula);
    } else if(EVALUATE_FLAG_WRITE_STACK & flag) {
        if(block_stack_push(block, TYPE_ENG, buf))
            goto failed;
    }

    /* keep logic tree in node */
    if (EVALUATE_FLAG_KEEP_LOGIC_TREE & flag)
        if (root_node->cond != NULL) {
            if (block->logic_tree == NULL) {
                block->logic_tree = copy_deep_any_tree(root_node->cond);
            } else {
                /* new logic trees are stack onot the previous logic tree */
                temp = block->logic_tree;
                block->logic_tree = copy_deep_any_tree(root_node->cond);
                block->logic_tree->stack = temp;
            }
        }

    /* return the root node */
    if (EVALUATE_FLAG_KEEP_NODE & flag)
        return root_node;

    node_free(root_node);
    return NULL;

failed:
    SAFE_FREE(formula);
    return NULL;
}

node_t * evaluate_expression_recursive(block_r * block, char ** expr, int start, int end, logic_node_t * logic_tree, int flag) {
    int  i = 0;
    char c = 0;
    int operand = 0;

    /* linked list builder */
    node_t * root_node = NULL;
    node_t * iter_node = NULL;
    node_t * temp_node = NULL;

    /* initialize the root node */
    iter_node = root_node = calloc(1, sizeof(node_t));

    /* circularly link the free and node list */
    root_node->list = root_node;
    root_node->next = root_node;
    root_node->prev = root_node;

    for(i = start; i < end; i++) {
        if(expr[i][0] == '(') {
            /* create subexpression node */
            if(evaluate_expression_sub(block, expr, &i, end, logic_tree, flag, &temp_node))
                goto failed;
            operand++;
        } else if(SCRIPT_BINARY(expr[i][0])) {
            /* create single or dual operator node */
            temp_node = calloc(1, sizeof(node_t));
            if(NULL == temp_node)
                goto failed;

            /* set the operator symbol and type */
            temp_node->op = expr[i][0];
            temp_node->type = (operand) ? NODE_TYPE_OPERATOR : NODE_TYPE_UNARY;

            /* check for dual operators */
            if(NODE_TYPE_OPERATOR == temp_node->type) {
                /* peek ahead for dual operator */
                if(i + 1 < end) {
                    c = expr[i + 1][0];
                    if(SCRIPT_BINARY(c) && !SCRIPT_UNARY(c)) {
                        /* consume the operator */
                        temp_node->op += expr[i + 1][0];
                        i++;
                    }
                }
                /* reset the operand count */
                operand = 0;
            }
        } else if(isdigit(expr[i][0])) {
            /* create numeric constant */
            temp_node = calloc(1, sizeof(node_t));
            if(NULL == temp_node)
                goto failed;

            temp_node->type = NODE_TYPE_OPERAND;
            temp_node->min = convert_integer(expr[i], 10);
            temp_node->max = temp_node->min;
            operand++;
        } else if(SCRIPT_STRING(expr[i][0]) || SCRIPT_SYMBOL(expr[i][0])) {
            /* create variables, functions, and identifiers */
            if(evaluate_expression_var(block, expr, &i, end, logic_tree, flag, &temp_node))
                goto failed;
            operand++;
        } else {
            /* something has gone wrong during processing */
            exit_func_safe("invalid token '%s' in item %d", expr[i], block->item_id);
            goto failed;
        }

        /* circular linked the free list before checking errors
         * otherwise failed will attempt to deference a null */
        iter_node->list = temp_node;
        temp_node->list = root_node;

        /* doubly link the nodes */
        iter_node->next = temp_node;
        temp_node->prev = iter_node;
        root_node->prev = temp_node;
        temp_node->next = root_node;

        /* error on operator without operand */
        if(operand > 1) {
            exit_func_safe("operand '%s' without operator in item %d", expr[i], block->item_id);
            goto failed;
        }

        /* error on invalid node */
        if(NULL == temp_node) {
            exit_func_safe("invalid node in item %d", block->item_id);
            goto failed;
        }

        /* create range if no range */
        if(0x3E & temp_node->type &&
           NULL == temp_node->range)
            temp_node->range = mkrange(INIT_OPERATOR, temp_node->min, temp_node->max, DONT_CARE);

        /* iterator set to the tail */
        iter_node = temp_node;

        /* reset the temp_node reference */
        temp_node = NULL;
    }

    /* error on empty node list */
    if(root_node == iter_node)
        goto failed;

    /* the actual result is in root_node->next so we need
     * to copy any value we want to return using the root
     * node */
    if(node_structure(root_node) ||
       node_evaluate(root_node->next, node_dbg, logic_tree, flag) ||
       node_steal(root_node->next, root_node)) {
        exit_func_safe("failed to evaluate node tree in item %d", block->item_id);
        goto failed;
    }

    /* free every node except the root */
    iter_node = root_node->list;
    while(iter_node != root_node) {
        temp_node = iter_node;
        iter_node = iter_node->list;
        node_free(temp_node);
    }

    return root_node;

failed:
    /* free every node */
    iter_node = root_node->list;
    while(iter_node != root_node) {
        temp_node = iter_node;
        iter_node = iter_node->list;
        node_free(temp_node);
    }
    node_free(root_node);
    return NULL;
}

int evaluate_expression_sub(block_r * block, char ** expr, int * start, int end, logic_node_t * logic_tree, int flag, node_t ** node) {
    int  i = 0;
    char c = 0;
    int  level = 0;
    node_t * _node = NULL;

    /* find the ending parenthesis */
    for(i = *start; i < end; i++) {
        c = expr[i][0];
        if('(' == c) {
            level++;
        } else if(')' == c) {
            level--;

            /* ending parenthesis founded */
            if(0 == level)
                break;
        }
    }

    /* error on missing parenthesis */
    if(0 < level || i >= end)
        return exit_func_safe("missing parenthesis in item %d", block->item_id);

    /* error on empty subexpression */
    if(1 == (i - *start))
        return exit_func_safe("empty subexpression in item %d", block->item_id);

    /* peek ahead to check if subexpression is part of ? operator */
    _node = (i + 1 < end && expr[i + 1][0] == '?') ?
        evaluate_expression_recursive(block, expr, *start + 1, i, logic_tree, flag | EVALUATE_FLAG_EXPR_BOOL):
        evaluate_expression_recursive(block, expr, *start + 1, i, logic_tree, flag);
    if(NULL == _node)
        return exit_func_safe("failed to evaluate subexpression in item %d", block->item_id);

    /* set node type is subexpression */
    _node->type = NODE_TYPE_SUB;

    *node = _node;
    *start = i;
    return CHECK_PASSED;
}

int evaluate_expression_var(block_r * block, char ** expr, int * start, int end, logic_node_t * logic_tree, int flag, node_t ** node) {
    int  i = 0;
    char c = 0;
    int  level = 0;
    int len = 0;
    node_t * _node = NULL;  /* node */
    db_t * db = NULL;       /* resource or athena database */
    var_res var;            /* variable or function search */
    map_res map;            /* mapping search */
    const_t constant;       /* constant search */
    block_r * set = NULL;   /* search variable */
    node_t * __node = NULL; /* set node */

    /* error on empty identifer */
    len = strlen(expr[*start]);
    if(0 >= len)
        return CHECK_FAILED;

    /* create the variable and function node */
    _node = calloc(1, sizeof(node_t));
    if(NULL == _node)
        return CHECK_FAILED;

    /* search the database */
    db = block->script->db;

    if(!var_name(db, &var, expr[*start], len)) {
        /* copy the variable id */
        _node->var = var.id;
        _node->cond_cnt = 1;

        /* copy the variable or function name */
        _node->id = convert_string(var.desc);
        if(NULL == _node->id)
            goto failed;

        if(var.flag & VARI_FLAG) {
            _node->type = NODE_TYPE_VARIABLE;
            _node->range = mkrange(INIT_OPERATOR, var.min, var.max, DONT_CARE);
            _node->min = minrange(_node->range);
            _node->max = minrange(_node->range);
        } else if(var.flag & FUNC_FLAG) {
            _node->type = NODE_TYPE_FUNCTION;

            /* find the ending parenthesis */
            for(i = *start; i < end; i++) {
                c = expr[i][0];
                if('(' == c) {
                    level++;
                } else if(')' == c) {
                    level--;

                    /* ending parenthesis founded */
                    if(0 == level)
                        break;
                }
            }

            /* error on missing parenthesis */
            if(0 < level || i >= end) {
                exit_func_safe("missing parenthesis in item %d", block->item_id);
                goto failed;
            }

            /* set the functions return type */
            _node->return_type |= var.type;

            if(var.flag & FUNC_CONST_FLAG) {
                /* function uses a static min, max, and desc */
                _node->formula = convert_string(var.desc);
                _node->min = var.min;
                _node->max = var.max;
            } else {
                /* evaluate the function */
                if(evaluate_function(block, expr, *start + 1, i, &var, _node))
                    goto failed;
            }

            /* skip the function's argument list */
            *start = i;
        } else {
            goto failed;
        }
    } else {
        /* copied the identifier */
        _node->id = convert_string(expr[*start]);
        if(NULL == _node->id)
            goto failed;

        if(!map_name(db, &map, expr[*start], len)) {
            /* map name */
            _node->type = NODE_TYPE_CONSTANT;
            _node->min = map.id;
            _node->max = map.id;
        } else if(!const_name(db, &constant, expr[*start], len)) {
            /* constant name */
            _node->type = NODE_TYPE_CONSTANT;
            _node->min = constant.value;
            _node->max = constant.value;
        } else {
            /* set default value */
            _node->type = NODE_TYPE_LOCAL;
            _node->min = 0;
            _node->max = 0;

            /* find the set variable in the linked list */
            set = block->set;
            while(NULL != set) {
                /* copy the set block's node */
                if(0 == ncs_strcmp(set->ptr[0], expr[*start])) {
                    __node = set->set_node;
                    if(NULL == __node)
                        goto failed;

                    _node->var = 0;

                    /* copy the range */
                    _node->range = copyrange(__node->range);
                    _node->min = minrange(_node->range);
                    _node->max = maxrange(_node->range);

                    /* copy the logic tree */
                    if(NULL != __node->cond)
                        _node->cond = copy_deep_any_tree(__node->cond);
                    _node->cond_cnt = __node->cond_cnt;

                    /* copy the formula */
                    if(NULL != __node->formula)
                        _node->formula = convert_string(__node->formula);
                    break;
                }

                /* iterate the set blocks */
                set = set->set;
            }

            /* set the range 0 to INT_MAX on 'for'
             * block's condition expression */
            if(flag & EVALUATE_FLAG_ITERABLE_SET)
                _node->max = 2147483647;
        }
    }

    /* set the result */
    *node = _node;
    return CHECK_PASSED;

failed:
    node_free(_node);
    return CHECK_FAILED;
}

int evaluate_function(block_r * block, char ** expr, int start, int end, var_res * func, node_t * node) {
    int i = 0;
    int ret = 0;
    int arg_off = 0;
    int arg_cnt = 0;
    int eng_off = 0;
    token_r * token = NULL;

    /* error on invalid references */
    exit_null_safe(4, block, expr, func, node);

    /* error on invalid indexes */
    if(start > end)
        return CHECK_FAILED;

    /* substitute with malloc to improve performance */
    token = calloc(1, sizeof(token_r));
    if(NULL == token)
        return CHECK_FAILED;

    for(i = start; i <= end; i++)
        token->script_ptr[token->script_cnt++] = expr[i];

    /* save block->eng stack top */
    eng_off = block->eng_cnt;

    /* push function arguments onto the block->ptr stack */
    arg_off = block->ptr_cnt;
    if(ret = stack_ptr_call_(block, token, &arg_cnt))
        goto clean;

    /* execute the proper handler for the function */
    switch(func->id) {
        case 2: ret = evaluate_function_getequiprefinerycnt(block, arg_off, arg_cnt, func, node);   break; /* getequiprefinerycnt */
        case 3: ret = evaluate_function_readparam(block, arg_off, arg_cnt, func, node);             break; /* readparam */
        case 4: ret = evaluate_function_getskilllv(block, arg_off, arg_cnt, func, node);            break; /* getskilllv */
        case 5: ret = evaluate_function_rand(block, arg_off, arg_cnt, func, node);                  break; /* rand */
        case 8: ret = evaluate_function_getiteminfo(block, arg_off, arg_cnt, func, node);           break; /* getiteminfo */
        case 9: ret = evaluate_function_getequipid(block, arg_off, arg_cnt, func, node);            break; /* getequipid */
        case 13: ret = evaluate_function_isequipped(block, arg_off, arg_cnt, func, node);           break; /* isequipped */
        case 49: ret = evaluate_function_groupranditem(block, arg_off, arg_cnt, func, node);        break; /* groupranditem */
        default:
            exit_func_safe("unsupported function '%s' in item %d", func->name, block->item_id);
            goto clean;
    }

    /* pop the function arguments from the block->ptr stack */
    for(i = 0; i < arg_cnt; i++)
        if(block_stack_pop(block, TYPE_PTR))
            return CHECK_FAILED;

    /* pop the any translation pushed onto the block->eng stack */
    for(i = eng_off; i < block->eng_cnt;)
        if(block_stack_pop(block, TYPE_ENG))
            return CHECK_FAILED;

clean:
    SAFE_FREE(token);
    return ret;
}

/* rand takes one or two arguments; [0, max) for
 * one argument and [min, max] for two arguments */
int evaluate_function_rand(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int ret = 0;
    node_t * operand_1 = NULL;
    node_t * operand_2 = NULL;

    switch(cnt) {
        case 1: /* [0, max) */
            operand_1 = evaluate_expression(block, block->ptr[off], 1, EVALUATE_FLAG_KEEP_NODE);
            if(NULL == operand_1)
                goto failed;

            node->min = 0;
            node->max = maxrange(operand_1->range) - 1;
            break;
        case 2: /* [min, max] */
            operand_1 = evaluate_expression(block, block->ptr[off], 1, EVALUATE_FLAG_KEEP_NODE);
            if(NULL == operand_1)
                goto failed;

            operand_2 = evaluate_expression(block, block->ptr[off + 1], 1, EVALUATE_FLAG_KEEP_NODE);
            if(NULL == operand_2)
                goto failed;

            node->min = RANGE_MIN(operand_1->range, operand_2->range);
            node->max = RANGE_MAX(operand_1->range, operand_2->range);
            break;
        default:
            return exit_func_safe("invalid argument count to "
            "function '%s' in %d", func->name, block->item_id);
    }

    node->formula = convert_string("random");

clean:
    node_free(operand_1);
    node_free(operand_2);
    return ret;

failed:
    ret = CHECK_FAILED;
    goto clean;
}

/* groupranditem takes on a constant id or value */
int evaluate_function_groupranditem(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int i = 0;
    int ret = 0;
    int min = 0;
    node_t * group_id = NULL;
    range_t * temp = NULL;
    range_t * iter = NULL;
    item_group_t item_group;

    /* error on invalid references */
    exit_null_safe(3, block, func, node);

    switch(cnt) {
        case 1:
            /* group id must evaluate to a integer */
            group_id = evaluate_expression(block, block->ptr[off], 1, EVALUATE_FLAG_KEEP_NODE);
            if(NULL == group_id)
                goto failed;

            /* group id must be a constant */
            if(group_id->range->min != group_id->range->max)
                goto failed;

            memset(&item_group, 0, sizeof(item_group_t));
            if(item_group_id(block->script->db, &item_group, group_id->range->min))
                goto failed;

            if(0 >= item_group.size)
                goto failed;

            if(1 == item_group.size) {
                /* evaluate_expression_recursive will make the range node */
                node->min = item_group.item_id[0];
                node->max = item_group.item_id[0];
            }

            /* build linked list of discontinous item id in item group */
            min = item_group.item_id[i];
            for(i = 1; i < item_group.size - 1; i++) {
                /* create a new range for each discontinous item id */
                if(item_group.item_id[i] + 1 != item_group.item_id[i + 1]) {
                    if(node->range == NULL) {
                        iter = node->range = mkrange(INIT_OPERATOR, min, item_group.item_id[i], DONT_CARE);
                    } else {
                        temp = mkrange(INIT_OPERATOR, min, item_group.item_id[i], DONT_CARE);

                        /* linked the nodes */
                        iter->next = temp;
                        temp->prev = iter;
                        iter = temp;
                    }
                    min = item_group.item_id[i + 1];
                }
            }


            break;
        default:
            return exit_func_safe("invalid argument count to "
            "function '%s' in %d", func->name, block->item_id);
    }

clean:
    node_free(group_id);
    item_group_free(&item_group);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int evaluate_function_readparam(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int ret = 0;
    int argc = 0;

    /* error on invalid references */
    exit_null_safe(3, block, func, node);

    /* error on invalid argument count */
    if(1 != cnt)
        return exit_func_safe("invalid argument count to "
        "function '%s' in %d", func->name, block->item_id);

    /* write the dependency */
    if (stack_eng_map(block, block->ptr[off], MAP_READPARAM_FLAG, &argc) ||
        argc != 1)
        goto failed;

    node->formula = convert_string(block->eng[block->eng_cnt - 1]);

    /* readparam is a fixed range */
    node->min = func->min;
    node->max = func->max;

clean:
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int evaluate_function_getskilllv(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int ret = 0;
    int len = 0;
    skill_t skill;
    node_t * id = NULL;

    /* error on invalid references */
    exit_null_safe(3, block, func, node);

    /* error on invalid argument count */
    if(1 != cnt)
        return exit_func_safe("invalid argument count to "
        "function '%s' in %d", func->name, block->item_id);

    /* search for skill by name */
    if( SCRIPT_STRING(block->ptr[off][0]) &&
        !skill_name(block->script->db, &skill, block->ptr[off], strlen(block->ptr[off])))
        goto found;
    else
        return CHECK_FAILED;

    /* fallback by evaluating expression for skill id */
    id = evaluate_expression(block, block->ptr[off], 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == id)
        goto failed;

    /* support constant only */
    if(id->min != id->max ||
       skill_id(block->script->db, &skill, id->min))
        goto failed;

found:
    len = strlen(skill.desc) + 16;
    node->formula = calloc(len, sizeof(char));
    if(NULL == node->formula)
        goto failed;
    sprintf(node->formula, "%s Level", skill.desc);
    node->min = 0;
    node->max = skill.maxlv;

clean:
    node_free(id);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int evaluate_function_isequipped(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int i = 0;
    int ret = 0;
    int len = 0;
    int offset = 0;
    char * buf = NULL;
    int item_off = 0;
    int item_cnt = 0;

    /* error on invalid references */
    exit_null_safe(3, block, func, node);

    /* error on invalid argument count */
    if(0 == cnt)
        return exit_func_safe("invalid argument count to "
        "function '%s' in %d", func->name, block->item_id);

    /* push item names into the block->eng stack */
    len = block->arg_cnt;
    item_off = block->eng_cnt;
    for(i = 0; i < cnt; i++)
        if(stack_eng_item(block, block->ptr[off + i], &item_cnt))
            return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    /* write the isequip string */
    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    offset += sprintf(&buf[offset], "%s", block->eng[item_off]);
    if(block->eng_cnt - offset == 2) {
        /* <item> and <item> */
        offset += sprintf(&buf[offset], " and %s", block->eng[item_off + 1]);
    } else {
        /* <item>, <item>, and <item> */
        for(i = item_off + 1; i < block->eng_cnt; i++)
            offset += (i + 1 == block->eng_cnt) ?
                sprintf(&buf[offset], ", and %s", block->eng[i]):
                sprintf(&buf[offset], ", %s", block->eng[i]);
    }
    offset += sprintf(&buf[offset], " is equipped");

    /* isequipped is a boolean function */
    node->formula = convert_string(buf);
    node->min = 0;
    node->max = 1;

    SAFE_FREE(buf);
    return ret;
}

int evaluate_function_getequiprefinerycnt(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int ret = 0;
    int len = 0;
    char * buf = NULL;
    int equip_cnt = 0;

    /* error on invalid references */
    exit_null_safe(3, block, func, node);

    /* error on invalid argument count */
    if(1 != cnt)
        return exit_func_safe("invalid argument count to "
        "function '%s' in %d", func->name, block->item_id);

    /* push equip location into the block->eng stack */
    len = block->arg_cnt;
    if(stack_eng_map(block, block->ptr[off], MAP_REFINE_FLAG, &equip_cnt) ||
       equip_cnt != 1)
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + strlen(node->id) + 32;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "%s's %s", block->eng[block->eng_cnt - 1], node->id);

    /* refine rate is a fixed range */
    node->formula = convert_string(buf);
    node->min = func->min;
    node->max = func->max;

    SAFE_FREE(buf);
    return ret;
}

int evaluate_function_getiteminfo(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int len = 0;
    int argc = 0;
    char * buf = NULL;

    /* error on invalid references */
    exit_null_safe(3, block, func, node);

    if(2 != cnt)
        return exit_func_safe("invalid argument count to "
        "function '%s' in %d", func->name, block->item_id);

    /* evaluate the item info type */
    len = block->arg_cnt;
    if(stack_eng_item(block, block->ptr[off], &argc) || argc != 1 ||
       stack_eng_map(block, block->ptr[off+1], MAP_ITEM_INFO_FLAG, &argc) || argc != 1)
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 32;

    node->formula = calloc(len, sizeof(char));
    if(NULL == node->formula)
        return CHECK_FAILED;

    sprintf(node->formula, "%s's %s", block->eng[block->eng_cnt - 2], block->eng[block->eng_cnt - 1]);
    node->min = func->min;
    node->max = func->max;

    return CHECK_PASSED;
}

int evaluate_function_getequipid(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int argc = 0;

    /* error on invalid references */
    exit_null_safe(3, block, func, node);

    if(1 != cnt)
        return exit_func_safe("invalid argument count to "
        "function '%s' in %d", func->name, block->item_id);

    if(stack_eng_map(block, block->ptr[off], MAP_REFINE_FLAG, &argc) ||
       argc != 1)
        return CHECK_FAILED;

    node->formula = convert_string(block->eng[block->eng_cnt - 1]);
    node->min = func->min;
    node->max = func->max;

    return CHECK_PASSED;
}

int node_steal(node_t * src, node_t * des) {
    exit_null_safe(2, src, des);

    des->id = src->id;
    des->formula = src->formula;
    des->cond = src->cond;
    des->range = src->range;
    des->min = minrange(des->range);
    des->max = maxrange(des->range);
    des->cond_cnt = src->cond_cnt;
    des->type = (src->type == NODE_TYPE_OPERATOR) ? NODE_TYPE_SUB : src->type;
    des->return_type |= src->return_type;

    src->id = NULL;
    src->formula = NULL;
    src->cond = NULL;
    src->range = NULL;
    return CHECK_PASSED;
}

int node_structure(node_t * root_node) {
    int i = 0;
    int j = 0;
    node_t * iter_node = NULL;

    /* operator precedence tree */
    static int op_pred[11][5] = {
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
    for(i = 0; i < 11; i++) {
        iter_node = root_node;
        do {
            if(iter_node->type == NODE_TYPE_OPERATOR) {
                for(j = 0; j < 5; j++) {
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

    return CHECK_PASSED;
}

/* node_evaluate uses the post-order traversal of a binary tree;
 * evaluate the left expression, then the right expression, and
 * then the current expression.
 *
 * to get a real good understanding of how this algorithm works,
 * simply draw a few examples and work through it by hand; which
 * was how the algorithm was developed. !D */
int node_evaluate(node_t * node, FILE * stm, logic_node_t * logic_tree, int flag) {
    range_t * temp = NULL;
    range_t * result = NULL;
    logic_node_t * new_tree = NULL;

    if(node->left == node->right) {
        /* unary operator has one expression
         * (operator) (expression) */
        if(node->left != NULL)
            if(node_evaluate(node->left, stm, logic_tree, flag))
                return SCRIPT_FAILED;
    } else {
        /* binary operators has two expression
         * (left-expression) (operator) (right-expression) */

        /* : operator node's left child is the (true) expression
         * and the node's right child is the (false) expression */
        if(node->type == NODE_TYPE_OPERATOR && node->op == ':' && logic_tree != NULL) {
            /* (condition) ? (true) : (false)
             *
             * explanation
             *  a logic tree represents a tree of conditions and each condition
             *  represents a range of values for a variable or function; (true)
             *  uses the logic tree generated from (condition) and (false) uses
             *  an inverse of the logic tree generated from (condition).
             *
             * example
             *  (getrefine() < 5) ? getrefine() + 5 : getrefine() - 5;
             *
             *  (condition)
             *  getrefine(0 - 4)     ; original logic tree use by (true)
             *  getrefine(5 - 15)    ; inverse logic tree use by (false)
             *
             *  (true)
             *  getrefine() + 5
             *  (0 - 4) + 5
             *  (5 - 9)
             *
             *  (false)
             *  getrefine() - 5
             *  (5 - 15) - 5
             *  (0 - 10)
             *
             *  (result)
             *  0 ~ 10 (refine rate)
             */

            /* (true)  original logic tree */
            if(node->left != NULL)
                if(node_evaluate(node->left, stm, logic_tree, flag))
                    return SCRIPT_FAILED;

            /* (false) inverse logic tree */
            new_tree = inverse_logic_tree(logic_tree);
            if(node->right != NULL)
                if(node_evaluate(node->right, stm, new_tree, flag))
                    return SCRIPT_FAILED;
            freenamerange(new_tree);

        /* normal traversal
         * evaluate the left expression before the right expression */
        } else {
            if(node->left != NULL)
                if(node_evaluate(node->left, stm, logic_tree, flag))
                    return SCRIPT_FAILED;

            /* ? operator node's left child is the (condition) expression and
             * the node's right child is the (true) : (false) expression */
            if(node->type == NODE_TYPE_OPERATOR && node->op == '?' && node->left->cond != NULL) {
                /* stack the (condition) on the existing logic tree;
                 * the search algorithm will search for variable and
                 * function from top to bottom */
                new_tree = node->left->cond;
                new_tree->stack = logic_tree;
                /* keep the logic tree of ? conditional */
                if(EVALUATE_FLAG_KEEP_TEMP_TREE & flag)
                    node->cond = copy_any_tree(new_tree);
                /* evaluate the (true) : (false) expression */
                if(node->right != NULL)
                    if(node_evaluate(node->right, stm, new_tree, flag))
                        return SCRIPT_FAILED;
            } else {
                if(node->right != NULL)
                    if(node_evaluate(node->right, stm, logic_tree, flag))
                        return SCRIPT_FAILED;
            }
        }
    }

    /* handle variable and function nodes; generate condition node */
    if( (node->type & (NODE_TYPE_FUNCTION | NODE_TYPE_VARIABLE)) ||
        ((node->type & NODE_TYPE_LOCAL) && (flag & EVALUATE_FLAG_ITERABLE_SET))) {
        /* error on invalid variable, function, or athena variable name */
        if(node->id == NULL)
            return SCRIPT_FAILED;

        /* search logic tree for variable, function or athena variable name */
        if(logic_tree != NULL) {
            result = search_tree_dependency(logic_tree, node->id, node->range);
            if(result != NULL) {
                temp = node->range;
                node->range = copyrange(result);
                node->min = minrange(node->range);
                node->max = maxrange(node->range);
                freerange(temp);
            }
        }

        /* create a condition node that can be added to a logic tree */
        node->cond = make_cond(node->var, node->id, node->range, NULL);

    /* handle unary operator nodes */
    } else if(node->type & NODE_TYPE_UNARY) {
        /* error on invalid node */
        if(node->left == NULL ||
           node->left->range == NULL)
            return SCRIPT_FAILED;

        /* evaluate the unary operator */
        switch(node->op) {
            case '-': node->range = negaterange(node->left->range); break;
            case '!': node->range = notrange(node->left->range); break;
            default: return SCRIPT_FAILED;
        }

        node_inherit(node);
    /* handle binary operator nodes */
    } else if(node->type & NODE_TYPE_OPERATOR) {
        /* error on invalid nodes */
        if(node->left == NULL ||
           node->right == NULL ||
           node->left->range == NULL ||
           node->right->range == NULL)
            return SCRIPT_FAILED;

        /* calculate range for operator node */
        switch(node->op) {
            case '*':
            case '/':
            case '+':
            case '-':
                node->range = calcrange(node->op, node->left->range, node->right->range);
                node_inherit(node);
                break;
            case '<' + '=':
            case '<':
            case '>' + '=':
            case '>':
            case '!' + '=':
            case '=' + '=':
                if(flag & EVALUATE_FLAG_EXPR_BOOL) {
                    /* typically, scripts use boolean expressions like 100 + 600 * (getrefine() > 5),
                     * which can be interpreted wrong for non-boolean expression, which should result
                     * in either 0 or 1 rather than range. */
                    node->range = calcrange(node->op, node->left->range, node->right->range);
                } else if(flag & EVALUATE_FLAG_VARIANT_SET) {
                    /* variant need to determine whether the loop is increasing or decreasing
                     * -1 less than (increasing), 0 equal, 1 greater than (decreasing) */
                    if(IS_EQUALSET(node->left->range, node->right->range)) {
                        node->range = mkrange(INIT_OPERATOR, 0, 0, DONT_CARE);
                    } else if(maxrange(node->left->range) <  maxrange(node->right->range)) {
                        node->range = mkrange(INIT_OPERATOR, -1, -1, DONT_CARE);
                    } else {
                        node->range = mkrange(INIT_OPERATOR, 1, 1, DONT_CARE);
                    }
                } else {
                    node->range = mkrange(INIT_OPERATOR, 0, 1, 0);
                }

                /* variable and functions generate logic trees that are inherited by up until the root node */
                node_inherit(node);
                break;
            case '&':
            case '|':
                node->range = calcrange(node->op, node->left->range, node->right->range);
                /* variable and functions generate logic trees that are inherited by up until the root node */
                node_inherit(node);
                break;
            case '&' + '&':
                if(flag & EVALUATE_FLAG_EXPR_BOOL) {
                    node->range = calcrange(node->op, node->left->range, node->right->range);
                    if(node->left->cond != NULL && node->right->cond != NULL)
                        node->cond = new_cond(LOGIC_NODE_AND, node->left->cond, node->right->cond);
                } else {
                    node->range = mkrange(INIT_OPERATOR, 0, 1, 0);
                }
                break;
            case '|' + '|':
                if(flag & EVALUATE_FLAG_EXPR_BOOL) {
                    node->range = calcrange(node->op, node->left->range, node->right->range);
                    if(node->left->cond != NULL && node->right->cond != NULL)
                        node->cond = new_cond(LOGIC_NODE_OR, node->left->cond, node->right->cond);
                } else {
                    node->range = mkrange(INIT_OPERATOR, 0, 1, 0);
                }
                break;
            case ':':
                /* iterable use the ? operator to handle the for condition */
                node->range = (flag & EVALUATE_FLAG_ITERABLE_SET) ?
                    copyrange(node->left->range) :
                    orrange(node->left->range, node->right->range);
                break;
            case '?':
                node->range = copyrange(node->right->range); /* right node is : operator */
                break;
            default:
                exit_func_safe("unsupported operator %c", node->op);
                return SCRIPT_FAILED;
        }

        /* track total number of conditions */
        node->cond_cnt = node->left->cond_cnt + node->right->cond_cnt;
    }

    /* calculate min and max */
    node->min = minrange(node->range);
    node->max = maxrange(node->range);

    /* dump the node list at each step;
     * used only for debugging and you
     * need to set the global node_dbg
     * stream */
    node_dump(node, stm);

    return SCRIPT_PASSED;
}

/* function must be called AFTER node->range is calc */
int node_inherit(node_t * node) {
    exit_null_safe(1, node);

    /* inherit only from left or right but not both;
     * conditions cannot be accurately interpreted */
    if(node->left->cond != NULL && node->right->cond == NULL) {
        /* inherit the logic tree from the left node */
        node->cond = make_cond(
            node->left->cond->var,
            node->left->cond->name,
            node->range,
            node->left->cond);
    } else if(node->left->cond == NULL && node->right->cond != NULL) {
        /* inherit the logic tree from the right node */
        node->cond = make_cond(
            node->right->cond->var,
            node->right->cond->name,
            node->range,
            node->right->cond);

    } else if(node->left == node->right && node->left->cond != NULL) {
        /* inherit for unary operator */
        node->cond = make_cond(
            node->left->cond->var,
            node->left->cond->name,
            node->range,
            node->left->cond);
    }

    /* inherit the return type */
    node->return_type |= node->left->return_type;
    node->return_type |= node->right->return_type;

    return SCRIPT_PASSED;
}

void node_free(node_t * node) {
    if(NULL != node) {
        if(NULL != node->cond)
            freenamerange(node->cond);
        if(NULL != node->range)
            freerange(node->range);
        SAFE_FREE(node->formula);
        SAFE_FREE(node->id);
        SAFE_FREE(node);
    }
}

void node_dump(node_t * node, FILE * stream) {
    if(node != NULL && stream != NULL) {
        fprintf(stream," -- Node %p --\n", (void *) node);
        switch(node->type) {
            case NODE_TYPE_OPERATOR:
                switch(node->op) {
                     case '<' + '=':    fprintf(stream,"     Type: Operator <=\n"); break;
                     case '>' + '=':    fprintf(stream,"     Type: Operator >=\n"); break;
                     case '!' + '=':    fprintf(stream,"     Type: Operator !=\n"); break;
                     case '=' + '=':    fprintf(stream,"     Type: Operator ==\n"); break;
                     case '&' + '&':    fprintf(stream,"     Type: Operator &&\n"); break;
                     case '|' + '|':    fprintf(stream,"     Type: Operator ||\n"); break;
                     default:           fprintf(stream,"     Type: Operator %c\n", node->op); break;
                 }
                 break;
            case NODE_TYPE_UNARY:       fprintf(stream,"     Type: Operator %c\n", node->op); break;
            case NODE_TYPE_OPERAND:     fprintf(stream,"     Type: Literal %d:%d\n", node->min, node->max); break;
            case NODE_TYPE_FUNCTION:    fprintf(stream,"     Type: Function %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_VARIABLE:    fprintf(stream,"     Type: Variable %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_LOCAL:       fprintf(stream,"     Type: Local %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_CONSTANT:    fprintf(stream,"     Type: Constant %s; %d:%d\n", node->id, node->min, node->max); break;
            case NODE_TYPE_SUB:         fprintf(stream,"     Type: Subexpression %s; %d:%d\n", node->formula, node->min, node->max); break;
            default:                    fprintf(stream,"     Type: %d\n", node->op); break;
        }
        fprintf(stream," Variable: %d\n", node->var);
        fprintf(stream,"  Formula: %s\n", node->formula);
        fprintf(stream,"   R-Type: %d\n", node->return_type);
        dmprange(node->range, stream, "Range; ");
        dmpnamerange(node->cond, stream, 0);
        fprintf(stream,"\n");
    }
}

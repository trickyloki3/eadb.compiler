/*
 *   file: script.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "script.h"

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

int list_tail(block_list_t * list, block_r ** block) {
    /* check null paramaters */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* check empty list */
    if(list->tail == list->head)
        return SCRIPT_FAILED;
    *block = list->tail;
    return SCRIPT_PASSED;
}

int list_head(block_list_t * list, block_r ** block) {
    /* check null paramaters */
    if(exit_null_safe(2, list, block))
        return SCRIPT_FAILED;
    /* check empty list */
    if(list->tail == list->head)
        return SCRIPT_FAILED;
    *block = list->head->next;
    return SCRIPT_PASSED;
}

int script_block_alloc(script_t * script, block_r ** block) {
    /* check null paramaters */
    if (exit_null_safe(2, script, block))
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
    /* reset the buffer while were at it :D */
    script->offset = 0;
    script->buffer[0] = '\0';
    return SCRIPT_PASSED;
}

int script_block_release(block_r * block) {
    int i = 0;

    /* free block name */
    SAFE_FREE(block->name);

    /* logic trees are generated from conditional blocks and
     * inherited by enclosing scope of the conditional block. */
    deepfreenamerange(block->logic_tree);
    block->logic_tree = NULL;

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

int script_buffer_write(block_r * block, int type, const char * str) {
    int ret = 0;
    int cnt = 0;
    int off = 0;
    int len = 0;
    char * buf = NULL;

    /* check for infidels */
    if (exit_null_safe(2, block, str))
        return CHECK_FAILED;

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

int script_buffer_unwrite(block_r * block, int type) {
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

int script_buffer_reset(block_r * block, int type) {
    int i = 0;
    int cnt = 0;

    /* error on invalid references */
    if(exit_null_safe(1, block))
        return CHECK_FAILED;

    /* get the number of elements on the stack */
    switch(type) {
        case TYPE_PTR: cnt = block->ptr_cnt; break;
        case TYPE_ENG: cnt = block->eng_cnt; break;
        default:
            return exit_func_safe("invalid stack type %d in item %d", type, block->item_id);
    }

    /* pop each element from the stack */
    for(i = 0; i < cnt; i++)
        script_buffer_unwrite(block, type);

    return CHECK_PASSED;
}

int script_formula_write(block_r * block, int index, node_t * node, char ** out) {
    int ret = 0;
    char * buffer = NULL;
    char * prefix = NULL;
    char * formula = NULL;
    int buffer_size = 0;
    int prefix_len = 0;
    int formula_len = 0;

    if (exit_null_safe(2, block, node))
        return SCRIPT_FAILED;

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
        buffer[ret] = '\0';
    }

    *out = buffer;
    return SCRIPT_PASSED;
}

int script_map_init(script_t * script, const char * map_path) {
    if(exit_null_safe(2, script, map_path))
        return CHECK_FAILED;

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

    if(exit_null_safe(2, map, val))
        return CHECK_FAILED;

    /* push table */
    lua_getglobal(block->map, map);
    if(!lua_istable(block->map, -1)) {
        exit_func_safe("%s is not a valid map", map);
        goto failed;
    }

    /* push key */
    index = lua_gettop(block->map);
    lua_pushinteger(block->map, id);
    lua_gettable(block->map, index);
    if(!lua_isstring(block->map, -1)) {
        exit_func_safe("%s at index %d does not map to a string", map, id);
        goto failed;
    }

    /* table[key] */
    *val = convert_string(lua_tostring(block->map, -1));

clean:
    /* reset the lua stack */
    lua_settop(block->map, 0);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int script_map_deit(script_t * script) {
    if(exit_null_safe(1, script))
        return CHECK_PASSED;

    if(NULL != script->map)
        lua_close(script->map);

    return CHECK_PASSED;
}

int script_extend_block(script_t * script, char * subscript, block_r * parent, block_r ** var) {
    token_r token;
    /* lex subscript */
    if(script_lexical(&token, subscript)) {
        exit_func_safe("failed to lex on '%s' in item %d", subscript, script->item.id);
        return SCRIPT_FAILED;
    }

    /* check empty token list */
    if(token.script_cnt <= 0) {
        exit_func_safe("zero tokens on '%s' in item %d", subscript, script->item.id);
        return SCRIPT_FAILED;
    }

    /* indirect-recurisve analysis on subscript */
    if(script_analysis(script, &token, parent, var) == SCRIPT_FAILED) {
        exit_func_safe("failed to parse %s in item %d", subscript, script->item.id);
        return SCRIPT_FAILED;
    }
    return SCRIPT_PASSED;
}

int script_block_write(block_r * block, char * fmt, ...) {
    int off = 0;
    int len = 0;
    int cnt = 0;
    char * str = NULL;
    va_list expr_list;

    /* check null */
    if (exit_null_safe(2, block, fmt))
        return SCRIPT_FAILED;

    /* get buffer state */
    off = block->arg_cnt;
    str = &block->arg[off];
    cnt = block->ptr_cnt;

    /* check argument string pointer size */
    if (cnt >= PTR_SIZE)
        return exit_func_safe("insufficient argument string pointers size exceeding %d in item %d", PTR_SIZE, block->item_id);

    /* check buffer size after format string is written to buffer
     * the program could  buffer overflow and crash at this point
     * if not, then check overflow and exit program */
    va_start(expr_list, fmt);
    len = vsprintf(str, fmt, expr_list);
    va_end(expr_list);

    if (off + len > BUF_SIZE)
        return exit_func_safe("buffer overflow with offset %d and length %d exceeding %d size on item %d", off, len, BUF_SIZE, block->item_id);

    /* update buffer state */
    block->arg[off + len] = '\0';
    block->arg_cnt += len + 1;

    /* set an argument string pointer to written string */
    block->ptr[cnt] = str;
    block->ptr_cnt++;
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

int script_block_dump(script_t * script, FILE * stm) {
    int i = 0;
    int off = 0;
    char buf[BUF_SIZE];
    block_r * iter = NULL;

    /* ignore dump if stream is NULL */
    if (stm == NULL)
        return SCRIPT_PASSED;

    /* check null paramaters */
    if(exit_null_safe(1, script))
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
            " Block Type: %d\n"
            " Block Paramater: %d\n",
            iter->item_id,
            (void *) iter,
            (void *) iter->link,
            (void *) iter->set,
            iter->type,
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
        /* dump the buffer */
        for (i = 0; i < iter->arg_cnt; i++)
            (iter->arg[i] == '\0') ?
                fprintf(stm, "NULL ") :
                fprintf(stm, "%c ", iter->arg[i]);
        fprintf(stm, "\n");
        iter = iter->next;
        off = 0;
    } while(iter != script->block.head);
    return SCRIPT_PASSED;
}

int script_init(script_t ** script, const char * rdb_path, const char * sdb_path, const char * map_path, int mode) {
    script_t * _script = NULL;

    /* error on invalid reference */
    if(exit_null_safe(3, script, rdb_path, sdb_path))
        return CHECK_FAILED ;

    /* error on invalid server mode */
    if(!(mode & DB_MODE))
        return CHECK_FAILED;

    /* construct script struct */
    _script = calloc(1, sizeof(script_t));
    if(NULL == _script)
        return CHECK_FAILED;

    /* set the mode */
    _script->mode = mode;

    /* initialize block doubly linked lists */
    list_init_head(&_script->block);
    list_init_head(&_script->free);

    /* initialize resource and server databases and mapping tables */
    if (init_db_load(&_script->db, rdb_path, sdb_path, _script->mode) ||
        script_map_init(_script, map_path)) {
        script_deit(&_script);
        return CHECK_FAILED;
    }

    *script = _script;
    return CHECK_PASSED;
}

int script_deit(script_t ** script) {
    script_t * _script = NULL;

    if(exit_null_safe(2, script, *script))
        return CHECK_PASSED;

    _script = *script;
    /* release all mappings */
    script_map_deit(_script);
    /* release all databases */
    if(_script->db)
        deit_db_load(&_script->db);
    /* release all blocks */
    script_block_finalize(_script);
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
    int j = 0;
    int len = 0;
    int ret = 0;
    int block_cnt = 0;
    char ** token = NULL;
    int token_cnt = 0;
    block_res block_type;       /* block id in block_db.txt */
    block_r * block = NULL;     /* current block being parsed */
    block_r * link = parent;    /* link blocks to if, else-if, else, and for */
    block_r * set = (var != NULL) ? *var : NULL;
    block_r * iterable_set_block = NULL;
    block_r * direction_set_block = NULL;
    char subscript[BUF_SIZE];   /* inject new code */

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
        if(!block_name(script->db, &block_type, token[i], strlen(token[i]))) {
            /* count the number of blocks parsed */
            block_cnt++;

            /* allocate and initialize a block */
            if(script_block_alloc(script, &block))
                return SCRIPT_FAILED;

            block->name = convert_string(block_type.name);
            block->item_id = script->item.id;
            block->type = block_type.id;

            /* copy the references from script to block
             * to avoid massive rewrite for passing the
             * references. */
            block->db = script->db;
            block->mode = script->mode;
            block->map = script->map;

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
                    if(token[i+1][0] != '(') return SCRIPT_FAILED;
                    ret = script_parse(token_list, &i, block, '\0', ')', FLAG_PARSE_LAST_TOKEN);
                    if(ret == SCRIPT_FAILED || token[i][0] != ')')
                        return SCRIPT_FAILED;

                    /* compound or simple statement */
                    if((token[i+1][0] == '{') ?
                        script_parse(token_list, &i, block, '\0', '}', FLAG_PARSE_LAST_TOKEN):
                        script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA)
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
                        ret = script_parse(token_list, &i, block, '\0', ')', FLAG_PARSE_LAST_TOKEN);
                        if(ret == SCRIPT_FAILED || token[i][0] != ')')
                            return SCRIPT_FAILED;

                        /* compound or simple statement */
                        if((token[i+1][0] == '{') ?
                            script_parse(token_list, &i, block, '\0', '}', FLAG_PARSE_LAST_TOKEN):
                            script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA)
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
                            script_parse(token_list, &i, block, '\0', '}', FLAG_PARSE_LAST_TOKEN):
                            script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA)
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
                    if(script_parse(token_list, &i, block, ',', ';', FLAG_PARSE_NORMAL))
                        return SCRIPT_FAILED;

                    /* extend the block list by using subscript */
                    if(script_extend_block(script, block->ptr[0], block, &set))
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
                        script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_LAST_TOKEN | FLAG_PARSE_KEEP_COMMA)
                        == SCRIPT_FAILED)
                        return SCRIPT_FAILED;
                    /* check that the compound or simple statement is not empty */
                    if(check_loop_expression(script, block->ptr[3], ";")) {
                        exit_func_safe("missing block expression"
                        " in for-loop for item id %d", script->item.id);
                        return SCRIPT_FAILED;
                    }

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

                    /* create set block */
                    if(script_extend_block(script, block->ptr[0], parent, &set)) {
                        exit_func_safe("failed to create iterable set block '%s' "
                        "in item id %d", block->ptr[0], script->item.id);
                        return SCRIPT_FAILED;
                    }

                    if(list_tail(&script->block, &iterable_set_block)) {
                        exit_func_safe("failed to query set block '%s' "
                        "in item id %d", block->ptr[0], script->item.id);
                        return SCRIPT_FAILED;
                    }

                    /* create the variant set block */
                    if(script_extend_block(script, block->ptr[2], parent, &set)) {
                        exit_func_safe("failed to create direction set block '%s' "
                        "in item id %d", block->ptr[2], script->item.id);
                        return SCRIPT_FAILED;
                    }

                    if(list_tail(&script->block, &direction_set_block)) {
                        exit_func_safe("failed to query set block '%s' "
                        "in item id %d", block->ptr[2], script->item.id);
                        return SCRIPT_FAILED;
                    }

                    /* rearranging the condition and set block in this format is a design choice
                     * for building the initial condition tree over the entire integer range and
                     * then depending on the direction, adjust the range from the set block to
                     * produce the best 'guess' of the iterable range. */
                    sprintf(subscript, "(%s) ? (%s) : 0", block->ptr[1], iterable_set_block->ptr[1]);
                    /* write the new subscript */
                    iterable_set_block->ptr_cnt--;
                    script_block_write(iterable_set_block, "%s", subscript);
                    iterable_set_block->flag |= EVALUATE_FLAG_ITERABLE_SET;

                    /* cheap hack to evaluate a 0 (increasing) or 1 (decreasing) */
                    sprintf(subscript, "(%s) < (%s)", direction_set_block->ptr[0], direction_set_block->ptr[1]);
                    /* write the new subscript */
                    direction_set_block->ptr_cnt--;
                    script_block_write(direction_set_block, "%s", subscript);
                    direction_set_block->flag |= EVALUATE_FLAG_VARIANT_SET;

                    /* create the if block */
                    sprintf(subscript,"%s", block->ptr[3]);
                    if(script_extend_block(script, subscript, parent, &set)) {
                        exit_func_safe("failed to create if block '%s' "
                        "in item id %d", subscript, script->item.id);
                        return SCRIPT_FAILED;
                    }
                    break;
                case 28:
                    /* variables have no enclosing scope within same script */
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
    if(block_cnt == 0) 
        return SCRIPT_FAILED;
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
        if(script_ptr[i][0] == ',' && !bracket_level && !subexpr_level && !(flag & FLAG_PARSE_KEEP_COMMA))
            continue;

        /* write the argument */
        len = strlen(script_ptr[i]);
        for (j = 0; j < len; j++)
            arg[arg_cnt++] = script_ptr[i][j];

        /* don't add space after athena symbol prefixes */
        if (!ATHENA_SCRIPT_SYMBOL(script_ptr[i][0]) && script_ptr[i][0] != '=')
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
    logic_node_t * logic_tree = NULL;

    /* handling iterable set ranges */
    range_t * iterable_range;
    range_t * temp_range;
    int val_min = 0;
    int val_max = 0;

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
            case 8: ret = translate_heal(iter); break;                                                              /* heal */
            case 9: ret = translate_heal(iter); break;                                                              /* percentheal */
            case 10: ret = translate_heal(iter); break;                                                             /* itemheal */
            case 11: ret = translate_skill(iter); break;                                                            /* skill */
            case 12: ret = translate_itemskill(iter); break;                                                        /* itemskill */
            case 13: ret = translate_skill_block(iter, iter->type); break;                                          /* unitskilluseid */
            case 14: ret = translate_status(iter); break;                                                           /* sc_start */
            case 15: ret = exit_func_safe("maintenance"); break;                                                     /* sc_start4 */
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
            case 41: ret = exit_func_safe("maintenance"); break;                                              /* makerune */
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
            case 54: ret = exit_func_safe("maintenance"); break;                                                           /* sc_start2 */
            case 55: ret = translate_petloot(iter); break;                                                          /* petloot */
            case 56: ret = translate_petrecovery(iter); break;                                                      /* petrecovery */
            case 57: ret = translate_petskillbonus(iter); break;                                                    /* petskillbonus */
            case 58: ret = translate_petskillattack(iter); break;                                                   /* petskillattack */
            case 59: ret = translate_petskillattack2(iter); break;                                                  /* petskillattack2 */
            case 60: ret = translate_petskillsupport(iter); break;                                                  /* petskillsupport */
            case 61: ret = translate_petheal(iter); break;                                                          /* petheal */
            /* non-simple structures */
            case 26:
               evaluate_expression(iter, iter->ptr[0], 1,
               EVALUATE_FLAG_KEEP_LOGIC_TREE | EVALUATE_FLAG_EXPR_BOOL);
               break;                                                                                              /* if */
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

        /* any failed translation indicate block cannot be translated */
        if(ret)
            return SCRIPT_FAILED;

        /* process next block */
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
            /* blocks that might have other blocks linked to it */
            case 26: /* if */
            case 27: /* else */
                /*if(iter->logic_tree != NULL)
                    if(script_generate_cond(iter->logic_tree, stdout, buf, buffer, offset, iter))
                        return SCRIPT_FAILED;*/
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


int script_recursive(db_t * db, int mode, lua_State * map, char * script, char ** value) {
    int ret = 0;
    script_t * scribe = NULL;

    if(exit_null_safe(5, db, mode, map, script, value))
        return CHECK_FAILED;

    /* initialize the script */
    scribe = calloc(1, sizeof(script_t));
    if(NULL == scribe)
        return CHECK_FAILED;

    scribe->db = db;
    scribe->mode = mode;
    scribe->map = map;
    scribe->offset = 0;
    list_init_head(&scribe->block);
    list_init_head(&scribe->free);

    /* compile the item script */
    if (script_lexical(&scribe->token, script) ||
        script_analysis(scribe, &scribe->token, NULL, NULL) ||
        script_translate(scribe) ||
        script_generate(scribe, scribe->buffer, &scribe->offset))
        goto failed;

    /* return the buffer */
    *value = convert_string(scribe->buffer);

clean:
    script_block_reset(scribe);
    SAFE_FREE(scribe);
    return ret;

failed:
    script_block_dump(scribe, stderr);
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
    if(exit_null_safe(3, block, expr, argc))
        return CHECK_FAILED;

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
    if(exit_null_safe(3, block, token, argc))
        return CHECK_FAILED;

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
    if(exit_null_safe(3, block, expr, argc))
        return CHECK_FAILED;

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
        if(!item_name(block->db, item, expr, len)) {
            if(script_buffer_write(block, TYPE_ENG, item->name))
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
                if( item_id(block->db, item, i) ||
                    script_buffer_write(block, TYPE_ENG, item->name)) {
                    /* invalid item id or block->eng stacked overflow */
                    ret = CHECK_FAILED;
                    goto clean;
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
    if(exit_null_safe(3, block, expr, argc))
        return CHECK_FAILED;

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
        if(!skill_name(block->db, skill, expr, len)) {
            if(script_buffer_write(block, TYPE_ENG, skill->desc))
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
                if( skill_id(block->db, skill, i) ||
                    script_buffer_write(block, TYPE_ENG, skill->desc)) {
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

    if(exit_null_safe(2, block, expr))
        return CHECK_FAILED;

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

    if(exit_null_safe(2, block, expr))
        return CHECK_FAILED;

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
    if(exit_null_safe(4, block, expr, pos, neg))
        return CHECK_FAILED;

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
    if (exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;

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

    if(exit_null_safe(3, block, expr, argc))
        return CHECK_FAILED;

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
    if(produce_id(block->db, &produces, item_level->min))
        goto failed;

    /* write the produce header */
    switch(item_level->min) {
        case 1: ret = script_buffer_write(block, TYPE_ENG, "Use to craft level 1 weapons.\n"); break;                     /* lv1 weapons */
        case 2: ret = script_buffer_write(block, TYPE_ENG, "Use to craft level 2 weapons.\n"); break;                     /* lv2 weapons */
        case 3: ret = script_buffer_write(block, TYPE_ENG, "Use to craft level 3 weapons.\n"); break;                     /* lv3 weapons */
        case 11: ret = script_buffer_write(block, TYPE_ENG, "Use to cook recipes with rank 5 success rate.\n"); break;    /* cooking sets */
        case 12: ret = script_buffer_write(block, TYPE_ENG, "Use to cook recipes with rank 4 success rate.\n"); break;
        case 13: ret = script_buffer_write(block, TYPE_ENG, "Use to cook recipes with rank 3 success rate.\n"); break;
        case 14: ret = script_buffer_write(block, TYPE_ENG, "Use to cook recipes with rank 2 success rate.\n"); break;
        case 15: ret = script_buffer_write(block, TYPE_ENG, "Use to cook recipes with rank 1 success rate.\n"); break;
        case 21: ret = script_buffer_write(block, TYPE_ENG, "Use to manufacture metals.\n"); break;                       /* metals */
        default:
            exit_func_safe("unsupported item level %d in item %d", item_level->min, block->item_id);
            goto failed;
    }
    if(ret)
        goto failed;

    produce = produces;
    while(produce != NULL) {
        if(item_id(block->db, item, produce->item_id))
            goto failed;

        sprintf(buf, "Recipe for %s", item->name);

        if(script_buffer_write(block, TYPE_ENG, buf))
            goto failed;

        for(i = 0; i < produce->ingredient_count; i++) {
            if(item_id(block->db, item, produce->item_id_req[i]))
                goto failed;

            sprintf(buf, "%d %s", produce->item_amount_req[i], item->name);

            if(script_buffer_write(block, TYPE_ENG, buf))
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

    if(exit_null_safe(3, block, expr, argc) || 0 == flag)
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

            /* failed to find resolve the id */
            exit_func_safe("failed to map %d (%s) on flag "
            "%d in item %d", i, expr, flag, block->item_id);
            goto failed;
found:
            /* write the mapped value on the block->eng stack */
            if(script_buffer_write(block, TYPE_ENG, value))
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

    if(exit_null_safe(3, block, expr, argc) || 0 == flag)
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

                if(skill_id(block->db, skill, i) ||
                   script_buffer_write(block, TYPE_ENG, skill->desc))
                    goto failed;

                SAFE_FREE(skill);
                continue;
            }
            if(DB_ITEM_ID & flag) {
                item = calloc(1, sizeof(item_t));
                if(NULL == item)
                    goto failed;

                if(item_id(block->db, item, i) ||
                   script_buffer_write(block, TYPE_ENG, item->name))
                    goto failed;

                SAFE_FREE(item);
                continue;
            }
            if(DB_MOB_ID & flag) {
                mob = calloc(1, sizeof(mob_t));
                if(NULL == mob)
                    goto failed;

                if(mob_id(block->db, mob, i) ||
                   script_buffer_write(block, TYPE_ENG, mob->name))
                    goto failed;

                SAFE_FREE(mob);
                continue;
            }
            if(DB_MERC_ID & flag) {
                merc = calloc(1, sizeof(merc_t));
                if(NULL == merc)
                    goto failed;

                if(merc_id(block->db, merc, i) ||
                   script_buffer_write(block, TYPE_ENG, merc->name))
                    goto failed;

                SAFE_FREE(merc);
                continue;
            }
            if(DB_PET_ID & flag) {
                pet = calloc(1, sizeof(pet_t));
                if(NULL == pet)
                    goto failed;

                if(pet_id(block->db, pet, i) ||
                   script_buffer_write(block, TYPE_ENG, pet->name))
                    goto failed;

                SAFE_FREE(pet);
                continue;
            }
            if(DB_MAP_ID & flag) {
                map = calloc(1, sizeof(map_res));
                if(NULL == map)
                    goto failed;

                if(map_id(block->db, map, i) ||
                   script_buffer_write(block, TYPE_ENG, map->name))
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

    if(exit_null_safe(3, block, expr, argc))
        return CHECK_FAILED;

    top = block->eng_cnt;

    /* group id must evaluate to a integer */
    group_id = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == group_id)
        goto failed;

    /* group id must be a constant */
    if(group_id->min != group_id->max)
        goto failed;

    if(item_group_id(block->db, &item_group, group_id->min))
        goto failed;

    if(0 >= item_group.size)
        goto failed;

    sprintf(buf, "group %d (%d items)", group_id->min, item_group.size);

    if(script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(2, block, expr))
        return CHECK_FAILED;

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

    off += sprintf(&buf[off], "activate on ");

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
            off += sprintf(&buf[off], "skills ");
        else if(BF_NORMAL & val)
            off += sprintf(&buf[off], "normal attack ");
        else
            /* unsupported bit */
            goto failed;
    } else {
        /* default depends on trigger type */
        if(BF_MISC & val || BF_MAGIC & val)
            off += sprintf(&buf[off], "skills ");
        else if(BF_WEAPON & val)
            off += sprintf(&buf[off], "normal attack ");
    }

    if(script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(2, block, expr))
        return CHECK_FAILED;

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
     *  [weapon or magic and misc]
     *  [on self or target ]
     */

    off += sprintf(&buf[off], "activate on ");

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
        off += sprintf(&buf[off], "magic and misc");
    else if(ATF_MAGIC & val)
        off += sprintf(&buf[off], "magic ");
    else if(ATF_MISC & val)
        off += sprintf(&buf[off], "misc ");
    else
        /* unsupported bit */
        goto failed;

    /* trigger target */
    if(ATF_SELF & val)
        off += sprintf(&buf[off], "on self");
    else if(ATF_TARGET & val)
        off += sprintf(&buf[off], "on target");
    else
        /* default is target */
        off += sprintf(&buf[off], "on target");

    if(script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(2, block, script))
        return CHECK_FAILED;

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
    if(script_recursive(block->db, block->mode, block->map, script, &buf) ||
       script_buffer_write(block, TYPE_ENG, buf))
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
    if (script_buffer_write(block, TYPE_ENG, buf) ||
        script_formula_write(block, block->eng_cnt - 1, node, &formula) ||
        formula == NULL)
        goto failed;

    /* pop results and write final translated string */
    if (script_buffer_unwrite(block, TYPE_ENG) ||
        script_buffer_write(block, TYPE_ENG, formula))
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
    if(exit_null_safe(4, block, argc, size, func))
        return CHECK_FAILED;

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

    if(exit_null_safe(1, block))
        return CHECK_FAILED;

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

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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

    if(exit_null_safe(1, block))
        return CHECK_FAILED;

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

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(1, block))
        return CHECK_FAILED;

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

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(1, block))
        return CHECK_FAILED;

    /* error on invalid argument count */
    if(2 > block->ptr_cnt)
        return exit_func_safe("missing hp or sp argument for %s in item %d", block->name, block->item_id);

    len = block->arg_cnt;
    if(stack_eng_int_signed(block, block->ptr[0], 1, "Recover HP by", "Drain HP by") &&
       stack_eng_int_signed(block, block->ptr[1], 1, "Recover SP by", "Drain SP by"))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    for(i = 0; i < block->eng_cnt; i++)
        off += sprintf(&buf[off], "%s.", block->eng[i]);

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(1, block))
        return CHECK_FAILED;

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
        script_buffer_unwrite(block, TYPE_ENG);

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf)) {
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
    if(status_id(block->db, &status, effect->min)) {
        printf("%d\n", effect->min);
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

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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

    if(status_id(block->db, &status, effect->min))
        goto failed;

    len = strlen(status.scend);
    if(0 >= len)
        goto failed;

    buf = calloc(len + 32, sizeof(char));
    if(NULL == buf)
        goto failed;

    sprintf(buf, "Cures %s.", status.scend);

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(1, block))
        return CHECK_FAILED;

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
    if(pet_id(block->db, &pet, id->min) ||
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

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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
    if(exit_null_safe(2, block, prefix) ||
       1 > block->ptr_cnt)
        return SCRIPT_FAILED;

    /* error on empty string */
    blen = strlen(block->ptr[0]);
    plen = strlen(prefix);
    if(0 >= blen || 0 >= plen)
        return SCRIPT_FAILED;

    /* search for bonus information */
    bonus = calloc(1, sizeof(bonus_res));
    if(NULL == bonus)
        return SCRIPT_FAILED;

    if(bonus_name(block->db, bonus, prefix, plen, block->ptr[0], blen)) {
        exit_func_safe("unsupported %s %s for item "
        "%d", prefix, block->ptr[0], block->item_id);
        goto failed;
    }

    len = block->arg_cnt;

    /* translate each bonus argument by argument type */
    for(i = 0, j = 1; i < bonus->type_cnt; i++, j++) {

        /* push the argument on the block->eng stack */
        switch(bonus->type[i]) {
            case 'n': ret = stack_eng_int(block, block->ptr[j], 1);                             break; /* Integer Value */
            case 'p': ret = stack_eng_int(block, block->ptr[j], 1);                             break; /* Integer Percentage */
            case 'r': ret = stack_eng_map(block, block->ptr[j], MAP_RACE_FLAG, &cnt);           break; /* Race */
            case 'l': ret = stack_eng_map(block, block->ptr[j], MAP_ELEMENT_FLAG, &cnt);        break; /* Element */
            case 'w': ret = stack_eng_grid(block, block->ptr[j]);                               break; /* Splash */
            case 'z':                                                                           break; /* Meaningless */
            case 'e': ret = stack_eng_map(block, block->ptr[j], MAP_EFFECT_FLAG, &cnt);         break; /* Effect */
            case 'q': ret = stack_eng_int(block, block->ptr[j], 100);                           break; /* Integer Percentage / 100 */
            case 'k': ret = stack_eng_db(block, block->ptr[j], DB_SKILL_ID, &cnt);              break; /* Skill */
            case 's': ret = stack_eng_map(block, block->ptr[j], MAP_SIZE_FLAG, &cnt);           break; /* Size */
            case 'c': ret = stack_eng_db(block, block->ptr[j], DB_MOB_ID, &cnt);                break; /* Monster Class & Job ID * Monster ID */
            case 'o': ret = stack_eng_int(block, block->ptr[j], 10);                            break; /* Integer Percentage / 10 */
            case 'm': ret = stack_eng_db(block, block->ptr[j], DB_ITEM_ID, &cnt);               break; /* Item ID */
            case 'x': ret = stack_eng_int(block, block->ptr[j], 1);                             break; /* Level */
            case 'g': ret = stack_eng_map(block, block->ptr[j], MAP_REGEN_FLAG, &cnt);          break; /* Regen */
            case 'a': ret = stack_eng_int(block, block->ptr[j], 1000);                          break; /* Millisecond */
            case 'h': ret = stack_eng_int(block, block->ptr[j], 1);                             break; /* SP Gain Bool */
            case 'v': ret = stack_eng_map(block, block->ptr[j], MAP_CAST_FLAG, &cnt);           break; /* Cast Self, Enemy */
            case 't': ret = stack_eng_trigger_bt(block, block->ptr[j]);                         break; /* Trigger BT */
            case 'y': ret = stack_eng_item_group_name(block, block->ptr[j], &cnt);              break; /* Item Group */
            case 'd': ret = stack_eng_trigger_atf(block, block->ptr[j]);                        break; /* Triger ATF */
            case 'f': ret = stack_eng_int(block, block->ptr[j], 1);                             break; /* Cell */
            case 'b': ret = stack_eng_map(block, block->ptr[j], MAP_TARGET_FLAG, &cnt);         break; /* Flag Bitfield */
            case 'i': ret = stack_eng_map(block, block->ptr[j], MAP_WEAPON_FLAG, &cnt);         break; /* Weapon Type */
            case 'j': ret = (stack_eng_map(block, block->ptr[j], MAP_CLASS_FLAG, &cnt) &&
                             stack_eng_db(block, block->ptr[j], DB_MOB_ID, &cnt));              break; /* Class Group & Monster */
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
        default:
            exit_func_safe("unsupport bonus argument count "
            "%d in item %d", bonus->type_cnt, block->item_id);
    }

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
        goto failed;

clean:
    SAFE_FREE(buf);
    SAFE_FREE(bonus);
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
    if(exit_null_safe(1, block) ||
       block->ptr_cnt < 2)
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

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
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
    int off = 0;
    char * buf = NULL;

    /* error on invalid reference and empty argument */
    if(exit_null_safe(1, block) ||
       block->ptr_cnt < 1)
        return CHECK_FAILED;

    len = block->arg_cnt;
    if(stack_eng_int(block, block->ptr[0], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    off += sprintf(buf, "Pet can loot and hold upto %s items.\n"
                        "Use pet performance to transfer items to inventory.",
                        block->eng[0]);

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_petskillbonus(block_r * block) {
    int ret = 0;
    int len = 0;
    char * buf = NULL;

    len = block->arg_cnt;
    if( translate_bonus(block, "bonus") ||
        stack_eng_int(block, block->ptr[2], 1) ||
        stack_eng_int(block, block->ptr[3], 1))
        return CHECK_FAILED;
    len = (block->arg_cnt - len) + 128;

    buf = calloc(len, sizeof(char));
    if(NULL == buf)
        return CHECK_FAILED;

    sprintf(buf, "Pet skill bonus is activated for %s seconds with a delay"
    " of %s seconds.\n -> %s", block->eng[1], block->eng[2], block->eng[0]);

    if(script_buffer_reset(block, TYPE_ENG) ||
       script_buffer_write(block, TYPE_ENG, buf))
        ret = CHECK_FAILED;

    SAFE_FREE(buf);
    return ret;
}

int translate_petskillattack2(block_r * block) {
    /*int ret = 0;
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
    if(brate != NULL) node_free(brate);*/
    return exit_abt_safe("maintenance");
}

int translate_skill(block_r * block) {
    int ret = 0;
    int cnt = 0;
    return exit_abt_safe("maintenance");
    /* error on invalid references
     * and error on empty argument */
    if(exit_null_safe(1, block) ||
       block->ptr_cnt < 2)
        return CHECK_FAILED;

    /* translate skill name and level */
    if(stack_eng_db(block, block->ptr[0], DB_SKILL_ID, &cnt) ||
       cnt > 1 || /* skill id must be constant */
       stack_eng_int(block, block->ptr[1], 1))
        goto failed;

    /* translate skill flag */
    if(block->ptr_cnt == 3)
        ;


    goto failed;

clean:
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int translate_searchstore(block_r * block) {

    /* check for null references */
    if(exit_null_safe(1, block))
        return CHECK_FAILED;

    /* check empty block->ptr stack */
    if(1 > block->ptr_cnt)
        return exit_func_safe("searchstore is missing "
        "amount or effect in item %d", block->item_id);


    return SCRIPT_PASSED;
}

int translate_buyingstore(block_r * block) {
    return SCRIPT_PASSED;
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

int translate_getexp(block_r * block, int handler) {
    //int offset = 0;
    //char buf[BUF_SIZE];
    //node_t * exp_amount = NULL;
    ///* check null paramater */
    //if(exit_null_safe(1, block))
    //    return SCRIPT_FAILED;
    //exp_amount = evaluate_expression(block, block->ptr[0], 1,
    //    EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA);
    //if(exp_amount != NULL) {
    //    offset = sprintf(buf,"Gain %s %s experience.",
    //             status_formula(&exp_amount->stack[exp_amount->
    //                stack_cnt], block->eng[1], exp_amount, 'n', 1),
    //             (handler == 43)?"player":"guild");
    //    buf[offset] = '\0';
    //    translate_write(block, buf, 1);
    //    node_free(exp_amount);
    //} else {
    //    return SCRIPT_FAILED;
    //}
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

int translate_autobonus(block_r * block, int flag) {
    //int offset = 0;
    //char buf[BUF_SIZE];
    //char aux[BUF_SIZE];
    //node_t * rate = NULL;
    ///* check null paramater */
    //if(exit_null_safe(1, block))
    //    return SCRIPT_FAILED;
    //rate = evaluate_expression(block, block->ptr[1], 1, EVALUATE_FLAG_KEEP_NODE|EVALUATE_FLAG_WRITE_FORMULA);
    //if(rate == NULL) return SCRIPT_FAILED;
    //translate_bonus_float_percentage(block, rate, &offset, 100);
    //if (stack_eng_time(block, block->ptr[2], 1) == SCRIPT_FAILED)
    //    return SCRIPT_FAILED;
    ///* translate for autobonus and autobonus2 */
    //if(flag & 0x01 || flag & 0x02) {
    //    if(block->ptr_cnt > 3)
    //        if(translate_trigger(block, block->ptr[3], 1) == SCRIPT_FAILED)
    //            return SCRIPT_FAILED;
    //    offset = sprintf(buf,"Add %s chance to autobonus for %s when attacked.%s",
    //        formula(aux, block->eng[1], rate), block->eng[3], (block->eng[5] != NULL)?block->eng[5]:"");
    //}

    //if(flag & 0x04) {
    //    if(block->ptr_cnt > 3)
    //        if(translate_skill(block, block->ptr[3]) == SCRIPT_FAILED)
    //            return SCRIPT_FAILED;
    //    offset = sprintf(buf,"Add %s chance to autobonus for %s when using skill %s.",
    //        formula(aux, block->eng[1], rate), block->eng[3], (block->eng[5] != NULL)?block->eng[5]:"");
    //}
    //buf[offset] = '\0';
    //translate_write(block, buf, 1);
    //if(rate != NULL) node_free(rate);
    return exit_abt_safe("maintenance");
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

int translate_petrecovery(block_r * block) {
    //int offset = 0;
    //char buf[BUF_SIZE];
    //node_t * delay = NULL;
    //if(exit_null_safe(1, block))
    //    return SCRIPT_FAILED;
    //if(translate_status_end(block) == SCRIPT_FAILED)
    //    return SCRIPT_FAILED;
    ///* evaluate the delay expression */
    //delay = evaluate_argument(block, block->ptr[1]);
    //if(delay == NULL) return SCRIPT_FAILED;
    //offset += sprintf(buf, "Pet cures %s every %d seconds.", block->eng[0], delay->min);
    //buf[offset] = '\0';
    //translate_write(block, buf, 1);
    //node_free(delay);
    return exit_abt_safe("maintenance");
}

int translate_petskillattack(block_r * block) {
    /*int ret = 0;
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
    if(brate != NULL) node_free(brate);*/
    return exit_abt_safe("maintenance");
}

int translate_petskillsupport(block_r * block) {
    /*int ret = 0;
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
    if(sp != NULL) node_free(sp);*/
    return exit_abt_safe("maintenance");
}

int translate_petheal(block_r * block) {
    /*int ret = 0;
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
    if(sp != NULL) node_free(sp);*/
    return exit_abt_safe("maintenance");
}

int translate_write(block_r * block, char * desc, int flag) {
    return script_buffer_write(block, TYPE_ENG, desc);
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

int id_write(node_t * node, char * fmt, ...) {
    if(exit_null_safe(2, node, fmt))
        return CHECK_FAILED;

    va_list expr_list;
    va_start(expr_list, fmt);
    int offset = node->stack_cnt;                               /* current offset on stack */
    char * temp = &node->stack[offset];                         /* record current offset on stack */
    offset += vsprintf(node->stack + offset, fmt, expr_list);   /* write new string onto stack */
    va_end(expr_list);
    node->stack_cnt = offset + 1;                               /* set new offset on stack */
    node->id = temp;
    return CHECK_PASSED;
}

int var_write(node_t * node, char * fmt, ...) {
    if(exit_null_safe(2, node, fmt))
        return CHECK_FAILED;

    va_list expr_list;
    va_start(expr_list, fmt);
    int offset = node->stack_cnt;                               /* current offset on stack */
    char * temp = &node->stack[offset];                         /* record current offset on stack */
    offset += vsprintf(node->stack + offset, fmt, expr_list);
    node->stack[offset] = '\0';                                 /* null terminate new string */
    va_end(expr_list);
    node->stack_cnt = offset + 1;
    node->var_str[node->var_cnt] = temp;
    node->var_cnt++;
    return CHECK_PASSED;
}

int expression_write(node_t * node, char * fmt, ...) {
    if(exit_null_safe(2, node, fmt))
        return CHECK_FAILED;

    va_list expr_list;
    va_start(expr_list, fmt);
    int offset = node->stack_cnt;                               /* current offset on stack */
    char * temp = &node->stack[offset];                         /* record current offset on stack */
    offset += vsprintf(node->stack + offset, fmt, expr_list);   /* write new string onto stack */
    node->stack[offset] = '\0';                                 /* null terminate new string */
    va_end(expr_list);
    node->stack_cnt = offset + 1;                               /* set new offset on stack */
    node->formula = temp;
    return CHECK_PASSED;
}

node_t * evaluate_expression(block_r * block, char * expr, int modifier, int flag) {
    int i = 0;
    node_t * result = NULL;
    token_r tokens;

    /* check null */
    if (exit_null_safe(2, block, expr))
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
    return evaluate_expression_post(block, result, modifier, flag);
}

node_t * evaluate_expression_post(block_r * block, node_t * root_node, int modifier, int flag) {
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
        if(script_buffer_write(block, TYPE_ENG, buf) ||
           script_formula_write(block, block->eng_cnt - 1, root_node, &formula) ||
           script_buffer_unwrite(block, TYPE_ENG) ||
           script_buffer_write(block, TYPE_ENG, formula))
            goto failed;
        SAFE_FREE(formula);
    } else if(EVALUATE_FLAG_WRITE_STACK & flag) {
        if(script_buffer_write(block, TYPE_ENG, buf))
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
    int i = 0;
    int j = 0;
    int len = 0;
    int ret = 0;
    int off = 0;
    int temp = 0;
    int expr_inx_open = 0;
    int expr_inx_close = 0;
    int expr_sub_level = 0;
    int op_cnt = 0;
    block_r * set_iter = NULL;
    const_t const_info;
    var_res var_info;
    map_res map_info;

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

            /* check for empty sub expressions */
            if ((expr_inx_close - expr_inx_open) == 1) {
                exit_func_safe("empty sub expression in item %d", block->item_id);
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
            id_write(temp_node,"%s", expr[i]);

            /* handle variable or function */
            memset(&var_info, 0, sizeof(var_res));
            if(!var_name(block->db, &var_info, expr[i], strlen(expr[i]))) {
                temp_node->cond_cnt = 1;
                temp_node->var = var_info.tag;
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

                    /* function returns a fixed range */
                    if(var_info.fflag & FUNC_DIRECT) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                    /* function returns a varied range */
                    } else if(var_info.fflag & FUNC_PARAMS) {
                        if( evaluate_function(block, expr, expr_inx_open + 1, expr_inx_close, &var_info, temp_node)) {
                            exit_func_safe("failed to evaluate function '%s' in item %d", expr[expr_inx_open], block->item_id);
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
            } else if(!const_name(block->db, &const_info, expr[i], strlen(expr[i]))) {
                /* create constant node */
                temp_node->type = NODE_TYPE_CONSTANT;
                temp_node->min = temp_node->max = const_info.value;
            /* handle map name */
            } else if(!map_name(block->db, &map_info, expr[i], strlen(expr[i]))) {
                temp_node->type = NODE_TYPE_CONSTANT;
                temp_node->min = temp_node->max = map_info.id;
            /* handle set'd variable */
            } else {
                /* create set block node */
                temp_node->type = NODE_TYPE_LOCAL; /* assume string literal unless resolved in for loop */
                temp_node->min = temp_node->max = 0;
                /* search for the set block in the link list */
                set_iter = block->set;
                while(set_iter != NULL) {
                    if(ncs_strcmp(set_iter->ptr[0], expr[i]) == 0) {
                        if(set_iter->set_node != NULL) {
                            /* inherit from the set block */
                            temp_node->var = (long) set_iter;
                            temp_node->min = set_iter->set_node->min;
                            temp_node->max = set_iter->set_node->max;
                            if(set_iter->set_node->range != NULL)
                                temp_node->range = copyrange(set_iter->set_node->range);
                            if(set_iter->set_node->cond != NULL)
                                temp_node->cond = copy_deep_any_tree(set_iter->set_node->cond);
                            temp_node->cond_cnt = set_iter->set_node->cond_cnt;
                            expression_write(temp_node, "%s", set_iter->set_node->formula);
                            node_write_recursive(set_iter->set_node, temp_node);
                            break;
                        }
                    }
                    set_iter = set_iter->set;
                }
                /* iterable set flag indicates condition is evaluated on
                 * the entire range of an integer 0 ~ INT_MAX, which is
                 * later combine with the range calculated */
                if(flag & EVALUATE_FLAG_ITERABLE_SET)
                    temp_node->max = 2147483647;
            }
            op_cnt++;
        /* catch everything else */
        } else {
            exit_func_safe("invalid token '%s' in item %d", expr[i], block->item_id);
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

    /* evaluate the infix expression with pre-order traversal */
    ret = evaluate_node(root_node->next, node_dbg, logic_tree, flag, &temp);

    /* the actual result is in root_node->next so we need
     * to copy any value we want to return using the root node */
    if(ret == SCRIPT_PASSED) {
        if(root_node->next->id != NULL)
            id_write(root_node, "%s", root_node->next->id);
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

        /* collect all the arguments on the last stack */
        node_write_recursive(root_node->next, root_node);

        /* write the formula on last stack */
        if(root_node->var_cnt > 0) {
            off = root_node->stack_cnt;
            root_node->formula = &root_node->stack[off];
            for(i = 0; i < root_node->var_cnt; i++) {
                if(root_node->var_set[i] != 0)
                    off += (i > 0) ?
                        sprintf(root_node->stack + off, ", %s", root_node->var_str[i]):
                        sprintf(root_node->stack + off, "%s", root_node->var_str[i]);
            }
            root_node->stack[off] = '\0';
            root_node->stack_cnt += off;
        }
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
    if( (node->type & (NODE_TYPE_FUNCTION | NODE_TYPE_VARIABLE)) ||
        ((node->type & NODE_TYPE_LOCAL) && (flag & EVALUATE_FLAG_ITERABLE_SET))) {
        if(node->id == NULL) {
            exit_abt_safe("invalid node id");
            return SCRIPT_FAILED;
        }
        /* check logic tree for dependency */
        if(logic_tree != NULL) {
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
        node->cond = make_cond(node->var, node->id, node->range, NULL);
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
            node->cond = make_cond(node->left->cond->var, node->left->cond->name, node->range, node->left->cond);

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
                node_inherit_cond(node);
                (*complexity)++;
                break;
            case '&':
            case '|':
                node->range = calcrange(node->op, node->left->range, node->right->range);
                /* variable and functions generate logic trees that are inherited by up until the root node */
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
                /* iterable use the ? operator to handle the for condition */
                node->range = (flag & EVALUATE_FLAG_ITERABLE_SET) ?
                    copyrange(node->left->range) :
                    orrange(node->left->range, node->right->range);
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
        node->cond = make_cond(node->left->cond->var, node->left->cond->name, node->range, node->left->cond);
    } else if(node->left->cond == NULL && node->right->cond != NULL) {
        node->cond = make_cond(node->right->cond->var, node->right->cond->name, node->range, node->right->cond);
    }
}

void node_write_recursive(node_t * node, node_t * root) {
    int i = 0;
    int j = 0;
    if(node->left == node->right) {
        /* unary node */
        if(node->left != NULL) node_write_recursive(node->left, root);
    } else {
        /* binary node */
        if(node->left != NULL) node_write_recursive(node->left, root);
        if(node->right != NULL) node_write_recursive(node->right, root);
    }
    /* write function or variable to simple list */
    if(node->var_cnt > 0) {
        /* include var stack of any node with var on stack */
        for(i = 0; i < node->var_cnt; i++) {
            /* check whether var or func is already included */
            for(j = 0; j < root->var_cnt; j++)
                if(root->var_set[j] == node->var_set[i])
                    break;
            /* add var or func to list */
            if(j == root->var_cnt || root->var_cnt == 0) {
                root->var_set[root->var_cnt] = node->var_set[i];
                var_write(root, "%s", node->var_str[i]);
            }
        }
    } else if(node->type & (NODE_TYPE_FUNCTION | NODE_TYPE_VARIABLE)) {
        /* check whether var or func is already included */
        for(i = 0; i < root->var_cnt; i++)
            if(root->var_set[i] == node->var)
                break;
        /* add var or func to list */
        if(i == root->var_cnt || root->var_cnt == 0) {
            root->var_set[root->var_cnt] = node->var;
            var_write(root, "%s", node->id);
        }
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
        fprintf(stm,"      var: %d\n", node->var);
        fprintf(stm,"stack_cnt: %d\n", node->stack_cnt);
        fprintf(stm," cond_cnt: %d\n", node->cond_cnt);
        for(int i = 0; i < node->var_cnt; i++)
            fprintf(stm,"  var_set: [%d] %s\n", node->var_set[i], node->var_str[i]);
        fprintf(stm,"  var_cnt: %d\n", node->var_cnt);
        fprintf(stm,"  formula: %s\n", node->formula);
        dmprange(node->range, stm, "range; ");
        dmpnamerange(node->cond, stm, 0);
        fprintf(stm,"\n");
    }
}

void node_free(node_t * node) {
    if(node != NULL) {
        if(node->cond != NULL) freenamerange(node->cond);
        if(node->range != NULL) freerange(node->range);
        free(node);
    }
}

int evaluate_function(block_r * block, char ** expr, int start, int end, var_res * func, node_t * node) {
    int i = 0;
    int ret = 0;
    int arg_off = 0;
    int arg_cnt = 0;
    token_r * token = NULL;

    /* error on invalid references */
    if(exit_null_safe(4, block, expr, func, node))
        return CHECK_FAILED;

    /* error on invalid indexes */
    if(start > end)
        return CHECK_FAILED;

    /* substitute with malloc to improve performance */
    token = calloc(1, sizeof(token_r));
    if(NULL == token)
        return CHECK_FAILED;

    for(i = start; i <= end; i++)
        token->script_ptr[token->script_cnt++] = expr[i];

    /* push function arguments onto the block->ptr stack */
    arg_off = block->ptr_cnt;
    if(ret = stack_ptr_call_(block, token, &arg_cnt))
        goto clean;

    /* execute the proper handler for the function */
    switch(func->tag) {
        case 3: ret = evaluate_function_readparam(block, arg_off, arg_cnt, func, node);       break; /* readparam */
        case 4: ret = evaluate_function_getskilllv(block, arg_off, arg_cnt, func, node);      break; /* getskilllv */
        case 5: ret = evaluate_function_rand(block, arg_off, arg_cnt, func, node);            break; /* rand */
        case 49: ret = evaluate_function_groupranditem(block, arg_off, arg_cnt, func, node);  break; /* groupranditem */
        default:
            exit_func_safe("unsupported function '%s' in item %d", func->id, block->item_id);
            goto clean;
    }

    /* pop the function arguments from the block->ptr stack */
    for(i = 0; i < arg_cnt; i++)
        script_buffer_unwrite(block, TYPE_PTR);

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
            return exit_func_safe("invalid argument count to"
            " function '%s' in %d", func->id, block->item_id);
    }

    /* set the new formula and write dependency identifer */
    if( expression_write(node, "random") ||
        id_write(node, "random"))
        goto failed;

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
    if(exit_null_safe(3, block, func, node))
        return CHECK_FAILED;

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
            if(item_group_id(block->db, &item_group, group_id->range->min))
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
            return exit_func_safe("invalid argument count to"
            " function '%s' in %d", func->id, block->item_id);
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
    char * param = NULL;
    node_t * id = NULL;

    /* error on invalid references */
    if(exit_null_safe(3, block, func, node))
        return CHECK_FAILED;

    /* error on invalid argument count */
    if(1 != cnt)
        return exit_func_safe("invalid argument count to"
        " function '%s' in %d", func->id, block->item_id);

    id = evaluate_expression(block, block->ptr[off], 1, EVALUATE_FLAG_KEEP_NODE);
    if(NULL == id)
        return CHECK_FAILED;

    /* support only constant value */
    if(id->min != id->max)
        goto failed;

    /* write the dependency */
    if(script_map_id(block, "readparam", id->range->min, &param) ||
       expression_write(node, param) ||
       id_write(node, param))
        goto failed;

    /* readparam always range from 0 to max status points */
    node->min = func->min;
    node->max = func->max;

clean:
    SAFE_FREE(param);
    node_free(id);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
}

int evaluate_function_getskilllv(block_r * block, int off, int cnt, var_res * func, node_t * node) {
    int ret = 0;
    skill_t skill;
    node_t * id = NULL;
    /* error on invalid references */
    if(exit_null_safe(3, block, func, node))
        return CHECK_FAILED;

    /* error on invalid argument count */
    if(1 != cnt)
        return exit_func_safe("invalid argument count to"
        " function '%s' in %d", func->id, block->item_id);

    /* search for skill by name */
    if(ATHENA_SCRIPT_IDENTIFER(block->ptr[off][0]) &&
       skill_name(block->db, &skill, block->ptr[off], strlen(block->ptr[off]))) {
            /* fallback by evaluating expression for skill id */
            id = evaluate_expression(block, block->ptr[off], 1, EVALUATE_FLAG_KEEP_NODE);
            if(NULL == id)
                goto failed;

            /* support constant only */
            if(id->min != id->max ||
               skill_id(block->db, &skill, id->min))
                goto failed;
    }

    set_func(node, skill.id);
    expression_write(node, "%s Level", skill.desc);
    id_write(node, "%s Level", skill.desc);

    /* set min and max skill level */
    node->min = 0;
    node->max = skill.maxlv;

clean:
    node_free(id);
    return ret;
failed:
    ret = CHECK_FAILED;
    goto clean;
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
int script_generate_cond(logic_node_t * tree, FILE * stm, char * prefix, char * buffer, int * offset, block_r * block) {
    char buf[BUF_SIZE];
    int off = 0;
    buf[0] = '\0';

    switch(tree->type) {
        /* each OR node means that multiple conditions */
        case LOGIC_NODE_OR:
            if(tree->left != NULL)
                if(script_generate_cond(tree->left, stm, prefix, buffer, offset, block) == SCRIPT_FAILED)
                    return SCRIPT_FAILED;
            if(tree->right != NULL)
                if(script_generate_cond(tree->right, stm, prefix, buffer, offset, block) == SCRIPT_FAILED)
                    return SCRIPT_FAILED;
            break;
        /* write each OR's children (non-OR node) as separate condition */
        case LOGIC_NODE_AND:
            if(script_generate_and_chain(tree, buf, &off, block) == SCRIPT_FAILED)
                return SCRIPT_FAILED;
            *offset += sprintf(buffer + *offset, "%s[%s]\n", prefix, buf);
            break;
        case LOGIC_NODE_COND:
            if(script_generate_cond_node(tree, buf, &off, block) == SCRIPT_FAILED)
                return SCRIPT_FAILED;
            *offset += sprintf(buffer + *offset, "%s[%s]\n", prefix, buf);
            break;
        default: break;
    }
    return SCRIPT_PASSED;
}

/* recursively chain all cond nodes into one buffer */
int script_generate_and_chain(logic_node_t * tree, char * buf, int * offset, block_r * block) {
    int ret_value = 0;
    if(tree->left != NULL)
        switch(tree->left->type) {
            case LOGIC_NODE_AND: ret_value = script_generate_and_chain(tree->left, buf, offset, block); break;
            case LOGIC_NODE_COND: ret_value = script_generate_cond_node(tree->left, buf, offset, block); break;
            default: break;
        }
    if(ret_value == SCRIPT_FAILED) return SCRIPT_FAILED;

    if(tree->right != NULL)
        switch(tree->right->type) {
            case LOGIC_NODE_AND: ret_value = script_generate_and_chain(tree->right, buf, offset, block); break;
            case LOGIC_NODE_COND: ret_value = script_generate_cond_node(tree->right, buf, offset, block); break;
            default: break;
        }
    return ret_value;
}

int script_generate_cond_node(logic_node_t * tree, char * buf, int * offset, block_r * block) {
    int ret = SCRIPT_PASSED;
    /* add 'and' when chaining condition */
    if(*offset > 0) condition_write_format(buf, offset, " and ");
    switch(get_var(tree)) {
        case 1:     /* refine rate */
        case 2:     /* refine rate */
        case 3:     /* paramater */
        case 4:     /* skill level */
        case 6:     /* pow */
        case 10:    /* gettime */
        case 17:    /* job level */
        case 18:    /* base level */
        case 19:    /* max hp */
        case 23:    /* hp */
        case 24:    /* zeny */
        case 26:    /* callfunc (noncondition) */
        case 27:    /* getrandgroupitem (noncondition) */
        case 30:    /* countitem */
        case 46:    /* max */
        case 47:    /* min */
        case 49:    /* groupranditem (noncondition) */
            ret = condition_write_range(buf, offset, tree->range, tree->name); break;
        case 5:     /* random */
        case 28:    /* getpartnerid */
        case 31:    /* checkoption */
        case 32:    /* checkfalcon */
        case 33:    /* checkmadogear */
        case 34:    /* rebirth */
        case 48:    /* checkmount */
            ret = condition_write_format(buf, offset, "%s", tree->name); break;
        case 13:    /* isequipped */
            ret = condition_write_format(buf, offset, "%s Equipped", tree->name); break;
        case 20:    /* base class */
        case 21:    /* base job */
        case 22:    /* class */
            ret = condition_write_class(buf, offset, tree->range, tree->name); break;
        case 9:     /* getequipid */
            ret = condition_write_item(buf, offset, tree->range, block);
            ret = condition_write_format(buf, offset,"Equipped");
            break;
        case 8:     /* getiteminfo */
            ret = condition_write_getiteminfo(buf, offset, tree); break;
        case 29:    /* strcharinfo */
            ret = condition_write_strcharinfo(buf, offset, tree, block); break;
        default: break;
    }
    return ret;
}

int condition_write_strcharinfo(char * buf, int * offset, logic_node_t * tree, block_r * block) {
    map_res map_info;
    int val_min = minrange(tree->range);
    switch(get_func(tree)) {
        case 3: /* maps */
            if(map_id(block->db, &map_info, val_min)) {
                exit_func_safe("failed to search for map id"
                " %d in item %d", val_min, block->item_id);
                return SCRIPT_FAILED;
            }
            condition_write_format(buf, offset, "On Map %s", map_info.name);
            break;
        default: condition_write_format(buf, offset, "%s", tree->name); break;
    }
    return SCRIPT_PASSED;
}

int condition_write_getiteminfo(char * buf, int * offset, logic_node_t * tree) {
    //int val_min = minrange(tree->range);
    //switch(get_func(tree)) {
    //    case 2: /* item type */
    //        condition_write_format(buf, offset,"%s Is A %s", tree->name, item_type_tbl(val_min));
    //        break;
    //    case 4: /* gender */
    //        switch(val_min) {
    //            case 0: condition_write_format(buf, offset,"%s Is A Female Only", tree->name); break;
    //            case 1: condition_write_format(buf, offset,"%s Is A Male Only", tree->name); break;
    //            case 2: condition_write_format(buf, offset,"%s Is A Unisex", tree->name); break;
    //            default: return SCRIPT_FAILED;
    //        }
    //        break;
    //    case 5: /* equip location */
    //        condition_write_format(buf, offset,"%s Is A %s", tree->name, loc_tbl(val_min));
    //        break;
    //    case 11: /* weapon type */
    //        condition_write_format(buf, offset,"%s Is A %s", tree->name, weapon_tbl(val_min));
    //        break;
    //    default:
    //        condition_write_range(buf, offset, tree->range, tree->name); break;
    //}
    return exit_func_safe("maintenance");
}

int condition_write_item(char * buf, int * offset, range_t * range, block_r * block) {
    int i = 0;
    item_t item;
    range_t * itr = range;
    /* iterate the range of item id */
    while(itr != NULL) {
        for(i = itr->min; i <= itr->max; i++) {
            /* search for item name from item id */
            memset(&item, 0, sizeof(item_t));
            if(item_id(block->db, &item, i)) {
                exit_func_safe("failed to search for item %d in %d", i, block->item_id);
                return SCRIPT_FAILED;
            }
            /* add the item name to the list of items */
            *offset += sprintf(buf + *offset,"%s, ", item.name);
            buf[*offset] = '\0';
        }
        itr = itr->next;
    }
    /* discard last comma */
    buf[(*offset) - 2] = ' ';
    (*offset) -= 1;
    return SCRIPT_PASSED;
}

int condition_write_class(char * buf, int * offset, range_t * range, char * template) {
    //int i = 0;
    //int val_min = 0;
    //int val_max = 0;
    //range_t * itrrange = NULL;
    //range_t * negrange = NULL;

    //if(exit_null_safe(4, buf, offset, range, template))
    //    return SCRIPT_FAILED;

    //val_min = minrange(range);
    //val_max = maxrange(range);

    ///* assume (!=) for mismatch min and max value in range
    // * because otherwise all class logic node have matching
    // * min and max. */
    //if(val_min != val_max) {
    //    negrange = notrange(range);
    //    itrrange = negrange;
    //} else {
    //    itrrange = range;
    //}

    ///* iterate through the range */
    //while(itrrange != NULL) {
    //    for(i = itrrange->min; i <= itrrange->max; i++) {
    //        *offset += sprintf(buf + *offset,"%s, ", job_tbl(i));
    //        buf[*offset] = '\0';
    //    }
    //    itrrange = itrrange->next;
    //}
    ///* discard last comma */
    //buf[(*offset) - 2] = ' ';
    //(*offset) -= 1;

    ///* append base class, job, or class at the end */
    //*offset += sprintf(buf + *offset,"%s",template);

    ///* free negate range */
    //if(negrange != NULL) freerange(negrange);
    return exit_func_safe("maintenance");
}

int condition_write_range(char * buf, int * offset, range_t * range, char * template) {
    int val_min = 0;
    int val_max = 0;
    if(exit_null_safe(3, buf, offset, template))
        return SCRIPT_FAILED;
    val_min = minrange(range);
    val_max = maxrange(range);
    *offset += (val_min != val_max) ?
        sprintf(buf + *offset, "%s %d ~ %d", template, val_min, val_max) :
        sprintf(buf + *offset, "%s %d", template, val_min);
    buf[*offset] = '\0';
    return SCRIPT_PASSED;
}

int condition_write_format(char * buf, int * offset, char * format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    *offset += vsprintf(buf + *offset, format, arg_list);
    buf[*offset] = '\0';
    va_end(arg_list);
    return SCRIPT_PASSED;
}

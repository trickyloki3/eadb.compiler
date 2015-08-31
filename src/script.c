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

/* parse function paramater list */


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

int check_loop_expression(script_t * script, char * expr, char * end) {
    token_r token;
    if(script_lexical(&token, expr)) {
        exit_func_safe("failed to tokenize '%s' for item id %d", expr, script->item_id);
        return SCRIPT_FAILED;
    }

    /* check whether ; semicolon exist */
    if(token.script_cnt <= 0) {
        exit_func_safe("missing '%s' in for loop for item id %d", end, script->item_id);
        return SCRIPT_FAILED;
    }

    /* check whether ; is the first token */
    return (strcmp(token.script_ptr[0], end) == 0) ? SCRIPT_FAILED : SCRIPT_PASSED;
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
                        "item id %d current token is %s", script->item_id, token[i+1]);
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
                        " in for-loop for item id %d", script->item_id);
                        return SCRIPT_FAILED;
                    }

                    /* parse the for-loop condition */
                    if(script_parse(token_list, &i, block, '\0', ';', FLAG_PARSE_ALL_FLAGS))
                        return SCRIPT_FAILED;

                    /* check whether the initialization is empty */
                    if(check_loop_expression(script, block->ptr[1], ";")) {
                        exit_func_safe("missing conditional expression"
                        " in for-loop for item id %d", script->item_id);
                        return SCRIPT_FAILED;
                    }

                    /* parse the variant to know whether it goes up or down */
                    if(script_parse(token_list, &i, block, '\0', ')', FLAG_PARSE_ALL_FLAGS))
                        return SCRIPT_FAILED;

                    if(check_loop_expression(script, block->ptr[2], ")")) {
                        exit_func_safe("missing variant expression"
                        " in for-loop for item id %d", script->item_id);
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
                        " in for-loop for item id %d", script->item_id);
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
                        "in item id %d", block->ptr[0], script->item_id);
                        return SCRIPT_FAILED;
                    }

                    if(list_tail(&script->block, &iterable_set_block)) {
                        exit_func_safe("failed to query set block '%s' "
                        "in item id %d", block->ptr[0], script->item_id);
                        return SCRIPT_FAILED;
                    }

                    /* create the variant set block */
                    if(script_extend_block(script, block->ptr[2], parent, &set)) {
                        exit_func_safe("failed to create direction set block '%s' "
                        "in item id %d", block->ptr[2], script->item_id);
                        return SCRIPT_FAILED;
                    }

                    if(list_tail(&script->block, &direction_set_block)) {
                        exit_func_safe("failed to query set block '%s' "
                        "in item id %d", block->ptr[2], script->item_id);
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
                        "in item id %d", subscript, script->item_id);
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
    if(block_cnt == 0) return SCRIPT_SKIP;
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
    if(script_analysis(script, &token, parent, var) == SCRIPT_FAILED) {
        exit_func_safe("failed to parse %s in item %d", subscript, script->item_id);
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
    if(script_parse(&token, &offset, block, ',',')', FLAG_PARSE_SKIP_LEVEL)) {
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
            case 8: ret = translate_heal(iter, iter->type); break;                                                  /* heal */
            case 9: ret = translate_heal(iter, iter->type); break;                                                  /* percentheal */
            case 10: ret = translate_heal(iter, iter->type); break;                                                 /* itemheal */
            case 11: ret = translate_skill_block(iter, iter->type); break;                                          /* skill */
            case 12: ret = translate_skill_block(iter, iter->type); break;                                          /* itemskill */
            case 13: ret = translate_skill_block(iter, iter->type); break;                                          /* unitskilluseid */
            case 14: ret = translate_status(iter, iter->type); break;                                               /* sc_start */
            case 15: ret = translate_status(iter, iter->type); break;                                               /* sc_start4 */
            case 16: ret = translate_status_end(iter); break;                                                       /* sc_end */
            case 17: ret = translate_verbitem(iter, "Receive"); break;                                              /* getitem */
            case 18: ret = translate_rentitem(iter); break;                                                         /* rentitem */
            case 19: ret = translate_verbitem(iter, "Remove"); break;                                               /* delitem */
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

    /* used for aggregating bonus fields by loop */
    static const int agg_cnt = 5;
    static const int agg_flag[5] = {
        BONUS_FLAG_AG_1, BONUS_FLAG_AG_2,
        BONUS_FLAG_AG_3, BONUS_FLAG_AG_4,
        BONUS_FLAG_AG_5
    };

    iter = script->block.head->next;
    if(iter == script->block.head) return SCRIPT_FAILED;
    for(; iter != script->block.head; iter = iter->next) {
        memset(total, 0, sizeof(int) * BONUS_SIZE);
        memset(offset, 0, sizeof(int) * BONUS_SIZE);
        cur_blk = iter;
        cur_blk_type = cur_blk->type;
        cur_blk_flag = cur_blk->bonus.flag;

        /* check block is a bonus block and not nullified and can be minimized */
        if(cur_blk_type > BONUS_ID_MAX || cur_blk_type == BLOCK_NULLIFIED || !(cur_blk_flag & 0x000C0000))
            continue;

        iter_sub = iter->next;
        for(; iter_sub != script->block.head; iter_sub = iter_sub->next) {
            cmp_blk = iter_sub;
            /* bonus must be in same enclosing block and bonus id must match. */
            if( cur_blk->bonus.id != cmp_blk->bonus.id || cur_blk->link != cmp_blk->link)
                continue;

            /* MINIZ - indicates that arguments must be checked
             * MINIS - indicates that only one single argument for the bonus and also an aggregate. */
            if(cur_blk_flag & BONUS_FLAG_MINIZ) {
                /* check all specified arguments in item_bonus.txt all arguments are process from left to right in
                 * translate_bonus, therefore indexing the same block->eng[i] yields the same argument order.
                 * bonus block supports only up to 5 arguments. */
                if( (cur_blk_flag & BONUS_FLAG_EQ_1 && ncs_strcmp(cur_blk->eng[0], cmp_blk->eng[0]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_2 && ncs_strcmp(cur_blk->eng[1], cmp_blk->eng[1]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_3 && ncs_strcmp(cur_blk->eng[2], cmp_blk->eng[2]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_4 && ncs_strcmp(cur_blk->eng[3], cmp_blk->eng[3]) != 0) ||
                    (cur_blk_flag & BONUS_FLAG_EQ_5 && ncs_strcmp(cur_blk->eng[4], cmp_blk->eng[4]) != 0))
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

            /* duplicate bonuses are typically errors */
            if(cur_blk_flag & BONUS_FLAG_NODUP) {
                if( (cur_blk_flag & BONUS_FLAG_AG_1 && ncs_strcmp(cur_blk->eng[0], cmp_blk->eng[0]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_2 && ncs_strcmp(cur_blk->eng[1], cmp_blk->eng[1]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_3 && ncs_strcmp(cur_blk->eng[2], cmp_blk->eng[2]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_4 && ncs_strcmp(cur_blk->eng[3], cmp_blk->eng[3]) == 0) ||
                    (cur_blk_flag & BONUS_FLAG_AG_5 && ncs_strcmp(cur_blk->eng[4], cmp_blk->eng[4]) == 0)) {
                    /* throw error and nullified duplicate block */
                    fprintf(stderr,"[warn]: bonus duplicated on item id %d; in "
                    "block %d and %d; see dump.txt.\n", cur_blk->item_id, i, j);
                    iter_sub->type = BLOCK_NULLIFIED;
                    continue;
                }
            }

            /* aggregate the bonus fields */
            for(j = 0; j < agg_cnt; j++) {
                if(cur_blk_flag & agg_flag[j]) {
                    offset[j] += (offset[j] <= 0) ?
                        sprintf(buffer[j] + offset[j],"%s, %s", cur_blk->eng[j], cmp_blk->eng[j]):
                        sprintf(buffer[j] + offset[j],", %s", cmp_blk->eng[j]);
                    total[j]++;
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
            /* nullified block mean they're deleted */
            case BLOCK_NULLIFIED: break;
            /* merge all sc_end into one block */
            case 16:
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
                            if(iter_sub->next != script->block.head) {
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
                    if(script_generate_cond(iter->logic_tree, stdout, buf, buffer, offset, iter))
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

int script_generate_combo(int item_id, char * buffer, int * offset, db_t * db, int mode) {
    char * item_desc = NULL;
    combo_t * item_combo_list = NULL;
    combo_t * item_combo_iter = NULL;

    /* eathena does not have item combos */
    if(EATHENA == mode)
        return CHECK_PASSED;

    /* search the item id for combos */
    if(!item_combo_id(db, &item_combo_list, item_id)) {
        item_combo_iter = item_combo_list;
        while(item_combo_iter != NULL) {
            *offset += sprintf(buffer + *offset, "%s\n", item_combo_iter->group);
            /* compile each script write to buffer */
            item_desc = script_compile(item_combo_iter->script, item_id, db, mode);
            if(item_desc != NULL) {
                *offset += sprintf(buffer + *offset, "%s", item_desc);
                free(item_desc);
            }
            item_combo_iter = item_combo_iter->next;
        }
        buffer[*offset] = '\0';
        /* free the list of item combo */
        if(item_combo_list != NULL)
            item_combo_free(&item_combo_list);
    }
    return SCRIPT_PASSED;
}

char * script_compile_raw(char * subscript, int item_id, FILE * dbg, db_t * db, int mode) {
	char * str = convert_string(subscript);
    char buffer[BUF_SIZE];
    int offset = 0;
    script_t script;
    memset(&script, 0, sizeof(script_t));
    script.mode = mode;
    script.db = db;
    script.item_id = item_id;
    list_init_head(&script.block);
    /* compilation process per script */
    if(!script_lexical(&script.token, str))
        if(!script_analysis(&script, &script.token, NULL, NULL))
            if(!script_translate(&script))
                if(!script_bonus(&script))
                    if(!script_generate(&script, buffer, &offset))
                        ;
	free(str);
    script_block_reset(&script);
    script_block_finalize(&script);
    return (offset <= 0) ? NULL : convert_string(buffer);
}

/* translate function argument list and push arguments onto the stack */
int stack_ptr_call(block_r * block, char * expr) {
	int i = 0;
	int len = 0;
	int pos = 0;
	int cnt = 0;
	token_r * token = NULL;

	/* check empty argument list */
	len = strlen(expr);
	if (len <= 0) {
		exit_func_safe("empty argument list in item %d", block->item_id);
		goto failed;
	}

	/* tokenize argument list */
	token = calloc(1, sizeof(token_r));
	if (token == NULL) {
		exit_func_safe("out of memory in item %d", block->item_id);
		goto failed;
	}
	if (script_lexical(token, expr) || token->script_cnt <= 0) {
		exit_func_safe("failed to tokenize '%s' in item %d", block->item_id);
		goto failed;
	}

	/* first token must be a '(' */
	if (token->script_ptr[0][0] != '(') {
		exit_func_safe("missing initial '(' in '%s' in item %d", expr, block->item_id);
		goto failed;
	}

	/* remove the last ')' from parsing */
	token->script_cnt--;

	/* script analysis sets up the initial argument pointers for script parse,
	 * but we can set it again just in case. redundant, but not much overhead. */
	cnt = block->ptr_cnt;
	block->ptr[cnt] = &block->arg[block->arg_cnt];
	block->ptr_cnt++;

	/* parser will parse and place all results on the argument stack */
	script_parse(token, &pos, block, ',', '\0', FLAG_PARSE_NORMAL);

	/* BANDAID
	 * the last argument parsed was not ended on a delimiter, i.e. ',', the
	 * loop exited on the end of the string, which means that the parser did
	 * not properly set the next argument pointer to write the NULL character */
	block->arg[block->arg_cnt - 1] = '\0';

	/* release the resources */
	free(token);
	return block->ptr_cnt - cnt;

failed:
	if (token != NULL)
		free(token);
	return -1;
}

/* translate item id or name expression and push item names onto the stack */
int stack_eng_item(block_r * block, char * expr) {
	int i = 0;
	int cnt = 0;
	int len = 0;
	item_t * item_sql = NULL;
	node_t * item_ids = NULL;
	range_t * iter = NULL;

	/* check empty item id or name */
	len = strlen(expr);
	if (len <= 0) {
		exit_func_safe("empty item id or name in item %d", block->item_id);
		goto failed;
	}

	/* get item search buffer */
	item_sql = malloc(sizeof(item_t));
	if (item_sql == NULL) {
		exit_func_safe("out of memory in item %d", block->item_id);
		goto failed;
	}

	/* if the expression is an item name, then write the item name on the eng stack */
	if (isalpha(expr[0])) {
		if (!item_name(block->db, item_sql, expr, strlen(expr))) {
			if (script_buffer_write(block, TYPE_ENG, item_sql->name))
				goto failed;
			free(item_sql);
			return 1;
		}
	}

	/* evaluate the expression for item id values */
    item_ids = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
	if (item_ids == NULL || item_ids->range == NULL) {
		goto failed;
	} else {
		script_buffer_unwrite(block, TYPE_ENG);
	}

	/* write an item name for each item id on the eng stack */
	iter = item_ids->range;
	while (iter != NULL) {
		for (i = iter->min; i <= iter->max; i++) {
			if (item_id(block->db, item_sql, i)) {
				exit_func_safe("item id %d does not exist in item %d", i, block->item_id);
				goto failed;
			}
			if (script_buffer_write(block, TYPE_ENG, item_sql->name))
				goto failed;
			cnt++;
		}
		iter = iter->next;
	}

	/* release resources */
	free(item_sql);
	node_free(item_ids);
	return cnt;

failed:
	if (item_sql != NULL)
		free(item_sql);
	if (item_ids != NULL)
		node_free(item_ids);
	return -1;
}

/* translate integer expression and push an integer + formula onto the stack */
int stack_eng_int(block_r * block, char * expr) {
	int flag = 0;
	node_t * result = NULL;

	/* push the integer result on the stack */
	flag = EVALUATE_FLAG_KEEP_NODE | EVALUATE_FLAG_WRITE_FORMULA;
	result = evaluate_expression(block, expr, 1, flag);
	if (result == NULL) {
		exit_func_safe("expected an integer result in '%s' in item %d", expr, block->item_id);
		return -1;
	}

	node_free(result);
	return 1;
}

int stack_eng_time(block_r * block, char * expr) {
	node_t * time = NULL;
	int tick_min = 0;
	int tick_max = 0;
	int time_unit = 0;
	char * time_suffix = NULL;
	char * formula = NULL;

	/* maximum 32-bit integer require up to 11 characters plus -
	* enough padding room for additional suffix and formatting
	* characters. */
	int len = 0;
	char buf[64];

	/* check null */
	if (exit_null_safe(2, block, expr))
		return SCRIPT_FAILED;

	/* evaluate the expression and convert to time string */
	time = evaluate_expression(block, expr, 1, EVALUATE_FLAG_KEEP_NODE);
	if (time == NULL)
		return SCRIPT_FAILED;

	/* get the minimum and maximum of the time expression */
	tick_min = minrange(time->range);
	tick_max = maxrange(time->range);

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

	/* get time expression formula */
	if (script_buffer_write(block, TYPE_ENG, buf) ||
		script_formula_write(block, block->eng_cnt - 1, time, &formula) ||
		formula == NULL)
		goto failed;

	/* pop results and write final translated string */
	if (script_buffer_unwrite(block, TYPE_ENG) ||
		script_buffer_unwrite(block, TYPE_ENG) ||
		script_buffer_write(block, TYPE_ENG, formula))
		goto failed;

	/* reap the memory */
	free(formula);
	node_free(time);
	return 1;

failed:
	if (formula != NULL)
		free(formula);
	if (time != NULL)
		node_free(time);
	return -1;
}

int translate_verbitem(block_r * block, char * action) {
	int i = 0;
	int off = 0;
	int size = 0;
	int item_off = 0;
	int item_len = 0;
	char * buf = NULL;
	if (block->ptr_cnt == 1) {
		if (stack_ptr_call(block, block->ptr[0]) < 2)
			return exit_func_safe("missing getitem or delitem arguments for item %d", block->item_id);
		item_off = block->eng_cnt;
		item_len = stack_eng_item(block, block->ptr[1]);
		if (item_len < 0)
			goto failed;
		if(stack_eng_int(block, block->ptr[2]) < 0)
			goto failed;
	} else {
		if (block->ptr_cnt < 2)
			exit_func_safe("missing getitem or delitem arguments for item %d", block->item_id);
		item_off = block->eng_cnt;
		item_len = stack_eng_item(block, block->ptr[0]);
		if (item_len < 0)
			goto failed;
		if (stack_eng_int(block, block->ptr[1]) < 0)
			goto failed;
	}

	/* item names are push onto the stack in sequence
	 * and the translated pointers point to the same
	 * buffer, therefore the total length of all the
	 * items could be can be calculate from the diff
	 * of the first and last translated pointers. */
	size = block->eng[item_off + item_len] - block->eng[item_off];
	if (size <= 0) {
		exit_func_safe("empty item names for item %d", block->item_id);
		goto failed;
	}

	/* allocate a buffer with enough padding */
	buf = calloc(size + 128, sizeof(char));
	if (buf == NULL) {
		exit_func_safe("out of memory in item %d", block->item_id);
		goto failed;
	}

	/* write the getitem format */
	off += sprintf(&buf[off], "%s %s %s%s",
		action,
		block->eng[item_off + item_len],
		block->eng[item_off],
		(item_len > 1) ? ", " : "");
	for (i = item_off + 1; i < item_len; i++)
		off += (i + 1 == item_len) ?
			sprintf(&buf[off], "and %s", block->eng[i]) :
			sprintf(&buf[off], "%s, ", block->eng[i]);
	off += sprintf(&buf[off], ".");

	/* write to the end of the translation */
	if (script_buffer_write(block, TYPE_ENG, buf))
		goto failed;

	free(buf);
	return SCRIPT_PASSED;

failed:
	if (buf != NULL)
		free(buf);
	return SCRIPT_FAILED;
}

int translate_rentitem(block_r * block) {
	int i = 0;
	int off = 0;
	int size = 0;
	int item_off = 0;
	int item_len = 0;
	char * buf = NULL;
	if (block->ptr_cnt == 1) {
		if (stack_ptr_call(block, block->ptr[0]) < 2)
			return exit_func_safe("missing rentitem arguments for item %d", block->item_id);
		item_off = block->eng_cnt;
		item_len = stack_eng_item(block, block->ptr[1]);
		if (item_len < 0)
			goto failed;
		if (stack_eng_time(block, block->ptr[2]) < 1)
			goto failed;
	}
	else {
		if (block->ptr_cnt < 2)
			exit_func_safe("missing rentitem arguments for item %d", block->item_id);
		item_off = block->eng_cnt;
		item_len = stack_eng_item(block, block->ptr[0]);
		if (item_len < 0)
			goto failed;
		if (stack_eng_time(block, block->ptr[1]) < 1)
			goto failed;
	}

	/* item names are push onto the stack in sequence
	 * and the translated pointers point to the same
	 * buffer, therefore the total length of all the
	 * items could be can be calculate from the diff
	 * of the first and last translated pointers. */
	size = block->eng[item_off + item_len] - block->eng[item_off];
	if (size <= 0) {
		exit_func_safe("empty item names for item %d", block->item_id);
		goto failed;
	}

	/* allocate a buffer with enough padding */
	buf = calloc(size + 128, sizeof(char));
	if (buf == NULL) {
		exit_func_safe("out of memory in item %d", block->item_id);
		goto failed;
	}

	/* write the getitem format */
	off += sprintf(&buf[off], "Rent a %s %s%s",
		aeiou(block->eng[item_off][0]) ? "a" : "an",
		block->eng[item_off],
		(item_len > 1) ? ", " : "");
	for (i = item_off + 1; i < item_len; i++)
		off += (i + 1 == item_len) ?
		sprintf(&buf[off], "and %s", block->eng[i]) :
		sprintf(&buf[off], "%s, ", block->eng[i]);
	off += sprintf(&buf[off], " for %s.", block->eng[item_off + item_len]);

	/* write to the end of the translation */
	if (script_buffer_write(block, TYPE_ENG, buf))
		goto failed;

	free(buf);
	return SCRIPT_PASSED;

failed:
	return SCRIPT_FAILED;
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
		stack_eng_time(block, block->ptr[1]))
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
    pet_t pet;
    node_t * pet_ids = NULL;
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
    pet_ids = evaluate_expression(block, block->ptr[0], 1, EVALUATE_FLAG_KEEP_NODE);
    if(pet_ids == NULL || pet_ids->min != pet_ids->max) {
        if(pet_ids != NULL) node_free(pet_ids);
        exit_func_safe("failed to search for pet id or pet id is not constant"
        " %d ~ %d in item %d\n", pet_ids->min, pet_ids->max, block->item_id);
        return SCRIPT_FAILED;
    }

    /* search for pet and loyal script using pet id */
    memset(&pet, 0, sizeof(pet_t));
    if(pet_id(block->db, &pet, pet_ids->min)) {
        node_free(pet_ids);
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
        pet_script = script_compile(pet.pet_script, block->item_id, block->db, block->mode);
        if(pet_script == NULL) {
            node_free(pet_ids);
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
        loyal_script = script_compile(pet.loyal_script, block->item_id, block->db, block->mode);
        if(loyal_script == NULL) {
            node_free(pet_ids);
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
    if(pet_ids != NULL) node_free(pet_ids);
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
			stack_eng_time(block, block->ptr[1]))
            return SCRIPT_FAILED;
        offset = sprintf(buf, "Transform into a %s for %s.", block->eng[0], block->eng[2]);
    } else {
        if(translate_write(block, block->ptr[0], 1) ||
			stack_eng_time(block, block->ptr[1]))
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
            (!CHECK_ZERO(block->eng[0])?"\n":""),
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
    if(bonus_name(block->db, &block->bonus, bonus, strlen(bonus), block->ptr[0], strlen(block->ptr[0]))) {
        exit_func_safe("failed to search for bonus '%s %s' for item %d", bonus, block->ptr[0], block->item_id);
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
            case 'j': ret = translate_const(block, block->ptr[j], 0x10 | 0x40);                 break; /* Class Group & Monster */
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
    char * tbl_str = "error";
    mob_t mob;
    const_t const_info;
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;

    if(isdigit(expr[0])) {
        tbl_index = convert_integer(expr,10);
    } else {
        if(const_name(block->db, &const_info, expr, strlen(expr))) {
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
    /* 0x02 is for item group id; simplified */
    if(flag & 0x20)
        tbl_str = "a box"; /* lol */
    if(flag & 0x40 && strcmp(tbl_str, "error") == 0)
        tbl_str = (mob_id(block->db, &mob, tbl_index) == CHECK_PASSED) ? mob.name : "error";

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
    skill_t skill_info;
    memset(&skill_info, 0, sizeof(skill_t));
    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;
    if(((isdigit(expr[0]))) ?
        skill_id(block->db, &skill_info, convert_integer(expr, 10)):
        skill_name(block->db, &skill_info, expr, strlen(expr))) {
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
    block->eng_cnt--;    /* remove value from stack */
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

int translate_id(block_r * block, char * expr, int flag) {
    int id = 0;
    merc_t merc;
    mob_t mob;
    item_t item;
    memset(&merc, 0, sizeof(merc_t));
    memset(&mob, 0, sizeof(mob_t));
    memset(&item, 0, sizeof(item_t));

    /* check null paramater */
    if(exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;

    /* check if integer */
    if(!isdigit(expr[0]))
        return exit_func_safe("invalid id %s for %d", expr, block->item_id);

    id = convert_integer(expr, 10);
    if(flag & 0x01 && !mob_id(block->db, &mob, id)) {
        translate_write(block, mob.name, 0x01);
        return SCRIPT_PASSED;
    }

    if(flag & 0x02 && !item_id(block->db, &item, id)) {
        translate_write(block, item.name, 0x01);
        return SCRIPT_PASSED;
    }

    if(flag & 0x04 && !merc_id(block->db, &merc, id)) {
        translate_write(block, merc.name, 0x01);
        return SCRIPT_PASSED;
    }
    /* last resort assuming that it could be a constant */
    if(translate_const(block, expr, 0xBF) == SCRIPT_PASSED)
        return SCRIPT_PASSED;

    exit_func_safe("failed to resolve id %s for %d", expr, block->item_id);
    return SCRIPT_FAILED;
}

int translate_item(block_r * block, char * expr) {
    item_t item;
    /* check null */
	if (exit_null_safe(2, block, expr))
        return SCRIPT_FAILED;
    /* search the item id if literal */
    if(isdigit(expr[0]))
        return translate_id(block, expr, 0x02);
    /* search the const id if identifer */
    else if(isalpha(expr[0])) {
        memset(&item, 0, sizeof(item_t));
        if(item_name(block->db, &item, expr, strlen(expr)))
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
	if (stack_eng_time(block, block->ptr[2]) == SCRIPT_FAILED)
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
    produce_t * produce = NULL;
    produce_t * iter = NULL;
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
    if(produce_id(block->db, &produce, item_lv)) {
        node_free(result);
        return SCRIPT_FAILED;
    }

    iter = produce;
    while(iter != NULL) {
            sprintf(num,"%d",iter->item_id);
            translate_id(block, num, 0x02);
            offset += sprintf(buf + offset,"Recipe: %s\n", block->eng[recipe_offset++]);
            for(j = 0; j < iter->ingredient_count; j++) {
                sprintf(num,"%d",iter->item_id_req[j]);
                translate_id(block, num, 0x02);
                offset += sprintf(buf + offset," - %s [%d]\n", block->eng[recipe_offset++], iter->item_amount_req[j]);
            }
        iter = iter->next;
    }
    produce_free(&produce);
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
    item_t item;
    status_res status;
    const_t const_info;
    node_t * result[BONUS_SIZE];
    node_t * rate = NULL;
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;
    memset(&item, 0, sizeof(item_t));
    memset(&status, 0, sizeof(status_res));
    memset(&result, 0, sizeof(node_t *) * BONUS_SIZE);

    if(const_name(block->db, &const_info, block->ptr[0], strlen(block->ptr[0]))) {
        exit_func_safe("failed to search const %s in item %d", block->ptr[0], block->item_id);
        return SCRIPT_FAILED;
    }

    if(status_name(block->db, &status, const_info.value, block->ptr[0], strlen(block->ptr[0]))) {
        if(block->mode == HERCULE) {
            /* Hercules decided to change the status constants by + or - 1; what the fuck? */
            if(status_name(block->db, &status, const_info.value - 1, block->ptr[0], strlen(block->ptr[0])))
                if(status_name(block->db, &status, const_info.value + 1, block->ptr[0], strlen(block->ptr[0]))) {
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
	if (stack_eng_time(block, block->ptr[1]))
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
        if(item_id(block->db, &item, convert_integer(block->ptr[2], 10))) {
            exit_func_safe("failed to search item id %d in item %s (sc_itemscript)", block->item_id, block->ptr[2]);
            return SCRIPT_FAILED;
        }
        item_script = script_compile(item.script, item.id, block->db, block->mode);
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
    status_res status;
    const_t const_info;
    memset(&status, 0, sizeof(status_res));
    /* check null paramater */
    if(exit_null_safe(1, block))
        return SCRIPT_FAILED;

    if(const_name(block->db, &const_info, block->ptr[0], strlen(block->ptr[0]))) {
        exit_func_safe("failed to search const %s in "
            "item %d", block->ptr[0], block->item_id);
        return SCRIPT_FAILED;
    }

    if(status_name(block->db, &status, const_info.value, block->ptr[0], strlen(block->ptr[0]))) {
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

/* deprecate */
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

void var_write(node_t * node, char * fmt, ...) {
    if(exit_null_safe(2, node, fmt)) return;
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

	/* push the values on the translation stack */
	if (script_buffer_write(block, TYPE_ENG, buf))
		return NULL;

	/* write formula if dependencies exist and only when flag is set */
	if (EVALUATE_FLAG_WRITE_FORMULA & flag && root_node->cond_cnt > 0) {
		if (script_formula_write(block, block->eng_cnt - 1, root_node, &formula) || formula == NULL) {
			/* unwrite the values and report error */
			if (script_buffer_unwrite(block, TYPE_ENG))
				goto failed;
			exit_func_safe("failed to write formula in item %d", block->item_id);
			goto failed;
		}
		/* unwrite the formula and write the formula */
		if (script_buffer_unwrite(block, TYPE_ENG) ||
			script_buffer_write(block, TYPE_ENG, formula))
			goto failed;
		free(formula);
		formula = NULL;
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
	if (formula != NULL)
		free(formula);
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

                    /* static function value */
                    if(var_info.fflag & FUNC_DIRECT) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                    /* dynamic function value */
                    } else if(var_info.fflag & FUNC_PARAMS) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                        if( evaluate_function(block, expr, expr[expr_inx_open],
                            expr_inx_open + 2, expr_inx_close, temp_node) == SCRIPT_FAILED) {
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

int evaluate_function(block_r * block, char ** expr, char * func, int start, int end, node_t * node) {
    int i = 0;
    int index = 0;
    char * name = NULL;

    /* operand variables */
    node_t * resultOne = NULL;
    node_t * resultTwo = NULL;

    /* token variable to handle special argument */
    token_r token;
    skill_t skill;
    item_t item;
    const_t const_info;
    option_res option;

    if(ncs_strcmp(func,"getskilllv") == 0) {
        /* search by skill id or skill constant for max level */
        memset(&skill, 0, sizeof(skill_t));
        if(skill_name(block->db, &skill, expr[start], strlen(expr[start])))
            if(isdigit(expr[start][0]) && skill_id(block->db, &skill, convert_integer(expr[start], 10))) {
                exit_func_safe("failed to search for skill %s in %d", expr[start], block->item_id);
                return SCRIPT_FAILED;
            }
        /* write the skill name */
        set_func(node, skill.id);
        expression_write(node, "%s Level", skill.desc);
        id_write(node, "%s Level", skill.desc);
        /* skill level minimum is 0 (not learnt) or maximum level */
        node->min = 0;
        node->max = skill.maxlv;
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
            node->min = (int) pow(resultOne->min, resultTwo->min);
            node->max = (int) pow(resultOne->max, resultTwo->max);
            /*expression_write(node, "(%s)^%s", resultOne->formula, resultTwo->formula);*/
            id_write(node, "(%s)^%s", resultOne->formula, resultTwo->formula);
            node->type = NODE_TYPE_CONSTANT;
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
            node->min = (int) (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
            node->max = (int) (resultOne->max < resultTwo->max)?resultOne->max:resultTwo->max;
            expression_write(node, "Minimum(%s,%s)", resultOne->formula, resultTwo->formula);
            id_write(node, "Minimum(%s,%s)", resultOne->formula, resultTwo->formula);
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
            node->min = (int) (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
            node->max = (int) (resultOne->max < resultTwo->max)?resultOne->max:resultTwo->max;
            expression_write(node, "Maximum(%s,%s)", resultOne->formula, resultTwo->formula);
            id_write(node, "Maximum(%s,%s)", resultOne->formula, resultTwo->formula);
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
                node->min = (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
                node->max = (resultOne->max > resultTwo->max)?resultOne->max:resultTwo->max;
            }
        } else {
            node->min = resultOne->min;
            node->max = resultOne->max;
        }
        expression_write(node, "%s", "Random");
        id_write(node, "%s", "Random");
    /* callfunc calls global functions; just enumerate .. */
    } else if(ncs_strcmp(func,"callfunc") == 0) {
        block->flag = 0x01 | 0x02;
        /* the paramaters begin at a certain offset */
        if(ncs_strcmp(expr[start], "F_Rand") == 0) {
            block->offset = block->ptr_cnt;
            id_write(node, "%s", "Random Item");
            set_func(node, 1);
        } else if(ncs_strcmp(expr[start], "F_RandMes") == 0) {
            block->offset = block->ptr_cnt + 1;
            id_write(node, "%s", "Random");
            set_func(node, 2);
        } else {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }

        /* load the extended series onto the special buffer */
        for(i = 0; i < end + 1; i++) /* +1 include ) for parsing below */
            token.script_ptr[i] = expr[i];
        token.script_cnt = end + 1;

        /* parse the callfunc paramaters */
        if(script_parse(&token, &start, block, ',',')',FLAG_PARSE_SKIP_LEVEL) == SCRIPT_FAILED) {
            exit_func_safe("failed to evaluate function '%s' in item id %d\n", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        block->ptr_cnt--;
        node->min = node->max = 0;
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
        /* set the function tag */
        set_func(node, resultOne->min);
        switch(resultOne->min) {
            case 0: id_write(node, "Character"); break;
            case 1: id_write(node, "Party"); break;
            case 2: id_write(node, "Guild"); break;
            case 3: id_write(node, "Map"); break;
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
        memset(&item, 0, sizeof(item_t));
        if(item_id(block->db, &item, resultOne->min)) {
            exit_func_safe("failed to search for item %d in %d", resultOne->min, block->item_id);
            return SCRIPT_FAILED;
        }
        /* write the item name on the argument stack */
        set_func(node, resultOne->min);
        id_write(node, "%s Inventory Count", item.name);
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
        set_func(node, resultOne->min);
        switch(resultOne->min) {
            case 1: id_write(node, "Seconds"); node->min = 0; node->max = 60; break;
            case 2: id_write(node, "Minutes"); node->min = 0; node->max = 60; break;
            case 3: id_write(node, "Hours"); node->min = 0; node->max = 24; break;
            case 4: id_write(node, "Weeks"); node->min = 1; node->max = 7; break;
            case 5: id_write(node, "Day"); node->min = 0; node->max = 31; break;
            case 6: id_write(node, "Month");  node->min = 1; node->max = 12; break;
            case 7: id_write(node, "Year"); node->min = 0; node->max = 2020; break;
            case 8: id_write(node, "Day of Year"); node->min = 0; node->max = 365; break;
            default:
                exit_func_safe("invalid '%s' argument in item %d\n", expr, block->item_id);
                return SCRIPT_FAILED;
        }
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
        /* support getequipid as item_id argument */
        if(resultOne->cond != NULL && get_var(resultOne->cond) == 9) {
            name = resultOne->cond->name;
        } else {
            /* search for item name from item id */
            memset(&item, 0, sizeof(item_t));
            if(item_id(block->db, &item, resultOne->min)) {
                exit_func_safe("failed to search for item %d in %d", resultOne->min, block->item_id);
                return SCRIPT_FAILED;
            }
            name = item.name;
        }
        /* set the function tag */
        set_func(node, resultTwo->min);
        switch(resultTwo->min) {
            case 0: id_write(node, "%s's Buy Price", name); break;
            case 1: id_write(node, "%s's Sell Price", name); break;
            case 2: id_write(node, "%s's Item Type", name); break;
            case 3: id_write(node, "%s's Max Drop Chance", name); node->min = 0; node->max = 100; break;
            case 4: id_write(node, "%s's Gender", name); node->min = 0; node->max = 2; break;
            case 5: id_write(node, "%s's Equip Location", name); break;
            case 6: id_write(node, "%s's Weight", name); break;
            case 7: id_write(node, "%s's Attack", name); break;
            case 8: id_write(node, "%s's Defense", name); break;
            case 9: id_write(node, "%s's Range", name); node->min = 0; node->max = 16; break;
            case 10: id_write(node, "%s's Slot", name); node->min = 0; node->max = 4; break;
            case 11: id_write(node, "%s's Weapon Type", name); break;
            case 12: id_write(node, "%s's Requirement Level", name); node->min = 0; node->max = 15; break;
            case 13: id_write(node, "%s's Weapon Level", name); node->min = 0; node->max = 4; break;
            case 14: id_write(node, "%s's View ID", name); break;
            case 15: id_write(node, "%s's Magical Attack", name); break;
            default:
                exit_func_safe("invalid '%s' argument in item %d\n", expr, block->item_id);
                return SCRIPT_FAILED;
        }
    } else if(ncs_strcmp(func,"getequipid") == 0) {
        if(const_name(block->db, &const_info, expr[start], strlen(expr[start]))) {
            exit_func_safe("failed to search for const '%s' in %d", expr[start], block->item_id);
            return SCRIPT_FAILED;
        }
        set_func(node, const_info.value);
        switch(const_info.value) {
            case 1: id_write(node, "Upper Headgear"); break;
            case 2: id_write(node, "Armor"); break;
            case 3: id_write(node, "Left Hand"); break;
            case 4: id_write(node, "Right Hand"); break;
            case 5: id_write(node, "Garment"); break;
            case 6: id_write(node, "Shoes"); break;
            case 7: id_write(node, "Left Accessory"); break;
            case 8: id_write(node, "Right Accessory"); break;
            case 9: id_write(node, "Middle Headgear"); break;
            case 10: id_write(node, "Lower Headgear"); break;
            case 11: id_write(node, "Lower Costume Headgear"); break;
            case 12: id_write(node, "Middle Costume Headgear"); break;
            case 13: id_write(node, "Upper Costume Headgear"); break;
            case 14: id_write(node, "Costume Garment"); break;
            case 15: id_write(node, "Shadow Armor"); break;
            case 16: id_write(node, "Shadow Weapon"); break;
            case 17: id_write(node, "Shadow Shield"); break;
            case 18: id_write(node, "Shadow Shoes"); break;
            case 19: id_write(node, "Shadow Left Accessory"); break;
            case 20: id_write(node, "Shadow Right Accessory"); break;
        }
    } else if(ncs_strcmp(func,"isequipped") == 0) {
        node->id = &node->stack[node->stack_cnt];
        for(i = start; i < end; i++) {
            if(expr[i][0] != ',') {
                memset(&item, 0, sizeof(item_t));
                if(item_id(block->db, &item, convert_integer(expr[i], 10))) {
                    exit_func_safe("failed to search for item %s in %d", expr[i], block->item_id);
                    return SCRIPT_FAILED;
                }
                node->stack_cnt += sprintf(&node->stack[node->stack_cnt],"%s", item.name);
                set_func(node, get_func(node) + item.id);
            }
            node->stack[node->stack_cnt] = '\0';
        }
    } else if(ncs_strcmp(func,"groupranditem") == 0) {
        evaluate_groupranditem(block, expr, start, end, node);
    } else if(ncs_strcmp(func,"getequiprefinerycnt") == 0) {
        resultOne = evaluate_expression_recursive(block, expr, start, end, block->logic_tree, EVALUATE_FLAG_WRITE_FORMULA);
        if(resultOne == NULL) {
            exit_func_safe("failed to evaluate function '%s' in item id %d", expr, block->item_id);
            return SCRIPT_FAILED;
        }
        set_func(node, resultOne->min);
        switch(resultOne->min) {
            case 1: id_write(node, "%s's %s", "Upper Headgear", node->id); break;
            case 2: id_write(node, "%s's %s", "Armor", node->id); break;
            case 3: id_write(node, "%s's %s", "Left Hand", node->id); break;
            case 4: id_write(node, "%s's %s", "Right Hand", node->id); break;
            case 5: id_write(node, "%s's %s", "Garment", node->id); break;
            case 6: id_write(node, "%s's %s", "Shoes", node->id); break;
            case 7: id_write(node, "%s's %s", "Left Accessory", node->id); break;
            case 8: id_write(node, "%s's %s", "Right Accessory", node->id); break;
            case 9: id_write(node, "%s's %s", "Middle Headgear", node->id); break;
            case 10: id_write(node, "%s's %s", "Lower Headgear", node->id); break;
            case 11: id_write(node, "%s's %s", "Lower Costume Headgear", node->id); break;
            case 12: id_write(node, "%s's %s", "Middle Costume Headgear", node->id); break;
            case 13: id_write(node, "%s's %s", "Upper Costume Headgear", node->id); break;
            case 14: id_write(node, "%s's %s", "Costume Garment", node->id); break;
            case 15: id_write(node, "%s's %s", "Ammo", node->id); break;
            case 16: id_write(node, "%s's %s", "Shadow Armor", node->id); break;
            case 17: id_write(node, "%s's %s", "Shadow Weapon", node->id); break;
            case 18: id_write(node, "%s's %s", "Shadow Shield", node->id); break;
            case 19: id_write(node, "%s's %s", "Shadow Shoes", node->id); break;
            case 20: id_write(node, "%s's %s", "Shadow Right Accessory", node->id); break;
            case 21: id_write(node, "%s's %s", "Shadow left Accessory", node->id); break;
            default: id_write(node, expr[start]); break;
        }
    } else if(ncs_strcmp(func,"readparam") == 0) {
        if(const_name(block->db, &const_info, expr[start], strlen(expr[start]))) {
            exit_func_safe("failed to search for const '%s' in %d", expr[start], block->item_id);
            return SCRIPT_FAILED;
        }
        /* values correspond to the const.txt for rAthena */
        set_func(node, const_info.value);
        switch(const_info.value) {
            case 6: id_write(node, "Maximum HP"); break;
            case 8: id_write(node, "Maximum SP"); break;
            case 13: id_write(node, "Strength"); break;
            case 14: id_write(node, "Agility"); break;
            case 15: id_write(node, "Vitality"); break;
            case 16: id_write(node, "Intelligence"); break;
            case 17: id_write(node, "Dexterity"); break;
            case 18: id_write(node, "Luck"); break;
            case 41: id_write(node, "Base Attack"); break;
            case 42: id_write(node, "Weapon Attack"); break;
            case 45: id_write(node, "Base Defense"); break;
            case 46: id_write(node, "Armor Defense"); break;
            case 47: id_write(node, "Base Magical Defense"); break;
            case 48: id_write(node, "Armor Magical Defense"); break;
            case 49: id_write(node, "Hit"); break;
            case 50: id_write(node, "Base Flee"); break;
            case 51: id_write(node, "Armor Flee"); break;
            case 52: id_write(node, "Critical Rate"); break;
            case 53: id_write(node, "Attack Speed"); break;
            case 59: id_write(node, "Fame"); break;
            case 60: id_write(node, "Unbreakable"); break;
            default:
                exit_func_safe("invalid constant value for %s -> %d in item "
                    "%d\n", const_info.name, const_info.value, block->item_id);
                return SCRIPT_FAILED;
        }
    } else if(ncs_strcmp(func,"checkoption") == 0) {
        /* constants are not defined in const.txt, but specified
         * in the script command. See option_db.txt for mappings.*/
        memset(&option, 0, sizeof(option_res));
        if(opt_name(block->db, &option, expr[start], strlen(expr[start]))) {
            exit_func_safe("failed to search for option '%s' in %d", expr[start], block->item_id);
            return SCRIPT_FAILED;
        }
        id_write(node,"%s", option.name);
    } else {
        exit_func_safe("failed to search function handler for '%s' in item %d", func, block->item_id);
        return SCRIPT_FAILED;
    }

    if(resultOne != NULL) {
        node_write_recursive(resultOne, node);
        node_free(resultOne);
    }
    if(resultTwo != NULL) {
        node_write_recursive(resultTwo, node);
        node_free(resultTwo);
    }
    return SCRIPT_PASSED;
}

int evaluate_groupranditem(block_r * block, char ** expr, int start, int end, node_t * node) {
    int i = 0;

    for(i = start; i < end; i++)
        printf("%s ", expr[i]);
    printf("\n");

    return CHECK_PASSED;
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

int translate_bonus_desc(node_t ** result, block_r * block, bonus_res * bonus) {
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

    /*if(bonus->attr == 5) printf("%s\n", buffer);*/
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
    int val_min = minrange(tree->range);
    switch(get_func(tree)) {
        case 2: /* item type */
            condition_write_format(buf, offset,"%s Is A %s", tree->name, item_type_tbl(val_min));
            break;
        case 4: /* gender */
            switch(val_min) {
                case 0: condition_write_format(buf, offset,"%s Is A Female Only", tree->name); break;
                case 1: condition_write_format(buf, offset,"%s Is A Male Only", tree->name); break;
                case 2: condition_write_format(buf, offset,"%s Is A Unisex", tree->name); break;
                default: return SCRIPT_FAILED;
            }
            break;
        case 5: /* equip location */
            condition_write_format(buf, offset,"%s Is A %s", tree->name, loc_tbl(val_min));
            break;
        case 11: /* weapon type */
            condition_write_format(buf, offset,"%s Is A %s", tree->name, weapon_tbl(val_min));
            break;
        default:
            condition_write_range(buf, offset, tree->range, tree->name); break;
    }
    return SCRIPT_PASSED;
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
    int i = 0;
    int val_min = 0;
    int val_max = 0;
    range_t * itrrange = NULL;
    range_t * negrange = NULL;

    if(exit_null_safe(4, buf, offset, range, template))
        return SCRIPT_FAILED;

    val_min = minrange(range);
    val_max = maxrange(range);

    /* assume (!=) for mismatch min and max value in range
     * because otherwise all class logic node have matching
     * min and max. */
    if(val_min != val_max) {
        negrange = notrange(range);
        itrrange = negrange;
    } else {
        itrrange = range;
    }

    /* iterate through the range */
    while(itrrange != NULL) {
        for(i = itrrange->min; i <= itrrange->max; i++) {
            *offset += sprintf(buf + *offset,"%s, ", job_tbl(i));
            buf[*offset] = '\0';
        }
        itrrange = itrrange->next;
    }
    /* discard last comma */
    buf[(*offset) - 2] = ' ';
    (*offset) -= 1;

    /* append base class, job, or class at the end */
    *offset += sprintf(buf + *offset,"%s",template);

    /* free negate range */
    if(negrange != NULL) freerange(negrange);
    return SCRIPT_PASSED;
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

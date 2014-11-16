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
	for(i = 0; i < size; i++) {
		if(block[i].arg != NULL)
			free(block[i].arg);
		if(block[i].type != NULL) {
			free(block[i].type->keyword);
			free(block[i].type);
		}
		deepfreenamerange(block[i].logic_tree);
	}
	free(block);
}

int script_lexical(token_r * token, char * buffer) {
	/* script data */
	char script[BUF_SIZE];
	int script_len = 0;

	/* lexical data */
	int i = 0;
	char * script_buf = NULL;     /* script's buffer */
	char ** script_ptr = NULL;    /* script's string */
	int script_buf_cnt = 0;   /* script's buffer index */
	int script_ptr_cnt = 0;   /* script's string index */
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

	/* dump the token list */
	if(DEBUG_LEXICAL)
		for(i = 0; i < script_ptr_cnt; i++)
			printf("%s ", script_ptr[i]);

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
            block_init->type->keyword = info.keyword;
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

   /* readjust size when recursively parsing */
   *block_cnt = block_size - block_dep;
   return SCRIPT_PASSED;
}

int script_parse(token_r * token, int * position, block_r * block, char delimit, char end, int flag) {
	/* token reading data */
	int i, j;
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
	if(*position < 0 || *position >= script_cnt) return SCRIPT_FAILED;

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
					for(j = 0; j < strlen(script_ptr[i]); j++) 
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
		if(script_ptr[i][0] == ',' && !bracket_level && !subexpr_level && !(flag & 0x02)) continue;

		/* write the argument */
		for(j = 0; j < strlen(script_ptr[i]); j++) 
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
        switch(block[i].type->bk_id) {
            /*case 0: translate_bonus(&block[i], 0x01); break;                                                */                  /* bonus */
            /*case 1: translate_bonus(&block[i], 0x02); break;                                                */                  /* bonus2 */
            /*case 2: translate_bonus(&block[i], 0x04); break;                                                */                  /* bonus3 */
            /*case 3: translate_bonus(&block[i], 0x08); break;                                                */                  /* bonus4 */
            /*case 4: translate_bonus(&block[i], 0x10); break;                                                */                  /* bonus5 */
            /*case 5: translate_autobonus(&block[i], 0x01); break;                                            */                  /* autobonus */
            /*case 6: translate_autobonus(&block[i], 0x02); break;                                            */                  /* autobonus2 */
            /*case 7: translate_autobonus(&block[i], 0x04); break;                                            */                  /* autobonus3 */
            /*case 8: translate_generic(&block[i], 0x01, block[i].type->bk_id); break;                        */                  /* heal */
            /*case 9: translate_generic(&block[i], 0x01, block[i].type->bk_id); break;                        */                  /* percentheal */
            /*case 10: translate_generic(&block[i], 0x01, block[i].type->bk_id); break;                       */                  /* itemheal */
            /*case 11: translate_generic(&block[i], 0x02, block[i].type->bk_id); break;                       */                  /* skill */
            /*case 12: translate_generic(&block[i], 0x02, block[i].type->bk_id); break;                       */                  /* itemskill */
            /*case 13: translate_generic(&block[i], 0x02, block[i].type->bk_id); break;                       */                  /* unitskilluseid */
            /*case 14: translate_status(&block[i], block[i].type->bk_id); break;                              */                  /* sc_start */
            /*case 15: translate_status(&block[i], block[i].type->bk_id); break;                              */                  /* sc_start4 */
            /*case 16: translate_status_end(&block[i]); break;                                                */                  /* sc_end */
            /*case 17: translate_generic(&block[i], 0x04, block[i].type->bk_id); break;                       */                  /* getitem */
            /*case 18: translate_generic(&block[i], 0x08, block[i].type->bk_id); break;                       */                  /* rentitem */
            /*case 19: translate_generic(&block[i], 0x200, block[i].type->bk_id); break;                      */                  /* delitem */
            /*case 20: translate_generic(&block[i], 0x10, block[i].type->bk_id); break;                       */                  /* getrandgroupitem */
            /*case 23: translate_write(&block[i], "Set new font.", 1); break;                                 */                  /* setfont */
            /*case 24: translate_generic(&block[i], 0x100, block[i].type->bk_id); break;                      */                  /* buyingstore */
            /*case 25: translate_generic(&block[i], 0x100, block[i].type->bk_id); break;                      */                  /* searchstores */
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
                        break;                                                                                                                      /* else */
            /*case 28: evaluate_expression(&block[i], block[i].ptr[1], 1, EVALUATE_FLAG_KEEP_LOGIC_TREE); break;        *//* set */
            /*case 30: translate_write(&block[i], "Send a message through the announcement system.", 1); break;         *//* announce */
            /*case 31: translate_misc(&block[i], "callfunc"); break;                                                    *//* callfunc */
            /*case 33: translate_misc(&block[i], "warp"); break;                                                        *//* warp */
            /*case 34: translate_generic(&block[i], 0x40, block[i].type->bk_id); break;                                 *//* pet */
            /*case 35: translate_write(&block[i], "Hatch a pet egg.", 1); break;                                        *//* bpet */
            /*case 36: translate_generic(&block[i], 0x20, block[i].type->bk_id); break;                                 *//* mercenary_create */
            /*case 37: translate_generic(&block[i], 0x01, block[i].type->bk_id); break;                                 *//* mercenary_heal */
            /*case 38: translate_status(&block[i], block[i].type->bk_id);                                               *//* mercenary_sc_status */
            /*case 39: translate_produce(&block[i], block[i].type->bk_id); break;                                       *//* produce */
            /*case 40: translate_produce(&block[i], block[i].type->bk_id); break;                                       *//* cooking */
            /*case 41: translate_produce(&block[i], block[i].type->bk_id); break;                                       *//* makerune */
            /*case 42: translate_generic(&block[i], 0x80, block[i].type->bk_id); break;                                 *//* getguildexp */
            /*case 43: translate_generic(&block[i], 0x80, block[i].type->bk_id); break;                                 *//* getexp */
            /*case 44: translate_misc(&block[i], "monster"); break;                                                     *//* monster */
            /*case 45: translate_write(&block[i], "Evolve homunculus when requirements are met.", 1); break;            *//* homevolution */
            /*case 46: translate_write(&block[i], "Change to Summer Outfit when worn.", 1); break;                      *//* setoption */
            /*case 47: translate_write(&block[i], "Summon a creature to mount. [Work for all classes].", 1); break;     *//* setmounting */
            /*case 48: translate_write(&block[i], "Summon a falcon. [Also use to recall falcon].", 1); break;           *//* setfalcon */
            /*case 49: translate_generic(&block[i], 0x10, block[i].type->bk_id); break;                                 *//* getgroupitem */
            /*case 50: translate_write(&block[i], "Reset all status points.", 1); break;                                *//* resetstatus */
            /*case 52: translate_write(&block[i], "Play another background song.", 1); break;                           *//* playbgm */
            /*case 53: translate_transform(&block[i]); break;                                                           *//* transform */
            /*case 54: translate_status(&block[i], block[i].type->bk_id); break;                                        *//* sc_start2 */
            default: break;
        }
    }
    
    return SCRIPT_PASSED;
}

int translate_bonus(block_r * block, int flag) {
    int i, j;
    /* keep expression information to improve translation quality;
     * only integer type bonus arguments keep their node; these
     * nodes also keep track of variables use to calculation final
     * result. */
    node_t * result[BONUS_SIZE];  
    bonus_t bonus;
    memset(&bonus, 0, sizeof(bonus_t));
    memset(result, 0, sizeof(node_t *) * BONUS_SIZE);
    exit_null("bonus is null", 1, block);

    /* search the bonus database for the buff identifier */
    if(flag & 0x01) 
    	if(bonus_name_search(global_db, &bonus, "bonus", block->ptr[0]))
    		exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x02)
        if(bonus_name_search(global_db, &bonus, "bonus2", block->ptr[0]))
        	exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x04)
        if(bonus_name_search(global_db, &bonus, "bonus3", block->ptr[0]))
        	exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x08)
        if(bonus_name_search(global_db, &bonus, "bonus4", block->ptr[0]))
        	exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));
    if(flag & 0x10)
        if(bonus_name_search(global_db, &bonus, "bonus5", block->ptr[0]))
        	exit_abt(build_buffer(global_err,"failed to search for bonus %s for item %d", block->ptr[0], block->item_id));

    /* translate each argument individually 
        i = subscript the bonus type array
        j = offset and subscript the block argument array (ptr)
        j = 0; is the buff identifier
        j = 1; is where the arguments start
    */
    for(i = 0, j = 1; i < bonus.type_cnt; i++, j++) {
        switch(bonus.type[i]) {
            case 'n': result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE);     break; /* Integer Value */
            case 'p': result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE);     break; /* Integer Percentage */
            case 'r': translate_const(block, block->ptr[j], 0x01);                                           break; /* Race */
            case 'l': translate_const(block, block->ptr[j], 0x02);                                           break; /* Element */
            case 'w': translate_splash(block, block->ptr[j]);                                                break; /* Splash */
            case 'z': block->eng_cnt++;                                                                      break; /* Meaningless */
            case 'e': translate_const(block, block->ptr[j], 0x04);                                           break; /* Effect */
            case 'q': result[i] = evaluate_expression(block, block->ptr[j], 100, EVALUATE_FLAG_KEEP_NODE);   break; /* Integer Percentage / 100 */
            case 'k': translate_skill(block, block->ptr[j]);                                                 break; /* Skill */
            case 's': translate_const(block, block->ptr[j], 0x08);                                           break; /* Size */
            case 'c': translate_id(block, block->ptr[j], 0x01);                                              break; /* Monster Class & Job ID * Monster ID */
            case 'o': result[i] = evaluate_expression(block, block->ptr[j], 10, EVALUATE_FLAG_KEEP_NODE);    break; /* Integer Percentage / 10 */
            case 'm': translate_id(block, block->ptr[j], 0x02);                                              break; /* Item ID */
            case 'x': result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE);     break; /* Level */
            case 'g': translate_tbl(block, block->ptr[j], 0x01);                                             break; /* Regen */
            case 'a': result[i] = evaluate_expression(block, block->ptr[j], 1000, EVALUATE_FLAG_KEEP_NODE);  break; /* Millisecond */
            case 'h': result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE);     break; /* SP Gain Bool */
            case 'v': translate_tbl(block, block->ptr[j], 0x04);                                             break; /* Cast Self, Enemy */
            case 't': translate_trigger(block, block->ptr[j], 1);                                            break; /* Trigger */
            case 'y': translate_item(block, block->ptr[j]);                                                  break; /* Item Group */
            case 'd': translate_trigger(block, block->ptr[j], 2);                                            break; /* Triger ATF */
            case 'f': result[i] = evaluate_expression(block, block->ptr[j], 1, EVALUATE_FLAG_KEEP_NODE);     break; /* Cell */
            case 'b': translate_tbl(block, block->ptr[j], 0x08);                                             break; /* Flag Bitfield */
            case 'i': translate_tbl(block, block->ptr[j], 0x10);                                             break; /* Weapon Type */
            case 'j': translate_const(block, block->ptr[j], 0x10);                                           break; /* Class Group */
            default: break;
        }
    }

    /* the number of arguments must match the number of translations */
    if(block->ptr_cnt-1 != block->eng_cnt) exit_abt("failed to translate block");

    /* process the bonus block's integers and write translation */
    translate_bonus_desc(result, block, &bonus);

    /* free all the result nodes */
    for(i = 0; i < BONUS_SIZE; i++)
        if(result[i] != NULL)
            node_free(result[i]);

    if(bonus.pref != NULL) free(bonus.pref);
	if(bonus.buff != NULL) free(bonus.buff);
	if(bonus.desc != NULL) free(bonus.desc);
	if(bonus.type != NULL) free(bonus.type);
	if(bonus.order != NULL) free(bonus.order);
    return SCRIPT_PASSED;
}

int translate_const(block_r * block, char * expr, int flag) {
    int tbl_index = 0;
    char * tbl_str = NULL;
 
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
    else if(flag & 0x02)
        tbl_str = ele_tbl(tbl_index);
    else if(flag & 0x04)
        tbl_str = eff_tbl(tbl_index);
    else if(flag & 0x08)
        tbl_str = size_tbl(tbl_index);
    else if(flag & 0x10) 
        tbl_str = class_tbl(tbl_index);
    else if(flag & 0x20) 
        tbl_str = itemgrp_tbl(tbl_index);
 
    /* check for invalid index */
    if(tbl_str == NULL || strcmp(tbl_str,"error") == 0)
        exit_abt(build_buffer(global_err, "failed to resolve constant %s for %d", expr, block->item_id));
 
    /* write the tbl_str */
    translate_write(block, tbl_str, 0x01);
    if(const_info.name != NULL) free(const_info.name);
    return SCRIPT_PASSED;
}

int translate_skill(block_r * block, char * expr) {
    ic_skill_t skill_info;
    memset(&skill_info, 0, sizeof(ic_skill_t)); 
    exit_null("null paramater", 2, block, expr);

    if(((isdigit(expr[0]))) ?
    	skill_name_search_id(global_db, &skill_info, convert_integer(expr, 10), global_mode):
    	skill_name_search(global_db, &skill_info, expr, global_mode))
		exit_abt(build_buffer(global_err, "failed to resolve skill %s for %d", expr, block->item_id));

    /* write the skill name */
    translate_write(block, skill_info.desc, 0x01);
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
    exit_abt("invalid result");

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
    int tick = 0;
    char buf[256];
    exit_null("null paramater", 2, block, expr);
    /* evalute the expression and convert to time string */
    evaluate_expression(block, expr, 1, 0);
    tick = convert_integer(block->eng[block->eng_cnt - 1], 10);
    if((tick / 86400000) > 1) {                        /* Convert seconds to days */
        tick /= 86400000;
        sprintf(buf,"%d days",tick);
    } else if((tick / 3600000) > 1) {                /* Convert seconds to hours */
        tick /= 3600000;
        sprintf(buf,"%d hours",tick);
    } else if((tick / 60000) > 1) {                  /* Convert seconds to minutes */
        tick /= 60000;
        sprintf(buf,"%d minutes",tick);
    } else {                                                 /* Convert to seconds */
        tick /= 1000;
        sprintf(buf,"%d seconds",tick);
    }
    translate_write(block, buf, 1);
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

    if(mob_id_search(global_db, &mob, id, global_mode) && flag & 0x01) {
    	translate_write(block, mob.iro, 0x01);
    	return SCRIPT_PASSED;
    }

    if(item_name_id_search(global_db, &item, id, global_mode) && flag & 0x02) {
    	translate_write(block, item.name, 0x01);
    	return SCRIPT_PASSED;
    }

    if(merc_id_search(global_db, &merc, id, global_mode) && flag & 0x04) {
    	translate_write(block, merc.name, 0x01);
    	return SCRIPT_PASSED;
    }

    if(pet_id_search(global_db, &pet, id, global_mode) && flag & 0x08) {
    	translate_write(block, pet.pet_name, 0x01);
    	return SCRIPT_PASSED;
    }

    exit_abt(build_buffer(global_err, "failed to resolve id %s for %d", expr, block->item_id));
    return SCRIPT_FAILED;
}

int translate_item(block_r * block, char * expr) {
    exit_null("null paramater", 2, block, expr);

    /* search the item id if literal */
    if(isdigit(expr[0]))
        return translate_id(block, expr, 0x02);
    /* search the const id if identifer */
    else if(isalpha(expr[0]))
        return translate_const(block, expr, 0x20);
    else {
        exit_abt(build_buffer(global_err, "failed to resolve item or const id %s for %d", expr, block->item_id));
    }
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

void block_debug_dump(block_r * block, FILE * stm, char * msg) {
    /* display block type and argument */
    int j;
    fprintf(stm,"========================================\n");
    if(msg != NULL) fprintf(stm,"Message: %s\n", msg);
    fprintf(stm,"Item ID: %d\n", block->item_id);
    fprintf(stm,"Block Type: %s\n", block->type->bk_kywd);
    fprintf(stm,"Block ID: %d\n", block->block_id);
    fprintf(stm,"Block Link: %d\n", block->link);
    fprintf(stm,"Block Buffer Size: %d\n", block->arg_cnt);
    fprintf(stm,"Block Argument Count: %d\n", block->ptr_cnt);
    for(j = 0; j < block->ptr_cnt; j++)
        fprintf(stm,"Block Argument %d: %s\n", j, block->ptr[j]);
    fprintf(stm,"Block Translation Count: %d\n", block->eng_cnt);
    for(j = 0; j < block->eng_cnt; j++)
        fprintf(stm,"Block Translation %d: %s\n", j, block->eng[j]);
    if(block->desc != NULL) fprintf(stm,"Translation: %s\n", block->desc);
    /* Dumping the first 64 byte usually tells the story */
    fprintf(stm,"Buffer: ");
    for(j = 0; j < 255; j++) {
        if(block->arg_cnt == j) {
            fprintf(stm,">%c<", block->arg[j]);
            break;
        } else
            fprintf(stm,"%c", block->arg[j]);
    }
    fprintf(stm,"%c\n", block->arg[j]);
    fprintf(stm,"Block Flag: %d\n", block->flag);
    fprintf(stm,"Block Offset: %d\n", block->offset);
    /* Write out the logic tree */
    if(block->logic_tree != NULL) {
        fprintf(stm, "Block Logic Tree:\n");
        dmpnamerange(block->logic_tree, stm, 0);
    }
    fflush(stm);
}

void block_debug_dump_all(block_r * block, int size, FILE * stm) {
   /* display block type and argument */
    int i, j;
    for(i = 0; i < size; i++) {
        fprintf(stm,"========================================\n");
        fprintf(stm,"Block Address: %p\n", (void *) &block[i]);
        fprintf(stm,"Item ID: %d\n", block[i].item_id);
        fprintf(stm,"Block ID: %d\n", block[i].block_id);
        fprintf(stm,"Block Link: %d\n", block[i].link);
        fprintf(stm,"Block Type: %s\n", block[i].type->bk_kywd);
        fprintf(stm,"Block Buffer Size: %d\n", block[i].arg_cnt);
        fprintf(stm,"Block Argument Count: %d\n", block[i].ptr_cnt);
        for(j = 0; j < block[i].ptr_cnt; j++)
             fprintf(stm,"Block Argument %d: %s\n", j, block[i].ptr[j]);
        fprintf(stm,"Block Translation Count: %d\n", block[i].eng_cnt);
        for(j = 0; j < block[i].eng_cnt; j++)
           fprintf(stm,"Block Translation %d: %s\n", j, block[i].eng[j]);
        /* Dumping the first 64 byte usually tells the story */
        if(block[i].desc != NULL) fprintf(stm,"Translation: %s\n", block[i].desc);
        fprintf(stm,"Buffer: ");
        for(j = 0; j < 255; j++) {
            if(block[i].arg_cnt == j) {
                fprintf(stm,">%c<", block[i].arg[j]);
                break;
            } else
                fprintf(stm,"%c", block[i].arg[j]);
        }
        fprintf(stm,"%c\n", block[i].arg[j]);
  
        /* Write out the dependency tree */
        if(block[i].depd_cnt > 0) {
            fprintf(stm,"Block Dependence: ");
            block_debug_dump_depd_recurse(block[i].depd, block[i].depd_cnt, block[i].block_id, 0, stm);
        } else {
            fprintf(stm,"Block Dependence: N/A\n");
        }
        /* Write out the logic tree */
        if(block[i].logic_tree != NULL) {
            fprintf(stm, "Block Logic Tree:\n");
            deepdmpnamerange(block[i].logic_tree, stm, "logic tree on stack");
        }
    }
}

void block_debug_dump_depd_recurse(struct block_r ** block, int block_cnt, int block_id, int depth, FILE * stm) {
    int i, j;
    fprintf(stm,"\n");
    if(block_cnt > 0) {
        for(i = 0; i < block_cnt; i++) {
            for(j = 0; j < depth; j++) fprintf(stm,"\t");
            fprintf(stm,"%d -> %d ", block_id, block[i]->block_id);
            block_debug_dump_depd_recurse(block[i]->depd, block[i]->depd_cnt, block[i]->block_id, depth + 1,stm);
        }
    } else {
        for(j = 0; j < depth; j++) fprintf(stm,"\t");
        fprintf(stm,"%d ROOT\n", block_id);
    }
}

node_t * evaluate_expression(block_r * block, char * expr, int modifier, int flag) {
    node_t * root_node;

    token_r expr_token;
    int length;
    logic_node_t * temp = NULL;

    /* lexical analysis on expression and return failed if failed */
    if(script_lexical(&expr_token, expr) == SCRIPT_FAILED) return NULL;
    if(expr_token.script_cnt <= 0) return NULL;

    /* evaluate the expression */
    root_node = evaluate_expression_recursive(block, expr_token.script_ptr, 0, expr_token.script_cnt, block->logic_tree, flag);
    if(root_node == NULL) return NULL;

    /* write the result into argument translation */
    if(modifier != 0) {
        block->eng[block->eng_cnt] = &block->arg[block->arg_cnt];
        if(root_node->min == root_node->max)
            length = sprintf(&block->arg[block->arg_cnt],"%d",(root_node->min) / modifier);
        else
            length = sprintf(&block->arg[block->arg_cnt],"%d ~ %d",(root_node->min) / modifier, (root_node->max) / modifier);
      
        if(length <= 0) return NULL;

        /* extend the buffer to the next length */
        block->arg_cnt += length + 1;
        block->eng_cnt++;
    }

    /* add the min and max for set blocks */
    if(block->type->bk_id == 28) {
        block->set_min = root_node->min;
        block->set_max = root_node->max;
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

    /* make sure the evaluate expression has a node to return if needed */
    if(root_node == NULL) exit(EXIT_FAILURE);

    /* free ALL the nodes */
    if(!(flag & EVALUATE_FLAG_KEEP_NODE)) {
        node_free(root_node);
        root_node = NULL;
    }

    return root_node;
}

node_t * evaluate_expression_recursive(block_r * block, char ** expr, int start, int end, logic_node_t * logic_tree, int flag) {
    int i = 0;
    int j = 0;
    int len = 0;

    int expr_inx_open = 0;
    int expr_inx_close = 0;
    int expr_sub_level = 0;

    int op_cnt = 0;

    const_t const_info;
    var_t var_info;

    /* operator precedence tree */
    int op_pred[PRED_LEVEL_MAX][PRED_LEVEL_PER] = {
        {'*'          ,'/'          ,'\0' ,'\0'         ,'\0'},
        {'+'          ,'-'          ,'\0' ,'\0'         ,'\0'},
        {'<'          ,'<' + '='  ,'>'  ,'>' + '='  ,'\0'},
        {'&'          ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'|'          ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'=' + '='  ,'!' + '='  ,'\0' ,'\0'         ,'\0'},
        {'&' + '&'  ,'\0'         ,'\0' ,'\0'         ,'\0'},
        {'|' + '|'  ,'\0'         ,'\0' ,'\0'         ,'\0'},
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
    memset(&const_info, 0, sizeof(const_t));
    memset(&var_info, 0, sizeof(var_t));
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
            	exit_buf("lokic: failed interpreting item %d; unmatch parentheses.\n", block->item_id);
            temp_node = evaluate_expression_recursive(block, expr, expr_inx_open + 1, expr_inx_close, logic_tree, flag);
            temp_node->type = NODE_TYPE_SUB;    /* subexpression value is now a special operand */
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
            len = strlen(expr[i]);

            /* check if entirely numeric */
            for(j = 0; j < len; j++)
                if(!isdigit(expr[i][j]))
                	exit_buf("lokic: failed interpreting item %d; invalid operand %s.\n", block->item_id, expr[i]);

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
            if(var_keyword_search(global_db, &var_info, expr[i])) {
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
                        exit_buf("lokic: failed interpreting item %d; unmatch parentheses.\n", block->item_id);

                    /* create function node */
                    temp_node->type = NODE_TYPE_FUNCTION;

                    /* static function value */
                    if(var_info.fflag & FUNC_DIRECT) {
                        temp_node->min = var_info.min;
                        temp_node->max = var_info.max;
                    /* dynamic function value */
                    } else if(var_info.fflag & FUNC_PARAMS) {
                        evaluate_function(block, expr, expr[expr_inx_open], expr_inx_open + 2, expr_inx_close,
                                                &(temp_node->min), &(temp_node->max), temp_node);
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
            } else if(const_keyword_search(global_db, &const_info, expr[i], global_mode)) {
                /* create constant node */
                temp_node->type = NODE_TYPE_CONSTANT;
                temp_node->min = temp_node->max = const_info.value;

            /* handle set'd variable */
            } else {
                /* create set block node */
                temp_node->type = NODE_TYPE_LOCAL; /* assume string literal unless resolved in for loop */
                temp_node->min = temp_node->max = 0;
                /* loop through all the dependencies and apply the last matching min and max */
                for(j = 0; j < block->depd_cnt; j++) {
                    if(strcmp(block->depd[j]->ptr[0], expr[i]) == 0) {
                        temp_node->min = block->depd[j]->set_min;
                        temp_node->max = block->depd[j]->set_max;
                        if(block->depd[j]->logic_tree != NULL) {
                            if(temp_node != NULL) freenamerange(temp_node->cond);
                            temp_node->cond = copy_any_tree(block->depd[j]->logic_tree);
                        }
                    }
                }
            }
            op_cnt++;
        
        /* catch everything else */
        } else {
            exit_buf("lokic: failed interpreting item %d; invalid token %s.\n", block->item_id, expr[i]);
        }

        if(op_cnt > 1)
            exit_buf("lokic: failed interpreting item %d; operator without operand.\n", block->item_id, expr[i]);

        if(temp_node != NULL) {
            /* generate range for everything but subexpression and operators */
            if(temp_node->type & 0x3e)
                temp_node->range = mkrange(INIT_OPERATOR, temp_node->min, temp_node->max, DONT_CARE);

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
        exit_buf("lokic: failed interpreting item %d; detected zero nodes.\n", block->item_id, expr[i]);

    /* check that the list is completely linked */
    iter_node = root_node;
    do {
        if(iter_node->next == NULL || iter_node->prev == NULL)
            exit_buf("lokic: failed interpreting item %d; invalid node linking.\n", block->item_id, expr[i]);
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
    evaluate_node(root_node->next, NULL, logic_tree, flag);

    /* setup the subexpression or expression node */
    strncpy(root_node->expr, root_node->next->expr, strlen(root_node->next->expr));
    root_node->min = root_node->next->min;
    root_node->max = root_node->next->max;
    root_node->range = copyrange(root_node->next->range);
    
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
    if(const_info.name != NULL) free(const_info.name);
    if(var_info.id != NULL) free(var_info.id);
    return root_node;
}

int evaluate_function(block_r * block, char ** expr, char * func, int start, int end, int * min, int * max, node_t * node) {
    int i;
    int index = 0;

    /* parsing variables */
    int subexpr_level = 0;

    /* operand variables */
    node_t * resultOne = NULL;
    node_t * resultTwo = NULL;

    /* token variable to handle special argument */
    token_r token;
    ic_skill_t skill;
    ic_item_t item;
    const_t const_info;
    memset(&skill, 0, sizeof(ic_skill_t));
    memset(&item, 0, sizeof(ic_item_t));
    memset(&const_info, 0, sizeof(const_t));

    if(ncs_strcmp(func,"getskilllv") == 0) {
        /* search by skill id or skill constant for max level */
        if(skill_name_search(global_db, &skill, expr[start], global_mode) && isdigit(expr[start][0]))
        	if(skill_name_search_id(global_db, &skill, convert_integer(expr[start], 10), global_mode))
        		exit_abt(build_buffer(global_err, "failed to search for skill %s in %d", expr[start], block->item_id));

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
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, 0);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, 0);
        *min = (int) pow(resultOne->min, resultTwo->min);
        *max = (int) pow(resultOne->max, resultTwo->max);
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
        resultOne = evaluate_expression_recursive(block, expr, start, i, block->logic_tree, 0);
        resultTwo = evaluate_expression_recursive(block, expr, i + 1, end, block->logic_tree, 0);
        *min = (resultOne->min < resultTwo->min)?resultOne->min:resultTwo->min;
        *max = (resultOne->max > resultTwo->max)?resultOne->max:resultTwo->max;
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
    } else {
        fprintf(stderr,"warn[%d]: unable to search for handler for function %s.\n", block->item_id, func);
        return SCRIPT_FAILED;
    }

    if(resultOne != NULL) node_free(resultOne);
    if(resultTwo != NULL) node_free(resultTwo);
 	if(skill.name != NULL) free(skill.name);
 	if(skill.desc != NULL) free(skill.desc);
 	if(item.name != NULL) free(item.name);
 	if(const_info.name != NULL) free(const_info.name);
    return SCRIPT_PASSED;
}

void evaluate_node(node_t * node, FILE * stm, logic_node_t * logic_tree, int flag) {
    int i = 0;
    range_t * temp = NULL;
    range_t * result = NULL;
    logic_node_t * new_tree = NULL;

    /* support unary and binary operators */
    if(node->left == node->right) {
        if(node->left != NULL) evaluate_node(node->left, stm, logic_tree, flag);
    } else {
        /* support ?: flip the logic tree for one side */
        if(node->type == NODE_TYPE_OPERATOR && node->op == ':') {
            if(logic_tree != NULL) {
                new_tree = inverse_logic_tree(logic_tree);
                /* use the 'true' logic tree for left; 'false' logic tree for right */
                if(node->left != NULL) evaluate_node(node->left, stm, logic_tree, flag);
                if(node->right != NULL) evaluate_node(node->right, stm, new_tree, flag);
                freenamerange(new_tree);
            } else{
                evaluate_node(node->left, stm, logic_tree, flag);
                evaluate_node(node->right, stm, logic_tree, flag);
            }
        /* normal pre-order traversal */
        } else {
            if(node->left != NULL) evaluate_node(node->left, stm, logic_tree, flag);
            /* support ?: add logic tree on stack */
            if(node->type == NODE_TYPE_OPERATOR && node->op == '?') {
                /* check that logic tree exist for left side */
                if(node->left->cond != NULL) {
                    new_tree = node->left->cond;
                    new_tree->stack = logic_tree;
                    if(node->right != NULL) evaluate_node(node->right, stm, new_tree, flag);
                } else {
                    if(node->right != NULL) evaluate_node(node->right, stm, logic_tree, flag);
                }
            } else {
                if(node->right != NULL) evaluate_node(node->right, stm, logic_tree, flag);
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

        /* inherit logic node's function argument translations */
        if(node->cond != NULL && node->args_cnt > 0) {
            for(i = 0; i < node->args_cnt; i++)
                node->cond->args[i] = node->args[i];
            for(i = 0; i < node->args_ptr_cnt; i++)
                node->cond->args_ptr[i] = node->args_ptr[i];
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
                /*if(flag & EVALUATE_FLAG_EXPR_BOOL)*/
                    node->range = calcrange(node->op, node->left->range, node->right->range);
                /*else
                    node->range = mkrange(INIT_OPERATOR, 0, 1, 0);*/
                node_inherit_cond(node); 
                break;
            case '&': node->range = calcrange(node->op, node->left->range, node->right->range); node_inherit_cond(node); break;
            case '|': node->range = calcrange(node->op, node->left->range, node->right->range); node_inherit_cond(node); break;
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
            case ':': node->range = orrange(node->left->range, node->right->range); break;
            case '?': node->range = copyrange(node->right->range); /* right node is : operator */ break;
            default: 
                fprintf(stderr,"evaluate_node; supported operator %c\n", node->op);
                break;            
        }

        /* extract the local min and max from the range */
        node->min = minrange(node->range);
        node->max = maxrange(node->range);
    }

    /* write expression and print node */
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
        fprintf(stm,"     Node: %p\n", (void *) node);
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
        fprintf(stm,"  Minimum: %d\n", node->min);
        fprintf(stm,"  Maximum: %d\n", node->max);
        fprintf(stm,"  Arg_Cnt: %d\n", node->args_cnt);
        fprintf(stm,"  Arg_Ptr: %d\n", node->args_ptr_cnt);
        fprintf(stm,"  Maximum: %d\n", node->max);
        fprintf(stm,"   Parent: %p\n", (void *) node->parent);
        fprintf(stm,"     Left: %p\n", (void *) node->left);
        fprintf(stm,"    Right: %p\n", (void *) node->right);      
        fprintf(stm,"     Next: %p\n", (void *) node->next);
        fprintf(stm,"     Prev: %p\n", (void *) node->prev);
        fprintf(stm,"     Expr: %s\n", node->expr);
        dmprange(node->range, stdout, "node_dmp");
        dmpnamerange(node->cond, stdout, 0);
        fprintf(stm,"  --  \n");
    }
}

/* generate human readable string of the expression */
void node_expr_append(node_t * left, node_t * right, node_t * dest) {
    /* dest is a leave node */
    if(left == NULL && right == NULL) {
        switch(dest->type) {
            case NODE_TYPE_OPERATOR:
                switch(dest->op) {
                    case '!' + '=': sprintf(dest->expr,"!="); break;
                    case '=' + '=': sprintf(dest->expr,"=="); break;
                    case '<' + '=': sprintf(dest->expr,"<="); break;
                    case '>' + '=': sprintf(dest->expr,">="); break;
                    case '&' + '&': sprintf(dest->expr,"&&"); break;
                    case '|' + '|': sprintf(dest->expr,"||"); break;
                    default: sprintf(dest->expr,"%c", dest->op); break;
                }
                break;
            case NODE_TYPE_UNARY:      sprintf(dest->expr,"%c", dest->op);    break;
            case NODE_TYPE_OPERAND:    sprintf(dest->expr,"%d", dest->max);   break;
            case NODE_TYPE_FUNCTION:   sprintf(dest->expr,"%s", dest->id);    break;
            case NODE_TYPE_VARIABLE:   sprintf(dest->expr,"%s", dest->id);    break;
            case NODE_TYPE_LOCAL:      sprintf(dest->expr,"%s", dest->id);    break;
            case NODE_TYPE_CONSTANT:   sprintf(dest->expr,"%s", dest->id);    break;
            case NODE_TYPE_SUB:        sprintf(dest->expr,"%s", dest->expr);  break;
            default: fprintf(stderr,"Invalid node type detected; %d.\n", dest->type); exit(EXIT_FAILURE);
        }
    /* dest is a operand node */
    } else if(left != NULL && right != NULL) {
        switch(dest->type) {
            case NODE_TYPE_OPERATOR:
                switch(dest->op) {
                    case '!' + '=': sprintf(dest->expr,"%s != %s", left->expr, right->expr); break;
                    case '=' + '=': sprintf(dest->expr,"%s == %s", left->expr, right->expr); break;
                    case '<' + '=': sprintf(dest->expr,"%s <= %s", left->expr, right->expr); break;
                    case '>' + '=': sprintf(dest->expr,"%s >= %s", left->expr, right->expr); break;
                    case '&' + '&': sprintf(dest->expr,"%s && %s", left->expr, right->expr); break;
                    case '|' + '|': sprintf(dest->expr,"%s || %s", left->expr, right->expr); break;
                    default: sprintf(dest->expr,"%s %c %s", left->expr, dest->op, right->expr); break;
                }
                break;
            case NODE_TYPE_UNARY:      sprintf(dest->expr,"%c %s", dest->op, left->expr); break;
            case NODE_TYPE_OPERAND:    sprintf(dest->expr,"%s %d %s", left->expr, dest->max, right->expr);   break;
            case NODE_TYPE_FUNCTION:   sprintf(dest->expr,"%s %s %s", left->expr, dest->id, right->expr);    break;
            case NODE_TYPE_VARIABLE:   sprintf(dest->expr,"%s %s %s", left->expr, dest->id, right->expr);    break;
            case NODE_TYPE_LOCAL:      sprintf(dest->expr,"%s %s %s", left->expr, dest->id, right->expr);    break;
            case NODE_TYPE_CONSTANT:   sprintf(dest->expr,"%s %s %s", left->expr, dest->id, right->expr);    break;
            case NODE_TYPE_SUB:        sprintf(dest->expr,"%s %s %s", left->expr, dest->expr, right->expr);    break;
            default: fprintf(stderr,"Invalid node type detected; %d.\n", dest->type); exit(EXIT_FAILURE);
        }
    }
}

void node_free(node_t * node) {
   if(node != NULL) {
      freenamerange(node->cond);
      freerange(node->range);
      free(node);
   }
}

/* write into function argument */
void argument_write(node_t * node, char * desc) {
   int offset = node->args_cnt;                       /* current offset on stack */
   node->args_ptr[node->args_ptr_cnt++] = offset;     /* record current offset on stack */
   offset += sprintf(node->args + offset, "%s", desc);/* write new string onto stack */
   node->args[offset] = '\0';                         /* null terminate new string */
   node->args_cnt = offset + 1;                         /* set new offset on stack */
}

int translate_bonus_desc(node_t ** result, block_r * block, bonus_t * bonus) {
    int i = 0;
    int arg_cnt = bonus->type_cnt;
    int offset = 0;            /* current buffer offset */
    char buffer[BUF_SIZE];  /* buffer for handling modifier and qualifiers */
    int order[BONUS_SIZE];

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
        case 22: translate_bonus_float_percentage2(block, result[1], &order[0], "Reduce %s", "Reduce %s", "Add %s", 1000); break;
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
        case 1: translate_bonus_template(buffer, &offset, bonus->desc, block->eng[order[0]]); break;
        case 2: translate_bonus_template(buffer, &offset, bonus->desc, block->eng[order[0]], block->eng[order[1]]); break;
        case 3: translate_bonus_template(buffer, &offset, bonus->desc, block->eng[order[0]], block->eng[order[1]], block->eng[order[2]]); break;
        case 4: translate_bonus_template(buffer, &offset, bonus->desc, block->eng[order[0]], block->eng[order[1]], block->eng[order[2]], block->eng[order[3]]); break;
        case 5: translate_bonus_template(buffer, &offset, bonus->desc, block->eng[order[0]], block->eng[order[1]], block->eng[order[2]], block->eng[order[3]], block->eng[order[4]]); break;
    }
    translate_write(block, buffer, 1);

    /* debugging purposes */
    switch(bonus->attr) {
        case 30: printf("[%d]%s\n", block->item_id, buffer); break;
        default: break;
    }

    return SCRIPT_PASSED;
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
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

int script_lexical(token_r * token, const unsigned char * buffer) {
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

int script_analysis(token_r * token, block_r * block, int * block_cnt, int item_id, int flag, int block_dep, struct ic_db_t * db) {   
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
		memset(&info, 0, sizeof(block_t));
        status = block_keyword_search(db, &info, script_ptr[i]);
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
					script_lexical(new_token, (const unsigned char *) block_init->ptr[str_script]):
					script_lexical(new_token, (const unsigned char *) block_init->ptr[str_script]);
				if(ret_value == SCRIPT_FAILED) return SCRIPT_FAILED;
				
				/* allocate a new set of block, which will be merge
				 * with top level set of blocks */
				new_block = calloc(BLOCK_SIZE, sizeof(block_r));
				ret_value = script_analysis(new_token, new_block, &new_block_cnt, item_id, 0x00, block_size, db);
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

node_t * evaluate_expression_recursive(block_r * block, char ** expr, int32_t start, int32_t end, logic_node_t * logic_tree, int32_t flag) {
    int32_t i = 0;
    int32_t j = 0;
    int32_t len = 0;

    int32_t expr_inx_open = 0;
    int32_t expr_inx_close = 0;
    int32_t expr_sub_level = 0;

    int32_t op_cnt = 0;

    int32_t cdb_inx = 0;
    int32_t vdb_inx = 0;

    var_t * vdb_ent = NULL;
    const_t * cdb_ent = NULL;

    /* operator precedence tree */
    int32_t op_pred[PRED_LEVEL_MAX][PRED_LEVEL_PER] = {
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

            if(!expr_inx_close || expr_sub_level) {
                fprintf(file_res,"fatal: evaluate_expression_recursive on item %d; cannot "
                                      "find matching parentheses or missing %d parentheses.\n"
                                      , script_res->item_id, expr_sub_level);
            }

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
            for(j = 0; j < len; j++)
                if(!isdigit(expr[i][j])) {
                    fprintf(file_res,"fatal: evaluate_expression_recursive on item %d; invalid "
                                          "numeric literal detected; %s.\n"
                                          , script_res->item_id, expr[i]);
                }

            /* create an operand node */
            temp_node = calloc(1, sizeof(node_t));
            temp_node->type = NODE_TYPE_OPERAND;
            temp_node->min = temp_node->max = convert_integer(expr[i],10);
            op_cnt++;
        /* handler constant, variable, or function call */
        } else if(ATHENA_SCRIPT_IDENTIFER(expr[i][0]) || ATHENA_SCRIPT_SYMBOL(expr[i][0])) {
            cdb_inx = 0;
            vdb_inx = 0;

            temp_node = calloc(1, sizeof(node_t));
            temp_node->id = expr[i];

            cdb_inx = bsearch_ncs_str(script_res->const_db->db, script_res->const_db->size, 
                                              expr[i], constdb_name, constdb_getstr);
            vdb_inx = bsearch_ncs_str(script_res->var_db->db, script_res->var_db->size, 
                                              expr[i], vardb_id, vardb_getstr);

            /* handle variable or function */
            if(vdb_inx) {
                vdb_ent = &(script_res->var_db->db[vdb_inx]);
                
                /* handle function call */
                if(vdb_ent->type & TYPE_FUNC) {
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

                    if(!expr_inx_close || expr_sub_level) {
                        fprintf(file_res,"fatal: evaluate_expression_recursive on item %d; cannot "
                                              "find matching parentheses or missing %d parentheses.\n"
                                              , script_res->item_id, expr_sub_level);
                    }

                    /* create function node */
                    temp_node->type = NODE_TYPE_FUNCTION;

                    /* static function value */
                    if(vdb_ent->fflag & FUNC_DIRECT) {
                        temp_node->min = vdb_ent->min;
                        temp_node->max = vdb_ent->max;
                    /* dynamic function value */
                    } else if(vdb_ent->fflag & FUNC_PARAMS) {
                        evaluate_function(block, expr, expr[expr_inx_open], expr_inx_open + 2, expr_inx_close,
                                                &(temp_node->min), &(temp_node->max), temp_node);
                    }

                /* handle variable */
                } else if(vdb_ent->type & TYPE_VARI) {
                    /* create function node */
                    temp_node->type = NODE_TYPE_VARIABLE;

                    /* static variable value */
                    if(vdb_ent->vflag & VARI_DIRECT) {
                        temp_node->min = vdb_ent->min;
                        temp_node->max = vdb_ent->max;
                    }
                }
            /* handle constant */
            } else if(cdb_inx) {
                cdb_ent = &(script_res->const_db->db[cdb_inx]);
                /* create constant node */
                temp_node->type = NODE_TYPE_CONSTANT;
                temp_node->min = temp_node->max = cdb_ent->value;

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
            fprintf(file_res,"fatal: evaluate_expression_recursive on item %d; cannot "
                                  "evaluate %s due to syntax error.\n"
                                    , script_res->item_id, expr[i]);
            exit(EXIT_FAILURE);
        }

        /* operand without operator; invalid expression */
        if(op_cnt > 1) {
            fprintf(file_res,"fatal: evaluate_expression_recursive on item %d; operand "
                                  "missing operator.\n"
                                  , script_res->item_id);
            exit(EXIT_FAILURE);
        }

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
    if(root_node == iter_node) {
        fprintf(file_res,"fatal: evaluate_expression_recursive on item %d; invalid "
                              "expression; no nodes were added.\n"
                              , script_res->item_id);  
    }

    /* check that the list is completely linked */
    iter_node = root_node;
    do {
        if(iter_node->next == NULL || iter_node->prev == NULL)
            fprintf(file_res,"fatal: evaluate_expression_recursive on item %d; invalid "
                                  "node linking; NULL next or prev detected.\n"
                                  , script_res->item_id);
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

    return root_node;
}

node_t * evaluate_expression(block_r * block, char * expr, int modifier, int flag) {
    node_t * root_node;

    token_r expr_token;
    int length;
    logic_node_t * temp = NULL;

    /* lexical analysis on expression and return failed if failed */
    if(script_lexical(&expr_token, (const unsigned char *) expr) == SCRIPT_FAILED) return NULL;
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

/* evaluate the tree structure by pre-order traversal */
void evaluate_node(node_t * node, FILE * stm, logic_node_t * logic_tree, int32_t flag) {
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
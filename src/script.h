/*
 *   file: script.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef SCRIPT_H
#define SCRIPT_H
    /* standard c library */
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    /* lua api */
    #include "lua.h"
    #include "lauxlib.h"

    #include "util.h"
    #include "name_range.h"
    #include "range.h"
    #include "db_search.h"

    #define SCRIPT_PASSED                   CHECK_PASSED
    #define SCRIPT_FAILED                   CHECK_FAILED
    #define SCRIPT_SKIPPED                  2
    #define BUF_SIZE                        4096        /* total size of script, stack, and etc */
    #define PTR_SIZE                        4096        /* =.=; total number of possible tokens */
    #define ARG_SIZE                        64          /* total argument to keep track of */

    /* set this file stream to get node debug output;
     * node_dbg is passed to top level evaluate_node,
     * don't set this when compiling all item scripts
     * otherwise you get a lot of output. */
    FILE * node_dbg;

    typedef struct {
        char script[BUF_SIZE];
        char * script_ptr[PTR_SIZE];
        int script_cnt;
    } token_r;

    typedef struct node {
        int var;                /* var_db.txt tag for variable and functions */
        int type;               /* see node types macros */
        int op;                 /* operator (NODE_TYPE_OPERATOR) */
        char * id;              /* identifier (NODE_TYPE_FUNCTION / VARIABLE / LOCAL / CONSTANT / SUB) */
        /* range is a discontinuous set of values
         * between the minimum and maximum value */
        int min;                /* minrange() */
        int max;                /* maxrange() */
        range_t * range;        /* range values */
        /* logic tree is a 'balance' and-or-cond tree */
        logic_node_t * cond;    /* logic tree */
        int cond_cnt;           /* total number of variable and functions in the logic tree */
        char * formula;         /* user-friendly string of the expression */
        /* general stack */
        char stack[BUF_SIZE];   /* stack holding the variable, identifers, and formula strings */
        int stack_cnt;          /* stack offset */
        char * var_str[ARG_SIZE];
        int var_set[ARG_SIZE];
        int var_cnt;
        /* expression precedence and associative */
        struct node * left;
        struct node * right;
        struct node * next;
        struct node * prev;
        struct node * list; /* singly linked list for memory management */
    } node_t;

/* forward declaration for block_r */
struct script_t;

    typedef struct block_r {
        int item_id;                         /* item id defined in item_db.txt */
        /* block identification */
        char * name;                         /* block name */
        int type;                            /* block id defined in res/block_db.txt */
        /* ptr is the argument stack
         * eng is the translate stack
         * arg is the buffer for both stacks */
        char arg[BUF_SIZE];
        char * ptr[PTR_SIZE];                /* argument string array */
        char * eng[PTR_SIZE];                /* translated string array */
        int arg_cnt;                         /* current offset in arg buffer */
        int ptr_cnt;
        int eng_cnt;
        /* each block is part of doubly linked list maintained by
         * a script_t structure for memory management purposes */
        struct block_r * next;               /* next block */
        struct block_r * prev;               /* prev block */
        /* blocks form a child-parent relationship for the purposes
         * of inheriting logic trees, which is a singly linked list
         * from child to parent */
        struct block_r * link;               /* linked to if, else-if, else, for block */
        /* a singly linked list of variable blocks that the current
         * block can reference and use in evaluate_expression */
        struct block_r * set;                /* point to end of current linked list of set block */
        /* set for variable block */
        node_t * set_node;
        /* block references the resource from script_t
         * to prevent passing script_t as an argument
         * to every function */
        db_t * db;                           /* sqlite3 database handle to athena */
        int mode;                            /* multiplexer for rathena, eathena, or hercule tables */
        lua_State * map;
        struct script_t * scribe;            /* reference the enclosing script context */
        /* the logic tree is set for if, else, and for blocks
         * which is inherited by child blocks */
        logic_node_t * logic_tree;           /* calculational and dependency information */
        /* don't remember what this is for */
        int flag;
    } block_r;

    /* doubly linked list with a head node (non-circular) */
    typedef struct block_list_t {
        block_r root;
        block_r * head;
        block_r * tail;
    } block_list_t;

    /* abstract handle to contain everything for script processing */
    typedef struct script_t {
        /* variant per item */
        item_t item;
        block_list_t block;     /* linked list of allocated blocks */
        block_list_t free;      /* linked list of deallocated blocks */
        token_r token;          /* tokenize script */
        char buffer[BUF_SIZE];  /* item script translation */
        int offset;
        /* invariant; databases */
        db_t * db;              /* sqlite3 database handle to athena */
        int mode;               /* multiplexer for rathena, eathena, or hercule tables */
        /* invariant; mappings */
        lua_State * map;
    } script_t;

    /* script api */
    int script_init(script_t **, const char *, const char *, const char *, int);
    int script_deit(script_t **);
    /* script_lexical macros */
    #define ATHENA_SCRIPT_SYMBOL(X)         ((X) == '@' || (X) == '$' || (X) == '.' || (X) == '\'' || (X) == '#')
    #define ATHENA_SCRIPT_OPERATOR(X)       ((X) == '|' || (X) == '&' || (X) == '?' || (X) == ':' || (X) == '=' || \
                                             (X) == '>' || (X) == '<' || (X) == '-' || (X) == '+' || (X) == '/' || \
                                             (X) == '*' || (X) == '!')
    #define ATHENA_SCRIPT_UNARY(X)          ((X) == '!' || (X) == '-')
    #define ATHENA_SCRIPT_IDENTIFER(X)  (isalpha(X) || isdigit(X) || (X) == '_')
    int script_lexical(token_r *, char *);
    int script_analysis(script_t *, token_r *, block_r *, block_r **);
    /* script_parse flags */
    #define FLAG_PARSE_NORMAL               0x00
    #define FLAG_PARSE_LAST_TOKEN           0x01    /* include the last token into the parsed string */
    #define FLAG_PARSE_KEEP_COMMA           0x02    /* include all comma into the parsed string */
    #define FLAG_PARSE_SKIP_LEVEL           0x04    /* bypass () and {} when encountering delimiter and sentinel */
    #define FLAG_PARSE_ALL_FLAGS            0x07
    int script_parse(token_r *, int *, block_r *, char, char, int);
    int script_translate(script_t *);
    int script_generate(script_t *, char *, int *);
    int script_combo(int, char *, int *, db_t *, int);

    /* script recursive will inherit the parent script_t resources (invariant)
     * but is required to have its own resources (variant) to compiled without
     * any problems.
     *
     * the script api can be used to compile the script recursively, but that
     * would require loading separated databases and mappings, which is vvery
     * expensive. */
    /* revised */ int script_recursive(db_t *, int, lua_State *, char *, char **);

    /* block linked list */
    int list_forward(block_list_t *);
    int list_backward(block_list_t *);
    void list_init_head(block_list_t * list);
    int list_append_tail(block_list_t * list, block_r * block);
    int list_append_head(block_list_t * list, block_r * block);
    int list_add(block_list_t *, block_r *, block_r *);
    int list_rem(block_list_t *, block_r *);
    int list_pop_head(block_list_t *, block_r **);
    int list_pop_tail(block_list_t *, block_r **);
    int list_check(block_list_t *, block_r *);
    int list_tail(block_list_t *, block_r **);
    int list_head(block_list_t *, block_r **);

    /* block memory management */
    int script_block_alloc(script_t *, block_r **);                                 /* create a new block or get a block from the free list and place on the used list */
    int script_block_dealloc(script_t *, block_r **);                               /* remove a block from the used list and add the block to the free list */
    int script_block_reset(script_t *);                                             /* remove every block from used list, reset the block, and add to the free list */
    int script_block_release(block_r *);                                            /* reset the block */
    int script_block_finalize(script_t *);                                          /* remove every block from the free list and free the memory of each block */

    /* block buffer functions */
    #define TYPE_PTR                        1    /* block->ptr stack */
    #define TYPE_ENG                        2    /* block->eng stack */
    /* revised */ int script_buffer_write(block_r *, int, const char *);            /* push a string to block->ptr or block->eng stack */
    /* revised */ int script_buffer_unwrite(block_r *, int);                        /* pop a string from block->ptr or block->eng stack */
    /* revised */ int script_buffer_reset(block_r *, int);                          /* reset the entire stack */
    /* revised */ int script_formula_write(block_r *, int, node_t *, char **);      /* concatenate a node's formula string with block->eng stack string */

    /* script map id to string functions */
    /* revised */ int script_map_init(script_t *, const char *);                    /* create a lua stack containing the tables */
    /* revised */ int script_map_id(block_r *, char *, int, char **);               /* resolve integer id to string values */
    /* revised */ int script_map_deit(script_t *);

    /* script auxiliary functions */
    int script_extend_block(script_t *, char *, block_r *, block_r **);             /* recursively parse a block->ptr string; indirect recursion on script_analysis */
    int script_block_write(block_r *, char *, ...);                                 /* write to block->ptr stack using vararg */
    int check_loop_expression(script_t *, char *, char *);                          /* parse a statement within a for loop */
    int script_block_dump(script_t *, FILE *);                                      /* dump the script stacks */

    /* script stack functions */
    #define MAX_ITEM_LIST                   5 /* limits the number of item names pushed on
                                               * to the block->eng stack by stack_eng_item */
    /* revised */ int stack_ptr_call(block_r *, char *, int *);
    /* revised */ int stack_ptr_call_(block_r *, token_r *, int *);
    /* revised */ int stack_eng_item(block_r *, char *, int *);
    /* revised */ int stack_eng_skill(block_r *, char *, int *);
    /* revised */ int stack_eng_grid(block_r *, char *);
    /* revised */ int stack_eng_int(block_r *, char *, int);
    /* revised */ int stack_eng_int_signed(block_r *, char *, int, const char *, const char *);
    /* revised */ int stack_eng_time(block_r *, char *, int);
    /* revised */ int stack_eng_produce(block_r *, char *, int *);
    /* stack_eng_map flag (bitmask) */
    #define MAP_AMMO_FLAG                   0x0001
    #define MAP_CAST_FLAG                   0x0002
    #define MAP_CLASS_FLAG                  0x0004
    #define MAP_EFFECT_FLAG                 0x0008
    #define MAP_ELEMENT_FLAG                0x0010
    #define MAP_LOCATION_FLAG               0x0020
    #define MAP_ITEM_FLAG                   0x0040
    #define MAP_JOB_FLAG                    0x0080
    #define MAP_RACE_FLAG                   0x0100
    #define MAP_READPARAM_FLAG              0x0200
    #define MAP_REGEN_FLAG                  0x0400
    #define MAP_SEARCHSTORE_FLAG            0x0800
    #define MAP_SIZE_FLAG                   0x1000
    #define MAP_SP_FLAG                     0x2000
    #define MAP_TARGET_FLAG                 0x4000
    #define MAP_WEAPON_FLAG                 0x8000
    /* revised */ int stack_eng_map(block_r *, char *, int, int *);
    /* stack_eng_db flag (bitmask) */
    #define DB_SKILL_ID                     0x01
    #define DB_ITEM_ID                      0x02
    #define DB_MOB_ID                       0x04
    #define DB_MERC_ID                      0x08
    #define DB_PET_ID                       0x10
    #define DB_MAP_ID                       0x20
    /* revised */ int stack_eng_db(block_r *, char *, int, int *);
    /* revised */ int stack_eng_item_group_name(block_r *, char *, int *);
    #define BF_WEAPON                       0x0001
    #define BF_MAGIC                        0x0002
    #define BF_MISC                         0x0004
    #define BF_SHORT                        0x0010
    #define BF_LONG                         0x0040
    #define BF_SKILL                        0x0100
    #define BF_NORMAL                       0x0200
    #define BF_WEAPONMASK                   0x000F
    #define BF_RANGEMASK                    0x00F0
    #define BF_SKILLMASK                    0x0F00
    /* revised */ int stack_eng_trigger_bt(block_r *, char *);
    #define ATF_SELF                        0x01
    #define ATF_TARGET                      0x02
    #define ATF_SHORT                       0x04
    #define ATF_LONG                        0x08
    #define ATF_WEAPON                      0x10
    #define ATF_MAGIC                       0x20
    #define ATF_MISC                        0x40
    #define ATF_SKILL                       0x60

    /* revised */ int stack_eng_trigger_atf(block_r *, char *);
    /* revised */ int stack_eng_script(block_r *, char *);
    /* revised */ int stack_aux_formula(block_r *, node_t *, char *);

    /* script stack-translation functions to prevent source code
     * repetitions by factoring and simplifying similar patterns */
    /* revised */ int translate_id_amount(block_r *, int *, int *, const char *);

    /* script translation functions */
    /* revised */ int translate_getitem(block_r *);
    /* revised */ int translate_delitem(block_r *);
    /* revised */ int translate_rentitem(block_r *);
    /* revised */ int translate_heal(block_r *);
    /* revised */ int translate_produce(block_r *, int);
    /* revised */ int translate_status(block_r *);
    /* revised */ int translate_status_end(block_r *);
    /* revised */ int translate_pet_egg(block_r *);
    /* revised */ int translate_bonus(block_r *, char *);
    /* revised */ int translate_skill(block_r *);
    /* revised */ int translate_itemskill(block_r *);
    /* revised */ int translate_petloot(block_r *);
    /* revised */ int translate_petheal(block_r *);
    /* revised */ int translate_petrecovery(block_r *);
    /* revised */ int translate_petskillbonus(block_r *);
    /* revised */ int translate_petskillattack(block_r *);
    /* revised */ int translate_petskillattack2(block_r *);
    /* revised */ int translate_petskillsupport(block_r *);
    /* revised */ int translate_getexp(block_r *, int);
    /* revised */ int translate_autobonus(block_r *, int);

    int translate_searchstore(block_r *);
    int translate_buyingstore(block_r *);
    int translate_getrandgroup(block_r *, int);
    int translate_hire_merc(block_r *, int);
    int translate_transform(block_r *);
    int translate_skill_block(block_r *, int);
    int translate_misc(block_r *, char *);
    int translate_bonus_script(block_r *);
    int translate_setfalcon(block_r *);

    int translate_write(block_r *, char *, int);
    int translate_overwrite(block_r *, char *, int);

    /* writing the formula expressions */
    /* revised */ int id_write(node_t *, char *, ...);
    /* revised */ int var_write(node_t *, char *, ...);
    /* revised */ int expression_write(node_t *, char *, ...);    /* write the node->formula */

    /* evaluate an expression */
    #define EVALUATE_FLAG_KEEP_LOGIC_TREE   0x001 /* keep the logic tree */
    #define EVALUATE_FLAG_KEEP_NODE         0x002 /* keep the root node */
    #define EVALUATE_FLAG_EXPR_BOOL         0x004 /* relational operators returns 0 or 1 rather than range */
    #define EVALUATE_FLAG_WRITE_FORMULA     0x008 /* write the formula for expression */
    #define EVALUATE_FLAG_LIST_FORMULA      0x010
    #define EVALUATE_FLAG_KEEP_TEMP_TREE    0x020 /* keep logic tree for ?: operators; set blocks */
    #define EVALUATE_FLAG_ITERABLE_SET      0x040
    #define EVALUATE_FLAG_VARIANT_SET       0x080
    #define EVALUATE_FLAG_WRITE_STACK       0x100
    /* revised */ node_t * evaluate_expression(block_r *, char *, int, int);
    /* revised */ node_t * evaluate_expression_(block_r *, node_t *, int, int);
    /* revised */ node_t * evaluate_expression_recursive(block_r *, char **, int, int, logic_node_t *, int);

    /* evaluate a function with the expression */
    /* revised */ int evaluate_function(block_r *, char **, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_rand(block_r *, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_groupranditem(block_r *, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_readparam(block_r *, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_getskilllv(block_r *, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_isequipped(block_r *, int, int, var_res *, node_t *);

    #define NODE_TYPE_OPERATOR              0x01
    #define NODE_TYPE_OPERAND               0x02
    #define NODE_TYPE_UNARY                 0x80  /* unary operator */
    #define NODE_TYPE_FUNCTION              0x04  /* var.txt function */
    #define NODE_TYPE_VARIABLE              0x08  /* var.txt variable */
    #define NODE_TYPE_LOCAL                 0x10  /* set block variable */
    #define NODE_TYPE_CONSTANT              0x20  /* const.txt */
    #define NODE_TYPE_SUB                   0x40  /* subexpression node */
    /* revised */ int node_evaluate(node_t *, FILE *, logic_node_t *, int, int *);
    /* revised */ int node_cond_inherit(node_t *);
    /* revised */ void node_var_stack(node_t *, node_t *);
    /* revised */ void node_expr_append(node_t *, node_t *, node_t *);
    /* revised */ void node_dump(node_t *, FILE *);
    /* revised */ void node_free(node_t *);

    /* support generation */
    int script_linkage_count(block_r *, int);
    int script_generate_cond(logic_node_t *, FILE *, char *, char *, int *, block_r *);
    int script_generate_and_chain(logic_node_t *, char *, int *, block_r *);
    int script_generate_cond_node(logic_node_t *, char *, int *, block_r *);
    int condition_write_strcharinfo(char *, int *, logic_node_t *, block_r *);
    int condition_write_getiteminfo(char *, int *, logic_node_t *);
    int condition_write_item(char *, int *, range_t *, block_r *);
    int condition_write_class(char *, int *, range_t *, char *);
    int condition_write_range(char *, int *, range_t *, char *);
    int condition_write_format(char *, int *, char * fmt, ...);
#endif

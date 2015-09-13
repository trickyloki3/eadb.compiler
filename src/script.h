/*
 *   file: script.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef SCRIPT_H
#define SCRIPT_H
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include "util.h"
    #include "name_range.h"
    #include "range.h"
    #include "db_search.h"
    #include "lua.h"
    #include "lauxlib.h"

    /* return code from (nearly) all functions */
    #define SCRIPT_PASSED 0
    #define SCRIPT_FAILED 1
    #define SCRIPT_SKIP   2

    /* general constants */
    #define BUF_SIZE 4096       /* total size of script, stack, and etc */
    #define PTR_SIZE 4096       /* =.=; total number of possible tokens */
    #define ARG_SIZE 64         /* total argument to keep track of */
    #define BONUS_SIZE 5        /* total number of arguments for all bonus */
    #define BONUS_ID_MAX 5      /* bonus block identifer are from 0 to 4 */
    #define BLOCK_NULLIFIED -1  /* indicate that block should be ignore */

    /* macros to match athena syntax for script lexical  */
    #define ATHENA_SCRIPT_SYMBOL(X)     ((X) == '@' || (X) == '$' || (X) == '.' || (X) == '\'' || (X) == '#')
    #define ATHENA_SCRIPT_OPERATOR(X)   ((X) == '|' || (X) == '&' || (X) == '?' || (X) == ':' || (X) == '=' || \
                                         (X) == '>' || (X) == '<' || (X) == '-' || (X) == '+' || (X) == '/' || \
                                         (X) == '*' || (X) == '!')
    #define ATHENA_SCRIPT_UNARY(X)      ((X) == '!' || (X) == '-')
    #define ATHENA_SCRIPT_IDENTIFER(X)  (isalpha(X) || isdigit(X) || (X) == '_')

    /* flags to passed to script_parse */
    #define FLAG_PARSE_NORMAL       0x00
    #define FLAG_PARSE_LAST_TOKEN   0x01    /* include the last token into the parsed string */
    #define FLAG_PARSE_KEEP_COMMA   0x02    /* include all comma into the parsed string */
    #define FLAG_PARSE_SKIP_LEVEL   0x04    /* bypass () and {} when encountering delimiter and sentinel */
    #define FLAG_PARSE_ALL_FLAGS    0x07
    /* total precedence level and total operators within the same level for evaluate_expression_recursive */
    #define PRED_LEVEL_MAX 11
    #define PRED_LEVEL_PER 5

    /* node type determines how the node is process in evaluate_node */
    #define NODE_TYPE_OPERATOR    0x01
    #define NODE_TYPE_OPERAND     0x02
    #define NODE_TYPE_UNARY       0x80  /* unary operator */
    #define NODE_TYPE_FUNCTION    0x04  /* var.txt function */
    #define NODE_TYPE_VARIABLE    0x08  /* var.txt variable */
    #define NODE_TYPE_LOCAL       0x10  /* set block variable */
    #define NODE_TYPE_CONSTANT    0x20  /* const.txt */
    #define NODE_TYPE_SUB         0x40  /* subexpression node */

    /* alternative the normal behavior of all expression evaluation functions */
    #define EVALUATE_FLAG_KEEP_LOGIC_TREE   0x001 /* keep the logic tree */
    #define EVALUATE_FLAG_KEEP_NODE         0x002 /* keep the root node */
    #define EVALUATE_FLAG_EXPR_BOOL         0x004 /* flag for evaluating relational operators to either 0 or 1
                                                   * ignore expression simplification for conditional expression */
    #define EVALUATE_FLAG_WRITE_FORMULA     0x008 /* write the formula for expression */
    #define EVALUATE_FLAG_LIST_FORMULA      0x010
    #define EVALUATE_FLAG_KEEP_TEMP_TREE    0x020 /* keep logic tree for ?: operators; set blocks */
    #define EVALUATE_FLAG_ITERABLE_SET      0x040
    #define EVALUATE_FLAG_VARIANT_SET       0x080
    #define EVALUATE_FLAG_WRITE_STACK       0x100

    /* bonus flags for block minimization for script_bonus */
    #define BONUS_FLAG_NODUP        0x00020000  /* default: yes duplicate */
    #define BONUS_FLAG_MINIS        0x00040000  /* minimize by expanding list */
    #define BONUS_FLAG_MINIZ        0x00080000  /* minimize by checking arguments */

    /* check a set of arguments for equality for script_bonus */
    #define BONUS_FLAG_EQ_1         0x00000001  /* check 1st argument are equal */
    #define BONUS_FLAG_EQ_2         0x00000002  /* check 2nd argument are equal */
    #define BONUS_FLAG_EQ_3         0x00000004  /* check 3rd argument are equal */
    #define BONUS_FLAG_EQ_4         0x00000008  /* check 4th argument are equal */
    #define BONUS_FLAG_EQ_5         0x00000010  /* check 5th argument are equal */

    /* aggregate a set of arguments for script_bonus */
    #define BONUS_FLAG_AG_1         0x00000100  /* aggregate 1st argument */
    #define BONUS_FLAG_AG_2         0x00000200  /* aggregate 2nd argument */
    #define BONUS_FLAG_AG_3         0x00000400  /* aggregate 3rd argument */
    #define BONUS_FLAG_AG_4         0x00000800  /* aggregate 4th argument */
    #define BONUS_FLAG_AG_5         0x00001000  /* aggregate 5th argument */

    /* bf and atf trigger flags for translate_trigger */
    #define ATF_SELF    1
    #define ATF_TARGET  2
    #define ATF_SHORT   4
    #define ATF_LONG    8
    #define ATF_WEAPON  16
    #define ATF_MAGIC   32
    #define ATF_MISC    64
    #define ATF_SKILL   96
    #define BF_WEAPON   1
    #define BF_MAGIC    2
    #define BF_MISC     4
    #define BF_SHORT    16
    #define BF_LONG     64
    #define BF_SKILL    256
    #define BF_NORMAL   512

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
        int min;                /* minrange() and maxrange() values */
        int max;
        /* dependency */
        range_t * range;        /* min max range */
        logic_node_t * cond;
        int cond_cnt;           /* total count of variable and functions; cascaded by operator */
        /* single stack */
        char stack[BUF_SIZE];   /* stack to hold all strings */
        int stack_cnt;          /* size of stack */
        char * formula;         /* each node contains a (translated) subset of the entire expression */
        /* simplified formula */
        char * var_str[ARG_SIZE];
        int var_set[ARG_SIZE]; /* list of var written to var_str */
        int var_cnt;
        /* expression precedence and associative */
        struct node * left;
        struct node * right;
        struct node * next;
        struct node * prev;
        struct node * list;     /* arbitrary chain in sequence of creation; this allow freeing everything easier */
    } node_t;

/* forward declaration for block_r */
struct script_t;

    typedef struct block_r {
        char * name;                         /* block name */
        int item_id;                         /* item id defined in item_db.txt */
        int type;                            /* block id defined in res/block_db.txt */
        /* ptr loaded by parser & eng translated by translator */
        char arg[BUF_SIZE];
        char * ptr[PTR_SIZE];                /* argument string array */
        char * eng[PTR_SIZE];                /* translated string array */
        int arg_cnt;                         /* current offset in arg buffer */
        int ptr_cnt;
        int eng_cnt;
        /* linked list chaining blocks */
        struct block_r * next;               /* block_list_t handle linking */
        struct block_r * prev;               /* block_list_t handle linking */
        struct block_r * link;               /* if, else-if, else, for linking */
        struct block_r * set;                /* point to end of current linked list of set block */
        /* set block node for dependency */
        node_t * set_node;
        /* bonus block keep bonus query and integer results for post analysis */
        bonus_res bonus;                     /* bonus structure contain entry from item_bonus.txt */
        node_t * result[BONUS_SIZE];         /* keep until after minimization */
        /* database references; duplicate information from script_t to prevent
         * passing script_t to every translator; read only */
        db_t * db;                           /* sqlite3 database handle to athena */
        int mode;                            /* multiplexer for rathena, eathena, or hercule tables */
        lua_State * map;
        struct script_t * scribe;            /* reference the enclosing script context */
        /* translation information */
        logic_node_t * logic_tree;           /* calculational and dependency information */
        /* flag and offset are use for variable length arguments in functions */
        int flag;                            /* multi-purpose flag for special conditions
                                                0x01 - expanded the range of possible argument, i.e. callfunc(F_Rand, 1, 2, ..)
                                                0x02 - multivalues must be tagged random
                                                0x04 - use verbatim string */
        int offset;                          /* indicate the beginning of variable arguments */
    } block_r;

    /* block linked list */
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


    /* set this to some file descriptor to dump nodes */
    FILE * node_dbg;

    /* script api */
    int script_init(script_t **, const char *, const char *, const char *, int);
    int script_deit(script_t **);
    int script_lexical(token_r *, char *);
    int script_analysis(script_t *, token_r *, block_r *, block_r **);
    int script_parse(token_r *, int *, block_r *, char, char, int);
    int script_extend_block(script_t *, char *, block_r *, block_r **);
    int script_extend_paramater(block_r *, char *);
    int script_translate(script_t *);
    int script_bonus(script_t *);
    int script_generate(script_t *, char *, int *);
    int script_generate_combo(int, char *, int *, db_t *, int);

/* ! (o.o) !
 * core developers only
 * ! (o.o) !
 */

    /* script recursive will inherit the parent script_t resources (invariant)
     * but is required to have its own resources (variant) to compiled without
     * any problems.
     *
     * the script api can be used to compile the script recursively, but that
     * would require loading separated databases and mappins, which is v-very
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
    int script_block_alloc(script_t *, block_r **);                     /* create a new block or get a block from the free list and place on the used list */
    int script_block_dealloc(script_t *, block_r **);                   /* remove a block from the used list and add the block to the free list */
    int script_block_reset(script_t *);                                 /* remove every block from used list, reset the block, and add to the free list */
    int script_block_release(block_r *);                                /* reset the block */
    int script_block_finalize(script_t *);                              /* remove every block from the free list and free the memory of each block */

    /* block buffer functions */
    #define TYPE_PTR 1    /* block->ptr stack */
    #define TYPE_ENG 2    /* block->eng stack */
    /* revised */ int script_buffer_write(block_r *, int, const char *);              /* push a string to block->ptr or block->eng stack */
    /* revised */ int script_buffer_unwrite(block_r *, int);                          /* pop a string from block->ptr or block->eng stack */
    /* revised */ int script_buffer_reset(block_r *, int);                            /* reset the entire stack */
    /* revised */ int script_formula_write(block_r *, int, node_t *, char **);        /* concatenate a node's formula string with block->eng stack string */

    /* block 'for' functions */
    int script_block_write(block_r *, char *, ...);                     /* write to block->ptr stack using vararg */
    int check_loop_expression(script_t *, char *, char *);              /* parse a statement within a for loop */

    /* block debug functions */
    int script_block_dump(script_t *, FILE *);

    /* map id to string */
    /* revised */ int script_map_init(script_t *, const char *);
    /* revised */ int script_map_id(block_r *, char *, int, char **);
    /* revised */ int script_map_deit(script_t *);

    /* script stack functions */
    #define MAX_ITEM_LIST 5 /* limits the number of item names pushed on
                             * to the block->eng stack by stack_eng_item */
    /* revised */ int stack_ptr_call(block_r *, char *, int *);
    /* revised */ int stack_ptr_call_(block_r *, token_r *, int *);
    /* revised */ int stack_eng_item(block_r *, char *, int *);
    /* revised */ int stack_eng_int(block_r *, char *, int);
    /* revised */ int stack_eng_int_signed(block_r *, char *, int, const char *, const char *);
    /* revised */ int stack_eng_time(block_r *, char *, int);
    /* revised */ int stack_eng_produce(block_r *, char *, int *);
    /* stack_eng_map flag (bitmask) */
    #define MAP_AMMO_FLAG           0x0001
    #define MAP_CAST_FLAG           0x0002
    #define MAP_CLASS_FLAG          0x0004
    #define MAP_EFFECT_FLAG         0x0008
    #define MAP_ELEMENT_FLAG        0x0010
    #define MAP_LOCATION_FLAG       0x0020
    #define MAP_ITEM_FLAG           0x0040
    #define MAP_JOB_FLAG            0x0080
    #define MAP_RACE_FLAG           0x0100
    #define MAP_READPARAM_FLAG      0x0200
    #define MAP_REGEN_FLAG          0x0400
    #define MAP_SEARCHSTORE_FLAG    0x0800
    #define MAP_SIZE_FLAG           0x1000
    #define MAP_SP_FLAG             0x2000
    #define MAP_TARGET_FLAG         0x4000
    #define MAP_WEAPON_FLAG         0x8000
    /* revised */ int stack_eng_map(block_r *, char *, int, int *);
    /* stack_eng_db flag (bitmask) */
    #define DB_SKILL_ID             0x01
    #define DB_ITEM_ID              0x02
    #define DB_MOB_ID               0x04
    #define DB_MERC_ID              0x08
    #define DB_PET_ID               0x10
    #define DB_MAP_ID               0x20
    /* revised */ int stack_eng_db(block_r *, char *, int, int *);
    /* revised */ int stack_eng_item_group(block_r *, char *, int *);
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
    int translate_group_id(block_r *);
    int translate_searchstore(block_r *);
    int translate_buyingstore(block_r *);

    int translate_getrandgroup(block_r *, int);
    int translate_hire_merc(block_r *, int);
    int translate_getexp(block_r *, int);
    int translate_transform(block_r *);
    int translate_skill_block(block_r *, int);
    int translate_bonus(block_r *, char *);
    int translate_skill(block_r *, char *);
    int translate_splash(block_r *, char *);
    int translate_trigger(block_r *, char *, int); /* 0x01 - BF_TRIGGERS, 0x02 - ATF_TRIGGERS */
    int translate_id(block_r *, char *, int);
    int translate_autobonus(block_r *, int);
    int translate_misc(block_r *, char *);
    int translate_produce(block_r *, int);
    int translate_bonus_script(block_r *);
    int translate_setfalcon(block_r *);
    int translate_petloot(block_r *);
    int translate_petrecovery(block_r *);
    int translate_petskillbonus(block_r *);
    int translate_petskillattack(block_r *);
    int translate_petskillattack2(block_r *);
    int translate_petskillsupport(block_r *);
    int translate_petheal(block_r *);
    int translate_write(block_r *, char *, int);
    int translate_overwrite(block_r *, char *, int);

    /* writing the formula expressions */
    char * formula(char *, char *, node_t *);                            /* deprecated; use write_formula */
    char * status_formula(char *, char *, node_t *, int, int);
    int id_write(node_t *, char *, ...);
    int var_write(node_t *, char *, ...);
    int expression_write(node_t *, char *, ...);

    /* expression evaluation */
    node_t * evaluate_argument(block_r *, char *);
    node_t * evaluate_expression(block_r *, char *, int, int);
    node_t * evaluate_expression_post(block_r *, node_t *, int, int);
    node_t * evaluate_expression_recursive(block_r *, char **, int, int, logic_node_t *, int);
    int evaluate_node(node_t *, FILE *, logic_node_t *, int, int *);
    void node_inherit_cond(node_t *);
    void node_write_recursive(node_t *, node_t *);
    void node_expr_append(node_t *, node_t *, node_t *);
    void node_dmp(node_t *, FILE *);
    void node_free(node_t *);

    /* script function */
    /* revised */ int evaluate_function(block_r *, char **, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_rand(block_r *, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_groupranditem(block_r *, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_readparam(block_r *, int, int, var_res *, node_t *);
    /* revised */ int evaluate_function_getskilllv(block_r *, int, int, var_res *, node_t *);

    /* support translation */
    int translate_bonus_desc(node_t **, block_r *, bonus_res *);
    char * translate_bonus_template(char *, int *, char *, ...);
    void translate_bonus_integer(block_r *, node_t *, int *);
    void translate_bonus_integer2(block_r *, node_t *, int *, char *, char *, char *);
    void translate_bonus_float(block_r *, node_t *, int *, int);
    void translate_bonus_float_percentage(block_r *, node_t *, int *, int);
    void translate_bonus_percentage(block_r *, node_t *, int *);
    void translate_bonus_percentage2(block_r *, node_t *, int *, char *, char *, char *);
    void translate_bonus_float_percentage2(block_r *, node_t *, int *, char *, char *, char *, int);
    char * check_node_range(node_t *, char *);
    char * check_node_range_float(node_t *, char *, int);
    char * check_node_range_precentage(node_t *, char *);
    char * check_node_range_float_percentage(node_t *, char *, int);
    int check_node_affinity(node_t *);
    int script_linkage_count(block_r *, int);

    /* support generation */
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

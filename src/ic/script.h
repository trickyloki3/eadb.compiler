/*
 *    file: script.c
 *    date: 11/12/2014
 *    auth: trickyloki3
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

    /* eadb */
    #include "rbt.h"
    #include "util.h"
    #include "rbt_name_range.h"
    #include "rbt_range.h"
    #include "db_search.h"

    typedef struct rbt_node rbt_node;
    typedef struct rbt_tree rbt_tree;

    #define SCRIPT_PASSED                         CHECK_PASSED
    #define SCRIPT_FAILED                         CHECK_FAILED
    #define BUF_SIZE                              4096                         /* total size of script, stack, and etc */
    #define PTR_SIZE                              4096                         /* =.=; total number of possible tokens */

    /* set this file stream to get node debug output;
     * node_dbg is passed to top level evaluate_node,
     * don't set this when compiling all item scripts
     * otherwise you get a lot of output. */
    extern FILE * node_dbg;

    /* forward declaration for block_r
     * =.= postfix does not make sense */
    struct script_t;
    struct block_r;

    typedef struct {
         char script[BUF_SIZE];
         char * script_ptr[PTR_SIZE];
         int script_cnt;
    } token_r;

    struct node {
        int var;                                                                /* variable or function identifier number from var_db.txt */
        int type;                                                               /* type of node, i.e. function, variable, constant, etc */
        char * id;                                                              /* name of node, i.e. function name, variable name, constant name, etc*/
        int op;                                                                 /* operator, i.e. '+', '-', '*', etc */
        int return_type;                                                        /* bitmask of return types use for check formula compatibility */
        /* value range, logic tree, and node cache */
        int min;                                                                /* rbt_range_min value */
        int max;                                                                /* rbt_range_max value */
        rbt_range * value;                                                      /* range to keep track of a set of values */
        rbt_logic * logic;                                                      /* logic tree to keep track of predicates */
        int logic_count;                                                        /* total number of predicates in logic tree */
        char * formula;                                                         /* user-friendly representation of expression */
        struct node * list;                                                     /* singly linked list for memory management */
        struct script_t * script;                                               /* context containing the node cache */
        /* expression precedence and associative */
        struct node * left;
        struct node * right;
        struct node * next;
        struct node * prev;
    };

    typedef struct node node;

    typedef struct block_r {
        int item_id;                                                            /* item id defined in item_db.txt */
        /* block identification */
        char * name;                                                            /* block name */
        int type;                                                               /* block id defined in res/block_db.txt */
        /* ptr is the argument stack
         * eng is the translate stack
         * arg is the backing buffer for both stacks */
        char arg[BUF_SIZE];
        char * ptr[PTR_SIZE];                                                   /* argument string array */
        char * eng[PTR_SIZE];                                                   /* translated string array */
        int arg_cnt;                                                            /* current offset in arg buffer */
        int ptr_cnt;
        int eng_cnt;
        /* blocks form a child-parent relationship for the purposes
         * of inheriting logic trees, which is a singly linked list
         * from child to parent */
        struct block_r * link;                                                  /* linked to if, else-if, else, for block */
        /* a singly linked list of variable blocks that the current
         * block can reference and use in evaluate_expression */
        struct block_r * set;                                                   /* point to end of current linked list of set block */
        /* each set block keep their node so that referencing
         * blocks can copy the node for use in their expression */
        node * set_node;
        /* the logic tree is set for if, else, and for
         * blocks which is inherited by child blocks */
        rbt_logic * logic;
        int flag;                                                               /* use for handling for blocks in script_analysis */

        /* reference script context */
        struct script_t * script;

        /* block list interface */
        struct block_r * next;                                                  /* next block */
        struct block_r * prev;                                                  /* prev block */
        int free;                                                               /* free flag */
    } block_r;

    typedef struct script_t {
        /* variant for each item */
        item_t item;                                                            /* item entry in database */
        token_r token;                                                          /* script token stack */
        char buffer[BUF_SIZE];                                                  /* script translation */
        int offset;                                                             /* script buffer size */
        block_r * blocks;
        block_r * free_blocks;
        node * free_nodes;
        /* invariant */
        db_t * db;                                                              /* compiler and athena databases */
        int mode;                                                               /* EATHENA, RATHENA, or HERCULES */
        lua_State * map;                                                        /* map numeric constant to string */
    } script_t;

    /* block list interface */
    int block_init(block_r **);                                                 /* allocate a new block */
    int block_append(block_r *, block_r *);                                     /* add the block to the list */
    int block_remove(block_r *);                                                /* remove the block from the list */
    int block_reset(block_r *);                                                 /* free the block's fields */
    int block_deit(block_r **);                                                 /* free the block memory */

    /* block stack interface */
    #define TYPE_PTR 0x1                                                        /* block->ptr stack */
    #define TYPE_ENG 0x2                                                        /* block->eng stack */
    #define FLAG_CONCAT 0x4                                                     /* work only with vararg */
    #define FLAG_COMMA  0x8                                                     /* use comma delimiter */
    #define FLAG_EMPTY  0x10
    int block_stack_vararg(block_r *, int, const char *, ...);
    int block_stack_push(block_r *, int, const char *);                         /* push a string to the block->ptr or block->eng stack */
    int block_stack_pop(block_r *, int);                                        /* pop a string from the block->ptr or block->eng stack */
    int block_stack_reset(block_r *, int);                                      /* reset the stack; don't intermix block->ptr and block->eng stack */
    int block_stack_formula(block_r *, int, node *, char **);                 /* concatenate a node's formula string with block->eng stack string */
    int block_stack_dump(block_r *, FILE *);
    #define script_block_dump(script, stream) block_stack_dump((script->blocks->next), (stream))

    /* high level block list interface */
    int script_block_new(script_t *, block_r **);                               /* create a new block or reused a free block and append to head */
    int script_block_free(script_t *, block_r **);                              /* free the block and append to tail */
    int script_block_free_all(script_t *);                                      /* free every block on the list without reordering blocks */
    int script_block_release(script_t *);                                       /* free the block list from memory */

    /* lua script table interface maps integers to string */
    int script_map_init(script_t *, const char *);                              /* create a lua stack containing the tables */
    int script_map_id(block_r *, char *, int, char **);                         /* resolve integer id to string values */
    int script_map_deit(script_t *);                                            /* destroy the lua stack */

    /* public script interface */
    int script_init(script_t **, const char *, const char *, const char *, int);
    int script_deit(script_t **);

    /* script lexer macros used by script_lexical */
    #define SCRIPT_SYMBOL(X)               ((X) == '@' || (X) == '$' || (X) == '.' || (X) == '\'' || (X) == '#')
    #define SCRIPT_BINARY(X)               ((X) == '|' || (X) == '&' || (X) == '?' || (X) == ':' || (X) == '=' || \
                                            (X) == '>' || (X) == '<' || (X) == '-' || (X) == '+' || (X) == '/' || \
                                            (X) == '*' || (X) == '!')
    #define SCRIPT_UNARY(X)                ((X) == '!' || (X) == '-')
    #define SCRIPT_STRING(X)               (isalpha(X) || isdigit(X) || (X) == '_')

    /* script parser macros passed to script_parse */
    #define FLAG_PARSE_NORMAL              0x00
    #define FLAG_PARSE_LAST_TOKEN          0x01                                 /* include the last token into the parsed string */
    #define FLAG_PARSE_KEEP_COMMA          0x02                                 /* include all comma into the parsed string */
    #define FLAG_PARSE_SKIP_LEVEL          0x04                                 /* bypass () and {} when encountering delimiter and sentinel */
    #define FLAG_PARSE_ALL_FLAGS           0x07

    /* script core interface */
    int script_check(const char *);
    int script_lexical(token_r *, char *);
    int script_analysis(script_t *, token_r *, block_r *, block_r **);
    int script_analysis_(script_t *, char *, block_r *, block_r **);
    int check_loop_expression(script_t *, char *, char *);
    int script_parse(token_r *, int *, block_r *, char, char, int);
    int script_translate(script_t *);
    int script_generate(script_t *);
    int script_combo(int, char *, int *, db_t *, int);
    int script_recursive(db_t *, int, lua_State *, char *, char **);

    /* stack_eng_map bitmask flags */
    #define MAP_AMMO_FLAG                  0x000001
    #define MAP_CAST_FLAG                  0x000002
    #define MAP_CLASS_FLAG                 0x000004
    #define MAP_EFFECT_FLAG                0x000008
    #define MAP_ELEMENT_FLAG               0x000010
    #define MAP_LOCATION_FLAG              0x000020
    #define MAP_ITEM_FLAG                  0x000040
    #define MAP_JOB_FLAG                   0x000080
    #define MAP_RACE_FLAG                  0x000100
    #define MAP_READPARAM_FLAG             0x000200
    #define MAP_REGEN_FLAG                 0x000400
    #define MAP_SEARCHSTORE_FLAG           0x000800
    #define MAP_SIZE_FLAG                  0x001000
    #define MAP_SP_FLAG                    0x002000
    #define MAP_TARGET_FLAG                0x004000
    #define MAP_WEAPON_FLAG                0x008000
    #define MAP_REFINE_FLAG                0x010000
    #define MAP_NO_ERROR                   0x020000
    #define MAP_ITEM_INFO_FLAG             0x040000
    #define MAP_TIME_FLAG                  0x080000
    #define MAP_STRCHARINFO_FLAG           0x100000
    #define MAP_STATUSEFFECT_FLAG          0x200000

    /* stack_eng_db bitmask flags */
    #define DB_SKILL_ID                    0x01
    #define DB_ITEM_ID                     0x02
    #define DB_MOB_ID                      0x04
    #define DB_MERC_ID                     0x08
    #define DB_PET_ID                      0x10
    #define DB_MAP_ID                      0x20

    /* stack_eng_trigger_bt bitmask flags */
    #define BF_WEAPON                      0x0001
    #define BF_MAGIC                       0x0002
    #define BF_MISC                        0x0004
    #define BF_SHORT                       0x0010
    #define BF_LONG                        0x0040
    #define BF_SKILL                       0x0100
    #define BF_NORMAL                      0x0200
    #define BF_WEAPONMASK                  0x000F
    #define BF_RANGEMASK                   0x00F0
    #define BF_SKILLMASK                   0x0F00

    /* stack_eng_trigger_atf bitmask flags */
    #define ATF_SELF                       0x01
    #define ATF_TARGET                     0x02
    #define ATF_SHORT                      0x04
    #define ATF_LONG                       0x08
    #define ATF_WEAPON                     0x10
    #define ATF_MAGIC                      0x20
    #define ATF_MISC                       0x40
    #define ATF_SKILL                      0x60

    /* stack_eng_options bitmask flags */
    #define OPT_SIGHT                       0x00000001
    #define OPT_HIDE                        0x00000002
    #define OPT_CLOAK                       0x00000004
    #define OPT_CART1                       0x00000008
    #define OPT_FALCON                      0x00000010
    #define OPT_PECO                        0x00000020
    #define OPT_INVISIBLE                   0x00000040
    #define OPT_CART2                       0x00000080
    #define OPT_CART3                       0x00000100
    #define OPT_CART4                       0x00000200
    #define OPT_CART5                       0x00000400
    #define OPT_ORC                         0x00000800
    #define OPT_WEDDING                     0x00001000
    #define OPT_RUWACH                      0x00002000
    #define OPT_CHASEWALK                   0x00004000
    #define OPT_FLYING                      0x00008000
    #define OPT_XMAS                        0x00010000
    #define OPT_TRANSFORM                   0x00020000
    #define OPT_SUMMER                      0x00040000
    #define OPT_DRAGON1                     0x00080000
    #define OPT_WUG                         0x00100000
    #define OPT_WUGRIDER                    0x00200000
    #define OPT_MADOGEAR                    0x00400000
    #define OPT_DRAGON2                     0x00800000
    #define OPT_DRAGON3                     0x01000000
    #define OPT_DRAGON4                     0x02000000
    #define OPT_DRAGON5                     0x04000000
    #define OPT_HANBOK                      0x08000000
    #define OPT_OKTOBERFEST                 0x10000000

    /* stack_eng_item bitmask flags */
    #define FLAG_GETITEM                    0x1

    /* stack_eng_int_* bitmask flags */
    #define FORMAT_RATIO                    0x01
    #define FORMAT_PLUS                     0x02
    #define FORMAT_FLOAT                    0x04

    /* stack_eng_coordinate negative values */
    #define COORD_RANDOM                    -1
    #define COORD_CURRENT                   -2

    /* stack limits */
    #define MAX_SUB_GROUP                  4    /* set to MAX_ITEMGROUP_RANDGROUP for rathena */
    #define MAX_ITEM_LIST                  10

    /* script stack functions */
    int stack_ptr_call(block_r *, char *, int *);
    int stack_ptr_call_(block_r *, token_r *, int *);
    /* re */ int stack_eng_item(block_r *, char *, int *, int);
    /* re */ int stack_eng_skill(block_r *, char *, int *);
    /* re */ int stack_eng_grid(block_r *, char *);
    /* re */ int stack_eng_coordinate(block_r *, char *);
    /* re */ int stack_eng_int(block_r *, char *, int, int);
    /* re */ int stack_eng_int_signed(block_r *, char *, int, const char *, const char *, int);
    /* re */ int stack_eng_time(block_r *, char *, int);
    /* re */ int stack_eng_produce(block_r *, char *, int *);
    /* re */ int stack_eng_map(block_r *, char *, int, int *);
    /* re */ int stack_eng_db(block_r *, char *, int, int *);
    /* re */ int stack_eng_item_group(block_r *, char *);
    /* re */ int stack_eng_trigger_bt(block_r *, char *);
    /* re */ int stack_eng_trigger_atf(block_r *, char *);
    /* re */ int stack_eng_options(block_r *, char *);
    /* re */ int stack_eng_script(block_r *, char *);
    int stack_eng_status_val(block_r *, char *, int);
    int stack_aux_formula(block_r *, node *, char *);

    /* script stack functions for status */
    int stack_eng_re_aspd(block_r *, char *);

    /* script stack-translation functions to prevent source code
     * repetitions by factoring and simplifying similar patterns */
    int translate_id_amount(block_r *, int *, int *, const char *);

    /* script translation functions */
    int translate_getitem(block_r *);
    int translate_delitem(block_r *);
    int translate_rentitem(block_r *);
    int translate_heal(block_r *);
    int translate_produce(block_r *, int);
    int translate_status(block_r *);
    int translate_status_end(block_r *);
    int translate_pet_egg(block_r *);
    int translate_bonus(block_r *, char *);
    int translate_skill(block_r *);
    int translate_itemskill(block_r *);
    int translate_petloot(block_r *);
    int translate_petheal(block_r *);
    int translate_petrecovery(block_r *);
    int translate_petskillbonus(block_r *);
    int translate_petskillattack(block_r *);
    int translate_petskillattack2(block_r *);
    int translate_petskillsupport(block_r *);
    int translate_getexp(block_r *);
    int translate_getguildexp(block_r *);
    int translate_autobonus(block_r *, int);
    int translate_hire_mercenary(block_r *);
    int translate_buyingstore(block_r *);
    int translate_searchstore(block_r *);
    int translate_skill_block(block_r *);
    int translate_warp(block_r *);
    int translate_monster(block_r *);
    int translate_callfunc(block_r *);
    int translate_getrandgroupitem(block_r *);
    int translate_getgroupitem(block_r *);
    int translate_bonus_script(block_r *);
    int translate_transform(block_r *);
    int translate_setfalcon(block_r *);
    int translate_makerune(block_r *);

    /* evaluate_expression bitmask flags */
    #define EVALUATE_FLAG_KEEP_LOGIC_TREE  0x001 /* keep the logic tree */
    #define EVALUATE_FLAG_KEEP_NODE        0x002 /* keep the root node */
    #define EVALUATE_FLAG_EXPR_BOOL        0x004 /* relational operators returns 0 or 1 rather than range */
    #define EVALUATE_FLAG_WRITE_FORMULA    0x008 /* write the formula for expression */
    #define EVALUATE_FLAG_KEEP_TEMP_TREE   0x010 /* keep logic tree for ?: operators; set blocks */
    #define EVALUATE_FLAG_ALL              0xfff

    /* higher level wrappers over evaluate expression */
    int evaluate_numeric_constant(block_r *, char *, int, int *);

    /* evaluate an expression */
    node * evaluate_expression(block_r *, char *, int, int);
    node * evaluate_expression_recursive(block_r *, char **, int, int, rbt_logic *, rbt_tree * id_tree, int);
    int evaluate_expression_sub(block_r *, char **, int *, int, rbt_logic *, rbt_tree *, int, node **);
    int evaluate_expression_var(block_r *, char **, int *, int, rbt_logic *, int, node **);

    /* evaluate a function with the expression
     *
     * arguments with complex expressions can cause stack overflow
     * problems or produce results that are difficult to translate
     * therefore, complex expressions are typically not supported.
     *
     * vararg - variable support
     * const  - constant support
     *
     * =.= forgot to add back the documentation
     */
    int evaluate_function(block_r *, char **, int, int, var_res *, node *);
    int evaluate_function_rand(block_r *, int, int, var_res *, node *);
    int evaluate_function_groupranditem(block_r *, int, int, var_res *, node *);
    int evaluate_function_readparam(block_r *, int, int, var_res *, node *);
    int evaluate_function_getskilllv(block_r *, int, int, var_res *, node *);
    int evaluate_function_isequipped(block_r *, int, int, var_res *, node *);
    int evaluate_function_getequiprefinerycnt(block_r *, int, int, var_res *, node *);
    int evaluate_function_getiteminfo(block_r *, int, int, var_res *, node *);
    int evaluate_function_getequipid(block_r *, int, int, var_res *, node *);
    int evaluate_function_gettime(block_r *, int, int, var_res *, node *);
    int evaluate_function_callfunc(block_r *, int, int, var_res *, node *);
    int evaluate_function_countitem(block_r *, int, int, var_res *, node *);
    int evaluate_function_pow(block_r *, int, int, var_res *, node *);
    int evaluate_function_strcharinfo(block_r *, int, int, var_res *, node *);
    int evaluate_function_setoption(block_r *, int, int, var_res *, node *);

     /* node types */
    #define NODE_TYPE_OPERATOR             0x01
    #define NODE_TYPE_OPERAND              0x02
    #define NODE_TYPE_UNARY                0x80  /* unary operator */
    #define NODE_TYPE_FUNCTION             0x04  /* var.txt function */
    #define NODE_TYPE_VARIABLE             0x08  /* var.txt variable */
    #define NODE_TYPE_LOCAL                0x10  /* set block variable */
    #define NODE_TYPE_CONSTANT             0x20  /* const.txt */
    #define NODE_TYPE_SUB                  0x40  /* subexpression node */

    int node_steal(node *, node *);
    int node_structure(node *);
    int node_evaluate(node *, FILE *, rbt_logic *, rbt_tree *, int);
    int node_inherit(node *);
    void node_dump(node *, FILE *);

    #define node_free(x) if(x) { node_deit(x->script, &x); }
    int node_init(script_t *, node **);
    int node_deit(script_t *, node **);
    int node_release(script_t *);
    int node_append(node *, node *);
    int node_remove(node *);

    int id_tree_add(rbt_tree *, char *);
    int id_tree_free(struct rbt_node *, void *, int);
#endif

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
    #include "util.h"
    #include "name_range.h"
    #include "range.h"
    #include "db_search.h"

    #define SCRIPT_PASSED                         CHECK_PASSED
    #define SCRIPT_FAILED                         CHECK_FAILED
    #define BUF_SIZE                              4096                         /* total size of script, stack, and etc */
    #define PTR_SIZE                              4096                         /* =.=; total number of possible tokens */
    #define ARG_SIZE                              64                           /* total argument to keep track of */

    /* set this file stream to get node debug output;
     * node_dbg is passed to top level evaluate_node,
     * don't set this when compiling all item scripts
     * otherwise you get a lot of output. */
    extern FILE * node_dbg;

    typedef struct {
         char script[BUF_SIZE];
         char * script_ptr[PTR_SIZE];
         int script_cnt;
    } token_r;

    typedef struct node {
         int var;                                                              /* var_db.txt tag for variable and functions */
         int type;                                                             /* see node types macros */
         int op;                                                               /* operator (NODE_TYPE_OPERATOR) */
         char * id;                                                            /* identifier (NODE_TYPE_FUNCTION / VARIABLE / LOCAL / CONSTANT / SUB) */
         /* range is a discontinuous set of values
          * between the minimum and maximum value */
         int min;                                                              /* minrange() */
         int max;                                                              /* maxrange() */
         range_t * range;                                                      /* range values */
         /* logic tree is a and-or-cond tree */
         logic_node_t * cond;                                                  /* logic tree */
         int cond_cnt;                                                         /* total number of variable and functions in the logic tree */
         char * formula;                                                       /* user-friendly string of the expression */
         /* expression precedence and associative */
         struct node * left;
         struct node * right;
         struct node * next;
         struct node * prev;
         /* singly linked list for memory management */
         struct node * list;
    } node_t;

    /* forward declaration for block_r */
    struct script_t;

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
        node_t * set_node;
        /* the logic tree is set for if, else, and for
         * blocks which is inherited by child blocks */
        logic_node_t * logic_tree;                                              /* calculational and dependency information */
        int flag;                                                               /* use for handling for blocks in script_analysis */

        /* reference the enclosing script context */
        struct script_t * script;

        /* each block is part of doubly linked list maintained by
         * a script_t structure for memory management purposes */
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
        block_r * blocks;                                                       /* block list */
        /* invariant */
        db_t * db;                                                              /* compiler and athena databases */
        int mode;                                                               /* EATHENA, RATHENA, or HERCULES */
        lua_State * map;                                                        /* map numeric constant to string */
    } script_t;

    /* block list interface */
    /* revised */ int block_init(block_r **);                                   /* allocate a new block */
    /* revised */ int block_append(block_r *, block_r *);                       /* add the block to the list */
    /* revised */ int block_remove(block_r *);                                  /* remove the block from the list */
    /* revised */ int block_reset(block_r *);                                   /* free the block's fields */
    /* revised */ int block_deit(block_r **);                                   /* free the block memory */

    /* block stack interface */
    #define TYPE_PTR 1                                                          /* block->ptr stack */
    #define TYPE_ENG 2                                                          /* block->eng stack */
    /* revised */ int block_stack_push(block_r *, int, const char *);           /* push a string to the block->ptr or block->eng stack */
    /* revised */ int block_stack_pop(block_r *, int);                          /* pop a string from the block->ptr or block->eng stack */
    /* revised */ int block_stack_reset(block_r *, int);                        /* reset the stack; don't intermix block->ptr and block->eng stack */
    /* revised */ int block_stack_formula(block_r *, int, node_t *, char **);   /* concatenate a node's formula string with block->eng stack string */
    /* revised */ int block_stack_dump(block_r *, FILE *);
    #define script_block_dump(script, stream) block_stack_dump((script->blocks), (stream))

    /* high level block list interface */
    /* revised */ int script_block_new(script_t *, block_r **);                 /* create a new block or reused a free block and append to head */
    /* revised */ int script_block_free(script_t *, block_r **);                /* free the block and append to tail */
    /* revised */ int script_block_free_all(script_t *);                        /* free every block on the list without reordering blocks */
    /* revised */ int script_block_release(script_t *);                         /* free the block list from memory */

    /* lua script table interface maps integers to string */
    /* revised */ int script_map_init(script_t *, const char *);                /* create a lua stack containing the tables */
    /* revised */ int script_map_id(block_r *, char *, int, char **);           /* resolve integer id to string values */
    /* revised */ int script_map_deit(script_t *);                              /* destroy the lua stack */

    /* public script interface */
    /* revised */ int script_init(script_t **, const char *, const char *, const char *, int);
    /* revised */ int script_deit(script_t **);


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
    /* revised */ int script_lexical(token_r *, char *);
    /* revised */ int script_analysis(script_t *, token_r *, block_r *, block_r **);
    /* revised */ int script_analysis_(script_t *, char *, block_r *, block_r **);
    /* revised */ int check_loop_expression(script_t *, char *, char *);
    /* revised */ int script_parse(token_r *, int *, block_r *, char, char, int);
    /* revised */ int script_translate(script_t *);
    int script_generate(script_t *, char *, int *);
    int script_combo(int, char *, int *, db_t *, int);
    /* revised */ int script_recursive(db_t *, int, lua_State *, char *, char **);

    /* stack_eng_map bitmask flags */
    #define MAP_AMMO_FLAG                  0x00001
    #define MAP_CAST_FLAG                  0x00002
    #define MAP_CLASS_FLAG                 0x00004
    #define MAP_EFFECT_FLAG                0x00008
    #define MAP_ELEMENT_FLAG               0x00010
    #define MAP_LOCATION_FLAG              0x00020
    #define MAP_ITEM_FLAG                  0x00040
    #define MAP_JOB_FLAG                   0x00080
    #define MAP_RACE_FLAG                  0x00100
    #define MAP_READPARAM_FLAG             0x00200
    #define MAP_REGEN_FLAG                 0x00400
    #define MAP_SEARCHSTORE_FLAG           0x00800
    #define MAP_SIZE_FLAG                  0x01000
    #define MAP_SP_FLAG                    0x02000
    #define MAP_TARGET_FLAG                0x04000
    #define MAP_WEAPON_FLAG                0x08000
    #define MAP_REFINE_FLAG                0x10000
    #define MAP_NO_ERROR                   0x20000

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

    /* stack limits */
    #define MAX_ITEM_LIST                  5

    /* script stack functions */
    /* revised */ int stack_ptr_call(block_r *, char *, int *);
    /* revised */ int stack_ptr_call_(block_r *, token_r *, int *);
    /* revised */ int stack_eng_item(block_r *, char *, int *);
    /* revised */ int stack_eng_skill(block_r *, char *, int *);
    /* revised */ int stack_eng_grid(block_r *, char *);
    /* revised */ int stack_eng_int(block_r *, char *, int);
    /* revised */ int stack_eng_int_signed(block_r *, char *, int, const char *, const char *);
    /* revised */ int stack_eng_time(block_r *, char *, int);
    /* revised */ int stack_eng_produce(block_r *, char *, int *);
    /* revised */ int stack_eng_map(block_r *, char *, int, int *);
    /* revised */ int stack_eng_db(block_r *, char *, int, int *);
    /* revised */ int stack_eng_item_group_name(block_r *, char *, int *);
    /* revised */ int stack_eng_trigger_bt(block_r *, char *);
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

    /* evaluate_expression bitmask flags */
    #define EVALUATE_FLAG_KEEP_LOGIC_TREE  0x001 /* keep the logic tree */
    #define EVALUATE_FLAG_KEEP_NODE        0x002 /* keep the root node */
    #define EVALUATE_FLAG_EXPR_BOOL        0x004 /* relational operators returns 0 or 1 rather than range */
    #define EVALUATE_FLAG_WRITE_FORMULA    0x008 /* write the formula for expression */
    #define EVALUATE_FLAG_LIST_FORMULA     0x010
    #define EVALUATE_FLAG_KEEP_TEMP_TREE   0x020 /* keep logic tree for ?: operators; set blocks */
    #define EVALUATE_FLAG_ITERABLE_SET     0x040
    #define EVALUATE_FLAG_VARIANT_SET      0x080
    #define EVALUATE_FLAG_WRITE_STACK      0x100

    /* evaluate an expression */
    /* revised */ node_t * evaluate_expression(block_r *, char *, int, int);
    /* revised */ node_t * evaluate_expression_(block_r *, node_t *, int, int);
    /* revised */ node_t * evaluate_expression_recursive(block_r *, char **, int, int, logic_node_t *, int);

    /* evaluate a function with the expression
     *
     * arguments with complex expressions can cause stack overflow
     * problems or produce results that are difficult to translate
     * therefore, complex expressions are typically not supported.
     *
     * vararg - variable support
     * const  - constant support
     */
    /* revised;        */ int evaluate_function(block_r *, char **, int, int, var_res *, node_t *);
    /* revised; vararg */ int evaluate_function_rand(block_r *, int, int, var_res *, node_t *);
    /* revised; vararg */ int evaluate_function_groupranditem(block_r *, int, int, var_res *, node_t *);
    /* revised; const  */ int evaluate_function_readparam(block_r *, int, int, var_res *, node_t *);
    /* revised; const  */ int evaluate_function_getskilllv(block_r *, int, int, var_res *, node_t *);
    /* revised; vararg */ int evaluate_function_isequipped(block_r *, int, int, var_res *, node_t *);
    /* revised; const  */ int evaluate_function_getequiprefinerycnt(block_r *, int, int, var_res *, node_t *);
    /* revised; const  */ int evaluate_function_getiteminfo(block_r *, int, int, var_res *, node_t *);

     /* node types */
    #define NODE_TYPE_OPERATOR             0x01
    #define NODE_TYPE_OPERAND              0x02
    #define NODE_TYPE_UNARY                0x80  /* unary operator */
    #define NODE_TYPE_FUNCTION             0x04  /* var.txt function */
    #define NODE_TYPE_VARIABLE             0x08  /* var.txt variable */
    #define NODE_TYPE_LOCAL                0x10  /* set block variable */
    #define NODE_TYPE_CONSTANT             0x20  /* const.txt */
    #define NODE_TYPE_SUB                  0x40  /* subexpression node */

    /* revised */ int node_evaluate(node_t *, FILE *, logic_node_t *, int, int *);
    /* revised */ int node_condition_inherit(node_t *);
    /* revised */ void node_dump(node_t *, FILE *);
    /* revised */ void node_free(node_t *);
#endif

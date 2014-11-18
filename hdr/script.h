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
    #include "db.h"
    #include "api.h"
    #include "util.h"
    #include "name_range.h"
    #include "range.h"
    #include "table.h"

    /* general */
    #define DEBUG_LEXICAL 0
    #define BUF_SIZE 4096
    #define BLOCK_SIZE 32
    #define BONUS_SIZE 5
    #define SUB_SIZE 256

    /* script return status */
    #define SCRIPT_PASSED 0
    #define SCRIPT_FAILED 1

    /* athena syntax groups */
    #define ATHENA_SCRIPT_SYMBOL(X)           ((X) == '@' || (X) == '$' || (X) == '.' || (X) == '\'' || (X) == '#')
    #define ATHENA_SCRIPT_OPERATOR(X)         ((X) == '|' || (X) == '&' || (X) == '?' || (X) == ':' || (X) == '=' || (X) == '>' || (X) == '<' || (X) == '-' || (X) == '+' || (X) == '/' || (X) == '*' || (X) == '!')
    #define ATHENA_SCRIPT_UNARY(X)            ((X) == '!' || (X) == '-')
    #define ATHENA_SCRIPT_IDENTIFER(X)        (isalpha(X) || isdigit(X) || (X) == '_')
    #define CHECK_ZERO(X)                     (strlen(X) == 1 && X[0] == '0')
    #define CHECK_NEGATIVE(X)                 (strlen(X) > 0 && strstr(X,"-") != NULL)

    /* BF and ATF Trigger Flags */
    #define ATF_SELF 1
    #define ATF_TARGET 2
    #define ATF_SHORT 4
    #define ATF_LONG 8
    #define ATF_WEAPON 16
    #define ATF_MAGIC 32
    #define ATF_MISC 64
    #define ATF_SKILL 96
    #define BF_WEAPON 1
    #define BF_MAGIC 2
    #define BF_MISC 4
    #define BF_SHORT 16
    #define BF_LONG 64
    #define BF_SKILL 256
    #define BF_NORMAL 512

    typedef struct {
        char script[BUF_SIZE];
        char * script_ptr[BUF_SIZE];
        int script_cnt;
    } token_r;

    typedef struct block_r {
       int item_id;                         /* item to which the block belongs */
       int block_id;                        /* unique block id (index) */
       /* arg is where all strings should be store; ptr and eng are simply pointers to the arg buffer */
       char * arg;                          /* block raw and translation stack */
       char * ptr[BUF_SIZE];                /* raw arguments */
       char * eng[BUF_SIZE];                /* translated arguments */
       char * desc;                         /* final description translation */
       int arg_cnt;                         /* various counters for the above arrays */
       int ptr_cnt;
       int eng_cnt;
       block_t * type;                      /* access to translation information in block_db.txt from block_db */
       int link;                            /* indicate conditional linking for if-else statements (index to block) */
       struct block_r * depd[BLOCK_SIZE];   /* indicate calculation linking for set blocks */
       int depd_cnt;                        /* number of dependencies */
       int set_min;                         /* SET BLOCK SPECIFIC ONLY, i.e. CHEATING! */
       int set_max;
       int flag;                            /* multi-purpose flag for special conditions 
                                              0x01 - expanded the range of possible argument, i.e. callfunc(F_Rand, 1, 2, ..)
                                              0x02 - multivalues must be tagged random */
       int offset;                          /* indicate the beginning of special arguments */
       logic_node_t * logic_tree;           /* calculational and dependency information */
    } block_r;

    int global_mode;
    struct ic_db_t * global_db;
    char global_err[BUF_SIZE];
    #define exit_buf(X, ...) exit_abt(build_buffer(global_err, (X), __VA_ARGS__))
    void block_init(block_r **, int);
    void block_deinit(block_r *, int);

    /* compilation processes */
    int script_lexical(token_r *, char *);
    int script_analysis(token_r *, block_r *, int *, int, int, int);
    int script_parse(token_r *, int *, block_r *, char, char, int);
    int script_dependencies(block_r *, int);
    int script_translate(block_r *, int);
    int script_extend(block_r *, char *);
    int script_generate(ic_item_t *, block_r *, int);

    /* script translation functions */
    int translate_bonus(block_r *, int);
    int translate_const(block_r *, char *, int);
    int translate_skill(block_r *, char *);
    int translate_tbl(block_r *, char *, int);
    int translate_splash(block_r *, char *);
    int translate_trigger(block_r *, char *, int); /* 0x01 - BF_TRIGGERS, 0x02 - ATF_TRIGGERS */
    int translate_time(block_r *, char *);
    int translate_id(block_r *, char *, int);
    int translate_item(block_r *, char *);
    int translate_autobonus(block_r *, int);
    int translate_heal(block_r *, int);
    int translate_skill_block(block_r *, int);
    int translate_getitem(block_r *, int);
    int translate_rentitem(block_r *, int);
    int translate_delitem(block_r *, int);
    int translate_getrandgroup(block_r *, int);
    int translate_bstore(block_r *, int);
    int translate_hire_merc(block_r *, int);
    int translate_pet_egg(block_r *, int);
    int translate_getexp(block_r *, int);
    int translate_misc(block_r *, char *);
    int translate_transform(block_r *);
    int translate_produce(block_r *, int);
    int translate_status_end(block_r *);
    int translate_status(block_r *, int);
    int translate_write(block_r *, char *, int);

    /* debug compiler by dumping the block list */
    void block_debug_dump(block_r *, FILE *, char *);
    void block_debug_dump_all(block_r *, int, FILE *);
    void block_debug_dump_depd_recurse(struct block_r **, int, int, int, FILE *);

     /* node types */
    #define NODE_TYPE_OPERATOR    0x01
    #define NODE_TYPE_OPERAND     0x02
    #define NODE_TYPE_UNARY       0x80  /* unary operator */
    #define NODE_TYPE_FUNCTION    0x04  /* var.txt function */
    #define NODE_TYPE_VARIABLE    0x08  /* var.txt variable */
    #define NODE_TYPE_LOCAL       0x10  /* set block variable */
    #define NODE_TYPE_CONSTANT    0x20  /* const.txt */
    #define NODE_TYPE_SUB         0x40  /* subexpression node */
    /* operator precedence array sizes */
    #define PRED_LEVEL_MAX        11
    #define PRED_LEVEL_PER        5
    /* flags for keeping node or logic */
    #define EVALUATE_FLAG_KEEP_LOGIC_TREE  0x1
    #define EVALUATE_FLAG_KEEP_NODE        0x2
    /* flag for evaluating relational operators to either 0 or 1 */
    #define EVALUATE_FLAG_EXPR_BOOL        0x4

    /* expression handling */
    typedef struct node {
        int type;               /* see node types macros */
        int op;                 /* operator (NODE_TYPE_OPERATOR) */
        char * id;              /* identifier (NODE_TYPE_FUNCTION / VARIABLE / LOCAL / CONSTANT / SUB) */
        int min;                /* minrange() and maxrange() values */
        int max;
        /* dependency */
        range_t * range;        /* min max range */
        logic_node_t * cond;
        /* expression translation */
        char expr[BUF_SIZE];    /* verbatim translation string */
        /* function argument stack */
        char args[BUF_SIZE];    /* function argument stack */
        int args_cnt;           /* function argument stack offset (top of stack) */
        int args_ptr[SUB_SIZE]; /* support up to 256 strings in stack */
        int args_ptr_cnt;
        /* expression precedence and associative */
        struct node * left;
        struct node * right;
        struct node * parent;   /* allow bridging through parent (L>P^P<R) mini diagram */
        struct node * next;
        struct node * prev;
        struct node * list;     /* arbitrary chain in sequence of creation; this allow freeing everything easier */
    } node_t;

    /* expression evaluation */
    node_t * evaluate_expression(block_r *, char *, int, int);
    node_t * evaluate_expression_recursive(block_r *, char **, int, int, logic_node_t *, int);
    int evaluate_function(block_r *, char **, char *, int, int, int *, int *, node_t *);
    void evaluate_node(node_t *, FILE *, logic_node_t *, int);
    void node_inherit_cond(node_t *);
    void node_expr_append(node_t *, node_t *, node_t *);
    void node_dmp(node_t *, FILE *);
    void node_free(node_t *);

    /* support translation */
    void argument_write(node_t *, char *);
    int translate_bonus_desc(node_t **, block_r *, bonus_t *);
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
    void script_generate_cond(logic_node_t *, FILE *, char *);
    void script_generate_and_chain(logic_node_t *, char *, int *);
    void script_generate_cond_node(logic_node_t *, char *, int *);
    void script_generate_class_generic(char *, int *, range_t *, char *);
    void script_generate_cond_generic(char *, int *, int, int, char *);
#endif
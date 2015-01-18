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
    #define EXIT_ON_ERROR 1
    #define BUF_SIZE 4096
    #define PTR_SIZE 4096  /* =.=; turns out this number isn't that bad with large scripts. */
    #define BLOCK_SIZE 32
    #define BONUS_SIZE 5
    #define BLOCK_COUNT 55  /* total number of unique blocks */
    #define BONUS_COUNT 209 /* total number of unique bonuses */
    /* script return status */
    #define SCRIPT_PASSED 0
    #define SCRIPT_FAILED 1
    /* explicit defined over hardcoded constants */
    #define BONUS_ID_MAX 5  /* bonus block id are 0 to 4; great for checking if block is a bonus block or not */
    #define BLOCK_NULLIFIED -1 /* indicates that the block is deleted; used by block->type->id */

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
    #define EVALUATE_FLAG_KEEP_LOGIC_TREE  0x01
    #define EVALUATE_FLAG_KEEP_NODE        0x02
    /* flag for evaluating relational operators to either 0 or 1
     * ignore expression simplification for conditional expression */
    #define EVALUATE_FLAG_EXPR_BOOL        0x04
    /* write the formula for expression */
    #define EVALUATE_FLAG_WRITE_FORMULA    0x08
    #define EVALUATE_FLAG_LIST_FORMULA     0x10
    /* keep logic tree for ?: operators; set blocks */
    #define EVALUATE_FLAG_KEEP_TEMP_TREE   0x20
    /* dump information when evaluating expression */
    #define EVALUATE_FLAG_DEBUG_DUMP       0x40

    /* bonus flags for block minimization */
    #define BONUS_FLAG_STACK        0x00010000  /* default: no stack */
    #define BONUS_FLAG_NODUP        0x00020000  /* default: yes duplicate */
    #define BONUS_FLAG_MINIS        0x00040000  /* minimize by expanding list */
    #define BONUS_FLAG_MINIZ        0x00080000  /* minimize by checking arguments */
    /* check a set of arguments for equality */
    #define BONUS_FLAG_EQ_1         0x00000001  /* check 1st argument are equal */
    #define BONUS_FLAG_EQ_2         0x00000002  /* check 2nd argument are equal */
    #define BONUS_FLAG_EQ_3         0x00000004  /* check 3rd argument are equal */
    #define BONUS_FLAG_EQ_4         0x00000008  /* check 4th argument are equal */
    #define BONUS_FLAG_EQ_5         0x00000010  /* check 5th argument are equal */
    /* aggregate a set of arguments */
    #define BONUS_FLAG_AG_1         0x00000100  /* aggregate 1st argument */
    #define BONUS_FLAG_AG_2         0x00000200  /* aggregate 2nd argument */
    #define BONUS_FLAG_AG_3         0x00000400  /* aggregate 3rd argument */
    #define BONUS_FLAG_AG_4         0x00000800  /* aggregate 4th argument */
    #define BONUS_FLAG_AG_5         0x00001000  /* aggregate 5th argument */
    /* stack a set of integer arguments */
    #define BONUS_FLAG_ST_1         0x01000000  /* stack 1st argument */
    #define BONUS_FLAG_ST_2         0x02000000  /* stack 2nd argument */
    #define BONUS_FLAG_ST_3         0x04000000  /* stack 3rd argument */
    #define BONUS_FLAG_ST_4         0x08000000  /* stack 4th argument */
    #define BONUS_FLAG_ST_5         0x10000000  /* stack 5th argument */

    typedef struct dep {
        char buf[BUF_SIZE];
        int cnt;
        int buf_ptr[PTR_SIZE];
        int buf_ptr_cnt;
    } dep_t;

    typedef struct {
        char script[BUF_SIZE];
        char * script_ptr[PTR_SIZE];
        int script_cnt;
    } token_r;

    /* I've gotten really lazy with proper state mangagement; 
     * I hope the comments are good enough explanation. */
    typedef struct node {
        int type;               /* see node types macros */
        int op;                 /* operator (NODE_TYPE_OPERATOR) */
        char * id;              /* identifier (NODE_TYPE_FUNCTION / VARIABLE / LOCAL / CONSTANT / SUB) */
        int min;                /* minrange() and maxrange() values */
        int max;
        /* dependency */
        range_t * range;        /* min max range */
        logic_node_t * cond;
        int cond_cnt;           /* total count of variable and functions; cascaded by operator */
        /* expression translation */
        int var_id;             /* useful for multplexing different schemes; unused */
        char expr[BUF_SIZE];    /* verbatim translation string */
        int expr_cnt;           /* size of expr */
        /* function and variable list */
        dep_t * dep;
        int complexity;         /* count the number of relational, equality, logical, or conditional operators */
        /* function argument stack */
        char args[BUF_SIZE];    /* function argument stack */
        int args_cnt;           /* function argument stack offset (top of stack) */
        int args_ptr[PTR_SIZE]; /* support up to 256 strings in stack */
        int args_ptr_cnt;
        /* expression precedence and associative */
        struct node * left;
        struct node * right;
        struct node * parent;   /* allow bridging through parent (L>P^P<R) mini diagram */
        struct node * next;
        struct node * prev;
        struct node * list;     /* arbitrary chain in sequence of creation; this allow freeing everything easier */
    } node_t;

    typedef struct block_r {
        int item_id;                         /* item to which the block belongs */
        int block_id;                        /* unique block id (index) */

        /* The argument, translation, and formula stacks 
         * arg is where all strings should be store; ptr 
         * and eng are simply pointers to the arg buffer */
        char * arg;                          /* block raw and translation stack */
        char * ptr[PTR_SIZE];                /* raw arguments */
        char * eng[PTR_SIZE];                /* translated arguments */
        char * exp[PTR_SIZE];                /* expression formula */
        char * desc;                         /* final description translation */
        int arg_cnt;                         /* various counters for the above arrays */
        int ptr_cnt;
        int eng_cnt;
        int exp_cnt;

        /* handle conditional and calculational 'dependencies' for
         * expressions with variable and function or set blocks */
        block_t * type;                      /* access to translation information in block_db.txt from block_db */
        int link;                            /* indicate conditional linking for if-else statements (index to block) */
        struct block_r * depd[BLOCK_SIZE];   /* indicate calculation linking for set blocks */
        int depd_cnt;                        /* number of dependencies */
        
        /* set_* variable allows a block to be inherited as a
         * node, which can be used when evaluating expressions. */
        int set_min;
        int set_max;
        int set_cond_cnt;
        dep_t * set_dep;
        char set_expr[BUF_SIZE];             /* contain the formula for the set block */
        int set_complexity;                  /* indicate that the set block contains a complex expression */

        /* flag and offset are use for variable length arguments in functions */
        int flag;                            /* multi-purpose flag for special conditions 
                                                0x01 - expanded the range of possible argument, i.e. callfunc(F_Rand, 1, 2, ..)
                                                0x02 - multivalues must be tagged random 
                                                0x04 - use verbatim string */
        int offset;                          /* indicate the beginning of special arguments */
        logic_node_t * logic_tree;           /* calculational and dependency information */

        /* bonus blocks will defer writing the final item description until script_bonus */
        node_t * result[BONUS_SIZE];         /* keep until after minimization */
        bonus_t bonus;                       /* ahhhhhhhhhhhhhhhhhhhhhhhhhhhh */
    } block_r;

    int global_mode;                /* indicates eathena, rathena, or hercules mode */
    struct ic_db_t * global_db;     /* global sqlite3 database reference */
    char global_err[BUF_SIZE];      /* static buffer to hold the error string */
    FILE * node_dbg;                /* file to dump evaluate expression */
    #define exit_buf(X, ...) exit_abt(build_buffer(global_err, (X), __VA_ARGS__))

    /* block structure memory management functions */
    void block_init(block_r **, int);
    void block_deinit(block_r *, int);
    void block_finalize(block_r *, int);

    /* compilation processes; exported functions, api functions */
    int script_lexical(token_r *, char *);
    int script_analysis(token_r *, block_r *, int *, int, int, int);
    int script_parse(token_r *, int *, block_r *, char, char, int);
    int script_extend(block_r *, char *);
    int script_dependencies(block_r *, int);
    int script_translate(block_r *, int);
    int script_bonus(block_r *, int);
    int script_generate(block_r *, int, char *, int *);
    char * script_compile_raw(char *, int, FILE *);     /* high level functions compiles from lexical to generate */
    #define script_compile(X, Y) script_compile_raw(X, Y, NULL)
    
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
    int translate_bonus_script(block_r *);
    int translate_setfalcon(block_r *);
    int translate_write(block_r *, char *, int);
    int translate_overwrite(block_r *, char *, int);

    /* debug compiler by dumping the block list */
    void block_debug_dump_all(block_r *, int, FILE *);
    void block_debug_dump_set_link(struct block_r **, int, int, int, FILE *);

    /* expression evaluation */
    node_t * evaluate_expression(block_r *, char *, int, int);
    node_t * evaluate_expression_recursive(block_r *, char **, int, int, logic_node_t *, int);
    int evaluate_function(block_r *, char **, char *, int, int, int *, int *, node_t *);
    int evaluate_node(node_t *, FILE *, logic_node_t *, int, int *);
    void node_inherit_cond(node_t *);
    void node_expr_append(node_t *, node_t *, node_t *);
    char * formula(char *, char *, node_t *);
    int formula_write(block_r *, char *);
    char * status_formula(char *, char *, node_t *, int, int);
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

    /* support generation */
    int script_generate_cond(logic_node_t *, FILE *, char *, char *, int *);
    int script_generate_and_chain(logic_node_t *, char *, int *);
    int script_generate_cond_node(logic_node_t *, char *, int *);
    int script_generate_class_generic(char *, int *, range_t *, char *);
    int script_generate_cond_generic(char *, int *, int, int, char *);

    /* support minimization */
    int minimize_stack(node_t * left, node_t * right);
#endif
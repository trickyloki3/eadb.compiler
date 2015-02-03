/*
 *   file: name_range.h
 *   date: 12/9/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef NAME_RANGE_H
#define NAME_RANGE_H
   #include <stdio.h>
   #include <stdlib.h>
   #include "util.h"
   #include "range.h"
   #define BUF_SIZE 4096
   #define SUB_SIZE 256
   #define LOGIC_NODE_COND 1
   #define LOGIC_NODE_AND  2
   #define LOGIC_NODE_OR   3 
   
   #define COND_COND_CASE(X,Y)   ((X)->type == LOGIC_NODE_COND && (Y)->type == LOGIC_NODE_COND)
   #define COND_AND_CASE(X,Y)    ((X)->type == LOGIC_NODE_COND && (Y)->type == LOGIC_NODE_AND)
   #define AND_COND_CASE(X,Y)    ((X)->type == LOGIC_NODE_AND && (Y)->type == LOGIC_NODE_COND)
   #define COND_OR_CASE(X,Y)     ((X)->type == LOGIC_NODE_COND && (Y)->type == LOGIC_NODE_OR)
   #define OR_COND_CASE(X,Y)     ((X)->type == LOGIC_NODE_OR && (Y)->type == LOGIC_NODE_COND)
   #define AND_OR_CASE(X,Y)      ((X)->type == LOGIC_NODE_AND && (Y)->type == LOGIC_NODE_OR)
   #define OR_AND_CASE(X,Y)      ((X)->type == LOGIC_NODE_OR && (Y)->type == LOGIC_NODE_AND)
   #define AND_AND_CASE(X,Y)     ((X)->type == LOGIC_NODE_AND && (Y)->type == LOGIC_NODE_AND)
   #define OR_OR_CASE(X,Y)       ((X)->type == LOGIC_NODE_OR && (Y)->type == LOGIC_NODE_OR)
   #define INVALID_NODE_TYPE(X)  (X < LOGIC_NODE_COND || X > LOGIC_NODE_OR)
   #define INVALID_NODE(X, Y)    ((X) == NULL || (Y) == NULL)

   typedef struct logic_node {
      int type;            /* or, and, or cond */
      char * name;         /* unique name */
      range_t * range;     /* range of values */
      struct logic_node * parent;
      struct logic_node * left;
      struct logic_node * right;
      struct logic_node * stack;
   } logic_node_t;

   logic_node_t * make_cond(char *, range_t *, logic_node_t * func);
   void dmpcond(logic_node_t *, FILE *);
   void free_cond(logic_node_t *);
   logic_node_t * new_cond(int, logic_node_t *, logic_node_t *);
   logic_node_t * base_cond_cond(int, logic_node_t *, logic_node_t *);
   logic_node_t * AND_cond_and(logic_node_t *, logic_node_t *);
   logic_node_t * AND_cond_or(logic_node_t *, logic_node_t *);
   logic_node_t * AND_or_and(logic_node_t *, logic_node_t *);
   logic_node_t * AND_and_and(logic_node_t *, logic_node_t *);
   logic_node_t * AND_and_and_iter(logic_node_t *, logic_node_t **);
   logic_node_t * AND_or_or(logic_node_t *, logic_node_t *);
   logic_node_t * OR_cond_and(logic_node_t *, logic_node_t *);
   logic_node_t * OR_cond_or(logic_node_t *, logic_node_t *);
   logic_node_t * OR_and_or(logic_node_t *, logic_node_t *);
   logic_node_t * OR_and_and(logic_node_t *, logic_node_t *);
   logic_node_t * OR_or_or(logic_node_t *, logic_node_t *);
   void search_identifier(logic_node_t *, char *, logic_node_t **);
   void dmpnamerange(logic_node_t *, FILE *, int);
   void deepdmpnamerange(logic_node_t *, FILE *, char *);
   void freenamerange(logic_node_t *);
   void deepfreenamerange(logic_node_t *);
   logic_node_t * copy_and_tree(logic_node_t *);
   logic_node_t * copy_or_tree(logic_node_t *);
   logic_node_t * copy_any_tree(logic_node_t *);
   logic_node_t * copy_deep_any_tree(logic_node_t *);
   range_t * search_tree_dependency(logic_node_t *, char *, range_t *);
   void search_tree_name_range(logic_node_t *, char *, range_t *, range_t **);
   logic_node_t * inverse_logic_tree(logic_node_t *);
   logic_node_t * deep_inverse_logic_tree(logic_node_t *);
#endif

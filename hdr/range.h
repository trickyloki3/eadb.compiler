/*
 *   file: range.h
 *   date: 12/9/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef RANGE_H
#define RANGE_H
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <limits.h>
   #define SET_DEBUG 0
   #define DONT_CARE 0
   #define INIT_OPERATOR '*'
   #define LT_OPERATOR '>'
   #define LE_OPERATOR '>' + '='
   #define GT_OPERATOR '<'
   #define GE_OPERATOR '<' + '='
   #define EQ_OPERATOR '=' + '='
   #define NE_OPERATOR '!' + '='
   /* non-empty */
   #define IS_ZERO_RANGE(X)         ((X)->min == 0 && (X)->max == 0)
   /* min and max of two numbers */
   #define MIN_RANGE(X, Y)          (((X)->id_min < (Y)->id_min)?(X)->id_min:(Y)->id_min)
   #define MAX_RANGE(X, Y)          (((X)->id_max > (Y)->id_max)?(X)->id_max:(Y)->id_max)
   /* is X equal to Y */
   #define IS_EQUALSET(X, Y)        ((X)->min == (Y)->min && (X)->max == (Y)->max)
   /* is X a subset of Y */
   #define IS_SUBSET_OF(X, Y)       ((Y)->min <= (X)->min && (X)->max <= (Y)->max)
   /* is X overlapping Y */
   #define IS_OVERLAP_LEFT(X, Y)    ((Y)->min <= (X)->min && (X)->min <= (Y)->max)
   #define IS_OVERLAP_RIGHT(X, Y)   ((Y)->min <= (X)->max && (X)->max <= (Y)->max)
   /* disjoint is the inverse of overlap; added for completeness */
   #define IS_DISJOINT_LEFT(X, Y)   !(IS_OVERLAP_LEFT(X, Y))
   #define IS_DISJOINT_RIGHT(X, Y)  !(IS_OVERLAP_RIGHT(X, Y))
   /* is [x1, x2] and [y1, y2] such that x2 + 1 = y1 or x2 = y1 + 1 */
   #define IS_BOUNDARY(X, Y)        (((X)->max + 1 == (Y)->min) || ((X)->max == (Y)->min + 1))

   /* specifies a x to y range set */
   typedef struct range {
      char * id;
      int min;
      int max;
      int id_min; /* absolute minimum limit of the identifier */
      int id_max; /* absolute maximum limit of the identifier */
      struct range * prev;
      struct range * next;
   } range_t;

   range_t * mkrange(int, int, int, int);
   range_t * andrange(range_t *, range_t *);
   range_t * orrange(range_t *, range_t *);
   range_t * notrange(range_t *);
   range_t * sortrange(range_t *);
   range_t * negaterange(range_t *);
   void swaprange(int *, int *);
   void negateminmax(range_t *);
   int minrange(range_t *);
   int maxrange(range_t *);
   void mergerange(range_t *);
   void mergerangejoint(range_t *);
   range_t * copyrange(range_t *);
   void freerange(range_t *);
   void dmprange(range_t *, FILE *, char *);
   range_t * calcrange(int, range_t *, range_t *);
   void fliprange(range_t *);
   void calcrangemin(int, range_t *, int);
   void calcrangemax(int, range_t *, int);
#endif

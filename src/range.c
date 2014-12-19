/*
 *   file: range.c
 *   date: 12/9/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "range.h"

/* range_mk is called by a relational and equality operator handlers */
range_t * mkrange(int operator, int min, int max, int val) {
   int swap = 0;
   range_t * tmp = NULL;
   range_t * tmp_2 = NULL;
   range_t * iter = NULL;
   range_t * temp = NULL;

   /* check bounds on min and max return empty range */
   if(min > max) {
      fprintf(stderr,"warning: swapping min(%d) and max(%d) in mkrange; handle negative operand.\n", min, max);
      swap = min;
      min = max;
      max = swap;
      /*fprintf(stderr,"fatal: min greater than max in mkrange; min(%d) max(%d).\n", min, max);
      exit(EXIT_FAILURE);*/
   }

   /* generate the range set */
   tmp = calloc(1, sizeof(range_t));
   tmp->id_min = min;
   tmp->id_max = max;
   switch(operator) {
      case INIT_OPERATOR:
         tmp->min = min;
         tmp->max = max;
         break;
      case LT_OPERATOR: /* (min, max) > val => (val + 1, max) */
         tmp->min = val + 1;
         tmp->max = max;
         break;
      case LE_OPERATOR: /* (min, max) >= val => (val, max) */
         tmp->min = val;
         tmp->max = max;
         break;
      case GT_OPERATOR: /* (min, max) < val => (min, val - 1) */
         tmp->min = min;
         tmp->max = val - 1;
         break;
      case GE_OPERATOR: /* (min, max) <= val => (min, val) */
         tmp->min = min;
         tmp->max = val;
         break;
      case EQ_OPERATOR: /* (min, max) == val => (val, val) */
         tmp->min = val;
         tmp->max = val;
         break;
      case NE_OPERATOR: /* (min, max) != val => (min, val - 1) and (val + 1, max) */
         /* special case if val == min or max */
         if(val == min) {
            tmp->min = min + 1;
            tmp->max = max;
         } else if(val == max) {
            tmp->min = min;
            tmp->max = max - 1;
         } else {
            tmp_2 = calloc(1, sizeof(range_t));
            tmp_2->id_min = min;
            tmp_2->id_max = max;
            tmp->min = min;
            tmp->max = val - 1;
            tmp->next = tmp_2; /* (min, val - 1) => (val + 1, max) */
            tmp_2->min = val + 1;
            tmp_2->max = max;
            tmp_2->prev = tmp;
            tmp_2->next = NULL;
         }
         break;
      default: 
         /* non relational or equality operator invocation is fatal error */
         fprintf(stderr,"range_mk; invalid operator; %d\n", operator);
         exit(EXIT_FAILURE);
   }

   /* check underflow and overflow; the zero-one check */
   iter = tmp;
   while(iter != NULL) {
      /* root failed zero-one check; reset root node */
      if(iter->min > iter->max) {
         /* remove the root node */
         if(iter == tmp) {
            tmp = tmp->next;
            free(iter);
            iter = tmp;
         /* remove the tail node */
         } else if(iter->next == NULL) {
            temp = iter;
            iter->prev->next = NULL;
            iter = NULL;
            free(temp);
         /* remove intermediate node */
         } else {
            iter->prev->next = iter->next;
            iter->next->prev = iter->prev;
            temp = iter;
            iter = iter->next;
            free(temp);
         }
      /* passed the zero-one check */
      } else {
         iter = iter->next;
      }
   }

   if(tmp == NULL) {
      /*fprintf(stderr,"warning: empty range in mkrange; return 0 ~ 0; %d %c %d ~ %d\n", val, operator, min, max);*/
      tmp = mkrange(INIT_OPERATOR, 0, 0, DONT_CARE);
   }

   return tmp;
}
/* set union is called by || operator handler */
range_t * orrange(range_t * range_1, range_t * range_2) {
   range_t * root = NULL;
   range_t * iter = NULL;   /* append to the end of the list */
   range_t * temp = NULL;   /* temporary created node */
   range_t * iter_1 = NULL; /* iterate range_1 and range_2 */
   range_t * iter_2 = NULL;

   if(range_1 == NULL || range_2 == NULL) {
      fprintf(stderr,"fatal: null range_1 or range_2 in orrange.\n");
      exit(EXIT_FAILURE);
   }

   /* find union ranges */
   for(iter_1 = range_1; iter_1 != NULL; iter_1 = iter_1->next) {
      for(iter_2 = range_2; iter_2 != NULL; iter_2 = iter_2->next) {
         if(IS_EQUALSET(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_1->min;
            temp->max = iter_1->max;
            if(SET_DEBUG)
               fprintf(stderr,"orrange X[%d ~ %d] and Y[%d ~ %d]; X same as Y; [%d ~ %d] union.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* [x1 ~ x2] and [y1 ~ y2], where y1 <= x1 and x2 <= y2 
          * union range must therefore be y1 to y2; the 
          * larger of the two. */
         } else if(IS_SUBSET_OF(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_2->min;
            temp->max = iter_2->max;
            if(SET_DEBUG)
               fprintf(stderr,"orrange X[%d ~ %d] and Y[%d ~ %d]; X subset Y; [%d ~ %d] union.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         } else if(IS_SUBSET_OF(iter_2, iter_1)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_1->min;
            temp->max = iter_1->max;
            if(SET_DEBUG)
               fprintf(stderr,"orrange X[%d ~ %d] and Y[%d ~ %d]; Y subset X; [%d ~ %d] union.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* check if overlap on the left, i.e. 
          * [x1 ~ x2] and [y1 ~ y2] where y1 <= x1 <= y2
          * union range must therefore be x1 to y2 */
         } else if(IS_OVERLAP_LEFT(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_2->min;
            temp->max = iter_1->max;
            if(SET_DEBUG)
               fprintf(stderr,"orrange X[%d ~ %d] and Y[%d ~ %d] left overlap; [%d ~ %d] union.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* check if overlap on the right, i.e.
          * [x1 ~ x2] and [y1 ~ y2] where y1 <= x2 <= y2
          * union range must therefore be y1 to x2 */
         } else if(IS_OVERLAP_RIGHT(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_1->min;
            temp->max = iter_2->max;
            if(SET_DEBUG)
               fprintf(stderr,"orrange X[%d ~ %d] and Y[%d ~ %d] right overlap; [%d ~ %d] union.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* discard all disjoin set and don't modify the root */
         } else {
            /* append the first one */
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_1->min;
            temp->max = iter_1->max;
            temp->id_min = (iter_1->id_min < iter_2->id_min)?iter_1->id_min:iter_2->id_min;
            temp->id_max = (iter_1->id_max > iter_2->id_max)?iter_1->id_max:iter_2->id_max;
            /* set the root node or append to the end of the list */
            if(root == NULL) {
               temp->prev = NULL;
               iter = root = temp;
            } else {
               temp->prev = iter;
               iter = iter->next = temp;
            }
            /* append the second one at the end of the loop */
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_2->min;
            temp->max = iter_2->max;
            temp->id_min = (iter_1->id_min < iter_2->id_min)?iter_1->id_min:iter_2->id_min;
            temp->id_max = (iter_1->id_max > iter_2->id_max)?iter_1->id_max:iter_2->id_max;
            if(SET_DEBUG)
               fprintf(stderr,"orrange X[%d ~ %d] and Y[%d ~ %d] disjoint; both union.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max);
         }

         /* set the id_min and id_max; both side should be the same */
         temp->id_min = (iter_1->id_min < iter_2->id_min)?iter_1->id_min:iter_2->id_min;
         temp->id_max = (iter_1->id_max > iter_2->id_max)?iter_1->id_max:iter_2->id_max;

         /* set the root node or append to the end of the list */
         if(root == NULL) {
            temp->prev = NULL;
            iter = root = temp;
         } else {
            temp->prev = iter;
            iter = iter->next = temp;
         }
      }
   }

   /* visualize by drawing multiple parallel lines relative to some point;
      each line consist of two points on a number line relative to some point;
      sort each line by the leftmost point (sortrange)
      merge all the overlapping lines, e.g
               0 - 1 - 2 - 3 - 4 - 5
      line 1     * - *
      line 2               * - *
      line 3           * - - - - - *

      => (sortrange)

      line 1     * - *
      line 3           * - - - - - *
      line 2               * - *

      => (mergerange)

      line 1     * - *
      line 3           * - - - - - *

      => (mergejoin)

      line 4     * - - - - - - - - *
    */

   /* sort ranges by minimum */
   root = sortrange(root);
   /* merge overlapping nodes */
   mergerange(root);
   /* merge nodes on boundary of each other */
   mergerangejoint(root);

   /* orrange should never return NULL if both arguments are not NULL */
   if(root == NULL) {
      fprintf(stderr,"fatal: orrange returning null root.\n");
      exit(EXIT_FAILURE);
   }
   return root;
}
/* set_intersect is called by && operator handler */
range_t * andrange(range_t * range_1, range_t * range_2) {
   range_t * root = NULL;
   range_t * iter = NULL;   /* append to the end of the list */
   range_t * temp = NULL;   /* temporary created node */
   range_t * iter_1 = NULL; /* iterate range_1 and range_2 */
   range_t * iter_2 = NULL;

   if(range_1 == NULL || range_2 == NULL) {
      fprintf(stderr,"fatal: null range_1 or range_2 in andrange.\n");
      exit(EXIT_FAILURE);
   }

   /* find intersecting ranges */
   for(iter_1 = range_1; iter_1 != NULL; iter_1 = iter_1->next) {
      for(iter_2 = range_2; iter_2 != NULL; iter_2 = iter_2->next) {
         if(IS_EQUALSET(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_1->min;
            temp->max = iter_1->max;
            if(SET_DEBUG)
               fprintf(stderr,"andrange X[%d ~ %d] and Y[%d ~ %d]; X same as Y; [%d ~ %d] intersect.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* [x1 ~ x2] and [y1 ~ y2], where y1 <= x1 and x2 <= y2 
          * intersect range must therefore be x1 to x2; the 
          * smaller of the two. */
         } else if(IS_SUBSET_OF(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_1->min;
            temp->max = iter_1->max;
            if(SET_DEBUG)
               fprintf(stderr,"andrange X[%d ~ %d] and Y[%d ~ %d]; X subset Y; [%d ~ %d] intersect.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         } else if(IS_SUBSET_OF(iter_2, iter_1)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_2->min;
            temp->max = iter_2->max;
            if(SET_DEBUG)
               fprintf(stderr,"andrange X[%d ~ %d] and Y[%d ~ %d]; Y subset X; [%d ~ %d] intersect.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* check if overlap on the left, i.e. 
          * [x1 ~ x2] and [y1 ~ y2] where y1 <= x1 <= y2
          * intersect range must therefore be x1 to y2 */
         } else if(IS_OVERLAP_LEFT(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_1->min;
            temp->max = iter_2->max;
            if(SET_DEBUG)
               fprintf(stderr,"andrange X[%d ~ %d] and Y[%d ~ %d] left overlap; [%d ~ %d] intersect.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* check if overlap on the right, i.e.
          * [x1 ~ x2] and [y1 ~ y2] where y1 <= x2 <= y2
          * intersect range must therefore be y1 to x2 */
         } else if(IS_OVERLAP_RIGHT(iter_1, iter_2)) {
            temp = calloc(1, sizeof(range_t));
            temp->min = iter_2->min;
            temp->max = iter_1->max;
            if(SET_DEBUG)
               fprintf(stderr,"andrange X[%d ~ %d] and Y[%d ~ %d] right overlap; [%d ~ %d] intersect.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max, temp->min, temp->max);
         /* discard all disjoin set and don't modify the root */
         } else {
            if(SET_DEBUG)
               fprintf(stderr,"andrange X[%d ~ %d] and Y[%d ~ %d] disjoint; NULL intersect.\n",
               iter_1->min, iter_1->max, iter_2->min, iter_2->max);
            continue;
         }

         /* set the id_min and id_max; both side should be the same */
         temp->id_min = (iter_1->id_min < iter_2->id_min)?iter_1->id_min:iter_2->id_min;
         temp->id_max = (iter_1->id_max > iter_2->id_max)?iter_1->id_max:iter_2->id_max;

         /* set the root node or append to the end of the list */
         if(root == NULL) {
            temp->prev = NULL;
            iter = root = temp;
         } else {
            temp->prev = iter;
            iter = iter->next = temp;
         }
      }
   }

   /* andrange would produce null if none of the intersection overlap */
   if(root == NULL) {
      /*fprintf(stderr,"warning: empty range in andrange; return 0 ~ 0.\n");
      dmprange(range_1, stdout, "and left operand");
      dmprange(range_2, stdout, "and right operand");*/
      root = mkrange(INIT_OPERATOR, 0, 0, DONT_CARE);
   }
   return root;
}
/* inverse the range */
range_t * notrange(range_t * root) {
   range_t * new_root = NULL;
   range_t * new_root_iter = NULL;
   range_t * iter = NULL;
   range_t * temp = NULL;

   /* special case variables */
   range_t * left = NULL;
   range_t * right = NULL;

   /* ignore if the root is NULL */
   if(root == NULL) 
      return NULL;
   else
      iter = root;

   /* special case if only one node exist; compare with limit boundaries (absolute min and max) */
   if(iter->next == NULL) {
      if(iter->min > iter->id_min) {
         left = calloc(1, sizeof(range_t));
         left->min = iter->id_min;
         left->max = iter->min - 1;
         left->id_min = iter->id_min;
         left->id_max = iter->id_max;
      }
      if(iter->max < iter->id_max) {
         right = calloc(1, sizeof(range_t));
         right->min = iter->max + 1;
         right->max = iter->id_max;
         right->id_min = iter->id_min;
         right->id_max = iter->id_max;
      }
      if(iter->min == 0 && iter->max == 0) {
         left = calloc(1, sizeof(range_t));
         left->min = 1;
         left->max = 1;
         left->id_min = 0;
         left->id_max = 1;  
      }

      /* special case; the inverse of the entire set equals no set */
      if(left == NULL && right == NULL) {
         /*fprintf(stderr,"warning: empty range in notrange; return 0 ~ 0.\n");
         dmprange(root, stdout, "empty range");*/
         return mkrange(INIT_OPERATOR, 0, 0, DONT_CARE);
      /* only left side is valid */
      } else if(left != NULL && right == NULL) {
         new_root = left;
         left->next = NULL;
         return new_root;
      } else if(left == NULL && right != NULL) {
         new_root = right;
         right->next = NULL;
         return new_root;
      } else {
         new_root = left;
         left->next = right;
         right->next = NULL;
         return new_root;
      }
   }

   /* handle the head node */
   if(iter->min > iter->id_min) {
      left = calloc(1, sizeof(range_t));
      left->min = iter->id_min;
      left->max = iter->min - 1;
      left->id_min = iter->id_min;
      left->id_max = iter->id_max;
      new_root_iter = new_root = left;
   }
   /* handle the inbetween node */
   while(iter->next != NULL) {
      /* check if not the tail */
      if(iter->next == NULL) break;
      /* add the inbetween partition */
      temp = calloc(1, sizeof(range_t));
      temp->min = iter->max + 1;
      temp->max = iter->next->min - 1;
      temp->id_min = iter->id_min;
      temp->id_max = iter->id_max;
      /* add the node to the new list */
      if(new_root == NULL) 
         new_root_iter = new_root = temp;
      else
         new_root_iter = new_root_iter->next = temp;
      /* evaluate the next node */
      iter = iter->next;
   }
   
   /* handle the tail node */
   if(iter->max < iter->id_max) {
      right = calloc(1, sizeof(range_t));
      right->min = iter->max + 1;
      right->max = iter->id_max;
      right->id_min = iter->id_min;
      right->id_max = iter->id_max;
      if(new_root_iter != NULL)
         new_root_iter = new_root_iter->next = right;
      else
         new_root = right;
   }
   if(new_root == NULL) {
      /*fprintf(stderr,"warning: empty range in notrange; return 0 ~ 0.\n");
      dmprange(root, stdout, "empty range");*/
      new_root = mkrange(INIT_OPERATOR, 0, 0, DONT_CARE);
   }
   return new_root;
}

range_t * negaterange(range_t * root) {
   range_t * new_root = NULL;
   range_t * iter = copyrange(root);
   range_t * temp = NULL;

   while(iter != NULL) {
      negateminmax(iter);
      temp = iter;
      iter = iter->next;
      /* reverse the list */
      if(new_root == NULL) {
         new_root = temp;
         new_root->next = NULL;
         new_root->prev = NULL;
      } else {
         temp->next = new_root;
         temp->prev = NULL;
         new_root->prev = temp;
         new_root = temp;
      }
   }
   return new_root;
}

void negateminmax(range_t * root) {
   /* swap all the min and max */
   root->id_min *= -1;
   root->id_max *= -1;
   swaprange(&root->id_min, &root->id_max);

   root->min *= -1;
   root->max *= -1;
   swaprange(&root->min, &root->max);
}

void swaprange(int * a, int * b) {
   int temp = *a;
   *a = *b;
   *b = temp;
}

/* sort the node by min */
range_t * sortrange(range_t * root) {
   range_t * new_root = NULL;
   range_t * new_iter = NULL;
   range_t * iter = NULL;
   range_t * smallest = NULL;
   range_t head;
   range_t tail;
   /* initialize head and tail node */
   memset(&head, 0, sizeof(range_t));
   memset(&tail, 0, sizeof(range_t));
   /* set the head and tail */
   head.next = root;
   root->prev = &head;
   iter = root;
   while(iter != NULL) {
      if(iter->next == NULL) {
         iter->next = &tail;
         tail.prev = iter;
         break;
      }
      iter = iter->next;
   }

   /* sort the nodes by min */
   while(head.next != &tail) {
      /* search for the smallest node */
      iter = smallest = head.next;
      while(iter != &tail) {
         if(smallest->min > iter->min)
            smallest = iter;
         iter = iter->next;
      }

      /* remove smallest from the list */
      smallest->prev->next = smallest->next;
      smallest->next->prev = smallest->prev;

      /* append smallest to the new root */
      if(new_root == NULL) {
         new_iter = new_root = smallest;
         new_iter->next = NULL;
         new_iter->prev = NULL;
      } else {
         new_iter->next = smallest;
         smallest->prev = new_iter;
         new_iter = smallest;
         new_iter->next = NULL;
      }
   }
   return new_root;
}
/* forward merge overlapping ranges */
void mergerange(range_t * root) {
   range_t * iter = NULL;
   range_t * temp = NULL;
   iter = root;
   while(iter != NULL) {
      if(iter->next != NULL && iter->max >= iter->next->min) {
         if(SET_DEBUG)
            printf("mergerange; [%d ~ %d] merging with [%d ~ %d]\n", 
            iter->min, iter->max, iter->next->min, iter->next->max);
         temp = iter->next;
         iter->max = (iter->max < iter->next->max)?iter->next->max:iter->max;

         iter->id_min = (iter->id_min < iter->next->id_min)?iter->id_min:iter->next->id_min;
         iter->id_max = (iter->id_max > iter->next->id_max)?iter->id_max:iter->next->id_max;

         if(iter->next->next != NULL) {
            iter->next = iter->next->next;
            iter->next->prev = iter;
            if(SET_DEBUG) 
               printf("iter->next: %p\niter->next->prev: %p\n", 
               (void *) iter->next, (void *) iter->next->prev);
         } else {
            iter->next = NULL;
         }
         
         free(temp);
      } else {
         iter = iter->next;
      }
   }
}
/* forward merge the boundaries */
void mergerangejoint(range_t * root) {
range_t * iter = NULL;
   range_t * temp = NULL;
   iter = root;
   while(iter != NULL) {
      if(iter->next != NULL && IS_BOUNDARY(iter, iter->next)) {
         if(SET_DEBUG)
            printf("mergerange; [%d ~ %d] merging with [%d ~ %d]\n", 
            iter->min, iter->max, iter->next->min, iter->next->max);
         temp = iter->next;
         iter->max = iter->next->max;

         iter->id_min = (iter->id_min < iter->next->id_min)?iter->id_min:iter->next->id_min;
         iter->id_max = (iter->id_max > iter->next->id_max)?iter->id_max:iter->next->id_max;

         if(iter->next->next != NULL) {
            iter->next = iter->next->next;
            iter->next->prev = iter;
            if(SET_DEBUG) 
               printf("iter->next: %p\niter->next->prev: %p\n", 
               (void *) iter->next, (void *) iter->next->prev);
         } else {
            iter->next = NULL;
         }
         
         free(temp);
      } else {
         iter = iter->next;
      }
   }  
}
int minrange(range_t * root) {
   range_t * iter = root;
   int min = iter->min;
   while(iter != NULL) {
      if(iter->min < min)
         min = iter->min;
      iter = iter->next;
   }
   return min;
}
int maxrange(range_t * root) {
   range_t * iter = root;
   int max = iter->max;
   while(iter != NULL) {
      if(iter->max > max)
         max = iter->max;
      iter = iter->next;
   }
   return max;
}
range_t * copyrange(range_t * root) {
   struct range * new_root = NULL;
   struct range * list = NULL;
   struct range * temp = NULL;
   struct range * iter = root;
   while(iter != NULL) {
      /* add the node the new chain */
      temp = calloc(1, sizeof(struct range));
      temp->min = iter->min;
      temp->max = iter->max;
      temp->id_min = iter->id_min;
      temp->id_max = iter->id_max;
      if(new_root == NULL)
         list = new_root = temp;
      else
         list = list->next = temp;
      iter = iter->next;
   }
   return new_root;
}
void freerange(range_t * root) {
   struct range * temp = NULL;
   struct range * iter = root;
   while(iter != NULL) {
      temp = iter;
      iter = iter->next;
      free(temp);
   }
}
/* display the ranges */
void dmprange(range_t * range_1, FILE * stm, char * tag) {
   range_t * iter = NULL;
   fprintf(stm,"dmprange; %s ", tag);
   if(range_1 != NULL) {
      for(iter = range_1; iter != NULL; iter = iter->next)
         /*fprintf(stm,"[%d ~ %d | (%d ~ %d)](%p)%s", iter->id_min, iter->id_max, iter->min, iter->max, (void *) iter, (iter->next == NULL)?"\n":" U ");*/
         fprintf(stm,"(%d ~ %d) %s", iter->min, iter->max, (iter->next == NULL)?"\n":" U ");
   } 
   /*else {
      fprintf(stderr,"fatal: null range in dmprange.\n");
      exit(EXIT_FAILURE);
   }*/
}

range_t * calcrange(int operator, range_t * range_1, range_t * range_2) {
   /*int min_1 = 0;*/
   int min_2 = 0;
   /*int max_1 = 0;*/
   int max_2 = 0;
   int min_g = 0;
   int max_g = 0;
   range_t * temp_1 = NULL;
   range_t * temp_2 = NULL;
   range_t * result = NULL;

   if(range_1 == NULL || range_2 == NULL) {
      fprintf(stderr, "fatal: null range_1 or range_2 in calcrange.\n");
      exit(EXIT_FAILURE);
   }

   /* copy the range; don't modify the original */
   range_1 = copyrange(range_1);
   range_2 = copyrange(range_2);
   /*min_1 = minrange(range_1);*/
   /*max_1 = maxrange(range_1);*/
   min_2 = minrange(range_2);
   max_2 = maxrange(range_2);
   min_g = MIN_RANGE(range_1, range_2);
   max_g = MAX_RANGE(range_1, range_2);
   switch(operator) {
      /* require min and max iteration */
      /* commutative; a + b = b + a*/
      case '*': 
      case '/': 
      case '+': 
      case '-': 
      case '&': 
      case '|':
         /* perform calculation on respective min and max */
         calcrangemin(operator, range_1, min_2);
         calcrangemax(operator, range_1, max_2);
         result = copyrange(range_1);
         break; 
      /* simply say that one set is not in the other;
       * zero - indicate that the condition is false
       * nonzero - indicate the valid range of values
       * should only use in context of variables / functions
       * ALL VALUES CALCULATED RELATED TO LEFT!  */
      case '<':
      case '<' + '=':
         temp_1 = mkrange(operator, min_g, max_g, min_2);
         result = andrange(range_1, temp_1);
         freerange(temp_1);
         break;
      case '>' + '=':
      case '>':
         temp_1 = mkrange(operator, min_g, max_g, max_2);
         result = andrange(range_1, temp_1);
         freerange(temp_1);
         break;
      case '!' + '=':
         temp_1 = andrange(range_1, range_2);
         temp_2 = notrange(temp_1);
         result = andrange(range_1, temp_2);
         freerange(temp_1);
         freerange(temp_2);
         break;
      case '=' + '=':
         result = andrange(range_1, range_2);
         break;
      /* natively support by range */
      case '&' + '&': result = andrange(range_1, range_2); break;
      case '|' + '|': result = orrange(range_1, range_2); break;
      default:
         fprintf(stderr,"fatal: invalid operator to calcrange.\n");
         exit(EXIT_FAILURE);
   }

   /* free a copy of the range_1 and range_2 pass into function */
   freerange(range_1);
   freerange(range_2);
   if(result == NULL) {
      fprintf(stderr,"warn: calcrange return null; defaulting to 0 ~ 0.\n");
      result = mkrange(INIT_OPERATOR, 0, 0, DONT_CARE);
   }
   return result;
}

void fliprange(range_t * range_1) {
   int min_1 = minrange(range_1);
   int max_1 = maxrange(range_1);
   if(min_1 > max_1) {
      range_1->min = max_1;
      range_1->max = min_1;
   }
}

void calcrangemin(int operator, range_t * range_1, int min) {
   range_t * iter = range_1;
   while(iter != NULL) {
      switch(operator) {
         case '*': iter->min *= min; break;
         case '/': iter->min /= (min)?min:1; break;
         case '+': iter->min += min; break;
         case '-': iter->min -= min; break;
         case '&': iter->min &= min; break;
         case '|': iter->min |= min; break;
         default:
            fprintf(stderr,"fatal: invalid operator to calcminrange.\n");
            exit(EXIT_FAILURE);
      }
      iter = iter->next;
   }
}

void calcrangemax(int operator, range_t * range_1, int max) {
   range_t * iter = range_1;
   while(iter != NULL) {
      switch(operator) {
         case '*': iter->max *= max; break;
         case '/': iter->max /= (max)?max:1; break;
         case '+': iter->max += max; break;
         case '-': iter->max -= max; break;
         case '&': iter->max &= max; break;
         case '|': iter->max |= max; break;
         default:
            fprintf(stderr,"fatal: invalid operator to calcmaxrange.\n");
            exit(EXIT_FAILURE);
      }
      iter = iter->next;
   }
}

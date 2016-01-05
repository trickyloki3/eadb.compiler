/*
 *   file: util.c
 *   date: 5/09/2014
 * update: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "util.h"

/* global error buffer */
char err_buf[BUF_SIZE];
int exit_echo = 1;

/* write formatted string into global error buffer */
char * exit_msg(char * msg, char * format, ...) {
   int offset = 0;
   va_list list;
   va_start(list, format);
   offset = vsprintf(msg, format, list);
   msg[offset] = '\0';
   va_end(list);
   return msg;
}

/* check whether variable arguments are NULL */
int exit_null(const char * file_name, const char * function_name, const int line_number, int argc, ...) {
   int i = 0;
   va_list list;
   va_start(list, argc);
   for(i = 0; i < argc; i++) {
      if(va_arg(list, void *) == NULL) {
         fprintf(stderr,
            "[warn]: %s;%s;%d; null detected in position %d.\n",
            file_name, function_name, line_number, i + 1);
         va_end(list);
#if ENABLE_EXIT
         exit(EXIT_FAILURE);
#else
         return CHECK_FAILED;
#endif
      }
   }
   va_end(list);
   return CHECK_PASSED;
}

/* print error message before exiting */
int exit_func(const char * file_name, const char * function_name, const int line_number, int exitcode, const char * error) {
    if(exit_echo)
      fprintf(stderr, "[warn]: %s;%s;%d; %s.\n", file_name, function_name, line_number, error);
#if ENABLE_EXIT
   exit(exitcode);
#else
   return CHECK_FAILED;
#endif
}

/* option interface */
char * opt[26];

char * get_argv(char option) {
    return opt[option % 26];
}

int parse_argv(char ** argv, int argc) {
    int i = 0;          /* iterate the total number of arguments in argv array */
    int j = 0;          /* iterate the characters for a single option */
    int len = 0;        /* length of STRING */
    char * str = NULL;

    /* initialize the string array */
    memset(opt, 0, sizeof(char *) * 26);

    /* parse each argument string from argv */
    for (i = 1; i < argc; i++) {
        len = strlen(argv[i]);
        str = argv[i];

        /* check for double -- hypens for long option matching */
        if(2 < len && '-' == str[0] && '-' == str[1]) {
            /* check the argument exist for the long option */
            if(i + 1 >= argc)
                return exit_func_safe("missing %s argument", str);

            /* map the letter to argument */
            if(0 == ncs_strcmp(&str[2], "path")) {
                opt['p' % 26] = argv[++i];
            } else if(0 == ncs_strcmp(&str[2], "output")) {
                opt['o' % 26] = argv[++i];
            } else if(0 == ncs_strcmp(&str[2], "mode")) {
                opt['m' % 26] = argv[++i];
            } else {
                return exit_func_safe("invalid long option %s", &str[2]);
            }
        /* check for single hypen for short option matching */
        } else if(1 < len && '-' == str[0]) {
            /* check the argument exist for the short option */
            if(i + 1 >= argc)
                return exit_func_safe("missing %s argument", str);

            /* map the letter to argument */
            for(j = 1; j < len; j++) {
                switch(str[j]) {
                    case 'p': opt['p' % 26] = argv[++i]; break;
                    case 'o': opt['o' % 26] = argv[++i]; break;
                    case 'm': opt['m' % 26] = argv[++i]; break;
                    default:  return exit_func_safe("invalid short option %c", str[1]);
                }
            }
        }
    }

    return CHECK_PASSED;
}

int path_concat(char * path, size_t len, size_t max, const char * sub_path) {
    size_t i = 0;
    size_t ret = 0;
    size_t sub_len = 0;
    char separator = '/';

    exit_null_safe(2, path, sub_path);

    /* get the directory separator */
    for(i = len - 1; i >= 0; i--)
        if(path[i] == '/' || path[i] == '\\') {
            separator = path[i];
            break;
        }

    /* append the directory separator */
    if (path[len - 1] != separator) {
        ret = snprintf(&path[len], max - len, "%c", separator);
        if (sizeof(char) != ret)
            return exit_func_safe("failed concatenate %s an"
                "d %c on %d buffer size", path, separator, max);
    }

    /* error on empty sub-path string */
    sub_len = strlen(sub_path);
    if(0 >= sub_len)
        return exit_abt_safe("sub path is an empty string");

    /* write the sub path with directory separator accounted for */
    ret = snprintf(&path[len + ret], max - len + ret, "%s", sub_path);
    if(sub_len != ret)
        return exit_func_safe("failed concatenate %s and"
        " %s on %d buffer size", path, sub_path, max);

    for (i = len; i < len + sub_len; i++)
        if (path[i] == '\\' || path[i] == '/')
            path[i] = separator;

    return CHECK_PASSED;
}

/* generate random alpha-digit string */
char * random_string(int len) {
   static const char * alpha = "abcdefghijklmnopqrstuvwxyz0123456789";
   static int alpha_len = 36;

   int i = 0;
   char * str = NULL;

   /* length must be greater than 0 */
   if(len <= 0)
      return NULL;

   str = calloc(len + 1, sizeof(char));
   if(str == NULL)
      return str;

   /* generate random string */
   for(i = 0; i < len; i++)
      str[i] = alpha[rand() % alpha_len];
   str[i] = '\0';

   return str;
}

void strncopy(char * buf, int size, const unsigned char * str) {
   int len = 0;
   const char * src = NULL;

   /* check null */
   if (NULL == buf)
       return;

   if(str == NULL)
      return;

   if(size <= 0) {
      exit_abt_safe("invalid buffer size");
      return;
   }

   src = (const char *) str;
   len = strlen(src);
   if(len <= 0) {
      buf[0] = '\0';
      return;
   }

   /* check buffer size */
   if(len > size - 1) {
      exit_abt_safe("insufficient buffer size");
      len = size - 1;
   }

   /* copy string into buffer */
   strncpy(buf, src, len);
   buf[len] = '\0';
}

int strnload(char * buf, int size, char * str) {
   int len = 0;

   /* check null */
   exit_null_safe(2, buf, str);

   if(size <= 0)
      return exit_abt_safe("invalid buffer size");

   len = strlen(str);

   /* check destination is as large as source */
   if(len > size - 1) {
      exit_abt_safe("insufficient buffer size");
      len = size - 1;
   }

   /* copy string into buffer */
   strncpy(buf, str, len);
   buf[len] = '\0';
   return CHECK_PASSED;
}

/* non-case-sensitive (ncs) string comparsion (strcmp) */
int ncs_strcmp(const char * s1, const char * s2) {
   int inx_1 = 0;
   int inx_2 = 0;
   int s_len_adj_1 = 0;
   int s_len_adj_2 = 0;
   int s_len_1 = strlen(s1);
   int s_len_2 = strlen(s2);

   for(;; inx_1++, inx_2++) {
      /* jump to the next number and alpha */
      while(inx_1 < s_len_1 && !isalpha(s1[inx_1]) && !isdigit(s1[inx_1])) inx_1++, s_len_adj_1++;
      while(inx_2 < s_len_2 && !isalpha(s2[inx_2]) && !isdigit(s2[inx_2])) inx_2++, s_len_adj_2++;

      /* check whether new index is valid */
      if(!(inx_1 < s_len_1) || !(inx_2 < s_len_2)) break;

      /* check mismatch */
      if(toupper(s1[inx_1]) > toupper(s2[inx_2])) return 1;
      if(toupper(s1[inx_1]) < toupper(s2[inx_2])) return -1;
   }

   /* no-mismatch; determine by length */
   s_len_1 -= s_len_adj_1;
   s_len_2 -= s_len_adj_2;
   if(s_len_1 > s_len_2)
      return 1;
   else if(s_len_1 < s_len_2)
      return -1;
   else
      return 0;
}

/* convert a string to a signed integer */
int convert_integer(const char * str, int base) {
   int len = 0;         /* length of str */
   char * ptr = NULL;   /* pointer to str where strtol stopped */
   long int val = 0;    /* string converted to integer value */

   /* check null */
   exit_null_safe(1, str);

   /* check empty string */
   len = strlen(str);
   if (len <= 0)
       /*return exit_abt_safe("empty string");*/
       return 0;

   /* check octal or hexidecimal */
   if(str[0] == '0' && len > 1) {
      if (str[1] == 'x' && len > 2) {
         base = 16;  /* hexidecimal */
      } else {
         base = 8;   /* octal */
      }
   }

   /* convert string to number */
   errno = 0;
   val = strtol(str, &ptr, base);

   /* check errors */
   if(errno == EINVAL)
      return exit_func_safe("invalid integer-base %d on string %s", base, str);
   else if(errno == ERANGE)
      return exit_func_safe("out-of-range integer on string %s", str);

   /* check if str completely parsed; allow spaces as end of buffer since resource databases are space aligned */
   if(*ptr != '\0' && !isspace(*ptr))
      /*return exit_func_safe("failed to completely parsed string %s", str);*/
      return CHECK_FAILED;

   return (int) val;
}

/* convert a set of integer separated by delimiters */
void convert_integer_delimit(char * src, char * delimiters, int argc, ...) {
   int i = 0;
   int len = 0;
   const char * ptr = NULL;
   char buf[BUF_SIZE];

   /* initialize vararg of (int *) pointers */
   va_list argv;
   va_start(argv, argc);

   if(NULL != src && NULL != delimiters) {
       /* check source string is greater than 0 */
      len = strlen(src);
      if (len > 0) {
          for (i = 0, ptr = src; i < argc; i++) {
              /* copy substring from source buffer until delimiter */
              ptr = substr_delimit(ptr, buf, delimiters);

              /* convert substring to integer */
              *va_arg(argv, int *) = convert_integer(buf, 10);

              /* check end of source buffer */
              if (*ptr == '\0') {
                  i++;
                  break;
              }

              /* skip delimiter */
              ptr++;
          }

          /* check if entire string is parsed */
          if (*ptr != '\0')
              exit_func_safe("failed to completely parsed string %s", src);
      }
   }

   /* default any arguments without value to zero */
   for(; i < argc; i++)
      *va_arg(argv, int *) = 0;

   va_end(argv);
}

int convert_integer_delimit_array(const char * str, const char * delimiters, array_w * array) {
   int i = 0;
   const char * ptr = NULL;
   char buf[BUF_SIZE];

   int size = 0;
   int * list = NULL;

   /* check null */
   exit_null_safe(3, str, delimiters, array);

   /* size of list is the number of delimiters + 1 */
   size = count_delimit(str, delimiters) + 1;
   list = calloc(size, sizeof(int));
   if(list == NULL)
      return CHECK_FAILED;

   /* same algorithm as convert_integer_delimit */
   for(i = 0, ptr = str; i < size; i++) {
      ptr = substr_delimit(ptr, buf, delimiters);
      list[i] = convert_integer(buf, 10);
      if(*ptr == '\0') {
         i++;
         break;
      }
      ptr++;
   }
   if(*ptr != '\0')
      exit_func_safe("failed to completely parsed string %s", str);

   /* set array */
   array->array = (void *) list;
   array->size = size;
   return CHECK_PASSED;
}

int convert_integer_delimit_static(const char * str, const char * delimiters, int * list, int size, int * argc) {
   int i = 0;
   int len = 0;
   int * val = NULL;
   array_w array;
   memset(&array, 0, sizeof(array_w));

   len = strlen(str);
   if(0 >= len) {
      memset(list, 0, sizeof(int) * size);
      *argc = 0;
      return CHECK_PASSED;
   }

   /* check whether destination buffer can hold elements */
   if(convert_integer_delimit_array(str, delimiters, &array) ||
      array.size > size) {
      SAFE_FREE(array.array);
      return exit_func_safe("string %s has more elements than max buffer size of %d", str, size);
   }

   /* copy element into list */
   val = (int *) array.array;
   for(i = 0; i < array.size; i++)
      list[i] = val[i];

   if (val != NULL)
      free(val);

   *argc = array.size;
   return CHECK_PASSED;
}

/* convert a string to a unsigned integer */
int convert_uinteger(const char * str, int base) {
   int len = 0;                  /* length of str */
   char * ptr = NULL;            /* pointer to str where strtol stopped */
   unsigned long int val = 0;    /* string converted to integer value */

   /* check null */
   exit_null_safe(1, str);

   /* check empty string */
   len = strlen(str);
   if(len <= 0)
      /*return exit_abt_safe("empty string");*/
      return 0;

   /* check octal or hexidecimal */
   if(str[0] == '0') {
      if(len > 2 && str[1] == 'x') {
         base = 16;  /* hexidecimal */
      } else {
         base = 8;   /* octal */
      }
   }

   /* convert string to number */
   val = strtoul(str, &ptr, base);

   /* check errors */
   if(errno == EINVAL)
      return exit_func_safe("invalid integer-base on string %s", str);
   else if(errno == ERANGE)
      return exit_func_safe("out-of-range integer on string %s", str);

   /* check if str completely parsed */
   if(*ptr != '\0')
      return exit_func_safe("failed to completely parsed string %s", str);

   return (int) val;
}

/* create a copy of the string on the heap */
char * convert_string(const char * str) {
   int len = 0;
   char * tmp = NULL;   /* copy of str */

   /* check null */
   if (NULL == str)
       return NULL;

   len = strlen(str);
   if(len <= 0)
      return NULL;

   /* copy the string */
   tmp = calloc(len + 1, sizeof(char));

   if(tmp != NULL)
      strncpy(tmp, str, len + 1);

   return tmp;
}

char * convert_stringn(const char * str, int * size) {
   int len = 0;
   char * tmp = NULL;   /* copy of str */

   /* check null */
   if(NULL == str)
      return NULL;

   len = strlen(str);
   if(len <= 0)
      return NULL;

   /* copy the string */
   tmp = calloc(len + 1, sizeof(char));

   if(tmp != NULL)
      strncpy(tmp, str, len + 1);

   *size = len;
   return tmp;
}

/* count all the delimters in the source buffer */
int count_delimit(const char * src, const char * delimiters) {
   int i = 0;
   int j = 0;
   int cnt = 0;

   /* check null */
   if(NULL == src || NULL == delimiters)
      return cnt;

   /* get a total of all the delimiters */
   for(i = 0; src[i] != '\0'; i++)
      for(j = 0; delimiters[j] != '\0'; j++)
         if(src[i] == delimiters[j])
            cnt++;

   return cnt;
}

/* copy source buffer into destination buffer until delimiter */
const char * substr_delimit(const char * src, char * des, const char * delimiters) {
   int i = 0;
   int j = 0;
   int len = 0;

   /* check null */
   if(NULL == src || NULL == des || NULL == delimiters)
      return NULL;

   /* check empty source buffer or delimiter */
   if (*src == '\0') {
       exit_abt_safe("source is an empty string");
       return NULL;
   }
   if (*delimiters == '\0') {
       exit_abt_safe("delimiter is an empty string");
       return NULL;
   }

   /* copy the substring from source to destination buffer until delimiter */
   for(i = 0; src[i] != '\0'; i++)
      for(j = 0; delimiters[j] != '\0'; j++)
         if(src[i] == delimiters[j]) {
            strncpy(des, src, i);
            des[i] = '\0';
            return &src[i];
         }

   /* copy source to destination buffer if source buffer does not contain any delimiters */
   len = strlen(src);
   strncpy(des, src, len + 1);
   return &src[len];
}

char * array_to_string(char * buffer, int * array) {
   int i = 0;
   int offset = 0;
   for(i = 0; array[i] > 0; i++)
      offset += sprintf(buffer + offset, "%d%s", array[i], (array[i + 1] > 0) ? ":" : "");
   buffer[offset] = '\0';
   return buffer;
}

char * array_to_string_cnt(char * buffer, int * array, int size) {
   int i = 0;
   int offset = 0;
   for(i = 0; i < size; i++)
      offset += sprintf(buffer + offset, "%d%s", array[i], (i + 1 < size) ? ":" : "");
   buffer[offset] = '\0';
   return buffer;
}

int array_field_cnt(char * buf) {
   int i = 0;
   int cnt = 0;
   int len = strlen(buf);
   for(i = 0; i < len; i++)
      if(buf[i] == ':') cnt++;
   return cnt;
}

int aeiou(char letter) {
    switch (letter) {
        case 'a': case 'e': case 'i': case 'o': case 'u':
            return CHECK_PASSED;
        default:
            return CHECK_FAILED;
    }
}

void sift_down(void * list, size_t start, size_t end, swap_t * cb) {
    size_t swap = 0;
    size_t root = start;
    size_t child = 0;

    /* left child node index is computed by (root node index * 2) + 1,
     * but if left child node index > array size then the root node is
     * a leaf node with no child nodes. */
    while (root * 2 + 1 <= end) {
        /* set the left child node */
        child = root * 2 + 1;

        /* set the initial max node */
        swap = root;

        /* check if left child node is max node */
        if (cb->less(list, swap, child))
            swap = child;
        /* check if right child node is max node */
        if (child + 1 <= end && cb->less(list, swap, child + 1))
            swap = child + 1;
        /* if neither left or right child is greater than the root,
        * then that means the root does not need to be swap */
        if (swap == root) {
            return;
        }
        else {
            cb->swap(list, swap, root);
            root = swap;
        }
    }
}

int heap_sort(void * list, size_t size, swap_t * cb) {
    size_t end = 0;
    size_t start = (size - 2) / 2;

    /* convert to the max heap */
    while (start >= 0) {
        sift_down(list, start, size - 1, cb);
        if (start == 0)
            break;
        start--;
    }

    /* sort by using sift down as selection */
    end = size - 1;
    while (end > 0) {
        cb->swap(list, end, 0);

        end--;

        sift_down(list, 0, end, cb);
    }
    return 0;
}

int insertion_sort(void * list, size_t min, size_t max, swap_t * cb) {
    size_t i = 0;
    size_t j = 0;

    if (min == max)
        return 0;

    for (i = min; i <= max; i++)
        for (j = i; j > min && cb->less(list, j, j - 1); j--)
            cb->swap(list, j, j - 1);

    return 0;
}

size_t mof3(void * list, size_t min, size_t mid, size_t max, swap_t * cb) {
    return (cb->less(list, min, mid) ?
        cb->less(list, mid, max) ? mid :
        cb->less(list, min, max) ? max : min :
        cb->less(list, max, mid) ? mid :
        cb->less(list, max, min) ? max : min);
}


void pivot_sedgwick(void * list, size_t min, size_t max, swap_t * cb) {
    size_t mid = 0;

    /* (min + max) / 2 can cause integer overflow for large
     * min and max; https://en.wikipedia.org/wiki/Quicksort */
    mid = min + (max - min) / 2;
    mid = mof3(list, min, mid, max, cb);
    cb->swap(list, mid, min);
}

void pivot_ninther(void * list, size_t min, size_t max, size_t size, swap_t * cb) {
    size_t dif = 0;
    size_t mid = 0;

    /* divide the region from min to max into eight regions, then
     * calculate the three mof3 over the lower, middle, and upper
     * regions separate by the nine existing boundary points, and
     * then calculate the mof3 of the three mof3 */
    dif = size / 8;
    mid = min + size / 2;
    mid = mof3(list,
        mof3(list, min, min + dif, min + dif + dif, cb),
        mof3(list, mid - dif, mid, mid + dif, cb),
        mof3(list, max - dif - dif, max - dif, max, cb),
        cb);
    cb->swap(list, mid, min);
}

void partition_3way(void * list, size_t min, size_t max, size_t * _min, size_t * _max, swap_t * cb) {
    size_t k = 0;
    size_t i = 0;
    size_t j = 0;
    size_t p = 0;
    size_t q = 0;

    i = p = min;
    j = q = max + 1;

    while (1) {
        /* find an element from the  left that is greater than
         * or equal to the median starting from (i + 1) to max */
        while (cb->less(list, ++i, min))
            if (i == max)
                break;

        /* find an element from the right that is less than or
         * equal to the median starting from (j - 1) to min */
        while (cb->less(list, min, --j))
            if (j == min)
                break;

        /* copy the median in the middle to [equal-left] */
        if (i == j && cb->same(list, min, i)) {
            ++p;
            cb->swap(list, p, i);
        }

        if (i >= j) break;

        cb->swap(list, i, j);

        /* swap median from i to equal-left */
        if (cb->same(list, min, i)) {
            if (i < max) ++p;
            cb->swap(list, p, i);
        }
        /* swap median from j to equal-right */
        if (cb->same(list, min, j)) {
            if (j > min) --q;
            cb->swap(list, q, j);
        }
    }

    /*   [equal-left] [< median] [> median] [equal-right]
     * min----------p----------j-i----------q-----------max */
    i = j + 1;

    /* [< median] [equal-left] [> median] [equal-right]  */
    for (k = min; k <= p; k++) {
        cb->swap(list, k, j);
        if (j > min) j--;
    }

    /* [< median] [equal-left] [equal-right] [> median] */
    for (k = max; k >= q; k--) {
        cb->swap(list, k, i);
        if (i < max) i++;
    }

    *_min = j;
    *_max = i;
}

int _quick_sort(void * list, size_t min, size_t max, size_t max_depth, swap_t * cb) {
    size_t size = 0;
    size_t _min = 0;
    size_t _max = 0;
    size = max - min + 1;

    /* heap sort
     * asymptotic time complexity is O(nlg(n))
     * heap sort  is used as a fallback for  quick sort
     * to guarantee a time complexity of O(nlg(n)) when
     * quick's depth is greater than lg(n) * 2.
     */
    if (max_depth == 0)
        return heap_sort(list, size, cb);

    if (size < 8)
        /* insertion sort
         * asymptotic time complexity is O(n^2)
         * 1. efficient for n < 8 due to less overhead
         * 2. efficient for sorted list O(n)
         * 3. keep original order for equal elements
         * used in quicksort as an optimization for n < 8 to
         * eliminate the overhead and provide O(n) asymptotic
         * time complexity since partitions are nearly sorted
         */
        return insertion_sort(list, min, max, cb);
    else if (size <= 40)
        pivot_sedgwick(list, min, max, cb);
    else
        pivot_ninther(list, min, max, size, cb);

    partition_3way(list, min, max, &_min, &_max, cb);   /* 3way [< median] [= median] [> median] */
    _quick_sort(list, min, _min, max_depth - 1, cb);    /* sort [< median] */
    _quick_sort(list, _max, max, max_depth - 1, cb);    /* sort [> median] */
    return 0;
}

int quick_sort(void * list, size_t size, swap_t * cb) {
    if (size == 1) return 0;
    size_t max_depth = (size_t) log(size) * 2;
    _quick_sort(list, 0, size - 1, max_depth, cb);
    return 0;
}

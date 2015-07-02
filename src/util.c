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
   fprintf(stderr, "[warn]: %s;%s;%d; %s.\n", file_name, function_name, line_number, error);
#if ENABLE_EXIT
   exit(exitcode);
#else
   return CHECK_FAILED;
#endif
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
   if(exit_null_safe(1, buf) || str == NULL)
      return;

   if(size <= 0) {
      exit_abt_safe("invalid buffer size");
      return;
   }

   src = (const char *) str;
   len = strlen(src);
   if(len <= 0)
      return;

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
   if(exit_null_safe(2, buf, str))
      return CHECK_FAILED;

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
   if(exit_null_safe(1, str))
      return CHECK_FAILED;

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
      return exit_func_safe("failed to completely parsed string %s", str);

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

   if(!exit_null_safe(2, src, delimiters)) {
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
   if(exit_null_safe(3, str, delimiters, array))
      return CHECK_FAILED;

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

int convert_integer_delimit_static(const char * str, const char * delimiters, int * list, int size) {
   int i = 0;
   int * val = NULL;
   array_w array;
   memset(&array, 0, sizeof(array_w));
   convert_integer_delimit_array(str, delimiters, &array);

   /* check whether destination buffer can hold elements */
   if(array.size > size)
      return exit_func_safe("string %s has more elements than max buffer size of %d", str, size);

   /* copy element into list */
   val = (int *) array.array;
   for(i = 0; i < array.size; i++)
      list[i] = val[i];

   if (val != NULL)
	   free(val);

   return CHECK_PASSED;
}

/* convert a string to a unsigned integer */
int convert_uinteger(const char * str, int base) {
   int len = 0;                  /* length of str */
   char * ptr = NULL;            /* pointer to str where strtol stopped */
   unsigned long int val = 0;    /* string converted to integer value */

   /* check null */
   if(exit_null_safe(1, str))
      return CHECK_FAILED;

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
   if(exit_null_safe(1, str))
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

char * concatenate_string(char ** strs, int start, int end, char sep) {
	int i = 0;
	int off = 0;
	char buf[BUF_SIZE];

	/* concatenate string separated by character */
	for (i = start; i < end; i++)
		off += sprintf(&buf[off], "%s%c", strs[i], sep);

	/* check empty string */
	if (off <= 0) {
		exit_abt_safe("empty string from %d to %d", start, end);
		return NULL;
	}
	/* return string */
	return convert_string(buf);
}

/* count all the delimters in the source buffer */
int count_delimit(const char * src, const char * delimiters) {
   int i = 0;
   int j = 0;
   int cnt = 0;

   /* check null */
   if(exit_null_safe(2, src, delimiters))
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
   if(exit_null_safe(3, src, des, delimiters))
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
/*
 *   file: util.c
 *   date: 5/09/2014
 * update: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "util.h"

void exit_throw_error(const char * error, const char * file_name, const char * function_name, const int line_number) {
   fprintf(stderr, "%s;%s;%d; %s\n", file_name, function_name, line_number, error);
   exit(EXIT_FAILURE);
}

char * build_buffer(char * buffer, char * format, ...) {
   int offset = 0;
   va_list list;
   va_start(list, format);
   offset = vsprintf(buffer, format, list);
   buffer[offset] = '\0';
   va_end(list);
   return buffer;
}

int exit_check_null(int argument_count, ...) {
   int i = 0;
   void * ptr = NULL;
   va_list list;
   va_start(list, argument_count);
   for(i = 0; i < argument_count; i++) {
      ptr = va_arg(list, void *);
      if(ptr == NULL)
         return 1;
   }
   va_end(list);
   return 0;
}

char * random_string(int32_t length) {
   int32_t i = 0;
   char * random_string = malloc(sizeof(char) * (length + 1));
   if(random_string == NULL) return NULL;
   for(i = 0; i < length; i++)
      random_string[i] = ASCII_SET[rand() % ASCII_SZE];
   random_string[i] = '\0';
   return random_string;
}

int32_t ncs_strcmp(const char * s1, const char * s2) {
   int32_t inx_1 = 0;
   int32_t inx_2 = 0;
   int32_t s_len_adj_1 = 0;
   int32_t s_len_adj_2 = 0;
   int32_t s_len_1 = strlen(s1);
   int32_t s_len_2 = strlen(s2);

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

int32_t convert_integer(const char * str, int32_t base) {
   int64_t value = 0;
   char * endptr = NULL;

   if(str == NULL || *str == '\0')    /* check the paramater string */
      return 0;

   /* override the base for hexadecimal numbers; warning to fix error */
   if(strlen(str) > 2 && str[0] == '0' && str[1] == 'x' && base != 16) {
      fprintf(stdout,"warn: convert_integer detected hexadecimal notation, overriding base %d; %s.\n", base, str);
      base = 16;
   }

   value = strtol(str, &endptr, base);

   if(errno == ERANGE)                /* check if value is valid (out of range of the strtol function) */
      fprintf(stdout,"warn: convert_integer detected out-of-range string conversion to (int32_t) %s.\n", str);

   if(*endptr != '\0' && 0)               /* check if string consume / DISABLE */
      fprintf(stdout,"warn: convert_integer failed to consume entire string for conversion %s; unconsumed %s; base %d.\n", str, endptr, base);

   if(value > INT32_MAX)            /* check if out of range (although this might not work) */
      fprintf(stdout,"warn: convert_integer possible out-of-range string conversion to (int32_t) %s.\n", str);

   return (int32_t) value;
}

uint32_t convert_uinteger(const char * str, uint32_t base) {
   uint64_t value = 0;
   char * endptr = NULL;

   if(str == NULL || *str == '\0')    /* check the paramater string */
      return 0;

   value = strtoul(str, &endptr, base);

   if(errno == ERANGE)               /* check if value is valid (out of range of the strtoul function) */
      fprintf(stdout,"warn: convert_integer detected out-of-range string for conversion to long %s.\n", str);

   if(*endptr != '\0' && 0)               /* check if string consume  / DISABLE */
      fprintf(stdout,"warn: convert_integer failed to consume entire string for conversion %s; unconsumed %s; base %d.\n", str, endptr, base);

   if(value > UINT32_MAX)            /* check if out of range (although this might not work) */
      fprintf(stdout,"warn: convert_integer possible out-of-range string conversion to (uint32_t) %s.\n", str);

   return (uint32_t) value;
}

char * convert_string(const char * str) {
   char * temp = NULL;
   if(str != NULL && *str != '\0' && strlen(str) > 0) {  /* check the paramater string */
      temp = malloc(sizeof(char) * (strlen(str) + 1));   /* allocate space for the string */
      if(temp != NULL) strcpy(temp, str);                /* check if memory allocated */
   }
   return temp;
}

char * substr_delimit(char * src, char * dest, char delimit) {
   int32_t i;
   int32_t src_cnt = strlen(src);
   for(i = 0; i < src_cnt; i++)
      if((src[i] == delimit || src[i] == '\0') && i > 0) {
         strncpy(dest, src, i);
         dest[i] = '\0';
         return &src[i];
      }
   /* retrieve the whole string if unmatch delimit */
   strncpy(dest, src, src_cnt);
   dest[src_cnt] = '\0';
   return &src[src_cnt];
}

char * substr_delimit_list(char * src, char * dest, char * delimit) {
   int32_t i, j;
   int32_t src_cnt = strlen(src);
   int32_t del_cnt = strlen(delimit);

   for(i = 0; i < src_cnt; i++) {
      for(j = 0; j < del_cnt; j++)
         if(src[i] == delimit[j] && i > 0) {
            strncpy(dest, src, i);
            dest[i] = '\0';
            return &src[i];
         }
   }

   /* retrieve the whole string if unmatch delimit */
   strncpy(dest, src, src_cnt);
   dest[src_cnt] = '\0';
   return &src[src_cnt];
}

void convert_delimit_integer(char * str, char delimit, int32_t argc, ...) {
   int32_t i = 0;
   int32_t * temp = NULL;     /* current integer variable to fill */
   char * curptr = str;       /* current string position */
   char * endptr = str;       /* last string position */
   va_list argv;
   va_start(argv, argc);

   /* check the paramater string */
   if(str != NULL && *str != '\0' && strlen(str) > 0) {
      /* extract the integer from string */
      for(i = 0; i < argc && *endptr != '\0'; i++) {
         temp = va_arg(argv, int32_t *);
         *temp = (int32_t) strtol(curptr, &endptr, 10);
         curptr = endptr + 1;                        /* skip delimiter */

         if(errno == ERANGE)                         /* check if conversion is valid */
            fprintf(stdout,"warn: convert_delimit_integer detected out-of-range string for conversion to long.\n");

         if(*endptr != delimit && *endptr != '\0')   /* check if delimiter or null */
            fprintf(stdout,"warn: convert_delimit_integer failed to match delimiter for %s.\n", str);
      }

      /* check if the entire string is consumed */
      if(*endptr != '\0')
         fprintf(stdout,"warn: convert_delimit_integer failed to match all integers in %s, missing paramaters.\n", str);
   }

   /* set all remaining paramaters to default */
   for(; i < argc; i++) {
      temp = va_arg(argv, int32_t *);
      *temp = 0;
   }
   va_end(argv);
}

void convert_integer_list(char * str, char * delimit, array_w * list) {
   int32_t i, j;
   int32_t * int_list = NULL;
   int32_t str_cnt = strlen(str);
   int32_t del_cnt = strlen(delimit);
   int32_t fld_cnt = 1;
   char * end_ptr = NULL;
   char fld[BUF_SIZE];

   /* check the paramater */
   if(str == NULL || delimit == NULL || list == NULL) {
      fprintf(stdout,"warn: convert_integer_list detected NULL paramaters.\n");
      return;
   }

   /* find the total number of fields */
   for(i = 0; i < str_cnt; i++)
      for(j = 0; j < del_cnt; j++)
         if(str[i] == delimit[j]) {
            fld_cnt++;
            break;
         }

   /* allocate memory for the list */
   int_list = malloc(sizeof(int32_t) * fld_cnt);

   /* extract the fields */
   end_ptr = substr_delimit_list(str, fld, delimit);
   if(*str == '0' && *(str+1) == 'x')
      int_list[0] = convert_integer(fld, 16);
   else
      int_list[0] = convert_integer(fld, 10);
   for(i = 1; end_ptr != NULL && i < fld_cnt; i++) {
      end_ptr = substr_delimit_list(end_ptr + 1, fld, delimit);
      if(*str == '0' && *(str+1) == 'x')
         int_list[i] = convert_integer(fld, 16);
      else
         int_list[i] = convert_integer(fld, 10);
   }

   /* set the new array */
   list->array = (void *) int_list;
   list->size = fld_cnt;
   list->delimit = delimit[0];
}

void array_io(array_w array, FILE * file_stm) {
   int32_t i = 0;
   int32_t * list = array.array;
   int32_t size = array.size;
   for(i = 0; i < size - 1; i++)
      fprintf(file_stm,"%d%c", list[i], array.delimit);
   fprintf(file_stm,"%d,", list[i]);
}

void array_unload(array_w array) {
   if(array.array != NULL) free(array.array);
}
/*
 *   file: load.h
 *   date: 5/10/2014
 * update: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "load.h"

int sentinel_newline(char c) {
   return (c == '\r' || c == '\n' || c == '\0');
}

int sentinel_whitespace(char c) {
   return (sentinel_newline(c) || isspace(c));
}

int sentinel_comment(char c) {
   return (sentinel_newline(c) || c == '/');
}

int sentinel_semicolon(char c) {
   return (sentinel_newline(c) || c == ';');
}

int delimit_cvs(char c) {
   return (c == ',' || sentinel_newline(c));
}

int delimit_cvs_whitespace(char c) {
   return (c == ',' || sentinel_whitespace(c));
}

int delimit_cvs_comment(char c) {
   return (c == ',' || sentinel_comment(c));
}

int delimit_cvs_pound(char c) {
   return (c == '#');
}

int delimit_cvs_semicolon(char c) {
   return (c == ',' || sentinel_semicolon(c));
}

char * trim(const char * file_name, int * file_line, DB_TRIM file_trim) {
   char * trim_name = NULL;
   FILE * file_stm = NULL;
   FILE * trim_stm = NULL;

   /* generate trim file name */
   trim_name = random_string(16);
   if(trim_name == NULL) {
      exit_abt_safe("failed to allocated temporary filtered database name");
      return NULL;
   }

   /* open file descriptors for trimming */
   file_stm = fopen(file_name,"r");
   if(file_stm == NULL) {
      exit_func_safe("failed to open %s database file", file_name);
      free(trim_name);
      return NULL;
   }

   trim_stm = fopen(trim_name,"w");
   if(trim_stm == NULL) {
      exit_func_safe("failed to open %s filtered database file", trim_name);
      free(trim_name);
      fclose(file_stm);
      return NULL;
   }

   /* trim database file and store valid entries in trim file */
   *file_line = file_trim(file_stm, trim_stm);
   fclose(file_stm);
   fclose(trim_stm);

   /* check if any entries check */
   if(*file_line <= 0) {
      exit_func_safe("detected zero database entries in %s", file_name);
      free(trim_name);
   }
   /* return trim file name for loading process */
   return (*file_line > 0) ? trim_name : NULL;
}

int trim_general(FILE * file_stm, FILE * trim_stm) {
   int line_count = 0;
   char buf[BUF_SIZE];
   exit_null_safe(2, file_stm, trim_stm);
   while(fgets(buf, sizeof(buf), file_stm) != NULL)
      if(strlen(buf) > 0)          /* check if empty */
         if(!isspace(buf[0]))      /* check if whitespace */
            if(buf[0] != '/') {    /* check if comment */
               fprintf(trim_stm, "%s", buf);
               line_count++;
            }
   return line_count;
}

int trim_numeric(FILE * file_stm, FILE * trim_stm) {
   int line_count = 0;
   char buf[BUF_SIZE];
   exit_null_safe(2, file_stm, trim_stm);
   while(fgets(buf, sizeof(buf), file_stm) != NULL)
      if(strlen(buf) > 0)            /* check if empty */
         if(!isspace(buf[0]))        /* check if whitespace */
            if(buf[0] != '/')        /* check if comment */
               if(isdigit(buf[0])) { /* check if digit */
                  fprintf(trim_stm, "%s", buf);
                  line_count++;
               }
   return line_count;
}

int trim_alpha(FILE * file_stm, FILE * trim_stm) {
   int line_count = 0;
   char buf[BUF_SIZE];
   exit_null_safe(2, file_stm, trim_stm);
   while(fgets(buf, sizeof(buf), file_stm) != NULL)
      if(strlen(buf) > 0)            /* check if empty */
         if(!isspace(buf[0]))        /* check if whitespace */
            if(buf[0] != '/')        /* check if comment */
               if(isalpha(buf[0])) { /* check if alpha */
                  fprintf(trim_stm, "%s", buf);
                  line_count++;
               }
   return line_count;
}

int load_native_general(FILE * stm, void * mem, int trim_size, native_config_t * config) {
   int i = 0;
   char buf[BUF_SIZE];
   char fld[BUF_SIZE];
   int offset_buf = 0;
   int offset_fld = 0;
   int index_col = 0;  /* column index */
   int index_row = 0;  /* row index */
   int line_size = 0;  /* size of line */

   /* special handling columns */
   int bracket = 0;
   int quote = 0;

   while(fgets(buf, sizeof(buf), stm)) {
      /* reset reading variables */
      offset_buf = 0;
      offset_fld = 0;
      index_col = 0;
      bracket = 0;
      quote = 0;
      line_size = strlen(buf);

      for(offset_buf = 0; offset_buf < line_size; offset_buf++) {
         /* check bracket levels */
         if(config->flag & CHECK_BRACKET)
            switch(buf[offset_buf]) {
               case '{': bracket++; break;
               case '}': bracket--; break;
            }
         /* check quote levels */
         if(config->flag & CHECK_QUOTE)
            if(buf[offset_buf] == '\"')
               quote = (quote) ? quote - 1 : quote + 1;

         /* write buffers or write column field */
         if(!bracket && !quote && config->delimit(buf[offset_buf])) {
            fld[offset_fld] = '\0';
            /* strip the whitespace from the end */
            if(offset_fld > 0)
               for(i = offset_fld - 1; i >= 0; i--)
                  if(isspace(fld[i]))
                     offset_fld--;
                  else
                     break;

            /* write the column field */
            config->load(mem, index_row, index_col, fld);
            offset_fld = 0;
            index_col++;

            /* skip all beginning whitespace for the next field */
            if(config->flag & SKIP_NEXT_WS)
               while(isspace(buf[offset_buf+1]) && buf[offset_buf+1] != '\0')
                  offset_buf++;
         } else {
            if(!(isspace(buf[offset_buf]) && offset_fld <= 0) && buf[offset_buf] != '\"') {
               fld[offset_fld] = buf[offset_buf];
               offset_fld++;
            }
         }

         /* check end of entry line */
         if(config->sentinel(buf[offset_buf])) break;
      }

      /* report syntax errors */
      if(bracket)
         exit_func_safe("missing closing bracket.\n[warn]: %s\n", buf);
      if(quote)
         exit_func_safe("missing closing quote[%d]; %s\n", quote, buf);
      if(config->flag & CHECK_FIELD_COUNT && config->field_count != index_col)
         exit_func_safe("missing fields; expected %d fields but got %d fields.\n[warn]: %s\n[warn]: %s", config->field_count, index_col, buf, &buf[offset_buf]);

      index_row++;
   }

   return index_row;
}

int load_native(const char * file_name, DB_TRIM file_trim, NATIVE_DB_LOAD file_load, native_t * native, native_config_t * config) {
   void * db = NULL;
   int cnt = 0;

   /* trim database variables */
   char * trim_name = NULL;
   FILE * trim_stm = NULL;
   int trim_size = 0;

   /* check for invalid paramaters */
   exit_null_safe(5, file_name, file_trim, file_load, native, config);

   /* check the record size */
   if(config->record_size <= 0) {
      exit_func_safe("invalid record size %d", config->record_size);
      return CHECK_FAILED;
   }

   /* trim the database file and open stream to new trim file */
   trim_name = trim(file_name, &trim_size, file_trim);
   if (trim_name == NULL)
       return CHECK_FAILED;
   if(trim_size <= 0) {
      exit_func_safe("detected %d records from %s", trim_size, file_name);
      return CHECK_FAILED;
   }

   trim_stm = fopen(trim_name, "r");
   if(trim_stm == NULL) {
      exit_func_safe("failed to open filtered %s database file", trim_name);
      free(trim_name);
      return CHECK_FAILED;
   }

   /* allocate memory for the database */
   db = calloc(trim_size, config->record_size);
   if(db == NULL) {
      exit_func_safe("failed allocate memory for %s database file with %d records.\n", file_name, trim_size);
      free(trim_name);
      fclose(trim_stm);
      return CHECK_FAILED;
   }

   /* load the datbase */
   cnt = file_load(trim_stm, db, trim_size, config);
   if(cnt <= 0) {
      exit_func_safe("%d records detected in %s database file.\n", file_name, trim_size);
      free(trim_name);
      fclose(trim_stm);
      free(db);
      return CHECK_FAILED;
   }

   /* clean up trim database resources */
   fclose(trim_stm);
   remove(trim_name);
   free(trim_name);

   /* return generic database wrapper */
   native->db = db;
   native->size = trim_size;
   return CHECK_PASSED;
}

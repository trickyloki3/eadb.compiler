/*
 *   file: load.h
 *   date: 5/10/2014
 * update: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "load.h"

char * trim(const char * file_name, int32_t * file_line, DB_TRIM file_trim) {
   char * trim_name = NULL;
   FILE * file_stm = NULL;
   FILE * trim_stm = NULL;
   char buffer[BUF_SIZE];

   /* check for invalid paramaters */
   exit_null("file_name is null", 1, file_name);
   exit_null("file_line is null", 1, file_line);
   exit_null("file_trim is null", 1, file_trim);

   /* generate trim file name */
   trim_name = random_string(16);
   exit_null("failed to generate trim file name", 1, trim_name);

   /* open file descriptors for trimming */
   file_stm = fopen(file_name,"r");
   trim_stm = fopen(trim_name,"w");
   exit_null(build_buffer(buffer,"failed to open %s", file_name), 1, file_stm);
   exit_null(build_buffer(buffer,"failed to open %s", trim_name), 1, trim_stm);

   /* trim database file and store valid entries in trim file */
   *file_line = file_trim(file_stm, trim_stm);
   fclose(file_stm);
   fclose(trim_stm);

   /* check if any entries check */
   if(*file_line <= 0) {
      fprintf(stderr, "detected zero database entries in %s\n", file_name);
      free(trim_name);
   }
   /* return trim file name for loading process */
   return (*file_line > 0) ? trim_name : NULL;
}

int32_t trim_general(FILE * file_stm, FILE * trim_stm) {
   int32_t line_count = 0;
   char buf[BUF_SIZE];
   while(fgets(buf, sizeof(buf), file_stm) != NULL)
      if(strlen(buf) > 0)          /* check if empty */
         if(!isspace(buf[0]))      /* check if whitespace */
            if(buf[0] != '/') {    /* check if comment */
               fprintf(trim_stm, "%s", buf);
               line_count++;
            }
   return line_count;
}

int32_t trim_numeric(FILE * file_stm, FILE * trim_stm) {
   int32_t line_count = 0;
   char buf[BUF_SIZE];
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

int32_t trim_alpha(FILE * file_stm, FILE * trim_stm) {
   int32_t line_count = 0;
   char buf[BUF_SIZE];
   while(fgets(buf, sizeof(buf), file_stm) != NULL)
      if(strlen(buf) > 0)            /* check if empty */
         if(!isspace(buf[0]))        /* check if whitespace */
            if(buf[0] != '/')        /* check if comment */
               if(isalpha(buf[0])) { /* check if digit */
                  fprintf(trim_stm, "%s", buf);
                  line_count++;
               }
   return line_count;
}

void * load(const char * file_name, DB_TRIM file_trim, DB_LOAD file_load, load_cb_t * (*loader)()) {
   /* generic database wrapper */
   load_db_t * wrapper = NULL;
   void * db = NULL;
   int32_t cnt = 0;

   /* trim database variables */
   char * trim_name = NULL;
   FILE * trim_stm = NULL;
   int32_t trim_size = 0;
   char buffer[BUF_SIZE];
   load_cb_t * db_load = loader();

   /* check for invalid paramaters */
   exit_null("file_name is null", 1, file_name);
   exit_null("file_trim is null", 1, file_trim);
   exit_null("file_load is null", 1, file_load);
   if(db_load->type_size <= 0) exit_abt("db_load->type_size is less than zero");

   /* trim the database file and open stream to new trim file */
   trim_name = trim(file_name, &trim_size, file_trim);
   exit_null(build_buffer(buffer,"failed to database %s", file_name), 1, trim_name);
   trim_stm = fopen(trim_name, "r");
   exit_null("failed to load the trim database file", 1, trim_stm);   

   /* allocate memory for the database */
   db = calloc(trim_size, db_load->type_size);
   exit_null(build_buffer(buffer,"failed to allocated memory for %s", file_name), 1, db);

   /* load the datbase */
   cnt = file_load(trim_stm, db, trim_size, db_load);
   if(cnt <= 0) exit_abt("failed load any database entries.");

   /* clean up trim database resources */
   fclose(trim_stm);
   remove(trim_name);
   free(trim_name);

   /* return generic database wrapper */
   wrapper = malloc(sizeof(load_db_t));
   exit_null("failed to allocate memory for database wrapper", 1, wrapper);
   wrapper->db = db;
   wrapper->size = trim_size;
   wrapper->dealloc = db_load->dealloc;
   free(db_load);
   return wrapper;
}

int load_general(FILE * stm, void * mem, int trim_size, load_cb_t * loader) {
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
         if(loader->flag & CHECK_BRACKET)
            switch(buf[offset_buf]) {
               case '{': bracket++; break;
               case '}': bracket--; break;
            }
         /* check quote levels */
         if(loader->flag & CHECK_QUOTE)
            if(buf[offset_buf] == '\"')
               quote = (quote) ? quote - 1 : quote + 1;

         /* write buffers or write column field */
         if(!bracket && !quote && loader->is_row_delimiter(buf[offset_buf])) {
            fld[offset_fld] = '\0';
            /* write the column field */
            loader->load_column(mem, index_row, index_col, fld);
            offset_fld = 0;
            index_col++;

            if(loader->flag & SKIP_NEXT_WS) while(isspace(buf[offset_buf+1]) && buf[offset_buf+1] != '\0') offset_buf++;
         } else {
            if(!(isspace(buf[offset_buf]) && offset_fld <= 0) && buf[offset_buf] != '\"') {
               fld[offset_fld] = buf[offset_buf];
               offset_fld++;
            }
         }

         /* check end of entry line */
         if(loader->is_row_sentinel(buf[offset_buf])) break;
      }

      /* report syntax errors */
      if(bracket) 
         fprintf(stderr,"missing closing bracket; %s\n", buf);
      if(quote) 
         fprintf(stderr,"missing closing quote[%d]; %s\n", quote, buf);
      if(loader->column_count > 0 && loader->column_count != index_col) 
         fprintf(stderr,"missing columns[%d;%d]; %s\n", loader->column_count, index_col, buf);

      index_row++;
   }

   return index_row;
}
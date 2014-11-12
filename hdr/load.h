/*
 *   file: load.h
 *   date: 5/10/2014
 * update: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef LOAD_H
#define LOAD_H
   #include "util.h"

   /* generic database wrapper */
   typedef struct {
      void * db;
      int size;
      void (*dealloc)(void *, int);
   } load_db_t;

   /* flags for interface */
   #define CHECK_BRACKET   0x1
   #define CHECK_QUOTE     0x2
   #define SKIP_NEXT_WS    0x4

   /* generic database callback wrapper */
   typedef struct {
      int (*load_column)(void * db, int row, int col, char * val);
      int (*is_row_sentinel)(char);
      int (*is_row_delimiter)(char);
      int column_count;
      size_t type_size;
      int flag;
      void (*dealloc)(void *, int);
   } load_cb_t;

   /* trim database of invalid entries 
    * char * trim(const char * file_name, int * file_line, DB_TRIM file_trim)
    * file_name - name of the databse file, i.e. item_db.txt.
    * file_line - store the total number of lines written to file_trim.
    * file_trim - file stream reading from file_name.
    */
   typedef int (*DB_TRIM)(FILE *, FILE *);
   /* int trim_numeric(FILE * file_stm, FILE * trim_stm)
    * int trim_alpha(FILE * file_stm, FILE * trim_stm)
    * DB_TRIM function that expects numeric or alpha ID per row in the table.
    */
   int trim_general(FILE *, FILE *);
   int trim_numeric(FILE *, FILE *);
   int trim_alpha(FILE *, FILE *);
   /* char * trim(const char * file_name, int32_t * file_line, DB_TRIM file_trim)   
    * Refactor all the condition checking and generalize trimming functions.
    * Returns the trim filename and requires freeing from heap memory.
    */
   char * trim(const char *, int *, DB_TRIM);

   /* load trim database entries 
    * void * load(const char * file_name, DB_TRIM file_trim, DB_LOAD file_load, size_t type_size)
    * file_name - name of the database file, i.e. item_db.txt.
    * file_trim - function pointer to either trim_numeric or trim_alpha or custom.
    * file_load - function pointer to database load 
    * size_t - size of the database entry, i.e. sizeof(item_t) 
    */
   typedef int (*DB_LOAD)(FILE *, void *, int, load_cb_t *);
   int load_general(FILE *, void *, int, load_cb_t *);
   /* void * load(const char * file_name, DB_TRIM file_trim, DB_LOAD file_load, size_t type_size)
    * Refactor all the condition checking and generalize loading functions.
    * Returns generic database structure that must be deinit.
    */
   void * load(const char *, DB_TRIM, DB_LOAD, load_cb_t * (*loader)());

#endif

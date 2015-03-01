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

    /* filter the database with one record per line */
    typedef int (*DB_TRIM)(FILE *, FILE *);
    int trim_general(FILE *, FILE *);
    int trim_numeric(FILE *, FILE *);
    int trim_alpha(FILE *, FILE *);
    char * trim(const char *, int *, DB_TRIM);

    /* flags to native_db_load function */
    #define CHECK_BRACKET      0x1  /* check bracket delimiter for database with scripted fields */
    #define CHECK_QUOTE        0x2  /* check quote delimiter for databases with quoted fields */
    #define SKIP_NEXT_WS       0x4  /* skip whitespace at the start of each field */
    #define CHECK_FIELD_COUNT  0x8  /* check the field count for each record */    

    /* type definition for native configuration */
    typedef int (*RECORD_LOADER)(void * db, int row, int col, char * val);
    typedef int (*CHECK_SENTINEL)(char);
    typedef int (*CHECK_DELIMIT)(char);

    /* check sentinel and check delimit functions */
    int sentinel_newline(char c);
    int sentinel_whitespace(char c);
    int delimit_cvs(char c);
    int delimit_cvs_whitespace(char c);

    typedef struct {
        void * db;
        int size;
    } native_t;

    typedef struct {
        RECORD_LOADER load;
        CHECK_SENTINEL sentinel;    /* indicate the end of a record */
        CHECK_DELIMIT delimit;      /* indicate the end of a field */
        int flag;                   /* load algorithm checks */
        int field_count;            /* number of fields per record */
        int record_size;            /* sizeof the database record */
    } native_config_t;

    typedef int (*NATIVE_DB_LOAD)(FILE *, void *, int, native_config_t *);
    int load_native_general(FILE *, void *, int, native_config_t *);
    int load_native(const char *, DB_TRIM, NATIVE_DB_LOAD, native_t *, native_config_t *);

   /* legacy native database load interface */
    typedef struct {
        void * db;
        int size;
        void (*dealloc)(void *, int);
    } load_db_t;

    typedef struct {
        int (*load_column)(void * db, int row, int col, char * val);
        int (*is_row_sentinel)(char);
        int (*is_row_delimiter)(char);
        int column_count;
        size_t type_size;
        int flag;
        void (*dealloc)(void *, int);
    } load_cb_t;

    typedef int (*DB_LOAD)(FILE *, void *, int, load_cb_t *);
    int load_general(FILE *, void *, int, load_cb_t *);
    void * load(const char *, DB_TRIM, DB_LOAD, load_cb_t * (*loader)());
#endif

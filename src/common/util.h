/*
 *   file: util.h
 *   date: 5/09/2014
 * update: 11/09/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef UTILITY_H
#define UTILITY_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <errno.h>
    #include <ctype.h>
    #include <math.h>
    #include "setting.h"

    #define ENABLE_EXIT 0    /* set to 1 for debugging */
    #define CHECK_PASSED 0
    #define CHECK_FAILED 1
    extern int exit_echo;

    /* msvc's function name macro */
    #if defined ( _WIN32 )
        #define __func__ __FUNCTION__
    #endif

    #define SAFE_FREE(X) if(NULL != (X)) { free(X); (X) = NULL; }

    /* error functions */
    extern char err_buf[BUF_SIZE];
    #define exit_null_safe(X, ...) if(exit_null(__FILE__, __func__, __LINE__, (X), __VA_ARGS__)) return CHECK_FAILED
    #define exit_func_safe(X, ...) exit_func(__FILE__, __func__, __LINE__, EXIT_FAILURE, exit_msg(err_buf, (X), __VA_ARGS__))
    #define exit_abt_safe(X) exit_func(__FILE__, __func__, __LINE__, EXIT_FAILURE, (X))
    char * exit_msg(char * buffer, char * format, ...);
    int exit_null(const char *, const char *, const int, int, ...);
    int exit_func(const char *, const char *, const int, int, const char *);

    /* option interface */
    extern char * opt[26];
    char * get_argv(char);
    int parse_argv(char **, int);
    int path_concat(char *, size_t, size_t, const char *);

    /* array structure */
    typedef struct {
        void * array;
        int size;
        char delimit;
    } array_w;

    /* string functions */
    char * random_string(int);
    void strncopy(char *, int, const unsigned char *);
    int strnload(char * buf, int size, char * val);
    int ncs_strcmp(const char *, const char *);
    int convert_integer(const char *, int);
    void convert_integer_delimit(char *, char *, int, ...);
    int convert_integer_delimit_array(const char *, const char *, array_w *);
    int convert_integer_delimit_static(const char *, const char *, int *, int, int *);
    int convert_uinteger(const char *, int);
    char * convert_string(const char *);
    char * convert_stringn(const char *, int *);

    /* string delimit functions */
    int count_delimit(const char *, const char *);
    const char * substr_delimit(const char *, char *, const char *);

    /* array functions */
    char * array_to_string(char *, int *);
    char * array_to_string_cnt(char *, int *, int);
    int array_field_cnt(char *);

    /* grammer */
    int aeiou(char letter);

    /* sorting */
    typedef struct swap_t {
        int(*less)(void *, size_t, size_t);
        int(*same)(void *, size_t, size_t);
        void(*swap)(void *, size_t, size_t);
        void(*read)(void *, size_t);
    } swap_t;

    void sift_down(void *, size_t, size_t, swap_t *);
    int heap_sort(void *, size_t, swap_t *);
    int insertion_sort(void *, size_t, size_t, swap_t *);
    size_t mof3(void *, size_t, size_t, size_t, swap_t *);
    void pivot_sedgwick(void *, size_t, size_t, swap_t *);
    void pivot_ninther(void *, size_t, size_t, size_t, swap_t *);
    void partition_3way(void *, size_t, size_t, size_t *, size_t *, swap_t *);
    int _quick_sort(void *, size_t, size_t, size_t, swap_t *);
    int quick_sort(void *, size_t, swap_t *);
#endif

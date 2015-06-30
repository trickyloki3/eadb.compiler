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
 	#include "setting.h"

 	#define ENABLE_EXIT 0	/* set to 1 for debugging */
 	#define CHECK_PASSED 0
 	#define CHECK_FAILED 1
	
	/* msvc's function name macro */
	#if defined ( WIN32 )
		#define __func__ __FUNCTION__
	#endif

	

	/* error functions */
 	extern char err_buf[BUF_SIZE];
 	#define exit_null_safe(X, ...) exit_null(__FILE__, __func__, __LINE__, (X), __VA_ARGS__)
 	#define exit_func_safe(X, ...) exit_func(__FILE__, __func__, __LINE__, EXIT_FAILURE, exit_msg(err_buf, (X), __VA_ARGS__))
 	#define exit_abt_safe(X) exit_func(__FILE__, __func__, __LINE__, EXIT_FAILURE, (X))
 	char * exit_msg(char * buffer, char * format, ...);
	int exit_null(const char *, const char *, const int, int, ...);
	int exit_func(const char *, const char *, const int, int, const char *);

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
	int convert_integer_delimit_static(const char *, const char *, int *, int);
	int convert_uinteger(const char *, int);
	char * convert_string(const char *);

	/* string delimit functions */
	int count_delimit(const char *, const char *);
	const char * substr_delimit(const char *, char *, const char *);

	/* array functions */
	char * array_to_string(char *, int *);
	char * array_to_string_cnt(char *, int *, int);
	int array_field_cnt(char *);

	/* grammer */
	int add_es_or_s(char *);
#endif

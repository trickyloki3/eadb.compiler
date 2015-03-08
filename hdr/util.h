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
    #include <stdint.h>
 	#include "setting.h"

 	/* database buffer size */
 	#define BONUS_PREFIX_SIZE 8
 	#define BONUS_BONUS_SIZE 64
 	#define BONUS_FORMAT_SIZE 256
 	#define BONUS_PARAMATER_SIZE 5
 	#define CONST_NAME_SIZE 128
 	#define SKILL_NAME 64
 	#define SKILL_FORMAT 256
 	#define MOB_NAME_SIZE 64
 	#define MERC_NAME_SIZE 64
 	#define STATUS_NAME_SIZE 64
 	#define STATUS_FORMAT_SIZE 256
 	#define STATUS_PARAMATER_SIZE 4
 	#define VAR_ID 32
 	#define VAR_NAME 32
 	#define OPTION_SIZE 32
 	#define OPTION_NAME_SIZE 64
 	#define MAP_SIZE 64

	/* error checking and exiting */
 	#define ENABLE_EXIT 1
 	#define CHECK_PASSED 0 /* return by exit function for error checking */
 	#define CHECK_FAILED 1
 	extern char err_buf[BUF_SIZE];
 	#define exit_null_safe(X, ...) exit_null(__FILE__, __func__, __LINE__, (X), __VA_ARGS__)
 	#define exit_func_safe(X, ...) exit_func(__FILE__, __func__, __LINE__, EXIT_FAILURE, exit_msg(err_buf, (X), __VA_ARGS__))
 	#define exit_abt_safe(X) exit_func(__FILE__, __func__, __LINE__, EXIT_FAILURE, (X))
 	char * exit_msg(char * buffer, char * format, ...);
	int exit_null(const char *, const char *, const int, int, ...);
	int exit_func(const char *, const char *, const int, int, const char *);

	/* string of ASCII character set used for generating random string */
	#define ASCII_SET "abcdefghijklmnopqrstuvwxyz0123456789"
	#define ASCII_SZE 36
	/* Generate a string using the ASCII character set.
   	   int length - length of the string to generate. */
	char * random_string(int);

	/* Improved non-case sensitive string comparsion;
	 * Skip and adjust for non-digit and non-alpha;
	 * -1 s1 < s2 | 0 s1 == s2 | 1 s1 > s2 
	 */
	int ncs_strcmp(const char *, const char *);

	/* Convert the string into an integer.
	 * char * str - The string containing the integer.
	 * int base - The integer notation of the string, i.e
	 * binary, octal, decimal, hexadecimal, ... (2 - 32) 
	 */
	int convert_integer(const char *, int);
	int convert_uinteger(const char *, int);
	char * convert_string(const char *);

	/* utility array with sub-delimiting functions */
	typedef struct {  /* array wrapper */
		void * array;  /* array to int, long, etc  */
		int size;  /* total number of elements */
		char delimit;  /* delimiter to separate elements when writing */
	} array_w;
	void array_io(array_w, FILE *);
	void array_unload(array_w);

	/* special string sub-delimiting */
	void convert_delimit_integer(char *, char, int, ...);
	char * substr_delimit(char *, char *, char);
	const char * substr_delimit_list(const char *, char *, const char *);
	int convert_integer_list(char *, char *, array_w *);
	int convert_integer_list_static(const char *, const char *, int *, int);

	/* copy upto function */
	void strncopy(char *, int, const unsigned char *);
	int strnload(char * buf, int size, char * val);

	/* auxiliary converters */
	char * array_to_string(char *, int *);
	char * array_to_string_cnt(char *, int *, int);
	int array_field_cnt(char *);
#endif

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

	/* BUF_SIZE is the standard static buffer size */
	#define BUF_SIZE 4096

	/* standard program abortion function */
	#define exit_abt(X)	exit_throw_error((X), __FILE__, __func__, __LINE__)
	#define exit_null(Y, X, ...) if(exit_check_null((X), __VA_ARGS__)) exit_abt(Y);
	void exit_throw_error(const char *, const char *, const char *, const int);
	int exit_check_null(int, ...);
	char * build_buffer(char * buffer, char * format, ...);

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

	/* Allocate memory for the string.
   	 * char * str - The string to be placed into memory. 
   	 */
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
	void convert_integer_list(char *, char *, array_w *);
	char * substr_delimit(char *, char *, char);
	char * substr_delimit_list(char *, char *, char *);
#endif

/*
 *   file: setting.h
 *   date: 02/28/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */

#ifndef SETTING_H
#define SETTING_H
 	#define EATHENA_DB_COUNT		8			/* number of native eathena database loaders */
 	#define RATHENA_DB_COUNT		10 			/* number of native rathena database loaders */
 	#define HERCULES_DB_COUNT		7			/* number of native hercules database loaders */
 	#define RESOURCE_DB_COUNT		6			/* number of resource database loaders */

 	#define BUF_SIZE				4096		/* maximum buffer size in bytes */
	#define MAX_SCRIPT_SIZE 		4096		/* maximum length of script in bytes */
 	#define MAX_DESCRIPTION_SIZE	4096
 	#define MAX_NAME_SIZE 			128			/* maximum length of names in bytes */
 	#define MAX_FORMAT_SIZE			512			/* maximum length of format & description in bytes */
 	#define MAX_VARARG_SIZE 		128			/* maximum length of fields in bytes */
 	#define MAX_VARARG_COUNT 		32			/* maximum number of vararg fields */
 	#define MAX_INGREDIENT			16			/* maximum pair of item id and amount */
 	#define MAX_BONUS				5			/* maximum number of bonuses */
 
 	/* type definition for database with strings that contain parsable values,
 	 * 1:2:3:4:5 can be parsed into 5 separate values 1, 2, 3, 4, and 5 */
	typedef union {
		int var[MAX_VARARG_COUNT];	/* variable values parsed from string */
		char str[MAX_VARARG_SIZE];	/* string containing parsable values */
	} varlist;
#endif

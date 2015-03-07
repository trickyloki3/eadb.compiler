/*
 *   file: db_resources.h
 *   date: 03/07/2015
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef DB_RESOURCES_H
#define DB_RESOURCES_H
 	#include "load.h"
 	#include "util.h"
 	#include "setting.h"
 	#include "libconfig.h"

 	/* resource database type definitions 
	 * -----------------------------------------
	 * type name 	- 	type reference
	 * -----------------------------------------
	 * option_res 	- 	option database
	 * map_res 		- 	map name database
	 * bonus_res 	- 	bonus name database
	 * status_res 	- 	status name database
	 * var_res 		- 	variable and function name database
	 * block_res 	- 	block name database
	 */

	/* database record counts */
	#define OPTION_RES_FIELD_COUNT 	2
	#define MAP_RES_FIELD_COUNT 	2
	#define STATUS_RES_FIELD_COUNT	14
	#define VAR_RES_FIELD_COUNT		8
	#define BLOCK_RES_FIELD_COUNT	3

	typedef struct option_res {
		char option[MAX_NAME_SIZE];
		char name[MAX_NAME_SIZE];
	} option_res;

	typedef struct map_res {
		int id;
		char map[MAX_NAME_SIZE];
		char name[MAX_NAME_SIZE];
	} map_res;

	typedef struct {
		int id;
		int flag;
		int attr;
		char prefix[MAX_NAME_SIZE];
		char bonus[MAX_NAME_SIZE];
		char format[MAX_NAME_SIZE];
		int type[MAX_BONUS];
		int type_cnt;
		int order[MAX_BONUS];
		int order_cnt;
	} bonus_res;

	typedef struct {
		int scid;     					/* status id */
		char scstr[MAX_NAME_SIZE]; 		/* status string identifer */
		int type;
		char scfmt[MAX_FORMAT_SIZE]; 	/* format of status string */
		char scend[MAX_FORMAT_SIZE]; 	/* format of status end string */
		int vcnt;     					/* value count */
		int vmod[4];  					/* modifer for value */
		int voff[4];  					/* offset translation stack */
	} status_res;

	typedef struct {
      int tag;         			/* in-house identification */
      char id[MAX_NAME_SIZE]; 	/* identifier string */
      int type;        			/* type of identifer and flags */
      int vflag;
      int fflag;
      int min;         			/* help calculations */
      int max;
      char str[MAX_NAME_SIZE];
   } var_res;

   typedef struct block_res {
		int id;
		char name[MAX_NAME_SIZE];
		int flag;
	} block_res;

	/* resource database loading */
	extern native_config_t load_res_native[RESOURCE_DB_COUNT];
	int option_res_load(void *, int, int, char *);
	int map_res_load(void *, int, int, char *);
	int bonus_res_load(void *, int, int, char *);
	int status_res_load(void *, int, int, char *);
	int var_res_load(void *, int, int, char *);
	int block_res_load(void *, int, int, char *);
	int load_resource_database(const char * resource_path);
#endif
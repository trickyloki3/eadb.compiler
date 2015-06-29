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
 	#include "sqlite3.h"

 	/* resource database type definitions 
	 * -----------------------------------------
	 * type name 	- 	type reference
	 * -----------------------------------------
	 * option_res 	- 	option database
	 * bonus_res 	- 	bonus name database
	 * status_res 	- 	status name database
	 * var_res 		- 	variable and function name database
	 * block_res 	- 	block name database
	 * map_res 		- 	client-side map name table
	 * id_res 		- 	client-side identified item resource table
	 * numid_res 	- 	client-side unidentified item resource table
	 */

	/* database record counts */
	#define OPTION_RES_FIELD_COUNT 	2
	#define MAP_RES_FIELD_COUNT 	2
	#define STATUS_RES_FIELD_COUNT	14
	#define VAR_RES_FIELD_COUNT		8
	#define BLOCK_RES_FIELD_COUNT	3
	#define NID_RES_FIELD_COUNT		2

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

	/* type flag */
	#define TYPE_FUNC 0x00000001
	#define TYPE_VARI 0x00000002

	/* variable flags */
	#define VARI_DIRECT 0x00000001

	/* function flags */
	#define FUNC_DIRECT     0x00000001  /* retrieve min and max directly */
	#define FUNC_PARAMS     0x00000002  /* retrieve min and max from handler */
	#define FUNC_GETSKILLLV 0x10000000  /* getskilllv */
	#define FUNC_RAND       0x20000000  /* rand */
	#define FUNC_POW        0x40000000  /* pow */
	
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

	typedef struct nid_res {
		int id;
		char res[MAX_RESNAME_SIZE];
	} nid_res;

	/* resources sqlite3 database interface */
	#define resource_database_sql 	"DROP TABLE IF EXISTS option_res;"																	\
									"DROP TABLE IF EXISTS map_res;"																		\
									"DROP TABLE IF EXISTS bonus_res;"																	\
									"DROP TABLE IF EXISTS status_res;"																	\
									"DROP TABLE IF EXISTS var_res;"																		\
									"DROP TABLE IF EXISTS block_res;"																	\
									""																									\
									"CREATE TABLE IF NOT EXISTS option_res(opt TEXT, name TEXT, PRIMARY KEY(opt));"						\
									""																									\
									"CREATE TABLE IF NOT EXISTS map_res(id INTEGER, map TEXT, name TEXT, PRIMARY KEY(id));"				\
									""																									\
									"CREATE TABLE IF NOT EXISTS bonus_res(" 															\
									"id INTEGER, flag INTEGER, attr INTEGER, pref TEXT, buff TEXT, desc TEXT," 							\
									"type_param TEXT, type_cnt INTEGER, order_param TEXT, "												\
									"order_cnt INTEGER, PRIMARY KEY(id, pref, buff));"													\
									""																									\
									"CREATE TABLE IF NOT EXISTS status_res("															\
									"scid INTEGER, scstr TEXT, type INTEGER, scfmt TEXT, "												\
									"scend TEXT, vcnt INTEGER, vmod TEXT, voff TEXT, PRIMARY KEY(scid, scstr));"						\
									""																									\
									"CREATE TABLE IF NOT EXISTS var_res(tag INTEGER PRIMARY KEY, id TEXT, type INTEGER,  "				\
									"vflag INTEGER, fflag INTEGER, min INTEGER, max INTEGER, str TEXT);"								\
									""																									\
									"CREATE TABLE IF NOT EXISTS block_res(id INTEGER PRIMARY KEY, key TEXT, flag INTEGER);"				\
									""																									\
									"CREATE TABLE IF NOT EXISTS id_res(id INTEGER PRIMARY KEY, res TEXT);"								\
									""																									\
									"CREATE TABLE IF NOT EXISTS numid_res(id INTEGER PRIMARY KEY, res TEXT);"							\

	#define option_insert 			"INSERT INTO option_res VALUES(?, ?);"
	#define map_insert				"INSERT INTO map_res VALUES(?, ?, ?);"
	#define bonus_insert			"INSERT INTO bonus_res VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define status_insert			"INSERT INTO status_res VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
	#define var_insert				"INSERT INTO var_res VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
	#define block_insert			"INSERT INTO block_res VALUES(?, ?, ?);"
	#define id_res_insert			"INSERT INTO id_res VALUES(?, ?);"
	#define numid_res_insert		"INSERT INTO numid_res VALUES(?, ?);"

	typedef struct db_res_t {
		sqlite3 * db;
		sqlite3_stmt * option_sql_insert;
		sqlite3_stmt * map_sql_insert;
		sqlite3_stmt * bonus_sql_insert;
		sqlite3_stmt * status_sql_insert;
		sqlite3_stmt * var_sql_insert;
		sqlite3_stmt * block_sql_insert;
		sqlite3_stmt * id_res_sql_insert;
		sqlite3_stmt * numid_res_sql_insert;
	} db_res_t;

	/* resource database loading */
	/*ITEML_API extern native_config_t load_res_native[RESOURCE_DB_COUNT];*/
	static int option_res_load(void *, int, int, char *);
	static int map_res_load(void *, int, int, char *);
	static int bonus_res_load(void *, int, int, char *);
	static int status_res_load(void *, int, int, char *);
	static int var_res_load(void *, int, int, char *);
	static int block_res_load(void *, int, int, char *);
	static int nid_res_load(void *, int, int, char *);

	/* database loading depends on the path of the database */
	ITEML_API int create_resource_database(db_res_t * db, const char * path);
	ITEML_API int finalize_resource_database(db_res_t * db);
	ITEML_API int resource_option_sql_load(db_res_t * db, const char * path);
	ITEML_API int resource_map_sql_load(db_res_t * db, const char * path);
	ITEML_API int resource_bonus_sql_load(db_res_t * db, const char * path);
	ITEML_API int resource_status_sql_load(db_res_t * db, const char * path);
	ITEML_API int resource_var_sql_load(db_res_t * db, const char * path);
	ITEML_API int resource_block_sql_load(db_res_t * db, const char * path);
	ITEML_API int resource_id_res_sql_load(db_res_t * db, const char * path);
	ITEML_API int resource_num_id_res_sql_load(db_res_t * db, const char * path);
#endif

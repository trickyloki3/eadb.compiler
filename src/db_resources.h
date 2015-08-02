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
 	#include "sqlite3.h"

	/* database record counts */
	#define OPTION_RES_FIELD_COUNT 	2
	#define MAP_RES_FIELD_COUNT 	2
	#define STATUS_RES_FIELD_COUNT	14
	#define VAR_RES_FIELD_COUNT		8
	#define BLOCK_RES_FIELD_COUNT	3
	#define NID_RES_FIELD_COUNT		2

	/* option table maps option constants to a user-friendly string */
	typedef struct option_res {
		char option[MAX_NAME_SIZE];
		char name[MAX_NAME_SIZE];
	} option_res;

	#define RES_OPT_DELETE			"DROP TABLE IF EXISTS option_res;"
	#define RES_OPT_INSERT			"INSERT INTO option_res VALUES(?, ?);"
	#define RES_OPT_CREATE			"CREATE TABLE IF NOT EXISTS option_res("\
									"	opt TEXT,"\
									"	name TEXT,"\
									"	PRIMARY KEY(opt)"\
									");"


	/* map table maps the filename of the map to the name of the map */
	typedef struct map_res {
		int id;
		char map[MAX_NAME_SIZE];
		char name[MAX_NAME_SIZE];
	} map_res;

	#define RES_MAP_DELETE			"DROP TABLE IF EXISTS map_res;"
	#define RES_MAP_INSERT			"INSERT INTO map_res VALUES(?, ?, ?);"
	#define RES_MAP_CREATE			"CREATE TABLE IF NOT EXISTS map_res("\
									"	id INTEGER,"\
									"	map TEXT,"\
									"	name TEXT,"\
									"	PRIMARY KEY(id)"\
									");"

	/* bonus table consist of a set of template strings and argument types for translating bonuses. */
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

	#define RES_BNS_DELETE			"DROP TABLE IF EXISTS bonus_res;"
	#define RES_BNS_INSERT			"INSERT INTO bonus_res VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define RES_BNS_CREATE			"CREATE TABLE IF NOT EXISTS bonus_res("\
									"	id INTEGER,"\
									"	flag INTEGER,"\
									"	attr INTEGER,"\
									"	pref TEXT,"\
									"	buff TEXT,"\
									"	desc TEXT,"\
									"	type_param TEXT,"\
									"	type_cnt INTEGER,"\
									"	order_param TEXT,"\
									"	order_cnt INTEGER,"\
									"	PRIMARY KEY(id, pref, buff)"\
									");"

	/* status table consist of a set of template strings and argument types for translating statuses */
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

	#define RES_STA_DELETE			"DROP TABLE IF EXISTS status_res;"
	#define RES_STA_INSERT			"INSERT INTO status_res VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
	#define RES_STA_CREATE			"CREATE TABLE IF NOT EXISTS status_res("\
									"	scid INTEGER,"\
									"	scstr TEXT,"\
									"	type INTEGER,"\
									"	scfmt TEXT,"\
									"	scend TEXT,"\
									"	vcnt INTEGER,"\
									"	vmod TEXT,"\
									"	voff TEXT,"\
									"	PRIMARY KEY(scid, scstr)"\
									");"

	/* variable table contain specific information on the script engine's special variables */
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

   	#define RES_VAR_DELETE			"DROP TABLE IF EXISTS var_res;"
	#define RES_VAR_INSERT			"INSERT INTO var_res VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
	#define RES_VAR_CREATE			"CREATE TABLE IF NOT EXISTS var_res("\
   									"	tag INTEGER PRIMARY KEY,"\
   									"	id TEXT,"\
   									"	type INTEGER,"\
									"	vflag INTEGER,"\
									"	fflag INTEGER,"\
									"	min INTEGER,"\
									"	max INTEGER,"\
									"	str TEXT"\
									");"

   	#define TYPE_FUNC 				0x00000001
	#define TYPE_VARI 				0x00000002
	#define VARI_DIRECT 			0x00000001
	#define FUNC_DIRECT     		0x00000001  /* retrieve min and max directly */
	#define FUNC_PARAMS     		0x00000002  /* retrieve min and max from handler */
	#define FUNC_GETSKILLLV 		0x10000000  /* getskilllv */
	#define FUNC_RAND       		0x20000000  /* rand */
	#define FUNC_POW        		0x40000000  /* pow */

   /* block table contains a list of keywords */
   typedef struct block_res {
		int id;
		char name[MAX_NAME_SIZE];
		int flag;
	} block_res;

	#define RES_BLK_DELETE			"DROP TABLE IF EXISTS block_res;"
	#define RES_BLK_INSERT			"INSERT INTO block_res VALUES(?, ?, ?);"
	#define RES_BLK_CREATE			"CREATE TABLE IF NOT EXISTS block_res("\
									"	id INTEGER PRIMARY KEY,"\
									"	key TEXT,"\
									"	flag INTEGER"\
									");"

	/* sprite table contains item id mapped to a resource filename */
	typedef struct nid_res {
		int id;
		char res[MAX_RESNAME_SIZE];
	} nid_res;

	#define RES_RID_DELETE			"DROP TABLE IF EXISTS id_res;"
	#define RES_RID_INSERT			"INSERT INTO id_res VALUES(?, ?);"
	#define RES_RID_CREATE			"CREATE TABLE IF NOT EXISTS id_res("\
									"	id INTEGER PRIMARY KEY,"\
									"	res TEXT"\
									");"

	#define RES_NID_DELETE			"DROP TABLE IF EXISTS numid_res;"
	#define RES_NID_CREATE			"CREATE TABLE IF NOT EXISTS numid_res("\
									"	id INTEGER PRIMARY KEY,"\
									"	res TEXT"\
									");"
	#define RES_NID_INSERT			"INSERT INTO numid_res VALUES(?, ?);"

	typedef struct opt_db_t {
		sqlite3 * db;
		sqlite3_stmt * option_sql_insert;
		sqlite3_stmt * map_sql_insert;
		sqlite3_stmt * bonus_sql_insert;
		sqlite3_stmt * status_sql_insert;
		sqlite3_stmt * var_sql_insert;
		sqlite3_stmt * block_sql_insert;
		sqlite3_stmt * id_res_sql_insert;
		sqlite3_stmt * numid_res_sql_insert;
	} opt_db_t;

	/* resource database loading */
	int option_res_load(void *, int, int, char *);
	int map_res_load(void *, int, int, char *);
	int bonus_res_load(void *, int, int, char *);
	int status_res_load(void *, int, int, char *);
	int var_res_load(void *, int, int, char *);
	int block_res_load(void *, int, int, char *);
	int nid_res_load(void *, int, int, char *);

	int opt_db_init(opt_db_t **, const char *);
	int opt_db_deit(opt_db_t **);
	int opt_db_exec(opt_db_t *, char *);
	int opt_db_res_load(opt_db_t *, const char *);
	int opt_db_res_load_record(option_res *, int, sqlite3_stmt *);
	int opt_db_map_load(opt_db_t *, const char *);
	int opt_db_map_load_record(map_res *, int, sqlite3_stmt *);
	int opt_db_bns_load(opt_db_t *, const char *);
	int opt_db_bns_load_record(bonus_res *, int, sqlite3_stmt *);
	int opt_db_sta_load(opt_db_t *, const char *);
	int opt_db_sta_load_record(status_res *, int, sqlite3_stmt *);
	int opt_db_var_load(opt_db_t *, const char *);
	int opt_db_var_load_record(var_res *, int, sqlite3_stmt *);
	int opt_db_blk_load(opt_db_t *, const char *);
	int opt_db_blk_load_record(block_res *, int, sqlite3_stmt *);
	int opt_db_rid_load(opt_db_t *, const char *);
	int opt_db_rid_load_record(nid_res *, int, sqlite3_stmt *);
	int opt_db_nid_load(opt_db_t *, const char *);
	int opt_db_nid_load_record(nid_res *, int, sqlite3_stmt *);
#endif

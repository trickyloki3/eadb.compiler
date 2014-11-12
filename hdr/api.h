#ifndef SQL_H
#define SQL_H
	#include "db.h"
	#include "util.h"
	#include "sqlite3.h"
	#include <assert.h>
	#define ea_item_tbl "CREATE TABLE IF NOT EXISTS ea_item(" \
						"id INTEGER PRIMARY KEY, aegis TEXT, eathena TEXT," \
						"type INTEGER, buy INTEGER, sell INTEGER, weight INTEGER, " \
						"atk INTEGER, def INTEGER, range INTEGER, slots INTEGER, " \
						"job INTEGER, upper INTEGER, gender INTEGER, loc INTEGER, " \
						"wlv INTEGER, elv INTEGER, refineable INTEGER, view INTEGER, " \
						"script TEXT, onequip TEXT, onunequip TEXT);"
	#define ea_item_des "DROP TABLE IF EXISTS ea_item;"
	#define ea_item_ins "INSERT INTO ea_item VALUES(" \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 

	#define ra_item_tbl "CREATE TABLE IF NOT EXISTS ra_item(" \
						"id INTEGER PRIMARY KEY, aegis TEXT, eathena TEXT," \
						"type INTEGER, buy INTEGER, sell INTEGER, weight INTEGER, " \
						"matk INTEGER, atk INTEGER, def INTEGER, range INTEGER, slots INTEGER, " \
						"job INTEGER, upper INTEGER, gender INTEGER, loc INTEGER, " \
						"wlv INTEGER, elv INTEGER, refineable INTEGER, view INTEGER, " \
						"script TEXT, onequip TEXT, onunequip TEXT);"
	#define ra_item_des "DROP TABLE IF EXISTS ra_item;"
	#define ra_item_ins "INSERT INTO ra_item VALUES(?, " \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 

	#define he_item_tbl "CREATE TABLE IF NOT EXISTS he_item(" \
						"Id INTEGER PRIMARY KEY, " \
						"AegisName TEXT, " \
						"Name TEXT, " \
						"Type INTEGER, " \
						"Buy INTEGER, " \
						"Sell INTEGER, " \
						"Weight INTEGER, " \
						"Atk INTEGER, " \
						"Matk INTEGER, " \
						"Def INTEGER, " \
						"Range INTEGER, " \
						"Slots INTEGER, " \
						"Job INTEGER, " \
						"Upper INTEGER, " \
						"Gender INTEGER, " \
						"Loc INTEGER, " \
						"WeaponLv INTEGER, " \
						"EquipLvMin INTEGER, " \
						"EquipLvMax INTEGER, " \
						"Refine INTEGER, " \
						"View INTEGER, " \
						"BindOnEquip INTEGER, " \
						"BuyingStore INTEGER, " \
						"Delay INTEGER, " \
						"override INTEGER, " \
						"nodrop INTEGER, " \
						"notrade INTEGER, " \
						"partneroverride INTEGER, " \
						"noselltonpc INTEGER, " \
						"nocart INTEGER, " \
						"nostorage INTEGER, " \
						"nogstorage INTEGER, " \
						"nomail INTEGER, " \
						"noauction INTEGER, " \
						"ltoverride INTEGER, " \
						"sitting INTEGER, " \
						"StackAmount INTEGER, " \
						"StackType INTEGER, " \
						"Sprite Integer, " \
						"Script TEXT, " \
						"OnEquipScript TEXT, " \
						"OnUnequipScript TEXT);"
	
	#define he_item_des "DROP TABLE IF EXISTS he_item;"

	#define he_item_ins "INSERT INTO he_item(" \
						"Id, AegisName, Name, Type, Buy, Sell, Weight, Atk, Matk," \
						"Def, Range, Slots, Job, Upper, Gender, Loc, WeaponLv, " \
						"EquipLvMin, EquipLvMax, Refine, View, BindOnEquip, " \
						"BuyingStore, Delay, override, nodrop, notrade, partneroverride, " \
						"noselltonpc, nocart, nostorage, nogstorage, nomail, noauction, " \
						"ltoverride, sitting, StackAmount, StackType, Sprite, Script, " \
						"OnEquipScript, OnUnequipScript) VALUES(" \
						"$Id, $AegisName, $Name, $Type, $Buy, $Sell, $Weight, $Atk, $Matk," \
						"$Def, $Range, $Slots, $Job, $Upper, $Gender, $Loc, $WeaponLv, " \
						"$EquipLvMin, $EquipLvMax, $Refine, $View, $BindOnEquip, " \
						"$BuyingStore, $Delay, $override, $nodrop, $notrade, $partneroverride, " \
						"$noselltonpc, $nocart, $nostorage, $nogstorage, $nomail, $noauction, " \
						"$ltoverride, $sitting, $StackAmount, $StackType, $Sprite, $Script, " \
						"$OnEquipScript, $OnUnequipScript);" 

	#define ea_pet_tbl 	"CREATE TABLE IF NOT EXISTS ea_pet(" \
						"mob_id INTEGER PRIMARY KEY, pet_name TEXT, pet_jname TEXT, " \
						"lure_id INTEGER, egg_id INTEGER, equip_id INTEGER, " \
						"food_id INTEGER, fullness INTEGER, hungry_delay INTEGER, " \
						"r_hungry INTEGER, r_full INTEGER, intimate INTEGER, " \
						"die INTEGER, capture INTEGER, speed INTEGER, " \
						"s_performance INTEGER, talk_convert INTEGER, attack_rate INTEGER, " \
						"defence_attack_rate INTEGER, change_target_rate INTEGER, pet_script TEXT, " \
						"loyal_script TEXT);"
	#define ra_pet_tbl 	"CREATE TABLE IF NOT EXISTS ra_pet(" \
						"mob_id INTEGER PRIMARY KEY, pet_name TEXT, pet_jname TEXT, " \
						"lure_id INTEGER, egg_id INTEGER, equip_id INTEGER, " \
						"food_id INTEGER, fullness INTEGER, hungry_delay INTEGER, " \
						"r_hungry INTEGER, r_full INTEGER, intimate INTEGER, " \
						"die INTEGER, capture INTEGER, speed INTEGER, " \
						"s_performance INTEGER, talk_convert INTEGER, attack_rate INTEGER, " \
						"defence_attack_rate INTEGER, change_target_rate INTEGER, pet_script TEXT, " \
						"loyal_script TEXT);"
	#define he_pet_tbl 	"CREATE TABLE IF NOT EXISTS he_pet(" \
						"mob_id INTEGER PRIMARY KEY, pet_name TEXT, pet_jname TEXT, " \
						"lure_id INTEGER, egg_id INTEGER, equip_id INTEGER, " \
						"food_id INTEGER, fullness INTEGER, hungry_delay INTEGER, " \
						"r_hungry INTEGER, r_full INTEGER, intimate INTEGER, " \
						"die INTEGER, capture INTEGER, speed INTEGER, " \
						"s_performance INTEGER, talk_convert INTEGER, attack_rate INTEGER, " \
						"defence_attack_rate INTEGER, change_target_rate INTEGER, pet_script TEXT, " \
						"loyal_script TEXT);"
	#define ea_pet_ins "INSERT INTO ea_pet VALUES(" \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 
	#define ra_pet_ins "INSERT INTO ra_pet VALUES(" \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define he_pet_ins "INSERT INTO he_pet VALUES(" \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 
	#define ea_pet_des "DROP TABLE IF EXISTS ea_pet;"
	#define ra_pet_des "DROP TABLE IF EXISTS ra_pet;"
	#define he_pet_des "DROP TABLE IF EXISTS he_pet;"

	#define ea_merc_tbl "CREATE TABLE IF NOT EXISTS ea_merc(" \
						"id INTEGER PRIMARY KEY, sprite TEXT, name TEXT, " \
						"lv INTEGER, hp INTEGER, sp INTEGER, range1 INTEGER, " \
						"atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " \
						"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, " \
						"dex INTEGER, luk INTEGER, range2 INTEGER, range3 INTEGER, " \
						"scale INTEGER, race INTEGER, element INTEGER, speed INTEGER, " \
						"adelay INTEGER, amotion INTEGER, dmotion INTEGER);"
	#define ra_merc_tbl "CREATE TABLE IF NOT EXISTS ra_merc(" \
						"id INTEGER PRIMARY KEY, sprite TEXT, name TEXT, " \
						"lv INTEGER, hp INTEGER, sp INTEGER, range1 INTEGER, " \
						"atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " \
						"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, " \
						"dex INTEGER, luk INTEGER, range2 INTEGER, range3 INTEGER, " \
						"scale INTEGER, race INTEGER, element INTEGER, speed INTEGER, " \
						"adelay INTEGER, amotion INTEGER, dmotion INTEGER);"
	#define he_merc_tbl "CREATE TABLE IF NOT EXISTS he_merc(" \
						"id INTEGER PRIMARY KEY, sprite TEXT, name TEXT, " \
						"lv INTEGER, hp INTEGER, sp INTEGER, range1 INTEGER, " \
						"atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " \
						"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, " \
						"dex INTEGER, luk INTEGER, range2 INTEGER, range3 INTEGER, " \
						"scale INTEGER, race INTEGER, element INTEGER, speed INTEGER, " \
						"adelay INTEGER, amotion INTEGER, dmotion INTEGER);"
	#define ea_merc_ins "INSERT INTO ea_merc VALUES(" \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
						"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 
	#define ra_merc_ins "INSERT INTO ra_merc VALUES(" \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 
	#define he_merc_ins "INSERT INTO he_merc VALUES(" \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" 
	#define ea_merc_des "DROP TABLE IF EXISTS ea_merc;"
	#define ra_merc_des "DROP TABLE IF EXISTS ra_merc;"
	#define he_merc_des "DROP TABLE IF EXISTS he_merc;"
	
	#define ea_prod_tbl "CREATE TABLE IF NOT EXISTS ea_prod(" \
						"item_id INTEGER PRIMARY KEY, item_lv INTEGER, " \
						"req_skill INTEGER, req_skill_lv INTEGER, material TEXT, amount TEXT);"
	#define ra_prod_tbl "CREATE TABLE IF NOT EXISTS ra_prod(" \
						"id INTEGER PRIMARY KEY, item_id INTEGER, item_lv INTEGER, " \
						"req_skill INTEGER, req_skill_lv INTEGER, material TEXT, amount TEXT);"
	#define he_prod_tbl "CREATE TABLE IF NOT EXISTS he_prod(" \
						"item_id INTEGER PRIMARY KEY, item_lv INTEGER, " \
						"req_skill INTEGER, req_skill_lv INTEGER, material TEXT, amount TEXT);"
	#define ea_prod_ins "INSERT INTO ea_prod VALUES(?, ?, ?, ?, ?, ?);"
	#define ra_prod_ins "INSERT INTO ra_prod VALUES(?, ?, ?, ?, ?, ?, ?);"
	#define he_prod_ins "INSERT INTO he_prod VALUES(?, ?, ?, ?, ?, ?);"
	#define ea_prod_des "DROP TABLE IF EXISTS ea_prod;"
	#define ra_prod_des "DROP TABLE IF EXISTS ra_prod;"
	#define he_prod_des "DROP TABLE IF EXISTS he_prod;"

	#define ea_skill_tbl "CREATE TABLE IF NOT EXISTS ea_skill(" \
						 "id INTEGER PRIMARY KEY, range TEXT, hit INTEGER, inf INTEGER, " \
						 "element TEXT, nk INTEGER, splash TEXT, max INTEGER, hit_amount INTEGER, " \
						 "cast_cancel TEXT, cast_def_reduce_rate INTEGER, inf2 INTEGER, " \
						 "maxcount TEXT, type TEXT, blow_count TEXT, name TEXT, desc TEXT);"
	#define ra_skill_tbl "CREATE TABLE IF NOT EXISTS ra_skill(" \
						 "id INTEGER PRIMARY KEY, range TEXT, hit INTEGER, inf INTEGER, " \
						 "element TEXT, nk INTEGER, splash TEXT, max INTEGER, hit_amount INTEGER, " \
						 "cast_cancel TEXT, cast_def_reduce_rate INTEGER, inf2 INTEGER, " \
						 "maxcount TEXT, type TEXT, blow_count TEXT, inf3 INTEGER, name TEXT, desc TEXT);"
	#define he_skill_tbl "CREATE TABLE IF NOT EXISTS he_skill(" \
						 "id INTEGER PRIMARY KEY, range TEXT, hit INTEGER, inf INTEGER, " \
						 "element TEXT, nk INTEGER, splash TEXT, max INTEGER, hit_amount INTEGER, " \
						 "cast_cancel TEXT, cast_def_reduce_rate INTEGER, inf2 INTEGER, " \
						 "maxcount TEXT, type TEXT, blow_count TEXT, name TEXT, desc TEXT);"
	#define ea_skill_ins "INSERT INTO ea_skill VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define ra_skill_ins "INSERT INTO ra_skill VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define he_skill_ins "INSERT INTO he_skill VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
	#define ea_skill_des "DROP TABLE IF EXISTS ea_skill;"
	#define ra_skill_des "DROP TABLE IF EXISTS ra_skill;"
	#define he_skill_des "DROP TABLE IF EXISTS he_skill;"

	#define ea_mob_tbl	"CREATE TABLE IF NOT EXISTS ea_mob(" \
						"id INTEGER PRIMARY KEY, sprite TEXT, kro TEXT, iro TEXT, " \
						"lv INTEGER, hp INTEGER, sp INTEGER, exp INTEGER, jexp INTEGER, " \
						"range INTEGER, atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " \
						"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, dex INTEGER, " \
						"luk INTEGER, range2 INTEGER, range3 INTEGER, scale INTEGER, race INTEGER, " \
						"element INTEGER, mode INTEGER, speed INTEGER, adelay INTEGER, amotion INTEGER, " \
						"dmotion INTEGER, mexp INTEGER, expper INTEGER, mvp1id INTEGER, mvp1per INTEGER, " \
						"mvp2id INTEGER, mvp2per INTEGER, mvp3id INTEGER, mvp3per INTEGER, drop1id INTEGER, " \
						"drop1per INTEGER, drop2id INTEGER, drop2per INTEGER, drop3id INTEGER, drop3per INTEGER, " \
						"drop4id INTEGER, drop4per INTEGER, drop5id INTEGER, drop5per INTEGER, drop6id INTEGER, " \
						"drop6per INTEGER, drop7id INTEGER, drop7per INTEGER, drop8id INTEGER, drop8per INTEGER, " \
						"drop9id INTEGER, drop9per INTEGER, dropcardid INTEGER, dropcardper INTEGER); "
	#define ra_mob_tbl	"CREATE TABLE IF NOT EXISTS ra_mob(" \
						"id INTEGER PRIMARY KEY, sprite TEXT, kro TEXT, iro TEXT, " \
						"lv INTEGER, hp INTEGER, sp INTEGER, exp INTEGER, jexp INTEGER, " \
						"range INTEGER, atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " \
						"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, dex INTEGER, " \
						"luk INTEGER, range2 INTEGER, range3 INTEGER, scale INTEGER, race INTEGER, " \
						"element INTEGER, mode INTEGER, speed INTEGER, adelay INTEGER, amotion INTEGER, " \
						"dmotion INTEGER, mexp INTEGER, mvp1id INTEGER, mvp1per INTEGER, " \
						"mvp2id INTEGER, mvp2per INTEGER, mvp3id INTEGER, mvp3per INTEGER, drop1id INTEGER, " \
						"drop1per INTEGER, drop2id INTEGER, drop2per INTEGER, drop3id INTEGER, drop3per INTEGER, " \
						"drop4id INTEGER, drop4per INTEGER, drop5id INTEGER, drop5per INTEGER, drop6id INTEGER, " \
						"drop6per INTEGER, drop7id INTEGER, drop7per INTEGER, drop8id INTEGER, drop8per INTEGER, " \
						"drop9id INTEGER, drop9per INTEGER, dropcardid INTEGER, dropcardper INTEGER); "
	#define he_mob_tbl	"CREATE TABLE IF NOT EXISTS he_mob(" \
						"id INTEGER PRIMARY KEY, sprite TEXT, kro TEXT, iro TEXT, " \
						"lv INTEGER, hp INTEGER, sp INTEGER, exp INTEGER, jexp INTEGER, " \
						"range INTEGER, atk1 INTEGER, atk2 INTEGER, def INTEGER, mdef INTEGER, " \
						"str INTEGER, agi INTEGER, vit INTEGER, intr INTEGER, dex INTEGER, " \
						"luk INTEGER, range2 INTEGER, range3 INTEGER, scale INTEGER, race INTEGER, " \
						"element INTEGER, mode INTEGER, speed INTEGER, adelay INTEGER, amotion INTEGER, " \
						"dmotion INTEGER, mexp INTEGER, mvp1id INTEGER, mvp1per INTEGER, " \
						"mvp2id INTEGER, mvp2per INTEGER, mvp3id INTEGER, mvp3per INTEGER, drop1id INTEGER, " \
						"drop1per INTEGER, drop2id INTEGER, drop2per INTEGER, drop3id INTEGER, drop3per INTEGER, " \
						"drop4id INTEGER, drop4per INTEGER, drop5id INTEGER, drop5per INTEGER, drop6id INTEGER, " \
						"drop6per INTEGER, drop7id INTEGER, drop7per INTEGER, drop8id INTEGER, drop8per INTEGER, " \
						"drop9id INTEGER, drop9per INTEGER, dropcardid INTEGER, dropcardper INTEGER); "
	#define ea_mob_ins "INSERT INTO ea_mob VALUES(" \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?);"
	#define ra_mob_ins "INSERT INTO ra_mob VALUES(" \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?);"
	#define he_mob_ins "INSERT INTO he_mob VALUES(" \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?, ?, ?, ?," \
							"?, ?, ?, ?, ?, ?, ?);"
	#define ea_mob_des 	"DROP TABLE IF EXISTS ea_mob;"
	#define ra_mob_des 	"DROP TABLE IF EXISTS ra_mob;"
	#define he_mob_des 	"DROP TABLE IF EXISTS he_mob;"

	#define itm_block_tbl	"CREATE TABLE IF NOT EXISTS block(bk_id INTEGER, bk_kywd TEXT, bk_flag INTEGER);"
	#define itm_block_ins	"INSERT INTO block VALUES(?, ?, ?);"
	#define itm_block_des	"DROP TABLE IF EXISTS block;"

	#define itm_var_tbl		"CREATE TABLE IF NOT EXISTS var("\
							"tag INTEGER PRIMARY KEY, id TEXT, type INTEGER, vflag INTEGER, "\
							"fflag INTEGER, min INTEGER, max INTEGER);"
	#define itm_var_ins		"INSERT INTO var VALUES(?, ?, ?, ?, ?, ?, ?);"
	#define itm_var_des		"DROP TABLE IF EXISTS var;"

	#define itm_status_tbl	"CREATE TABLE IF NOT EXISTS status("\
							"scid INTEGER PRIMARY KEY, scstr TEXT, type INTEGER, scfmt TEXT, "\
							"scend TEXT, vcnt INTEGER, vmod TEXT, voff TEXT);"
	#define itm_status_ins	"INSERT INTO status VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
	#define itm_status_des	"DROP TABLE IF EXISTS status;"

	#define itm_bonus_tbl	"CREATE TABLE IF NOT EXISTS bonus(" \
							"pref TEXT, buff TEXT PRIMARY KEY, attr INTEGER, desc TEXT," \
							"type_param TEXT, type_cnt INTEGER, order_param TEXT, order_cnt INTEGER);"
	#define itm_bonus_ins	"INSERT INTO bonus VALUES(?, ?, ?, ?, ?, ?, ?, ?);"
	#define itm_bonus_des	"DROP TABLE IF EXISTS bonus;"

	#define ea_const_tbl	"CREATE TABLE IF NOT EXISTS ea_const(name TEXT, value INTEGER, type INTEGER);"
	#define ra_const_tbl	"CREATE TABLE IF NOT EXISTS ra_const(name TEXT, value INTEGER, type INTEGER);"
	#define he_const_tbl	"CREATE TABLE IF NOT EXISTS he_const(name TEXT, value INTEGER, type INTEGER);"
	#define ea_const_ins	"INSERT INTO ea_const VALUES(?, ?, ?);"
	#define ra_const_ins	"INSERT INTO ra_const VALUES(?, ?, ?);"
	#define he_const_ins	"INSERT INTO he_const VALUES(?, ?, ?);"
	#define ea_const_des 	"DROP TABLE IF EXISTS ea_const;"
	#define ra_const_des 	"DROP TABLE IF EXISTS ra_const;"
	#define he_const_des 	"DROP TABLE IF EXISTS he_const;"
	
	struct lt_db_t {
		sqlite3 * db;
		sqlite3_stmt * he_item_insert;
		sqlite3_stmt * ea_item_insert;
		sqlite3_stmt * ra_item_insert;
		/* pet table */
		sqlite3_stmt * ea_pet_insert;
		sqlite3_stmt * ra_pet_insert;
		sqlite3_stmt * he_pet_insert;
		/* mercenary table */
		sqlite3_stmt * ea_merc_insert;
		sqlite3_stmt * ra_merc_insert;
		sqlite3_stmt * he_merc_insert;
		/* produce table */
		sqlite3_stmt * ea_prod_insert;
		sqlite3_stmt * ra_prod_insert;
		sqlite3_stmt * he_prod_insert;
		/* skill table */
		sqlite3_stmt * ea_skill_insert;
		sqlite3_stmt * ra_skill_insert;
		sqlite3_stmt * he_skill_insert;
		/* mob table */
		sqlite3_stmt * ea_mob_insert;
		sqlite3_stmt * ra_mob_insert;
		sqlite3_stmt * he_mob_insert;
		/* item compile */
		sqlite3_stmt * block_insert;
		sqlite3_stmt * var_insert;
		sqlite3_stmt * status_insert;
		sqlite3_stmt * bonus_insert;
		/* const table */
		sqlite3_stmt * ea_const_insert;
		sqlite3_stmt * ra_const_insert;
		sqlite3_stmt * he_const_insert;
	};

	struct lt_db_t * init_db(const char *);
	void load_ea_item(struct lt_db_t *, ea_item_t *, int);
	void load_ra_item(struct lt_db_t *, ra_item_t *, int);
	void load_pet(struct lt_db_t *, sqlite3_stmt *, pet_t *, int);
	void load_merc(struct lt_db_t *, sqlite3_stmt *, merc_t *, int);
	void load_prod(struct lt_db_t *, sqlite3_stmt *, produce_t *, int);
	void ra_load_prod(struct lt_db_t * sql, sqlite3_stmt * ins, ra_produce_t * db, int size);
	void ea_load_mob(struct lt_db_t * sql, sqlite3_stmt * ins, ea_mob_t * db, int size);
	void load_mob(struct lt_db_t * sql, sqlite3_stmt * ins, mob_t * db, int size);
	void load_skill(struct lt_db_t * sql, sqlite3_stmt * ins, skill_t * db, int size);
	void ra_load_skill(struct lt_db_t * sql, sqlite3_stmt * ins, ra_skill_t * db, int size);
	void load_block(struct lt_db_t * sql, sqlite3_stmt * ins, block_t * db, int size);
	void load_var(struct lt_db_t * sql, sqlite3_stmt * ins, var_t * db, int size);
	void load_status(struct lt_db_t * sql, sqlite3_stmt * ins, status_t * db, int size);
	void load_bonus(struct lt_db_t * sql, sqlite3_stmt * ins, bonus_t * db, int size);
	void load_const(struct lt_db_t * sql, sqlite3_stmt * ins, const_t * db, int size);
	void deit_db(struct lt_db_t *);
	void trace_db(void *, const char *);
	char * array_to_string(char *, int *);
	char * array_to_string_cnt(char *, int *, int);
	int array_field_cnt(char *);
#endif
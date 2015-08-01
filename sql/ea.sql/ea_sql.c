#include "windows.h"
#include "Shlwapi.h"
#include <db_eathena.h>

#define DIR_SEP					"\\"
#define EA_DB_NAME				"ea.db"
#define ITEM_DB_PATH			"db"DIR_SEP"item_db.txt"
#define MOB_DB_PATH				"db"DIR_SEP"mob_db.txt"
#define SKILL_DB_PATH			"db"DIR_SEP"skill_db.txt"
#define PRODUCE_DB_PATH			"db"DIR_SEP"produce_db.txt"
#define MERCENARY_DB_PATH		"db"DIR_SEP"mercenary_db.txt"
#define PET_DB_PATH				"db"DIR_SEP"pet_db.txt"
#define CONST_PATH				"db"DIR_SEP"const.txt"
#define ITEM_GROUP_BLUEBOX 		"db"DIR_SEP"item_bluebox.txt"
#define ITEM_GROUP_VIOLETBOX 	"db"DIR_SEP"item_violetbox.txt"
#define ITEM_GROUP_CARDALBUM 	"db"DIR_SEP"item_cardalbum.txt"
#define ITEM_GROUP_COOKIEBAG 	"db"DIR_SEP"item_cookie_bag.txt"
#define ITEM_GROUP_FINDINGORE 	"db"DIR_SEP"item_findingore.txt"
#define ITEM_GROUP_GIFTBOX 		"db"DIR_SEP"item_giftbox.txt"
#define ITEM_GROUP_SCROLL 		"db"DIR_SEP"item_scroll.txt"
#define ITEM_GROUP_MISC 		"db"DIR_SEP"item_misc.txt"

int main(int argc, char * argv[]) {
	LPSTR out = NULL;
	LPSTR path = NULL;
	CHAR db_path[MAX_PATH];
	ea_db_t * ea = NULL;

	if (1 >= argc || getopts(argc, argv) || !(path = opt['p' % 26])) {
		fprintf(stderr, "ra.sql.exe -p <rathena path> -o <output path>\n");
		exit(EXIT_FAILURE);
	}

	out = opt['o' % 26];
	if (NULL != out) {
		if (!SetCurrentDirectory(out)) {
			fprintf(stderr, "ra.sql: invalid output path '%s'.\n", out);
			exit(EXIT_FAILURE);
		}
	}

	if(	ea_db_init(&ea, EA_DB_NAME) ||
		NULL == PathCombine(db_path, path, ITEM_DB_PATH) ||
		ea_db_item_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, MOB_DB_PATH) ||
		ea_db_mob_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, SKILL_DB_PATH) ||
		ea_db_skill_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, PRODUCE_DB_PATH) ||
		ea_db_produce_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, MERCENARY_DB_PATH) ||
		ea_db_merc_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, PET_DB_PATH) ||
		ea_db_pet_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_BLUEBOX) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_VIOLETBOX) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_CARDALBUM) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_COOKIEBAG) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_FINDINGORE) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_GIFTBOX) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_SCROLL) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GROUP_MISC) ||
		ea_db_item_group_load(ea, db_path) ||
		NULL == PathCombine(db_path, path, CONST_PATH) ||
		ea_db_const_load(ea, db_path)) {
		ea_db_deit(&ea);
		exit(EXIT_FAILURE);
	}

	ea_db_deit(&ea);
	exit(EXIT_SUCCESS);
}
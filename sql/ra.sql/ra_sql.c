#include "windows.h"
#include "Shlwapi.h"
#include "db_rathena.h"

#define DIR_SEP					"\\"
#define RA_DB_NAME				"ra.db"
#define ITEM_DB_PATH			"db"DIR_SEP"re"DIR_SEP"item_db.txt"
#define MOB_DB_PATH				"db"DIR_SEP"re"DIR_SEP"mob_db.txt"
#define SKILL_DB_PATH			"db"DIR_SEP"re"DIR_SEP"skill_db.txt"
#define PRODUCE_DB_PATH			"db"DIR_SEP"re"DIR_SEP"produce_db.txt"
#define MERCENARY_DB_PATH		"db"DIR_SEP"mercenary_db.txt"
#define PET_DB_PATH				"db"DIR_SEP"re"DIR_SEP"pet_db.txt"
#define CONST_PATH				"db"DIR_SEP"const.txt"
#define ITEM_BLUEBOX_PATH		"db"DIR_SEP"re"DIR_SEP"item_bluebox.txt"
#define ITEM_CARDALBUM_PATH		"db"DIR_SEP"re"DIR_SEP"item_cardalbum.txt"
#define ITEM_FINDIGNORE_PATH	"db"DIR_SEP"item_findingore.txt"
#define ITEM_GIFTBOX_PATH		"db"DIR_SEP"re"DIR_SEP"item_giftbox.txt"
#define ITEM_MISC_PATH			"db"DIR_SEP"re"DIR_SEP"item_misc.txt"
#define ITEM_VIOLETBOX_PATH		"db"DIR_SEP"re"DIR_SEP"item_violetbox.txt"
#define ITEM_PACKAGE_PATH		"db"DIR_SEP"re"DIR_SEP"item_package.txt"
#define ITEM_COMBO_PATH			"db"DIR_SEP"re"DIR_SEP"item_combo_db.txt"

int main(int argc, char * argv[]) {
	LPSTR out = NULL;
	LPSTR path = NULL;
	CHAR db_path[MAX_PATH];
	ra_db_t * ra = NULL;

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

	if(	ra_db_init(&ra, RA_DB_NAME) ||
		NULL == PathCombine(db_path, path, ITEM_DB_PATH) ||
		ra_db_item_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, MOB_DB_PATH) ||
		ra_db_mob_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, SKILL_DB_PATH) ||
		ra_db_skill_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, PRODUCE_DB_PATH) ||
		ra_db_produce_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, MERCENARY_DB_PATH) ||
		ra_db_merc_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, PET_DB_PATH) ||
		ra_db_pet_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, CONST_PATH) ||
		ra_db_const_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_BLUEBOX_PATH) ||
		ra_db_item_group_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_CARDALBUM_PATH) ||
		ra_db_item_group_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_FINDIGNORE_PATH) ||
		ra_db_item_group_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_GIFTBOX_PATH) ||
		ra_db_item_group_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_MISC_PATH) ||
		ra_db_item_group_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_VIOLETBOX_PATH) ||
		ra_db_item_group_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_PACKAGE_PATH) ||
		ra_db_item_pacakage_load(ra, db_path) ||
		NULL == PathCombine(db_path, path, ITEM_COMBO_PATH) ||
		ra_db_item_combo_load(ra, db_path)) {
		ra_db_deit(&ra);
		exit(EXIT_FAILURE);
	}
	ra_db_deit(&ra);
	exit(EXIT_SUCCESS);
}
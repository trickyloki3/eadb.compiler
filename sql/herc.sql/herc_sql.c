#include "windows.h"
#include "Shlwapi.h"
#include "db_hercules.h"

#define DIR_SEP				"\\"
#define HERCULES_DB_NAME	"herc.db"
#define ITEM_DB_PATH		"db"DIR_SEP"re"DIR_SEP"item_db.conf"
#define MOB_DB_PATH			"db"DIR_SEP"re"DIR_SEP"mob_db.txt"
#define SKILL_DB_PATH		"db"DIR_SEP"re"DIR_SEP"skill_db.txt"
#define PRODUCE_DB_PATH		"db"DIR_SEP"produce_db.txt"
#define MERCENARY_DB_PATH	"db"DIR_SEP"mercenary_db.txt"
#define PET_DB_PATH			"db"DIR_SEP"pet_db.txt"
#define CONST_PATH			"db"DIR_SEP"const.txt"
#define ITEM_COMBO_PATH		"db"DIR_SEP"re"DIR_SEP"item_combo_db.txt"
#define ITEM_GROUP_PATH		"db"DIR_SEP"re"DIR_SEP"item_group.conf"
#define ITEM_CHAIN_PATH		"db"DIR_SEP"re"DIR_SEP"item_chain.conf"

int GetErrorMessage() {
	LPSTR szError = NULL;
	DWORD dwError = GetLastError();
	DWORD dwFlag = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
	DWORD dwLang = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
	if (FormatMessage(dwFlag, NULL, dwError, dwLang, &szError, 0, NULL)) {
		fprintf(stderr, "herc.sql: %s\n", szError);
		LocalFree(szError);
	}
	return CHECK_PASSED;
}

int main(int argc, char * argv[]) {
	LPSTR out = NULL;
	LPSTR path = NULL;
	CHAR db_path[MAX_PATH];
	herc_db_t * herc = NULL;

	/* get herc server path */
	if (1 >= argc || getopts(argc, argv) || !(path = opt['p' % 26])) {
		fprintf(stderr, "herc.sql.exe -p <hercule path> -o <output path>\n");
		exit(EXIT_FAILURE);
	}

	/* change herc.db output directory */
	out = opt['o' % 26];
	if (NULL != out) {
		if (!SetCurrentDirectory(out)) {
			fprintf(stderr, "herc.sql: invalid output path '%s'.\n", out);
			GetErrorMessage();
		}
	}

	/* load the herc db */
	herc_db_init(&herc, HERCULES_DB_NAME);
	/*PathCombine(db_path, path, ITEM_DB_PATH);
	herc_load_item_db(herc, db_path);
	PathCombine(db_path, path, MOB_DB_PATH);
	herc_load_mob_db(herc, db_path);
	PathCombine(db_path, path, SKILL_DB_PATH);
	herc_load_skill_db(herc, db_path);
	PathCombine(db_path, path, PRODUCE_DB_PATH);
	herc_load_produce_db(herc, db_path);
	PathCombine(db_path, path, MERCENARY_DB_PATH);
	herc_load_merc_db(herc, db_path);
	PathCombine(db_path, path, PET_DB_PATH);
	herc_load_pet_db(herc, db_path);
	PathCombine(db_path, path, CONST_PATH);
	herc_load_const_db(herc, db_path);
	PathCombine(db_path, path, ITEM_COMBO_PATH);
	herc_load_combo_db(herc, db_path);
	PathCombine(db_path, path, ITEM_GROUP_PATH);
	herc_load_item_group_db(herc, db_path);*/
	PathCombine(db_path, path, ITEM_CHAIN_PATH);
	herc_load_item_chain_db(herc, db_path);
	herc_db_deit(&herc);
    return 0;
}
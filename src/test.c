#include "script.h"
int main(int argc, char * argv[]) {
	struct ic_db_t * global_db = init_ic_db("athena.db");
	pet_t pet;
	merc_t merc;
	ic_mob_t mob;
	bonus_t bonus;
	memset(&mob, 0, sizeof(ic_mob_t));
	memset(&merc, 0, sizeof(merc_t));
	memset(&pet, 0, sizeof(pet_t));
	memset(&bonus, 0, sizeof(bonus_t));
	/*var_t var_info;
	const_t const_info;
	ic_skill_t skill_info;
	ic_item_t item_info;
	memset(&var_info, 0, sizeof(var_t));
	memset(&const_info, 0, sizeof(const_t));
	memset(&skill_info, 0, sizeof(ic_skill_t));
	memset(&item_info, 0, sizeof(ic_item_t));
	var_keyword_search(global_db, &var_info, "getrefine");
	const_keyword_search(global_db, &const_info, "Job_Acolyte", RATHENA);
	skill_name_search(global_db, &skill_info, "MG_LIGHTNINGBOLT", RATHENA);
	skill_name_search_id(global_db, &skill_info, 20, RATHENA);
	item_name_search(global_db, &item_info, "Orange Potion", RATHENA);
	item_name_id_search(global_db, &item_info, 502, RATHENA);*/
	mob_id_search(global_db, &mob, 1001, RATHENA);
	merc_id_search(global_db, &merc, 1191, RATHENA);
	pet_id_search(global_db, &pet, 1002, RATHENA);
	bonus_name_search(global_db, &bonus, "bonus2", "bWeaponAtk", RATHENA);
	printf("%d;%s\n", pet.mob_id, pet.pet_name);
	deit_ic_db(global_db);
	return 0;
}
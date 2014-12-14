#include "db.h"

int main(int argc, char * argv[]) {
	int i = 0;
	int j = 0;
	int id = 0;
	FILE * file = fopen("item_bonus.txt", "w");
	load_db_t * db1 = load("res/item_bonus.txt", trim_alpha, load_general, bonus_load);
	bonus_t * bonus = NULL;
	bonus_t * bonus_db = NULL;

	exit_null("db1 is null", 1, db1);
	exit_null("file is null", 1, file);
	bonus_db = db1->db;
	for(i = 0; i < db1->size; i++, id++) {
		bonus = &bonus_db[i];
		fprintf(file,"%d,%s,%s,%d,\"%s\",%d", id, bonus->pref, bonus->buff, bonus->attr, bonus->desc, bonus->type_cnt);
		for(j = 0; j < bonus->type_cnt; j++) fprintf(file,",%c",bonus->type[j]);
		fprintf(file,",%d", bonus->order_cnt);
		for(j = 0; j < bonus->order_cnt; j++) fprintf(file,",%d",bonus->order[j]);
		fprintf(file,"\n");
	}

	db1->dealloc(db1->db, db1->size);
	free(db1);
	fclose(file);
}
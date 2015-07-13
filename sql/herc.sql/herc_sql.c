#include "db_hercules.h"

int main(int argc, char * argv[]) {
    herc_db_t * herc = NULL;
    herc_db_init(&herc, "herc.db");
	herc_load_item_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\re\\item_db.conf");
    herc_load_mob_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\re\\mob_db.txt");
    herc_load_skill_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\re\\skill_db.txt");
    herc_load_produce_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\produce_db.txt");
    herc_load_merc_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\mercenary_db.txt");
    herc_load_pet_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\pet_db.txt");
    herc_load_const_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\const.txt");
    herc_load_combo_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\re\\item_combo_db.txt");
    herc_db_deit(&herc);
    return 0;
}
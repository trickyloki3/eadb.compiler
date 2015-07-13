#include "db_hercules.h"

int main(int argc, char * argv[]) {
    herc_db_t * herc = NULL;
    herc_db_init(&herc, "herc.db");
	herc_load_item_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\re\\item_db.conf");
    herc_load_mob_db(herc, "C:\\Users\\jim\\Desktop\\git\\Hercules\\db\\re\\mob_db.txt");
    herc_db_deit(&herc);
    return 0;
}
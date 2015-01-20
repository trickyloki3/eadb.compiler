/*
 *   file: conv.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "api.h"
#include "db.h"
#include "load.h"
#include "util.h"
#include "script.h"

int main(int argc, char * argv[]) {
	if(argc >= 2) {
		if(ncs_strcmp("all",argv[1]) != 0)
			global_mode = (ncs_strcmp(argv[1],"rathena") == 0) ? RATHENA : 
						  (((ncs_strcmp(argv[1],"eathena") == 0) ? EATHENA : 
						  ((ncs_strcmp(argv[1],"hercules") == 0) ? HERCULES : -1)));
		else
			global_mode = 0x7;

		if(global_mode != RATHENA && global_mode != EATHENA && global_mode != HERCULES && global_mode != 0x7)
			exit_abt("invalid database type; only 'eathena', 'rathena', 'hercules', or 'all' is supported.");

		struct lt_db_t * db = init_db("athena.db", INITIALIZE_DB);
		if(global_mode & EATHENA) {
				load_db_t * db5 = load("../eadb/mob_db.txt", trim_numeric, load_general, ea_mob_load);
				load_db_t * db8 = load("../eadb/skill_db.txt", trim_numeric, load_general, skill_load);
				load_db_t * db11 = load("../eadb/produce_db.txt", trim_numeric, load_general, produce_load);
				load_db_t * db14 = load("../eadb/mercenary_db.txt", trim_numeric, load_general, merc_load);
				load_db_t * db17 = load("../eadb/pet_db.txt", trim_numeric, load_general, pet_load);
				load_db_t * db20 = load("../eadb/item_db.txt", trim_numeric, load_general, ea_item_load);
				load_db_t * db22 = load("../eadb/const.txt", trim_general, load_general, const_load);
				load_db_t * db25 = load("../eadb/item_group/item_bluebox.txt", trim_numeric, load_general, ea_item_group_load);
				load_db_t * db26 = load("../eadb/item_group/item_cardalbum.txt", trim_numeric, load_general, ea_item_group_load);
				load_db_t * db27 = load("../eadb/item_group/item_cookie_bag.txt", trim_numeric, load_general, ea_item_group_load);
				load_db_t * db28 = load("../eadb/item_group/item_findingore.txt", trim_numeric, load_general, ea_item_group_load);
				load_db_t * db29 = load("../eadb/item_group/item_giftbox.txt", trim_numeric, load_general, ea_item_group_load);
				load_db_t * db30 = load("../eadb/item_group/item_misc.txt", trim_numeric, load_general, ea_item_group_load);
				load_db_t * db31 = load("../eadb/item_group/item_scroll.txt", trim_numeric, load_general, ea_item_group_load);
				load_db_t * db32 = load("../eadb/item_group/item_violetbox.txt", trim_numeric, load_general, ea_item_group_load);
				ea_load_mob(db, db->ea_mob_insert, db5->db, db5->size);
				load_skill(db, db->ea_skill_insert, db8->db, db8->size);
				load_prod(db, db->ea_prod_insert, db11->db, db11->size);
				load_merc(db, db->ea_merc_insert, db14->db, db14->size);
				load_pet(db, db->ea_pet_insert, db17->db, db17->size);
				load_ea_item(db, db20->db, db20->size);
				load_const(db, db->ea_const_insert, db22->db, db22->size);
				load_ea_item_group(db, db->ea_item_group_insert, db25->db, db25->size);
				load_ea_item_group(db, db->ea_item_group_insert, db26->db, db26->size);
				load_ea_item_group(db, db->ea_item_group_insert, db27->db, db27->size);
				load_ea_item_group(db, db->ea_item_group_insert, db28->db, db28->size);
				load_ea_item_group(db, db->ea_item_group_insert, db29->db, db29->size);
				load_ea_item_group(db, db->ea_item_group_insert, db30->db, db30->size);
				load_ea_item_group(db, db->ea_item_group_insert, db31->db, db31->size);
				load_ea_item_group(db, db->ea_item_group_insert, db32->db, db32->size);
				db5->dealloc(db5->db, db5->size);
				db8->dealloc(db8->db, db8->size);
				db11->dealloc(db11->db, db11->size);
				db14->dealloc(db14->db, db14->size);
				db17->dealloc(db17->db, db17->size);
				db20->dealloc(db20->db, db20->size);
				db22->dealloc(db22->db, db22->size);
				db25->dealloc(db25->db, db25->size);
				db26->dealloc(db26->db, db26->size);
				db27->dealloc(db27->db, db27->size);
				db28->dealloc(db28->db, db28->size);
				db29->dealloc(db29->db, db29->size);
				db30->dealloc(db30->db, db30->size);
				db31->dealloc(db31->db, db31->size);
				db32->dealloc(db32->db, db32->size);
				free(db5);
				free(db8);
				free(db11);
				free(db14);
				free(db17);
				free(db20);
				free(db22);
				free(db25);
				free(db26);
				free(db27);
				free(db28);
				free(db29);
				free(db30);
				free(db31);
				free(db32);
		}
		if(global_mode & RATHENA) {
			load_db_t * db6 = load("../radb/mob_db.txt", trim_numeric, load_general, mob_load);
			load_db_t * db9 = load("../radb/skill_db.txt", trim_numeric, load_general, ra_skill_load);
			load_db_t * db12 = load("../radb/produce_db.txt", trim_numeric, load_general, ra_produce_load);
			load_db_t * db15 = load("../radb/mercenary_db.txt", trim_numeric, load_general, merc_load);
			load_db_t * db18 = load("../radb/pet_db.txt", trim_numeric, load_general, pet_load);
			load_db_t * db21 = load("../radb/item_db.txt", trim_numeric, load_general, ra_item_load);
			load_db_t * db23 = load("../radb/const.txt", trim_general, load_general, const_load);
			load_db_t * db33 = load("../radb/item_group/item_bluebox.txt", trim_alpha, load_general, ra_item_group_load);
			load_db_t * db34 = load("../radb/item_group/item_cardalbum.txt", trim_alpha, load_general, ra_item_group_load);
			load_db_t * db35 = load("../radb/item_group/item_findingore.txt", trim_alpha, load_general, ra_item_group_load);
			load_db_t * db36 = load("../radb/item_group/item_giftbox.txt", trim_alpha, load_general, ra_item_group_load);
			load_db_t * db37 = load("../radb/item_group/item_violetbox.txt", trim_alpha, load_general, ra_item_group_load);
			load_db_t * db38 = load("../radb/item_group/item_misc.txt", trim_alpha, load_general, ra_item_group_load);
			load_db_t * db39 = load("../radb/item_group/item_package.txt", trim_alpha, load_general, ra_item_package_load);
			load_db_t * db40 = load("../radb/item_combo_db.txt", trim_numeric, load_general, ra_item_combo_load);
			load_mob(db, db->ra_mob_insert, db6->db, db6->size);
			ra_load_skill(db, db->ra_skill_insert, db9->db, db9->size);
			ra_load_prod(db, db->ra_prod_insert, db12->db, db12->size);
			load_merc(db, db->ra_merc_insert, db15->db, db15->size);
			load_pet(db, db->ra_pet_insert, db18->db, db18->size);
			load_ra_item(db, db21->db, db21->size);
			load_const(db, db->ra_const_insert, db23->db, db23->size);
			load_ra_item_group(db, db->ra_item_group_insert, db33->db, db33->size);
			load_ra_item_group(db, db->ra_item_group_insert, db34->db, db34->size);
			load_ra_item_group(db, db->ra_item_group_insert, db35->db, db35->size);
			load_ra_item_group(db, db->ra_item_group_insert, db36->db, db36->size);
			load_ra_item_group(db, db->ra_item_group_insert, db37->db, db37->size);
			load_ra_item_group(db, db->ra_item_group_insert, db38->db, db38->size);
			load_ra_item_package(db, db->ra_item_group_insert, db39->db, db39->size);
			load_ra_item_combo(db, db40->db, db40->size);
			db6->dealloc(db6->db, db6->size);
			db9->dealloc(db9->db, db9->size);
			db12->dealloc(db12->db, db12->size);
			db15->dealloc(db15->db, db15->size);
			db18->dealloc(db18->db, db18->size);
			db21->dealloc(db21->db, db21->size);
			db23->dealloc(db23->db, db23->size);
			db33->dealloc(db33->db, db33->size);
			db34->dealloc(db34->db, db34->size);
			db35->dealloc(db35->db, db35->size);
			db36->dealloc(db36->db, db36->size);
			db37->dealloc(db37->db, db37->size);
			db38->dealloc(db38->db, db38->size);
			db39->dealloc(db39->db, db39->size);
			db40->dealloc(db40->db, db40->size);
			free(db6);
			free(db9);
			free(db12);
			free(db15);
			free(db18);
			free(db21);
			free(db23);
			free(db33);
			free(db34);
			free(db35);
			free(db36);
			free(db37);
			free(db38);
			free(db39);
			free(db40);
		}
		if(global_mode & HERCULES) {
			load_db_t * db7 = load("../hedb/mob_db.txt", trim_numeric, load_general, mob_load);
			load_db_t * db10 = load("../hedb/skill_db.txt", trim_numeric, load_general, skill_load);
			load_db_t * db13 = load("../hedb/produce_db.txt", trim_numeric, load_general, produce_load);
			load_db_t * db16 = load("../hedb/mercenary_db.txt", trim_numeric, load_general, merc_load);
			load_db_t * db19 = load("../hedb/pet_db.txt", trim_numeric, load_general, pet_load);
			load_db_t * db24 = load("../hedb/const.txt", trim_general, load_general, const_load);
			load_mob(db, db->he_mob_insert, db7->db, db7->size);
			load_skill(db, db->he_skill_insert, db10->db, db10->size);
			load_prod(db, db->he_prod_insert, db13->db, db13->size);
			load_merc(db, db->he_merc_insert, db16->db, db16->size);
			load_pet(db, db->he_pet_insert, db19->db, db19->size);
			load_const(db, db->he_const_insert, db24->db, db24->size);
			db7->dealloc(db7->db, db7->size);
			db10->dealloc(db10->db, db10->size);
			db13->dealloc(db13->db, db13->size);
			db16->dealloc(db16->db, db16->size);
			db19->dealloc(db19->db, db19->size);
			db24->dealloc(db24->db, db24->size);
			free(db7);
			free(db10);
			free(db13);
			free(db16);
			free(db19);
			free(db24);
		}

		/* load compiler databases */
		load_db_t * db1 = load("res/item_bonus.txt", trim_numeric, load_general, bonus_load);
		load_db_t * db2 = load("res/status_db.txt", trim_numeric, load_general, status_load);
		load_db_t * db3 = load("res/var_db.txt", trim_numeric, load_general, var_load);
		load_db_t * db4 = load("res/block_db.txt", trim_numeric, load_general, block_load);
		load_bonus(db, db->bonus_insert, db1->db, db1->size);
		load_status(db, db->status_insert, db2->db, db2->size);
		load_var(db, db->var_insert, db3->db, db3->size);
		load_block(db, db->block_insert, db4->db, db4->size);
		db1->dealloc(db1->db, db1->size);
		db2->dealloc(db2->db, db2->size);
		db3->dealloc(db3->db, db3->size);
		db4->dealloc(db4->db, db4->size);
		free(db1);
		free(db2);
		free(db3);
		free(db4);
		deit_db(db);
	} else {
		exit_buf("syntax '%s [eathena | rathena | hercules | all]'", argv[0]);
	}
	exit(EXIT_SUCCESS);
}

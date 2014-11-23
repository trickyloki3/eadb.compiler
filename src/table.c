/*=============================================================================
   file: table.c
   date: 5/11/2014
 update: 5/24/2014
   auth: trickyloki3
   desc: table management
=============================================================================*/
#include "table.h"

char * race_tbl(int i) {
	switch(i) {
		case 0: return "formless";
		case 1: return "undead";
		case 2: return "brute";
		case 3: return "plant";
		case 4: return "insect";
		case 5: return "fish";
		case 6: return "demon";
		case 7: return "demi-human";
		case 8: return "angel";
		case 9: return "dragon";
		case 10: return "player";
		case 11: return "non-mvp";
		case 12: return "non-demi-human";
		default: return "error";
	};
}

char * ele_tbl(int i) {
	switch(i) {
		case 0: return "neutral";
		case 1: return "water";
		case 2: return "earth";
		case 3: return "fire";
		case 4: return "wind";
		case 5: return "poison";
		case 6: return "holy";
		case 7: return "dark";
		case 8: return "ghost";
		case 9: return "undead";
		case 10: return "all elements";
		default: return "error";
	};
}

char * eff_tbl(int i) {
	switch(i) {
		case 0: return "stone";
		case 1: return "freeze";
		case 2: return "stun";
		case 3: return "sleep";
		case 4: return "poison";
		case 5: return "curse";
		case 6: return "silence";
		case 7: return "confusion";
		case 8: return "blind";
		case 9: return "bleeding";
		case 10: return "deadly Poison";
		case 11: return "fear";
		case 12: return "cold";
		case 13: return "burning";
		case 14: return "deep sleep";
		case 309: return "fear";
		case 310: return "burning";
		default: return "error";
	};
}

char * size_tbl(int i) {
	switch(i) {
		case 0: return "small";
		case 1: return "medium";
		case 2: return "large";
		case 3: return "all size";
		default: return "error";
	}
}

char * class_tbl(int i) {
	switch(i) {
		case 0: return "normal classes";
		case 1: return "boss classes";
		case 2: return "guardian classes";
		case 3: return "all classes";
		default: return "error";
	}
}

char * regen_tbl(int i) {
	switch(i) {
		case 1: return "HP"; break;
		case 2: return "SP"; break;
		default: return "Error"; break;
	};
}

char * spgd_tbl(int i) {
	switch(i) {
		case 0: return "heal SP"; break;
		case 1: return "drain SP"; break;
		default: return "Error"; break;
	};
}

char * castgt_tbl(int i) {
	switch(i) {
		case 0: return "on self"; break;
		case 1: return "on enemy"; break;
		case 2: return "on self and random skill level"; break;
		case 3: return "on enemey and random skill level"; break;
		default: return "Error"; break;
	};
}

char * flagtgt_tbl(int i) {
	switch(i) {
		case 1: return "on self";
		case 2: return "random skill level";
		case 3: return "on self and random skill level";
		default: return "Error";
	};
}

char * weapon_tbl(int i) {
	switch(i) {
		case 0: return "Fist";
		case 1: return "Dagger";
		case 2: return "One-handed Sword";
		case 3: return "Two-handed Sword";
		case 4: return "One-handed Spear";
		case 5: return "Two-handed Spear";
		case 6: return "One-handed Axe";
		case 7: return "Two-handed Axe";
		case 8: return "Mace";
		case 9: return "Unused";
		case 10: return "Stave";
		case 11: return "Bow";
		case 12: return "Knuckle";
		case 13: return "Muscial Instrument";
		case 14: return "Whip";
		case 15: return "Book";
		case 16: return "Katar";
		case 17: return "Revolver";
		case 18: return "Rifle";
		case 19: return "Gatling Gun";
		case 20: return "Shotgun";
		case 21: return "Grenade Launcher";
		case 22: return "Fuuma Shuriken";
		default: return "Error";
	};
}

char * itemgrp_tbl(int i) {
	switch(i) {
		case 1: return "blue box";
		case 2: return "violet box";
		case 3: return "card album";
		case 4: return "gift box";
		/* temporary name until final translation;
			possible retrieve the entire item group 
			and display the entire list, but this
			can get messy for some large item groups.

			self note:

			"Add %% chance of dropping items from X group.
			[X Item Group]
			XYZ item 1
			..
			XYZ item N
			""
		*/
		case 7: return "cookie bag";
		case 8: return "first aid box";
		case 9: return "herb's item's group";
		case 11: return "meat item's group";
		case 12: return "candy item's group";
		case 13: return "juice item's group";
		case 14: return "fish item's group";
		case 18: return "ore item's group";
		case 19: return "food item's group";
		case 20: return "recovery item's group";
		case 22: return "taming box";
		case 23: return "scroll box";
		case 24: return "quiver box";
		case 25: return "mask box";
		case 26: return "accessory box";
		case 27: return "jewelry item's group";
		case 28:
		case 29:
		case 30:
		case 31: return "gift box";
		case 32: return "egg boy box";
		case 33: return "egg girl box";
		case 34: return "china gift box";
		case 36: return "food bag";
		case 37: return "potion item's group";
		case 38: return "red box 2";
		case 39: return "blue Box";
		case 40: return "red box";
		case 41: return "green box";
		case 42: return "yellow box";
		case 43: return "old gift box";
		case 44: return "magic card album";
		case 45: return "home town gift box";
		case 46: return "masquerade box";
		case 48: return "masquerade box 2";
		case 49: return "easter scroll box";
		case 50: return "Pierre treasure box";
		case 51: return "cherish box";
		case 52: return "cherish box oridecon";
		case 53: return "Louise costume box";
		case 54: return "Xmas Gift";
		case 55: return "Fruit Basket";
		case 61: return "advance weapon box";
		case 63: return "armor card album";
		case 64: return "helm card album";
		case 65: return "accessory card album";
		case 66: return "shoes card album";
		case 67: return "shield card album";
		case 68: return "weapon card album";
		case 69: return "garment card album";
		case 70: return "flamel's card";				/* drops a card!? */
		case 77: return "crumpled paper";
		case 81: return "fortune cookie 1 box";
		case 84: return "new gift envelope";
		case 389: return "candy holder";
		case 390: return "lucky bag";
		case 391: return "holy egg";
		case 397: return "Lotto Box 1";
		case 398: return "Lotto Box 2";
		case 399: return "Lotto Box 3";
		case 400: return "Lotto Box 4";
		case 401: return "Lotto Box 5";
		default: return "error";
	}
}

char * job_tbl(int i) {
	switch(i) {
		case 0: return "Novice";
		case 1: return "Swordsman";
		case 2: return "Mage";
		case 3: return "Archer";
		case 4: return "Acolyte";
		case 5: return "Merchant";
		case 6: return "Thief";
		case 7: return "Knight";
		case 8: return "Priest";
		case 9: return "Wizard";
		case 10: return "Blacksmith";
		case 11: return "Hunter";
		case 12: return "Assassin";
		case 13: return "Knight";
		case 14: return "Crusader";
		case 15: return "Monk";
		case 16: return "Sage";
		case 17: return "Rogue";
		case 18: return "Alchemist";
		case 19: return "Bard";
		case 20: return "Dacer";
		case 21: return "Crusader";
		case 22: return "Wedding";
		case 23: return "Super Novice";
		case 24: return "Gunslinger";
		case 25: return "Ninja";
		case 26: return "Xmas";
		case 27: return "Summer";
		case 4001: return "High Novice";
		case 4002: return "High Swordsman";
		case 4003: return "High Mage";
		case 4004: return "High Archer";
		case 4005: return "High Acolyte";
		case 4006: return "High Merchant";
		case 4007: return "High Thief";
		case 4008: return "Lord Knight";
		case 4009: return "High Priest";
		case 4010: return "High Wizard";
		case 4011: return "Whitesmith";
		case 4012: return "Sniper";
		case 4013: return "Assassin Cross";
		case 4014: return "Lord Knight";
		case 4015: return "Paldin";
		case 4016: return "Champion";
		case 4017: return "Professor";
		case 4018: return "Stalker";
		case 4019: return "Creator";
		case 4020: return "Clown";
		case 4021: return "Gypsy";
		case 4022: return "Paladin";
		case 4023: return "Baby Novice";
		case 4024: return "Baby Swordsman";
		case 4025: return "Baby Mage";
		case 4026: return "Baby Archer";
		case 4027: return "Baby Acolyte";
		case 4028: return "Baby Merchant";
		case 4029: return "Baby Thief";
		case 4030: return "Baby Knight";	
		case 4031: return "Baby Priest";
		case 4032: return "Baby Wizard";
		case 4033: return "Baby Blacksmith";
		case 4034: return "Baby Hunter";
		case 4035: return "Baby Assassin";
		case 4036: return "Baby Knight";
		case 4037: return "Baby Crusader";
		case 4038: return "Baby Monk";
		case 4039: return "Baby Sage";
		case 4040: return "Baby Rogue";
		case 4041: return "Baby Alchemist";
		case 4042: return "Baby Bard";
		case 4043: return "Baby Dancer";
		case 4044: return "Baby Crusader";
		case 4045: return "Baby Super Novice";
		case 4046: return "Taekwon";
		case 4047: return "Star Gladiator";
		case 4048: return "Star Gladiator";
		case 4049: return "Soul Linker";
		case 4050: return "Gangsi";
		case 4051: return "Death Knight";
		case 4052: return "Dark Collector";
		case 4054: return "Rune Knight";
		case 4055: return "Warlock";
		case 4056: return "Ranger";
		case 4057: return "Arch Bishop";
		case 4058: return "Mechanic";
		case 4059: return "Guillotine Cross";
		case 4060: return "Transcendent Rune Knight";
		case 4061: return "Transcendent Warlock";
		case 4062: return "Transcendent Ranger";
		case 4063: return "Transcendent Arch Bishop";
		case 4064: return "Transcendent Mechanic";
		case 4065: return "Transcendent Guillotine Cross";
		case 4066: return "Royal Guard";
		case 4067: return "Sorcerer";
		case 4068: return "Minstrel";
		case 4069: return "Wanderer";
		case 4070: return "Sura";
		case 4071: return "Genetic";
		case 4072: return "Shadow Chaser";
		case 4073: return "Transcendent Royal Guard";
		case 4074: return "Transcendent Sorcerer";
		case 4075: return "Transcendent Minstrel";
		case 4076: return "Transcendent Wanderer";
		case 4077: return "Transcendent Sura";
		case 4078: return "Transcendent Genetic";
		case 4079: return "Transcendent Shadow Chaser";
		case 4080: return "Rune Knight";
		case 4081: return "Transcendent Rune Knight";
		case 4082: return "Royal Guard";
		case 4083: return "Transcendent Royal Guard";
		case 4084: return "Ranger";
		case 4085: return "Transcendent Ranger";
		case 4086: return "Mechanic";
		case 4087: return "Transcendent Mechanic";
		case 4096: return "Baby Rune Knight";
		case 4097: return "Baby Warlock";
		case 4098: return "Baby Ranger";
		case 4099: return "Baby Bishop";
		case 4100: return "Baby Mechanic";
		case 4101: return "Baby Guillotine Cross";
		case 4102: return "Baby RoyalGuard";
		case 4103: return "Baby Sorcerer";
		case 4104: return "Baby Minstrel";
		case 4105: return "Baby Wanderer";
		case 4106: return "Baby Sura";
		case 4107: return "Baby Genetic";
		case 4108: return "Baby Chaser";
		case 4109: return "Baby Rune Knight";
		case 4110: return "Baby Royal Guard";
		case 4111: return "Baby Ranger";
		case 4112: return "Baby Mechanic";
		case 4190: return "Baby Super Novice S";
		case 4191: return "Baby Novice S";
		case 4211: return "Kagerou";
		case 4212: return "Oboro";
		default: return "Error";
	};
}

/* UNUSED */

char * readparam_number(char * param) {
	if(ncs_strcmp(param,"bStr") == 0)
		return "STR";
	else if(ncs_strcmp(param,"bAgi") == 0)
		return "AGI";
	else if(ncs_strcmp(param,"bDex") == 0)
		return "DEX";
	else if(ncs_strcmp(param,"bVit") == 0)
		return "VIT";
	else if(ncs_strcmp(param,"bInt") == 0)
		return "INT";
	else if(ncs_strcmp(param,"bLuk") == 0)
		return "LUK";
	else if(ncs_strcmp(param,"StatusPoint") == 0)
		return "Status Point";
	else if(ncs_strcmp(param,"BaseLevel") == 0)
		return "Base Level";
	else if(ncs_strcmp(param,"SkillPoint") == 0)
		return "Skill Point";
	else if(ncs_strcmp(param,"Class") == 0)
		return "Class";
	else if(ncs_strcmp(param,"Upper") == 0)
		return "Upper";
	else if(ncs_strcmp(param,"Zeny") == 0)
		return "Zeny";
	else if(ncs_strcmp(param,"Sex") == 0)
		return "Sex";
	else if(ncs_strcmp(param,"Weight") == 0)
		return "Weight";
	else if(ncs_strcmp(param,"MaxWeight") == 0)
		return "Max Weight";
	else if(ncs_strcmp(param,"JobLevel") == 0)
		return "Job Level";
	else if(ncs_strcmp(param,"BaseExp") == 0)
		return "Base Experience";
	else if(ncs_strcmp(param,"JobExp") == 0)
		return "Job Experience";
	else if(ncs_strcmp(param,"NextBaseExp") == 0)
		return "Next Base Experience";
	else if(ncs_strcmp(param,"NextJobExp") == 0)
		return "Next Job Experience";
	else if(ncs_strcmp(param,"Hp") == 0)
		return "HP";
	else if(ncs_strcmp(param,"MaxHp") == 0)
		return "Max HP";
	else if(ncs_strcmp(param,"Sp") == 0)
		return "SP";
	else if(ncs_strcmp(param,"MaxSp") == 0)
		return "Max SP";
	else if(ncs_strcmp(param,"BaseJob") == 0)
		return "Base Job";
	else if(ncs_strcmp(param,"Karma") == 0)
		return "Karma";
	else if(ncs_strcmp(param,"Manner") == 0)
		return "Manner";
	else
		return "error";
}

/*char * getskilllv_const(char * param, skill_t * db, int size) {
	int skill_n = bsearch_ncs_str(db, size, param, skilldb_name, skilldb_getstr);
	if(skill_n)
		return db[skill_n].desc;
	else
		return "error";
}*/

char * getiteminfo_type(int type) {
	switch(type) {
		case 0: return "Buy Price";
		case 1: return "Sell Price";
		case 2: return "Type";
		case 3: return "Drop Chance";
		case 4: return "Sex";
		case 5: return "Equip";
		case 6: return "Weight";
		case 7: return "Attack";
		case 8: return "Defense";
		case 9: return "Range";
		case 10: return "Slot";
		case 11: return "Weapon Type";
		case 12: return "Require Level";
		case 13: return "Weapon Level";
		case 14: return "Type";
		default: return "error";
	}
}

/* getequipid must be paired with getitem info in item ID, otherwise arbitrary item ID */
char * getiteminfo_itemid(char ** arg, int start, int end) {
	char * param = arg[start+1];
	if(ncs_strcmp(param,"EQI_HEAD_TOP") == 0)
		return "Upper Headgear";
	else if(ncs_strcmp(param,"EQI_ARMOR") == 0)
		return "Armor";
	else if(ncs_strcmp(param,"EQI_HAND_L") == 0)
		return "Left Hand";
	else if(ncs_strcmp(param,"EQI_HAND_R") == 0)
		return "Right Hand";
	else if(ncs_strcmp(param,"EQI_GARMENT") == 0)
		return "Garment";
	else if(ncs_strcmp(param,"EQI_SHOES") == 0)
		return "Shoe";
	else if(ncs_strcmp(param,"EQI_ACC_L") == 0)
		return "Left Accessory";
	else if(ncs_strcmp(param,"EQI_ACC_R") == 0)
		return "Right Accessory";
	else if(ncs_strcmp(param,"EQI_HEAD_MID") == 0)
		return "Middle Headgear";
	else if(ncs_strcmp(param,"EQI_HEAD_LOW") == 0)
		return "Lower Headgear";
	else if(ncs_strcmp(param,"EQI_COSTUME_HEAD_LOW") == 0)
		return "Lower Costume Headgear";
	else if(ncs_strcmp(param,"EQI_COSTUME_HEAD_MID") == 0)
		return "Middle Costume Headgear";
	else if(ncs_strcmp(param,"EQI_COSTUME_HEAD_TOP") == 0)
		return "Upper Costume Headgear";
	else if(ncs_strcmp(param,"EQI_COSTUME_GARMENT") == 0)
		return "Costume Garment";
	else 
		return "error";
}

char * gettime_type(int type) {
	switch(type) {
		case 1: return "Second";
		case 2: return "Minute";
		case 3: return "Hour";
		case 4: return "Week";
		case 5: return "Day of Month";
		case 6: return "Month";
		case 7: return "Year";
		case 8: return "Day of Year";
		default: return "error";
	}
}

char * strcharinfo_type(int type) {
	switch(type) {
		case 0: return "Character Name";
		case 1: return "Party Name";
		case 2: return "Guild Name";
		case 3: return "Map";
		default: return "error";
	}
}

/*char * checkoption_type(char * type, const_t * db, int size) {
	int const_n = bsearch_ncs_str(db, size, type, constdb_name, constdb_getstr);
	const_n = db[const_n].value;
	switch(const_n) {
		case 0x1: return "Sight";
		case 0x2: return "Hide";
		case 0x4: return "Cloak";
		case 0x8: return "Cart 1";
		case 0x10: return "Falcon";
		case 0x20: return "Peco Peco";
		case 0x40: return "GM Hide";
		case 0x80: return "Cart 2";
		case 0x100: return "Cart 3";
		case 0x200: return "Cart 4";
		case 0x400: return "Cart 5";
		case 0x800: return "Orc Head";
		case 0x1000: return "Wedding Clothing";
		case 0x2000: return "Ruwach";
		case 0x4000: return "Chasewalk";
		case 0x8000: return "Santa Suit";
		case 0x10000: return "Sighttrasher";
		case 0x100000: return "Warg";
		case 0x200000: return "Riding Warg";
		default: return "error";
	}
}*/

char * retrieve_mapname(char * map) {
	if(ncs_strcmp(map,"job3_arch02") == 0)
		return "Odin Temple";
	else if(ncs_strcmp(map,"job3_rang02") == 0)
		return "Ranger Job Change Room";
	else if(ncs_strcmp(map,"job3_war02") == 0)
		return "Warlock Job Change Room";
	else if(ncs_strcmp(map,"job3_rune02") == 0)
		return "Rune Knight Job Change Room";
	else
		return "error";
}

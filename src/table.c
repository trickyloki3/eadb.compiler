/*
 *   file: table.c
 *   date: 12/9/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
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
		default: return "error"; break;
	};
}

char * spgd_tbl(int i) {
	switch(i) {
		case 0: return "heal SP"; break;
		case 1: return "drain SP"; break;
		default: return "error"; break;
	};
}

char * castgt_tbl(int i) {
	switch(i) {
		case 0: return "on self"; break;
		case 1: return "on enemy"; break;
		case 2: return "on self and random skill level"; break;
		case 3: return "on enemey and random skill level"; break;
		default: return "error"; break;
	};
}

char * flagtgt_tbl(int i) {
	switch(i) {
		case 1: return "on self";
		case 2: return "random skill level";
		case 3: return "on self and random skill level";
		default: return "error";
	};
}

char * item_type_tbl(int i) {
	switch(i) {
		case 0: return "Healing Item";
		case 2: return "Usable Item";
		case 3: return "Etc Item";
		case 4: return "Weapon";
		case 5: return "Armor";
		case 6: return "Card";
		case 7: return "Pet Egg";
		case 8: return "Pet Equipment";
		case 10: return "Ammo";
		case 11: return "Delayed Usable";
		case 12: return "Shadow Equipment";
		case 18: return "Delayed Usable";
		default: return "error";
	}
}

/* skip flag checking; select only one */
char * loc_tbl(int i) {
	switch(i) {
		case 0x000100: return "Upper Headgear";
		case 0x000200: return "Middle Headgear";
		case 0x000001: return "Lower Headgear";
		case 0x000010: return "Armor";
		case 0x000002: return "Weapon";
		case 0x000020: return "Shield";
		case 0x000004: return "Garment";
		case 0x000040: return "Shoes";
		case 0x000008: return "Right Accessory";
		case 0x000080: return "Left Accessory";
		case 0x000400: return "Costume Upper Headgear";
		case 0x000800: return "Costume Middle Headgear";
		case 0x001000: return "Costume Lower Headgear";
		case 0x002000: return "Costume Garment";
		case 0x008000: return "Ammo";
		case 0x010000: return "Shadow Armor";
		case 0x020000: return "Shadow Weapon";
		case 0x040000: return "Shadow Shield";
		case 0x080000: return "Shadow Shoes";
		case 0x100000: return "Shadow Right Accessory";
		case 0x200000: return "Shadow Left Accessory";
		default: return "error";
	}
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
		/* added from rathena */
		case 23: return "Two-Handed Staves";
		case 25: return "Dual-Wield Daggers";
		case 26: return "Dual-Wield Swords";
		case 27: return "Dual-Wield Axes";
		case 28: return "Dagger & Sword";
		case 29: return "Dagger & Axe";
		case 30: return "Sword & Axe";
		default: return "error";
	}
}

char * ammo_tbl(int i) {
	switch(i) {
		case 1: return "Arrows";
		case 2: return "Throwable Daggers";
		case 3: return "Bullets";
		case 4: return "Shells";
		case 5: return "Grenades";
		case 6: return "Shuriken";
		case 7: return "Kunai";
		case 8: return "Cannonballs";
		case 9: return "Throwable Items";
		default: return "error";
	}
}

char * itemgrp_tbl(int i) {
	switch(i) {
		case 1: return "blue box";
		case 2: return "violet box";
		case 3: return "card album";
		case 4: return "gift box";
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
		default: return "error";
	};
}

char * block_tbl(int i) {
	switch(i) {
		case 0: return "bonus";
		case 1: return "bonus2";
		case 2: return "bonus3";
		case 3: return "bonus4";
		case 4: return "bonus5";
		case 5: return "autobonus";
		case 6: return "autobonus2";
		case 7: return "autobonus3";
		case 8: return "heal";
		case 9: return "percentheal";
		case 10: return "itemheal";
		case 11: return "skill";
		case 12: return "itemskill";
		case 13: return "unitskilluseid";
		case 14: return "sc_start";
		case 15: return "sc_start4";
		case 16: return "sc_end";
		case 17: return "getitem";
		case 18: return "rentitem";
		case 19: return "delitem";
		case 20: return "getrandgroupitem";
		case 21: return "skilleffect";
		case 22: return "specialeffect2";
		case 23: return "setfont";
		case 24: return "buyingstore";
		case 25: return "searchstores";
		case 26: return "if";
		case 27: return "else";
		case 28: return "set";
		case 29: return "input";
		case 30: return "announce";
		case 31: return "callfunc";
		case 32: return "end";
		case 33: return "wrap";
		case 34: return "pet";
		case 35: return "bpet";
		case 36: return "mercenary_create";
		case 37: return "mercenary_heal";
		case 38: return "mercenary_sc_start";
		case 39: return "produce";
		case 40: return "cooking";
		case 41: return "makerune";
		case 42: return "guildgetexp";
		case 43: return "getexp";
		case 44: return "monster";
		case 45: return "homevolution";
		case 46: return "setoption";
		case 47: return "setmounting";
		case 48: return "setfalcon";
		case 49: return "getgroupitem";
		case 50: return "resetstatus";
		case 51: return "bonus_script";
		case 52: return "playbgm";
		case 53: return "transform";
		case 54: return "sc_start2";
		case 55: return "petloot";
		case 56: return "petrecovery";
		case 57: return "petskillbonus";
		case 58: return "petskillattack";
		case 59: return "petskillattack2";
		case 60: return "petskillsupport";
		case 61: return "petheal";
		case 62: return "for";
		default: return "error";
	}
}

int weapon_flag(int i) {
	switch(i) {
		case 0: return 0x10000000;
		case 1: return 0x00000001;
		case 2: return 0x00000002;
		case 3: return 0x00000004;
		case 4: return 0x00000008;
		case 5: return 0x00000010;
		case 6: return 0x00000020;
		case 7: return 0x00000040;
		case 8: return 0x00000080;
		case 10: return 0x00000100;
		case 11: return 0x00000200;
		case 12: return 0x00000400;
		case 13: return 0x00000800;
		case 14: return 0x00001000;
		case 15: return 0x00002000;
		case 16: return 0x00004000;
		case 17: return 0x00008000;
		case 18: return 0x00010000;
		case 19: return 0x00020000;
		case 20: return 0x00040000;
		case 21: return 0x00080000;
		case 22: return 0x00100000;
		case 23: return 0x00200000;
		case 25: return 0x00400000;
		case 26: return 0x00800000;
		case 27: return 0x01000000;
		case 28: return 0x02000000;
		case 29: return 0x04000000;
		case 30: return 0x08000000;
		default: break;
	}
	return 0;
}

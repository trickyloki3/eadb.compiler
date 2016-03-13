# Item Script Translation Project

A set of tools for translating **item_db.txt** into **idnum2itemdesctable.txt** or **itemInfo.lua**.

 * **db** - convert eathena and rathena databases from csv to sqlite3
 *  **ic** - lex, parse, and translate item scripts into script descriptions
 * **fmt** - format and write item attributes, flavour text, and script description

```item_db.txt --> db --> ic --> fmt --> idnum2itemdesctable.txt or itemInfo.lua```
# Build
Windows
 1. Open the visual studio 2015 solution
 2. Build x86 or x64 in debug or release mode.

Linux | OSX
 1. make

# Usage
```
db -p <server-path> -o <output-path> -m <mode>
db -p D:\\Git\\eathena -o . -m eathena
db -p D:\\Git\\rathena -o . -m rathena
db -p D:\\Git\\eadb.compiler\\db -o . -m resource
```
```
ic -i <item-db-path> -a <map-path> -d <resource-path> -o <output-path> -m <mode>
ic -i eathena.db -a ..\\db\\athena_db.txt -d resource.db -o result.txt -m eathena
ic -i rathena.db -a ..\\db\\athena_db.txt -d resource.db -o result.txt -m rathena
```

# Sample

**Support complex predicate generation** : Item ID 4427 : [Sword Guardian Card](http://ratemyserver.net/index.php?iname=4427&page=re_item_db&quick=1&isearch=Search)

```
.@i = getiteminfo(getequipid(EQI_HAND_R),11);
if(.@i==2||.@i==3) {
    bonus bHit,5;
    bonus bCritical,5;
    bonus2 bSkillAtk,62,25;
}
```
```
[4427]
[right hand's weapon type is one-handed sword, two-handed sword]
HIT +5
CRIT +5
+25% Bowling Bash damage
```

**Support item combo generation** : Item ID 4299 : [Clock Card](http://ratemyserver.net/index.php?iname=4299&page=re_item_db&quick=1&isearch=Search)

```
bonus3 bAutoSpellWhenHit,"CR_AUTOGUARD",3+7*(getskilllv("CR_AUTOGUARD")==10),30;
```
```
[4299]
+3% chance of casting level 3 ~ 10 (Guard Level) Guard when hit
[Tower Keeper Card, Alarm Card, Clock Card, Punk Card Combo]
DEF +3
MDEF +3
```

**Support nested if-else predicate generation** : Item ID 1426 : [Glorious Spear](http://ratemyserver.net/index.php?iname=1426&page=re_item_db&quick=1&isearch=Search)

```
bonus2 bAddRace,RC_DemiHuman,95;
bonus2 bAddRace,RC_Player,95;
bonus2 bIgnoreDefRaceRate,RC_DemiHuman,20;
bonus2 bIgnoreDefRaceRate,RC_Player,20;
bonus bUnbreakableWeapon,1;
.@r = getrefine();
if(.@r>5) {
    bonus2 bAddRace,RC_DemiHuman,pow(((.@r>14)?14:.@r)-4,2);
    bonus2 bAddRace,RC_Player,pow(((.@r>14)?14:.@r)-4,2);
    bonus2 bIgnoreDefRaceRate,RC_DemiHuman,5;
    bonus2 bIgnoreDefRaceRate,RC_Player,5; bonus bAspdRate,10;
}
if(.@r>8) {
    if(BaseJob==Job_Knight)
        bonus4 bAutoSpellOnSkill,"KN_PIERCE","NPC_CRITICALWOUND",2,200;
    else if(BaseJob==Job_Crusader)
        bonus3 bAutoSpell,"PA_PRESSURE",5,100;
}
```
```
[1426]
+95% physical damage against demi-human enemies
+95% physical damage against player enemies
pierce +20% physical defense of demi-human enemies
pierce +20% physical defense of player enemies
weapon is indestructible
[refine level 6 ~ 15]
+16% ~ +121% (pow) physical damage against demi-human enemies
+16% ~ +121% (pow) physical damage against player enemies
pierce +5% physical defense of demi-human enemies
pierce +5% physical defense of player enemies
ASPD +10%
[refine level 9 ~ 15]
[knight]
+20% chance of casting level 2 Critical Wounds when using Pierce
[crusader]
+10% chance of casting level 5 Gloria Domini when attacking
```

**Support pet script generation** : Item ID 12340 : [Chewy Rice Powder](http://ratemyserver.net/index.php?iname=12340&page=re_item_db&quick=1&isearch=Search)

```
pet 1815;
```
```
[12340]
Egg containing Rice Cake.
[Normal Bonus]
[HP < 50% and SP < 100%]
Cast Defending Aura [Lv. 3] for every 240 seconds.
[Loyal Bonus]
+1% neutral resistance
Max HP -1%
```

**Support item group generation.** : Item ID 603 : [Old Blue Box](http://ratemyserver.net/index.php?iname=603&page=re_item_db&quick=1&isearch=Search)

```
getrandgroupitem(IG_BlueBox,1);
```
```
[603]
Select 1 item from Blue Box group.
 * 52 healing items
 * 44 usable items
 * 423 etc items
 * 210 armor items
 * 239 weapon items
 * 20 pet equipment items
 * 28 ammo items
 * 6 delay usable items
```

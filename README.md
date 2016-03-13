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

Linux / OSX
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

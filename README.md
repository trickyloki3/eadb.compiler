db.exe | item database loader

usage
db.exe  -p    <server-path>  -o      <output-path>  -m    <server-type>
db.exe --path <server-path> --output <output-path> --mode <server-type>
<server-path> is the path to the root directory of the server
<output-path> is the path to the directory to hold the database
<server-type> can be "eathena", "rathena", "hercules", "resource"

example
db.exe -p G:\\git\\eathena            -o G:\\db -m eathena
db.exe -p G:\\git\\rathena            -o ..     -m rathena
db.exe -p G:\\git\\hercules           -o .      -m hercules
db.exe -p G:\\git\\eadb.compiler\\res -o G:\\db -m resources

ic.exe | item script compiler
** under construction **

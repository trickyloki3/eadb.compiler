/*
 *   file: itemr.c
 *   date: 12/13/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "script.h"

int main(int argc, char * argv[]) {
	int off = 0;
	FILE * file = NULL;
	FILE * file_dbg = NULL;
	int buf_size = BUF_SIZE * 2;
	char fld[buf_size];
	char buf[buf_size];
	char * result = NULL;
	memset(buf, 0, sizeof(buf_size));

	if(argc >= 3) {
		/* set the global database type */
		global_mode = (ncs_strcmp(argv[1],"rathena") == 0) ? RATHENA : 
					     (((ncs_strcmp(argv[1],"eathena") == 0) ? EATHENA : 
					     ((ncs_strcmp(argv[1],"hercules") == 0) ? HERCULES : -1)));
		if(global_mode != RATHENA && global_mode != EATHENA && global_mode != HERCULES) {
			fprintf(stderr,"[error]: invalid database type; only 'eathena', 'rathena', or 'hercules' is supported.");
			exit(EXIT_FAILURE);
		}

		/* open the file containing the script */
		file = fopen(argv[2],"r");
		if(file == NULL) fprintf(stderr,"failed to open file %s\n", argv[2]);
	} else {
		fprintf(stderr,"\n\tsyntax '%s [eathena | rathena | hercules] <file path>'\n"
					"\t -> '%s eathena myscript.txt'\n"
					"\t -> '%s rathena /home/loki/dev/script.txt'\n"
					"\t -> '%s hercules /home/loki/dev/file'\n", 
					argv[0], argv[0], argv[0], argv[0]);
	}

	/* initialize the global database */
	global_db = init_ic_db("athena.db");
	file_dbg = fopen("dump.txt", "w");
	if(file_dbg == NULL) {
		fprintf(stderr,"[error]: failed to load debug file.\n");
		exit(EXIT_FAILURE);
	}

	/* load the script file into the buffer */
	while(fgets(fld, buf_size, file) != NULL)
		off += sprintf(buf + off,"%s", fld);
	buf[off] = '\0';

	/* compile the script and display to stdout 
	 * use output redirection to store in a file */
	result = script_compile_raw(buf, 0, file_dbg);
	if(result != NULL) {
		printf(" -- script --\n%s\n-- item description --\n%s\n", buf, result);
		free(result);
	}
	
	/* clean up everything */
	deit_ic_db(global_db);
	fclose(file);
	fclose(file_dbg);
}

#include <script.h>
#if defined(_WIN32)
#include <windows.h>
#else
#define MAX_PATH PATH_MAX
#endif

char * path_db = NULL;
char * path_map = NULL;
char * path_item = NULL;
char * path_output = NULL;
int mode_athena = -1;
FILE * file_output = NULL;
FILE * file_error = NULL;

int setmode(int * result, char * mode) {
    if(0 == ncs_strcmp(mode, "eathena")) {
        *result = EATHENA;
    } else if(0 == ncs_strcmp(mode, "rathena")) {
        *result = RATHENA;
    } else if(0 == ncs_strcmp(mode, "hercule")) {
        *result = HERCULE;
    } else {
        return exit_mesg("invalid mode '%s'", mode);
    }

    return 0;
}

int setpath(char ** result, char * path) {
    int status = 0;
    char * buffer = NULL;

    buffer = calloc(MAX_PATH, sizeof(char));
    if(NULL == buffer) {
        status = exit_stop("out of memory");
#if defined(_WIN32)
    } else if(0 >= GetFullPathName(path, MAX_PATH, buffer, NULL)) {
#else
    } else if(NULL == realpath(path, buffer)) {
#endif
        status = exit_mesg("failed to resolve path '%s", path);
    } else {
        *result = buffer;
    }

    return status;
}

int getpath(int argc, char ** argv) {
    int i;
    int j;
    int len;
    char * str;
    int status = 0;

    for(i = 1; i < argc && !status; i++) {
        str = argv[i];
        len = (int) strlen(str);
        if(0 >= len || '-' != str[0])
            continue;
        for(j = 1; j < len; j++) {
            if( i + 1 >= argc ||
                '-' == argv[i + 1][0] ) {
                status = exit_mesg("missing argument for '%c'", str[j]);
            } else {
                switch(str[j]) {
                    case 'd': status = setpath(&path_db, argv[i + 1]); break;
                    case 'a': status = setpath(&path_map, argv[i + 1]); break;
                    case 'i': status = setpath(&path_item, argv[i + 1]); break;
                    case 'm': status = setmode(&mode_athena, argv[i + 1]); break;
#if defined(_WIN32)
                    case 'o': status = setpath(&path_output, argv[i + 1]); break;
#else
                    case 'o': path_output = convert_string(argv[i + 1]); break;
#endif
                }
            }
            if(status)
                return status;
        }
    }

    if(NULL == path_db) {
        status = exit_stop("missing -d <db-path>");
    } else if(NULL == path_map) {
        status = exit_stop("missing -a <map-path>");
    } else if(NULL == path_item) {
        status = exit_stop("missing -i <item-path>");
    } else if(NULL == path_output) {
        status = exit_stop("missing -o <output-path>");
    } else if(0 > mode_athena) {
        status = exit_stop("missing -m <mode>");
    } else {
        file_output = fopen(path_output, "w");
        if(NULL == file_output) {
            status = exit_mesg("failed to create file '%s'", path_output);
        } else {
            file_error = fopen("ic_error.txt", "w");
            if(NULL == file_error)
                status = exit_stop("failed to create error file 'ic_error.txt");
        }
    }

    if(status) {
        free_ptr(path_db);
        free_ptr(path_map);
        free_ptr(path_item);
        free_ptr(path_output);
        if(file_output)
            fclose(file_output);
        if (file_error)
            fclose(file_error);
    }

    return status;
}

int main(int argc, char * argv[]) {
    int status = 0;
    script_t * script = NULL;

    if(getpath(argc, argv)) {
        status = exit_mesg("%s -d <db-path> -a <map-path> -"
        "i <item-path> -m <mode> -o <output-path>", argv[0]);
    } else {
        if(script_init(&script, path_db, path_item, path_map, mode_athena)) {
            status = exit_stop("fail to load script context");
        } else {
            while(!item_iterate(script->db, &script->item)) {
                /* check that the script has at least one block */
                if(!script_check(script->item.script)) {
                    /* lex, parse, translate, and generate the blocks */
                    if( script_lexical(&script->token, script->item.script) ||
                        script_analysis(script, &script->token, NULL, NULL) ||
                        script_translate(script) ||
                        script_generate(script) ||
                        script_combo(script) ) {
                        fprintf(file_error, "[%d] script: %s\n", script->item.id, script->item.script);
                        if(script->blocks)
                            block_stack_dump(script->blocks, file_error);
                    } else{
                        fprintf(file_output, "[%d]\n%s", script->item.id, script->buffer);
                        if(item_desc_insert(script->db, script->item.id, script->buffer))
                            exit_mesg("failed to write %d and %s", script->item.id, script->buffer);
                    }
                }
                script_block_free_all(script);
            }
        }

        script_deit(&script);
        free_ptr(path_db);
        free_ptr(path_map);
        free_ptr(path_item);
        free_ptr(path_output);
        if (file_output)
            fclose(file_output);
        if (file_error)
            fclose(file_error);
    }

    return status;
}

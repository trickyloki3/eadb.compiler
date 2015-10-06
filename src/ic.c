#include <script.h>

#define RE_PATH     "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\resource.db"
#define RA_PATH     "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\rathena.db"
#define MAP_PATH    "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\athena_db.txt"

int main(int argc, char * argv[]) {
    /*script_t * context = NULL;
    if (script_init(&context, RE_PATH, RA_PATH, MAP_PATH, RATHENA))
        return 0;
    script_deit(&context);*/
    int i = 0;
    int level = 0;
    char * script = NULL;
    script_t * context = NULL;

    if(script_init(&context, RE_PATH, RA_PATH, MAP_PATH, RATHENA))
        return 0;

    while(!item_iterate(context->db, &context->item)) {
        /* skip any script without statements */
        script = context->item.script;
        for (i = 0; script[i] != '\0'; i++) {
            /* comments can be problematic */
            if (script[i + 1] != '\0' && script[i] == '/' && script[i + 1] == '*') {
                level++;
            } else if (script[i] != '\0'  && script[i] == '*' && script[i + 1] == '/') {
                level--;
                i += 2;
            }
            if (!level && ';' == script[i])
                goto compile;
        }
        continue;

    compile:
        /* compile the item script */
        if (script_lexical(&context->token, context->item.script) ||
            script_analysis(context, &context->token, NULL, NULL) ||
            script_translate(context) ||
            script_generate(context) ||
            context->item.id == 0) {
            fprintf(stderr, "[%d] script: %s\n", context->item.id, context->item.script);
            goto failed;
        }

        /*printf("%s", context->buffer);
        script_block_dump(context, stderr);*/
        script_block_free_all(context);
    }

    goto clean;

clean:
    script_deit(&context);
    return 0;

failed:
    if(NULL != context->blocks)
        script_block_dump(context, stderr);
    goto clean;
}

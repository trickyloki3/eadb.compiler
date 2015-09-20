#include <script.h>

#define RE_PATH     "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\opt.db"
#define EA_PATH     "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\ea.db"
#define MAP_PATH    "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\athena_db.txt"

int main(int argc, char * argv[]) {
    int i = 0;
    int len = 0;
    int ret = 0;
    char * script = NULL;
    script_t * context = NULL;

    if(script_init(&context, RE_PATH, EA_PATH, MAP_PATH, EATHENA))
        return 0;

    while(!item_iterate(context->db, &context->item)) {
        /* skip any script without statements */
        script = context->item.script;
        for (i = 0; script[i] != '\0'; i++)
            if (';' == script[i])
                goto compile;
        continue;

compile:
        /* compile the item script */
        if (script_lexical(&context->token, context->item.script) ||
            script_analysis(context, &context->token, NULL, NULL) ||
            script_translate(context) ||
            context->item.id == 0)
            goto failed;

        /* reset all the blocks */
        script_block_free_all(context);
    }

clean:
    script_deit(&context);
    return 0;

failed:
    script_block_dump(context, stderr);
    goto clean;
}

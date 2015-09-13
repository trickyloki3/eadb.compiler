#include <script.h>

int main(int argc, char * argv[]) {
    int i = 0;
    int len = 0;
    char * script = NULL;
    script_t * scribe = NULL;

    if (script_init(&scribe,
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\opt.db",
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\ea.db",
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\athena_db.txt",
        EATHENA))
        exit(EXIT_FAILURE);

    while(!item_iterate(scribe->db, &scribe->item)) {
        scribe->offset = 0;

        /* skip empty scripts */
        script = scribe->item.script;
        len = strlen(script);
        if (0 >= len)
            continue;

        /* skip on empty statements */
        for (i = 0; i < len; i++)
            if (';' == script[i])
                break;
        if (i == len)
            continue;

        /* compile the item script */
        if (script_lexical(&scribe->token, scribe->item.script) ||
            script_analysis(scribe, &scribe->token, NULL, NULL) ||
            script_translate(scribe)) {
            script_block_dump(scribe, stderr);
            printf("Error on item %d; %s!\n", scribe->item.id, scribe->item.script);
            break;
        }

        if( script_generate(scribe, scribe->buffer, &scribe->offset)) {
            script_block_dump(scribe, stderr);
            printf("Error on item %d; %s!\n", scribe->item.id, scribe->item.script);
            break;
        }

        if (scribe->item.id == 0) {
            script_block_dump(scribe, stderr);
            printf("%d; %s\n", scribe->item.id, scribe->buffer);
            break;
        }
        /*printf("%d; %s\n", scribe->item.id, scribe->buffer);*/
        script_block_reset(scribe);
    }

    script_deit(&scribe);
    return 0;
}

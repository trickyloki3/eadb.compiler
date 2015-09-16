#include <script.h>

int main(int argc, char * argv[]) {
    int i = 0;
    int len = 0;
    int ret = 0;
    char * script = NULL;
    script_t * scribe = NULL;

    if (script_init(&scribe,
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\opt.db",
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\ea.db",
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\res\\athena_db.txt",
        EATHENA))
        exit(EXIT_FAILURE);

    while(!item_iterate(scribe->db, &scribe->item)) {
        /* reset variant data */
        script_block_reset(scribe);

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
        if (script_lexical(&scribe->token, scribe->item.script))
            goto failed;

        ret = script_analysis(scribe, &scribe->token, NULL, NULL);
        if (SCRIPT_SKIPPED == ret)
            continue;
        else if (ret)
            goto failed;

        if (script_translate(scribe))
            goto failed;

        if (scribe->item.id == 4421) {
            script_block_dump(scribe, stderr);
            printf("%d; %s\n", scribe->item.id, scribe->buffer);
            break;
        }
    }

clean:
    script_deit(&scribe);
    return 0;

failed:
    script_block_dump(scribe, stderr);
    printf("Item %d; %s\n", scribe->item.id, scribe->item.script);
    goto clean;
}

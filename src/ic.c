#include <script.h>

int main(int argc, char * argv[]) {
    script_t * scribe = NULL;

    if (script_init(&scribe,
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\opt.db",
        "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\ea.db",
        EATHENA))
        exit(EXIT_FAILURE);

    while(!item_iterate(scribe->db, &scribe->item)) {
        if (script_lexical(&scribe->token, scribe->item.script) ||
            script_analysis(scribe, &scribe->token, NULL, NULL) ||
            script_translate(scribe) ||
            script_bonus(scribe) ||
            script_generate(scribe, scribe->buffer, &scribe->offset)) {
            script_block_dump(scribe, stderr);
            break;
        }
        script_block_reset(scribe);
    }

    script_deit(&scribe);
    return 0;
}

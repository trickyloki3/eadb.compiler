#include <script.h>

int main(int argc, char * argv[]) {
    script_t * scribe = NULL;

    if (script_init(&scribe, "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\opt.db", "C:\\Users\\trickyloki3\\Desktop\\git\\eadb.compiler\\out\\ea.db", EATHENA))
        return 1;

    while(!item_iterate(scribe->db, &scribe->item)) {
        script_block_reset(scribe);

        if(script_lexical(&scribe->token, scribe->item.script))
            continue;
        if(script_analysis(scribe, &scribe->token, NULL, NULL))
            continue;
        if(script_translate(scribe))
            continue;
        /*if(script_bonus(scribe))
            continue;
        if(script_generate(scribe, scribe->buffer, &scribe->offset))
            continue;*/
    }

    script_deit(&scribe);
    return 0;
}

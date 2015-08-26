/*
 *   file: item.c
 *   date: 11/12/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "lua.h"
#include "lauxlib.h"
#include "time.h"
#include "script.h"
#include "format.h"
#include "db_search.h"

int table_getfieldstring(lua_State * lstate, const char * field, const char ** str, int table);

typedef struct server_t {
    const char * format_name;   /* indicate txt or lua format */
    const char * server_name;   /* indicate eathena, rathena, or hercules */
    int format_type;            /* get_format_type to convert format_name */
    int server_type;            /* get_server_type to convert server_name */

    FILE * output;              /* the output file */
    const char * filename;      /* the output filename */

    script_t script;
    const char * resource_path; /* resource database path */
    const char * database_path; /* server database path */

    format_t format;
    const char * text_path;
} server_t;

int init_server(server_t **, lua_State *, int);
int deit_server(server_t **);
int get_server_type(const char *, int *);
int get_format_type(const char *, int *);
int translate_server(server_t *, lua_State *);

int main(int argc, char * argv[]) {
    server_t * server = NULL;
    int server_entry = 0;
    lua_State * lua = NULL;

    /* load the configuration file */
    lua = luaL_newstate();
    if(lua == NULL)
        return exit_abt_safe("failed to start lua virtual machine");

    if(luaL_loadfile(lua, "itemc.cfg") ||
       lua_pcall(lua, 0, 0, 0))
        return exit_func_safe("failed to load itemc.cfg; %s", lua_tostring(lua, -1));

    lua_getglobal(lua, "compilation_table");
    if(!lua_istable(lua, -1))
        return exit_abt_safe("compilation_table in itemc.cfg must be a table");

    /* translate each server's item database */
    lua_pushnil(lua);
    while(lua_next(lua, 1)) {
        server_entry = lua_gettop(lua);
        init_server(&server, lua, server_entry);
        translate_server(server, lua);
        deit_server(&server);

        /* get next server entry */
        lua_pop(lua, lua_gettop(lua) - server_entry + 1);
    }
    lua_close(lua);
    return 0;
}

int table_getfieldstring(lua_State * lstate, const char * field, const char ** str, int table) {
    lua_getfield(lstate, table, field);
    if(!lua_isstring(lstate, -1))
        return exit_func_safe("the '%s' must have a string value", field);
    *str = lua_tolstring(lstate, -1, NULL);
    return CHECK_PASSED;
}

int init_server(server_t ** server, lua_State * lua, int index) {
    server_t * _server = NULL;

    if(exit_null_safe(2, server, lua))
        return CHECK_FAILED;

    _server = calloc(1, sizeof(server_t));
    if(NULL == _server)
        return CHECK_FAILED;

    if( table_getfieldstring(lua, "output_format", &_server->format_name, index) ||
        table_getfieldstring(lua, "output_filename", &_server->filename, index) ||
        table_getfieldstring(lua, "server_type", &_server->server_name, index) ||
        table_getfieldstring(lua, "resource_db_path", &_server->resource_path, index) ||
        table_getfieldstring(lua, "server_db_path", &_server->database_path, index) ||
        table_getfieldstring(lua, "flavour_text_db_path", &_server->text_path, index) ||
        get_server_type(_server->server_name, &_server->server_type) ||
        get_format_type(_server->format_name, &_server->format_type)) {
        deit_server(&_server);
        return CHECK_FAILED;
    }

    _server->output = fopen(_server->filename, "w");
    if(NULL == _server->output) {
        exit_func_safe("failed to open '%s' for writing", _server->filename);
        goto failed;
    }

    _server->script.mode = _server->server_type;

    if(init_db_load(&_server->script.db, _server->resource_path, _server->database_path, _server->server_type)) {
        exit_func_safe("failed to open '%s' and '%s'", _server->resource_path, _server->database_path);
        goto failed;
    }

    if(init_flavour_db(&_server->format, _server->text_path)) {
        exit_func_safe("failed to open '%s'", _server->text_path);
        goto failed;
    }

    if(init_format(&_server->format, lua, index, _server->format_type, _server->server_type)) {
        exit_func_safe("failed to init formating on '%s' and '%s'", _server->server_name, _server->format_name);
        goto failed;
    }

    /* give reference to formatter */
    _server->format.server_db = _server->script.db;

    *server = _server;
    return CHECK_PASSED;

failed:
    deit_server(&_server);
    return CHECK_FAILED;
}

int deit_server(server_t ** server) {
    server_t * _server = NULL;

    if(exit_null_safe(2, server, *server))
        return CHECK_FAILED;

    _server = *server;

    deit_format(&_server->format);

    if(NULL != _server->script.db)
        deit_db_load(&_server->script.db);

    if(NULL != _server->output)
        fclose(_server->output);

    script_block_finalize(&_server->script);

    SAFE_FREE(_server);
    *server = NULL;
    return CHECK_PASSED;
}

int get_server_type(const char * server_name, int * server_type) {
    int _server_type = 0;

    if(exit_null_safe(2, server_name, server_type))
        return CHECK_FAILED;

    if(ncs_strcmp(server_name,"eathena") == 0) {
        _server_type= EATHENA;
    } else if(ncs_strcmp(server_name,"rathena") == 0) {
        _server_type = RATHENA;
    } else if(ncs_strcmp(server_name,"hercules") == 0) {
        _server_type = HERCULE;
    } else {
        return exit_func_safe("'%s' is not a recognized server type", server_name);
    }

    *server_type = _server_type;

    return CHECK_PASSED;
}

int get_format_type(const char * format_name, int * format_type) {
    int _format_type = 0;

    if(exit_null_safe(2, format_name, format_type))
        return CHECK_FAILED;

    if(ncs_strcmp(format_name, "txt") == 0) {
        _format_type = FORMAT_TXT;
    } else if(ncs_strcmp(format_name, "lua") == 0) {
        _format_type = FORMAT_LUA;
    } else {
        return exit_func_safe("'%s' is not a recognized format type", format_name);
    }

    *format_type = _format_type;
    return CHECK_PASSED;
}

int translate_server(server_t * server, lua_State * lua) {
    int offset = 0;
    char buffer[BUF_SIZE];
    item_t item;

    /* write the header */
    write_header(server->output, &server->format, lua);

    /* write the item description */
    while(!item_iterate(server->script.db, &item)) {
        if(item.script != NULL && strlen(item.script) > 0) {
            /* compile the item script */
            offset = 0;
            server->script.item_id = item.id;
            if(!script_lexical(&server->script.token, item.script)) {
                if(script_analysis(&server->script, &server->script.token, NULL, NULL) == SCRIPT_PASSED)
                    if(!script_translate(&server->script))
                        if(!script_bonus(&server->script))
                            if(!script_generate(&server->script, buffer, &offset))
                                if(!script_generate_combo(server->script.item_id, buffer, &offset, server->script.db, server->script.mode))
                                    ;
            }
            script_block_reset(&server->script);
            fprintf(stderr,"\r[info]: compiling item id %d ...\r", server->script.item_id);
        }

        /* write the item flavour text, attribute, and script per the file format */
        write_item(server->output, &server->format, &item, buffer);

        /* reset the script translation buffer */
        buffer[0] = '\0';
    }

    /* write the footer */
    write_footer(server->output, &server->format, lua);

    return CHECK_PASSED;
}

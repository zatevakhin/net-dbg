
#include "luaapi.h"


int luaapi_requiremodule(lua_State* L, char* libname, const char* code) {
    if (luaL_dostring(L, code)) {
        const char* str = lua_tostring( L, -1 );
        lua_pop(L, 1);
        printf("Error: in (%s) -> %s\n", libname, str);
    }
}

int luaapi_requirecode(lua_State* L, char* libname, const char* code) {
    if (!luaL_dostring(L, code)) {
        lua_setglobal(L, libname);
    } else {
        const char* str = lua_tostring( L, -1 );
        lua_pop(L, 1);
        printf("Error: in (%s) -> %s\n", libname, str);
    }
}

void luaapi_set_field_function(lua_State *L, char *field, lua_CFunction value) {
    lua_pushstring(L, field);
    lua_pushcfunction(L, value);
    lua_settable(L, -3);
}

void luaapi_set_field_string(lua_State *L, char *field, char* value) {
    lua_pushstring(L, field);
    lua_pushstring(L, value);
    lua_settable(L, -3);
}

void luaapi_set_field_int(lua_State *L, char *field, int value) {
    lua_pushstring(L, field);
    lua_pushinteger(L, value);
    lua_settable(L, -3);
}

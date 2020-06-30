
#ifndef LIB_LUAAPI_H
#define LIB_LUAAPI_H

#include <lua5.2/lauxlib.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lua.h>


int luaapi_requiremodule(lua_State* L, char* libname, const char* libcode);
int luaapi_requirecode(lua_State* L, char* libname, const char* libcode);

void luaapi_set_field_function(lua_State *L, char *variable, lua_CFunction value);
void luaapi_set_field_string(lua_State *L, char *variable, char* value);
void luaapi_set_field_int(lua_State *L, char *variable, int value);

#define luaapi_set_field(L, variable, value) _Generic(value, \
    int: luaapi_set_field_int,                               \
    char*: luaapi_set_field_string,                          \
    lua_CFunction: luaapi_set_field_function                 \
)(L, variable, value)

#endif //LIB_LUAAPI_H

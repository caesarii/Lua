#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static void
stackDump(lua_State *L) {

    // ջ������
    int top = lua_gettop(L);
    // ����ջ
    for(int i = 1; i <= top; i++) {
        // Ԫ������
        int t = lua_type(L, i);
        // printf("type %d \n", i);
        switch(t) {
            case LUA_TSTRING: {
                // �ַ���
                printf("'%s'", lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN: {
                // ����ֵ
                printf(lua_toboolean(L, i) ? "true":"false");
                break;
            }
            case LUA_TNUMBER: {
                // ����
                printf("%g", lua_tonumber(L, i));
                break;
            }
            default: {
                printf("%s", lua_typename(L, t));
                break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

static void
stackOperate(lua_State *L) {


    lua_pushboolean(L, true);
    lua_pushnumber(L, 10);
    lua_pushnil(L);
    lua_pushstring(L, "hello");

    stackDump(L);

    // ����ָ��Ԫ��
    lua_pushvalue(L, -4);

    stackDump(L);

    // ��ջ���滻ָ��Ԫ��
    lua_replace(L, 3);
    stackDump(L);

    // ����ջ��
    lua_settop(L, 6);
    stackDump(L);

    // �Ƴ�ָ��Ԫ��
    lua_remove(L, -3);
    stackDump(L);

    lua_settop(L, -5);
    stackDump(L);
}


#define MAX_COLOR 255

static int
getfield(lua_State *L, const char *key) {
    int result;
    // ���� table λ��ջ��
    // push key

    lua_pushstring(L, key);
    // get value
    int t = lua_gettable(L, -2);

    if(t == 3) {
        // number
        result = (int)lua_tonumber(L, -1);
    }
    // ���� value, �ָ�ջ
    lua_pop(L, 1);
    return result;
}

static const char *
getfieldstring(lua_State *L, const char *key) {
    const char * result;
    // ���� table λ��ջ��
    // push key

    lua_pushstring(L, key);
    // get value
    int t = lua_gettable(L, -2);

    if (t != 4) {
        // string
        printf("ERROR: is not a string %d", t);
    }

    result = lua_tostring(L, -1);

    // ���� value, �ָ�ջ
    lua_pop(L, 1);

    return result;
}

static int
loadConfig(lua_State *L, const char *fname, int *w, int *h, int *title) {
    if(luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0)) {
        printf("LUA ERROR: %s\n", lua_tostring(L, -1));
        return -1;
    }

    // ���ӱ���ʱ��Ҫ��������

    int t = lua_getglobal(L, "config");
    // printf("type %d", t);
    if(t != 5) {
        printf("\n ERROR: is not a table \n");
        return 0;
    }

    *w = getfield(L, "width");
    *h = getfield(L, "height");
    *title = getfield(L, "title");
    return 0;
}

int
main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // ջ����
    // stackOperate(L);

    // ��������
    const char *path = "gua.lua";
    int width;
    int height;
    int header;
    int *w = &width;
    int *h = &height;
    int *title = &header;
    loadConfig(L, path, w, h, title);
    printf("width: %d  height: %d title: %d", *w, *h, *title);

    lua_close(L);

    return 0;
}
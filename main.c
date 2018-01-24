#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static void
stackDump(lua_State *L) {

    // 栈顶索引
    int top = lua_gettop(L);
    // 遍历栈
    for(int i = 1; i <= top; i++) {
        // 元素类型
        int t = lua_type(L, i);
        // printf("type %d \n", i);
        switch(t) {
            case LUA_TSTRING: {
                // 字符串
                printf("'%s'", lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN: {
                // 布尔值
                printf(lua_toboolean(L, i) ? "true":"false");
                break;
            }
            case LUA_TNUMBER: {
                // 数字
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

    // 复制指定元素
    lua_pushvalue(L, -4);

    stackDump(L);

    // 以栈顶替换指定元素
    lua_replace(L, 3);
    stackDump(L);

    // 设置栈顶
    lua_settop(L, 6);
    stackDump(L);

    // 移除指定元素
    lua_remove(L, -3);
    stackDump(L);

    lua_settop(L, -5);
    stackDump(L);
}


#define MAX_COLOR 255

static int
getfield(lua_State *L, const char *key) {
    int result;
    // 假设 table 位于栈顶
    // push key

    lua_pushstring(L, key);
    // get value
    int t = lua_gettable(L, -2);

    if(t == 3) {
        // number
        result = (int)lua_tonumber(L, -1);
    }
    // 弹出 value, 恢复栈
    lua_pop(L, 1);
    return result;
}

static const char *
getfieldstring(lua_State *L, const char *key) {
    const char * result;
    // 假设 table 位于栈顶
    // push key

    lua_pushstring(L, key);
    // get value
    int t = lua_gettable(L, -2);

    if (t != 4) {
        // string
        printf("ERROR: is not a string %d", t);
    }

    result = lua_tostring(L, -1);

    // 弹出 value, 恢复栈
    lua_pop(L, 1);

    return result;
}

static int
loadConfig(lua_State *L, const char *fname, int *w, int *h, int *title) {
    if(luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0)) {
        printf("LUA ERROR: %s\n", lua_tostring(L, -1));
        return -1;
    }

    // 增加变量时需要由下向上

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
dump(lua_State *L) {
    int n = lua_gettop(L);
    printf("LUA STACK TOP %d \n", n);
    return 0;
}

struct Config {
    int width;
    int height;
    char * title;
};

int f(lua_State *L, struct Config *config) {
    int z;

    dump(L); // 0

    // 压入函数和参数
    lua_getglobal(L, "f");
    dump(L); // 1

    if(lua_pcall(L, 0, 3, 0) != 0) {
        printf("LUA ERROR: %s \n", lua_tostring(L, -1));
    }

    dump(L); // 3 弹出一个, 压入 3 个

    // 获取结果
    int w = lua_tonumber(L, -3);
    int h = lua_tonumber(L, -2);
    const char *title = lua_tostring(L, -1);
//    printf("w %d", w);
//    printf("h %d", h);
//    printf("title %s", title);


    config->height = h;
    config->width = w;
    config->title = title;
    lua_pop(L, 1);
    return z;
}



int
main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if(luaL_dofile(L, "config.lua")) {
        printf("LUA ERROR: %s\n", lua_tostring(L, -1));
        return -1;
    }

    // 加载配置
    struct Config *config;
    f(L, config);

    int w = config->width;
    int h = config->height;
    char *title;
    title = config->title;

    printf("w: %d, h: %d, title: %s \n", w, h, title);

    lua_close(L);

    return 0;
}

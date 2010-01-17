/*
 * Copyright (c) 2010, Albertas Vyšniauskas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of the software author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "DateTime.h"

#include <stdint.h>
#include <locale.h>
#include <langinfo.h>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

namespace lua {

static int lua_datetime_get_weekday_names(lua_State *L){


	lua_newtable(L);
	int table = lua_gettop(L);

	int ln_enums[] = {
		ABDAY_1, ABDAY_2, ABDAY_3, ABDAY_4, ABDAY_5, ABDAY_6, ABDAY_7
	};

	for (int i = 0; i < 7; i++){
		lua_pushnumber(L, i+1);
		lua_pushstring(L, nl_langinfo(ln_enums[i]));
		lua_settable(L, table);
	}

	return 1;
}

static int lua_datetime_get_first_weekday(lua_State *L){

	union { unsigned int word; char *string; } langinfo;
    int week_1stday = 0;
    int first_weekday = 1;
    unsigned int week_origin;

    langinfo.string = nl_langinfo(_NL_TIME_FIRST_WEEKDAY);
    first_weekday = langinfo.string[0];
    langinfo.string = nl_langinfo(_NL_TIME_WEEK_1STDAY);
    week_origin = langinfo.word;

    if (week_origin == 19971130) /* Sunday */
        week_1stday = 0;
    else if (week_origin == 19971201) /* Monday */
        week_1stday = 1;
    else
        return 0;

	lua_pushinteger(L, week_1stday + first_weekday);

	return 1;
}

static const struct luaL_reg lua_datetime_f[] = {
	{"get_first_weekday", lua_datetime_get_first_weekday},
	{"get_weekday_names", lua_datetime_get_weekday_names},
	{NULL, NULL}
};

int luaopen_datetime(lua_State *L){
	luaL_newmetatable(L, "datetime");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_register(L, "datetime", lua_datetime_f);

	lua_pop(L, 2);
	
	return 0;
}

}







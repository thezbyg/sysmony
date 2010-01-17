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

#include "Screen.h"

#include <gdk/gdk.h>

#include <stdint.h>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

namespace lua {

static int lua_screen_get_dimensions(lua_State *L){
	
	GdkScreen *screen = gdk_screen_get_default();

	lua_newtable(L);
	int table = lua_gettop(L);

	lua_pushstring(L, "width");
	lua_pushinteger(L, gdk_screen_get_width(screen));
	lua_settable(L, table);

	lua_pushstring(L, "height");
	lua_pushinteger(L, gdk_screen_get_height(screen));
	lua_settable(L, table);

	return 1;
}

static const struct luaL_reg lua_screen_f[] = {
	{"get_dimensions", lua_screen_get_dimensions},
	{NULL, NULL}
};

int luaopen_screen(lua_State *L){
	luaL_newmetatable(L, "screen");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_register(L, "screen", lua_screen_f);
	
	lua_pop(L, 2);

	return 1;
}

}

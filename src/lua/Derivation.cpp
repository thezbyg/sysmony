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

#include "Derivation.h"

#include <string.h>
#include <stdlib.h>

#include <iostream>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

namespace lua {

void* sm_lua_getuserdata(lua_State *L, int ud, const char *tname){

	if (lua_type(L, ud)==LUA_TUSERDATA && lua_getmetatable(L, ud)){

		lua_getfield(L, -1, "_sm_derived_from");
		if (lua_type(L, -1) == LUA_TTABLE){
			lua_pushstring(L, tname);
			lua_gettable(L, -2);
			if (lua_type(L, -1) == LUA_TNUMBER){
				lua_pop(L, 3);
				return lua_touserdata(L, ud);
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 1);

		lua_getfield(L, LUA_REGISTRYINDEX, tname);
		if (lua_rawequal(L, -1, -2)){
			lua_pop(L, 2);
			return lua_touserdata(L, ud);
		}else{
			lua_pop(L, 1);
		}
	}
	return NULL;
}

void sm_lua_add_derivation_info(lua_State *L, const char **derived_from){
	lua_pushstring(L, "_sm_derived_from");

	uint32_t len = 0;
	while (derived_from[len]){
		len++;
	}

	lua_createtable(L, 0, len);
	for (uint32_t i = 0; i < len; i++){
		lua_pushstring(L, derived_from[i]);
		lua_pushnumber(L, 0);
		lua_settable(L, -3);
	}

	lua_settable(L, -3);
}

}

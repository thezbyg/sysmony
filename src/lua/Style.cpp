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

#include "Style.h"
#include "Derivation.h"

#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <memory>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

using namespace std;
using namespace engine;

namespace lua {

static int lua_newstyle(lua_State *L){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<engine::Style>));
	shared_ptr<engine::Style> *style = new(dataptr) shared_ptr<engine::Style>();
	luaL_getmetatable(L, "style");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<engine::Style> lua_checkstyle(lua_State *L, int index){
	void *dataptr = sm_lua_getuserdata(L, index, "style");
	luaL_argcheck(L, dataptr != NULL, index, "`style' expected");
	return *reinterpret_cast<shared_ptr<engine::Style>*>(dataptr);
}

int lua_pushstyle(lua_State *L, shared_ptr<engine::Style> style_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<engine::Style>));
	shared_ptr<engine::Style> *style = new(dataptr) shared_ptr<engine::Style>(style_);
	luaL_getmetatable(L, "style");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_style_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "style");
	reinterpret_cast<shared_ptr<Style>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_style_f[] = {
	{"new", lua_newstyle},
	{NULL, NULL}
};

static const struct luaL_reg lua_style_m[] = {
	{NULL, NULL}
};

int luaopen_style(lua_State *L){
	luaL_newmetatable(L, "style");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_style_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"style", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_openlib(L, NULL, lua_style_m, 0);
	luaL_openlib(L, "style", lua_style_f, 0);

	return 1;
}


}

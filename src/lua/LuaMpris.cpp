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

#include "LuaMpris.h"

#include "Bindings.h"
#include "Time.h"
#include "Derivation.h"

#include <time.h>
#include <unistd.h>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

#include <iostream>

using namespace std;

namespace lua{

LuaUpdateMpris::LuaUpdateMpris(Instance *instance, RootWindow *root_window_, lua_State *L_, int function_ptr_, int argument_ptr_, const char *player_name):UpdateMpris(instance, root_window_, player_name){
	L = L_;
	function_ptr = function_ptr_;
	argument_ptr = argument_ptr_;
}

LuaUpdateMpris::~LuaUpdateMpris(){
	luaL_unref(L, LUA_REGISTRYINDEX, function_ptr);
	luaL_unref(L, LUA_REGISTRYINDEX, argument_ptr);
}

void LuaUpdateMpris::update(uint32_t now){
    lock();
	instance->lockLua();

	int status;
	int stack_top = lua_gettop(L);
	lua_rawgeti(L, LUA_REGISTRYINDEX, function_ptr);
	lua_rawgeti(L, LUA_REGISTRYINDEX, argument_ptr);
	
	lua_newtable(L);
	int table = lua_gettop(L);

	lua_pushstring(L, "title");
	lua_pushstring(L, title.c_str());
	lua_settable(L, table);
	lua_pushstring(L, "artist");
	lua_pushstring(L, artist.c_str());
	lua_settable(L, table);
 	lua_pushstring(L, "album");
	lua_pushstring(L, album.c_str());
	lua_settable(L, table);
 	lua_pushstring(L, "genre");
	lua_pushstring(L, genre.c_str());
	lua_settable(L, table);

 	lua_pushstring(L, "status");
	lua_newtable(L);
	int status_table = lua_gettop(L);

	for (int i = 0; i < 4; i++){
 		lua_pushinteger(L, i+1);
		lua_pushinteger(L, this->status[i]);
		lua_settable(L, status_table);
	}

	lua_settable(L, table);
 
 	
	if ((status=lua_pcall(L, 2, 0, 0))==0){

	}else{
		cerr<<"LuaUpdateMpris::update: "<<lua_tostring (L, -1)<<endl;
	}

	lua_settop(L, stack_top);

	instance->unlockLua();
	unlock();
}








static int lua_newluaupdatempris(lua_State *L){

	int function_ptr, argument_ptr;

    Instance *instance = lua_checkinstance(L, 2);

	shared_ptr<RootWindow> root_window = lua_checkrootwindow(L, 3);

	luaL_checktype(L, 4, LUA_TFUNCTION);
	lua_pushvalue(L, 4);
	function_ptr = luaL_ref(L, LUA_REGISTRYINDEX);
	
	luaL_checktype(L, 5, LUA_TTABLE);
	lua_pushvalue(L, 5);
	argument_ptr = luaL_ref(L, LUA_REGISTRYINDEX);

	const char *player_name = luaL_checkstring(L, 6);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<LuaUpdateMpris>));
	new(dataptr) shared_ptr<LuaUpdateMpris>(new LuaUpdateMpris(instance, root_window.get(), L, function_ptr, argument_ptr, player_name));
	luaL_getmetatable(L, "luaupdatempris");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<LuaUpdateMpris> lua_checkluaupdatempris(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "luaupdatempris"))){
		return *reinterpret_cast<shared_ptr<LuaUpdateMpris>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`luaupdatempris' expected");
	return shared_ptr<LuaUpdateMpris>();
}

int lua_pushluaupdatempris(lua_State *L, shared_ptr<LuaUpdateMpris> luaupdatempris_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<LuaUpdateMpris>));
	new(dataptr) shared_ptr<LuaUpdateMpris>(luaupdatempris_);
	luaL_getmetatable(L, "luaupdatempris");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_luaupdatempris_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "luaupdatempris");
	reinterpret_cast<shared_ptr<LuaUpdateMpris>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_luaupdatempris_f[] = {
	{"new", lua_newluaupdatempris},
	{NULL, NULL}
};

static const struct luaL_reg lua_luaupdatempris_m[] = {
	{NULL, NULL}
};

int luaopen_luaupdatempris(lua_State *L){
	luaL_newmetatable(L, "luaupdatempris");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_luaupdatempris_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"luaupdatempris", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_luaupdatempris_m);
	luaL_register(L, "luaupdatempris", lua_luaupdatempris_f);

	lua_pop(L, 2);
	
	return 1;
}




}


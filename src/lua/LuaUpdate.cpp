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

#include "LuaUpdate.h"
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

PollingLuaUpdate::PollingLuaUpdate(lua_State *L_, int function_ptr_, int argument_ptr_, uint32_t update_interval_, uint32_t now):PollingUpdate(update_interval_, now){
	L = L_;
	function_ptr = function_ptr_;
	argument_ptr = argument_ptr_;
}

PollingLuaUpdate::~PollingLuaUpdate(){
	luaL_unref(L, LUA_REGISTRYINDEX, function_ptr);
	luaL_unref(L, LUA_REGISTRYINDEX, argument_ptr);
}

void PollingLuaUpdate::update(){
	int status;
	int stack_top = lua_gettop(L);
	lua_rawgeti(L, LUA_REGISTRYINDEX, function_ptr);
	lua_rawgeti(L, LUA_REGISTRYINDEX, argument_ptr);
	if ((status=lua_pcall(L, 1, 0, 0))==0){

	}else{
		cerr<<"PollingLuaUpdate::update: "<<lua_tostring (L, -1)<<endl;
	}

	lua_settop(L, stack_top);
}








static int lua_newpollingluaupdate(lua_State *L){

	int function_ptr, argument_ptr;

	luaL_checktype(L, 2, LUA_TFUNCTION);
	lua_pushvalue(L, 2);
	function_ptr = luaL_ref(L, LUA_REGISTRYINDEX);
	
	luaL_checktype(L, 3, LUA_TTABLE);
	lua_pushvalue(L, 3);
	argument_ptr = luaL_ref(L, LUA_REGISTRYINDEX);

	double interval = luaL_checknumber(L, 4);

	uint32_t now = getTime();

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<PollingLuaUpdate>));
	new(dataptr) shared_ptr<PollingLuaUpdate>(new PollingLuaUpdate(L, function_ptr, argument_ptr, interval, now));
	luaL_getmetatable(L, "pollingluaupdate");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<PollingLuaUpdate> lua_checkpollingluaupdate(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "pollingluaupdate"))){
		return *reinterpret_cast<shared_ptr<PollingLuaUpdate>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`pollingluaupdate' expected");
	return shared_ptr<PollingLuaUpdate>();
}

int lua_pushpollingluaupdate(lua_State *L, shared_ptr<PollingLuaUpdate> pollingluaupdate_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<PollingLuaUpdate>));
	new(dataptr) shared_ptr<PollingLuaUpdate>(pollingluaupdate_);
	luaL_getmetatable(L, "pollingluaupdate");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_pollingluaupdate_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "pollingluaupdate");
	reinterpret_cast<shared_ptr<PollingLuaUpdate>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_pollingluaupdate_f[] = {
	{"new", lua_newpollingluaupdate},
	{NULL, NULL}
};

static const struct luaL_reg lua_pollingluaupdate_m[] = {
	{NULL, NULL}
};

int luaopen_pollingluaupdate(lua_State *L){
	luaL_newmetatable(L, "pollingluaupdate");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_pollingluaupdate_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"pollingluaupdate", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_pollingluaupdate_m);
	luaL_register(L, "pollingluaupdate", lua_pollingluaupdate_f);

	lua_pop(L, 2);
	
	return 1;
}




}

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

#include "Timer.h"
#include "../Time.h"
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

using namespace std;

namespace lua {

int lua_pushtimer(lua_State *L, uint32_t timer_);

static int lua_newtimer(lua_State *L){
	void *dataptr = lua_newuserdata(L, sizeof(uint32_t));
	uint32_t *timer = new(dataptr) uint32_t;
	luaL_getmetatable(L, "timer");
	lua_setmetatable(L, -2);
	*timer = 0;
	return 1;
}

static int lua_nowtimer(lua_State *L){
	lua_pushtimer(L, getTime());
    return 1;
}

uint32_t* lua_checktimer(lua_State *L, int index){
	void *dataptr = sm_lua_getuserdata(L, index, "timer");
	luaL_argcheck(L, dataptr != NULL, index, "`timer' expected");
	return reinterpret_cast<uint32_t*>(dataptr);
}

int lua_pushtimer(lua_State *L, uint32_t timer_){
	void *dataptr = lua_newuserdata(L, sizeof(uint32_t));
	uint32_t *timer = new(dataptr) uint32_t;
	luaL_getmetatable(L, "timer");
	lua_setmetatable(L, -2);
	*timer = timer_;
	return 1;
}

static const struct luaL_reg lua_timer_f[] = {
	{"new", lua_newtimer},
	{"now", lua_nowtimer},
	{NULL, NULL}
};

static int lua_difference(lua_State *L){
	uint32_t *timer1 = lua_checktimer(L, 1);
    uint32_t *timer2 = lua_checktimer(L, 2);
	
	lua_pushnumber(L, *timer2 - *timer1);
	return 1;
}

static int lua_now(lua_State *L){
	uint32_t *timer1 = lua_checktimer(L, 1);
	*timer1 = getTime();
	return 0;
}
 
static const struct luaL_reg lua_timer_m[] = {
	{"difference", lua_difference},
	{"now", lua_now},
	{NULL, NULL}
};

int luaopen_timer(lua_State *L){
	luaL_newmetatable(L, "timer");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	const char *derived_from[] = {"timer", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_timer_m);
	luaL_register(L, "timer", lua_timer_f);

	lua_pop(L, 2);
	
	return 1;
}


}


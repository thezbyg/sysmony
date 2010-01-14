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

PollingLuaUpdate::PollingLuaUpdate(lua_State *L_, int function_ptr_, uint32_t update_interval_, uint32_t now):PollingUpdate(update_interval_, now){
	L = L_;
	function_ptr = function_ptr_;
}

PollingLuaUpdate::~PollingLuaUpdate(){
	luaL_unref(L, LUA_REGISTRYINDEX, function_ptr);
}

void PollingLuaUpdate::update(){
	int status;
	int stack_top = lua_gettop(L);
	lua_rawgeti(L, LUA_REGISTRYINDEX, function_ptr);

	if ((status=lua_pcall(L, 0, 0, 0))==0){

	}else{
		cerr<<"PollingLuaUpdate::update: "<<lua_tostring (L, -1)<<endl;
	}

	lua_settop(L, stack_top);
}

}

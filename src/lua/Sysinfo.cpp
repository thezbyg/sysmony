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

#include "Sysinfo.h"

#include <sys/sysinfo.h>

#include <stdint.h>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

namespace lua {

static int lua_sysinfo_get(lua_State *L){
	struct sysinfo info;
	if (sysinfo(&info)==0){
		lua_newtable(L);
		int table = lua_gettop(L);

		lua_pushstring(L, "uptime");
		lua_pushnumber(L, info.uptime);
		lua_settable(L, table);

		lua_pushstring(L, "totalram");
		lua_pushnumber(L, info.totalram | (uint64_t(info.totalhigh)<<32));
		lua_settable(L, table);

		lua_pushstring(L, "freeram");
		lua_pushnumber(L, info.freeram | (uint64_t(info.freehigh)<<32));
		lua_settable(L, table);

		lua_pushstring(L, "sharedram");
		lua_pushnumber(L, info.sharedram);
		lua_settable(L, table);

		lua_pushstring(L, "bufferram");
		lua_pushnumber(L, info.bufferram);
		lua_settable(L, table);

		lua_pushstring(L, "totalswap");
		lua_pushnumber(L, info.totalswap);
		lua_settable(L, table);

		lua_pushstring(L, "freeswap");
		lua_pushnumber(L, info.freeswap);
		lua_settable(L, table);

		lua_pushstring(L, "procs");
		lua_pushnumber(L, info.procs);
		lua_settable(L, table);

		lua_pushstring(L, "loads");
		lua_newtable(L);
		int loads_table = lua_gettop(L);

		lua_pushstring(L, "1");
		lua_pushnumber(L, info.loads[0]);
		lua_settable(L, loads_table);

		lua_pushstring(L, "5");
		lua_pushnumber(L, info.loads[1]);
		lua_settable(L, loads_table);

		lua_pushstring(L, "15");
		lua_pushnumber(L, info.loads[2]);
		lua_settable(L, loads_table);

		lua_settable(L, table);

		lua_pushstring(L, "mem_unit");
		lua_pushnumber(L, info.mem_unit);
		lua_settable(L, table);

		return 1;
	}
	return 0;
}

static const struct luaL_reg lua_sysinfo_f[] = {
	{"get", lua_sysinfo_get},
	{NULL, NULL}
};

int luaopen_sysinfo(lua_State *L){
	luaL_newmetatable(L, "sysinfo");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_openlib(L, "sysinfo", lua_sysinfo_f, 0);

	return 0;
}

}

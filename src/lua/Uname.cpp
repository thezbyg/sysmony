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

#include "Uname.h"

#include <sys/utsname.h>

#include <stdint.h>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

namespace lua {

static int lua_uname_get(lua_State *L){
	struct utsname name;
	if (uname(&name)==0){
		lua_newtable(L);
		int table = lua_gettop(L);

		lua_pushstring(L, "sysname");
		lua_pushstring(L, name.sysname);
		lua_settable(L, table);

		lua_pushstring(L, "nodename");
		lua_pushstring(L, name.nodename);
		lua_settable(L, table);

		lua_pushstring(L, "release");
		lua_pushstring(L, name.release);
		lua_settable(L, table);

		lua_pushstring(L, "version");
		lua_pushstring(L, name.version);
		lua_settable(L, table);

		lua_pushstring(L, "machine");
		lua_pushstring(L, name.machine);
		lua_settable(L, table);

		return 1;
	}
	return 0;
}

static const struct luaL_reg lua_uname_f[] = {
	{"get", lua_uname_get},
	{NULL, NULL}
};

int luaopen_uname(lua_State *L){
	luaL_newmetatable(L, "uname");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_openlib(L, "uname", lua_uname_f, 0);

	return 0;
}

}

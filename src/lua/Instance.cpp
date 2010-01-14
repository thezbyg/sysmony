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

#include "Instance.h"

#include "Bindings.h"
#include "LuaUpdate.h"
#include "Derivation.h"

#include "engine_api/Render.h"

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

namespace lua {



static int lua_newrenderlib(lua_State *L){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<engine::Render>));
	shared_ptr<engine::Render> *renderlib = new(dataptr) shared_ptr<engine::Render>();
	luaL_getmetatable(L, "renderlib");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<engine::Render> lua_checkrenderlib(lua_State *L, int index){
	void *dataptr = sm_lua_getuserdata(L, index, "renderlib");
	luaL_argcheck(L, dataptr != NULL, index, "`renderlib' expected");
	return *reinterpret_cast<shared_ptr<engine::Render>*>(dataptr);
}

int lua_pushrenderlib(lua_State *L, shared_ptr<engine::Render> renderlib_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<engine::Render>));
	shared_ptr<engine::Render> *renderlib = new(dataptr) shared_ptr<engine::Render>(renderlib_);
	luaL_getmetatable(L, "renderlib");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_renderlib_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "renderlib");
	reinterpret_cast<shared_ptr<engine::Render>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_renderlib_f[] = {
	{"new", lua_newrenderlib},
	{NULL, NULL}
};

static const struct luaL_reg lua_renderlib_m[] = {
	{NULL, NULL}
};

int luaopen_renderlib(lua_State *L){
	luaL_newmetatable(L, "renderlib");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_renderlib_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"renderlib", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_openlib(L, NULL, lua_renderlib_m, 0);
	luaL_openlib(L, "renderlib", lua_renderlib_f, 0);

	return 1;
}









static int lua_newinstance(lua_State *L){
	Instance **instance = reinterpret_cast<Instance**>(lua_newuserdata(L, sizeof(Instance*)));
	*instance = 0;
	luaL_getmetatable(L, "instance");
	lua_setmetatable(L, -2);
	return 1;
}

Instance* lua_checkinstance(lua_State *L, int index){
	Instance **instance = reinterpret_cast<Instance**>(luaL_checkudata(L, index, "instance"));
	luaL_argcheck(L, instance != NULL, index, "`instance' expected");
	return *instance;
}

int lua_pushinstance(lua_State *L, Instance *instance_){
	Instance **instance = reinterpret_cast<Instance**>(lua_newuserdata(L, sizeof(Instance*)));
	*instance = instance_;
	luaL_getmetatable(L, "instance");
	lua_setmetatable(L, -2);
	return 1;
}

static const struct luaL_reg lua_instance_f[] = {
	{"new", lua_newinstance},
	{NULL, NULL}
};

static int lua_add_root_window(lua_State *L){
	Instance *instance = lua_checkinstance(L, 1);
	shared_ptr<RootWindow> rootwindow = lua_checkrootwindow(L, 2);

	instance->addRootWindow(rootwindow);

	return 0;
}

static int lua_get_render_lib(lua_State *L){
	Instance *instance = lua_checkinstance(L, 1);
	const char *name = luaL_checkstring(L, 2);

	shared_ptr<engine::Render> render = instance->getRenderLib(name);

	lua_pushrenderlib(L, render);

	return 1;
}

static const struct luaL_reg lua_instance_m[] = {
	{"add_root_window", lua_add_root_window},
	{"get_render_lib", lua_get_render_lib},
	{NULL, NULL}
};

int luaopen_instance(lua_State *L){
	luaL_newmetatable(L, "instance");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_openlib(L, NULL, lua_instance_m, 0);
	luaL_openlib(L, "instance", lua_instance_f, 0);

	return 1;
}








static int lua_newrootwindow(lua_State *L){

	shared_ptr<Window> window = lua_checkwindow(L, 2);
	shared_ptr<engine::Render> render_lib = lua_checkrenderlib(L, 3);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<RootWindow>));
	shared_ptr<RootWindow> *rootwindow = new(dataptr) shared_ptr<RootWindow>(new RootWindow(window, render_lib));
	luaL_getmetatable(L, "rootwindow");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<RootWindow> lua_checkrootwindow(lua_State *L, int index){
	void *dataptr = sm_lua_getuserdata(L, index, "rootwindow");
	luaL_argcheck(L, dataptr != NULL, index, "`rootwindow' expected");
	return *reinterpret_cast<shared_ptr<RootWindow>*>(dataptr);
}

int lua_pushrootwindow(lua_State *L, shared_ptr<RootWindow> rootwindow_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<RootWindow>));
	shared_ptr<RootWindow> *rootwindow = new(dataptr) shared_ptr<RootWindow>(rootwindow_);
	luaL_getmetatable(L, "rootwindow");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_rootwindow_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "rootwindow");
	reinterpret_cast<shared_ptr<RootWindow>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_rootwindow_f[] = {
	{"new", lua_newrootwindow},
	{NULL, NULL}
};

static int lua_add_updater(lua_State *L){
	shared_ptr<RootWindow> rootwindow = lua_checkrootwindow(L, 1);
	shared_ptr<Update> update = lua_checkpollingluaupdate(L, 2);

	rootwindow->addUpdater(update);

	return 0;
}

static const struct luaL_reg lua_rootwindow_m[] = {
	{"add_updater", lua_add_updater},
	{NULL, NULL}
};

int luaopen_rootwindow(lua_State *L){
	luaL_newmetatable(L, "rootwindow");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_rootwindow_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"rootwindow", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_openlib(L, NULL, lua_rootwindow_m, 0);
	luaL_openlib(L, "rootwindow", lua_rootwindow_f, 0);

	return 1;
}


}

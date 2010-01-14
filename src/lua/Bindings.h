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

#ifndef LUA_BINDINGS_H_
#define LUA_BINDINGS_H_

#include "layout/Window.h"
#include "layout/LayoutMain.h"
#include "layout/Vbox.h"
#include "layout/Hbox.h"
#include "layout/Label.h"
#include "layout/Alignment.h"
#include "Update.h"
#include "LuaUpdate.h"

extern "C"{
#include <lua.h>
}

#include <memory>

namespace lua {

int luaopen_all_bindingswindow(lua_State *L);

int lua_pushwindow(lua_State *L, std::shared_ptr<layout::Window> window);
std::shared_ptr<layout::Window> lua_checkwindow(lua_State *L, int index);

int lua_pushwidget(lua_State *L, std::shared_ptr<layout::Widget> widget);
std::shared_ptr<layout::Widget> lua_checkwidget(lua_State *L, int index);

int lua_pushlayout(lua_State *L, std::shared_ptr<LayoutMain> layout);
std::shared_ptr<layout::LayoutMain> lua_checklayout(lua_State *L, int index);

std::shared_ptr<layout::Label> lua_checklabel(lua_State *L, int index);
int lua_pushlabel(lua_State *L, std::shared_ptr<Label> label);

std::shared_ptr<PollingLuaUpdate> lua_checkpollingluaupdate(lua_State *L, int index);
int lua_pushpollingluaupdate(lua_State *L, std::shared_ptr<PollingLuaUpdate> pollingluaupdate);

}

#endif /* LUA_BINDINGS_H_ */

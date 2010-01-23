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

#include "Bindings.h"
#include "LuaUpdate.h"
#include "Derivation.h"
#include "Style.h"
#include "Time.h"

#include "layout/Separator.h"
#include "layout/Image.h"
#include "layout/Table.h"
#include "layout/Graph.h"

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
using namespace layout;

namespace lua {


static int lua_newwidget(lua_State *L){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Widget>));
	new(dataptr) shared_ptr<Widget>(new Widget());
	luaL_getmetatable(L, "widget");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Widget> lua_checkwidget(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "widget"))){
		return *reinterpret_cast<shared_ptr<Widget>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`widget' expected");
	return shared_ptr<Widget>();
}

int lua_pushwidget(lua_State *L, shared_ptr<Widget> widget_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Widget>));
	new(dataptr) shared_ptr<Widget>(widget_);
	luaL_getmetatable(L, "widget");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_widget_gc(lua_State *L){
	void *dataptr = luaL_checkudata(L, 1, "widget");
	reinterpret_cast<shared_ptr<Widget>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_widget_f[] = {
	{"new", lua_newwidget},
	{NULL, NULL}
};


static int lua_add_style(lua_State *L){
	shared_ptr<Widget> widget = lua_checkwidget(L, 1);
	shared_ptr<engine::Style> style = lua_checkstyle(L, 2);
	widget->addStyle(style);
	return 0;
}

static int lua_set_style_mask(lua_State *L){
	shared_ptr<Widget> widget = lua_checkwidget(L, 1);
	widget->setStyleMask(luaL_checkinteger(L, 2));
	return 0;
}

static int lua_min_size(lua_State *L){
	shared_ptr<Widget> widget = lua_checkwidget(L, 1);
	if ((lua_type(L, 2) == LUA_TNUMBER) && (lua_type(L, 3) == LUA_TNUMBER)){
		widget->setMinRequisition(Vector2<double>(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}else{
		Vector2<double> min_req = widget->getMinRequisition();
        lua_pushnumber(L, min_req.x);
		lua_pushnumber(L, min_req.y);
		return 2;
	}
	return 0;
}

static const struct luaL_reg lua_widget_m[] = {
	{"add_style", lua_add_style},
	{"set_style_mask", lua_set_style_mask},
	{"min_size", lua_min_size},
	{NULL, NULL}
};

static int luaopen_widget(lua_State *L){
	luaL_newmetatable(L, "widget");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_widget_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, "widget", lua_widget_f);
	
	lua_pop(L, 2);

	return 1;
}








 

static int lua_newgraph(lua_State *L){

	double lo = luaL_checknumber(L, 2);
	double hi = luaL_checknumber(L, 3);
	uint32_t data_size = luaL_checkinteger(L, 4);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Graph>));
	new(dataptr) shared_ptr<Graph>(new Graph(lo, hi, data_size));
	luaL_getmetatable(L, "graph");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Graph> lua_checkgraph(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "graph"))){
		return *reinterpret_cast<shared_ptr<Graph>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`graph' expected");
	return shared_ptr<Graph>();
}

int lua_pushgraph(lua_State *L, shared_ptr<Graph> graph_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Graph>));
	new(dataptr) shared_ptr<Graph>(graph_);
	luaL_getmetatable(L, "graph");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_graph_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "graph");
	reinterpret_cast<shared_ptr<Graph>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_graph_f[] = {
	{"new", lua_newgraph},
	{NULL, NULL}
};


static int lua_graph_push_value(lua_State *L){
	shared_ptr<Graph> graph = lua_checkgraph(L, 1);

	if (lua_type(L, 2)==LUA_TNUMBER){
		graph->pushValue(lua_tonumber(L, 2));
		return 0;
	}
	return 0;
}

static int lua_graph_logarithmic_scale(lua_State *L){
	shared_ptr<Graph> graph = lua_checkgraph(L, 1);
                                                                  
	if (lua_type(L, 2)==LUA_TNUMBER && lua_type(L, 3)==LUA_TNUMBER){
		graph->setLogarithmicScale(lua_tonumber(L, 2), lua_tonumber(L, 3));
		return 0;
	}
	return 0;
}


static const struct luaL_reg lua_graph_m[] = {
	{"push_value", lua_graph_push_value},
	{"logarithmic_scale", lua_graph_logarithmic_scale},
	{NULL, NULL}
};

static int luaopen_graph(lua_State *L){
	luaL_newmetatable(L, "graph");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_graph_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "graph", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_graph_m);
	luaL_register(L, "graph", lua_graph_f);

	lua_pop(L, 2);
	
	return 1;
}
 









static int lua_newlabel(lua_State *L){

	const char *text = luaL_checkstring(L, 2);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Label>));
	new(dataptr) shared_ptr<Label>(new Label(text));
	luaL_getmetatable(L, "label");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Label> lua_checklabel(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "label"))){
		return *reinterpret_cast<shared_ptr<Label>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`label' expected");
	return shared_ptr<Label>();
}

int lua_pushlabel(lua_State *L, shared_ptr<Label> label_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Label>));
	new(dataptr) shared_ptr<Label>(label_);
	luaL_getmetatable(L, "label");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_label_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "label");
	reinterpret_cast<shared_ptr<Label>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_label_f[] = {
	{"new", lua_newlabel},
	{NULL, NULL}
};


static int lua_label_text(lua_State *L){
	shared_ptr<Label> label = lua_checklabel(L, 1);

	if (lua_type(L, 2)==LUA_TSTRING){
		const char *text = luaL_checkstring(L, 2);
		label->setText(text);
		return 0;
	}else{
		lua_pushstring(L, label->getText());
		return 1;
	}
}

static int lua_label_align(lua_State *L){
	shared_ptr<Label> label = lua_checklabel(L, 1);

	if (lua_type(L, 2)==LUA_TNUMBER){
		label->setAlignment(static_cast<Label::Align>(luaL_checkinteger(L, 2)));
		return 0;
	}else{
		lua_pushnumber(L, label->getAlignment());
		return 1;
	}
}

static const struct luaL_reg lua_label_m[] = {
	{"text", lua_label_text},
	{"align", lua_label_align},
	{NULL, NULL}
};

static int luaopen_label(lua_State *L){
	luaL_newmetatable(L, "label");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_label_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "label", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_label_m);
	luaL_register(L, "label", lua_label_f);

 	lua_pushstring(L, "Left");
	lua_pushinteger(L, Label::Align::Left);
	lua_settable(L, -3);
   	lua_pushstring(L, "Center");
	lua_pushinteger(L, Label::Align::Center);
	lua_settable(L, -3);
 	lua_pushstring(L, "Right");
	lua_pushinteger(L, Label::Align::Right);
	lua_settable(L, -3);
 


	lua_pop(L, 2);
	
	return 1;
}



























static int lua_newimage(lua_State *L){

	const char *filename = luaL_checkstring(L, 2);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Image>));
	new(dataptr) shared_ptr<Image>(new Image(filename));
	luaL_getmetatable(L, "image");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Image> lua_checkimage(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "image"))){
		return *reinterpret_cast<shared_ptr<Image>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`image' expected");
	return shared_ptr<Image>();
}

int lua_pushimage(lua_State *L, shared_ptr<Image> image_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Image>));
	new(dataptr) shared_ptr<Image>(image_);
	luaL_getmetatable(L, "image");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_image_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "image");
	reinterpret_cast<shared_ptr<Image>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_image_f[] = {
	{"new", lua_newimage},
	{NULL, NULL}
};


static int lua_image_filename(lua_State *L){
	shared_ptr<Image> image = lua_checkimage(L, 1);

	if (lua_type(L, 2) == LUA_TSTRING){
		const char *filename= luaL_checkstring(L, 2);
		image->setImageFilename(filename);
		return 0;
	}else{
		//lua_pushstring(L, image->getText());
		return 0;
	}
}

static int lua_image_mask(lua_State *L){
	shared_ptr<Image> image = lua_checkimage(L, 1);

	if (lua_type(L, 2) == LUA_TBOOLEAN){
    	image->setImageAsMask(lua_toboolean(L, 2));
		return 0;
	}else{
        lua_pushboolean(L, image->getImageAsMask());
        return 1;
	}
}

static const struct luaL_reg lua_image_m[] = {
	{"filename", lua_image_filename},
	{"mask", lua_image_mask},
	{NULL, NULL}
};

static int luaopen_image(lua_State *L){
	luaL_newmetatable(L, "image");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_image_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "image", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_image_m);
	luaL_register(L, "image", lua_image_f);

	lua_pop(L, 2);
	
	return 1;
}







static int lua_newseparator(lua_State *L){

	bool horizontal = lua_toboolean(L, 2);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Separator>));
	new(dataptr) shared_ptr<Separator>(new Separator(horizontal));
	luaL_getmetatable(L, "separator");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Separator> lua_checkseparator(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "separator"))){
		return *reinterpret_cast<shared_ptr<Separator>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`separator' expected");
	return shared_ptr<Separator>();
}

int lua_pushseparator(lua_State *L, shared_ptr<Separator> separator_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Separator>));
	new(dataptr) shared_ptr<Separator>(separator_);
	luaL_getmetatable(L, "separator");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_separator_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "separator");
	reinterpret_cast<shared_ptr<Separator>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_separator_f[] = {
	{"new", lua_newseparator},
	{NULL, NULL}
};

static const struct luaL_reg lua_separator_m[] = {
	{NULL, NULL}
};

static int luaopen_separator(lua_State *L){
	luaL_newmetatable(L, "separator");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_separator_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "separator", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_separator_m);
	luaL_register(L, "separator", lua_separator_f);

	lua_pop(L, 2);
	
	return 1;
}








static int lua_newcontainer(lua_State *L){

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Container>));
	new(dataptr) shared_ptr<Container>(new Container());
	luaL_getmetatable(L, "container");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Container> lua_checkcontainer(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "container"))){
		return *reinterpret_cast<shared_ptr<Container>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`container' expected");
	return shared_ptr<Container>();
}

int lua_pushcontainer(lua_State *L, shared_ptr<Container> container_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Container>));
	new(dataptr) shared_ptr<Container>(container_);
	luaL_getmetatable(L, "container");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_container_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "container");
	reinterpret_cast<shared_ptr<Container>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_container_f[] = {
	{"new", lua_newcontainer},
	{NULL, NULL}
};


static int lua_container_add_widget(lua_State *L){
	shared_ptr<Container> container = lua_checkcontainer(L, 1);
	shared_ptr<Widget> widget = lua_checkwidget(L, 2);
	container->addWidget(widget);
	return 0;
}

static int lua_container_padding(lua_State *L){
	shared_ptr<Container> container = lua_checkcontainer(L, 1);

	if (lua_type(L, 2)==LUA_TNUMBER && lua_type(L, 3)==LUA_TNUMBER){
		double x = luaL_checknumber(L, 2);
		double y = luaL_checknumber(L, 3);
		container->setPadding(Vector2<double>(x, y));
		return 0;
	}else{
		Vector2<double> padding = container->getPadding();
		lua_pushnumber(L, padding.x);
		lua_pushnumber(L, padding.y);
		return 2;
	}

}


static const struct luaL_reg lua_container_m[] = {
	{"add_widget", lua_container_add_widget},
	{"padding", lua_container_padding},
	{NULL, NULL}
};

static int luaopen_container(lua_State *L){
	luaL_newmetatable(L, "container");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_container_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "container", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_container_m);
	luaL_register(L, "container", lua_container_f);

	lua_pop(L, 2);
	
	return 1;
}







static int lua_newsm_table(lua_State *L){

	double h_spacing = luaL_checknumber(L, 2);
	double v_spacing = luaL_checknumber(L, 3);
	bool homogenous_x = lua_toboolean(L, 4);
	bool homogenous_y = lua_toboolean(L, 5);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Table>));
	new(dataptr) shared_ptr<Table>(new Table(Vector2<double>(h_spacing, v_spacing), Vector2<bool>(homogenous_x, homogenous_y)));
	luaL_getmetatable(L, "sm_table");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Table> lua_checksm_table(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "sm_table"))){
		return *reinterpret_cast<shared_ptr<Table>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`sm_table' expected");
	return shared_ptr<Table>();
}

int lua_pushsm_table(lua_State *L, shared_ptr<Table> sm_table_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Table>));
	new(dataptr) shared_ptr<Table>(sm_table_);
	luaL_getmetatable(L, "sm_table");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_sm_table_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "sm_table");
	reinterpret_cast<shared_ptr<Table>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_sm_table_f[] = {
	{"new", lua_newsm_table},
	{NULL, NULL}
};


static int lua_sm_table_addwidget(lua_State *L){
	shared_ptr<Table> sm_table = lua_checksm_table(L, 1);

	shared_ptr<Widget> widget = lua_checkwidget(L, 2);
	uint32_t x1 = luaL_checkinteger(L, 3);
	uint32_t y1 = luaL_checkinteger(L, 4);
	uint32_t x2 = luaL_checkinteger(L, 5);
	uint32_t y2 = luaL_checkinteger(L, 6);

	Table::AttachOptions x_options = Table::AttachOptions(luaL_checkinteger(L, 7));
	Table::AttachOptions y_options = Table::AttachOptions(luaL_checkinteger(L, 8));

	double spacing = luaL_checknumber(L, 9);

	sm_table->addWidget(widget, x1, y1, x2, y2, x_options, y_options, spacing);

	return 0;
}


static const struct luaL_reg lua_sm_table_m[] = {
	{"addwidget", lua_sm_table_addwidget},
	{NULL, NULL}
};

static int luaopen_sm_table(lua_State *L){
	luaL_newmetatable(L, "sm_table");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_sm_table_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "container", "sm_table", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_container_m);
	luaL_register(L, NULL, lua_sm_table_m);
	luaL_register(L, "sm_table", lua_sm_table_f);

	lua_pushstring(L, "Expand");
	lua_pushnumber(L, Table::AttachOptions::Expand);
	lua_settable(L, -3);

	lua_pushstring(L, "Fill");
	lua_pushnumber(L, Table::AttachOptions::Fill);
	lua_settable(L, -3);

	lua_pop(L, 2);
	
	return 1;
}




static int lua_newbox(lua_State *L){

	double spacing = luaL_checknumber(L, 2);
	bool homogenous = lua_toboolean(L, 3);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Box>));
	new(dataptr) shared_ptr<Box>(new Box(spacing, homogenous));
	luaL_getmetatable(L, "box");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Box> lua_checkbox(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "box"))){
		return *reinterpret_cast<shared_ptr<Box>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`box' expected");
	return shared_ptr<Box>();
}

int lua_pushbox(lua_State *L, shared_ptr<Box> box_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Box>));
	new(dataptr) shared_ptr<Box>(box_);
	luaL_getmetatable(L, "box");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_box_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "box");
	reinterpret_cast<shared_ptr<Box>*>(dataptr)->~shared_ptr();;
	return 0;
}

static const struct luaL_reg lua_box_f[] = {
	{"new", lua_newbox},
	{NULL, NULL}
};


static int lua_box_add_widget(lua_State *L){
	shared_ptr<Box> box = lua_checkbox(L, 1);
	shared_ptr<Widget> widget = lua_checkwidget(L, 2);
	bool fill = lua_toboolean(L, 3);
	bool expand = lua_toboolean(L, 4);

	box->addWidget(widget, fill, expand);
	return 0;
}


static const struct luaL_reg lua_box_m[] = {
	{"add_widget", lua_box_add_widget},
	{NULL, NULL}
};

static int luaopen_box(lua_State *L){
	luaL_newmetatable(L, "box");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_box_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "container", "box", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_container_m);
	luaL_register(L, NULL, lua_box_m);
	luaL_register(L, "box", lua_box_f);
	
	lua_pop(L, 2);

	return 1;
}





static int lua_newvbox(lua_State *L){

	double spacing = luaL_checknumber(L, 2);
	bool homogenous = lua_toboolean(L, 3);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Vbox>));
	new(dataptr) shared_ptr<Vbox>(new Vbox(spacing, homogenous));
	luaL_getmetatable(L, "vbox");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Vbox> lua_checkvbox(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "vbox"))){
		return *reinterpret_cast<shared_ptr<Vbox>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`vbox' expected");
	return shared_ptr<Vbox>();
}

int lua_pushvbox(lua_State *L, shared_ptr<Vbox> vbox_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Vbox>));
	new(dataptr) shared_ptr<Vbox>(vbox_);
	luaL_getmetatable(L, "vbox");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_vbox_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "vbox");
	reinterpret_cast<shared_ptr<Vbox>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_vbox_f[] = {
	{"new", lua_newvbox},
	{NULL, NULL}
};

static const struct luaL_reg lua_vbox_m[] = {
	{NULL, NULL}
};

static int luaopen_vbox(lua_State *L){
	luaL_newmetatable(L, "vbox");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_vbox_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "container", "box", "vbox", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_container_m);
	luaL_register(L, NULL, lua_box_m);
	luaL_register(L, NULL, lua_vbox_m);
	luaL_register(L, "vbox", lua_vbox_f);

	lua_pop(L, 2);
	
	return 1;
}







static int lua_newhbox(lua_State *L){

	double spacing = luaL_checknumber(L, 2);
	bool homogenous = lua_toboolean(L, 3);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Hbox>));
	new(dataptr) shared_ptr<Hbox>(new Hbox(spacing, homogenous));
	luaL_getmetatable(L, "hbox");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Hbox> lua_checkhbox(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "hbox"))){
		return *reinterpret_cast<shared_ptr<Hbox>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`hbox' expected");
	return shared_ptr<Hbox>();
}

int lua_pushhbox(lua_State *L, shared_ptr<Hbox> hbox_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Hbox>));
	new(dataptr) shared_ptr<Hbox>(hbox_);
	luaL_getmetatable(L, "hbox");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_hbox_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "hbox");
	reinterpret_cast<shared_ptr<Hbox>*>(dataptr)->~shared_ptr();;
	return 0;
}

static const struct luaL_reg lua_hbox_f[] = {
	{"new", lua_newhbox},
	{NULL, NULL}
};

static const struct luaL_reg lua_hbox_m[] = {
	{NULL, NULL}
};

static int luaopen_hbox(lua_State *L){
	luaL_newmetatable(L, "hbox");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_hbox_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "container", "box", "hbox", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_container_m);
	luaL_register(L, NULL, lua_box_m);
	luaL_register(L, NULL, lua_hbox_m);
	luaL_register(L, "hbox", lua_hbox_f);

	lua_pop(L, 2);
	
	return 1;
}







static int lua_newalignment(lua_State *L){

	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Alignment>));
	new(dataptr) shared_ptr<Alignment>(new Alignment(Vector2<double>(x, y)));
	luaL_getmetatable(L, "alignment");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Alignment> lua_checkalignment(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "alignment"))){
		return *reinterpret_cast<shared_ptr<Alignment>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`alignment' expected");
	return shared_ptr<Alignment>();
}

int lua_pushalignment(lua_State *L, shared_ptr<Alignment> alignment_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Alignment>));
	new(dataptr) shared_ptr<Alignment>(alignment_);
	luaL_getmetatable(L, "alignment");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_alignment_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "alignment");
	reinterpret_cast<shared_ptr<Alignment>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_alignment_f[] = {
	{"new", lua_newalignment},
	{NULL, NULL}
};

static int lua_alignment_alignment(lua_State *L){
	shared_ptr<Alignment> alignment = lua_checkalignment(L, 1);

	if (lua_type(L, 2)==LUA_TNUMBER && lua_type(L, 3)==LUA_TNUMBER){
		double x = luaL_checknumber(L, 2);
		double y = luaL_checknumber(L, 3);
		alignment->setAlignment(Vector2<double>(x, y));
		return 0;
	}else{
		Vector2<double> align = alignment->getAlignment();
		lua_pushnumber(L, align.x);
		lua_pushnumber(L, align.y);
		return 2;
	}
}

static const struct luaL_reg lua_alignment_m[] = {
	{"alignment", lua_alignment_alignment},
	{NULL, NULL}
};

static int luaopen_alignment(lua_State *L){
	luaL_newmetatable(L, "alignment");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_alignment_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "container", "alignment", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_container_m);
	luaL_register(L, NULL, lua_alignment_m);
	luaL_register(L, "alignment", lua_alignment_f);

	lua_pop(L, 2);
	
	return 1;
}





static int lua_newwindow(lua_State *L){

	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Window>));
	new(dataptr) shared_ptr<Window>(new Window());
	luaL_getmetatable(L, "window");
	lua_setmetatable(L, -2);
	return 1;
}

shared_ptr<Window> lua_checkwindow(lua_State *L, int index){
	void *dataptr;
	if ((dataptr = sm_lua_getuserdata(L, index, "window"))){
		return *reinterpret_cast<shared_ptr<Window>*>(dataptr);
	}
	luaL_argcheck(L, false, index, "`window' expected");
	return shared_ptr<Window>();
}

int lua_pushwindow(lua_State *L, shared_ptr<Window> window_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<Window>));
	new(dataptr) shared_ptr<Window>(window_);
	luaL_getmetatable(L, "window");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_window_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "window");
	reinterpret_cast<shared_ptr<Window>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_window_f[] = {
	{"new", lua_newwindow},
	{NULL, NULL}
};


static int lua_window_resize(lua_State *L){
	shared_ptr<Window> window = lua_checkwindow(L, 1);
	double width = luaL_checknumber(L, 2);
	double height = luaL_checknumber(L, 3);
	window->resize(Vector2<double>(width, height));
	return 0;
}

static int lua_window_set_position(lua_State *L){
	shared_ptr<Window> window = lua_checkwindow(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	window->setPosition(Vector2<double>(x, y));
	return 0;
}


static const struct luaL_reg lua_window_m[] = {
	{"resize", lua_window_resize},
	{"set_position", lua_window_set_position},
	{NULL, NULL}
};

static int luaopen_window(lua_State *L){
	luaL_newmetatable(L, "window");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_window_gc);
	lua_settable(L, -3);

	const char *derived_from[] = {"widget", "container", "window", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_register(L, NULL, lua_widget_m);
	luaL_register(L, NULL, lua_container_m);
	luaL_register(L, NULL, lua_window_m);
	luaL_register(L, "window", lua_window_f);

	lua_pop(L, 2);
	
	return 1;
}









int luaopen_all_bindingswindow(lua_State *L){
	luaopen_widget(L);
	luaopen_label(L);
	luaopen_window(L);
	luaopen_box(L);
	luaopen_vbox(L);
	luaopen_hbox(L);
	luaopen_container(L);
	luaopen_alignment(L);
	luaopen_separator(L);
	luaopen_image(L);
	luaopen_graph(L);
	luaopen_sm_table(L);
	return 0;
}

}

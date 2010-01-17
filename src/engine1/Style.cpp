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
#include "lua/Derivation.h"
#include "lua/Color.h"

#include "layout/Window.h"
#include "layout/Separator.h"

#include <functional>
#include <list>
#include <algorithm>
#include <iostream>

#include <math.h>
#include <typeinfo>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

using namespace std;
using namespace engine;
using namespace layout;
using namespace lua;

RenderStyle::RenderStyle(){
	clear();
}

void RenderStyle::clear(){
	text_color_set = Undefined;
	separator_color_set = Undefined;
	window_color_set = Undefined;
	secondary_window_color_set = Undefined;
	font_size_set = Undefined;
	font_weight_set = Undefined;
	font_name_set = Undefined;
	window_border_color_set = Undefined;
}

RenderStyle::~RenderStyle(){
	
}

int RenderStyle::apply(Style *style){
	return apply_d(*dynamic_cast<RenderStyle*>(style));
}

int RenderStyle::apply_d(RenderStyle &style){
	if (text_color_set == Undefined && style.text_color_set != Undefined){
		text_color = style.text_color;
		text_color_set = Inherited;
	}
	
	if (separator_color_set == Undefined && style.separator_color_set != Undefined){
		separator_color = style.separator_color;
		separator_color_set = Inherited;
	}
	
	if (window_color_set == Undefined && style.window_color_set != Undefined){
		window_color = style.window_color;
		window_color_set = Inherited;
	}
	
	if (secondary_window_color_set == Undefined && style.secondary_window_color_set != Undefined){
		secondary_window_color = style.secondary_window_color;
		secondary_window_color_set = Inherited;
	}
	
	if (font_size_set == Undefined && style.font_size_set != Undefined){
		font_size = style.font_size;
		font_size_set = Inherited;
	}
	
	if (font_weight_set == Undefined && style.font_weight_set != Undefined){
		font_weight = style.font_weight;
		font_weight_set = Inherited;
	}
	
	if (font_name_set == Undefined && style.font_name_set != Undefined){
		font_name = style.font_name;
		font_name_set = Inherited;
	}
	
	if (window_border_color_set == Undefined && style.window_border_color_set != Undefined){
		window_border_color = style.window_border_color;
		window_border_color_set = Inherited;
	}
	
	return 0;
}


static void cairo_rounded_rectangle(cairo_t *cr, double x, double y, double width, double height, double roundness, double strength){

	cairo_move_to(cr, x+roundness, y);
	cairo_line_to(cr, x+width-roundness, y);
	cairo_arc(cr, x+width-roundness, y+roundness, roundness, -M_PI/2, 0);
	//cairo_curve_to(cr, x+width-roundness*strength, y, x+width, y+roundness*strength, x+width, y+roundness);
	cairo_line_to(cr, x+width, y+height-roundness);
	cairo_arc(cr, x+width-roundness, y+height-roundness, roundness, 0, M_PI/2);
	//cairo_curve_to(cr, x+width, y+height-roundness*strength, x+width-roundness*strength, y+height, x+width-roundness, y+height);
	cairo_line_to(cr, x+roundness, y+height);
	cairo_arc(cr, x+roundness, y+height-roundness, roundness, M_PI/2, M_PI);
	//cairo_curve_to(cr, x+roundness*strength, y+height, x, y+height-roundness*strength, x, y+height-roundness);
	cairo_line_to(cr, x, y+roundness);
	cairo_arc(cr, x+roundness, y+roundness, roundness, M_PI, -M_PI/2);
	//cairo_curve_to(cr, x, y+roundness*strength, x+roundness*strength, y, x+roundness, y);
	cairo_close_path (cr);

}

const layout::Vector2<double> RenderStyle::getThickness(layout::Widget *widget){
	return layout::Vector2<double>(0, 0);
}

void RenderStyle::predraw(layout::Widget *widget, DrawContext *draw_context){
	
	cairo_t *cr = draw_context->getCairo();
	Rect2<double> allocation = widget->getAllocation();
	
	if (typeid(*widget) == typeid(layout::Window)){
	
		cairo_rounded_rectangle(cr, 1.2, 1.2, allocation.width-2.4, allocation.height-2.4, 6, 0.5);
		
		cairo_set_source_rgba(cr, 0, 0, 0, 0.6);
		cairo_set_line_width(cr, 3);
		cairo_stroke_preserve(cr);
		
		cairo_pattern_t *linpat;
		linpat = cairo_pattern_create_linear(0, 0, 0, allocation.height);
		cairo_pattern_add_color_stop_rgba(linpat, 0, window_color.r, window_color.g, window_color.b, window_color.a);
		cairo_pattern_add_color_stop_rgba(linpat, 1, secondary_window_color.r, secondary_window_color.g, secondary_window_color.b, secondary_window_color.a);
		cairo_set_source(cr, linpat);

		cairo_fill_preserve(cr);
		cairo_pattern_destroy(linpat);
		

		cairo_set_source_rgba(cr, window_border_color.r, window_border_color.g, window_border_color.b, window_border_color.a);
		cairo_set_line_width(cr, 0.5);
		cairo_stroke(cr);
		
	}else if (typeid(*widget) == typeid(layout::Separator)){
		
		cairo_new_path(cr);
		if (dynamic_cast<layout::Separator*>(widget)->isHorizontal()){
			cairo_rectangle(cr, 0, allocation.height/2-1, allocation.width, 1);
		}else{
			cairo_rectangle(cr, allocation.width/2-1, 0, 1, allocation.height);
		}
		cairo_set_source_rgba(cr, separator_color.r, separator_color.g, separator_color.b, separator_color.a);
		cairo_fill(cr);
	
	}
	
}

void RenderStyle::postdraw(layout::Widget *widget, DrawContext *draw_context){
	
}


const layout::Color RenderStyle::getTextColor(layout::Widget *widget){
	return text_color;
}

double RenderStyle::getFontSize(layout::Widget *widget){
	if (font_size_set == Undefined) return 9;
	return font_size;
}

const char* RenderStyle::getFontName(layout::Widget *widget){
	if (font_name_set == Undefined) return "Sans";
	return font_name.c_str();
}

double RenderStyle::getFontWeight(layout::Widget *widget){
	if (font_weight_set == Undefined) return 400;
	return font_weight;
}

Style* RenderStyle::clone() const{
	RenderStyle *n = new RenderStyle(*this);
	return n;
}

















static int lua_newrenderstyle(lua_State *L){
	
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<RenderStyle>));
	shared_ptr<RenderStyle> *renderstyle = new(dataptr) shared_ptr<RenderStyle>(new RenderStyle); 
	luaL_getmetatable(L, "renderstyle");
	lua_setmetatable(L, -2);
	
	RenderStyle *style = renderstyle->get();
	
	if (lua_type(L, 2) == LUA_TTABLE){

		lua_getfield(L, 2, "apply_mask");
		if (lua_type(L, -1) != LUA_TNIL) style->setMask(lua_tointeger(L, -1));
		lua_pop(L, 1);
		
		lua_getfield(L, 2, "text_color");
		if (lua_type(L, -1) != LUA_TNIL){
			style->text_color = *lua_checkcolor(L, -1).get();
			style->text_color_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);	
		
		lua_getfield(L, 2, "separator_color");
		if (lua_type(L, -1) != LUA_TNIL){
			style->separator_color = *lua_checkcolor(L, -1).get();
			style->separator_color_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);	

		lua_getfield(L, 2, "window_color");
		if (lua_type(L, -1) != LUA_TNIL){
			style->window_color = *lua_checkcolor(L, -1).get();
			style->window_color_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);
		
		lua_getfield(L, 2, "secondary_window_color");
		if (lua_type(L, -1) != LUA_TNIL){
			style->secondary_window_color = *lua_checkcolor(L, -1).get();
			style->secondary_window_color_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);	
		
		lua_getfield(L, 2, "font_size");
		if (lua_type(L, -1) != LUA_TNIL){
			style->font_size = luaL_checknumber(L, -1);
			style->font_size_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);	
		
		lua_getfield(L, 2, "font_weight");
		if (lua_type(L, -1) != LUA_TNIL){
			style->font_weight = luaL_checknumber(L, -1);
			style->font_weight_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);
		
		lua_getfield(L, 2, "font_name");
		if (lua_type(L, -1) != LUA_TNIL){
			style->font_name = lua_tostring(L, -1);
			style->font_name_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);
		
		lua_getfield(L, 2, "window_border_color");
		if (lua_type(L, -1) != LUA_TNIL){
			style->window_border_color = *lua_checkcolor(L, -1).get();
			style->window_border_color_set = RenderStyle::Defined;
		}
		lua_pop(L, 1);	
	}
	
	return 1;
}

shared_ptr<RenderStyle> lua_checkrenderstyle(lua_State *L, int index){
	void *dataptr = sm_lua_getuserdata(L, index, "renderstyle");
	luaL_argcheck(L, dataptr != NULL, index, "`renderstyle' expected");
	return *reinterpret_cast<shared_ptr<RenderStyle>*>(dataptr);
}

int lua_pushrenderstyle(lua_State *L, shared_ptr<RenderStyle> renderstyle_){
	void *dataptr = lua_newuserdata(L, sizeof(shared_ptr<RenderStyle>));
	shared_ptr<RenderStyle> *renderstyle = new(dataptr) shared_ptr<RenderStyle>(renderstyle_); 
	luaL_getmetatable(L, "renderstyle");
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_renderstyle_gc(lua_State *L){
	void *dataptr= luaL_checkudata(L, 1, "renderstyle");
	reinterpret_cast<shared_ptr<RenderStyle>*>(dataptr)->~shared_ptr();
	return 0;
}

static const struct luaL_reg lua_renderstyle_f[] = {
	{"new", lua_newrenderstyle},
	{NULL, NULL}
};

static int lua_text_color(lua_State *L){
	shared_ptr<RenderStyle> render_style = lua_checkrenderstyle(L, 1);
	lua_pushcolor(L, shared_ptr<Color>(new Color(render_style->text_color)));
	return 1;
}

static const struct luaL_reg lua_renderstyle_m[] = {
	{"text_color", lua_text_color},
	{NULL, NULL}
};

int luaopen_renderstyle(lua_State *L){
	luaL_newmetatable(L, "renderstyle");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_renderstyle_gc);
	lua_settable(L, -3);
	
	const char *derived_from[] = {"style", "renderstyle", 0};
	sm_lua_add_derivation_info(L, derived_from);

	luaL_openlib(L, NULL, lua_renderstyle_m, 0);
	luaL_openlib(L, "renderstyle", lua_renderstyle_f, 0);
	
	lua_pop(L, 2);
		
	return 1;
}




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

#ifndef ENGINE1_STYLE_H_
#define ENGINE1_STYLE_H_

#include "engine_api/Style.h"

#include <string>

extern "C"{
#include <lua.h>
}

class RenderStyle:public engine::Style{
protected:


public:
	RenderStyle();
	virtual ~RenderStyle();

	virtual int apply(engine::Style *style);

	int apply_d(RenderStyle &style);

	virtual const layout::Vector2<double> getThickness(layout::Widget *widget);

	virtual void predraw(layout::Widget *widget, layout::DrawContext *draw_context);
	virtual void postdraw(layout::Widget *widget, layout::DrawContext *draw_context);

	virtual const layout::Color getTextColor(layout::Widget *widget);
	virtual double getFontSize(layout::Widget *widget);
	virtual const char* getFontName(layout::Widget *widget);
	virtual double getFontWeight(layout::Widget *widget);

	virtual Style* clone() const;
	virtual void clear();

public:
	
	layout::Color text_color;
	engine::Style::VariableState text_color_set;

	layout::Color separator_color;
	engine::Style::VariableState separator_color_set;

	layout::Color window_color;
	engine::Style::VariableState window_color_set;

	layout::Color secondary_window_color;
	engine::Style::VariableState secondary_window_color_set;

	layout::Color window_border_color;
	engine::Style::VariableState window_border_color_set;

	double font_size;
	engine::Style::VariableState font_size_set;

	double font_weight;
	engine::Style::VariableState font_weight_set;

	std::string font_name;
	engine::Style::VariableState font_name_set;

};

int luaopen_renderstyle(lua_State *L);

#endif /* ENGINE1_STYLE_H_ */

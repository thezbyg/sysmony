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

#include <functional>
#include <list>
#include <algorithm>

using namespace std;
using namespace layout;

namespace engine {

Style::Style(){
	apply_mask[0] = 0;
}

Style::~Style(){
	
}

bool Style::checkMask(const uint64_t *widget_mask) const{
	if (widget_mask[0] && apply_mask[0]){
		return ((widget_mask[0] & apply_mask[0]) != 0);
	}else{
		return true;
	}
}

void Style::setMask(uint64_t apply_mask_){
	apply_mask[0] = apply_mask_;
}

int Style::apply(Style *style){
	
	return 0;
}


const layout::Vector2<double> Style::getThickness(layout::Widget *widget){
	return layout::Vector2<double>(0, 0);
}

void Style::predraw(layout::Widget *widget, DrawContext *draw_context){
	
}

void Style::postdraw(layout::Widget *widget, DrawContext *draw_context){
	
}


const layout::Color Style::getTextColor(layout::Widget *widget){
	return layout::Color(0, 0, 0, 1);
}

double Style::getFontSize(layout::Widget *widget){
	return 0;
}

const char* Style::getFontName(layout::Widget *widget){
	return "";
}

double Style::getFontWeight(layout::Widget *widget){
	return 0;
}

Style* Style::clone() const{
	Style *n = new Style(*this);
	return n;
}

void Style::clear(){
	
}

}

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

#include "Widget.h"
#include "Window.h"
#include "StdEvents.h"

#include <stdio.h>
#include <iostream>

using namespace std;
using namespace engine;

namespace layout {

Widget::Widget(){
	visible = false;
	requisition.set(0, 0);
	configured = false;
	allocated = false;
	dirty = true;
	style_built = false;

	parent = 0;
	style_mask[0] = 0;
}

Widget::~Widget(){
}

void Widget::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){
	dirty = false;
}

void Widget::configure(){
	configured = true;
}

void Widget::build_style(){
	style_built = true;
}

const Rect2<double>& Widget::getAllocation() const{
	return allocation;
}

void Widget::setAllocation(const Rect2<double>& allocation_){
	allocation = allocation_;
}

const Vector2<double>& Widget::getRequisition() const{
	return requisition;
}

void Widget::setRequisition(const Vector2<double>& requisition_){
	requisition = requisition_;
}

void Widget::removeChild(shared_ptr<Widget> widget){
}

void Widget::markDirty(){
	dirty = true;
}

void Widget::markUnconfigured(){
	configured = false;
}

void Widget::markUnallocated(){
	allocated = false;
}

void Widget::markStyleDirty(){
	style_built = false;
}

Widget* Widget::getParent() const{
	return parent;
}

Widget* Widget::getTopLevel() const{
	Widget* p = const_cast<Widget*>(this);

	while (p->parent){
		p = p->parent;
	}

	return p;
}

Window* Widget::getTopLevelWindow() const{
	try{
		return dynamic_cast<Window*>(getTopLevel());
	}catch(std::bad_cast){
		return 0;
	}
}

void Widget::reparent(const Widget* widget){
	parent = const_cast<Widget*>(widget);
}

void Widget::allocate(){
	allocated = true;
}


void Widget::addStyle(std::shared_ptr<Style> style_){
	style.push_back(style_);
}

void Widget::setStyleMask(uint64_t style_mask_){
	if (style_mask[0] != style_mask_){
		style_mask[0] = style_mask_;
		markStyleDirty();
		markUnconfigured();
		if (current_style) current_style->clear();

		Window* window;
		if ((window = getTopLevelWindow())){
			window->queueEvent(shared_ptr<EventInvalidateRect>(new EventInvalidateRect(this, allocation.getSize())));
			window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
		}
	}
}

std::shared_ptr<engine::Style> Widget::getCurrentStyle(){
	if (!style_built){
		applyStyles();
	}
	return current_style;
}

void Widget::applyStyles(){
	if (style_built) return;

	for (list<shared_ptr<Style> >::reverse_iterator i = style.rbegin(); i != style.rend(); i++){
		if ((*i)->checkMask(style_mask)){
			if (current_style){
				current_style->apply((*i).get());
			}else{
				current_style = std::shared_ptr<engine::Style>((*i)->clone());
			}
		}
	}

	Widget* p = this->parent;


	while (p){
		if (p->current_style){
			if (current_style){
				current_style->apply(p->current_style.get());
			}else{
				current_style = std::shared_ptr<engine::Style>(p->current_style->clone());
			}
		}

		p = p->parent;
	}

	style_built = true;
}


}

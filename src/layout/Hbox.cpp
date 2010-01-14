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

#include "Hbox.h"
#include "Window.h"
#include "StdEvents.h"

#include <iostream>

using namespace std;

namespace layout {

Hbox::Hbox(double spacing_, bool homogeneous_):Box(spacing_, homogeneous_){

}

Hbox::~Hbox(){

}

void Hbox::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	Rect2<double> child_allocation, child_invalidated_rect;

	for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++){

		child_allocation = (*i)->getAllocation();

		if (invalidated_rect.isIntersecting(child_allocation)){
			child_invalidated_rect = invalidated_rect;
			child_invalidated_rect.x -= child_allocation.x;
			child_invalidated_rect.y -= child_allocation.y;

			cairo_save(cr);
			cairo_translate(cr, child_allocation.x, child_allocation.y);

			(*i)->draw(child_invalidated_rect, draw_context);

			cairo_restore(cr);
		}
	}

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}


void Hbox::allocate(){
	if (allocated) return;

	Vector2<double> child_requisition;
	Vector2<double> max_child_requisition;
	Rect2<double> child_allocation;

	double width = 0;

	uint32_t n_widgets = widgets.size();
	if (n_widgets>=1){

		width = (n_widgets-1) * spacing;

		uint32_t n_expanded = 0;

		uint32_t n_i = 0;
		for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++, n_i++){

			child_requisition = (*i)->getRequisition();

			max_child_requisition.max(child_requisition);

			width += child_requisition.x;

			if (expand[n_i]) n_expanded++;
		}

		if (homogeneous){
			width = (spacing + max_child_requisition.x) * n_widgets - spacing;
		}

		double extra_width = allocation.width - padding.x * 2 - width;

		if (!n_expanded) n_expanded=1;
		double extra_expanded_width = extra_width / n_expanded;

		child_allocation.x = padding.x;
		child_allocation.y = padding.y;

		n_i = 0;
		for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++, n_i++){

			child_requisition = (*i)->getRequisition();
			if (homogeneous){
				child_requisition.x = max_child_requisition.x;
			}

			if (expand[n_i]){
				child_allocation.width = child_requisition.x + extra_expanded_width;
			}else{
				child_allocation.width = child_requisition.x;
			}

			if (fill[n_i]){
				child_allocation.height = allocation.height - padding.y * 2;
			}else{
				child_allocation.height = child_requisition.y;
			}

			(*i)->setAllocation(child_allocation);
			(*i)->allocate();

			child_allocation.x += child_allocation.width + spacing;

		}

	}

	allocated = true;

}

void Hbox::configure(){
	applyStyles();
	if (configured) return;

	Vector2<double> child_requisition;
	Vector2<double> max_child_requisition;
	Rect2<double> child_allocation;

	double width = 0;

	uint32_t n_widgets = widgets.size();
	if (n_widgets>=1){

		width = (n_widgets-1) * spacing;

		for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++){
			(*i)->configure();
			child_requisition = (*i)->getRequisition();

			max_child_requisition.max(child_requisition);

			width += child_requisition.x;
		}

		if (homogeneous){
			width = (spacing + max_child_requisition.x) * n_widgets - spacing;
		}
	}

	Vector2<double> new_size(width, max_child_requisition.y);
	new_size += padding * 2;

	setRequisition(new_size);
	allocated = false;

	configured = true;
}

}

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

#include "Vbox.h"
#include "Window.h"
#include "StdEvents.h"

#include <iostream>

using namespace std;

namespace layout {

Vbox::Vbox(double spacing_, bool homogeneous_):Box(spacing_, homogeneous_){

}

Vbox::~Vbox(){

}

void Vbox::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	Rect2<double> child_allocation, child_invalidated_rect;

	for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++){
	    if ((*i)->getVisible()){

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
	}

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}

void Vbox::allocate(){
	if (allocated) return;

	Rect2<double> new_allocation, old_allocation;

	Vector2<double> child_requisition;
	Vector2<double> max_child_requisition;
	Rect2<double> child_allocation;

	double height = 0;

	uint32_t n_widgets = 0;
	uint32_t n_expanded = 0;
	uint32_t n_i = 0;
	for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++, n_i++){
	    if ((*i)->getVisible()){
		
			child_requisition = (*i)->getRequisition();

			max_child_requisition.max(child_requisition);

			height += child_requisition.y + spacing;

			if (expand[n_i]) n_expanded++;
			n_widgets++;
		}
	}

	if (n_widgets>=1){

		height -= spacing;

		if (homogeneous){
			height = (spacing + max_child_requisition.y) * n_widgets - spacing;
		}

		double extra_height = allocation.height - padding.y * 2 - height;

		if (!n_expanded) n_expanded=1;
		double extra_expanded_height = extra_height / n_expanded;

		child_allocation.x = padding.x;
		child_allocation.y = padding.y;

		n_i = 0;
		for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++, n_i++){
	    	if ((*i)->getVisible()){

				child_requisition = (*i)->getRequisition();
				if (homogeneous){
					child_requisition.y = max_child_requisition.y;
				}

				if (expand[n_i]){
					child_allocation.height = child_requisition.y + extra_expanded_height;
				}else{
					child_allocation.height = child_requisition.y;
				}

				if (fill[n_i]){
					child_allocation.width = allocation.width - padding.x * 2;
				}else{
					child_allocation.width = child_requisition.x;
				}

				old_allocation = (*i)->getAllocation();

				if (old_allocation != child_allocation){
					old_allocation += child_allocation;

					invalidateRect(old_allocation);
				}

				(*i)->setAllocation(child_allocation);
				(*i)->allocate();

				child_allocation.y += child_allocation.height + spacing;

			}
		}
	}

    if (dirty) invalidateLocalRect(previous_allocation);

	allocated = true;

}

void Vbox::configure(){
	applyStyles();
	if (configured) return;

	Vector2<double> child_requisition;
	Vector2<double> max_child_requisition;
	Rect2<double> child_allocation;

	double height = 0;
	uint32_t n_widgets = 0;

	for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++){
	    if ((*i)->getVisible()){

			(*i)->configure();
			child_requisition = (*i)->getRequisition();

			max_child_requisition.max(child_requisition);

			height += child_requisition.y + spacing;
			n_widgets++;
		}
	}
	if (n_widgets>=1){
		height -= spacing;
		if (homogeneous){
			height = (spacing + max_child_requisition.y) * n_widgets - spacing;
		}
	}

	Vector2<double> new_size(max_child_requisition.x, height);
	new_size += padding * 2;

    if (requisition != new_size){
	    setRequisition(new_size);
        markDirty();
    }

	configured = true;
}

}

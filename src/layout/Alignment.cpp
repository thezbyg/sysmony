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

#include "Alignment.h"
#include "StdEvents.h"

#include <iostream>

using namespace std;

namespace layout {

Alignment::Alignment(Vector2<double> alignment_){
	setAlignment(alignment_);
}

Alignment::~Alignment(){

}

void Alignment::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	Rect2<double> child_allocation, child_invalidated_rect;

	list<shared_ptr<Widget> >::iterator i = widgets.begin();
	if (i != widgets.end()){

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

void Alignment::allocate(){
	if (allocated) return;

	list<shared_ptr<Widget> >::iterator i = widgets.begin();
	if (i != widgets.end()){
		if ((*i)->getVisible()){
			
			Rect2<double> new_allocation;
			Rect2<double> old_allocation;
			Vector2<double> child_requisition;

			old_allocation = (*i)->getAllocation();

			child_requisition = (*i)->getRequisition();

			new_allocation = Rect2<double>(
				padding.x + (allocation.width - padding.x * 2 - child_requisition.x) * alignment.x,
				padding.y + (allocation.height - padding.y * 2 - child_requisition.y) * alignment.y,
				child_requisition.x,
				child_requisition.y);

			if (old_allocation != new_allocation){
				old_allocation += new_allocation;

				invalidateRect(old_allocation);
			}
			(*i)->setAllocation(new_allocation);
			(*i)->allocate();
		}
	}

	allocated = true;
}

void Alignment::configure(){
	applyStyles();
	if (configured) return;

	Vector2<double> child_requisition;

	list<shared_ptr<Widget> >::iterator i = widgets.begin();
	if (i != widgets.end()){
		if ((*i)->getVisible()){
	   		(*i)->configure();
			child_requisition = (*i)->getRequisition();
		}
	}

	child_requisition += padding * 2;

	if (child_requisition != requisition){
		setRequisition(child_requisition);
		markDirty();
	}

	configured = true;
}

void Alignment::setAlignment(Vector2<double> alignment_){
	alignment = alignment_;
    invalidateRect(allocation.getSize());
}

const Vector2<double>& Alignment::getAlignment() const{
	return alignment;
}

}

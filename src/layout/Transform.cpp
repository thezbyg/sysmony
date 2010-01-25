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

#include "Transform.h"
#include "StdEvents.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

namespace layout {

Transform::Transform(){
	cairo_matrix_init_identity(&transform_matrix);
}

Transform::~Transform(){

}

void Transform::rotate(double angle){
    cairo_matrix_rotate(&transform_matrix, angle);
    Window* window;
	if ((window = getTopLevelWindow())){
		window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
	}
}

void Transform::scale(double scale_x, double scale_y){
    cairo_matrix_scale(&transform_matrix, scale_x, scale_y);
    Window* window;
	if ((window = getTopLevelWindow())){
		window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
	}
}

static void transform_rect(cairo_matrix_t *matrix, Rect2<double>& rect){
    Vector2<double> point[4] = {
		Vector2<double>(rect.x, rect.y),
		Vector2<double>(rect.x + rect.width, rect.y),
		Vector2<double>(rect.x + rect.width, rect.y + rect.height),
		Vector2<double>(rect.x, rect.y + rect.height),
	};

	for (int i = 0; i < 4; i++){
    	cairo_matrix_transform_point(matrix, &point[i].x, &point[i].y);
	}

	double min_x, min_y, max_x, max_y;
	max_x = min_x = point[0].x;
    max_y = min_y = point[0].y;
	for (int i = 1; i < 4; i++){
    	if (point[i].x > max_x) max_x = point[i].x;
		else if (point[i].x < min_x) min_x = point[i].x;
    	if (point[i].y > max_y) max_y = point[i].y;
		else if (point[i].y < min_y) min_y = point[i].y; 
	}

	rect.width = (max_x - min_x);
	rect.height = (max_y - min_y);
    rect.x = min_x;
	rect.y = min_y;
}


void Transform::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	Rect2<double> child_allocation, child_invalidated_rect;

	list<shared_ptr<Widget> >::iterator i = widgets.begin();
	if (i != widgets.end()){

		if ((*i)->getVisible()){

            cairo_matrix_t inv_transform;
            memcpy(&inv_transform, &transform_matrix, sizeof(cairo_matrix_t));
            cairo_matrix_invert(&inv_transform);

			child_allocation = (*i)->getAllocation();
            
            Rect2<double> invalidated_rect_local = invalidated_rect;
            invalidated_rect_local.x -= offset.x;
            invalidated_rect_local.y -= offset.y;
            transform_rect(&inv_transform, invalidated_rect_local);

			if (invalidated_rect_local.isIntersecting(child_allocation)){
				child_invalidated_rect = invalidated_rect_local;
				child_invalidated_rect.x -= child_allocation.x;
				child_invalidated_rect.y -= child_allocation.y;
				
                cairo_save(cr);
				cairo_translate(cr, child_allocation.x + offset.x, child_allocation.y + offset.y);

                cairo_transform(cr, &transform_matrix);
                
                //cairo_rectangle(cr, child_invalidated_rect.x, child_invalidated_rect.y, child_invalidated_rect.width, child_invalidated_rect.height);
                //cairo_set_line_width(cr, 10);
                //cairo_stroke(cr);

				(*i)->draw(child_invalidated_rect, draw_context);

				cairo_restore(cr);

			}
		}
	}

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}

void Transform::allocate(){
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
				padding.x,
				padding.y,
				child_requisition.x,
				child_requisition.y);

            if (new_allocation != old_allocation){
                invalidateLocalRect(allocation);
                invalidateRect(new_allocation);
            }

			(*i)->setAllocation(new_allocation);
			(*i)->allocate();
            
    	}
	}

	allocated = true;
}

void Transform::configure(){
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

    Rect2<double> child_rect(0, 0, child_requisition.x, child_requisition.y);
    transform_rect(&transform_matrix, child_rect);   

	child_requisition.x = child_rect.width + padding.x * 2;
	child_requisition.y = child_rect.height + padding.y * 2;


    offset.x = -child_rect.x;
    offset.y = -child_rect.y;

	if (child_requisition != requisition){
		setRequisition(child_requisition);
		markDirty();
	}

	configured = true;
}

void Transform::transformRect(Rect2<double>& rect){
    
    Vector2<double> point[4] = {
		Vector2<double>(rect.x, rect.y),
		Vector2<double>(rect.x + rect.width, rect.y),
		Vector2<double>(rect.x + rect.width, rect.y + rect.height),
		Vector2<double>(rect.x, rect.y + rect.height),
	};

	for (int i = 0; i < 4; i++){
    	cairo_matrix_transform_point(&transform_matrix, &point[i].x, &point[i].y);
	}

	double min_x, min_y, max_x, max_y;
	max_x = min_x = point[0].x;
    max_y = min_y = point[0].y;
	for (int i = 1; i < 4; i++){
    	if (point[i].x > max_x) max_x = point[i].x;
		else if (point[i].x < min_x) min_x = point[i].x;
    	if (point[i].y > max_y) max_y = point[i].y;
		else if (point[i].y < min_y) min_y = point[i].y; 
	}

	rect.width = (max_x - min_x);
	rect.height = (max_y - min_y);
    rect.x = allocation.x + min_x + offset.x;
	rect.y = allocation.y + min_y + offset.y;
}


}


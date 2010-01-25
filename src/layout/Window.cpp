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

#include "Window.h"
#include "StdEvents.h"

#include <iostream>

#include <stdio.h>
#include <math.h>

using namespace std;

namespace layout {


Window::Window(){
	g_static_rec_mutex_init(&event_queue_mutex);
}

Window::~Window(){
	g_static_rec_mutex_free(&event_queue_mutex);
}

void Window::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	cairo_rectangle(cr, padding.x, padding.y, allocation.width-padding.x*2, allocation.height-padding.y*2);
	cairo_clip(cr);
	cairo_new_path(cr);

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
				cairo_translate(cr, padding.x, padding.y);

				(*i)->draw(child_invalidated_rect, draw_context);

				cairo_restore(cr);
			}
		}
	}

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}

void Window::allocate(){
	if (allocated) return;

	Rect2<double> new_allocation = Rect2<double>(padding.x, padding.y, allocation.width-padding.x*2, allocation.height-padding.y*2);
	Rect2<double> old_allocation;

	list<shared_ptr<Widget> >::iterator i = widgets.begin();
	if (i != widgets.end()){
        if ((*i)->getVisible()){
            old_allocation = (*i)->getAllocation();

            if (old_allocation != new_allocation){
                old_allocation += new_allocation;
                invalidateRect(old_allocation);
            }

            (*i)->setAllocation(new_allocation);
            (*i)->allocate();
        }
	}

    if (dirty) invalidateLocalRect(previous_allocation);

	allocated = true;
}

void Window::configure(){
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
		dirty = true;
	}

	configured = true;
}

void Window::resize(const Vector2<double> &size){
	setRequisition(size);
	setAllocation(Rect2<double>(position.x, position.y, size.x, size.y));

	Window* window = dynamic_cast<Window*>(getTopLevel());
	window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
}

void Window::addInvalidated(const Rect2<double>& invalidated_rect_){
	invalidated_rect += invalidated_rect_;
}

const Rect2<double>& Window::getInvalidated() const{
	return invalidated_rect;
}

void Window::clearInvalidated(){
	invalidated_rect = Rect2<double>(0, 0, 0, 0);
}

void Window::setPosition(const Vector2<double> &position_){
	position = position_;
}

const Vector2<double> Window::getPosition() const{
	return position;
}







void Window::queueEvent(shared_ptr<Event> event){
	g_static_rec_mutex_lock(&event_queue_mutex);
	event_queue.push_back(event);
	g_static_rec_mutex_unlock(&event_queue_mutex);
}

void Window::queueEventOnce(shared_ptr<Event> event){
	g_static_rec_mutex_lock(&event_queue_mutex);
	bool duplicate = false;
	for (deque<shared_ptr<Event> >::iterator i = event_queue.begin(); i != event_queue.end(); i++){
		if (*(*i).get() == *event.get()){
			duplicate = true;
			break;
		}
	}
	if (!duplicate) event_queue.push_back(event);
	g_static_rec_mutex_unlock(&event_queue_mutex);
}

void Window::processEvents(){
	g_static_rec_mutex_lock(&event_queue_mutex);

#ifndef NDEBUG
	printf("Events: %lu\n", event_queue.size());
#endif

	while (!event_queue.empty()){
		event_queue.front()->process(this);
		event_queue.pop_front();
	}
	g_static_rec_mutex_unlock(&event_queue_mutex);
}


}

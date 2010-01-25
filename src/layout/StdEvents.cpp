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

#include "StdEvents.h"
#include "Vector2.h"
#include "Window.h"

#include <stdio.h>
#include <typeinfo>

using namespace std;

namespace layout {

EventRequestReallocation::EventRequestReallocation(Widget *target_widget_){
	target_widget = target_widget_;
	top_level = const_cast<Widget*>(target_widget_->getTopLevel());
}

EventRequestReallocation::~EventRequestReallocation(){

}

void EventRequestReallocation::process(Window *window){

	Widget* w = target_widget;
	while (w){
		w->markUnconfigured();
		w->markUnallocated();
		w = const_cast<Widget*>(w->getParent());
	}

	window->queueEventOnce(shared_ptr<EventConfigure>(new EventConfigure(top_level)));
}

bool EventRequestReallocation::operator==(const Event& event) const{
	if (typeid(event) != typeid(EventRequestReallocation)) return false;

	const EventRequestReallocation* e = dynamic_cast<const EventRequestReallocation*>(&event);
	if ((e->top_level == top_level) && (e->target_widget == target_widget)) return true;
	return false;
}





EventConfigure::EventConfigure(Widget *target_widget_){
	target_widget = target_widget_;
}

EventConfigure::~EventConfigure(){
}

void EventConfigure::process(Window *window){
	target_widget->configure();
	target_widget->allocate();
}

bool EventConfigure::operator==(const Event& event) const{
	if (typeid(event) != typeid(EventConfigure)) return false;

	const EventConfigure* e = dynamic_cast<const EventConfigure*>(&event);
	if ((e->target_widget == target_widget)) return true;
	return false;
}




/*EventInvalidateRect::EventInvalidateRect(Widget *target_widget_, const Rect2<double> &rectangle_){
	target_widget = target_widget_;
	rectangle = rectangle_;
}

EventInvalidateRect::~EventInvalidateRect(){
}

void EventInvalidateRect::process(Window *window){

	Rect2<double> toplevel_rect = rectangle;
	Rect2<double> allocation;

	Widget *w = target_widget;
	Widget *top_level = target_widget;
	while (w){
		top_level = w;

		allocation = w->getAllocation();
		toplevel_rect.x += allocation.x;
		toplevel_rect.y += allocation.y;

		w = const_cast<Widget*>(w->getParent());
	}

	dynamic_cast<Window*>(top_level)->addInvalidated(toplevel_rect);
}
  */


}

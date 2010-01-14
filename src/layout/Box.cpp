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

#include "Box.h"
#include "Window.h"
#include "StdEvents.h"

#include <stdio.h>

using namespace std;

namespace layout {

Box::Box(double spacing_, bool homogeneous_){
	spacing = spacing_;
	homogeneous = homogeneous_;
}

Box::~Box(){

}

void Box::addWidget(shared_ptr<Widget> widget, bool fill_, bool expand_){
	Container::addWidget(widget);

	fill.push_back(fill_);
	expand.push_back(expand_);


	Window* window = getTopLevelWindow();
	if (window) window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
}

void Box::removeAll(){
	Container::removeAll();

	fill.clear();
	expand.clear();
}

void Box::removeChild(shared_ptr<Widget> widget){

	uint32_t position = 0;
	for (list<shared_ptr<Widget> >::iterator i = widgets.begin(); i != widgets.end(); i++){
		if (*i == widget){
			fill.erase(fill.begin() + position);
			expand.erase(expand.begin() + position);
			break;
		}
		position++;
	}

	Container::removeChild(widget);
}

}

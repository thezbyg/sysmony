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

#include "Image.h"
#include "Window.h"
#include "StdEvents.h"

#include <cairo/cairo.h>
#include <gdk/gdk.h>

#include <iostream>

using namespace std;

namespace layout {

Image::Image(const char* filename_){
	pixbuf = NULL;
	setImageFilename(filename_);
}

Image::~Image(){
	if (pixbuf) g_object_unref(pixbuf);
	pixbuf = NULL;
}

void Image::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();
	if (pixbuf){
		cairo_save(cr);
		gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
		cairo_paint (cr);

		cairo_restore(cr);
	}

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}

void Image::configure(){
	applyStyles();
	if (configured) return;

	if (pixbuf){
		setRequisition(Vector2<double>(gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf)));
	}else{
		setRequisition(Vector2<double>(0, 0));
	}

	configured = true;
}

void Image::setImageFilename(const char* filename_){

	if (filename != filename_){

		if (pixbuf) g_object_unref(pixbuf);
		pixbuf = NULL;

		GError *error = NULL;
		pixbuf = gdk_pixbuf_new_from_file(filename_, &error);
		if (error){
			cerr << error->message << endl;
			g_error_free(error);
		}
		filename = filename_;

		Window* window = getTopLevelWindow();

		if (window){
			window->queueEvent(shared_ptr<EventInvalidateRect>(new EventInvalidateRect(this, allocation.getSize())));
			window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
		}

		configured = false;

		/*Vector2<double> old_requisition = requisition;
		//configure();

		if (old_requisition != requisition){
			if (window)	window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
			configured = false;
		}*/

	}
}

}

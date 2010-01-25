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

#include "Label.h"
#include "StdEvents.h"
#include "Window.h"

#include <cairo/cairo.h>
#include <pango/pangocairo.h>

using namespace std;

namespace layout {

Label::Label(const char* text_){
	setText(text_);
	align = Left;
}

void Label::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	cairo_save(cr);

	PangoLayout *layout;
	PangoFontDescription *desc;

	layout = pango_cairo_create_layout(cr);

	pango_layout_set_markup(layout, text.c_str(), text.length());
	desc = pango_font_description_new();

	pango_font_description_set_family_static(desc, current_style->getFontName(this));
	pango_font_description_set_weight(desc, PangoWeight(current_style->getFontWeight(this)));
	pango_font_description_set_size(desc, current_style->getFontSize(this) * PANGO_SCALE);

	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

    pango_layout_set_width(layout, allocation.width * PANGO_SCALE);
	pango_layout_set_height(layout, allocation.height * PANGO_SCALE);
 	pango_layout_set_single_paragraph_mode(layout, true);

	pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
	
	const PangoAlignment alignment_flags[] = {PANGO_ALIGN_LEFT, PANGO_ALIGN_CENTER, PANGO_ALIGN_RIGHT};
	pango_layout_set_alignment(layout, alignment_flags[align]);

	//pango_cairo_update_layout(cr, layout);

	PangoRectangle ink_rect, logical_rect;
	pango_layout_get_extents(layout, &ink_rect, &logical_rect);
	cairo_translate(cr, logical_rect.x / (double)PANGO_SCALE, logical_rect.y / (double)PANGO_SCALE);

	Color color;
	if (current_style) color = current_style->getTextColor(this);

	cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
	pango_cairo_show_layout(cr, layout);

	g_object_unref(layout);

	cairo_restore(cr);

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}

void Label::configure(){
	applyStyles();
	if (configured) return;

	if (text.empty()){
		Vector2<double> new_size(1, 1);
		setRequisition(new_size);

	}else{

		cairo_surface_t *surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
		cairo_t *cr = cairo_create(surf);

		PangoLayout *pan_layout;
		PangoFontDescription *desc;

		pan_layout = pango_cairo_create_layout(cr);

		pango_layout_set_markup(pan_layout, text.c_str(), text.length());

		desc = pango_font_description_new();

		pango_font_description_set_family_static(desc, current_style->getFontName(this));
		pango_font_description_set_weight(desc, PangoWeight(current_style->getFontWeight(this)));
		pango_font_description_set_size(desc, current_style->getFontSize(this) * PANGO_SCALE);

		pango_layout_set_font_description(pan_layout, desc);
		pango_font_description_free(desc);

        pango_layout_set_single_paragraph_mode(pan_layout, true); 

		pango_cairo_update_layout(cr, pan_layout);

		PangoRectangle ink_rect, logical_rect;
		pango_layout_get_extents(pan_layout, &ink_rect, &logical_rect);

		Vector2<double> new_size(logical_rect.width / (double)PANGO_SCALE, logical_rect.height / (double)PANGO_SCALE);
		setRequisition(new_size);

		g_object_unref(pan_layout);
		cairo_destroy(cr);
		cairo_surface_destroy(surf);

	}

	configured = true;
}

const char* Label::getText() const{
	return text.c_str();
}

void Label::setText(const char* text_){
	if (text_ != text){
		text = text_;
		Window* window = getTopLevelWindow();

		dirty = true;
		if (window){
			invalidateRect(allocation.getSize());
			window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
		}
		configured = false;
	}
}

const Label::Align Label::getAlignment() const{
	return align;
}

void Label::setAlignment(const Align alignment){
	if (align != alignment){
    	align = alignment;	
		invalidateRect(allocation.getSize());
	}
}

}

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

#include "Graph.h"
#include "StdEvents.h"
#include "Window.h"

#include <math.h>
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

#include <iostream>

using namespace std;

namespace layout {

Graph::Graph(double lower_bound_, double upper_bound_, uint32_t data_points){
	lower_bound = lower_bound_;
	upper_bound = upper_bound_;

	values.resize(data_points);
	position = 0;	
	
	scale = LINEAR;
	scale_arg1 = 0;
	scale_arg2 = 0;
}

void Graph::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){
	
	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	cairo_save(cr);

	Color color;
	if (current_style) color = current_style->getTextColor(this);
	cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);

	uint32_t data_points = values.size();
	double step = (allocation.width - 2) / (data_points - 1);
    uint32_t data_i = position;

    double value;
	double value_clamped;

    if (scale == LINEAR){
		double multiplier = (allocation.height - 2) / (upper_bound - lower_bound);
		
		value = values[data_i];
		value_clamped = min(max(values[data_i], lower_bound), upper_bound); 
		cairo_move_to(cr, 0, allocation.height - 1 - (-lower_bound + value_clamped) * multiplier);

		for (uint32_t i = 1; i != data_points; ++i){
			data_i = (position + i) % data_points;
			value_clamped = min(max(values[data_i], lower_bound), upper_bound);
			cairo_line_to(cr, step * i, allocation.height - 1 - (-lower_bound + value_clamped) * multiplier);
		}

	}else if (scale == LOGARITHMIC){
		double min_value, max_value;

		min_value = log(scale_arg1);
		max_value = log(scale_arg2);

		double first_multiplier = (scale_arg2 - scale_arg1) / (upper_bound - lower_bound);
        double second_multiplier = (allocation.height - 2) / (max_value - min_value);
 
		value = scale_arg1 + min(max(values[data_i], lower_bound), upper_bound) * first_multiplier;
		cairo_move_to(cr, 0, allocation.height - 1 - (-min_value + log(value)) * second_multiplier);

		for (uint32_t i = 1; i != data_points; ++i){
			uint32_t data_i = (position + i) % data_points;
			
			value = scale_arg1 + min(max(values[data_i], lower_bound), upper_bound) * first_multiplier;
			cairo_line_to(cr, step * i, allocation.height - 1 - (-min_value + log(value)) * second_multiplier);
		}
	}
	
	cairo_stroke(cr);
	cairo_restore(cr);

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}

void Graph::configure(){
	applyStyles();
	if (configured) return;

	Vector2<double> new_size(1, 1);
	new_size.max(getMinRequisition());
	setRequisition(new_size);

	configured = true;
}

void Graph::pushValue(double val){
	values[position++] = val;
    if (position >= values.size()) position = 0;
    invalidateRect(allocation.getSize());
}



void Graph::setLogarithmicScale(double lower_x, double upper_x){
	scale_arg1 = max(lower_x, 0.000000001);			//check that bound is not equal or less than zero, we don't want NaN or -inf later 
	scale_arg2 = max(upper_x, 0.000000001);			//same as above for upper bound 

	scale = LOGARITHMIC;
    invalidateRect(allocation.getSize());
}

}

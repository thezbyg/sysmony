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

#include "Table.h"
#include "Window.h"
#include "StdEvents.h"

#include <stdio.h>
#include <algorithm>
#include <iostream>

using namespace std;

namespace layout {

Table::Table(const Vector2<double> &spacing_, const Vector2<bool> &homogeneous_){
	homogeneous = homogeneous_;
	spacing = spacing_;

	table_width = 0;
	table_height = 0;
}

Table::~Table(){


}

void Table::addWidget(std::shared_ptr<Widget> widget, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, AttachOptions x_options, AttachOptions y_options, double spacing){

	std::shared_ptr<Flags> flags = std::shared_ptr<Flags>(new Flags);
	flags->widget = widget;

	flags->x_options = x_options;
	flags->y_options = y_options;
	flags->spacing = spacing;

	flags->x1 = x1;
	flags->y1 = y1;
	flags->x2 = x2;
	flags->y2 = y2;


	table_width = max(table_width, x2);
	table_height = max(table_height, y2);

	/*if (table_width < x2 || table_height < y2){
		uint32_t max_x = max(table_width, x2);
		uint32_t max_y = max(table_height, y2);

		vector<std::shared_ptr<struct Flags> > new_table;
		new_table.resize(max_x * max_y);

		for (uint32_t y = 0; y < table_width; y++){
			for (uint32_t x = 0; x < table_height; x++){
				new_table[y * max_x + x] = table[y * table_width + x];
			}
		}

		table = new_table;
		table_width = max_x;
		table_height = max_y;

	}*/


	/*for (uint32_t y = y1; y < y2; y++){
		for (uint32_t x = x1; x < x2; x++){
			table[y * table_width + x] = flags;
		}
	}
	*/

	widgets.push_back(flags);

	widget->reparent(this);

	Window* window = getTopLevelWindow();
	if (window) window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
}

void Table::removeChild(shared_ptr<Widget> widget){
	/*for (vector<shared_ptr<Flags> >::iterator i = table.begin(); i != table.end(); i++){
		if ((*i)->widget == widget){
			*i = shared_ptr<Flags>();
		}
	}*/
	for (list<shared_ptr<Flags> >::iterator i = widgets.begin(); i != widgets.end(); i++){
		if ((*i)->widget == widget){
			widgets.erase(i);
			break;
		}
	}


	Window* window = getTopLevelWindow();
	if (window) window->queueEventOnce(shared_ptr<EventRequestReallocation>(new EventRequestReallocation(this)));
}

void Table::draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context){

	if (current_style) current_style->predraw(this, draw_context);

	cairo_t *cr = draw_context->getCairo();

	Rect2<double> child_allocation, child_invalidated_rect;

	for (list<shared_ptr<Flags> >::iterator i = widgets.begin(); i != widgets.end(); i++){

		child_allocation = (*i)->widget->getAllocation();

		if (invalidated_rect.isIntersecting(child_allocation)){
			child_invalidated_rect = invalidated_rect;
			child_invalidated_rect.x -= child_allocation.x;
			child_invalidated_rect.y -= child_allocation.y;

			cairo_save(cr);
			cairo_translate(cr, child_allocation.x, child_allocation.y);

			(*i)->widget->draw(child_invalidated_rect, draw_context);

			cairo_restore(cr);
		}
	}

	if (current_style) current_style->postdraw(this, draw_context);

	dirty = false;
}



void Table::configure(){
	applyStyles();
	if (configured) return;

	Vector2<double> child_requisition;
	Vector2<double> max_child_requisition;
	Rect2<double> child_allocation;

	collumn_props.clear();
	collumn_props.resize(table_width);
	double max_width = 0;

	row_props.clear();
	row_props.resize(table_height);
	double max_height = 0;

	//Calculate one cell widget sizes
	for (list<shared_ptr<Flags> >::iterator i = widgets.begin(); i != widgets.end(); i++){
		(*i)->widget->configure();
		child_requisition = (*i)->widget->getRequisition();
		child_requisition.x += (*i)->spacing * 2;
		child_requisition.y += (*i)->spacing * 2;

		if ((*i)->x1 == (*i)->x2 - 1){
			collumn_props[(*i)->x1].max_size = max(collumn_props[(*i)->x1].max_size, child_requisition.x);
			if ((*i)->x_options & AttachOptions::Expand) collumn_props[(*i)->x1].expand = true;
			max_width = max(max_width, child_requisition.x);
		}
		if ((*i)->y1 == (*i)->y2 - 1){
			row_props[(*i)->y1].max_size = max(row_props[(*i)->y1].max_size, child_requisition.y);
			if ((*i)->y_options & AttachOptions::Expand) row_props[(*i)->y1].expand = true;
			max_height = max(max_height, child_requisition.y);
		}
	}

	//Update if homogenous is enabled
	if (homogeneous.x){
		for (vector<Props>::iterator i = collumn_props.begin(); i != collumn_props.end(); i++){
			(*i).max_size = max((*i).max_size, max_width);
		}
	}
	if (homogeneous.y){
		for (vector<Props>::iterator i = row_props.begin(); i != row_props.end(); i++){
			(*i).max_size = max((*i).max_size, max_height);
		}
	}

	//Calculate multi-row/multi-collumn widgets
	for (list<shared_ptr<Flags> >::iterator i = widgets.begin(); i != widgets.end(); i++){
		child_requisition = (*i)->widget->getRequisition();
		child_requisition.x += (*i)->spacing * 2;
		child_requisition.y += (*i)->spacing * 2;

		if ((*i)->x1 != (*i)->x2 - 1){

			double available_width = 0;
			uint32_t expanding = 0;

			for (uint32_t j = (*i)->x1; j < (*i)->x2; j++){
				available_width += collumn_props[j].max_size;
				if (collumn_props[j].expand) expanding++;
			}

			available_width += spacing.x * ((*i)->x2 - (*i)->x1 - 1);

			if (available_width < child_requisition.x){

				double extra_space = child_requisition.x - available_width;

				if (expanding == 0){
					extra_space /= (*i)->x2 - (*i)->x1;

					for (uint32_t j = (*i)->x1; j < (*i)->x2; j++){
						collumn_props[j].max_size += extra_space;
					}
				}else{
					extra_space /= expanding;

					for (uint32_t j = (*i)->x1; j < (*i)->x2; j++){
						if (collumn_props[j].expand)
							collumn_props[j].max_size += extra_space;
					}
				}
			}
		}


		if ((*i)->y1 != (*i)->y2 - 1){

			double available_height = 0;
			uint32_t expanding = 0;

			for (uint32_t j = (*i)->y1; j < (*i)->y2; j++){
				available_height += row_props[j].max_size;
				if (row_props[j].expand) expanding++;
			}

			available_height += spacing.y * ((*i)->y2 - (*i)->y1 - 1);

			if (available_height < child_requisition.y){

				double extra_space = child_requisition.y - available_height;

				if (expanding == 0){
					extra_space /= (*i)->y2 - (*i)->y1;

					for (uint32_t j = (*i)->y1; j < (*i)->y2; j++){
						row_props[j].max_size += extra_space;
					}
				}else{
					extra_space /= expanding;

					for (uint32_t j = (*i)->y1; j < (*i)->y2; j++){
						if (row_props[j].expand)
							row_props[j].max_size += extra_space;
					}
				}
			}
		}
	}


	//Update if homogenous is enabled
	if (homogeneous.x){
		for (vector<Props>::iterator i = collumn_props.begin(); i != collumn_props.end(); i++){
			(*i).max_size = max((*i).max_size, max_width);
		}
	}
	if (homogeneous.y){
		for (vector<Props>::iterator i = row_props.begin(); i != row_props.end(); i++){
			(*i).max_size = max((*i).max_size, max_height);
		}
	}

	Vector2<double> new_size(0, 0);
	for (vector<Props>::iterator i = collumn_props.begin(); i != collumn_props.end(); i++){
		new_size.x += (*i).max_size + spacing.x;
	}
	new_size.x -= spacing.x;

	for (vector<Props>::iterator i = row_props.begin(); i != row_props.end(); i++){
		new_size.y += (*i).max_size + spacing.y;
	}
	new_size.y -= spacing.y;


	new_size += padding * 2;

	setRequisition(new_size);

	configured = true;
}

void Table::allocate(){
	if (allocated) return;

	Vector2<double> child_requisition;
	Rect2<double> child_allocation;

	Rect2<double> new_allocation, old_allocation;

	{
		double total_space, offset;
		uint32_t expanding;

		total_space = 0;
		expanding = 0;
		for (vector<Props>::iterator i = collumn_props.begin(); i != collumn_props.end(); i++){
			total_space += (*i).max_size + spacing.x;
			(*i).size = (*i).max_size;
			if ((*i).expand) expanding++;
		}
		total_space = total_space - spacing.x + padding.x * 2;

		if (expanding){
			double extra_space = (allocation.width - total_space) / expanding;
			if (extra_space > 0){
				for (vector<Props>::iterator i = collumn_props.begin(); i != collumn_props.end(); i++){
					if ((*i).expand)
						(*i).size += extra_space;
				}
			}
		}
		offset = 0;
		for (vector<Props>::iterator i = collumn_props.begin(); i != collumn_props.end(); i++){
			(*i).offset = offset;
			offset += (*i).size + spacing.x;
		}


		total_space = 0;
		expanding = 0;
		for (vector<Props>::iterator i = row_props.begin(); i != row_props.end(); i++){
			total_space += (*i).max_size + spacing.y;
			(*i).size = (*i).max_size;
			if ((*i).expand) expanding++;
		}
		total_space = total_space - spacing.y + padding.y * 2;

		if (expanding){
			double extra_space = (allocation.height - total_space) / expanding;
			if (extra_space > 0){
				for (vector<Props>::iterator i = row_props.begin(); i != row_props.end(); i++){
					if ((*i).expand)
						(*i).size += extra_space;
				}
			}
		}
		offset = 0;
		for (vector<Props>::iterator i = row_props.begin(); i != row_props.end(); i++){
			(*i).offset = offset;
			offset += (*i).size + spacing.y;
		}
	}

	for (list<shared_ptr<Flags> >::iterator i = widgets.begin(); i != widgets.end(); i++){
		child_requisition = (*i)->widget->getRequisition();

		child_allocation.x = padding.x + collumn_props[(*i)->x1].offset + (*i)->spacing;
		child_allocation.y = padding.y + row_props[(*i)->y1].offset + (*i)->spacing;

		if ((*i)->x_options & Fill){
			child_allocation.width = 0;

			for (uint32_t j = (*i)->x1; j < (*i)->x2; j++){
				child_allocation.width += collumn_props[j].size;
			}

			child_allocation.width -= (*i)->spacing;
		}else{
			child_allocation.width = child_requisition.x;
		}
		if ((*i)->y_options & Fill){
			child_allocation.height = 0;

			for (uint32_t j = (*i)->y1; j < (*i)->y2; j++){
				child_allocation.height += row_props[j].size;
			}

			child_allocation.height -= (*i)->spacing;
		}else{
			child_allocation.width = child_requisition.y;
		}

		old_allocation = (*i)->widget->getAllocation();

		if (old_allocation != child_allocation){
			old_allocation += child_allocation;

			invalidateRect(old_allocation);
		}

		(*i)->widget->setAllocation(child_allocation);
		(*i)->widget->allocate();

	}


	allocated = true;

}



}

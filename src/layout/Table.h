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

#ifndef LAYOUT_TABLE_H_
#define LAYOUT_TABLE_H_

#include "Container.h"

#include <stdbool.h>
#include <list>
#include <vector>

namespace layout {

class Table:public Container{
public:
	enum AttachOptions{
		Fill = 0x01,
		Expand = 0x02,
	};
protected:
	Vector2<double> spacing;
	Vector2<bool> homogeneous;

	class Flags{
	public:
		AttachOptions x_options;
		AttachOptions y_options;
		double spacing;

		uint32_t x1, y1, x2, y2;
		std::shared_ptr<Widget> widget;
	};
	class Props{
	public:
		double max_size;

		double offset;
		double size;

		bool expand;

		Props(){
			max_size = 0;
			expand = false;

			offset = 0;
			size = 0;
		};
	};

	uint32_t table_width;
	uint32_t table_height;

	std::vector<Props> collumn_props;
	std::vector<Props> row_props;

	//std::vector<std::shared_ptr<struct Flags> > table;
	std::list<std::shared_ptr<struct Flags> > widgets;
public:



	Table(const Vector2<double> &spacing, const Vector2<bool> &homogeneous);
	virtual ~Table();

	void addWidget(std::shared_ptr<Widget> widget, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, AttachOptions x_options, AttachOptions y_options, double spacing);
	void removeAll();
	virtual void removeChild(std::shared_ptr<Widget> widget);

	virtual void draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context);
	virtual void configure();
	virtual void allocate();

};

}

#endif /*LAYOUT_TABLE_H_*/

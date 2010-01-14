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

#ifndef LAYOUT_WIDGET_H_
#define LAYOUT_WIDGET_H_

namespace layout {
class Widget;
class Window;
}

#include "DrawContext.h"
#include "engine_api/Style.h"

#include "Rect2.h"
#include "Vector2.h"

#include <stdint.h>
#include <stdbool.h>
#include <memory>
#include <list>

namespace layout {

class Widget{
protected:
	Widget* parent;
	std::list<std::shared_ptr<engine::Style> > style;
	std::shared_ptr<engine::Style> current_style;

	bool visible;
	bool dirty;
	bool configured;
	bool allocated;
	bool style_built;

	Vector2<double> requisition;
	Rect2<double> allocation;

	uint64_t style_mask[1];

public:

	Widget();
	virtual ~Widget();

	Widget* getParent() const;
	Widget* getTopLevel() const;
	void reparent(const Widget* widget);

	Window* getTopLevelWindow() const;

	const Rect2<double>& getAllocation() const;
	void setAllocation(const Rect2<double>& allocation);

	const Vector2<double>& getRequisition() const;
	void setRequisition(const Vector2<double>& requisition);

	void addStyle(std::shared_ptr<engine::Style> style);
	void setStyleMask(uint64_t style_mask);
	void applyStyles();

	std::shared_ptr<engine::Style> getCurrentStyle();

	void markDirty();
	void markUnconfigured();
	void markUnallocated();
	void markStyleDirty();

	virtual void draw(const Rect2<double>& invalidated_rect, DrawContext *draw_context);
	virtual void configure();
	virtual void allocate();
	virtual void build_style();

	virtual void removeChild(std::shared_ptr<Widget> widget);
};

}

#endif /*LAYOUT_WIDGET_H_*/

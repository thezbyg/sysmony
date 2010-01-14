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

#ifndef LAYOUT_RECT2_H_
#define LAYOUT_RECT2_H_

namespace layout {

template<typename T>
class Rect2{
public:
	Rect2():x(0),y(0),width(0),height(0){
	};
	Rect2(const T &x_, const T &y_, const T &width_, const T &height_):x(x_),y(y_),width(width_),height(height_){
	};

	const Rect2 operator=(const Rect2 &rect){

		if (this == &rect) return *this;

		x = rect.x;
		y = rect.y;
		width = rect.width;
		height = rect.height;
		return *this;
	};

	const Rect2 operator+(const Rect2 &rect) const{

		if (rect.isEmpty()) return *this;
		if (isEmpty()) return rect;

		Rect2 r;

		if (x < rect.x) r.x = x;
		else r.x = rect.x;
		if (y < rect.y) r.y = y;
		else r.y = rect.y;

		if (x + width > rect.x + rect.width) r.width = (x + width) - r.x;
		else r.width = (rect.x + rect.width) - r.x;
		if (y + height > rect.y + rect.height) r.height = (y + height) - r.y;
		else r.height = (rect.y + rect.height) - r.y;

		return r;
	};

	const Rect2 operator+=(const Rect2 &rect){
		*this=*this+rect;
		return *this;
	};

	bool operator==(const Rect2 &rect) const{
		if (x != rect.x || y != rect.y || width != rect.width || height != rect.height) return false;
		return true;
	};

	bool operator!=(const Rect2 &rect) const{
		if (x != rect.x || y != rect.y || width != rect.width || height != rect.height) return true;
		return false;
	};

	bool isIntersecting(const Rect2 &rect) const{
		if (x < rect.x){
			if (x+width < rect.x) return false;
		}else{
			if (rect.x+rect.width < x) return false;
		}
		if (y < rect.y){
			if (y+height < rect.y) return false;
		}else{
			if (rect.y+rect.height < y) return false;
		}
		return true;
	};

	bool isInside(const T &x_, const T &y_) const{
		return !(x_ < x || x_ > x+width || y_ < y || y > y + height);
	}

	bool isEmpty() const{
		return (width <= 0 && height <= 0);
	};

	Rect2<double> getPosition() const{
		Rect2 r;
		r.x = x;
		r.y = y;
		return r;
	};

	Rect2<double> getSize() const{
		Rect2 r;
		r.width = width;
		r.height = height;
		return r;
	};

	T x, y, width, height;
};


}

#endif /* LAYOUT_RECT2_H_ */

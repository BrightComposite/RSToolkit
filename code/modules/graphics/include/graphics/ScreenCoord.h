//---------------------------------------------------------------------------

#pragma once

#ifndef SCREEN_COORD_H
#define SCREEN_COORD_H

//---------------------------------------------------------------------------

#include <math/rect.h>

//---------------------------------------------------------------------------

namespace asd
{
	using float_point = math::point<float>;
	using int_point = math::point<int>;
	
	using float_rect = math::rect<float>;
	using int_rect = math::rect<int>;
	
	using float_size = math::size<float>;
	using int_size = math::size<int>;
	
	subclass(abs_rect, int_rect);
	
	subclass(rel_rect, float_rect);
	
	subclass(sq_rect, float_rect);
	
	subclass(abs_point, int_point);
	
	subclass(rel_point, float_point);
	
	subclass(sq_point, float_point);
	
	subclass(abs_size, int_size);
	
	subclass(rel_size, float_size);
	
	subclass(sq_size, float_size);
	
	class viewport
	{
	public:
		viewport(int width, int height) : _size(width, height), _ratio(float(height) / float(width)), _invratio(float(width) / float(height)) {}
		
		viewport(const int_size & size) : _size(size), _ratio(float(size.y) / float(size.x)), _invratio(float(size.x) / float(size.y)) {}
		
		viewport & operator =(const int_size & size) {
			set(size);
			return *this;
		}
		
		operator const int_size &() const {
			return _size;
		}
		
		operator int_rect() const {
			return {_size};
		}
		
		const int_size & size() const {
			return _size;
		}
		
		int width() const {
			return _size.x;
		}
		
		int height() const {
			return _size.y;
		}
		
		float ratio() const {
			return _ratio;
		}
		
		float invratio() const {
			return _invratio;
		}
		
		void set(int x, int y) {
			_size.x = x;
			_size.y = y;
			updateRatio();
		}
		
		void set(const int_size & size) {
			_size = size;
			updateRatio();
		}
		
		void setX(int value) {
			_size.x = value;
			updateRatio();
		}
		
		void setY(int value) {
			_size.y = value;
			updateRatio();
		}
	
	protected:
		void updateRatio() {
			_ratio = float(_size.y) / float(_size.x);
			_invratio = float(_size.x) / float(_size.y);
		}
		
		int_size _size;
		
		float _ratio;
		float _invratio;
	};
	
	struct api(graphics) ScreenCoord
	{
		static void toRel(const int_rect & rectAbs, const int_size & viewport, float_point & posRel, float_size & sizeRel);
		static void toRel(const int_rect & rectAbs, const int_size & viewport, float_rect & rectRel);
		static void toRel(const int_point & posAbs, const int_size & viewport, float_point & posRel);
		static void toRel(const int_size & sizeAbs, const int_size & viewport, float_size & sizeRel);
		static rel_rect toRel(const int_rect & rectAbs, const int_size & viewport);
		static rel_point toRel(const int_point & posAbs, const int_size & viewport);
		static rel_size toRel(const int_size & sizeAbs, const int_size & viewport);
		
		static void toAbs(const float_rect & rectRel, const int_size & viewport, int_point & posAbs, int_size & sizeAbs);
		static void toAbs(const float_rect & rectRel, const int_size & viewport, int_rect & rectAbs);
		static void toAbs(const float_point & posRel, const int_size & viewport, int_point & posAbs);
		static void toAbs(const float_size & sizeRel, const int_size & viewport, int_size & sizeAbs);
		static abs_rect toAbs(const float_rect & rectRel, const int_size & viewport);
		static abs_point toAbs(const float_point & posRel, const int_size & viewport);
		static abs_size toAbs(const float_size & sizeRel, const int_size & viewport);
		
		static void toSq(const int_rect & rectAbs, const viewport & viewport, float_point & posSq, float_size & sizeSq);
		static void toSq(const int_rect & rectAbs, const viewport & viewport, float_rect & rectSq);
		static void toSq(const int_point & posAbs, const viewport & viewport, float_point & posSq);
		static void toSq(const int_size & sizeAbs, const viewport & viewport, float_size & sizeSq);
		static sq_rect toSq(const int_rect & rectAbs, const viewport & viewport);
		static sq_point toSq(const int_point & posAbs, const viewport & viewport);
		static sq_size toSq(const int_size & sizeAbs, const viewport & viewport);
		
		static void toRel(const float_rect & rectSq, const viewport & viewport, float_point & posRel, float_size & sizeRel);
		static void toRel(const float_rect & rectSq, const viewport & viewport, float_rect & rectRel);
		static void toRel(const float_point & posSq, const viewport & viewport, float_point & posRel);
		static void toRel(const float_size & sizeSq, const viewport & viewport, float_size & sizeRel);
		static sq_rect toRel(const float_rect & rectSq, const viewport & viewport);
		static sq_point toRel(const float_point & posSq, const viewport & viewport);
		static sq_size toRel(const float_size & sizeSq, const viewport & viewport);
	};
}

//---------------------------------------------------------------------------
#endif

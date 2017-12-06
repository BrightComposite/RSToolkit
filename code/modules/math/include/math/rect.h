//---------------------------------------------------------------------------

#pragma once

#ifndef RECT_H
#define RECT_H

//---------------------------------------------------------------------------

#include <meta/inheritance.h>

#include "point.h"
#include "range.h"

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template <typename T>
		subclass(size, point<T>);
		
		template <typename T>
		class rect
		{
		public:
			union
			{
				struct
				{
					T left, top, right, bottom;
				};
				
				array<T, 4> data;
			};
			
			rect() :
				left(static_cast<T>(0)), top(static_cast<T>(0)), right(static_cast<T>(0)), bottom(static_cast<T>(0)) {}
			
			rect(const rect & r) :
				left(r.left), top(r.top), right(r.right), bottom(r.bottom) {}
			
			rect(T left, T top, T right, T bottom) :
				left(left), top(top), right(right), bottom(bottom) {}
			
			rect(T w, T h) :
				left(static_cast<T>(0)), top(static_cast<T>(0)), right(w), bottom(h) {}
			
			template <typename U, useif<not_same_type<T, U>::value>>
			explicit rect(const rect<U> & r) :
				left(static_cast<T>(r.left)), top(static_cast<T>(r.top)), right(static_cast<T>(r.right)), bottom(static_cast<T>(r.bottom)) {}
			
			template <typename U, typename V>
			rect(const point<U> & min, const point<V> & max) :
				left(static_cast<T>(min.x)), top(static_cast<T>(min.y)), right(static_cast<T>(max.x)), bottom(static_cast<T>(max.y)) {}
			
			template <typename U, typename V>
			rect(const range<U> & h, const range<V> & v) :
				left(static_cast<T>(h.min)), top(static_cast<T>(v.min)), right(static_cast<T>(h.max)), bottom(static_cast<T>(v.max)) {}
			
			template <typename U, typename V>
			rect(const point<U> & pos, const size<V> & sz) :
				left(static_cast<T>(pos.x)), top(static_cast<T>(pos.y)), right(static_cast<T>(pos.x + sz.x)), bottom(static_cast<T>(pos.y + sz.y)) {}
			
			template <typename U>
			rect(const size<U> & sz) :
				left(static_cast<T>(0)), top(static_cast<T>(0)), right(static_cast<T>(sz.x)), bottom(static_cast<T>(sz.y)) {}
			
			template <typename U, typename V, useif<std::is_pod<U>::value, std::is_pod<V>::value>>
			rect(const initializer_list<U> & pt, const initializer_list<V> & sz) :
				left(static_cast<T>(pt.begin()[0])),
				top(static_cast<T>(pt.begin()[1])),
				right(static_cast<T>(pt.begin()[0] + sz.begin()[0])),
				bottom(static_cast<T>(pt.begin()[1] + sz.begin()[1])) {}
			
			bool isEmpty() {
				return width() <= 0 || height() <= 0;
			}
			
			std::make_unsigned_t<T> width() const {
				return std::abs(right - left);
			}
			
			std::make_unsigned_t<T> height() const {
				return std::abs(bottom - top);
			}
			
			std::make_unsigned_t<T> area() const {
				return width() * height();
			}
			
			point<T> pos() const {
				return point<T>(left, top);
			}
			
			point<T> min_pos() const {
				return point<T>(left, top);
			}
			
			point<T> max_pos() const {
				return point<T>(right, bottom);
			}
			
			range<T> horizontal() const {
				return range<T>(left, right);
			}
			
			range<T> vertical() const {
				return range<T>(top, bottom);
			}
			
			point<T> center() const {
				return point<T>(math::avg(left, right), math::avg(top, bottom));
			}
			
			math::size<T> size() const {
				return math::size<T>(width(), height());
			}
			
			void set_width(T value) {
				right = left + value;
			}
			
			void set_height(T value) {
				bottom = top + value;
			}
			
			void set_pos(T left, T top) {
				this->right += left - this->left;
				this->bottom += top - this->top;
				this->left = left;
				this->top = top;
			}
			
			void set_pos(const point<T> & pos) {
				this->right += pos.x - this->left;
				this->bottom += pos.y - this->top;
				this->left = pos.x;
				this->top = pos.y;
			}
			
			void set_max_pos(T right, T bottom) {
				this->left += right - this->right;
				this->top += bottom - this->bottom;
				this->right = right;
				this->bottom = bottom;
			}
			
			void set_max_pos(const point<T> & pos) {
				this->left += pos.x - this->right;
				this->top += pos.y - this->bottom;
				this->right = pos.x;
				this->bottom = pos.y;
			}
			
			void set_size(T width, T height) {
				right = left + width;
				bottom = top + height;
			}
			
			void set_size(const math::size<T> & s) {
				right = left + s.x;
				bottom = top + s.y;
			}
			
			void set_placement(T left, T top, T width, T height) {
				this->left = left;
				this->top = top;
				this->right = left + width;
				this->bottom = top + height;
			}
			
			void set_placement(const point<T> & pos, const math::size<T> & s) {
				this->left = pos.x;
				this->top = pos.y;
				this->right = pos.x + s.x;
				this->bottom = pos.y + s.y;
			}
			
			void set(T left, T top, T right, T bottom) {
				this->left = left;
				this->top = top;
				this->right = right;
				this->bottom = bottom;
			}
			
			template <typename U>
			void set(const rect<U> & r) {
				left = r.left;
				top = r.top;
				right = r.right;
				bottom = r.bottom;
			}
			
			template <typename U>
			void intersect(const rect<U> & r) {
				if (r.left > left) {
					left = r.left;
				}
				
				if (r.top > top) {
					top = r.top;
				}
				
				if (r.right < right) {
					right = r.right;
				}
				
				if (r.bottom < bottom) {
					bottom = r.bottom;
				}
				
				if (width() < 0) {
					right = left;
				}
				
				if (height() < 0) {
					bottom = top;
				}
			}
			
			template <typename U>
			void include(const rect<U> & r) {
				if (r.left < left) {
					left = r.left;
				}
				
				if (r.top < top) {
					top = r.top;
				}
				
				if (r.right > right) {
					right = r.right;
				}
				
				if (r.bottom > bottom) {
					bottom = r.bottom;
				}
			}
			
			template <typename U>
			int compare(const rect<U> & r) const {
				return left == r.left && top == r.top && right == r.right && bottom == r.bottom ? 0 : icomp(center(), r.center());
			}
			
			template <typename U>
			int compare(const point<U> & pt) const {
				return contains(pt) ? 0 : icomp(center(), pt);
			}
			
			template <typename U>
			bool contains(const rect<U> & r) const {
				return contains(r.min_pos()) && contains(r.max_pos());
			}
			
			template <typename U>
			bool contains(const point<U> & pt) const {
				return between(pt.x, left, right) && between(pt.y, top, bottom);
			}
			
			template <typename U>
			bool intersects(const rect<U> & r) const {
				return (r.right >= left && r.left <= right) && (r.bottom >= top && r.top <= bottom);
			}
			
			rect & resize(T dx, T dy) {
				left -= dx;
				top -= dy;
				right += dx;
				bottom += dy;
				
				return *this;
			}
			
			rect & resize(T delta) {
				left -= delta;
				top -= delta;
				right += delta;
				bottom += delta;
				
				return *this;
			}
			
			void add(T x, T y) {
				left += x;
				right += x;
				top += y;
				bottom += y;
			}
			
			void subtract(T x, T y) {
				left -= x;
				right -= x;
				top -= y;
				bottom -= y;
			}
			
			template <typename U>
			void add(const point<U> & pt) {
				left += pt.x;
				right += pt.x;
				top += pt.y;
				bottom += pt.y;
			}
			
			template <typename U>
			void subtract(const point<U> & pt) {
				left -= pt.x;
				right -= pt.x;
				top -= pt.y;
				bottom -= pt.y;
			}
			
			rect & moveTo(T x, T y) {
				add(x - left, y - top);
				return *this;
			}
			
			template <typename U>
			rect & moveTo(const point<U> & pt) {
				add(pt.x - left, pt.y - top);
				return *this;
			}
			
			rect & moveToZero() {
				subtract(left, top);
				return *this;
			}
			
			rect & centralize() {
				subtract(min_pos() + size() / static_cast<T>(2));
				return *this;
			}
			
			rect & set_minimum(T x, T y) {
				if (left < x) {
					right += x - left;
					left = x;
				}
				
				if (top < y) {
					bottom += y - top;
					top = y;
				}
				
				return *this;
			}
			
			rect & set_maximum(T x, T y) {
				if (right > x) {
					left -= right - x;
					right = x;
				}
				
				if (bottom > y) {
					top -= bottom - y;
					bottom = y;
				}
				
				return *this;
			}
			
			template <typename U>
			rect & set_minimum(const point<U> & pt) {
				return set_minimum(pt.x, pt.y);
			}
			
			template <typename U>
			rect & set_maximum(const point<U> & pt) {
				return set_maximum(pt.x, pt.y);
			}
			
			template <typename U>
			rect & operator =(const rect<U> & r) {
				set(r);
				return *this;
			}
			
			template <typename U>
			rect & operator &=(const rect<U> & r) {
				intersect(r);
				return *this;
			}
			
			template <typename U>
			rect & operator |=(const rect<U> & r) {
				include(r);
				return *this;
			}
			
			template <typename U>
			bool operator ==(const rect<U> & r) const {
				return compare(r) == 0;
			}
			
			template <typename U>
			bool operator !=(const rect<U> & r) const {
				return compare(r) != 0;
			}
			
			template <typename U>
			bool operator >=(const rect<U> & r) const {
				return compare(r) >= 0;
			}
			
			template <typename U>
			bool operator <=(const rect<U> & r) const {
				return compare(r) <= 0;
			}
			
			template <typename U>
			bool operator >(const rect<U> & r) const {
				return compare(r) > 0;
			}
			
			template <typename U>
			bool operator <(const rect<U> & r) const {
				return compare(r) < 0;
			}
			
			T & operator [] (int i) {
				return data[i];
			}
			
			const T & operator [] (int i) const {
				return data[i];
			}
			
			operator array<T, 4> &() {
				return data;
			}
			
			operator const array<T, 4> &() const {
				return data;
			}
			
			template <typename U>
			rect & operator +=(const point<U> & pt) {
				add(pt);
				return *this;
			}
			
			template <typename U>
			rect & operator -=(const point<U> & pt) {
				subtract(pt);
				return *this;
			}
			
			template <typename U>
			rect operator +(const point<U> & pt) const {
				return rect(*this) += pt;
			}
			
			template <typename U>
			rect operator -(const point<U> & pt) const {
				return rect(*this) -= pt;
			}
		};
		
		template <typename T, typename U>
		rect<T> operator &(const rect<T> & r1, const rect<U> & r2) {
			return rect<T>(r1) &= r2;
		}
		
		template <typename T, typename U>
		rect<T> operator |(const rect<T> & r1, const rect<U> & r2) {
			return rect<T>(r1) |= r2;
		}
		
		template <typename T>
		T min_side(const rect<T> & r) {
			T min = r[0];
			
			for (uint i = 0; i < 4; i++) {
				if (r[i] < min) {
					min = r[i];
				}
			}
			
			return min;
		}
		
		template <typename T>
		rect<T> centered_rect(const point<T> & pos, const size<T> & s) {
			const auto hs = s / static_cast<T>(2);
			return {pos.x - hs.x, pos.y - hs.y, pos.x + hs.x, pos.y + hs.y};
		}
		
		template <typename T>
		rect<T> centered_square(const point<T> & pos, T size) {
			const T hs = size / 2;
			return {pos.x - hs, pos.y - hs, pos.x + hs, pos.y + hs};
		}
		
		typedef rect<byte> byte_rect;
		typedef rect<int> int_rect;
		typedef rect<uint> uint_rect;
		typedef rect<long> long_rect;
		typedef rect<float> float_rect;
		
		typedef size<byte> byte_size;
		typedef size<int> int_size;
		typedef size<uint> uint_size;
		typedef size<long> long_size;
		typedef size<float> float_size;
	}
}

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------

#pragma once

#ifndef POINT_H
#define POINT_H

//---------------------------------------------------------------------------

#include <math/math.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<typename T>
		class point
		{
		public:
			point() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}
			
			point(const point & pt) : x(pt.x), y(pt.y) {}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point(U val)                : x(static_cast<T>(val)), y(static_cast<T>(val)) {}
			
			template<typename Tx, typename Ty, useif<std::is_pod<Tx>::value, std::is_pod<Ty>::value>>
			point(Tx x, Ty y)           : x(static_cast<T>(x)), y(static_cast<T>(y)) {}
			
			template<typename U, useif<std::is_convertible<U, T>::value>>
			point(const U (& pt)[2])        : x(static_cast<T>(pt[0])), y(static_cast<T>(pt[1])) {}
			
			template<typename U, useif<not_same_type<T, U>::value, std::is_convertible<U, T>::value>>
			explicit point(const point<U> & pt) : x(static_cast<T>(pt.x)), y(static_cast<T>(pt.y)) {}
			
			template<typename Tx, typename Ty, useif<std::is_pod<Tx>::value, std::is_pod<Ty>::value>>
			void set(Tx x, Ty y) {
				this->x = static_cast<T>(x);
				this->y = static_cast<T>(y);
			}
			
			template<typename U>
			void set(const point<U> & pt) {
				x = static_cast<T>(pt.x);
				y = static_cast<T>(pt.y);
			}
			
			template<typename U>
			void add(const point<U> & pt) {
				x += static_cast<T>(pt.x);
				y += static_cast<T>(pt.y);
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			void add(U value) {
				x += static_cast<T>(value);
				y += static_cast<T>(value);
			}
			
			template<typename U>
			void subtract(const point<U> & pt) {
				x -= static_cast<T>(pt.x);
				y -= static_cast<T>(pt.y);
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			void subtract(U value) {
				x -= static_cast<T>(value);
				y -= static_cast<T>(value);
			}
			
			template<typename U>
			void multiply(const point<U> & pt) {
				x = static_cast<T>(x * pt.x);
				y = static_cast<T>(y * pt.y);
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			void multiply(U value) {
				x = static_cast<T>(x * value);
				y = static_cast<T>(y * value);
			}
			
			template<typename U>
			void divide(const point<U> & pt) {
				x = static_cast<T>(x / pt.x);
				y = static_cast<T>(y / pt.y);
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			void divide(U value) {
				x = static_cast<T>(x / value);
				y = static_cast<T>(y / value);
			}
			
			void invert() {
				x = -x;
				y = -y;
			}
			
			/*=================================
	
			 ^ y
			\|
			 |
			 |\
			 | \   *C
			 |  \
			 |   *B
			 |    \
			 | *A  \
			-+-----------> x
			0|       \
	
				C > B and A < B
	
			=================================*/
			
			template<typename U>
			int compare(const point<U> & pt) const {
				return (x - pt.x) + (y - pt.y);
			}
			
			template<typename U>
			point middle(const point<U> & pt) {
				return {avg(x, pt.x), avg(y, pt.y)};
			}
			
			template<typename U>
			point & operator =(const point<U> & pt) {
				set(pt);
				return *this;
			}
			
			template<typename U>
			bool operator ==(const point<U> & pt) const {
				return x == pt.x && y == pt.y;
			}
			
			template<typename U>
			bool operator !=(const point<U> & pt) const {
				return x != pt.x || y != pt.y;
			}
			
			template<typename U>
			bool operator >(const point<U> & pt) const {
				return x + y > pt.x + pt.y;
			}
			
			template<typename U>
			bool operator <(const point<U> & pt) const {
				return x + y < pt.x + pt.y;
			}
			
			template<typename U>
			bool operator >=(const point<U> & pt) const {
				return x + y >= pt.x + pt.y;
			}
			
			template<typename U>
			bool operator <=(const point<U> & pt) const {
				return x + y <= pt.x + pt.y;
			}
			
			T & operator [](int axis) {
				return data[axis];
			}
			
			const T & operator [](int axis) const {
				return data[axis];
			}
			
			template<typename U>
			point & operator +=(const point<U> & pt) {
				add(pt);
				return *this;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point & operator +=(U value) {
				add(value);
				return *this;
			}
			
			template<typename U>
			point & operator -=(const point<U> & pt) {
				subtract(pt);
				return *this;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point & operator -=(U value) {
				subtract(value);
				return *this;
			}
			
			template<typename U>
			point & operator *=(const point<U> & pt) {
				multiply(pt);
				return *this;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point & operator *=(U value) {
				multiply(value);
				return *this;
			}
			
			template<typename U>
			point & operator /=(const point<U> & pt) {
				divide(pt);
				return *this;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point & operator /=(U value) {
				divide(value);
				return *this;
			}
			
			template<typename U>
			point operator +(const point<U> & pt) const {
				return point(*this) += pt;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point operator +(U value) const {
				return point(*this) += value;
			}
			
			template<typename U>
			point operator -(const point<U> & pt) const {
				return point(*this) -= pt;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point operator -(U value) const {
				return point(*this) -= value;
			}
			
			template<typename U>
			point operator *(const point<U> & pt) const {
				return point(*this) *= pt;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point operator *(U value) const {
				return point(*this) *= value;
			}
			
			template<typename U>
			point operator /(const point<U> & pt) const {
				return point(*this) /= pt;
			}
			
			template<typename U, useif<std::is_pod<U>::value>>
			point operator /(U value) const {
				return point(*this) /= value;
			}
			
			operator array<T, 2> &() & {
				return data;
			}
			
			operator const array<T, 2> &() const & {
				return data;
			}
			
			operator array<T, 2> &&() && {
				return move(data);
			}
			
			union
			{
				struct
				{
					T x, y;
				};
				
				array<T, 2> data;
			};
		};
		
		typedef point<long> long_point;
		typedef point<int> int_point;
		typedef point<float> float_point;
	}
}

//---------------------------------------------------------------------------
#endif

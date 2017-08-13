//---------------------------------------------------------------------------

#pragma once

#ifndef RANGE_H
#define RANGE_H

//---------------------------------------------------------------------------

#include <math/math.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<typename T>
		class range;
		
		template<typename T>
		class range_iterator
		{
			static_assert(std::is_integral<T>::value, "Can iterate only integral range!");
		
		public:
			using iterator_category = std::random_access_iterator_tag;
			
			range_iterator(T val) : val(val) {}
			
			bool operator ==(const range_iterator & it) const {
				return val == it.val;
			}
			
			bool operator !=(const range_iterator & it) const {
				return val != it.val;
			}
			
			T operator *() const {
				return val;
			}
			
			range_iterator & operator ++() {
				++val;
				return *this;
			}
			
			range_iterator & operator --() {
				--val;
				return *this;
			}
			
			range_iterator operator ++(int) {
				return {val++};
			}
			
			range_iterator operator --(int) {
				return {val--};
			}
			
			range_iterator & operator +=(T diff) {
				val += diff;
				return *this;
			}
			
			range_iterator & operator -=(T diff) {
				val -= diff;
				return *this;
			}
			
			range_iterator operator +(T diff) {
				return {val + diff};
			}
			
			range_iterator operator -(T diff) {
				return {val - diff};
			}
			
			T operator [](T diff) const {
				return val + diff;
			}
		
		protected:
			T val;
		};
		
		template<typename T>
		class range
		{
		public:
			using iterator = range_iterator<T>;
			using const_iterator = range_iterator<T>;
			
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			
			T min, max;
			
			range() : min(0), max(0) {}
			
			range(T min, T max) : min(min), max(max) {}
			
			template<typename U>
			range(const range<U> & range) : min(range.min), max(range.max) {}
			
			bool isEmpty() const {
				return length() == 0;
			}
			
			T length() const {
				return max - min;
			}
			
			T center() const {
				return math::avg(min, max);
			}
			
			void set(T min, T max) {
				this->min = min;
				this->max = max;
			}
			
			template<typename U>
			void set(const range<U> & range) {
				min = range.min;
				max = range.max;
			}
			
			void setWidth(T value) {
				max = min + value;
			}
			
			template<typename U>
			void intersect(const range<U> & range) {
				if(range.min > min) {
					min = range.min;
				}
				
				if(range.max < max) {
					max = range.max;
				}
				
				if(length() < 0) {
					max = min;
				}
			}
			
			template<typename U>
			void include(const range<U> & range) {
				if(range.min < min) {
					min = range.min;
				}
				
				if(range.max > max) {
					max = range.max;
				}
			}
			
			template<typename U>
			int compare(const range<U> & range) const {
				return (min == range.min && max == range.max) ? 0 : icomp(length(), range.length());
			}
			
			int compare(T value) const {
				return (value > max) ? 1 : (value < min) ? -1 : 0;
			}
			
			template<typename U>
			bool contains(const range<U> & range) const {
				return (compare(range.min) & compare(range.max)) == 0;
			}
			
			bool contains(T value) const {
				return between(value, min, max);
			}
			
			template<typename U>
			bool intersects(const range<U> & range) const {
				return range.max >= min && range.min <= max;
			}
			
			template<typename U>
			range & operator =(const range<U> & range) {
				set(range);
				return *this;
			}
			
			template<typename U>
			range & operator &=(const range <U> & range) {
				intersect(range);
				return *this;
			}
			
			template<typename U>
			range & operator |=(const range <U> & range) {
				include(range);
				return *this;
			}
			
			template<typename U>
			bool operator ==(const range <U> & range) {
				return compare(range) == 0;
			}
			
			template<typename U>
			bool operator !=(const range <U> & range) {
				return compare(range) != 0;
			}
			
			template<typename U>
			bool operator >=(const range <U> & range) {
				return compare(range) >= 0;
			}
			
			template<typename U>
			bool operator <=(const range <U> & range) {
				return compare(range) <= 0;
			}
			
			template<typename U>
			bool operator >(const range <U> & range) {
				return compare(range) > 0;
			}
			
			template<typename U>
			bool operator <(const range <U> & range) {
				return compare(range) < 0;
			}
			
			template<typename U>
			range operator &(const range <U> & range) {
				return range(*this) &= range;
			}
			
			template<typename U>
			range operator |(const range <U> & range) {
				return range(*this) |= range;
			}
			
			iterator begin() {
				return {min};
			}
			
			const_iterator begin() const {
				return {min};
			}
			
			iterator end() {
				return {max};
			}
			
			const_iterator end() const {
				return {max};
			}
			
			reverse_iterator rbegin() {
				return end();
			}
			
			const_reverse_iterator rbegin() const {
				return end();
			}
			
			reverse_iterator rend() {
				return begin();
			}
			
			const_reverse_iterator rend() const {
				return begin();
			}
			
			const_iterator cbegin() const {
				return begin();
			}
			
			const_iterator cend() const {
				return end();
			}
			
			const_reverse_iterator crbegin() const {
				return rbegin();
			}
			
			const_reverse_iterator crend() const {
				return rend();
			}
		};
		
		using IntRange = range<int>;
		using LongRange = range<long>;
		using FloatRange = range<float>;
	}
}

//---------------------------------------------------------------------------
#endif

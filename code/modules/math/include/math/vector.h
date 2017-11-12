//---------------------------------------------------------------------------

#pragma once

#ifndef VECTOR_H
#define VECTOR_H

//---------------------------------------------------------------------------

#include <core/intrinsic/Intrinsic.h>
#include <math/math.h>
#include <core/String.h>
#include <core/memory/aligned.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template <typename T, bool>
		struct vector;
		
		template <typename T, size_t Mask>
		using VectorMaskAxis = IntrinsicMask<T, IntrinMax, Mask>;
		
		template <typename T, size_t Mask>
		using VectorSignAxis = IntrinsicMask<T, IntrinSignmask, Mask>;
		
		template <int ... Sequence>
		struct SwizzleSequence {};
		
		template <typename T, class S, bool Assignable = S::assignable>
		struct SwizzleVector {};
		
		template <int X, int Y>
		struct SwizzleSequence<X, Y>
		{
			static constexpr bool assignable = (X != Y);
			
			template <int Component>
			using get_component = std::integral_constant<int, (X == Component) ? 0 : (Y == Component) ? 1 : -1>;
			
			enum : int
			{
				HX = get_component<0>::value,
				HY = get_component<1>::value,
				HZ = get_component<2>::value,
				HW = get_component<3>::value,
				
				MX = X >= 0 ? 1 : 0,
				MY = Y >= 0 ? 1 : 0,
				MZ = 0,
				MW = 0,
				SX = X >= 0 ? X : 0,
				SY = Y >= 0 ? Y : 0,
				SZ = 0,
				SW = 0,
				
				BX = HX >= 0 ? 1 : 0,
				BY = HY >= 0 ? 1 : 0,
				BZ = HZ >= 0 ? 1 : 0,
				BW = HW >= 0 ? 1 : 0,
				AX = HX >= 0 ? HX : 0,
				AY = HY >= 0 ? HY : 0,
				AZ = HZ >= 0 ? HZ : 0,
				AW = HW >= 0 ? HW : 0
			};
		};
		
		template <int X, int Y, int Z>
		struct SwizzleSequence<X, Y, Z>
		{
			static constexpr bool assignable = ((X != Y) && (X != Z) && (Y != Z));
			
			template <int Component>
			using get_component = std::integral_constant<int, (X == Component) ? 0 : (Y == Component) ? 1 : (Z == Component) ? 2 : -1>;
			
			enum : int
			{
				HX = get_component<0>::value,
				HY = get_component<1>::value,
				HZ = get_component<2>::value,
				HW = get_component<3>::value,
				
				MX = X >= 0 ? 1 : 0,
				MY = Y >= 0 ? 1 : 0,
				MZ = Z >= 0 ? 1 : 0,
				MW = 0,
				SX = X >= 0 ? X : 0,
				SY = Y >= 0 ? Y : 0,
				SZ = Z >= 0 ? Z : 0,
				SW = 0,
				
				BX = HX >= 0 ? 1 : 0,
				BY = HY >= 0 ? 1 : 0,
				BZ = HZ >= 0 ? 1 : 0,
				BW = HW >= 0 ? 1 : 0,
				AX = HX >= 0 ? HX : 0,
				AY = HY >= 0 ? HY : 0,
				AZ = HZ >= 0 ? HZ : 0,
				AW = HW >= 0 ? HW : 0
			};
		};
		
		template <int X, int Y, int Z, int W>
		struct SwizzleSequence<X, Y, Z, W>
		{
			static constexpr bool assignable = ((X != Y) && (X != Z) && (X != W) && (Y != Z) && (Y != W) && (Z != W));
			
			template <int Component>
			using get_component = std::integral_constant<int, (X == Component) ? 0 : (Y == Component) ? 1 : (Z == Component) ? 2 : (W == Component) ? 3 : -1>;
			
			enum : int
			{
				HX = get_component<0>::value,
				HY = get_component<1>::value,
				HZ = get_component<2>::value,
				HW = get_component<3>::value,
				
				MX = X >= 0 ? 1 : 0,
				MY = Y >= 0 ? 1 : 0,
				MZ = Z >= 0 ? 1 : 0,
				MW = W >= 0 ? 1 : 0,
				SX = X >= 0 ? X : 0,
				SY = Y >= 0 ? Y : 0,
				SZ = Z >= 0 ? Z : 0,
				SW = W >= 0 ? W : 0,
				
				BX = HX >= 0 ? 1 : 0,
				BY = HY >= 0 ? 1 : 0,
				BZ = HZ >= 0 ? 1 : 0,
				BW = HW >= 0 ? 1 : 0,
				AX = HX >= 0 ? HX : 0,
				AY = HY >= 0 ? HY : 0,
				AZ = HZ >= 0 ? HZ : 0,
				AW = HW >= 0 ? HW : 0
			};
		};
		
		template <typename T, int ... Mask>
		using Swizzle = SwizzleVector<T, SwizzleSequence<Mask...>>;
		
		template<typename T, bool Aligned>
		struct vector_impl;
		
		template<typename T>
		struct alignas(sizeof(T) * 4) vector_impl<T, false>
		{
			using Intrin = Intrinsic<T, 4>;
			using IntrinType = intrin_t<T, 4>;
			
			member_cast(v, array<T, 4>);
			
			vector_impl(IntrinType intrin) : v(IntrinData<T, 4>::cast(intrin)) {}
			vector_impl(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
			
			union
			{
				struct
				{
					T x, y, z, w;
				};
				
				std::array<T, 4> v;
				IntrinData<T, 4> data;
				T elements[4];
				
				Swizzle<T, 0, 1, 2> xyz;
				Swizzle<T, 0, 2, 1> xzy;
				Swizzle<T, 1, 2, 0> yzx;
				Swizzle<T, 1, 0, 2> yxz;
				Swizzle<T, 2, 0, 1> zxy;
				Swizzle<T, 2, 1, 0> zyx;
			};
			
			operator IntrinType() const {
				return Intrin::load(x, y, z, w);
			}
		};
		
		template<typename T>
		struct alignas(sizeof(T) * 4) vector_impl<T, true>
		{
			using Intrin = Intrinsic<T, 4>;
			using IntrinType = intrin_t<T, 4>;
			
			member_cast(v, array<T, 4>);
			
			vector_impl(IntrinType intrin) : intrinsic(intrin) {}
			vector_impl(T x, T y, T z, T w) : intrinsic(Intrin::load(x, y, z, w)) {}
			
			union
			{
				struct
				{
					T x, y, z, w;
				};
				
				std::array<T, 4> v;
				IntrinData<T, 4> data;
				T elements[4];
				IntrinType intrinsic;
				
				Swizzle<T, 0, 1, 2> xyz;
				Swizzle<T, 0, 2, 1> xzy;
				Swizzle<T, 1, 2, 0> yzx;
				Swizzle<T, 1, 0, 2> yxz;
				Swizzle<T, 2, 0, 1> zxy;
				Swizzle<T, 2, 1, 0> zyx;
			};
			
			operator IntrinType() const {
				return intrinsic;
			}
		};
		
		template<class T, bool numeric = std::is_arithmetic<T>::value>
		struct vector_constants {};
		
		template <typename T, bool Aligned = false>
		struct alignas(sizeof(T) * 4) vector : vector_impl<T, Aligned>, aligned_alloc
		{
			using intrin_type = intrin_t<T, 4>;
			using methods = Intrinsic<T, 4>;
			using return_type = vector<T, Aligned>;
			using impl_type = vector_impl<T, Aligned>;
			using constants = vector_constants<T>;
			
			vector() : vector(constants::identity) {}
			
			vector(const intrin_type & v) : impl_type(v) {}
			
			template<bool A>
			vector(const vector<T, A> & v) : impl_type(v) {}
			
			template<bool A>
			vector(const vector<T, A> & v, T a) : impl_type(v) { this->w = a; }
			
			template<bool A>
			vector(const vector<T, A> & v, T a, T b) : impl_type(v) {
				this->z = a;
				this->w = b;
			}
			
			template<bool A>
			vector(T a, const vector<T, A> & v) : impl_type(methods::template shuffle<3, 0, 1, 2>(v)) { this->x = a; }
			
			template<bool A>
			vector(T a, T b, const vector<T, A> & v) : impl_type(methods::template shuffle<2, 3, 0, 1>(v)) {
				this->x = a;
				this->y = b;
			}
			
			template<bool A>
			vector(T a, const vector<T, A> & v, T b) : impl_type(methods::template shuffle<3, 0, 1, 2>(v)) {
				this->x = a;
				this->w = b;
			}
			
			template <class U, bool A, useif<!is_same<T, U>::value>>
			vector(const vector<U, A> & v) : impl_type(intrin_cvt<intrin_type>(v)) {}
			
			template <class U, bool A, useif<!is_same<T, U>::value>>
			vector(const vector<U, A> & v, T a)      : impl_type(intrin_cvt<intrin_type>(v)) { this->w = a; }
			
			template <class U, bool A, useif<!is_same<T, U>::value>>
			vector(const vector<U, A> & v, T a, T b) : impl_type(intrin_cvt<intrin_type>(v)) {
				this->z = a;
				this->w = b;
			}
			
			template <class U, bool A, useif<!is_same<T, U>::value>>
			vector(T a, const vector<U, A> & v)      : impl_type(intrin_cvt<intrin_type>(methods::template shuffle<3, 0, 1, 2>(v))) { this->x = a; }
			
			template <class U, bool A, useif<!is_same<T, U>::value>>
			vector(T a, T b, const vector<U, A> & v) : impl_type(intrin_cvt<intrin_type>(methods::template shuffle<2, 3, 0, 1>(v))) {
				this->x = a;
				this->y = b;
			}
			
			template <class U, bool A, useif<!is_same<T, U>::value>>
			vector(T a, const vector<U, A> & v, T b) : impl_type(intrin_cvt<intrin_type>(methods::template shuffle<3, 0, 1, 2>(v))) {
				this->x = a;
				this->w = b;
			}
			
			template <class U, useif<can_cast<U, vector>::value>>
			vector(const U & v) : vector(constants::identity) {
				cast::to(*this, v);
			}
			
			vector(const T * v) : vector(methods::load(v)) {}
			
			vector(T value) : impl_type({value, value, value, value}) {}
			
			vector(T x, T y, T z = 0, T w = 0) : impl_type(x, y, z, w) {}
			
			template<bool A = Aligned, useif<A>>
			vector & operator =(const vector & vec) {
				this->intrinsic = vec.intrinsic;
				return *this;
			}
			
			template<bool A = Aligned, skipif<A>>
			vector & operator =(const vector & vec) {
				this->v = vec.v;
				return *this;
			}
			
			template<bool A = Aligned, useif<A>>
			vector & operator =(const vector<T, !A> & vec) {
				this->intrinsic = vec;
				return *this;
			}
			
			template<bool A = Aligned, skipif<A>>
			vector & operator =(const vector<T, !A> & vec) {
				this->x = vec.x;
				this->y = vec.y;
				this->z = vec.z;
				this->w = vec.w;
				return *this;
			}
			
			template<bool A = Aligned, useif<A>>
			vector & operator =(const intrin_type & intrin) {
				this->intrinsic = intrin;
				return *this;
			}
			
			template<bool A = Aligned, skipif<A>>
			vector & operator =(const intrin_type & intrin) {
				this->data = intrin;
				return *this;
			}
			
			template <class U, useif<can_cast<U, vector>::value>>
			vector & operator =(const U & v) {
				cast::to(*this, v);
				return *this;
			}
			
			vector & set(const T * v) {
				memory<T>::move(this->v, v);
				return *this;
			}
			
			vector & set(T x, T y, T z, T w) {
				this->x = x, this->y = y, this->z = z, this->w = w;
				return *this;
			}
			
			vector & fill(T value) {
				return *this = methods::fill(value);
			}
			
			return_type operator +() const {
				return *this;
			}
			
			return_type operator -() const {
				return methods::negate(*this);
			}
			
			vector & abs() {
				return *this = methods::abs(*this);
			}
			
			vector & round() {
				return *this = methods::round(*this);
			}
			
			vector & floor() {
				return *this = methods::floor(*this);
			}
			
			vector & invert() {
				return *this = methods::invert(*this);
			}
			
			return_type inverse() const {
				return methods::invert(*this);
			}
			
			void rounded(vector & vec) const {
				vec = methods::round(*this);
			}
			
			void inversed(vector & vec) const {
				vec = methods::invert(*this);
			}
			
			vector & operator +=(const vector & v) {
				return *this = methods::add(*this, v);
			}
			
			vector & operator -=(const vector & v) {
				return *this = methods::sub(*this, v);
			}
			
			vector & operator *=(const vector & v) {
				return *this = methods::mul(*this, v);
			}
			
			vector & operator /=(const vector & v) {
				return *this = methods::div(*this, v);
			}
			
			vector & operator +=(T value) {
				*this = methods::add(*this, methods::fill(value));
				return *this;
			}
			
			vector & operator -=(T value) {
				*this = methods::sub(*this, methods::fill(value));
				return *this;
			}
			
			vector & operator *=(T k) {
				*this = methods::mul(*this, methods::fill(k));
				return *this;
			}
			
			vector & operator /=(T k) {
				if (k == 0) {
					return *this = math::constants<vector<T >>::infinity;
				}
				
				*this = methods::div(*this, methods::fill(k));
				return *this;
			}
			
			return_type operator &(const vector & v) const {
				return methods::bit_and(*this, v);
			}
			
			return_type operator |(const vector & v) const {
				return methods::bit_or(*this, v);
			}
			
			return_type operator ^(const vector & v) const {
				return methods::bit_xor(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type operator >>(const vector & v) const {
				return methods::bit_shr(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type operator <<(const vector & v) const {
				return methods::bit_shl(*this, v);
			}
			
			return_type bit_and(const vector & v) const {
				return methods::bit_and(*this, v);
			}
			
			return_type bit_or(const vector & v) const {
				return methods::bit_or(*this, v);
			}
			
			return_type bit_xor(const vector & v) const {
				return methods::bit_xor(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type shift(const vector & v) const {
				return methods::bit_shr(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type unshift(const vector & v) const {
				return methods::bit_shl(*this, v);
			}
			
			template <int I, class U = T, useif<std::is_integral<U>::value>>
			return_type shift() const /* >> */
			{
				return methods::template bit_shr<I>(*this);
			}
			
			template <int I, class U = T, useif<std::is_integral<U>::value>>
			return_type unshift() const /* << */
			{
				return methods::template bit_shl<I>(*this);
			}
			
			return_type dot(const vector & v) const {
				return methods::fill_sum(*this * v);
			}
			
			T dot1(const vector & v) const {
				return methods::sum(*this * v);
			}
			
			return_type cross(const vector & v) const {
				return
					shuffle<1, 2, 0, 3>() * v.template shuffle<2, 0, 1, 3>() -
						shuffle<2, 0, 1, 3>() * v.template shuffle<1, 2, 0, 3>();
			}
			
			bool operator ==(const vector & v) const {
				return methods::equal(*this, v);
			}
			
			bool operator !=(const vector & v) const {
				return methods::notequal(*this, v);
			}
			
			T & operator [](size_t index) {
				return this->v[index];
			}
			
			const T & operator [](size_t index) const {
				return this->v[index];
			}
			
			T & operator [](int index) {
				return this->v[index];
			}
			
			const T & operator [](int index) const {
				return this->v[index];
			}
			
			T magnitudeSq() const {
				return dot1(*this);
			}
			
			return_type magnitudeSqVector() const {
				return dot(*this);
			}
			
			T magnitude() const {
				return std::sqrt(magnitudeSq());
			}
			
			return_type magnitudeVector() const {
				return methods::sqrt(magnitudeSqVector());
			}
			
			vector & normalize() & {
				return *this = methods::div(*this, magnitudeVector());
			}
			
			vector && normalize() && {
				return std::forward<vector>(*this = methods::div(*this, magnitudeVector()));
			}
			
			return_type normalized() const {
				return methods::div(*this, methods::fill(magnitude()));
			}
			
			inline T max() const {
				return_type v = methods::abs(*this);
				return std::max({v.x, v.y, v.z});
			}
			
			inline return_type abs() const {
				return methods::abs(*this);
			}
			
			inline return_type sqr() const {
				return methods::sqr(*this);
			}
			
			inline T sum() const {
				return methods::sum(*this);
			}
			
			inline return_type fill_sum() const {
				return methods::fill_sum(*this);
			}
			
			inline T square_distance_to(const vector & p) const {
				return methods::sum(methods::sqr(methods::sub(*this, p)));
			}
			
			inline auto distance_to(const vector & p) const {
				return std::sqrt(square_distance_to(p));
			}
			
			inline T distance_to_axis(const vector & p, int axis) const {
				return std::abs(this->v[axis] - p[axis]);
			}
			
			inline int get_main_axis() const {
				return this->x >= this->y ? 0 : this->y >= this->z ? 1 : 2;
			}
			
			template <byte X, byte Y, byte Z, byte W, useif<(X < 2 && Y < 2 && Z < 2 && W < 2)>>
			return_type mask() const // select some components (e.g. if X == 1 then result.x = v.x else result.x = 0)
			{
				return methods::bit_and(VectorMaskAxis<T, mk_mask4(X, Y, Z, W)>::get(), *this);
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			return_type mask_axis() const // set all components of a array_list to zero except of one
			{
				return methods::bit_and(VectorMaskAxis<T, bitmask<Axis>::value>::get(), *this);
			}
			
			vector mask_x() const {
				return mask_axis<0>();
			}
			
			return_type mask_y() const {
				return mask_axis<1>();
			}
			
			return_type mask_z() const {
				return mask_axis<2>();
			}
			
			return_type mask_w() const {
				return mask_axis<3>();
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			return_type clear_axis() const // set a single component to zero
			{
				return methods::bit_and(VectorMaskAxis<T, 0xF ^ bitmask<Axis>::value>::get(), *this);
			}
			
			return_type clear_x() const {
				return clear_axis<0>();
			}
			
			return_type clear_y() const {
				return clear_axis<1>();
			}
			
			return_type clear_z() const {
				return clear_axis<2>();
			}
			
			return_type clear_w() const {
				return clear_axis<3>();
			}
			
			template <byte X, byte Y, byte Z, byte W, useif<(X < 2 && Y < 2 && Z < 2 && W < 2)>>
			return_type negate() const // negate some components (e.g. if X == 1 then result.x = -v.x else result.x = v.x)
			{
				return methods::bit_xor(VectorSignAxis<T, mk_mask4(X, Y, Z, W)>::get(), *this);
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			return_type negate_axis() const // negate one component
			{
				return methods::bit_xor(VectorSignAxis<T, bitmask<Axis>::value>::get(), *this);
			}
			
			return_type negate_x() const {
				return negate_axis<0>();
			}
			
			return_type negate_y() const {
				return negate_axis<1>();
			}
			
			return_type negate_z() const {
				return negate_axis<2>();
			}
			
			return_type negate_w() const {
				return negate_axis<3>();
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			return_type spread_axis() const // get a array_list filled with a single component of a src array_list
			{
				return shuffle<Axis, Axis, Axis, Axis>();
			}
			
			return_type spread_x() const {
				return spread_axis<0>();
			}
			
			return_type spread_y() const {
				return spread_axis<1>();
			}
			
			return_type spread_z() const {
				return spread_axis<2>();
			}
			
			return_type spread_w() const {
				return spread_axis<3>();
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			vector & add_axis(const vector & vec) {
				this->v[Axis] += vec.mask_axis<Axis>();
				return *this;
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			vector & subtract_axis(const vector & vec) {
				this->v[Axis] -= vec.mask_axis<Axis>();
				return *this;
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			vector & add_axis(T val) {
				this->v[Axis] += val;
				return *this;
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			vector & subtract_axis(T val) {
				this->v[Axis] -= val;
				return *this;
			}
			
			vector & add_axis(const vector & vec, int Axis) {
				this->v[Axis] += vec[Axis];
				return *this;
			}
			
			vector & subtract_axis(const vector & vec, int Axis) {
				this->v[Axis] -= vec[Axis];
				return *this;
			}
			
			vector & add_axis(T val, int Axis) {
				this->v[Axis] += val;
				return *this;
			}
			
			vector & subtract_axis(T val, int Axis) {
				this->v[Axis] -= val;
				return *this;
			}
			
			return_type clamp(T low, T high) {
				return clamp(vector(low), vector(high));
			}
			
			return_type clamp(const vector & low, const vector & high) const {
				return methods::min(methods::max(*this, low), high);
			}
			
			template <byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
			return_type shuffle() const {
				return methods::template shuffle<A, B, C, D>(*this);
			}
			
			template <byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
			return_type shuffle(const vector & v) const {
				return methods::template shuffle2<A, B, C, D>(*this, v);
			}
			
			template <byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
			return_type blend(const vector & v) const {
				return methods::template blend<A, B, C, D>(*this, v);
			}
			
			//---------------------------------------------------------------------------
			
			static return_type minimum(const vector & v1, const vector & v2) {
				return methods::min(v1, v2);
			}
			
			static return_type maximum(const vector & v1, const vector & v2) {
				return methods::max(v1, v2);
			}
			
			static int compare(const vector & v1, const vector & v2) {
				return static_cast<int>(methods::sum(methods::sub(v1, v2)));
			}
			
		};
		
		template<class T>
		struct vector_constants<T, true>
		{
			static api(math) const vector<T> zero;         // [  0,  0,  0,  0 ]
			static api(math) const vector<T> one;          // [  1,  1,  1,  1 ]
			static api(math) const vector<T> two;          // [  2,  2,  2,  2 ]
			static api(math) const vector<T> oneXYZ;       // [  1,  1,  1,  0 ]
			static api(math) const vector<T> twoXYZ;       // [  2,  2,  2,  0 ]
			static api(math) const vector<T> minusOne;     // [ -1, -1, -1, -1 ]
			static api(math) const vector<T> half;         // [ .5, .5, .5, .5 ]
			
			static api(math) const vector<T> positiveX;    // [  1,  0,  0,  0 ]
			static api(math) const vector<T> positiveY;    // [  0,  1,  0,  0 ]
			static api(math) const vector<T> positiveZ;    // [  0,  0,  1,  0 ]
			static api(math) const vector<T> positiveW;    // [  0,  0,  0,  1 ]
			static api(math) const vector<T> negativeX;    // [ -1,  0,  0,  0 ]
			static api(math) const vector<T> negativeY;    // [  0, -1,  0,  0 ]
			static api(math) const vector<T> negativeZ;    // [  0,  0, -1,  0 ]
			static api(math) const vector<T> negativeW;    // [  0,  0,  0, -1 ]
			
			static api(math) const vector<T> & right;
			static api(math) const vector<T> & up;
			static api(math) const vector<T> & forward;
			
			static api(math) const vector<T> & left;
			static api(math) const vector<T> & down;
			static api(math) const vector<T> & back;
			
			static api(math) const vector<T> & identity;
		};
		
		template <typename T, int ... Mask>
		struct SwizzleVector<T, SwizzleSequence<Mask...>, true>
		{
			using IntrinType = intrin_t<T, 4>;
			using Intrin = Intrinsic<T, 4>;
			using S = SwizzleSequence<Mask...>;
			
			SwizzleVector & operator =(const vector<T> & v) {
				*this = Intrin::template blend<S::BX, S::BY, S::BZ, S::BW>(*this, v.template shuffle<S::AX, S::AY, S::AZ, S::AW>());
				return *this;
			}
			
			template <class U, useif<not_same_type<U, T>::value>>
			operator vector<U>() const {
				return vector<T>(Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(*this)));
			}
			
			operator vector<T>() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(*this));
			}
			
			operator IntrinType() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(*this));
			}
		
		protected:
			IntrinType intrinsic;
		};
		
		template <typename T, int ... Mask>
		struct SwizzleVector<T, SwizzleSequence<Mask...>, false>
		{
			using IntrinType = intrin_t<T, 4>;
			using Intrin = Intrinsic<T, 4>;
			using S = SwizzleSequence<Mask...>;
			
			template <class U, useif<not_same_type<U, T>::value>>
			operator vector<U>() const {
				return vector<T>(Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(*this)));
			}
			
			operator vector<T>() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(*this));
			}
			
			operator IntrinType() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(*this));
			}
		
		protected:
			IntrinType intrinsic;
		};

//---------------------------------------------------------------------------
		
		using byte_vector = vector<byte>;
		using int_vector = vector<int>;
		using float_vector = vector<float>;
		
		using bytev = byte_vector;
		using intv = int_vector;
		using floatv = float_vector;
		
		using bvec = vector<byte>;
		using ivec = vector<int>;
		using fvec = vector<float>;
		
		using xmm_vec = vector<float, true>;

#ifdef USE_AVX
		using DoubleVector = vector<double>;
		using doublev = DoubleVector;
		using dvec = vector<double>;
#endif

//---------------------------------------------------------------------------
		
		template <class T, bool A>
		inline vector<T> abs(const vector<T, A> & x) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::abs(x);
		}
		
		template <class T, bool A>
		inline vector<T> sqr(const vector<T, A> & x) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::sqr(x);
		}
		
		template <class T, bool A>
		inline vector<T> sqrt(const vector<T, A> & x) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::sqrt(x);
		}
		
		template <class T, bool A1, bool A2>
		inline vector<T> mod(const vector<T, A1> & x, const vector<T, A2> & y) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::mod(x, y);
		}
		
		template <class T, bool A1, bool A2>
		inline vector<T> avg(const vector<T, A1> & x, const vector<T, A2> & y) {
			return vector<T>::half * (x + y);
		}
		
		template <class T, bool A1, bool A2, bool A3>
		inline vector<T> clamp(const vector<T, A1> & x, const vector<T, A2> & low, const vector<T, A3> & high) {
			return x.clamp(low, high);
		}
		
		template <class T, bool A>
		inline vector<T> invert(const vector<T, A> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::invert(v);
		}
		
		template <class T, bool A>
		inline vector<T> trunc(const vector<T, A> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::trunc(v);
		}
		
		template <class T, bool A>
		inline vector<T> floor(const vector<T, A> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::floor(v);
		}
		
		template <class T, bool A>
		inline vector<T> ceil(const vector<T, A> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::ceil(v);
		}
		
		template <class T, bool A>
		inline vector<T, true> round(const vector<T, A> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::round(v);
		}
		
		template <class T, class Y>
		inline vector<T> lerp(const vector<T> & a, const vector<T> & b, const Y & t) {
			return a + (b - a) * static_cast<T>(t);
		}
		
		template <class T>
		inline vector<T> sin(const vector<T> & angle) {
			vector<T> s;
			sin(angle, s);
			return s;
		}
		
		template <class T>
		inline vector<T> cos(const vector<T> & angle) {
			vector<T> c;
			cos(angle, c);
			return c;
		}
		
		template <class T>
		inline void sin(const vector<T> & angle, vector<T> & s) {
			using Intrin = Intrinsic<T, 4>;
			
			auto x = normalize_angle(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), constants<vector<T>>::half_pi);
			
			s = Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(constants<vector<T>>::pi, sign), x)));
			x = Intrin::sqr(s);
			
			s *=
				vector_constants<T>::one + x * (
					coefficients<vector<T>>::sin[0] + x * (
						coefficients<vector<T>>::sin[1] + x * (
							coefficients<vector<T>>::sin[2] + x * (
								coefficients<vector<T>>::sin[3] + x * (
									coefficients<vector<T>>::sin[4]
								)))));
		}
		
		template <class T>
		inline void cos(const vector<T> & angle, vector<T> & c) {
			using Intrin = Intrinsic<T, 4>;
			
			auto x = normalize_angle(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), constants<vector<T>>::half_pi);
			
			x = Intrin::sqr(Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(constants<vector<T>>::pi, sign), x))));
			
			c = Intrin::bit_or(Intrin::bit_and(comp, vector_constants<T>::one), Intrin::bit_andnot(comp, vector_constants<T>::minusOne)) * (
				vector_constants<T>::one + x * (
					coefficients<vector<T>>::cos[0] + x * (
						coefficients<vector<T>>::cos[1] + x * (
							coefficients<vector<T>>::cos[2] + x * (
								coefficients<vector<T>>::cos[3] + x * (
									coefficients<vector<T>>::cos[4]
								))))));
		}
		
		template <class T, bool A1, bool A2, bool A3>
		inline void sincos(const vector<T, A1> & angle, vector<T, A2> & s, vector<T, A3> & c) {
			using Intrin = Intrinsic<T, 4>;
			
			auto x = normalize_angle(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), constants<vector<T>>::half_pi);
			
			s = Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(constants<vector<T>>::pi, sign), x)));
			x = Intrin::sqr(s);
			
			s *=
				vector_constants<T>::one + x * (
					coefficients<vector<T>>::sin[0] + x * (
						coefficients<vector<T>>::sin[1] + x * (
							coefficients<vector<T>>::sin[2] + x * (
								coefficients<vector<T>>::sin[3] + x * (
									coefficients<vector<T>>::sin[4]
								)))));
			
			c = Intrin::bit_or(Intrin::bit_and(comp, vector_constants<T>::one), Intrin::bit_andnot(comp, vector_constants<T>::minusOne)) * (
				vector_constants<T>::one + x * (
					coefficients<vector<T>>::cos[0] + x * (
						coefficients<vector<T>>::cos[1] + x * (
							coefficients<vector<T>>::cos[2] + x * (
								coefficients<vector<T>>::cos[3] + x * (
									coefficients<vector<T>>::cos[4]
								))))));
		}
		
		// put an angle into [-pi; pi] range
		template <class T, bool A>
		inline vector<T, true> normalize_angle(const vector<T, A> & angle) {
			return angle - round(angle * constants<vector<T>>::inv_pi2) * constants<vector<T>>::pi2;
		}
		
		// returns [ sin(angle), cos(angle), -sin(angle), 0 ]
		template <class T>
		inline vector<T> trigon(T angle) {
			return sin(vector<T>{angle, angle + constants<T>::half_pi, -angle, 0});
		}

#ifdef USE_AVX
		using DoubleVectorMath = VectorMath<double>;
		using DoubleVectorCfs = VectorCfs<double>;
		using DoubleVectorConstants = VectorConstants<double>;
#endif

//---------------------------------------------------------------------------
		
		inline vector<float> vec() {
			return vector_constants<float>::zero;
		}
		
		inline vector<float> vec(float x, float y, float z) {
			return {x, y, z, 1};
		}
		
		inline vector<float> vec(float x, float y, float z, float w) {
			return {x, y, z, w};
		}

#ifdef USE_AVX
		inline vector<double> vecd()
		{
			return vector<double>::zero;
		}
	
		inline vector<double> vecd(double x, double y, double z)
		{
			return {x, y, z, 1};
		}
	
		inline vector<double> vecd(double x, double y, double z, double w)
		{
			return {x, y, z, w};
		}
#endif
		
		template <typename T, bool A1, bool A2>
		inline vector<T, true> operator +(const vector<T, A1> & v1, const vector<T, A2> & v2) {
			return Intrinsic<T, 4>::add(v1, v2);
		}
		
		template <typename T, bool A1, bool A2>
		inline vector<T, true> operator -(const vector<T, A1> & v1, const vector<T, A2> & v2) {
			return Intrinsic<T, 4>::sub(v1, v2);
		}
		
		template <typename T, bool A1, bool A2>
		inline vector<T, true> operator *(const vector<T, A1> & v1, const vector<T, A2> & v2) {
			return Intrinsic<T, 4>::mul(v1, v2);
		}
		
		template <typename T, bool A1, bool A2>
		inline vector<T, true> operator /(const vector<T, A1> & v1, const vector<T, A2> & v2) {
			return Intrinsic<T, 4>::div(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator +(const vector<T, A> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::add(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator -(const vector<T, A> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::sub(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator *(const vector<T, A> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::mul(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator /(const vector<T, A> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::div(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator +(const intrin_t<T, 4> & v1, const vector<T, A> & v2) {
			return Intrinsic<T, 4>::add(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator -(const intrin_t<T, 4> & v1, const vector<T, A> & v2) {
			return Intrinsic<T, 4>::sub(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator *(const intrin_t<T, 4> & v1, const vector<T, A> & v2) {
			return Intrinsic<T, 4>::mul(v1, v2);
		}
		
		template <typename T, bool A>
		inline vector<T, true> operator /(const intrin_t<T, 4> & v1, const vector<T, A> & v2) {
			return Intrinsic<T, 4>::div(v1, v2);
		}
		
		template <typename T, bool A, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T, true> operator +(const vector<T, A> & vec, U a) {
			return Intrinsic<T, 4>::add(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template <typename T, bool A, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T, true> operator -(const vector<T, A> & vec, U a) {
			return Intrinsic<T, 4>::sub(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template <typename T, bool A, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T, true> operator *(const vector<T, A> & vec, U a) {
			return Intrinsic<T, 4>::mul(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template <typename T, bool A, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T, true> operator /(const vector<T, A> & vec, U a) {
			return Intrinsic<T, 4>::div(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template <typename T, bool A, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T, true> operator *(U a, const vector<T, A> & vec) {
			return Intrinsic<T, 4>::mul(Intrinsic<T, 4>::fill(static_cast<T>(a)), vec);
		}
		
		template <typename T, bool A, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T, true> operator /(U a, const vector<T, A> & vec) {
			return Intrinsic<T, 4>::div(Intrinsic<T, 4>::fill(static_cast<T>(a)), vec);
		}
		
		template <typename T, bool A>
		inline T sum(const vector<T, A> & v) {
			return v.sum();
		}
		
		template <typename T, bool A>
		inline T magnitude(const vector<T, A> & v) {
			return v.magnitude();
		}
		
		template <typename T, bool A1, bool A2>
		inline T dot(const vector<T, A1> & v1, const vector<T, A2> & v2) {
			return v1.dot1(v2);
		}
		
		template <typename T, bool A1, bool A2>
		inline auto cross(const vector<T, A1> & v1, const vector<T, A2> & v2) {
			return v1.cross(v2);
		}
		
		template <typename T>
		inline auto mixedProduct(const vector<T> & a, const vector<T> & b, const vector<T> & c) {
			return dot(a, cross(b, c));
		}
		
		template <typename T>
		inline bool areCollinear(const vector<T> & a, const vector<T> & b, const vector<T> & c) {
			return cross(b - a, c - a).magnitudeSq() < constants<T>::eps2;
		}
		
		template <typename T>
		inline bool areComplanar(const vector<T> & a, const vector<T> & b, const vector<T> & c, const vector<T> & d) {
			return abs(mixedProduct(b - a, c - a, d - a)) < constants<T>::eps3;
		}
	}
	
	inline math::vector<float> operator "" _v(long double v) {
		return {static_cast<float>(v)};
	}

#ifdef USE_AVX
	inline DoubleVector operator "" _vd(long double v)
	{
		return {static_cast<double>(v)};
	}
#endif
	
	template <class T>
	inline void print(String & s, const math::vector<T> & v) {
		s << String::assemble("(", v.x, ", ", v.y, ", ", v.z, ", ", v.w, ")");
	}
	
	template <class T>
	inline void print(String & s, const aligned<math::vector<T>> & v) {
		s << String::assemble("(", v->x, ", ", v->y, ", ", v->z, ", ", v->w, ")");
	}
	
	inline void print(String & s, const intrin_t<float, 4> & v) {
		s << String::assemble(
			"(",
			IntrinData<float, 4>::get<0>(v), ", ",
			IntrinData<float, 4>::get<1>(v), ", ",
			IntrinData<float, 4>::get<2>(v), ", ",
			IntrinData<float, 4>::get<3>(v), ")"
		);
	}

#ifdef USE_AVX
	inline void print(String & s, const intrin_t<double, 4> & v)
	{
		s << String::assemble(
			"(",
			IntrinData<double, 4>::get<0>(v), ", ",
			IntrinData<double, 4>::get<1>(v), ", ",
			IntrinData<double, 4>::get<2>(v), ", ",
			IntrinData<double, 4>::get<3>(v), ")"
			);
	}
#endif
}

//---------------------------------------------------------------------------
#endif

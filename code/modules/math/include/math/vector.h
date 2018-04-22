//---------------------------------------------------------------------------

#pragma once

#ifndef VECTOR_H
#define VECTOR_H

//---------------------------------------------------------------------------

#include <core/intrinsic/Intrinsic.h>
#include <math/math.h>
#include <core/String.h>
#include <core/memory/aligned.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template <typename T, typename Implementation, typename Storage>
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
		
		template <typename T, typename Implementation = Intrinsic<T, 4>>
		struct alignas(sizeof(T) * 4) CommonVectorStorage
		{
			using intrin_type = typename Implementation::type;
			
			member_cast(v, array<T, 4>);
			
			CommonVectorStorage(intrin_type intrin) {
				set(intrin);
			}
			
			CommonVectorStorage(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
			
			forceinline void set(const CommonVectorStorage<T> & vec) {
				v = vec.v;
			}
			
			template <class Storage, useif<!is_same<Storage, CommonVectorStorage>::value>>
			forceinline void set(const Storage & s) {
				x = s.x;
				y = s.y;
				z = s.z;
				w = s.w;
			}
			
			forceinline void set(const intrin_type & intrin) {
				Implementation::store(intrin, elements);
			}
			
			forceinline operator intrin_type() const {
				return Implementation::load(x, y, z, w);
			}
			
			union
			{
				struct
				{
					T x, y, z, w;
				};
				
				std::array<T, 4> v;
				T elements[4];
				
				Swizzle<T, 0, 1, 2> xyz;
				Swizzle<T, 0, 2, 1> xzy;
				Swizzle<T, 1, 2, 0> yzx;
				Swizzle<T, 1, 0, 2> yxz;
				Swizzle<T, 2, 0, 1> zxy;
				Swizzle<T, 2, 1, 0> zyx;
			};
		};
		
		template <typename T, typename Implementation = Intrinsic<T, 4>>
		struct alignas(sizeof(T) * 4) AlignedVectorStorage : aligned_alloc
		{
			using intrin_type = typename Implementation::type;
			
			member_cast(v, array<T, 4>);
			
			AlignedVectorStorage(intrin_type intrin) : intrinsic(intrin) {}
			
			AlignedVectorStorage(T x, T y, T z, T w) : intrinsic(Implementation::load(x, y, z, w)) {}
			
			forceinline void set(AlignedVectorStorage s) {
				intrinsic = s.intrinsic;
			}
			
			template <class Storage, useif<!is_same<Storage, AlignedVectorStorage>::value>>
			forceinline void set(const Storage & s) {
				v = s.v;
			}
			
			forceinline void set(intrin_type intrin) {
				intrinsic = intrin;
			}
			
			forceinline operator intrin_type() const {
				return intrinsic;
			}
			
			union
			{
				struct
				{
					T x, y, z, w;
				};
				
				std::array<T, 4> v;
				T elements[4];
				intrin_type intrinsic;
				
				Swizzle<T, 0, 1, 2> xyz;
				Swizzle<T, 0, 2, 1> xzy;
				Swizzle<T, 1, 2, 0> yzx;
				Swizzle<T, 1, 0, 2> yxz;
				Swizzle<T, 2, 0, 1> zxy;
				Swizzle<T, 2, 1, 0> zyx;
			};
		};
		
		template <class T, bool numeric = std::is_arithmetic<T>::value>
		struct vector_constants {};
		
#ifdef UNALIGNED_VECTORS
#define DefaultVectorStorage CommonVectorStorage
#else
#define DefaultVectorStorage AlignedVectorStorage
#endif
		
		template <typename T, typename Implementation = Intrinsic<T, 4>, typename Storage = DefaultVectorStorage<T, Implementation>>
		struct alignas(sizeof(T) * 4) vector : Storage
		{
			using intrin_type = typename Implementation::type;
			using return_type = vector<T, Implementation, AlignedVectorStorage<T, Implementation>>;
			using base_type = Storage;
			using constants = vector_constants<T>;
			
			vector() : vector(constants::identity) {}
			
			vector(const intrin_type & v) : base_type(v) {}
			
			template <class I, class S>
			vector(const vector<T, I, S> & v) : base_type(v) {}
			
			template <class I, class S>
			vector(const vector<T, I, S> & v, T a) : base_type(v) { this->w = a; }
			
			template <class I, class S>
			vector(const vector<T, I, S> & v, T a, T b) : base_type(v) {
				this->z = a;
				this->w = b;
			}
			
			template <class I, class S>
			vector(T a, const vector<T, I, S> & v) : base_type(Implementation::template shuffle<3, 0, 1, 2>(v)) { this->x = a; }
			
			template <class I, class S>
			vector(T a, T b, const vector<T, I, S> & v) : base_type(Implementation::template shuffle<2, 3, 0, 1>(v)) {
				this->x = a;
				this->y = b;
			}
			
			template <class I, class S>
			vector(T a, const vector<T, I, S> & v, T b) : base_type(Implementation::template shuffle<3, 0, 1, 2>(v)) {
				this->x = a;
				this->w = b;
			}
			
			template <class U, class I, class S, useif<!is_same<T, U>::value>>
			vector(const vector<U, I, S> & v) : base_type(intrin_cvt<intrin_type>(v)) {}
			
			template <class U, class I, class S, useif<!is_same<T, U>::value>>
			vector(const vector<U, I, S> & v, T a)      : base_type(intrin_cvt<intrin_type>(v)) { this->w = a; }
			
			template <class U, class I, class S, useif<!is_same<T, U>::value>>
			vector(const vector<U, I, S> & v, T a, T b) : base_type(intrin_cvt<intrin_type>(v)) {
				this->z = a;
				this->w = b;
			}
			
			template <class U, class I, class S, useif<!is_same<T, U>::value>>
			vector(T a, const vector<U, I, S> & v)      : base_type(intrin_cvt<intrin_type>(Implementation::template shuffle<3, 0, 1, 2>(v))) { this->x = a; }
			
			template <class U, class I, class S, useif<!is_same<T, U>::value>>
			vector(T a, T b, const vector<U, I, S> & v) : base_type(intrin_cvt<intrin_type>(Implementation::template shuffle<2, 3, 0, 1>(v))) {
				this->x = a;
				this->y = b;
			}
			
			template <class U, class I, class S, useif<!is_same<T, U>::value>>
			vector(T a, const vector<U, I, S> & v, T b) : base_type(intrin_cvt<intrin_type>(Implementation::template shuffle<3, 0, 1, 2>(v))) {
				this->x = a;
				this->w = b;
			}
			
			template <class U, useif<can_cast<U, vector>::value>>
			vector(const U & v) : vector(constants::identity) {
				cast::to(*this, v);
			}
			
			vector(const T * v) : vector(Implementation::load(v)) {}
			
			vector(T value) : base_type(value, value, value, value) {}
			
			vector(T x, T y, T z = 0, T w = 0) : base_type(x, y, z, w) {}
			
			template <class I, class S>
			vector & operator =(const vector<T, I, S> & vec) {
				Storage::set(vec);
				return *this;
			}
			
			vector & operator =(intrin_type intrin) {
				Storage::set(intrin);
				return *this;
			}
			
			template <class U, useif<can_cast<U, vector>::value>>
			vector & operator =(const U & v) {
				cast::to(*this, v);
				return *this;
			}
			
			using base_type::set;
			
			vector & set(const T * v) {
				memory<T>::move(this->v, v);
				return *this;
			}
			
			vector & set(T x, T y, T z, T w) {
				this->x = x, this->y = y, this->z = z, this->w = w;
				return *this;
			}
			
			vector & fill(T value) {
				return *this = Implementation::fill(value);
			}
			
			return_type operator +() const {
				return *this;
			}
			
			return_type operator -() const {
				return Implementation::negate(*this);
			}
			
			vector & abs() {
				return *this = Implementation::abs(*this);
			}
			
			vector & round() {
				return *this = Implementation::round(*this);
			}
			
			vector & floor() {
				return *this = Implementation::floor(*this);
			}
			
			vector & invert() {
				return *this = Implementation::invert(*this);
			}
			
			return_type inverse() const {
				return Implementation::invert(*this);
			}
			
			void rounded(vector & vec) const {
				vec = Implementation::round(*this);
			}
			
			void inversed(vector & vec) const {
				vec = Implementation::invert(*this);
			}
			
			forceinline vector & operator +=(const vector & v) {
				Storage::set(Implementation::add(*this, v));
				return *this;
			}
			
			vector & operator -=(const vector & v) {
				Storage::set(Implementation::sub(*this, v));
				return *this;
			}
			
			vector & operator *=(const vector & v) {
				Storage::set(Implementation::mul(*this, v));
				return *this;
			}
			
			vector & operator /=(const vector & v) {
				Storage::set(Implementation::div(*this, v));
				return *this;
			}
			
			vector & operator +=(T value) {
				return *this = Implementation::add(*this, Implementation::fill(value));
			}
			
			vector & operator -=(T value) {
				return *this = Implementation::sub(*this, Implementation::fill(value));
			}
			
			vector & operator *=(T k) {
				return *this = Implementation::mul(*this, Implementation::fill(k));
			}
			
			vector & operator /=(T k) {
				if (k == 0) {
					return *this = math::constants<vector<T >>::infinity;
				}
				
				return *this = Implementation::div(*this, Implementation::fill(k));
			}
			
			return_type operator &(const vector & v) const {
				return Implementation::bit_and(*this, v);
			}
			
			return_type operator |(const vector & v) const {
				return Implementation::bit_or(*this, v);
			}
			
			return_type operator ^(const vector & v) const {
				return Implementation::bit_xor(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type operator >>(const vector & v) const {
				return Implementation::bit_shr(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type operator <<(const vector & v) const {
				return Implementation::bit_shl(*this, v);
			}
			
			return_type bit_and(const vector & v) const {
				return Implementation::bit_and(*this, v);
			}
			
			return_type bit_or(const vector & v) const {
				return Implementation::bit_or(*this, v);
			}
			
			return_type bit_xor(const vector & v) const {
				return Implementation::bit_xor(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type shift(const vector & v) const { /* >> */
				return Implementation::bit_shr(*this, v);
			}
			
			template <class U = T, useif<std::is_integral<U>::value>>
			return_type unshift(const vector & v) const { /* << */
				return Implementation::bit_shl(*this, v);
			}
			
			template <int I, class U = T, useif<std::is_integral<U>::value>>
			return_type shift() const { /* >> */
				return Implementation::template bit_shr<I>(*this);
			}
			
			template <int I, class U = T, useif<std::is_integral<U>::value>>
			return_type unshift() const { /* << */
				return Implementation::template bit_shl<I>(*this);
			}
			
			return_type dot(const vector & v) const {
				return Implementation::fill_sum(*this * v);
			}
			
			T dot1(const vector & v) const {
				return Implementation::sum(*this * v);
			}
			
			return_type cross(const vector & v) const {
				return
					shuffle<1, 2, 0, 3>() * v.template shuffle<2, 0, 1, 3>() -
						shuffle<2, 0, 1, 3>() * v.template shuffle<1, 2, 0, 3>();
			}
			
			bool operator ==(const vector & v) const {
				return Implementation::equal(*this, v);
			}
			
			bool operator !=(const vector & v) const {
				return Implementation::notequal(*this, v);
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
				return Implementation::sqrt(magnitudeSqVector());
			}
			
			vector & normalize() & {
				return *this = Implementation::div(*this, magnitudeVector());
			}
			
			vector && normalize() && {
				return std::forward<vector>(*this = Implementation::div(*this, magnitudeVector()));
			}
			
			return_type normalized() const {
				return Implementation::div(*this, Implementation::fill(magnitude()));
			}
			
			inline T max() const {
				return_type v = Implementation::abs(*this);
				return std::max({v.x, v.y, v.z});
			}
			
			inline return_type abs() const {
				return Implementation::abs(*this);
			}
			
			inline return_type sqr() const {
				return Implementation::sqr(*this);
			}
			
			inline T sum() const {
				return Implementation::sum(*this);
			}
			
			inline return_type fill_sum() const {
				return Implementation::fill_sum(*this);
			}
			
			inline T square_distance_to(const vector & p) const {
				return Implementation::sum(Implementation::sqr(Implementation::sub(*this, p)));
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
				return Implementation::bit_and(VectorMaskAxis<T, mk_mask4(X, Y, Z, W)>::get(), *this);
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			return_type mask_axis() const // set all components of a array_list to zero except of one
			{
				return Implementation::bit_and(VectorMaskAxis<T, bitmask<Axis>::value>::get(), *this);
			}
			
			return_type mask_x() const {
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
				return Implementation::bit_and(VectorMaskAxis<T, 0xF ^ bitmask<Axis>::value>::get(), *this);
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
				return Implementation::bit_xor(VectorSignAxis<T, mk_mask4(X, Y, Z, W)>::get(), *this);
			}
			
			template <uint Axis, useif<(Axis < 4)>>
			return_type negate_axis() const // negate one component
			{
				return Implementation::bit_xor(VectorSignAxis<T, bitmask<Axis>::value>::get(), *this);
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
				return Implementation::min(Implementation::max(*this, low), high);
			}
			
			template <byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
			return_type shuffle() const {
				return Implementation::template shuffle<A, B, C, D>(*this);
			}
			
			template <byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
			return_type shuffle(const vector & v) const {
				return Implementation::template shuffle2<A, B, C, D>(*this, v);
			}
			
			template <byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
			return_type blend(const vector & v) const {
				return Implementation::template blend<A, B, C, D>(*this, v);
			}
			
			//---------------------------------------------------------------------------
			
			static return_type minimum(const vector & v1, const vector & v2) {
				return Implementation::min(v1, v2);
			}
			
			static return_type maximum(const vector & v1, const vector & v2) {
				return Implementation::max(v1, v2);
			}
			
			static int compare(const vector & v1, const vector & v2) {
				return static_cast<int>(Implementation::sum(Implementation::sub(v1, v2)));
			}
			
		};
		
		template <class T>
		struct vector_constants<T, true>
		{
			static api(math) const vector<T> zero;         // [  0,  0,  0,  0 ]
			static api(math) const vector<T> one;          // [  1,  1,  1,  1 ]
			static api(math) const vector<T> two;          // [  2,  2,  2,  2 ]
			static api(math) const vector<T> one_xyz;       // [  1,  1,  1,  0 ]
			static api(math) const vector<T> two_xyz;       // [  2,  2,  2,  0 ]
			static api(math) const vector<T> minus_one;     // [ -1, -1, -1, -1 ]
			static api(math) const vector<T> half;         // [ .5, .5, .5, .5 ]
			
			static api(math) const vector<T> positive_x;    // [  1,  0,  0,  0 ]
			static api(math) const vector<T> positive_y;    // [  0,  1,  0,  0 ]
			static api(math) const vector<T> positive_z;    // [  0,  0,  1,  0 ]
			static api(math) const vector<T> positive_w;    // [  0,  0,  0,  1 ]
			static api(math) const vector<T> negative_x;    // [ -1,  0,  0,  0 ]
			static api(math) const vector<T> negative_y;    // [  0, -1,  0,  0 ]
			static api(math) const vector<T> negative_z;    // [  0,  0, -1,  0 ]
			static api(math) const vector<T> negative_w;    // [  0,  0,  0, -1 ]
			
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
				intrinsic = Intrin::template blend<S::BX, S::BY, S::BZ, S::BW>(*this, v.template shuffle<S::AX, S::AY, S::AZ, S::AW>());
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
		
		template <class T, class I = Intrinsic<T, 4>>
		using aligned_vec = vector<T, I, AlignedVectorStorage<T, I>>;

#ifdef USE_AVX
		using DoubleVector = vector<double>;
		using doublev = DoubleVector;
		using dvec = vector<double>;
#endif

//---------------------------------------------------------------------------
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> abs(const vector<T, I, S> & x) {
			return I::abs(x);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> sqr(const vector<T, I, S> & x) {
			return I::sqr(x);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> sqrt(const vector<T, I, S> & x) {
			return I::sqrt(x);
		}
		
		template <class T, class I1, class S1, class I2, class S2>
		inline aligned_vec<T, I1> mod(const vector<T, I1, S1> & x, const vector<T, I2, S2> & y) {
			return I1::mod(x, y);
		}
		
		template <class T, class I1, class S1, class I2, class S2>
		inline aligned_vec<T, I1> avg(const vector<T, I1, S1> & x, const vector<T, I2, S2> & y) {
			return vector_constants<T>::half * (x + y);
		}
		
		template <class T, class I1, class S1, class I2, class S2, class I3, class S3>
		inline aligned_vec<T, I1> clamp(const vector<T, I1, S1> & x, const vector<T, I2, S2> & low, const vector<T, I3, S3> & high) {
			return x.clamp(low, high);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> invert(const vector<T, I, S> & v) {
			return I::invert(v);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> trunc(const vector<T, I, S> & v) {
			return I::trunc(v);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> floor(const vector<T, I, S> & v) {
			return I::floor(v);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> ceil(const vector<T, I, S> & v) {
			return I::ceil(v);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> round(const vector<T, I, S> & v) {
			return I::round(v);
		}
		
		template <class T, class I1, class S1, class I2, class S2, class Y>
		inline aligned_vec<T, I1> lerp(const vector<T, I1, S1> & a, const vector<T, I2, S2> & b, const Y & t) {
			return a + (b - a) * static_cast<T>(t);
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> sin(const vector<T, I, S> & angle) {
			aligned_vec<T, I> s;
			sin(angle, s);
			return s;
		}
		
		template <class T, class I, class S>
		inline aligned_vec<T, I> cos(const vector<T, I, S> & angle) {
			vector<T, I, S> c;
			cos(angle, c);
			return c;
		}
		
		template <class T, class I1, class S1, class I2, class S2>
		inline void sin(const vector<T, I1, S1> & angle, vector<T, I2, S2> & s) {
			using Intrin = I1;
			
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
		
		template <class T, class I1, class S1, class I2, class S2>
		inline void cos(const vector<T, I1, S1> & angle, vector<T, I2, S2> & c) {
			using Intrin = I1;
			
			auto x = normalize_angle(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), constants<vector<T>>::half_pi);
			
			x = Intrin::sqr(Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(constants<vector<T>>::pi, sign), x))));
			
			c = Intrin::bit_or(Intrin::bit_and(comp, vector_constants<T>::one), Intrin::bit_andnot(comp, vector_constants<T>::minus_one)) * (
				vector_constants<T>::one + x * (
					coefficients<vector<T>>::cos[0] + x * (
						coefficients<vector<T>>::cos[1] + x * (
							coefficients<vector<T>>::cos[2] + x * (
								coefficients<vector<T>>::cos[3] + x * (
									coefficients<vector<T>>::cos[4]
								))))));
		}
		
		template <class T, class I1, class S1, class I2, class S2, class I3, class S3>
		inline void sincos(const vector<T, I1, S1> & angle, vector<T, I2, S2> & s, vector<T, I3, S3> & c) {
			using Intrin = I1;
			
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
			
			c = Intrin::bit_or(Intrin::bit_and(comp, vector_constants<T>::one), Intrin::bit_andnot(comp, vector_constants<T>::minus_one)) * (
				vector_constants<T>::one + x * (
					coefficients<vector<T>>::cos[0] + x * (
						coefficients<vector<T>>::cos[1] + x * (
							coefficients<vector<T>>::cos[2] + x * (
								coefficients<vector<T>>::cos[3] + x * (
									coefficients<vector<T>>::cos[4]
								))))));
		}
		
		// put an angle into [-pi; pi] range
		template <class T, class I, class S>
		inline aligned_vec<T, I> normalize_angle(const vector<T, I, S> & angle) {
			return angle - round(angle * constants<vector<T>>::inv_pi2) * constants<vector<T>>::pi2;
		}
		
		// returns [ sin(angle), cos(angle), -sin(angle), 0 ]
		template <class T>
		inline aligned_vec<T> trigon(T angle) {
			return sin(aligned_vec<T>{angle, angle + constants<T>::half_pi, -angle, 0});
		}

#ifdef USE_AVX
		using DoubleVectorMath = VectorMath<double>;
		using DoubleVectorCfs = VectorCfs<double>;
		using DoubleVectorConstants = VectorConstants<double>;
#endif

//---------------------------------------------------------------------------
		
		inline aligned_vec<float> vec() {
			return vector_constants<float>::zero;
		}
		
		inline aligned_vec<float> vec(float x, float y, float z) {
			return {x, y, z, 1};
		}
		
		inline aligned_vec<float> vec(float x, float y, float z, float w) {
			return {x, y, z, w};
		}

#ifdef USE_AVX
		inline aligned_vec<double> vecd()
		{
			return vector_constants<double>::zero;
		}
	
		inline aligned_vec<double> vecd(double x, double y, double z)
		{
			return {x, y, z, 1};
		}
	
		inline aligned_vec<double> vecd(double x, double y, double z, double w)
		{
			return {x, y, z, w};
		}
#endif
		
		template <typename T, class I1, class S1, class I2, class S2>
		inline aligned_vec<T, I1> operator +(const vector<T, I1, S1> & v1, const vector<T, I2, S2> & v2) {
			return I1::add(v1, v2);
		}
		
		template <typename T, class I1, class S1, class I2, class S2>
		inline aligned_vec<T, I1> operator -(const vector<T, I1, S1> & v1, const vector<T, I2, S2> & v2) {
			return I1::sub(v1, v2);
		}
		
		template <typename T, class I1, class S1, class I2, class S2>
		inline aligned_vec<T, I1> operator *(const vector<T, I1, S1> & v1, const vector<T, I2, S2> & v2) {
			return I1::mul(v1, v2);
		}
		
		template <typename T, class I1, class S1, class I2, class S2>
		inline aligned_vec<T, I1> operator /(const vector<T, I1, S1> & v1, const vector<T, I2, S2> & v2) {
			return I1::div(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator +(const vector<T, I, S> & v1, const intrin_t<T, 4> & v2) {
			return I::add(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator -(const vector<T, I, S> & v1, const intrin_t<T, 4> & v2) {
			return I::sub(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator *(const vector<T, I, S> & v1, const intrin_t<T, 4> & v2) {
			return I::mul(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator /(const vector<T, I, S> & v1, const intrin_t<T, 4> & v2) {
			return I::div(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator +(const intrin_t<T, 4> & v1, const vector<T, I, S> & v2) {
			return I::add(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator -(const intrin_t<T, 4> & v1, const vector<T, I, S> & v2) {
			return I::sub(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator *(const intrin_t<T, 4> & v1, const vector<T, I, S> & v2) {
			return I::mul(v1, v2);
		}
		
		template <typename T, class I, class S>
		inline aligned_vec<T, I> operator /(const intrin_t<T, 4> & v1, const vector<T, I, S> & v2) {
			return I::div(v1, v2);
		}
		
		template <typename T, class I, class S, typename U, useif<std::is_arithmetic<U>::value>>
		inline aligned_vec<T, I> operator +(const vector<T, I, S> & vec, U a) {
			return I::add(vec, I::fill(static_cast<T>(a)));
		}
		
		template <typename T, class I, class S, typename U, useif<std::is_arithmetic<U>::value>>
		inline aligned_vec<T, I> operator -(const vector<T, I, S> & vec, U a) {
			return I::sub(vec, I::fill(static_cast<T>(a)));
		}
		
		template <typename T, class I, class S, typename U, useif<std::is_arithmetic<U>::value>>
		inline aligned_vec<T, I> operator *(const vector<T, I, S> & vec, U a) {
			return I::mul(vec, I::fill(static_cast<T>(a)));
		}
		
		template <typename T, class I, class S, typename U, useif<std::is_arithmetic<U>::value>>
		inline aligned_vec<T, I> operator /(const vector<T, I, S> & vec, U a) {
			return I::div(vec, I::fill(static_cast<T>(a)));
		}
		
		template <typename T, class I, class S, typename U, useif<std::is_arithmetic<U>::value>>
		inline aligned_vec<T, I> operator *(U a, const vector<T, I, S> & vec) {
			return I::mul(I::fill(static_cast<T>(a)), vec);
		}
		
		template <typename T, class I, class S, typename U, useif<std::is_arithmetic<U>::value>>
		inline aligned_vec<T, I> operator /(U a, const vector<T, I, S> & vec) {
			return I::div(I::fill(static_cast<T>(a)), vec);
		}
		
		template <typename T, class I, class S>
		inline T sum(const vector<T, I, S> & v) {
			return v.sum();
		}
		
		template <typename T, class I, class S>
		inline T magnitude(const vector<T, I, S> & v) {
			return v.magnitude();
		}
		
		template <typename T, class I1, class S1, class I2, class S2>
		inline T dot(const vector<T, I1, S1> & v1, const vector<T, I2, S2> & v2) {
			return v1.dot1(v2);
		}
		
		template <typename T, class I1, class S1, class I2, class S2>
		inline auto cross(const vector<T, I1, S1> & v1, const vector<T, I2, S2> & v2) {
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
	
	template <class T, class I, class S>
	inline void print(String & s, const math::vector<T, I, S> & v) {
		s << String::assemble("(", v.x, ", ", v.y, ", ", v.z, ", ", v.w, ")");
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

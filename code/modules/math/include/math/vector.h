//---------------------------------------------------------------------------

#pragma once

#ifndef VECTOR_H
#define VECTOR_H

//---------------------------------------------------------------------------

#include <core/intrinsic/Intrinsic.h>
#include <math/math.h>
#include <core/String.h>
#include <core/memory/allocator/aligned_alloc.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<typename T>
		struct vector;
		
		template<typename T, size_t Mask>
		using VectorMaskAxis = IntrinsicMask<T, IntrinMax, Mask>;
		
		template<typename T, size_t Mask>
		using VectorSignAxis = IntrinsicMask<T, IntrinSignmask, Mask>;
		
		template<int ... Sequence>
		struct SwizzleSequence {};
		
		template<typename T, class S, bool Assignable = S::assignable>
		struct SwizzleVector {};
		
		template<int X, int Y>
		struct SwizzleSequence<X, Y>
		{
			static constexpr bool assignable = (X != Y);
			
			template<int Component>
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
		
		template<int X, int Y, int Z>
		struct SwizzleSequence<X, Y, Z>
		{
			static constexpr bool assignable = ((X != Y) && (X != Z) && (Y != Z));
			
			template<int Component>
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
		
		template<int X, int Y, int Z, int W>
		struct SwizzleSequence<X, Y, Z, W>
		{
			static constexpr bool assignable = ((X != Y) && (X != Z) && (X != W) && (Y != Z) && (Y != W) && (Z != W));
			
			template<int Component>
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
		
		template<typename T, int ... Mask>
		using Swizzle = SwizzleVector<T, SwizzleSequence<Mask...>>;
		
		template<typename T>
		struct alignas(sizeof(T) * 4) vector : aligned_alloc
		{
			using IntrinType = intrin_t<T, 4>;
			using Intrin = Intrinsic<T, 4>;
			
			union
			{
				struct
				{
					T x, y, z, w;
				};
				
				IntrinType intrinsic;
				
				array<T, 4> v;
				T elements[4];
				
				Swizzle<T, 0, 1, 2> xyz;
				Swizzle<T, 0, 2, 1> xzy;
				Swizzle<T, 1, 2, 0> yzx;
				Swizzle<T, 1, 0, 2> yxz;
				Swizzle<T, 2, 0, 1> zxy;
				Swizzle<T, 2, 1, 0> zyx;
			};
			
			member_cast(v, array<T, 4>);
			member_cast(intrinsic, IntrinType);
			
			vector() : intrinsic(identity) {}
			
			vector(const IntrinType & v) : intrinsic(v) {}
			
			vector(const vector & v) : intrinsic(v) {}
			
			vector(const vector & v, T a) : intrinsic(v) { w = a; }
			
			vector(const vector & v, T a, T b) : intrinsic(v) {
				z = a;
				w = b;
			}
			
			vector(T a, const vector & v) : intrinsic(Intrin::template shuffle<3, 0, 1, 2>(v)) { x = a; }
			
			vector(T a, T b, const vector & v) : intrinsic(Intrin::template shuffle<2, 3, 0, 1>(v)) {
				x = a;
				y = b;
			}
			
			vector(T a, const vector & v, T b) : intrinsic(Intrin::template shuffle<3, 0, 1, 2>(v)) {
				x = a;
				w = b;
			}
			
			template<class U, useif<!is_same<T, U>::value>>
			vector(const vector<U> & v) : intrinsic(intrin_cvt<IntrinType>(v.intrinsic)) {}
			
			template<class U, useif<!is_same<T, U>::value>>
			vector(const vector<U> & v, T a)      : intrinsic(intrin_cvt<IntrinType>(v.intrinsic)) { w = a; }
			
			template<class U, useif<!is_same<T, U>::value>>
			vector(const vector<U> & v, T a, T b) : intrinsic(intrin_cvt<IntrinType>(v.intrinsic)) {
				z = a;
				w = b;
			}
			
			template<class U, useif<!is_same<T, U>::value>>
			vector(T a, const vector<U> & v)      : intrinsic(intrin_cvt<IntrinType>(Intrin::template shuffle<3, 0, 1, 2>(v))) { x = a; }
			
			template<class U, useif<!is_same<T, U>::value>>
			vector(T a, T b, const vector<U> & v) : intrinsic(intrin_cvt<IntrinType>(Intrin::template shuffle<2, 3, 0, 1>(v))) {
				x = a;
				y = b;
			}
			
			template<class U, useif<!is_same<T, U>::value>>
			vector(T a, const vector<U> & v, T b) : intrinsic(intrin_cvt<IntrinType>(Intrin::template shuffle<3, 0, 1, 2>(v))) {
				x = a;
				w = b;
			}
			
			template<class U, useif<can_cast<U, vector>::value>>
			vector(const U & v) {
				cast::to(*this, v);
			}
			
			vector(const T * v) {
				memory<T>::move(this->v, v);
			}
			
			vector(T value) : intrinsic(Intrin::fill(value)) {}
			
			vector(T x, T y, T z = 0, T w = 0) : intrinsic(Intrin::load(x, y, z, w)) {}
			
			vector & operator =(const vector & v) {
				intrinsic = v;
				return *this;
			}
			
			vector & operator =(const IntrinType & v) {
				intrinsic = v;
				return *this;
			}
			
			template<class U, useif<can_cast<U, vector>::value>>
			vector & operator =(const U & v) {
				cast::to(*this, v);
				return *this;
			}
			
			vector & set(const T * v) {
				memory<T>::move(this->v, v);
				return *this;
			}
			
			vector & set(T x, T y, T z, T w) {
				intrinsic = Intrin::load(x, y, z, w);
				return *this;
			}
			
			vector & fill(T value) {
				intrinsic = Intrin::fill(value);
				return *this;
			}
			
			vector operator +() const {
				return intrinsic;
			}
			
			vector operator -() const {
				return Intrin::negate(intrinsic);
			}
			
			vector & abs() {
				intrinsic = Intrin::abs(intrinsic);
				return *this;
			}
			
			vector & round() {
				intrinsic = Intrin::round(intrinsic);
				return *this;
			}
			
			vector & floor() {
				intrinsic = Intrin::floor(intrinsic);
				return *this;
			}
			
			vector & invert() {
				intrinsic = Intrin::invert(intrinsic);
				return *this;
			}
			
			vector inverse() const {
				return Intrin::invert(intrinsic);
			}
			
			void getRounded(vector & vec) const {
				vec.intrinsic = Intrin::round(intrinsic);
			}
			
			void getInverse(vector & vec) const {
				vec.intrinsic = Intrin::invert(intrinsic);
			}
			
			vector & operator +=(const vector & v) {
				intrinsic = Intrin::add(intrinsic, v);
				return *this;
			}
			
			vector & operator -=(const vector & v) {
				intrinsic = Intrin::sub(intrinsic, v);
				return *this;
			}
			
			vector & operator *=(const vector & v) {
				intrinsic = Intrin::mul(intrinsic, v);
				return *this;
			}
			
			vector & operator /=(const vector & v) {
				intrinsic = Intrin::div(intrinsic, v);
				return *this;
			}
			
			vector & operator +=(T value) {
				intrinsic = Intrin::add(intrinsic, Intrin::fill(value));
				return *this;
			}
			
			vector & operator -=(T value) {
				intrinsic = Intrin::sub(intrinsic, Intrin::fill(value));
				return *this;
			}
			
			vector & operator *=(T k) {
				intrinsic = Intrin::mul(intrinsic, Intrin::fill(k));
				return *this;
			}
			
			vector & operator /=(T k) {
				if(k == 0) {
					return *this = math::constants < vector < T >> ::infinity;
				}
				
				intrinsic = Intrin::div(intrinsic, Intrin::fill(k));
				return *this;
			}
			
			vector operator &(const vector & v) const {
				return Intrin::bit_and(intrinsic, v);
			}
			
			vector operator |(const vector & v) const {
				return Intrin::bit_or(intrinsic, v);
			}
			
			vector operator ^(const vector & v) const {
				return Intrin::bit_xor(intrinsic, v);
			}
			
			template<class U = T, useif<std::is_integral<U>::value>>
			vector operator >>(const vector & v) const {
				return Intrin::bit_shr(intrinsic, v);
			}
			
			template<class U = T, useif<std::is_integral<U>::value>>
			vector operator <<(const vector & v) const {
				return Intrin::bit_shl(intrinsic, v);
			}
			
			vector bit_and(const vector & v) const {
				return Intrin::bit_and(intrinsic, v);
			}
			
			vector bit_or(const vector & v) const {
				return Intrin::bit_or(intrinsic, v);
			}
			
			vector bit_xor(const vector & v) const {
				return Intrin::bit_xor(intrinsic, v);
			}
			
			template<class U = T, useif<std::is_integral<U>::value>>
			vector shift(const vector & v) const {
				return Intrin::bit_shr(intrinsic, v);
			}
			
			template<class U = T, useif<std::is_integral<U>::value>>
			vector unshift(const vector & v) const {
				return Intrin::bit_shl(intrinsic, v);
			}
			
			template<int I, class U = T, useif<std::is_integral<U>::value>>
			vector shift() const /* >> */
			{
				return Intrin::template bit_shr<I>(intrinsic);
			}
			
			template<int I, class U = T, useif<std::is_integral<U>::value>>
			vector unshift() const /* << */
			{
				return Intrin::template bit_shl<I>(intrinsic);
			}
			
			vector dot(const vector & v) const {
				return Intrin::fillsum(*this * v);
			}
			
			T dot1(const vector & v) const {
				return Intrin::sum(*this * v);
			}
			
			vector cross(const vector & v) const {
				return
					shuffle<1, 2, 0, 3>() * v.template shuffle<2, 0, 1, 3>() -
						shuffle<2, 0, 1, 3>() * v.template shuffle<1, 2, 0, 3>();
			}
			
			bool operator ==(const vector & v) const {
				return Intrin::equal(intrinsic, v);
			}
			
			bool operator !=(const vector & v) const {
				return Intrin::notequal(intrinsic, v);
			}
			
			T & operator [](size_t index) {
				return v[index];
			}
			
			const T & operator [](size_t index) const {
				return v[index];
			}
			
			T & operator [](int index) {
				return v[index];
			}
			
			const T & operator [](int index) const {
				return v[index];
			}
			
			T magnitudeSq() const {
				return dot1(intrinsic);
			}
			
			vector magnitudeSqVector() const {
				return dot(intrinsic);
			}
			
			T magnitude() const {
				return std::sqrt(magnitudeSq());
			}
			
			vector magnitudeVector() const {
				return Intrin::sqrt(magnitudeSqVector());
			}
			
			vector & normalize() & {
				intrinsic = Intrin::div(intrinsic, magnitudeVector());
				return *this;
			}
			
			vector && normalize() && {
				intrinsic = Intrin::div(intrinsic, magnitudeVector());
				return std::forward<vector>(*this);
			}
			
			vector normalized() const {
				return Intrin::div(intrinsic, Intrin::fill(magnitude()));
			}
			
			inline T max() const {
				vector v = Intrin::abs(intrinsic);
				return std::max({v.x, v.y, v.z});
			}
			
			inline vector abs() const {
				return Intrin::abs(intrinsic);
			}
			
			inline vector sqr() const {
				return Intrin::sqr(intrinsic);
			}
			
			inline T sum() const {
				return Intrin::sum(intrinsic);
			}
			
			inline vector fillsum() const {
				return Intrin::fillsum(intrinsic);
			}
			
			inline T distanceToSq(const vector & p) const {
				return Intrin::sum(Intrin::sqr(Intrin::sub(intrinsic, p)));
			}
			
			inline auto distanceTo(const vector & p) const {
				return std::sqrt(distanceToSq(p));
			}
			
			inline T distanceToAxis(const vector & p, int axis) const {
				return std::abs(v[axis] - p[axis]);
			}
			
			inline int getMainAxis() const {
				return x >= y ? 0 : y >= z ? 1 : 2;
			}
			
			template<byte X, byte Y, byte Z, byte W, useif<(X < 2 && Y < 2 && Z < 2 && W < 2)>>
			vector mask() const // select some components (e.g. if X == 1 then result.x = v.x else result.x = 0)
			{
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(X, Y, Z, W)>::get(), intrinsic);
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector maskAxis() const // set all components of a array_list to zero except of one
			{
				return Intrin::bit_and(VectorMaskAxis<T, bitmask<Axis>::value>::get(), intrinsic);
			}
			
			vector maskX() const {
				return maskAxis<0>();
			}
			
			vector maskY() const {
				return maskAxis<1>();
			}
			
			vector maskZ() const {
				return maskAxis<2>();
			}
			
			vector maskW() const {
				return maskAxis<3>();
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector clearAxis() const // set a single component to zero
			{
				return Intrin::bit_and(VectorMaskAxis<T, 0xF ^ bitmask<Axis>::value>::get(), intrinsic);
			}
			
			vector clearX() const {
				return clearAxis<0>();
			}
			
			vector clearY() const {
				return clearAxis<1>();
			}
			
			vector clearZ() const {
				return clearAxis<2>();
			}
			
			vector clearW() const {
				return clearAxis<3>();
			}
			
			template<byte X, byte Y, byte Z, byte W, useif<(X < 2 && Y < 2 && Z < 2 && W < 2)>>
			vector negate() const // negate some components (e.g. if X == 1 then result.x = -v.x else result.x = v.x)
			{
				return Intrin::bit_xor(VectorSignAxis<T, mk_mask4(X, Y, Z, W)>::get(), intrinsic);
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector negateAxis() const // negate one component
			{
				return Intrin::bit_xor(VectorSignAxis<T, bitmask<Axis>::value>::get(), intrinsic);
			}
			
			vector negateX() const {
				return negateAxis<0>();
			}
			
			vector negateY() const {
				return negateAxis<1>();
			}
			
			vector negateZ() const {
				return negateAxis<2>();
			}
			
			vector negateW() const {
				return negateAxis<3>();
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector spreadAxis() const // get a array_list filled with a single component of a src array_list
			{
				return shuffle<Axis, Axis, Axis, Axis>();
			}
			
			vector spreadX() const {
				return spreadAxis<0>();
			}
			
			vector spreadY() const {
				return spreadAxis<1>();
			}
			
			vector spreadZ() const {
				return spreadAxis<2>();
			}
			
			vector spreadW() const {
				return spreadAxis<3>();
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector & addAxis(const vector & vec) {
				v += vec.maskAxis<Axis>();
				return *this;
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector & subtractAxis(const vector & vec) {
				v -= vec.maskAxis<Axis>();
				return *this;
			}
			
			vector & addAxis(const vector & vec, int Axis) {
				v[Axis] += vec[Axis];
				return *this;
			}
			
			vector & subtractAxis(const vector & vec, int Axis) {
				v[Axis] -= vec[Axis];
				return *this;
			}
			
			vector & addAxis(T val, int Axis) {
				v[Axis] += val;
				return *this;
			}
			
			vector & subtractAxis(T val, int Axis) {
				v[Axis] -= val;
				return *this;
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector & addAxis(T val) {
				v[Axis] += val;
				return *this;
			}
			
			template<uint Axis, useif<(Axis < 4)>>
			vector & subtractAxis(T val) {
				v[Axis] -= val;
				return *this;
			}
			
			vector clamp(T low, T high) {
				return clamp(vector(low), vector(high));
			}
			
			vector clamp(const vector & low, const vector & high) const {
				return Intrin::min(Intrin::max(intrinsic, low), high);
			}
			
			template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
			vector shuffle() const {
				return Intrin::template shuffle<A, B, C, D>(intrinsic);
			}
			
			template<byte A, byte B, byte C, byte D, useif<(A < 4 && B < 4 && C < 4 && D < 4)>>
			vector shuffle(const vector & v) const {
				return Intrin::template shuffle2<A, B, C, D>(intrinsic, v);
			}
			
			template<byte A, byte B, byte C, byte D, useif<(A < 2 && B < 2 && C < 2 && D < 2)>>
			vector blend(const vector & v) const {
				return Intrin::template blend<A, B, C, D>(intrinsic, v);
			}
			
			//---------------------------------------------------------------------------
			
			static vector minimum(const vector & v1, const vector & v2) {
				return Intrin::min(v1, v2);
			}
			
			static vector maximum(const vector & v1, const vector & v2) {
				return Intrin::max(v1, v2);
			}
			
			static int compare(const vector & v1, const vector & v2) {
				return static_cast<int>(Intrin::sum(Intrin::sub(v1, v2)));
			}
			
			static api(math) const vector zero;            // [  0,  0,  0,  0 ]
			static api(math) const vector one;            // [  1,  1,  1,  1 ]
			static api(math) const vector two;            // [  2,  2,  2,  2 ]
			static api(math) const vector oneXYZ;        // [  1,  1,  1,  0 ]
			static api(math) const vector twoXYZ;        // [  2,  2,  2,  0 ]
			static api(math) const vector minusOne;        // [ -1, -1, -1, -1 ]
			static api(math) const vector half;            // [ .5, .5, .5, .5 ]
			
			static api(math) const vector positiveX;    // [  1,  0,  0,  0 ]
			static api(math) const vector positiveY;    // [  0,  1,  0,  0 ]
			static api(math) const vector positiveZ;    // [  0,  0,  1,  0 ]
			static api(math) const vector positiveW;    // [  0,  0,  0,  1 ]
			static api(math) const vector negativeX;    // [ -1,  0,  0,  0 ]
			static api(math) const vector negativeY;    // [  0, -1,  0,  0 ]
			static api(math) const vector negativeZ;    // [  0,  0, -1,  0 ]
			static api(math) const vector negativeW;    // [  0,  0,  0, -1 ]
			
			static api(math) const vector & right;
			static api(math) const vector & up;
			static api(math) const vector & forward;
			
			static api(math) const vector & left;
			static api(math) const vector & down;
			static api(math) const vector & back;
			
			static api(math) const vector & identity;
		};
		
		template<typename T, int ... Mask>
		struct SwizzleVector<T, SwizzleSequence<Mask...>, true>
		{
			using IntrinType = intrin_t<T, 4>;
			using Intrin = Intrinsic<T, 4>;
			using S = SwizzleSequence<Mask...>;
			
			SwizzleVector & operator =(const vector<T> & v) {
				intrinsic = Intrin::template blend<S::BX, S::BY, S::BZ, S::BW>(intrinsic, v.template shuffle<S::AX, S::AY, S::AZ, S::AW>());
				return *this;
			}
			
			template<class U, useif<not_same_type<U, T>::value>>
			operator vector<U>() const {
				return vector<T>(Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(intrinsic)));
			}
			
			operator vector<T>() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(intrinsic));
			}
			
			operator IntrinType() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(intrinsic));
			}
		
		protected:
			IntrinType intrinsic;
		};
		
		template<typename T, int ... Mask>
		struct SwizzleVector<T, SwizzleSequence<Mask...>, false>
		{
			using IntrinType = intrin_t<T, 4>;
			using Intrin = Intrinsic<T, 4>;
			using S = SwizzleSequence<Mask...>;
			
			template<class U, useif<not_same_type<U, T>::value>>
			operator vector<U>() const {
				return vector<T>(Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(intrinsic)));
			}
			
			operator vector<T>() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(intrinsic));
			}
			
			operator IntrinType() const {
				return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(S::MX, S::MX, S::MX, S::MX)>::get(), Intrin::template shuffle<S::SX, S::SY, S::SZ, S::SW>(intrinsic));
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
		
		template<class T>
		using aligned_vector = aligned<vector<T>>;
		
		using bvec = aligned_vector<byte>;
		using ivec = aligned_vector<int>;
		using fvec = aligned_vector<float>;

#ifdef USE_AVX
		using DoubleVector = vector<double>;
		using doublev = DoubleVector;
		using dvec = aligned_vector<double>;
#endif

//---------------------------------------------------------------------------
		
		template<class T>
		inline vector<T> abs(const vector<T> & x) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::abs(x);
		}
		
		template<class T>
		inline vector<T> sqr(const vector<T> & x) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::sqr(x);
		}
		
		template<class T>
		inline vector<T> sqrt(const vector<T> & x) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::sqrt(x);
		}
		
		template<class T>
		inline vector<T> mod(const vector<T> & x, const vector<T> & y) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::mod(x, y);
		}
		
		template<class T>
		inline vector<T> avg(const vector<T> & x, const vector<T> & y) {
			return vector<T>::half * (x + y);
		}
		
		template<class T>
		inline vector<T> clamp(const vector<T> & x, const vector<T> & low, const vector<T> & high) {
			return x.clamp(low, high);
		}
		
		template<class T>
		inline vector<T> invert(const vector<T> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::invert(v.intrinsic);
		}
		
		template<class T>
		inline vector<T> trunc(const vector<T> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::trunc(v.intrinsic);
		}
		
		template<class T>
		inline vector<T> floor(const vector<T> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::floor(v.intrinsic);
		}
		
		template<class T>
		inline vector<T> ceil(const vector<T> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::ceil(v.intrinsic);
		}
		
		template<class T>
		inline vector<T> round(const vector<T> & v) {
			using Intrin = Intrinsic<T, 4>;
			
			return Intrin::round(v.intrinsic);
		}
		
		template<class T, class Y>
		inline vector<T> lerp(const vector<T> & a, const vector<T> & b, const Y & t) {
			return a + (b - a) * static_cast<T>(t);
		}
		
		template<class T>
		inline vector<T> sin(const vector<T> & angle) {
			vector<T> s;
			sin(angle, s);
			return s;
		}
		
		template<class T>
		inline vector<T> cos(const vector<T> & angle) {
			vector<T> c;
			cos(angle, c);
			return c;
		}
		
		template<class T>
		inline void sin(const vector<T> & angle, vector<T> & s) {
			using Intrin = Intrinsic<T, 4>;
			
			auto x = normalize_angle(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), constants<vector<T>>::half_pi);
			
			s = Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(constants<vector<T>>::pi, sign), x)));
			x = Intrin::sqr(s);
			
			s *=
				vector<T>::one + x * (
					coefficients<vector<T>>::sin[0] + x * (
						coefficients<vector<T>>::sin[1] + x * (
							coefficients<vector<T>>::sin[2] + x * (
								coefficients<vector<T>>::sin[3] + x * (
									coefficients<vector<T>>::sin[4]
								)))));
		}
		
		template<class T>
		inline void cos(const vector<T> & angle, vector<T> & c) {
			using Intrin = Intrinsic<T, 4>;
			
			auto x = normalize_angle(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), constants<vector<T>>::half_pi);
			
			x = Intrin::sqr(Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(constants<vector<T>>::pi, sign), x))));
			
			c = Intrin::bit_or(Intrin::bit_and(comp, vector<T>::one), Intrin::bit_andnot(comp, vector<T>::minusOne)) * (
				vector<T>::one + x * (
					coefficients<vector<T>>::cos[0] + x * (
						coefficients<vector<T>>::cos[1] + x * (
							coefficients<vector<T>>::cos[2] + x * (
								coefficients<vector<T>>::cos[3] + x * (
									coefficients<vector<T>>::cos[4]
								))))));
		}
		
		template<class T>
		inline void sincos(const vector<T> & angle, vector<T> & s, vector<T> & c) {
			using Intrin = Intrinsic<T, 4>;
			
			auto x = normalize_angle(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), constants<vector<T>>::half_pi);
			
			s = Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(constants<vector<T>>::pi, sign), x)));
			x = Intrin::sqr(s);
			
			s *=
				vector<T>::one + x * (
					coefficients<vector<T>>::sin[0] + x * (
						coefficients<vector<T>>::sin[1] + x * (
							coefficients<vector<T>>::sin[2] + x * (
								coefficients<vector<T>>::sin[3] + x * (
									coefficients<vector<T>>::sin[4]
								)))));
			
			c = Intrin::bit_or(Intrin::bit_and(comp, vector<T>::one), Intrin::bit_andnot(comp, vector<T>::minusOne)) * (
				vector<T>::one + x * (
					coefficients<vector<T>>::cos[0] + x * (
						coefficients<vector<T>>::cos[1] + x * (
							coefficients<vector<T>>::cos[2] + x * (
								coefficients<vector<T>>::cos[3] + x * (
									coefficients<vector<T>>::cos[4]
								))))));
		}
		
		// returns [ sin(angle), cos(angle), -sin(angle), 0 ]
		template<class T>
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
			return vector<float>::zero;
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
		
		template<typename T>
		inline vector<T> operator +(const vector<T> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::add(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator -(const vector<T> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::sub(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator *(const vector<T> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::mul(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator /(const vector<T> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::div(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator +(const vector<T> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::add(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator -(const vector<T> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::sub(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator *(const vector<T> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::mul(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator /(const vector<T> & v1, const intrin_t<T, 4> & v2) {
			return Intrinsic<T, 4>::div(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator +(const intrin_t<T, 4> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::add(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator -(const intrin_t<T, 4> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::sub(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator *(const intrin_t<T, 4> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::mul(v1, v2);
		}
		
		template<typename T>
		inline vector<T> operator /(const intrin_t<T, 4> & v1, const vector<T> & v2) {
			return Intrinsic<T, 4>::div(v1, v2);
		}
		
		template<typename T, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T> operator +(const vector<T> & vec, U a) {
			return Intrinsic<T, 4>::add(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template<typename T, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T> operator -(const vector<T> & vec, U a) {
			return Intrinsic<T, 4>::sub(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template<typename T, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T> operator *(const vector<T> & vec, U a) {
			return Intrinsic<T, 4>::mul(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template<typename T, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T> operator /(const vector<T> & vec, U a) {
			return Intrinsic<T, 4>::div(vec, Intrinsic<T, 4>::fill(static_cast<T>(a)));
		}
		
		template<typename T, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T> operator *(U a, const vector<T> & vec) {
			return Intrinsic<T, 4>::mul(Intrinsic<T, 4>::fill(static_cast<T>(a)), vec.intrinsic);
		}
		
		template<typename T, typename U, useif<std::is_arithmetic<U>::value>>
		inline vector<T> operator /(U a, const vector<T> & vec) {
			return Intrinsic<T, 4>::div(Intrinsic<T, 4>::fill(static_cast<T>(a)), vec.intrinsic);
		}
		
		template<typename T>
		inline T sum(const vector<T> & v) {
			return v.sum();
		}
		
		template<typename T>
		inline T magnitude(const vector<T> & v) {
			return v.magnitude();
		}
		
		template<typename T>
		inline T dot(const vector<T> & v1, const vector<T> & v2) {
			return v1.dot1(v2);
		}
		
		template<typename T>
		inline auto cross(const vector<T> & v1, const vector<T> & v2) {
			return v1.cross(v2);
		}
		
		template<typename T>
		inline auto mixedProduct(const vector<T> & a, const vector<T> & b, const vector<T> & c) {
			return dot(a, cross(b, c));
		}
		
		template<typename T>
		inline bool areCollinear(const vector<T> & a, const vector<T> & b, const vector<T> & c) {
			return cross(b - a, c - a).magnitudeSq() < constants<T>::eps2;
		}
		
		template<typename T>
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
	
	template<class T>
	inline void print(String & s, const math::vector<T> & v) {
		s << String::assemble("(", v.x, ", ", v.y, ", ", v.z, ", ", v.w, ")");
	}
	
	template<class T>
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

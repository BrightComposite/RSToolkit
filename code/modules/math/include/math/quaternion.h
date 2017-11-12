//---------------------------------------------------------------------------

#pragma once

#ifndef QUATERNION_H
#define QUATERNION_H

//---------------------------------------------------------------------------

#include <math/math.h>
#include <core/intrinsic/Intrinsic.h>

#include "matrix.h"

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<typename T>
		struct alignas(sizeof(T) * 4) quaternion : aligned_alloc
		{
			using Data = intrin_data<T, 4>;
			using IntrinType = typename Data::type;
			using Intrin = Intrinsic<T, 4>;
			using VectorType = vector<T>;
			
			union
			{
				struct
				{
					T x, y, z, w;
				};
				
				Data data;
				
				array<T, 4> q;
				T elements[4];
				VectorType v;
			};
			
			member_cast(q, array<T, 4>);
			member_cast(data, Data);
			
			quaternion() : v(vector_constants<T>::positiveW) {}
			
			quaternion(const quaternion & q) : v(q.v) {}
			
			template<class U, useif<not_same_type<T, U>::value>>
			quaternion(const quaternion<U> & q) : v(q.v) {}
			
			quaternion(T x, T y, T z, T w) : v{x, y, z, w} {}
			
			quaternion(T pitch, T yaw, T roll) {
				fromEuler({pitch, yaw, roll});
			}
			
			quaternion(const VectorType & from, const VectorType & to) {
				fromVectors(from, to);
			}
			
			quaternion(const VectorType & v) : v{v} {}
			
			quaternion(const Data & data) : v{data} {}
			
			template<class U, useif<can_cast<U, quaternion>::value>>
			quaternion(const U & v) {
				cast::to(*this, v);
			}
			
			quaternion(const VectorType & axis, T angle) : quaternion(trigon(angle * 0.5f).template shuffle<0, 0, 0, 1>() * axis.template blend<0, 0, 0, 1>(VectorType::positiveW)) {}
			//	[sx sy sz c]																							 [ s  s  s  c ]						 [ x  y  z  1 ]
			
			quaternion & operator =(const quaternion & q) {
				v = q.v;
				return *this;
			}
			
			template<class U, useif<can_cast<U, quaternion>::value>>
			quaternion & operator =(const U & q) {
				cast::to(*this, q);
				return *this;
			}
			
			operator IntrinType() const {
				return Intrin::load(data);
			}
			
			IntrinType intrin() const {
				return Intrin::load(data);
			}
			
			bool operator ==(const quaternion & q) const {
				return v == q.v;
			}
			
			bool operator !=(const quaternion & q) const {
				return v != q.v;
			}
			
			quaternion operator +() const {
				return *this;
			}
			
			quaternion operator -() const {
				return Intrin::negate(data);
			}
			
			quaternion & set(const T * a) {
				v.set(a);
				return *this;
			}
			
			quaternion & set(T x, T y, T z, T w) {
				v.set(x, y, z, w);
				return *this;
			}
			
			quaternion & negate() {
				Intrin::negate(v);
				return *this;
			}
			
			quaternion negation() const {
				return Intrin::negate(data);
			}
			
			quaternion & conjugate() {
				v = v.template negate<1, 1, 1, 0>();
				return *this;
			}
			
			quaternion conjugation() const {
				return v.template negate<1, 1, 1, 0>();
			}
			
			quaternion & invert() {
				return conjugate().downscale(magnitude());
			}
			
			quaternion inverse() const {
				return conjugation().downscale(magnitude());
			}
			
			quaternion & operator +=(const quaternion & q) {
				v += q.v;
				return *this;
			}
			
			quaternion & operator -=(const quaternion & q) {
				v -= q.v;
				return *this;
			}
			
			quaternion & rotateBy(const quaternion & q) {
				return *this = q * *this;
			}
			
			VectorType applyTo(const VectorType & d) const {
				const auto xyz = v.clear_w();
				const auto t = 2 * cross(xyz, d);
				return d + w * t + cross(xyz, t);
				
				//return std::move((*this * quaternion(d) * inverse()).v);
			}
			
			VectorType right() const {
				return applyTo(VectorType::right);
			}
			
			VectorType up() const {
				return applyTo(VectorType::up);
			}
			
			VectorType forward() const {
				return applyTo(VectorType::forward);
			}
			
			quaternion & rotate(const VectorType & axis, T angle) {
				return rotateBy({axis, angle});
			}
			
			quaternion & rotateX(T angle) {
				return rotate(VectorType::positiveX, angle);
			}
			
			quaternion & rotateY(T angle) {
				return rotate(VectorType::positiveY, angle);
			}
			
			quaternion & rotateZ(T angle) {
				return rotate(VectorType::positiveZ, angle);
			}
			
			quaternion & scale(T s) {
				v *= s;
				return *this;
			}
			
			quaternion & downscale(T s) & {
				v /= s;
				return *this;
			}
			
			quaternion && downscale(T s) && {
				v /= s;
				return std::forward<quaternion>(*this);
			}
			
			T & operator [](size_t index) {
				return q[index];
			}
			
			const T & operator [](size_t index) const {
				return q[index];
			}
			
			T norm() const {
				return v.sqr().sum();
			}
			
			T magnitude() const {
				return sqrt(norm());
			}
			
			quaternion & normalize() & {
				v /= sqrt(v.sqr().fill_sum());
				return *this;
			}
			
			quaternion && normalize() && {
				v /= sqrt(v.sqr().fill_sum());
				return std::forward<quaternion>(*this);
			}
			
			// pitch (x), yaw (y), roll (z)
			quaternion & fromEuler(const VectorType & angles) {
				auto halfs = angles * vector_constants<T>::half;
				
				VectorType sine, cosine;
				sincos<T, Intrinsic<T, 4>>(halfs, sine, cosine);
				
				auto p = sine.template shuffle<0, 0, 0, 0>(cosine);
				auto y = sine.template shuffle<1, 1, 1, 1>(cosine);
				auto r = sine.template shuffle<2, 2, 2, 2>(cosine);
				
				v =
					p.template shuffle<0, 2, 2, 2>() * y.template shuffle<2, 0, 2, 2>() * r.template shuffle<2, 2, 0, 2>() +
						p.template shuffle<2, 0, 0, 0>().
							template negate<0, 1, 0, 1>() * y.template shuffle<0, 2, 0, 0>() * r.template shuffle<0, 0, 2, 0>();
				
				return *this;
			}
			
			// 'from' and 'to' are unit vectors
			quaternion & fromVectors(const VectorType & from, const VectorType & to) {
				auto d = from.dot(to);
				
				if(d.x >= 1.0f) { // 0 degrees, no rotation
					return *this = identity;
				}
				
				if(d.x < (-1.0f + constants<T>::eps)) // 180 degrees, find any acceptable orthogonal vector to represent a rotation
				{
					auto axis = cross(VectorType::positiveX, from);
					
					if(axis.magnitudeSq() < constants<T>::eps2) {
						axis = cross(VectorType::positiveY, from);
					}
					
					return *this = {axis.normalize(), constants<T>::pi};
				}
				
				v = cross(from, to).template blend<0, 0, 0, 1>(VectorType::one + d); // orthogonal vector [x y z] and rotation angle [w]
				return normalize();
			}
			
			matrix <T> to_matrix() const {
				VectorType s = -v.sqr();
				s = s.template shuffle<1, 2, 0, 3>() + s.template shuffle<2, 0, 1, 3>();
				VectorType a = v.template shuffle<1, 2, 0, 3>() * v.template shuffle<2, 0, 1, 3>();
				VectorType b = v * v.spread_w().template negate<1, 0, 0, 0>();
				
				return {
					vector_constants<T>::positiveX + vector_constants<T>::twoXYZ * s.template blend<0, 1, 1, 0>((a + b.template negate<0, 0, 1, 0>()).template shuffle<2, 2, 1, 1>()),    // 1.0f - 2.0f * (y * y + z * z),	2.0f * (x * y - w * z),			2.0f * (x * z + w * y),			0.0f
					vector_constants<T>::positiveY + vector_constants<T>::twoXYZ * s.template blend<1, 0, 1, 0>((a + b).template shuffle<2, 2, 0, 0>()),                                    // 2.0f * (x * y + w * z),			1.0f - 2.0f * (x * x + z * z),	2.0f * (y * z - w * x),			0.0f
					vector_constants<T>::positiveZ + vector_constants<T>::twoXYZ * (a - b).template shuffle<1, 0, 2, 3>(s),                                                                // 2.0f * (x * z - w * y),			2.0f * (y * z + w * x),			1.0f - 2.0f * (x * x + y * y),	0.0f
					vector_constants<T>::positiveW
				};
			}
			
			void to_matrix(matrix <T> & m) const {
				VectorType s = -v.sqr();
				s = s.template shuffle<1, 2, 0, 3>() + s.template shuffle<2, 0, 1, 3>();
				VectorType a = v.template shuffle<1, 2, 0, 3>() * v.template shuffle<2, 0, 1, 3>();
				VectorType b = v * v.spread_w().template negate<1, 0, 0, 0>();
				
				m[0] = vector_constants<T>::positiveX + vector_constants<T>::twoXYZ * s.template blend<0, 1, 1, 0>((a + b.template negate<0, 0, 1, 0>()).template shuffle<2, 2, 1, 1>()); // 1.0f - 2.0f * (y * y + z * z),	2.0f * (x * y - w * z),			2.0f * (x * z + w * y),			0.0f
				m[1] = vector_constants<T>::positiveY + vector_constants<T>::twoXYZ * s.template blend<1, 0, 1, 0>((a + b).template shuffle<2, 2, 0, 0>());                                // 2.0f * (x * y + w * z),			1.0f - 2.0f * (x * x + z * z),	2.0f * (y * z - w * x),			0.0f
				m[2] = vector_constants<T>::positiveZ + vector_constants<T>::twoXYZ * (a - b).template shuffle<1, 0, 2, 3>(s);                                                            // 2.0f * (x * z - w * y),			2.0f * (y * z + w * x),			1.0f - 2.0f * (x * x + y * y),	0.0f
				m[3] = vector_constants<T>::positiveW;
			}
			
			static api(math) const quaternion identity;
		};
		
		using FloatQuaternion = quaternion<float>;
		using DoubleQuaternion = quaternion<double>;
		using floatq = FloatQuaternion;
		using doubleq = DoubleQuaternion;
		
		using fquat = quaternion<float>;
		using dquat = quaternion<double>;
		
		template<class T>
		quaternion<T> operator +(const quaternion<T> & q1, const quaternion<T> & q2) {
			return q1.v + q2.v;
		}
		
		template<class T>
		quaternion<T> operator -(const quaternion<T> & q1, const quaternion<T> & q2) {
			return q1.v - q2.v;
		}
		
		template<class T>
		quaternion<T> operator *(const quaternion<T> & q2, const quaternion<T> & q1) {
			//q2.w*q1.x + q2.x*q1.w + q2.y*q1.z - q2.z*q1.y
			//q2.w*q1.y + q2.y*q1.w + q2.z*q1.x - q2.x*q1.z
			//q2.w*q1.z + q2.z*q1.w + q2.x*q1.y - q2.y*q1.x
			//q2.w*q1.w - q2.x*q1.x - q2.y*q1.y - q2.z*q1.z
			
			vector<T> v1 = q1.v;
			vector<T> v2 = q2.v;
			
			return {
				v2.spread_w() * v1 +
					v2.template shuffle<0, 1, 2, 0>() * v1.template shuffle<3, 3, 3, 0>().negate_w() +
					v2.template shuffle<1, 2, 0, 1>() * v1.template shuffle<2, 0, 1, 1>().negate_w() -
					v2.template shuffle<2, 0, 1, 2>() * v1.template shuffle<1, 2, 0, 2>()
			};
		}
	}
}

//---------------------------------------------------------------------------
#endif

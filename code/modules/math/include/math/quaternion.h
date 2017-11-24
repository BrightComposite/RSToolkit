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
				IntrinType intrinsic;
			};
			
			member_cast(q, array<T, 4>);
			member_cast(data, Data);
			
			quaternion() : v(VectorType::positiveW) {}
			
			quaternion(const quaternion & q) : v(q.v) {}
			
			template<class U, useif<not_same_type<T, U>::value>>
			quaternion(const quaternion<U> & q) : v(q.v) {}
			
			quaternion(T x, T y, T z, T w) : v{x, y, z, w} {}
			
			quaternion(T pitch, T yaw, T roll) {
				from_euler({pitch, yaw, roll});
			}
			
			quaternion(const VectorType & from, const VectorType & to) {
				from_vectors(from, to);
			}
			
			quaternion(const VectorType & v) : v{v} {}
			
			quaternion(const Data & data) : v{data} {}
			
			template<class U, useif<can_cast<U, quaternion>::value>>
			quaternion(const U & v) {
				cast::to(*this, v);
			}
			
			quaternion(const VectorType & axis, T angle) : quaternion(trigon(angle * 0.5f).template shuffle<0, 0, 0, 1>() * axis.template blend<0, 0, 0, 1>(VectorType::positiveW)) {}
			//	            [sx sy sz c]															      [ s  s  s  c ]					  [ x  y  z  1 ]
			
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
				Intrin::negate(intrinsic);
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
			
			quaternion & rotate_by(const quaternion & q) {
				return *this = q * *this;
			}
			
			VectorType apply_to(const VectorType & d) const {
				const auto xyz = v.clearW();
				const auto t = 2 * cross(xyz, d);
				return d + w * t + cross(xyz, t);
			}
			
			VectorType right() const {
				return apply_to(VectorType::right);
			}
			
			VectorType up() const {
				return apply_to(VectorType::up);
			}
			
			VectorType forward() const {
				return apply_to(VectorType::forward);
			}
			
			quaternion & rotate(const VectorType & axis, T angle) {
				return rotate_by({axis, angle});
			}
			
			quaternion & rotate_x(T angle) {
				return rotate(VectorType::positiveX, angle);
			}
			
			quaternion & rotate_y(T angle) {
				return rotate(VectorType::positiveY, angle);
			}
			
			quaternion & rotate_z(T angle) {
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
				v /= sqrt(v.sqr().fillsum());
				return *this;
			}
			
			quaternion && normalize() && {
				v /= sqrt(v.sqr().fillsum());
				return std::forward<quaternion>(*this);
			}
			
			// pitch (x), yaw (y), roll (z), radians
			static quaternion from_euler(const VectorType & angles) {
				auto halfs = angles * VectorType::half;
				
				VectorType sine, cosine;
				sincos(halfs, sine, cosine);
				
				auto p = sine.template shuffle<0, 0, 0, 0>(cosine);
				auto y = sine.template shuffle<1, 1, 1, 1>(cosine);
				auto r = sine.template shuffle<2, 2, 2, 2>(cosine);
				
				return
					p.template shuffle<0, 2, 2, 2>() * y.template shuffle<2, 0, 2, 2>() * r.template shuffle<2, 2, 0, 2>() +
						p.template shuffle<2, 0, 0, 0>().
							template negate<0, 1, 0, 1>() * y.template shuffle<0, 2, 0, 0>() * r.template shuffle<0, 0, 2, 0>();
			}
			
			// 'from' and 'to' are unit vectors
			static quaternion from_vectors(const VectorType & from, const VectorType & to) {
				auto d = from.dot(to);
				
				if(d.x >= 1.0f) { // 0 degrees, no rotation
					return identity;
				}
				
				if(d.x < (-1.0f + constants<T>::eps)) { // 180 degrees, find any acceptable orthogonal vector to represent a rotation
					auto axis = cross(VectorType::positiveX, from);
					
					if(axis.magnitudeSq() < constants<T>::eps2) {
						axis = cross(VectorType::positiveY, from);
					}
					
					return {axis.normalize(), constants<T>::pi};
				}
				
				auto v = cross(from, to).template blend<0, 0, 0, 1>(VectorType::one + d); // orthogonal vector [x y z] and rotation angle [w]

				return quaternion(v).normalize();
			}
			
			matrix <T> to_matrix() const {
				VectorType s = -v.sqr();
				s = s.template shuffle<1, 2, 0, 3>() + s.template shuffle<2, 0, 1, 3>();
				VectorType a = v.template shuffle<1, 2, 0, 3>() * v.template shuffle<2, 0, 1, 3>();
				VectorType b = v * v.spreadW().template negate<1, 0, 0, 0>();
				
				return {
					VectorType::positiveX + VectorType::twoXYZ * s.template blend<0, 1, 1, 0>((a + b.template negate<0, 0, 1, 0>()).template shuffle<2, 2, 1, 1>()),    // 1.0f - 2.0f * (y * y + z * z),	2.0f * (x * y - w * z),			2.0f * (x * z + w * y),			0.0f
					VectorType::positiveY + VectorType::twoXYZ * s.template blend<1, 0, 1, 0>((a + b).template shuffle<2, 2, 0, 0>()),                                    // 2.0f * (x * y + w * z),			1.0f - 2.0f * (x * x + z * z),	2.0f * (y * z - w * x),			0.0f
					VectorType::positiveZ + VectorType::twoXYZ * (a - b).template shuffle<1, 0, 2, 3>(s),                                                                // 2.0f * (x * z - w * y),			2.0f * (y * z + w * x),			1.0f - 2.0f * (x * x + y * y),	0.0f
					VectorType::positiveW
				};
			}
			
			void to_matrix(matrix <T> & m) const {
				VectorType s = -v.sqr();
				s = s.template shuffle<1, 2, 0, 3>() + s.template shuffle<2, 0, 1, 3>();
				VectorType a = v.template shuffle<1, 2, 0, 3>() * v.template shuffle<2, 0, 1, 3>();
				VectorType b = v * v.spreadW().template negate<1, 0, 0, 0>();
				
				m[0] = VectorType::positiveX + VectorType::twoXYZ * s.template blend<0, 1, 1, 0>((a + b.template negate<0, 0, 1, 0>()).template shuffle<2, 2, 1, 1>()); // 1.0f - 2.0f * (y * y + z * z),	2.0f * (x * y - w * z),			2.0f * (x * z + w * y),			0.0f
				m[1] = VectorType::positiveY + VectorType::twoXYZ * s.template blend<1, 0, 1, 0>((a + b).template shuffle<2, 2, 0, 0>());                                // 2.0f * (x * y + w * z),			1.0f - 2.0f * (x * x + z * z),	2.0f * (y * z - w * x),			0.0f
				m[2] = VectorType::positiveZ + VectorType::twoXYZ * (a - b).template shuffle<1, 0, 2, 3>(s);                                                            // 2.0f * (x * z - w * y),			2.0f * (y * z + w * x),			1.0f - 2.0f * (x * x + y * y),	0.0f
				m[3] = VectorType::positiveW;
			}
			
			static api(math) const quaternion identity;
		};
		
		using FloatQuaternion = quaternion<float>;
		using DoubleQuaternion = quaternion<double>;
		using floatq = FloatQuaternion;
		using doubleq = DoubleQuaternion;
		
		template<class T>
		using AlignedQuaternion = aligned<quaternion<T>>;
		
		using fquat = AlignedQuaternion<float>;
		using dquat = AlignedQuaternion<double>;
		
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
				v2.spreadW() * v1 +
					v2.template shuffle<0, 1, 2, 0>() * v1.template shuffle<3, 3, 3, 0>().negateW() +
					v2.template shuffle<1, 2, 0, 1>() * v1.template shuffle<2, 0, 1, 1>().negateW() -
					v2.template shuffle<2, 0, 1, 2>() * v1.template shuffle<1, 2, 0, 2>()
			};
		}

		template<class T>
		vector<T> operator *(const vector<T> & v, const quaternion<T> & q) {
			return q.apply_to(v);
		}
	}
}

//---------------------------------------------------------------------------
#endif

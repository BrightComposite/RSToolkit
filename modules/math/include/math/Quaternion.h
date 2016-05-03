//---------------------------------------------------------------------------

#ifndef QUATERNION_H
#define QUATERNION_H

//---------------------------------------------------------------------------

#include <math/Math.h>
#include <core/intrinsic/Intrinsic.h>

#include "Matrix.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T>
	struct alignas(sizeof(T) * 4) Quaternion
	{
		typedef intrin_data<T, 4> Data;
		typedef Intrinsic<T, 4> Intrin;

		union
		{
			Data data;

			struct
			{
				T x, y, z, w;
			};

			T q[4];
			Vector<T> v;
		};

		Quaternion() : data {Vector<T>::positiveZ} {}
		Quaternion(const Quaternion & q) : data {q.data} {}
		Quaternion(T x, T y, T z, T w) : q {x, y, z, w} {}

		Quaternion(T x, T y, T z)
		{
			fromEuler({x, y, z});
		}

		Quaternion(const Data & data) : data {data} {}

		template<class U, useif <not_same_type<T, U>::value> endif>
		Quaternion(const Quaternion<U> & q) : v(q.v) {}

		Quaternion(const Vector<T> & axis, T angle) : Quaternion(VectorMath<T>::trigon(angle * 0.5f).shuffle<0, 0, 0, 1>() * axis.blend<0, 0, 0, 1>(Vector<T>::positiveW)) {}
		//																								   [ s  s  s  c ]             [ x  y  z  1 ]

		bool operator == (const Quaternion & q) const
		{
			return v == q.v;
		}

		bool operator != (const Quaternion & q) const
		{
			return v != q.v;
		}

		Quaternion operator + () const
		{
			return *this;
		}

		Quaternion operator - () const
		{
			return Intrin::negate(data);
		}

		Quaternion & set(const T * v)
		{
			Intrin::load(v, data);
			return *this;
		}

		Quaternion & set(T x, T y, T z, T w)
		{
			Intrin::load(x, y, z, w, data);
			return *this;
		}

		Quaternion & negate()
		{
			Intrin::negate(data, data);
			return *this;
		}

		Quaternion negation()
		{
			return Intrin::negate(data);
		}

		Quaternion & operator += (const Quaternion & q)
		{
			v += q.v;
			return *this;
		}

		Quaternion & operator -= (const Quaternion & q)
		{
			v -= q.v;
			return *this;
		}

		Quaternion & operator *= (const Quaternion & q)
		{
			return *this = *this * q;
		}

		Vector<T> rotate(const Vector<T> & v)
		{
			return *this * Quaternion(v) * negation();
		}

		T & operator [] (size_t index)
		{
			return q[index];
		}

		const T & operator [] (size_t index) const
		{
			return q[index];
		}

		operator array_t<T, 4> & ()
		{
			return q;
		}

		operator const array_t<T, 4> & () const
		{
			return q;
		}

		operator Data & ()
		{
			return data;
		}

		operator const Data & () const
		{
			return data;
		}

		Quaternion & fromEuler(const Vector<T> & angles)
		{
			auto halfs = angles * Vector<T>::half;

			Vector<T> sine, cosine;
			VectorMath<T>::sincos(halfs, sine, cosine);

			auto r = sine.shuffle<0, 0, 0, 0>(cosine);
			auto p = sine.shuffle<1, 1, 1, 1>(cosine);
			auto y = sine.shuffle<2, 2, 2, 2>(cosine);

			v = r.shuffle<0, 2, 2, 2>() * p.shuffle<2, 0, 2, 2>() * y.shuffle<2, 2, 0, 2>() + r.shuffle<2, 0, 0, 0>().negate<0, 1, 0, 1>() * p.shuffle<0, 2, 0, 0>() * y.shuffle<0, 0, 2, 0>();
			return *this;
		}

		Matrix<T> toMatrix() const
		{
			Vector<T> vs = v.sqr();
			Vector<T> v0 = v.shuffle<1, 2, 0, 3>() * v.shuffle<2, 0, 1, 3>();
			vs = -(vs.shuffle<1, 2, 0, 3>() + vs.shuffle<2, 0, 1, 3>());
			Vector<T> v1 = v * v.spreadW().negate<1, 0, 0, 0>();

			return {
				Vector<T>::positiveX + Vector<T>::twoXYZ * vs.blend<0, 1, 1, 0>((v0 + v1.negate<0, 0, 1, 0>()).shuffle<2, 2, 1, 1>()), // 1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y - w * z), 2.0f * (x * z + w * y), 0.0f
				Vector<T>::positiveY + Vector<T>::twoXYZ * vs.blend<1, 0, 1, 0>((v0 + v1).shuffle<2, 2, 0, 0>()), // 2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z - w * x), 0.0f
				Vector<T>::positiveZ + Vector<T>::twoXYZ * (v0 - v1).shuffle<1, 0, 2, 3>(vs) // 2.0f * (x * z - w * y), 2.0f * (y * z + w * x), 1.0f - 2.0f * (x * x + y * y), 0.0f
			};
		}

		void toMatrix(Matrix<T> & m) const
		{
			Vector<T> vs = v.sqr();
			Vector<T> v0 = v.shuffle<1, 2, 0, 3>() * v.shuffle<2, 0, 1, 3>();
			vs = -(vs.shuffle<1, 2, 0, 3>() + vs.shuffle<2, 0, 1, 3>());
			Vector<T> v1 = v * v.spreadW().negate<1, 0, 0, 0>();

			m[0] = Vector<T>::positiveX + Vector<T>::twoXYZ * vs.blend<0, 1, 1, 0>((v0 + v1.negate<0, 0, 1, 0>()).shuffle<2, 2, 1, 1>()); // 1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y - w * z), 2.0f * (x * z + w * y), 0.0f
			m[1] = Vector<T>::positiveY + Vector<T>::twoXYZ * vs.blend<1, 0, 1, 0>((v0 + v1).shuffle<2, 2, 0, 0>()); // 2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z - w * x), 0.0f
			m[2] = Vector<T>::positiveZ + Vector<T>::twoXYZ * (v0 - v1).shuffle<1, 0, 2, 3>(vs); // 2.0f * (x * z - w * y), 2.0f * (y * z + w * x), 1.0f - 2.0f * (x * x + y * y), 0.0f
			m[3] = Vector<T>::positiveW;
		}
	};

	using FloatQuaternion = Quaternion<float>;
	using DoubleQuaternion = Quaternion<double>;

	template<class T>
	Quaternion<T> operator + (const Quaternion<T> & q1, const Quaternion<T> & q2)
	{
		return q1.v + q2.v;
	}

	template<class T>
	Quaternion<T> operator - (const Quaternion<T> & q1, const Quaternion<T> & q2)
	{
		return q1.v - q2.v;
	}

	template<class T>
	Quaternion<T> operator * (const Quaternion<T> & q1, const Quaternion<T> & q2)
	{
		//q1.y * q2.z - q1.z * q2.y + q1.w * q2.x + q1.x * q2.w,
		//q1.z * q2.x - q1.x * q2.z + q1.w * q2.y + q1.y * q2.w,
		//q1.x * q2.y - q1.y * q2.x + q1.w * q2.z + q1.z * q2.w,
		//q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z

		const Vector<T> & v1 = q1.v;
		const Vector<T> & v2 = q2.v;
		const Vector<T> v3 = v2.negate<0, 1, 1, 0>();

		return {
			v1.shuffle<1, 2, 0, 3>() * v2.shuffle<2, 0, 1, 3>() - 
			v1.shuffle<2, 0, 1, 0>() * v2.shuffle<1, 2, 0, 0>() + 
			v1.shuffle<3, 3, 3, 1>() * v3.shuffle<0, 1, 2, 1>() + 
			v1.shuffle<0, 1, 2, 2>() * v3.shuffle<3, 3, 3, 2>()
		};
	}

}

//---------------------------------------------------------------------------
#endif

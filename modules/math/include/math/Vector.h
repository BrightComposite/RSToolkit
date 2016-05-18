//---------------------------------------------------------------------------

#ifndef VECTOR_H
#define VECTOR_H

//---------------------------------------------------------------------------

#include <math/Math.h>
#include <core/intrinsic/Intrinsic.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T>
	struct Vector;

	template<typename T, size_t Mask>
	using VectorMaskAxis = IntrinsicMask<T, IntrinMax, Mask>;

	template<typename T, size_t Mask>
	using VectorSignAxis = IntrinsicMask<T, IntrinSignmask, Mask>;

	template<class T>
	struct VectorMath : Math<Vector<T>> {};

	template<class T>
	using VectorCfs = Cfs<Vector<T>>;

	template<class T>
	using VectorConstants = MathConstants<Vector<T>>;

	template<typename T>
	struct alignas(sizeof(T) * 4) Vector
	{
		using Data = intrin_data<T, 4>;
		using IntrinType = typename Data::type;
		using Intrin = Intrinsic<T, 4>;

		union
		{
			Data data;
			IntrinType intrinsic;

			struct
			{
				T x, y, z, w;
			};

			array<T, 4> v;
			T elements[4];
		};

		Vector() {}
		Vector(const Vector & v) : data(v.data) {}
		Vector(const Data & data) : data(data) {}
		Vector(Vector && v) : data(move(v.data)) {}
		Vector(Data && data) : data(forward<Data>(data)) {}

		template<class U, useif <!is_same<T, U>::value> endif>
		Vector(const Vector<U> & v) : data(intrin_cvt<IntrinType>(v.intrinsic)) {}

		template<class U, useif <can_cast<U, Vector>::value> endif>
		Vector(const U & v)
		{
			Cast<U, Vector>::cast(*this, v);
		}

		Vector(const T * v)
		{
			Intrin::load(v, data);
		}

		Vector(T value)
		{
			Intrin::fill(value, data);
		}

		Vector(T x, T y, T z, T w = 0)
		{
			Intrin::load(x, y, z, w, data);
		}

		Vector & operator = (const Vector & v)
		{
			data = v.data;
			return *this;
		}

		template<class U, useif <can_cast<U, Vector>::value> endif>
		Vector & operator = (const U & v)
		{
			Cast<U, Vector>::cast(*this, v);
			return *this;
		}

		Vector & set(const T * v)
		{
			Intrin::load(v, data);
			return *this;
		}

		Vector & set(T x, T y, T z, T w)
		{
			Intrin::load(x, y, z, w, data);
			return *this;
		}

		Vector & fill(T value)
		{
			Intrin::fill(value, data);
			return *this;
		}

		Vector operator + () const
		{
			return data;
		}

		Vector operator - () const
		{
			return Intrin::negate(data);
		}

		auto & abs()
		{
			Intrin::abs(data, data);
			return *this;
		}

		auto & negate()
		{
			Intrin::negate(data, data);
			return *this;
		}

		auto & round()
		{
			Intrin::round(data, data);
			return *this;
		}

		auto & invert()
		{
			Intrin::invert(data, data);
			return *this;
		}

		Vector rounded() const
		{
			return Intrin::round(data);
		}

		Vector inverse() const
		{
			return Intrin::invert(data);
		}

		void getRounded(Vector & vec) const
		{
			Intrin::round(data, vec.data);
		}

		void getInverse(Vector & vec) const
		{
			Intrin::invert(data, vec.data);
		}

		auto & __vectorcall operator += (const Data & v)
		{
			Intrin::add(data, v, data);
			return *this;
		}

		auto & __vectorcall operator -= (const Data & v)
		{
			Intrin::sub(data, v, data);
			return *this;
		}

		auto & __vectorcall operator *= (const Data & v)
		{
			Intrin::mul(data, v, data);
			return *this;
		}

		auto & __vectorcall operator /= (const Data & v)
		{
			Intrin::div(data, v, data);
			return *this;
		}

		Vector & operator += (T value)
		{
			Intrin::add(data, Intrin::fill(value), data);
			return *this;
		}

		Vector & operator -= (T value)
		{
			Intrin::sub(data, Intrin::fill(value), data);
			return *this;
		}

		Vector & operator *= (T k)
		{
			Intrin::mul(data, Intrin::fill(k), data);
			return *this;
		}

		Vector & operator /= (T k)
		{
			if(k == 0)
				return *this = VectorMath<T>::infinity;

			Intrin::div(data, Intrin::fill(k), data);
			return *this;
		}

		Vector operator & (const Data & v) const
		{
			return Intrin::bit_and(data, v);
		}

		Vector operator | (const Data & v) const
		{
			return Intrin::bit_or(data, v);
		}

		Vector operator ^ (const Data & v) const
		{
			return Intrin::bit_xor(data, v);
		}

		Vector dot(const Data & v) const
		{
			return Intrin::fillsum(Intrin::mul(data, v));
		}

		T dot1(const Data & v) const
		{
			return Intrin::sum(Intrin::mul(data, v));
		}

		Vector cross(const Data & v) const
		{
			return Intrin::sub(
				Intrin::mul(Intrin::shuffle<1, 2, 0, 3>(data), Intrin::shuffle<2, 0, 1, 3>(v)),
				Intrin::mul(Intrin::shuffle<2, 0, 1, 3>(data), Intrin::shuffle<1, 2, 0, 3>(v))
			);
		}

		bool operator == (const Data & v) const
		{
			return Intrin::equal(data, v);
		}

		bool operator != (const Data & v) const
		{
			return Intrin::notequal(data, v);
		}

		T & operator [] (size_t index)
		{
			return v[index];
		}

		const T & operator [] (size_t index) const
		{
			return v[index];
		}

		T & operator [] (int index)
		{
			return v[index];
		}

		const T & operator [] (int index) const
		{
			return v[index];
		}

		T length() const
		{
			return std::sqrt(lengthSq());
		}

		T norm() const
		{
			return abs().sum();
		}

		T lengthSq() const
		{
			return dot1(data);
		}

		Vector & normalize()
		{
			Intrin::div(data, Intrin::fill(length()));
			return *this;
		}

		inline T maxLength() const
		{
			Data v = Intrin::abs(data);
			return std::max({v.x, v.y, v.z});
		}

		inline Vector abs() const
		{
			return Intrin::abs(data);
		}

		inline Vector sqr() const
		{
			return Intrin::sqr(data);
		}

		inline T sum() const
		{
			return Intrin::sum(data);
		}

		inline T distanceToSq(const Data & p) const
		{
			return Intrin::sum(Intrin::sqr(Intrin::sub(data, p)));
		}

		inline auto distanceTo(const Data & p) const
		{
			return std::sqrt(distanceToSq(p));
		}

		inline T distanceToAxis(const Data & p, int Axis) const
		{
			return std::abs(v[Axis] - p[Axis]);
		}

		inline int getMainAxis() const
		{
			return x >= y ? 0 : y >= z ? 1 : 2;
		}

		template<byte X, byte Y, byte Z, byte W, useif <(X < 2 && Y < 2 && Z < 2 && W < 2)> endif>
		Vector mask() const // select some components (e.g. if X == 1 then result.x = v.x else result.x = 0)
		{
			return Intrin::bit_and(VectorMaskAxis<T, mk_mask4(X, Y, Z, W)>::get(), data);
		}

		template<uint Axis, useif <(Axis < 4)> endif>
		Vector maskAxis() const // set all components of a vector to zero except of one
		{
			return Intrin::bit_and(VectorMaskAxis<T, bitmask<Axis>::value>::get(), data);
		}

		Vector maskX() const
		{
			return maskAxis<0>();
		}

		Vector maskY() const
		{
			return maskAxis<1>();
		}

		Vector maskZ() const
		{
			return maskAxis<2>();
		}

		Vector maskW() const
		{
			return maskAxis<3>();
		}

		template<uint Axis, useif <(Axis < 4)> endif>
		Vector clearAxis() const // set a single component to zero
		{
			return Intrin::bit_and(VectorMaskAxis<T, 0xF ^ bitmask<Axis>::value>::get(), data);
		}

		Vector clearX() const
		{
			return clearAxis<0>();
		}

		Vector clearY() const
		{
			return clearAxis<1>();
		}

		Vector clearZ() const
		{
			return clearAxis<2>();
		}

		Vector clearW() const
		{
			return clearAxis<3>();
		}

		template<byte X, byte Y, byte Z, byte W, useif <(X < 2 && Y < 2 && Z < 2 && W < 2)> endif>
		Vector negate() const // negate some components (e.g. if X == 1 then result.x = -v.x else result.x = v.x)
		{
			return Intrin::bit_xor(VectorSignAxis<T, mk_mask4(X, Y, Z, W)>::get(), data);
		}

		template<uint Axis, useif <(Axis < 4)> endif>
		Vector negateAxis() const // negate one component
		{
			return Intrin::bit_xor(VectorSignAxis<T, bitmask<Axis>::value>::get(), data);
		}

		Vector negateX() const
		{
			return negateAxis<0>();
		}

		Vector negateY() const
		{
			return negateAxis<1>();
		}

		Vector negateZ() const
		{
			return negateAxis<2>();
		}

		Vector negateW() const
		{
			return negateAxis<3>();
		}

		template<uint Axis, useif <(Axis < 4)> endif>
		Vector spreadAxis() const // get a vector filled with a single component of a src vector
		{
			return shuffle<Axis, Axis, Axis, Axis>();
		}

		Vector spreadX() const
		{
			return spreadAxis<0>();
		}

		Vector spreadY() const
		{
			return spreadAxis<1>();
		}

		Vector spreadZ() const
		{
			return spreadAxis<2>();
		}

		Vector spreadW() const
		{
			return spreadAxis<3>();
		}

		template<uint Axis, useif <(Axis < 4)> endif>
		Vector & addAxis(const Vector & vec)
		{
			v += vec.maskAxis<Axis>();
			return *this;
		}

		template<uint Axis, useif <(Axis < 4)> endif>
		Vector & subtractAxis(const Vector & vec)
		{
			v -= vec.maskAxis<Axis>();
			return *this;
		}

		Vector & addAxis(const Vector & vec, int Axis)
		{
			v[Axis] += vec[Axis];
			return *this;
		}

		Vector & subtractAxis(const Vector & vec, int Axis)
		{
			v[Axis] -= vec[Axis];
			return *this;
		}

		Vector & addAxis(T val, int Axis)
		{
			v[Axis] += val;
			return *this;
		}

		Vector & subtractAxis(T val, int Axis)
		{
			v[Axis] -= val;
			return *this;
		}

		Vector & clamp(T low, T high)
		{
			Math<T>::clamp(x, low, high);
			Math<T>::clamp(y, low, high);
			Math<T>::clamp(z, low, high);

			return *this;
		}

		operator array<T, 4> & () &
		{
			return v;
		}

		operator const array<T, 4> & () const &
		{
			return v;
		}

		operator array<T, 4> && () &&
		{
			return move(v);
		}

		operator Data & () &
		{
			return data;
		}

		operator const Data & () const &
		{
			return data;
		}

		operator Data && () &&
		{
			return move(data);
		}

		operator IntrinType & () &
		{
			return intrinsic;
		}

		operator const IntrinType & () const &
		{
			return intrinsic;
		}

		operator IntrinType && () &&
		{
			return move(intrinsic);
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)> endif>
		inline Vector shuffle() const
		{
			return Intrin::shuffle<A, B, C, D>(data);
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 4 && B < 4 && C < 4 && D < 4)> endif>
		inline Vector shuffle(const Data & v) const
		{
			return Intrin::shuffle2<A, B, C, D>(data, v);
		}

		template<byte A, byte B, byte C, byte D, useif <(A < 2 && B < 2 && C < 2 && D < 2)> endif>
		inline Vector blend(const Data & v) const
		{
			return Intrin::blend<A, B, C, D>(data, v);
		}

		static inline Vector minimum(const Data & v1, const Data & v2)
		{
			return Intrin::min(v1, v2);
		}

		static inline Vector maximum(const Data & v1, const Data & v2)
		{
			return Intrin::max(v1, v2);
		}

		static inline int compare(const Data & v1, const Data & v2)
		{
			return static_cast<int>(Intrin::sum(Intrin::sub(v1, v2)));
		}
		
		static const Vector zero;		// [  0,  0,  0,  0 ]
		static const Vector one;		// [  1,  1,  1,  1 ]
		static const Vector two;		// [  2,  2,  2,  2 ]
		static const Vector oneXYZ;		// [  1,  1,  1,  0 ]
		static const Vector twoXYZ;		// [  2,  2,  2,  0 ]
		static const Vector minusOne;	// [ -1, -1, -1, -1 ]
		static const Vector half;		// [ .5, .5, .5, .5 ]

		static const Vector positiveX;	// [  1,  0,  0,  0 ]
		static const Vector positiveY;	// [  0,  1,  0,  0 ]
		static const Vector positiveZ;	// [  0,  0,  1,  0 ]
		static const Vector positiveW;	// [  0,  0,  0,  1 ]
		static const Vector negativeX;	// [ -1,  0,  0,  0 ]
		static const Vector negativeY;	// [  0, -1,  0,  0 ]
		static const Vector negativeZ;	// [  0,  0, -1,  0 ]
		static const Vector negativeW;	// [  0,  0,  0, -1 ]
	};

	using ByteVector = Vector<byte>;
	using IntVector = Vector<int>;
	using FloatVector = Vector<float>;
	using DoubleVector = Vector<double>;

	using bvec = Vector<byte>;
	using ivec = Vector<int>;
	using fvec = Vector<float>;
	using dvec = Vector<double>;

	template<class T>
	struct BasicMath<Vector<T>, false>
	{
		typedef Intrinsic<T, 4> Intrin;

		static inline Vector<T> abs(const Vector<T> & x)
		{
			return Intrin::abs(x);
		}

		static inline Vector<T> sqr(const Vector<T> & x)
		{
			return Intrin::sqr(x);
		}

		static inline Vector<T> avg(const Vector<T> & x, const Vector<T> & y)
		{
			return Vector<T>::half * (x + y);
		}

		static inline void clamp(Vector<T> & x, const Vector<T> & low, const Vector<T> & high)
		{
			x.clamp(low, high);
		}

		static inline Vector<T> invert(const Vector<T> & v)
		{
			return v.inverse();
		}

		static inline Vector<T> round(const Vector<T> & v)
		{
			return v.rounded();
		}
	};

	template<class T>
	struct SinCos<Vector<T>>
	{
		using Intrin = Intrinsic<T, 4>;

		static inline Vector<T> sin(const Vector<T> & angle)
		{
			Vector<T> s;
			sin(angle, s);
			return s;
		}

		static inline Vector<T> cos(const Vector<T> & angle)
		{
			Vector<T> c;
			cos(angle, c);
			return c;
		}

		static inline void sin(const Vector<T> & angle, Vector<T> & s)
		{
			auto x = VectorMath<T>::rmod(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), VectorMath<T>::half_pi);

			s = Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(VectorMath<T>::pi, sign), x)));
			x = Intrin::sqr(s);

			s *= 
				Vector<T>::one + x * (
					VectorCfs<T>::sin[0] + x * (
						VectorCfs<T>::sin[1] + x * (
							VectorCfs<T>::sin[2] + x * (
								VectorCfs<T>::sin[3] + x * (
									VectorCfs<T>::sin[4])))));
		}

		static inline void cos(const Vector<T> & angle, Vector<T> & c)
		{
			auto x = VectorMath<T>::rmod(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), VectorMath<T>::half_pi);

			x = Intrin::sqr(Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(VectorMath<T>::pi, sign), x))));

			c = Intrin::bit_or(Intrin::bit_and(comp, Vector<T>::one), Intrin::bit_andnot(comp, Vector<T>::minusOne)) * (
				Vector<T>::one + x * (
					VectorCfs<T>::cos[0] + x * (
						VectorCfs<T>::cos[1] + x * (
							VectorCfs<T>::cos[2] + x * (
								VectorCfs<T>::cos[3] + x * (
									VectorCfs<T>::cos[4]))))));
		}

		static inline void sincos(const Vector<T> & angle, Vector<T> & s, Vector<T> & c)
		{
			auto x = VectorMath<T>::rmod(angle);
			auto sign = Intrin::sign(x);
			auto comp = Intrin::cmple(Intrin::bit_andnot(sign, x), VectorMath<T>::half_pi);

			s = Intrin::bit_or(Intrin::bit_and(comp, x), Intrin::bit_andnot(comp, Intrin::sub(Intrin::bit_or(VectorMath<T>::pi, sign), x)));
			x = Intrin::sqr(s);

			s *=
				Vector<T>::one + x * (
					VectorCfs<T>::sin[0] + x * (
						VectorCfs<T>::sin[1] + x * (
							VectorCfs<T>::sin[2] + x * (
								VectorCfs<T>::sin[3] + x * (
									VectorCfs<T>::sin[4])))));

			c = Intrin::bit_or(Intrin::bit_and(comp, Vector<T>::one), Intrin::bit_andnot(comp, Vector<T>::minusOne)) * (
				Vector<T>::one + x * (
					VectorCfs<T>::cos[0] + x * (
						VectorCfs<T>::cos[1] + x * (
							VectorCfs<T>::cos[2] + x * (
								VectorCfs<T>::cos[3] + x * (
									VectorCfs<T>::cos[4]))))));
		}

		// returns [ sin(angle), cos(angle), -sin(angle), 0 ]
		static inline Vector<T> trigon(T angle)
		{
			return sin(Vector<T>{angle, angle + Math<T>::half_pi, -angle, 0});
		}
	};

	using FloatVectorMath = VectorMath<float>;
	using DoubleVectorMath = VectorMath<double>;

	using FloatVectorCfs = VectorCfs<float>;
	using DoubleVectorCfs = VectorCfs<double>;

	using FloatVectorConstants = VectorConstants<float>;
	using DoubleVectorConstants = VectorConstants<double>;

	inline Vector<float> vec()
	{
		return Vector<float>::zero;
	}

	inline Vector<float> vec(float x, float y, float z)
	{
		return {x, y, z, 1};
	}

	inline Vector<float> vec(float x, float y, float z, float w)
	{
		return {x, y, z, w};
	}

	inline Vector<double> vecd()
	{
		return Vector<double>::zero;
	}

	inline Vector<double> vecd(double x, double y, double z)
	{
		return {x, y, z, 1};
	}

	inline Vector<double> vecd(double x, double y, double z, double w)
	{
		return {x, y, z, w};
	}

	template<typename T>
	inline Vector<T> operator + (const Vector<T> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::add(v1.data, v2.data);
	}

	template<typename T>
	inline Vector<T> operator - (const Vector<T> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::sub(v1.data, v2.data);
	}

	template<typename T>
	inline Vector<T> operator * (const Vector<T> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::mul(v1.data, v2.data);
	}

	template<typename T>
	inline Vector<T> operator / (const Vector<T> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::div(v1.data, v2.data);
	}

	template<typename T>
	inline Vector<T> operator + (const Vector<T> & v1, const intrin_t<T, 4> & v2)
	{
		return Intrinsic<T, 4>::add(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator - (const Vector<T> & v1, const intrin_t<T, 4> & v2)
	{
		return Intrinsic<T, 4>::sub(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator * (const Vector<T> & v1, const intrin_t<T, 4> & v2)
	{
		return Intrinsic<T, 4>::mul(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator / (const Vector<T> & v1, const intrin_t<T, 4> & v2)
	{
		return Intrinsic<T, 4>::div(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator + (const Vector<T> & v1, const IntrinData<T, 4> & v2)
	{
		return Intrinsic<T, 4>::add(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator - (const Vector<T> & v1, const IntrinData<T, 4> & v2)
	{
		return Intrinsic<T, 4>::sub(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator * (const Vector<T> & v1, const IntrinData<T, 4> & v2)
	{
		return Intrinsic<T, 4>::mul(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator / (const Vector<T> & v1, const IntrinData<T, 4> & v2)
	{
		return Intrinsic<T, 4>::div(v1.data, v2);
	}

	template<typename T>
	inline Vector<T> operator + (const intrin_t<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::add(v1, v2.data);
	}

	template<typename T>
	inline Vector<T> operator - (const intrin_t<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::sub(v1, v2.data);
	}

	template<typename T>
	inline Vector<T> operator * (const intrin_t<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::mul(v1, v2.data);
	}

	template<typename T>
	inline Vector<T> operator / (const intrin_t<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::div(v1, v2.data);
	}

	template<typename T>
	inline Vector<T> operator + (const IntrinData<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::add(v1, v2.data);
	}

	template<typename T>
	inline Vector<T> operator - (const IntrinData<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::sub(v1, v2.data);
	}

	template<typename T>
	inline Vector<T> operator * (const IntrinData<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::mul(v1, v2.data);
	}

	template<typename T>
	inline Vector<T> operator / (const IntrinData<T, 4> & v1, const Vector<T> & v2)
	{
		return Intrinsic<T, 4>::div(v1, v2.data);
	}

	template<typename T, typename U, useif <std::is_pod<U> endif> endif>
	inline Vector<T> operator + (const Vector<T> & vec, U a)
	{
		return Intrinsic<T, 4>::add(vec.data, Intrinsic<T, 4>::fill(static_cast<T>(a)));
	}

	template<typename T, typename U, useif <std::is_pod<U> endif> endif>
	inline Vector<T> operator - (const Vector<T> & vec, U a)
	{
		return Intrinsic<T, 4>::sub(vec.data, Intrinsic<T, 4>::fill(static_cast<T>(a)));
	}

	template<typename T, typename U, useif <std::is_pod<U> endif> endif>
	inline Vector<T> operator * (const Vector<T> & vec, U a)
	{
		return Intrinsic<T, 4>::mul(vec.data, Intrinsic<T, 4>::fill(static_cast<T>(a)));
	}

	template<typename T, typename U, useif <std::is_pod<U>::value> endif>
	inline Vector<T> operator / (const Vector<T> & vec, U a)
	{
		return Intrinsic<T, 4>::div(vec.data, Intrinsic<T, 4>::fill(static_cast<T>(a)));
	}

	template<typename T, typename U, useif <std::is_pod<U>::value> endif>
	inline Vector<T> operator * (const U & a, const Vector<T> & vec)
	{
		return Intrinsic<T, 4>::mul(Intrinsic<T, 4>::fill(a), vec.data);
	}

	template<typename T, typename U, useif <std::is_pod<U>::value> endif>
	inline Vector<T> operator / (const U & a, const Vector<T> & vec)
	{
		return Intrinsic<T, 4>::div(Intrinsic<T, 4>::fill(a), vec.data);
	}

	template<typename T>
    inline auto lerp(const Vector<T> & a, const Vector<T> & b, float t)
    {
        return a + t * (b - a);
    }

	template<typename T>
	inline T sum(const Vector<T> & v)
	{
		return v.sum();
	}

	template<typename T>
	inline T dot(const Vector<T> & v1, const Vector<T> & v2)
	{
		return v1.dot1(v2);
	}

	template<typename T>
	inline auto mixedProduct(const Vector<T> & a, const Vector<T> & b, const Vector<T> & c)
	{
		return dot(a, b.cross(c));
	}

	template<typename T>
	inline bool areCollinear(const Vector<T> & a, const Vector<T> & b, const Vector<T> & c)
	{
		return (b - a).cross(c - a).lengthSq() < Math<T>::eps2;
	}

	template<typename T>
	inline bool areComplanar(const Vector<T> & a, const Vector<T> & b, const Vector<T> & c, const Vector<T> & d)
	{
		return Math<T>::abs(mixedProduct(b - a, c - a, d - a)) < Math<T>::eps3;
	}

	inline FloatVector operator "" _v(long double v)
	{
		return {static_cast<float>(v)};
	}

	inline DoubleVector operator "" _vd(long double v)
	{
		return {static_cast<double>(v)};
	}

	template<class T>
	inline void print(String & s, const Vector<T> & v)
	{
		s << String::assemble("(", v.x, ", ", v.y, ", ", v.z, ", ", v.w, ")");
	}

	inline void print(String & s, const intrin_t<float, 4> & v)
	{
		s << String::assemble(
			"(",
			IntrinData<float, 4>::get<0>(v), ", ",
			IntrinData<float, 4>::get<1>(v), ", ",
			IntrinData<float, 4>::get<2>(v), ", ",
			IntrinData<float, 4>::get<3>(v), ")"
			);
	}

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
}

//---------------------------------------------------------------------------
#endif

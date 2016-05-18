//---------------------------------------------------------------------------

#ifndef TRANSFORM_H
#define TRANSFORM_H

//---------------------------------------------------------------------------

#include <meta/Inheritance.h>

#include "Vector.h"
#include "Quaternion.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template <typename T>
	subclass(Translation, Vector<T>);

	template <typename T>
	subclass(Scaling, Vector<T>);

	using FloatTranslation = Translation<float>;
	using DoubleTranslation = Translation<double>;

	using FloatScaling = Scaling<float>;
	using DoubleScaling = Scaling<double>;

	template<class T>
	class Transform
	{
	public:
		Transform() {}
		Transform(const Transform & t) : matrix(t.matrix) {}
		Transform(Transform && t) : matrix(move(t.matrix)) {}
		Transform(const Matrix<T> & m) : matrix(m) {}
		Transform(Matrix<T> && m) : matrix(forward<Matrix<T>>(m)) {}

		Transform(const Vector<T> & p)
		{
			matrix.w = p.template blend<0, 0, 0, 1>(matrix.w);
		}

		Transform(const Translation<T> & p) : Transform(static_cast<const Vector<T> &>(p)) {}

		Transform(const Scaling<T> & s)
		{
			matrix.x = s.maskX();
			matrix.y = s.maskY();
			matrix.z = s.maskZ();
		}

		Transform(const Quaternion<T> & r)
		{
			matrix = r.toMatrix().transpose();
		}

		Transform(const Vector<T> & p, const Quaternion<T> & r)
		{
			matrix = r.toMatrix().transpose();
			matrix.w = p.template blend<0, 0, 0, 1>(matrix.w);
		}

		Transform(const Translation<T> & p, const Quaternion<T> & r) : Transform(static_cast<const Vector<T> &>(p), r) {}

		Transform(const Vector<T> & p, const Vector<T> & s)
		{
			matrix.x = s.maskX();
			matrix.y = s.maskY();
			matrix.z = s.maskZ();
			matrix.w = p.template blend<0, 0, 0, 1>(matrix.w);
		}

		Transform(const Translation<T> & p, const Scaling<T> & s) : Transform(static_cast<const Vector<T> &>(p), s) {}

		Transform(const Vector<T> & p, const Quaternion<T> & r, const Vector<T> & s)
		{
			matrix = r.toMatrix().transpose();
			matrix.x *= s;
			matrix.y *= s;
			matrix.z *= s;
			matrix.w = p.template blend<0, 0, 0, 1>(matrix.w);
		}

		Transform(const Translation<T> & p, const Quaternion<T> & r, const Scaling<T> & s) : Transform(static_cast<const Vector<T> &>(p), r, static_cast<const Vector<T> &>(s)) {}

		Transform(const Point<T> & p)
		{
			matrix(3, 0) = p.x;
			matrix(3, 1) = p.y;
		}

		Transform(const Size<T> & s)
		{
			matrix(0, 0) = s.x;
			matrix(1, 1) = s.y;
		}

		Transform(const Point<T> & p, const Size<T> & s)
		{
			matrix(0, 0) = s.x;
			matrix(1, 1) = s.y;
			matrix(3, 0) = p.x;
			matrix(3, 1) = p.y;
		}

		Transform(const Point<T> & p, const Size<T> & s, T depth)
		{
			matrix(0, 0) = s.x;
			matrix(1, 1) = s.y;
			matrix(3, 0) = p.x;
			matrix(3, 1) = p.y;
			matrix(3, 2) = depth;
		}

		Transform & operator = (const Transform & t)
		{
			matrix = t.matrix;
			return *this;
		}

		Transform & operator = (Transform && t)
		{
			matrix = move(t.matrix);
			return *this;
		}

		Transform & operator = (const Matrix<T> & m)
		{
			matrix = m;
			return *this;
		}

		Transform & operator = (Matrix<T> && m)
		{
			matrix = forward<Matrix<T>>(m);
			return *this;
		}

		void apply(const Matrix<T> & mat)
		{
			matrix = mat.transposition() * matrix;
		}

		void apply(const Transform & t)
		{
			matrix = t.matrix * matrix;
		}

		void applyTo(Matrix<T> & mat) const
		{
			mat.transpose();
			mat *= matrix;
		}

		void applyTo(Transform & t) const
		{
			t.matrix *= matrix;
		}

		Transform & operator *= (const Transform & t)
		{
			matrix *= t.matrix;
			return *this;
		}

		Transform & operator *= (const Scaling<T> & s)
		{
			matrix *= Matrix<T>::scaleMatrix(s);
			return *this;
		}

		operator Matrix<T> & () &
		{
			return matrix;
		}

		operator const Matrix<T> & () const &
		{
			return matrix;
		}

		operator Matrix<T> && () &&
		{
			return move(matrix);
		}

		Matrix<T> matrix;
	};

	template<class T>
	Transform<T> operator * (const Transform<T> & t1, const Transform<T> & t2)
	{
		return t1.matrix * t2.matrix;
	}

	template<class T>
	Transform<T> operator * (const Transform<T> & t, const Scaling<T> & s)
	{
		return t.matrix * Matrix<T>::scaleMatrix(s);
	}

	using FloatTransform = Transform<float>;
	using DoubleTransform = Transform<double>;
}

//---------------------------------------------------------------------------
#endif

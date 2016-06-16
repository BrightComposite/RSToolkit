//---------------------------------------------------------------------------

#ifndef TRANSFORM_H
#define TRANSFORM_H

//---------------------------------------------------------------------------

#include <meta/Inheritance.h>

#include "Vector.h"
#include "Quaternion.h"
#include "Rect.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T>
	subclass(Translation, Vector<T>);

	template<typename T>
	subclass(Scaling, Vector<T>);

	using FloatTranslation = Translation<float>;
	using DoubleTranslation = Translation<double>;

	using FloatScaling = Scaling<float>;
	using DoubleScaling = Scaling<double>;

	template<class T>
	struct alignas(sizeof(T) * 4) Transform
	{
	public:
		Transform() {}
		Transform(const Transform & t) : _matrix(t._matrix) {}
		Transform(Transform && t) : _matrix(move(t._matrix)) {}
		Transform(const Matrix<T> & m) : _matrix(m) {}
		Transform(Matrix<T> && m) : _matrix(forward<Matrix<T>>(m)) {}

		Transform(const Vector<T> & p) : _matrix(Matrix<T>::translation(p)) {}
		Transform(const Scaling<T> & s) : _matrix(Matrix<T>::scaling(s)) {}
		Transform(const Quaternion<T> & r) { r.toMatrix(_matrix); }

		Transform(const Vector<T> & p, const Quaternion<T> & r) : Transform(r)
		{
			_matrix
				.translate(p);
		}

		Transform(const Vector<T> & p, const Vector<T> & s)
		{
			_matrix
				.translate(p)
				.scale(s);
		}

		Transform(const Vector<T> & p, const Quaternion<T> & r, const Vector<T> & s)
		{
			_matrix
				.scale(s)
				.apply(r.toMatrix())
				.translate(p);
		}

		Transform(const Translation<T> & p) : Transform(static_cast<const Vector<T> &>(p)) {}
		Transform(const Translation<T> & p, const Quaternion<T> & r) : Transform(static_cast<const Vector<T> &>(p), r) {}
		Transform(const Translation<T> & p, const Scaling<T> & s) : Transform(static_cast<const Vector<T> &>(p), s) {}
		Transform(const Translation<T> & p, const Quaternion<T> & r, const Scaling<T> & s) : Transform(static_cast<const Vector<T> &>(p), r, static_cast<const Vector<T> &>(s)) {}

		Transform(const Point<T> & p)
		{
			_matrix(0, 3) = p.x;
			_matrix(1, 3) = p.y;
		}

		Transform(const Size<T> & s)
		{
			_matrix.scale({s.x, s.y, 1.0f});
		}

		Transform(const Point<T> & p, const Size<T> & s)
		{
			_matrix(3, 0) = p.x;
			_matrix(3, 1) = p.y;
			_matrix.scale({s.x, s.y, 1.0f});
		}

		Transform(const Point<T> & p, const Size<T> & s, T depth)
		{
			_matrix(3, 0) = p.x;
			_matrix(3, 1) = p.y;
			_matrix(3, 2) = depth;
			_matrix.scale({s.x, s.y, 1.0f});
		}

		Transform & operator = (const Transform & t)
		{
			_matrix = t._matrix;
			return *this;
		}

		Transform & operator = (Transform && t)
		{
			_matrix = move(t._matrix);
			return *this;
		}

		Transform & operator = (const Matrix<T> & m)
		{
			_matrix = m;
			return *this;
		}

		Transform & operator = (Matrix<T> && m)
		{
			_matrix = forward<Matrix<T>>(m);
			return *this;
		}

		Matrix<T> output() const
		{
			return _matrix.transposition();
		}

		void apply(const Transform & t)
		{
			_matrix *= t._matrix;
		}

		void applyTo(Transform & t) const
		{
			t._matrix *= _matrix;
		}

		Transform & operator *= (const Transform & t)
		{
			_matrix *= t.matrix;
			return *this;
		}

		Transform & translate(const Vector<T> & s)
		{
			_matrix.translate(s);
			return *this;
		}

		Transform & rotate(const Quaternion<T> & q)
		{
			_matrix *= q.toMatrix();
			return *this;
		}

		Transform & scale(const Vector<T> & s)
		{
			_matrix.scale(s);
			return *this;
		}

	protected:
		Matrix<T> _matrix;
	};

	template<class T>
	Transform<T> operator * (const Transform<T> & t1, const Transform<T> & t2)
	{
		return t1._matrix * t2._matrix;
	}

	using FloatTransform = Transform<float>;
	using DoubleTransform = Transform<double>;
}

//---------------------------------------------------------------------------
#endif

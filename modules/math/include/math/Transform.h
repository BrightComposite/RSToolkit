//---------------------------------------------------------------------------

#pragma once

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
	template<class T>
	struct alignas(sizeof(T) * 4) Transform
	{
	public:
		Transform() {}
		Transform(const Transform & t) : _matrix(t._matrix) {}
		Transform(Transform && t) : _matrix(move(t._matrix)) {}
		Transform(const AlignedMatrix<T> & m) : _matrix(m) {}
		Transform(AlignedMatrix<T> && m) : _matrix(forward<AlignedMatrix<T>>(m)) {}

		Transform(const Vector<T> & p) : _matrix(Matrix<T>::translation(p)) {}
		Transform(const Quaternion<T> & r) { r.toMatrix(_matrix); }

		Transform(const Vector<T> & p, const Quaternion<T> & r) : Transform(r)
		{
			(*_matrix)
				.translate(p);
		}

		Transform(const Vector<T> & p, const Vector<T> & s)
		{
			(*_matrix)
				.scale(s)
				.translate(p);
		}

		Transform(const Vector<T> & p, const Quaternion<T> & r, const Vector<T> & s)
		{
			(*_matrix)
				.scale(s)
				.apply(r.toMatrix())
				.translate(p);
		}

		Transform(const Point<T> & p)
		{
			(*_matrix)(0, 3) = p.x;
			(*_matrix)(1, 3) = p.y;
		}

		Transform(const Size<T> & s)
		{
			(*_matrix).scale({s.x, s.y, 1.0f});
		}

		Transform(const Point<T> & p, const Size<T> & s)
		{
			(*_matrix)
				.scale({s.x, s.y, 1.0f})
				.translate({p.x, p.y, 0.0f});
		}

		Transform(const Point<T> & p, const Size<T> & s, T depth)
		{
			(*_matrix)
				.scale({s.x, s.y, 1.0f})
				.translate({p.x, p.y, depth});
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

		Transform & operator = (const AlignedMatrix<T> & m)
		{
			_matrix = m;
			return *this;
		}

		Transform & operator = (AlignedMatrix<T> && m)
		{
			_matrix = forward<AlignedMatrix<T>>(m);
			return *this;
		}

		Matrix<T> output() const
		{
			return _matrix->transposition();
		}

		void apply(const Transform & t)
		{
			*_matrix *= t._matrix;
		}

		void applyTo(Transform & t) const
		{
			*t._matrix *= _matrix;
		}

		Transform & operator *= (const Transform & t) &
		{
			*_matrix *= t.matrix;
			return *this;
		}

		Transform && operator *= (const Transform & t) &&
		{
			*_matrix *= t.matrix;
			return move(*this);
		}

		Transform & translate(const Vector<T> & s) &
		{
			_matrix->translate(s);
			return *this;
		}

		Transform && translate(const Vector<T> & s) &&
		{
			_matrix->translate(s);
			return move(*this);
		}

		Transform & rotate(const Quaternion<T> & q) &
		{
			*_matrix *= q.toMatrix();
			return *this;
		}

		Transform && rotate(const Quaternion<T> & q) &&
		{
			*_matrix *= q.toMatrix();
			return move(*this);
		}

		Transform & scale(const Vector<T> & s) &
		{
			_matrix->scale(s);
			return *this;
		}

		Transform && scale(const Vector<T> & s) &&
		{
			_matrix->scale(s);
			return move(*this);
		}

	protected:
		AlignedMatrix<T> _matrix;
	};

	template<class T>
	Transform<T> operator * (const Transform<T> & t1, const Transform<T> & t2)
	{
		return *t1._matrix * *t2._matrix;
	}

	using FloatTransform = Transform<float>;
	using DoubleTransform = Transform<double>;

	using ftrans = FloatTransform;
	using dtrans = DoubleTransform;
}

//---------------------------------------------------------------------------
#endif

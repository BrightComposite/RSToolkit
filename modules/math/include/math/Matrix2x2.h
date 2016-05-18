//---------------------------------------------------------------------------

#ifndef MATRIX2x2_H
#define MATRIX2x2_H

//---------------------------------------------------------------------------

#include "Vector.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T>
	struct alignas(sizeof(T) * 4) Matrix2x2
	{
		typedef intrin_data<T, 4> Data;
		typedef typename Data::type IntrinType;
		typedef Intrinsic<T, 4> Intrin;
		typedef array<T, 2> Row;

		static const Matrix2x2 initial;
		static const Matrix2x2 negative;
		static const Matrix2x2 inversemask;

		union
		{
			struct
			{
				T m00, m01;
				T m10, m11;
			};

			struct
			{
				T xx, xy;
				T yx, yy;
			};

			T a[4];
			array<T, 4> m;
			array<Row, 2> rows;
			Data data;
			IntrinType intrinsic;
		};

		Matrix2x2() : data {initial.data} {}
		Matrix2x2(const Matrix2x2 & matrix) : data {matrix.data} {}
		Matrix2x2(const Data & data) : data {data} {}

		Matrix2x2(const T(&m)[4]) : a {m[0], m[1], m[2], m[3]} {}
		Matrix2x2(const T(&v)[2][2]) : a {v[0][0], v[0][1], v[1][0], v[1][1]} {}
		Matrix2x2(T xx, T xy, T yx, T yy) : a {xx, xy, yx, yy} {}

		Matrix2x2 & operator = (const Matrix2x2 & matrix)
		{
			data = matrix.data;
			return *this;
		}

		void clear()
		{
			data = Vector<T>::zero.data;
		}

		Matrix2x2 & identity()
		{
			data = initial.data;
			return *this;
		}

		Matrix2x2 & identity(float a)
		{
			data = Intrin::load(a, 0, 0, a);
			return *this;
		}

		Matrix2x2 & operator += (const Matrix2x2 & mat);
		Matrix2x2 & operator -= (const Matrix2x2 & mat);
		Matrix2x2 & operator *= (const Matrix2x2 & mat);
		Matrix2x2 & operator *= (T val);
		Matrix2x2 & operator /= (T val);

		const Row & operator [] (size_t row) const
		{
			return rows[row];
		}

		Row & operator [] (size_t row)
		{
			return rows[row];
		}

		T operator () (size_t i) const
		{
			return m[i];
		}

		T & operator () (size_t i)
		{
			return m[i];
		}

		T operator () (size_t row, size_t column) const
		{
			return rows[row][column];
		}

		T & operator () (size_t row, size_t column)
		{
			return rows[row][column];
		}

		operator array<T, 4> & ()
		{
			return m;
		}

		operator const array<T, 4> & () const
		{
			return m;
		}

		operator array<Row, 2> & ()
		{
			return rows;
		}

		operator const array<Row, 2> & () const
		{
			return rows;
		}

		operator Data & ()
		{
			return data;
		}

		operator const Data & () const
		{
			return data;
		}

		operator IntrinType & ()
		{
			return intrinsic;
		}

		operator const IntrinType & () const
		{
			return intrinsic;
		}

		T norm() const
		{
			return std::max(std::abs(m[0]) + std::abs(m[1]), std::abs(m[2]) + std::abs(m[3]));
		}

		T determinant() const
		{
			return m[0] * m[3] - m[2] * m[1];
		}

		Matrix2x2 inverse() const;
		Matrix2x2 & invert();

		Matrix2x2 & transpose()
		{
			swap(m[1], m[2]);
			return *this;
		}

		Vector<T> transform(const Vector<T> & v) const;

        static inline Matrix2x2 rotationMatrix(T radians)
		{
			const T c = Math<T>::cos(radians), s = Math<T>::sin(radians);
			return {c, -s, s, c};
		}

        static inline Matrix2x2 scaleMatrix(const Vector<T> & v)
		{
			return {v.x, 0, 0, v.y};
		}
    };

	template<class T>
	Matrix2x2<T> operator + (const Matrix2x2<T> & a, const Matrix2x2<T> & b)
	{
		return {Intrinsic<T, 4>::add(a.data, b.data)};
	}

	template<class T>
	Matrix2x2<T> operator - (const Matrix2x2<T> & a, const Matrix2x2<T> & b)
	{
		return {Intrinsic<T, 4>::sub(a.data, b.data)};
	}

	template<class T>
	Matrix2x2<T> operator * (const Matrix2x2<T> & a, const Matrix2x2<T> & b)
	{
		typedef Intrinsic<T, 4> Intrin;

		return {
			Intrin::add(
				Intrin::mul(
					Intrin::template shuffle<0, 0, 2, 2>(a.data),
					Intrin::template shuffle<0, 1, 0, 1>(b.data)
					), 
				Intrin::mul(
					Intrin::template shuffle<1, 1, 3, 3>(a.data),
					Intrin::template shuffle<2, 3, 2, 3>(b.data)
					)
				)
		};
	}

	template<class T>
	Matrix2x2<T> operator * (const Matrix2x2<T> & a, T b)
	{
		return {Intrinsic<T, 4>::mul(a.data, Intrinsic<T, 4>::fill(b))};
	}

	template<class T>
	Matrix2x2<T> operator * (T b, const Matrix2x2<T> & a)
	{
		return {Intrinsic<T, 4>::mul(Intrinsic<T, 4>::fill(b), a.data)};
	}

	template<class T>
	Vector<T> operator * (const Matrix2x2<T> & m, const Vector<T> & v)
	{
		return {
			m(0, 0) * v.x + m(0, 1) * v.y,
			m(1, 0) * v.x + m(1, 1) * v.y,
			v.z,
			v.w
		};
	}

	template<class T>
	Matrix2x2<T> & Matrix2x2<T>::operator += (const Matrix2x2 & mat)
	{
		Intrin::add(data, mat.data, data);
		return *this;
	}

	template<class T>
	Matrix2x2<T> & Matrix2x2<T>::operator -= (const Matrix2x2 & mat)
	{
		Intrin::sub(data, mat.data, data);
		return *this;
	}

	template<class T>
	Matrix2x2<T> & Matrix2x2<T>::operator *= (const Matrix2x2 & mat)
	{
		Intrin::add(
			Intrin::mul(
				Intrin::template shuffle<0, 0, 2, 2>(data),
				Intrin::template shuffle<0, 1, 0, 1>(mat.data)
				),
			Intrin::mul(
				Intrin::template shuffle<1, 1, 3, 3>(data),
				Intrin::template shuffle<2, 3, 2, 3>(mat.data)
				),
			data
			);

		return *this;
	}

	template<class T>
	Matrix2x2<T> & Matrix2x2<T>::operator *= (T val)
	{
		Intrin::mul(data, Intrin::fill(val), data);
		return *this;
	}

	template<class T>
	Matrix2x2<T> & Matrix2x2<T>::operator /= (T val)
	{
		Intrin::div(data, Intrin::fill(val), data);
		return *this;
	}

	template<class T>
	inline Vector<T> Matrix2x2<T>::transform(const Vector<T> & v) const
	{
		return operator * (*this, v);
	}

	template<class T>
	inline Matrix2x2<T> Matrix2x2<T>::inverse() const
	{
		return {Intrin::div(Intrin::mul(Intrin::template shuffle<3, 1, 2, 0>(data), inversemask), Intrin::fill(determinant()))};
	}

	template<class T>
	inline Matrix2x2<T> & Matrix2x2<T>::invert()
	{
		Intrin::div(Intrin::mul(Intrin::template shuffle<3, 1, 2, 0>(data), inversemask), Intrin::fill(determinant()), data);
		return *this;
	}

	template<class T>
	inline void print(String & s, const Matrix2x2<T> & mat)
	{
		s << String::assemble("(", mat(0, 0), ", ", mat(0, 1), " | ", mat(1, 0), ", ", mat(1, 1), ")");
	}
}

//---------------------------------------------------------------------------
#endif

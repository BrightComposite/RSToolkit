//---------------------------------------------------------------------------

#pragma once

#ifndef MATRIX_H
#define MATRIX_H

//---------------------------------------------------------------------------

#include "Vector.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<class T>
    struct alignas(sizeof(T) * 4) Matrix : AlignedAllocator
    {
		typedef intrin_data<T, 4> Data;
		typedef intrin_data<T, 2> Pair;
		typedef Intrinsic<T, 4> Intrin;
		typedef Vector<T> Row;

		union
		{
			struct
			{
				T m00, m01, m02, m03;
				T m10, m11, m12, m13;
				T m20, m21, m22, m23;
				T m30, m31, m32, m33;
			};

			struct
			{
				T xx, xy, xz, xw;
				T yx, yy, yz, yw;
				T zx, zy, zz, zw;
				T wx, wy, wz, ww;
			};

			struct
			{
				Row x, y, z, w;
			};

			array<T, 16> m;
			array<array<T, 4>, 4> a;

			array<Vector<T>, 4> v;
			array<Row, 4>		rows;
			array<Data, 4>		data;
			array<Pair, 8>		pairs;
		};

		member_cast(m, array<T, 16>);
		member_cast(rows, array<Row, 4>);
		member_cast(data, array<Data, 4>);
		member_cast(a, array<array<T, 4>, 4>);

		Matrix() : x(Vector<T>::positiveX), y(Vector<T>::positiveY), z(Vector<T>::positiveZ), w(Vector<T>::positiveW) {}
		Matrix(const Matrix & matrix) : x {matrix.x}, y {matrix.y}, z {matrix.z}, w {matrix.w} {}
		Matrix(Matrix && matrix) : m {move(matrix.m)} {}

		Matrix(const Row (& v)[4]) : x {v[0]}, y {v[1]}, z {v[2]}, w {v[3]} {}
		Matrix(const Row & x, const Row & y, const Row & z, const Row & w) : x {x}, y {y}, z {z}, w {w} {}
		Matrix(const Row & x, const Row & y, const Row & z) : x {x}, y {y}, z {z}, w {Vector<T>::positiveW} {}

		Matrix(const T(&m)[16]) :
			x(m[0x0], m[0x1], m[0x2], m[0x3]),
			y(m[0x4], m[0x5], m[0x6], m[0x7]),
			z(m[0x8], m[0x9], m[0xA], m[0xB]),
			w(m[0xC], m[0xD], m[0xE], m[0xF])
		{}

		Matrix(const T(&rows)[4][4]) : x(rows[0]), y(rows[1]), z(rows[2]), w(rows[3]) {}

		Matrix(
			T xx, T xy, T xz, T xw,
			T yx, T yy, T yz, T yw,
			T zx, T zy, T zz, T zw,
			T wx, T wy, T wz, T ww
			) :
			x(xx, xy, xz, xw),
			y(yx, yy, yz, yw),
			z(zx, zy, zz, zw),
			w(wx, wy, wz, ww)
		{}
		
		Matrix & operator = (const Matrix & matrix)
		{
			Memory<T>::move(m, matrix.m);
			return *this;
		}

		Matrix & operator = (Matrix && matrix)
		{
			m.swap(matrix.m);
			return *this;
		}

		Matrix & clear()
        {
			m.fill(0);
			return *this;
        }

		Matrix & identity()
		{
			x = Vector<T>::positiveX;
			y = Vector<T>::positiveY;
			z = Vector<T>::positiveZ;
			w = Vector<T>::positiveW;

			return *this;
		}

		Matrix & identity(T a)
		{
			Vector<T> v(a);

			x = v.maskX();
			y = v.maskY();
			z = v.maskZ();
			w = v.maskW();

			return *this;
		}

		Matrix & operator += (const Matrix & matrix)
		{
			rows[0] += matrix[0];
			rows[1] += matrix[1];
			rows[2] += matrix[2];
			rows[3] += matrix[3];

			return *this;
		}

		Matrix & operator -= (const Matrix & matrix)
		{
			rows[0] -= matrix[0];
			rows[1] -= matrix[1];
			rows[2] -= matrix[2];
			rows[3] -= matrix[3];

			return *this;
		}

		Matrix & operator *= (const Matrix & matrix)
		{
			return *this = operator * (*this, matrix);
		}

		Matrix & operator *= (T value)
		{
			rows[0] *= value;
			rows[1] *= value;
			rows[2] *= value;
			rows[3] *= value;

			return *this;
		}

		Matrix & operator /= (T value)
		{
			rows[0] /= value;
			rows[1] /= value;
			rows[2] /= value;
			rows[3] /= value;

			return *this;
		}

		Row & operator [] (int row) &
		{
			return rows[row];
		}

		const Row & operator [] (int row) const &
		{
			return rows[row];
		}

		Row && operator [] (int row) &&
		{
			return move(rows[row]);
		}

		T & operator () (int i)
        {
            return m[i];
        }

		T operator () (int i) const
		{
			return m[i];
		}

		T & operator () (int row, int column)
		{
			return a[row][column];
		}

		T operator () (int row, int column) const
		{
			return a[row][column];
		}

        inline T norm() const;
		inline Vector<T> determinant() const;

		Matrix & invert() &
		{
			getInverse(*this);
			return *this;
		}

		Matrix && invert() &&
		{
			getInverse(*this);
			return forward<Matrix>(*this);
		}

		Matrix & transpose() &
		{
			getTransposition(*this);
			return *this;
		}

		Matrix && transpose() &&
		{
			getTransposition(*this);
			return forward<Matrix>(*this);
		}

		Matrix inverse() const
		{
			Matrix mat;
			getInverse(mat);
			return mat;
		}

		Matrix transposition() const
		{
			Matrix mat;
			getTransposition(mat);
			return mat;
		}

		inline void getInverse(Matrix & mat) const;
		inline void getTransposition(Matrix & mat) const;

		inline Vector<T> transformPoint(const Vector<T> & b) const;
		inline Vector<T> transformDirection(const Vector<T> & b) const;

		inline Matrix & apply(const Matrix<T> & m);

		inline Matrix & scale(const Vector<T> & s);
		inline Matrix & scale(const T s);

		inline Matrix & rotateX(T);
		inline Matrix & rotateY(T);
		inline Matrix & rotateZ(T);
		inline Matrix & rotate(const Vector<T> & axis, T angle);
		inline Matrix & rotate(const Vector<T> & rotation);

		inline Matrix & mirrorX();
		inline Matrix & mirrorY();
		inline Matrix & mirrorZ();

		inline Matrix & translate(const Vector<T> & v);

        static inline Matrix rotationX(T angle);
        static inline Matrix rotationY(T angle);
        static inline Matrix rotationZ(T angle);
        static inline Matrix rotation(const Vector<T> & v, T angle);
        static inline Matrix rotation(const Vector<T> & r);
        static inline Matrix scaling(const Vector<T> & s);
        static inline Matrix translation(const Vector<T> & t);

		static inline Matrix ortho(T x0, T x1, T y0, T y1, T z0, T z1);
		static inline Matrix orthot(T x0, T x1, T y0, T y1, T z0, T z1);
		static inline Matrix perspective(T fov, T aspect, T z0, T z1);
		static inline Matrix perspectivet(T fov, T aspect, T z0, T z1);
		static inline Matrix frustum(T x0, T x1, T y0, T y1, T z0, T z1);
		static inline Matrix lookAt(const Vector<T> & position, const Vector<T> & center, const Vector<T> & up);
		static inline Matrix lookTo(const Vector<T> & position, const Vector<T> & direction, const Vector<T> & up);
    };

	using FloatMatrix = Matrix<float>;
	using DoubleMatrix = Matrix<double>;
	using floatm = FloatMatrix;
	using doublem = DoubleMatrix;

	template<class T>
	using AlignedMatrix = Aligned<Matrix<T>>;

	using fmat = AlignedMatrix<float>;
	using dmat = AlignedMatrix<double>;

	template<class T>
	inline Matrix<T> operator + (const Matrix<T> & m1, const Matrix<T> & m2)
	{
		return Matrix<T>(m1) += m2;
	}

	template<class T>
	inline Matrix<T> operator - (const Matrix<T> & m1, const Matrix<T> & m2)
	{
		return Matrix<T>(m1) -= m2;
	}

	template<class T>
	inline Matrix<T> operator * (const Matrix<T> & a, const Matrix<T> & b)
	{
		return {
			a[0] * b[0].spreadX() + a[1] * b[0].spreadY() + a[2] * b[0].spreadZ() + a[3] * b[0].spreadW(),
			a[0] * b[1].spreadX() + a[1] * b[1].spreadY() + a[2] * b[1].spreadZ() + a[3] * b[1].spreadW(),
			a[0] * b[2].spreadX() + a[1] * b[2].spreadY() + a[2] * b[2].spreadZ() + a[3] * b[2].spreadW(),
			a[0] * b[3].spreadX() + a[1] * b[3].spreadY() + a[2] * b[3].spreadZ() + a[3] * b[3].spreadW()
		};
	}

	template<class T>
	inline Matrix<T> operator * (const Matrix<T> & matrix, T value)
	{
		return Matrix<T>(matrix) *= value;
	}

	template<class T>
	inline Matrix<T> operator * (T value, const Matrix<T> & matrix)
	{
		return Matrix<T>(matrix) *= value;
	}

	template<class T>
	inline Vector<T> operator * (const Matrix<T> & matrix, const Vector<T> & vec)
	{
		matrix.transformPoint(vec);
	}

	template<class T>
	inline T Matrix<T>::norm() const
	{
		return std::max({v[0].max(), v[1].max(), v[2].max(), v[3].max()});
	}

	template<class T>
	inline Vector<T> Matrix<T>::determinant() const
	{
		Matrix tmp1, tmp2;

		tmp1[0] = rows[2].template shuffle<1, 0, 0, 0>();
		tmp1[1] = rows[2].template shuffle<2, 2, 1, 1>();
		tmp1[2] = rows[2].template shuffle<3, 3, 3, 2>();
		tmp1[3] = rows[3].template shuffle<1, 0, 0, 0>();
		tmp2[0] = rows[3].template shuffle<2, 2, 1, 1>();
		tmp2[1] = rows[3].template shuffle<3, 3, 3, 2>();

		tmp2[2] = tmp1[1] * tmp2[1] - tmp1[2] * tmp2[0];
		tmp2[3] = tmp1[0] * tmp2[1] - tmp1[2] * tmp1[3];
		tmp2[1] = tmp1[0] * tmp2[0] - tmp1[1] * tmp1[3];

		tmp1[0] = rows[1].template shuffle<1, 0, 0, 0>();
		tmp1[1] = rows[1].template shuffle<2, 2, 1, 1>();
		tmp1[2] = rows[1].template shuffle<3, 3, 3, 2>();

		return rows[0].template negate<0, 1, 0, 1>().dot(tmp1[0] * tmp2[2] - tmp1[1] * tmp2[3] + tmp1[2] * tmp2[1]);
	}

	template<class T>
	inline void Matrix<T>::getInverse(Matrix<T> & m) const
	{
		Vector<T> r[4], temp;

		temp = v[0].template shuffle<0, 1, 0, 1>(v[1]);
		r[1] = v[2].template shuffle<0, 1, 0, 1>(v[3]);
		r[0] = temp.template shuffle<0, 2, 0, 2>(r[1]);
		r[1] = r[1].template shuffle<1, 3, 1, 3>(temp);
		temp = v[0].template shuffle<2, 3, 2, 3>(v[1]);
		r[3] = v[2].template shuffle<2, 3, 2, 3>(v[3]);
		r[2] = temp.template shuffle<0, 2, 0, 2>(r[3]);
		r[3] = r[3].template shuffle<1, 3, 1, 3>(temp);

		temp = r[2] * r[3];
		temp = temp.template shuffle<1, 0, 3, 2>() - temp.template shuffle<3, 2, 1, 0>();
		m[0] = -r[1] * temp;
		m[1] = -r[0] * temp;
		m[1] = m[1].template shuffle<2, 3, 0, 1>();

		temp = r[1] * r[2];
		temp = temp.template shuffle<1, 0, 3, 2>() - temp.template shuffle<3, 2, 1, 0>();
		m[0] += r[3] * temp;
		m[3] = r[0] * temp;
		m[3] = m[3].template shuffle<2, 3, 0, 1>();

		temp = r[1].template shuffle<2, 3, 0, 1>() * r[3];
		temp = temp.template shuffle<1, 0, 3, 2>() - temp.template shuffle<3, 2, 1, 0>();
		r[2] = r[2].template shuffle<2, 3, 0, 1>();
		m[0] += r[2] * temp;
		m[2] = r[0] * temp;
		m[2] = m[2].template shuffle<2, 3, 0, 1>();

		temp = r[0] * r[1];
		temp = temp.template shuffle<1, 0, 3, 2>() - temp.template shuffle<3, 2, 1, 0>();
		m[2] -= r[3] * temp;
		m[3] += r[2] * temp;

		temp = r[0] * r[3];
		temp = temp.template shuffle<1, 0, 3, 2>() - temp.template shuffle<3, 2, 1, 0>();
		m[1] -= r[2] * temp;
		m[2] += r[1] * temp;

		temp = r[0] * r[2];
		temp = temp.template shuffle<1, 0, 3, 2>() - temp.template shuffle<3, 2, 1, 0>();
		m[1] += r[3] * temp;
		m[3] -= r[1] * temp;

		auto det = r[0].dot(m[0]).invert();
		m[0] *= det; m[1] *= det; m[2] *= det; m[3] *= det;
	}

	template<class T>
	inline void Matrix<T>::getTransposition(Matrix<T> & mat) const
	{
		auto v0 = rows[0].template shuffle<0, 1, 0, 1>(rows[1]);
		auto v1 = rows[0].template shuffle<2, 3, 2, 3>(rows[1]);
		auto v2 = rows[2].template shuffle<0, 1, 0, 1>(rows[3]);
		auto v3 = rows[2].template shuffle<2, 3, 2, 3>(rows[3]);

		Matrix tmp {v0, v1, v2, v3};

		mat[0] = tmp[0].template shuffle<0, 2, 0, 2>(tmp[2]);
		mat[1] = tmp[0].template shuffle<1, 3, 1, 3>(tmp[2]);
		mat[2] = tmp[1].template shuffle<0, 2, 0, 2>(tmp[3]);
		mat[3] = tmp[1].template shuffle<1, 3, 1, 3>(tmp[3]);
	}

	template<class T>
	inline Vector<T> Matrix<T>::transformPoint(const Vector<T> & v) const
	{
		return {
			sum(rows[0] * v) + m03,
			sum(rows[1] * v) + m13,
			sum(rows[2] * v) + m23
		};
	}

	template<class T>
	inline Vector<T> Matrix<T>::transformDirection(const Vector<T> & v) const
	{
		return {
			sum(rows[0] * v),
			sum(rows[1] * v),
			sum(rows[2] * v)
		};
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::apply(const Matrix<T> & m)
	{
		return *this *= m;
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::scale(const Vector<T> & s)
	{
		return apply(scaling(s));
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::scale(const T s)
	{
		return apply(scaling({s}));
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotateX(T angle)
	{
		return apply(rotationX(angle));
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotateY(T angle)
	{
		return apply(rotationY(angle));
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotateZ(T angle)
	{
		return apply(rotationZ(angle));
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotate(const Vector<T> & axis, T angle)
	{
		return apply(rotation(axis, angle));
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotate(const Vector<T> & v)
	{
		return apply(rotation(v));
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::mirrorX()
	{
		return apply({
			Vector<T>::negativeX,
			Vector<T>::positiveY,
			Vector<T>::positiveZ
		});
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::mirrorY()
	{
		return apply({
			Vector<T>::positiveX,
			Vector<T>::negativeY,
			Vector<T>::positiveZ
		});
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::mirrorZ()
	{
		return apply({
			Vector<T>::positiveX,
			Vector<T>::positiveY,
			Vector<T>::negativeZ
		});
	}

	template<class T>
	Matrix<T> & Matrix<T>::translate(const Vector<T> & v)
	{
		return apply(translation(v));
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationX(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			Vector<T>::positiveX,		// 1,  0,  0
			v.template shuffle<3, 1, 2, 3>(),	// 0,  c, -s
			v.template shuffle<3, 0, 1, 3>()		// 0,  s,  c
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationY(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			v.template shuffle<1, 3, 0, 3>(),	// c,  0,  s
			Vector<T>::positiveY,		// 0,  1,  0
			v.template shuffle<2, 3, 1, 3>()		//-s,  0,  c
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationZ(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			v.template shuffle<1, 2, 3, 3>(),	// c, -s,  0
			v.template shuffle<0, 1, 3, 3>(),	// s,  c,  0
			Vector<T>::positiveZ
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotation(const Vector<T> & v, T angle)
	{
		auto sc = VectorMath<T>::trigon(angle);
		auto cc = sc.template shuffle<1, 1, 1, 3>();
		auto nc = (Vector<T>::oneXYZ - cc) * v;
		auto ss = sc.template shuffle<0, 0, 0, 3>() * v;

		sc = nc.template shuffle<1, 0, 0, 3>() * v.template shuffle<2, 2, 1, 3>();
		cc += v * nc;

		nc = sc + ss;
		sc -= ss;

		return {
			cc.template shuffle<0, 0, 2, 2>(nc).template shuffle<0, 2, 1, 3>(sc), // cc.x, nc.z, sc.y, 0,
			sc.template shuffle<2, 2, 1, 1>(cc).template shuffle<0, 2, 0, 3>(nc), // sc.z, cc.y, nc.x, 0,
			nc.template shuffle<1, 1, 0, 0>(sc).template shuffle<0, 2, 2, 3>(cc)  // nc.y, sc.x, cc.z, 0,
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotation(const Vector<T> & euler)
	{
		Vector<T> sine, cosine;
		VectorMath<T>::sincos(euler, sine, cosine);

		Vector<T> x = sine.template shuffle<0, 0, 0, 0>(cosine).template blend<0, 1, 1, 0>(Vector<T>::positiveY);									// [ sx, 1, 0, cx ]
		Vector<T> y = sine.template shuffle<1, 1, 1, 1>(cosine).template blend<0, 1, 1, 0>(Vector<T>::positiveY);									// [ sy, 1, 0, cy ]
		Vector<T> z = sine.template shuffle<2, 2, 2, 2>(cosine).template blend<0, 1, 1, 0>(Vector<T>::positiveY).template shuffle<3, 0, 1, 2>();	// [ cz, sz, 1, 0 ]

		Vector<T> yz = y.template shuffle<0, 0, 3, 2>().negateZ() * z;	// [ sy * cz, sy * sz, -cy, 0 ]
		Vector<T> mz = z.template shuffle<1, 0, 2, 3>().negateX();		// [ -sz, cz, 1, 0 ]

		return {
			y.template shuffle<3, 3, 0, 0>() * z,						//  cy * cz,			   | cy * sz,				| sy,	  | 0
			x.template shuffle<3, 3, 2, 2>() * mz + x.spreadX() * yz,	// -cx * sz + sx * sy * cz | cx * cz + sx * sy * sz |-sx * cy |	0
			x.template shuffle<0, 0, 2, 2>() * mz - x.spreadW() * yz	// -sx * sz - cx * sy * cz | sx * cz - cx * sy * sz | cx * cy | 0
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::scaling(const Vector<T> & s)
	{
		return {
			s.maskX(), s.maskY(), s.maskZ()
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::translation(const Vector<T> & t)
	{
		return {
			Vector<T>::positiveX.template blend<0, 0, 0, 1>(t.spreadX()),
			Vector<T>::positiveY.template blend<0, 0, 0, 1>(t.spreadY()),
			Vector<T>::positiveZ.template blend<0, 0, 0, 1>(t.spreadZ())
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::ortho(T x0, T x1, T y0, T y1, T z0, T z1)
	{
		const Vector<T> max { x1, y1, z1, 1 };
		const Vector<T> min { x0, y0, z0, 0 };

		Vector<T> d = (max - min).inverse();
		const Vector<T> t = -(max + min) * d;
		d *= Vector<T>::two;

		return {
			d.maskX().template blend<0, 0, 0, 1>(t.spreadX()),	// 2/w |  0  |  0  | -(x0+x1)/w
			d.maskY().template blend<0, 0, 0, 1>(t.spreadY()),	//  0  | 2/h |  0  | -(y0+y1)/h
			d.maskZ().template blend<0, 0, 0, 1>(t.spreadZ()),	//  0  |  0  | 2/l | -(z0+z1)/l
			Vector<T>::positiveW								//  0  |  0  |  0  |    1
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::orthot(T x0, T x1, T y0, T y1, T z0, T z1)
	{
		const Vector<T> max { x1, y1, z1, 1 };
		const Vector<T> min { x0, y0, z0, 0 };

		Vector<T> d = (max - min).inverse();
		const Vector<T> t = -(max + min) * d;
		d *= Vector<T>::two;

		return {
			d.maskX(),											//    2/w    |     0     |     0    | 0
			d.maskY(),											//     0     |    2/h    |     0    | 0
			d.maskZ(),											//     0     |     0     |    2/l   | 0
			t.template blend<0, 0, 0, 1>(Vector<T>::positiveW)	//-(x0+x1)/w |-(y0+y1)/h |-(z0+z1)/l| 1
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::perspective(T fov, T aspect, T z0, T z1)
	{
		const T f = 1 / std::tan(Math<T>::dtor(fov) / 2);
		const T z = z1 / (z1 - z0);
		const Vector<T> v = {f * aspect, f, z, -z0 * z};

		return {
			v.maskX(),						// f/a | 0 | 0 |  0
			v.maskY(),						//  0  | f | 0 |  0
			v.template mask<0, 0, 1, 1>(),	//  0  | 0 | z |-z0*z
			Vector<T>::positiveZ			//  0  | 0 | 1 |  0
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::perspectivet(T fov, T aspect, T z0, T z1)
	{
		const T f = 1 / std::tan(Math<T>::dtor(fov) / 2);
		const T z = z1 / (z1 - z0);
		const Vector<T> v = {f * aspect, f, z, -z0 * z};

		return {
			v.maskX(),											// f*a | 0 |  0  | 0
			v.maskY(),											//  0  | f |  0  | 0
			v.template blend<1, 1, 0, 1>(Vector<T>::positiveW),	//  0  | 0 |  z  | 1
			v.spreadW().maskZ(),								//  0  | 0 |-z0*z| 0
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::frustum(T x0, T x1, T y0, T y1, T z0, T z1)
	{
		const T n = 2 * z1;
		const Vector<T> v {1 / (x1 - x0), 1 / (y1 - y0), 1 / (z1 - z0), 0};

		return {
			v.spreadX() * Vector<T>{ n, 0, x1 + x0, 0      },
			v.spreadY() * Vector<T>{ 0, n, y1 + y0, 0      },
			v.spreadZ() * Vector<T>{ 0, 0, z1 + z0, n * z0 },
			Vector<T>::negativeZ
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::lookAt(const Vector<T> & position, const Vector<T> & center, const Vector<T> & up)
	{
		const Vector<T> f = (center - position).normalize();
		const Vector<T> s = (up.cross(f)).normalize();
		const Vector<T> u = (f.cross(s)).normalize();

		return {
			s.template blend<0, 0, 0, 1>(-s.dot(position)),
			u.template blend<0, 0, 0, 1>(-u.dot(position)),
			f.template blend<0, 0, 0, 1>(-f.dot(position))
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::lookTo(const Vector<T> & position, const Vector<T> & direction, const Vector<T> & up)
	{
		const Vector<T> f = direction;
		const Vector<T> s = (up.cross(f)).normalize();
		const Vector<T> u = (f.cross(s)).normalize();

		return {
			s.template blend<0, 0, 0, 1>(-s.dot(position)),
			u.template blend<0, 0, 0, 1>(-u.dot(position)),
			f.template blend<0, 0, 0, 1>(-f.dot(position))
		};
	}

	template<class T>
	inline void print(String & s, const Matrix<T> & mat)
	{
		s << String::assemble(
			"(",
			mat(0, 0), ", ", mat(0, 1), ", ", mat(0, 2), ", ", mat(0, 3), " | ",
			mat(1, 0), ", ", mat(1, 1), ", ", mat(1, 2), ", ", mat(1, 3), " | ",
			mat(2, 0), ", ", mat(2, 1), ", ", mat(2, 2), ", ", mat(2, 3), " | ",
			mat(3, 0), ", ", mat(3, 1), ", ", mat(3, 2), ", ", mat(3, 3),
			")"
		);
	}
}

//---------------------------------------------------------------------------
#endif

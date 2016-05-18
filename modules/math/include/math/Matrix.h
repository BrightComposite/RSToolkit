//---------------------------------------------------------------------------

#ifndef MATRIX4x4_H
#define MATRIX4x4_H

//---------------------------------------------------------------------------

#include "Vector.h"
#include "Matrix2x2.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T>
    struct alignas(sizeof(T) * 4) Matrix
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

		Matrix() : x {Vector<T>::positiveX}, y {Vector<T>::positiveY}, z {Vector<T>::positiveZ}, w {Vector<T>::positiveW} {}
		Matrix(const Matrix & matrix) : x {matrix.x}, y {matrix.y}, z {matrix.z}, w {matrix.w} {}
		Matrix(Matrix && matrix) : m {move(matrix.m)} {}

		Matrix(const Matrix2x2<T> & m) : x {m.xx, m.xy, 0, 0}, y {m.yx, m.yy, 0, 0}, z {Vector<T>::positiveZ}, w {Vector<T>::positiveW} {}
		Matrix(const Row (& v)[4]) : x {v[0]}, y {v[1]}, z {v[2]}, w {v[3]} {}
		Matrix(const Data & x, const Data & y, const Data & z, const Data & w) : x {x}, y {y}, z {z}, w {w} {}
		Matrix(const Data & x, const Data & y, const Data & z) : x {x}, y {y}, z {z}, w {Vector<T>::positiveW} {}

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
			m.assign(0);
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

		Matrix & operator *= (const Matrix & m2)
		{
			return *this = operator * (*this, m2);
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

		operator array<T, 16> & () &
		{
			return m;
		}

		operator const array<T, 16> & () const &
		{
			return m;
		}

		operator array<T, 16> && () &&
		{
			return move(m);
		}

		operator array<Vector<T>, 4> & () &
		{
			return rows;
		}

		operator const array<Vector<T>, 4> & () const &
		{
			return rows;
		}

		operator array<Vector<T>, 4> && () &&
		{
			return move(rows);
		}

		operator array<array<T, 4>, 4> & () &
		{
			return a;
		}

		operator const array<array<T, 4>, 4> & () const &
		{
			return a;
		}

		operator array<array<T, 4>, 4> && () &&
		{
			return move(a);
		}

		operator array<Data, 4> & () &
		{
			return data;
		}

		operator const array<Data, 4> & () const &
		{
			return data;
		}

		operator array<Data, 4> && () &&
		{
			return move(data);
		}

        inline T norm() const;
		inline Vector<T> determinant() const;

		Matrix & invert()
		{
			getInverse(*this);
			return *this;
		}

		Matrix & transpose()
		{
			getTransposition(*this);
			return *this;
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

        static inline Matrix rotationMatrixX(T angle);
        static inline Matrix rotationMatrixY(T angle);
        static inline Matrix rotationMatrixZ(T angle);
        static inline Matrix rotationMatrix(const Vector<T> & v, T angle);
        static inline Matrix rotationMatrix(const Vector<T> & r);
        static inline Matrix scaleMatrix(const Vector<T> & s);
        static inline Matrix translationMatrix(const Vector<T> & t);
    };

	using FloatMatrix = Matrix<float>;
	using DoubleMatrix = Matrix<double>;

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
	inline Matrix<T> operator * (const Matrix<T> & m1, const Matrix<T> & m2)
	{
		return {
			m2[0] * m1[0].spreadX() + m2[1] * m1[0].spreadY() + m2[2] * m1[0].spreadZ() + m2[3] * m1[0].spreadW(),
			m2[0] * m1[1].spreadX() + m2[1] * m1[1].spreadY() + m2[2] * m1[1].spreadZ() + m2[3] * m1[1].spreadW(),
			m2[0] * m1[2].spreadX() + m2[1] * m1[2].spreadY() + m2[2] * m1[2].spreadZ() + m2[3] * m1[2].spreadW(),
			m2[0] * m1[3].spreadX() + m2[1] * m1[3].spreadY() + m2[2] * m1[3].spreadZ() + m2[3] * m1[3].spreadW()
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
		return std::max({v[0].norm(), v[1].norm(), v[2].norm(), v[3].norm()});
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
		Matrix tmp {
			rows[0].template shuffle<0, 1, 0, 1>(rows[1]),
			rows[0].template shuffle<2, 3, 2, 3>(rows[1]),
			rows[2].template shuffle<0, 1, 0, 1>(rows[3]),
			rows[2].template shuffle<2, 3, 2, 3>(rows[3])
		};

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
	inline Matrix<T> & Matrix<T>::scale(const Vector<T> & s)
	{
		return *this *= scaleMatrix(s);
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::scale(const T s)
	{
		return *this *= scaleMatrix({s});
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotateX(T angle)
	{
		return *this *= rotationMatrixX(angle);
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotateY(T angle)
	{
		return *this *= rotationMatrixY(angle);
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotateZ(T angle)
	{
		return *this *= rotationMatrixZ(angle);
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotate(const Vector<T> & axis, T angle)
	{
		return *this *= rotationMatrix(axis, angle);
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::rotate(const Vector<T> & rotation)
	{
		return *this *= rotationMatrix(rotation);
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::mirrorX()
	{
		return *this *= {
			Vector<T>::negativeX,
			Vector<T>::positiveY,
			Vector<T>::positiveZ
		};
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::mirrorY()
	{
		return *this *= {
			Vector<T>::positiveX,
			Vector<T>::negativeY,
			Vector<T>::positiveZ
		};
	}

	template<class T>
	inline Matrix<T> & Matrix<T>::mirrorZ()
	{
		return *this *= {
			Vector<T>::positiveX,
			Vector<T>::positiveY,
			Vector<T>::negativeZ
		};
	}

	template<class T>
	Matrix<T> & Matrix<T>::translate(const Vector<T> & v)
	{
		return *this *= translationMatrix(v);
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrixX(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			Vector<T>::positiveX,		// 1,  0,  0
			v.template shuffle<3, 1, 2, 3>(),	// 0,  c, -s
			v.template shuffle<3, 0, 1, 3>()		// 0,  s,  c
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrixY(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			v.template shuffle<1, 3, 0, 3>(),	// c,  0,  s
			Vector<T>::positiveY,		// 0,  1,  0
			v.template shuffle<2, 3, 1, 3>()		//-s,  0,  c
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrixZ(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			v.template shuffle<1, 2, 3, 3>(),	// c, -s,  0
			v.template shuffle<0, 1, 3, 3>(),	// s,  c,  0
			Vector<T>::positiveZ
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrix(const Vector<T> & v, T angle)
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
	inline Matrix<T> Matrix<T>::rotationMatrix(const Vector<T> & euler)
	{
		Vector<T> sine, cosine;
		VectorMath<T>::sincos(euler, sine, cosine);

		Vector<T> x = sine.template shuffle<0, 0, 0, 0>(cosine).template blend<0, 1, 1, 0>(Vector<T>::positiveY);	// [ sx, 1, 0, cx ]
		Vector<T> y = sine.template shuffle<1, 1, 1, 1>(cosine).template blend<0, 1, 1, 0>(Vector<T>::positiveY);	// [ sy, 1, 0, cy ]
		Vector<T> z = sine.template shuffle<2, 2, 2, 2>(cosine).template blend<0, 1, 1, 0>(Vector<T>::positiveY).template shuffle<3, 0, 1, 2>(); // [ cz, sz, 1, 0 ]

		Vector<T> yz = y.template shuffle<0, 0, 3, 2>().negateZ() * z;	// [ sy * cz, sy * sz, -cy, 0 ]
		Vector<T> mz = z.template shuffle<1, 0, 2, 3>().negateX();		// [ -sz, cz, 1, 0 ]

		return {
			y.template shuffle<3, 3, 0, 0>() * z,						//  cy * cz,			   | cy * sz,				| sy,	  | 0
			x.template shuffle<3, 3, 2, 2>() * mz + x.spreadX() * yz,	// -cx * sz + sx * sy * cz | cx * cz + sx * sy * sz |-sx * cy |	0
			x.template shuffle<0, 0, 2, 2>() * mz - x.spreadW() * yz		// -sx * sz - cx * sy * cz | sx * cz - cx * sy * sz | cx * cy | 0
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::scaleMatrix(const Vector<T> & s)
	{
		return {
			s.maskX(), s.maskY(), s.maskZ()
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::translationMatrix(const Vector<T> & t)
	{
		return {
			Vector<T>::positiveX.template blend<0, 0, 0, 1>(t.spreadX()),
			Vector<T>::positiveY.template blend<0, 0, 0, 1>(t.spreadY()),
			Vector<T>::positiveZ.template blend<0, 0, 0, 1>(t.spreadZ())
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

	template<class T>
	inline Matrix<T> orthoMatrix(T x0, T x1, T y0, T y1, T z0, T z1)
	{
		static const Vector<T> q { 2,  2, -2, 0 };

		Vector<T> max { x1, y1,    z1,   1 };
		Vector<T> min { x0,  y0, z0, -1 };

		Vector<T> k = (min - max).inverse();
		Vector<T> t = (max + min) * k;
		Vector<T> s = q * k;

		return {
			s.template shuffle<0, 3, 3, 0>(t),
			s.template shuffle<3, 1, 3, 1>(t),
			t.template shuffle<3, 3, 3, 2>().template blend<0, 0, 1, 0>(s),
		};
	}

	template<class T>
	inline Matrix<T> perspectiveMatrix(T fov, T aspect, T z0, T z1)
	{
		const T f = 1 / std::tan(Math<T>::dtor(fov) / 2);
		const T delta = z0 - z1;
		const Vector<T> v = {f / aspect, f, (z1 + z0) / delta, (2 * z1 * z0) / delta};

		return {
			v.maskX(),
			v.maskY(),
			v.template maskV<0, 0, 1, 1>(),
			Vector<T>::negativeZ
		};
	}

	template<class T>
	inline Matrix<T> frustumMatrix(T x0, T x1, T y0, T y1, T z0, T z1)
	{
		const T n = 2 * z0;
		const Vector<T> v {1 / (x1 - x0), 1 / (y1 - y0), 1 / (z0 - z1), 0};

		return {
			v.spreadX() * Vector<T>{ n, 0, x1 + x0,   0       },
			v.spreadY() * Vector<T>{ 0, n, y1   + y0, 0       },
			v.spreadZ() * Vector<T>{ 0, 0, z0  + z1,    n * z1 },
			Vector<T>::negativeZ
		};
	}

	template<class T>
	inline Matrix<T> lookAtMatrix(const Vector<T> & position, const Vector<T> & center, const Vector<T> & up)
	{
		Vector<T> f = (position - center).normalize();
		Vector<T> s = (up.cross(f)).normalize();
		Vector<T> u = (f.cross(s)).normalize();

		return {
			s.template blend<0, 0, 0, 1>(-s.dot(position)),
			u.template blend<0, 0, 0, 1>(-u.dot(position)),
			f.template blend<0, 0, 0, 1>(-f.dot(position))
		};
	}
}

//---------------------------------------------------------------------------
#endif

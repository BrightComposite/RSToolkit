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

			T m[16];
			T a[4][4];

			Vector<T> v[4];
			Row rows[4];
			Data data[4];
			Pair pairs[8];
		};

		Matrix() : x {Vector<T>::positiveX}, y {Vector<T>::positiveY}, z {Vector<T>::positiveZ}, w {Vector<T>::positiveW} {}
		Matrix(const Matrix & matrix) : x {matrix.x}, y {matrix.y}, z {matrix.z}, w {matrix.w} {}
		Matrix(const Matrix2x2<T> & m) : x {m.m[0x0], m.m[0x1], 0, 0}, y {m.m[0x2], m.m[0x3], 0, 0}, z {Vector<T>::positiveZ}, w {Vector<T>::positiveW} {}
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
			Memory<T>::move(m, matrix.m, 16);
			return *this;
		}

		Matrix & clear()
        {
			Memory<T>::fill(m, 0, 16);
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

		Matrix & Matrix::operator += (const Matrix & matrix)
		{
			rows[0] += matrix[0];
			rows[1] += matrix[1];
			rows[2] += matrix[2];
			rows[3] += matrix[3];

			return *this;
		}

		Matrix & Matrix::operator -= (const Matrix & matrix)
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
			return a[row][column];
		}

		T & operator () (size_t row, size_t column)
		{
			return a[row][column];
		}

		operator array_t<T, 16> & ()
		{
			return m;
		}

		operator const array_t<T, 16> & () const
		{
			return m;
		}

		operator array_t<Vector<T>, 4> & ()
		{
			return rows;
		}

		operator const array_t<Vector<T>, 4> & () const
		{
			return rows;
		}

		operator array_t<array_t<T, 4>, 4> & ()
		{
			return a;
		}

		operator const array_t<array_t<T, 4>, 4> & () const
		{
			return a;
		}

		operator array_t<Data, 4> & ()
		{
			return data;
		}

		operator const array_t<Data, 4> & () const
		{
			return data;
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

		tmp1[0] = rows[2].shuffle<1, 0, 0, 0>();
		tmp1[1] = rows[2].shuffle<2, 2, 1, 1>();
		tmp1[2] = rows[2].shuffle<3, 3, 3, 2>();
		tmp1[3] = rows[3].shuffle<1, 0, 0, 0>();
		tmp2[0] = rows[3].shuffle<2, 2, 1, 1>();
		tmp2[1] = rows[3].shuffle<3, 3, 3, 2>();

		tmp2[2] = tmp1[1] * tmp2[1] - tmp1[2] * tmp2[0];
		tmp2[3] = tmp1[0] * tmp2[1] - tmp1[2] * tmp1[3];
		tmp2[1] = tmp1[0] * tmp2[0] - tmp1[1] * tmp1[3];

		tmp1[0] = rows[1].shuffle<1, 0, 0, 0>();
		tmp1[1] = rows[1].shuffle<2, 2, 1, 1>();
		tmp1[2] = rows[1].shuffle<3, 3, 3, 2>();

		return rows[0].negate<0, 1, 0, 1>().dot(tmp1[0] * tmp2[2] - tmp1[1] * tmp2[3] + tmp1[2] * tmp2[1]);
	}

	template<class T>
	inline void Matrix<T>::getInverse(Matrix<T> & m) const
	{
		Vector<T> r[4], temp;

		temp = v[0].shuffle<0, 1, 0, 1>(v[1]);
		r[1] = v[2].shuffle<0, 1, 0, 1>(v[3]);
		r[0] = temp.shuffle<0, 2, 0, 2>(r[1]);
		r[1] = r[1].shuffle<1, 3, 1, 3>(temp);
		temp = v[0].shuffle<2, 3, 2, 3>(v[1]);
		r[3] = v[2].shuffle<2, 3, 2, 3>(v[3]);
		r[2] = temp.shuffle<0, 2, 0, 2>(r[3]);
		r[3] = r[3].shuffle<1, 3, 1, 3>(temp);

		temp = r[2] * r[3];
		temp = temp.shuffle<1, 0, 3, 2>() - temp.shuffle<3, 2, 1, 0>();
		m[0] = -r[1] * temp;
		m[1] = -r[0] * temp;
		m[1] = m[1].shuffle<2, 3, 0, 1>();

		temp = r[1] * r[2];
		temp = temp.shuffle<1, 0, 3, 2>() - temp.shuffle<3, 2, 1, 0>();
		m[0] += r[3] * temp;
		m[3] = r[0] * temp;
		m[3] = m[3].shuffle<2, 3, 0, 1>();

		temp = r[1].shuffle<2, 3, 0, 1>() * r[3];
		temp = temp.shuffle<1, 0, 3, 2>() - temp.shuffle<3, 2, 1, 0>();
		r[2] = r[2].shuffle<2, 3, 0, 1>();
		m[0] += r[2] * temp;
		m[2] = r[0] * temp;
		m[2] = m[2].shuffle<2, 3, 0, 1>();

		temp = r[0] * r[1];
		temp = temp.shuffle<1, 0, 3, 2>() - temp.shuffle<3, 2, 1, 0>();
		m[2] -= r[3] * temp;
		m[3] += r[2] * temp;

		temp = r[0] * r[3];
		temp = temp.shuffle<1, 0, 3, 2>() - temp.shuffle<3, 2, 1, 0>();
		m[1] -= r[2] * temp;
		m[2] += r[1] * temp;

		temp = r[0] * r[2];
		temp = temp.shuffle<1, 0, 3, 2>() - temp.shuffle<3, 2, 1, 0>();
		m[1] += r[3] * temp;
		m[3] -= r[1] * temp;

		auto det = r[0].dot(m[0]).invert();
		m[0] *= det; m[1] *= det; m[2] *= det; m[3] *= det;
	}

	template<class T>
	inline void Matrix<T>::getTransposition(Matrix<T> & mat) const
	{
		Matrix tmp {
			rows[0].shuffle<0, 1, 0, 1>(rows[1]),
			rows[0].shuffle<2, 3, 2, 3>(rows[1]),
			rows[2].shuffle<0, 1, 0, 1>(rows[3]),
			rows[2].shuffle<2, 3, 2, 3>(rows[3])
		};

		mat[0] = tmp[0].shuffle<0, 2, 0, 2>(tmp[2]);
		mat[1] = tmp[0].shuffle<1, 3, 1, 3>(tmp[2]);
		mat[2] = tmp[1].shuffle<0, 2, 0, 2>(tmp[3]);
		mat[3] = tmp[1].shuffle<1, 3, 1, 3>(tmp[3]);
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
			v.shuffle<3, 1, 2, 3>(),	// 0,  c, -s
			v.shuffle<3, 0, 1, 3>()		// 0,  s,  c
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrixY(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			v.shuffle<1, 3, 0, 3>(),	// c,  0,  s
			Vector<T>::positiveY,		// 0,  1,  0
			v.shuffle<2, 3, 1, 3>()		//-s,  0,  c
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrixZ(T angle)
	{
		auto v = VectorMath<T>::trigon(angle);

		return {
			v.shuffle<1, 2, 3, 3>(),	// c, -s,  0
			v.shuffle<0, 1, 3, 3>(),	// s,  c,  0
			Vector<T>::positiveZ
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrix(const Vector<T> & v, T angle)
	{
		auto sc = VectorMath<T>::trigon(angle);
		auto cc = sc.shuffle<1, 1, 1, 3>();
		auto nc = (Vector<T>::oneXYZ - cc) * v;
		auto ss = sc.shuffle<0, 0, 0, 3>() * v;

		sc = nc.shuffle<1, 0, 0, 3>() * v.shuffle<2, 2, 1, 3>();
		cc += v * nc;

		nc = sc + ss;
		sc -= ss;

		return {
			cc.shuffle<0, 0, 2, 2>(nc).shuffle<0, 2, 1, 3>(sc), // cc.x, nc.z, sc.y, 0,
			sc.shuffle<2, 2, 1, 1>(cc).shuffle<0, 2, 0, 3>(nc), // sc.z, cc.y, nc.x, 0,
			nc.shuffle<1, 1, 0, 0>(sc).shuffle<0, 2, 2, 3>(cc)  // nc.y, sc.x, cc.z, 0,
		};
	}

	template<class T>
	inline Matrix<T> Matrix<T>::rotationMatrix(const Vector<T> & euler)
	{
		Vector<T> sine, cosine;
		VectorMath<T>::sincos(euler, sine, cosine);

		Vector<T> x = sine.shuffle<0, 0, 0, 0>(cosine).blend<0, 1, 1, 0>(Vector<T>::positiveY);	// [ sx, 1, 0, cx ]
		Vector<T> y = sine.shuffle<1, 1, 1, 1>(cosine).blend<0, 1, 1, 0>(Vector<T>::positiveY);	// [ sy, 1, 0, cy ]
		Vector<T> z = sine.shuffle<2, 2, 2, 2>(cosine).blend<0, 1, 1, 0>(Vector<T>::positiveY).shuffle<3, 0, 1, 2>(); // [ cz, sz, 1, 0 ]

		Vector<T> yz = y.shuffle<0, 0, 3, 2>().negateZ() * z;	// [ sy * cz, sy * sz, -cy, 0 ]
		Vector<T> mz = z.shuffle<1, 0, 2, 3>().negateX();		// [ -sz, cz, 1, 0 ]

		return {
			y.shuffle<3, 3, 0, 0>() * z,						//  cy * cz,			   | cy * sz,				| sy,	  | 0
			x.shuffle<3, 3, 2, 2>() * mz + x.spreadX() * yz,	// -cx * sz + sx * sy * cz | cx * cz + sx * sy * sz |-sx * cy |	0
			x.shuffle<0, 0, 2, 2>() * mz - x.spreadW() * yz		// -sx * sz - cx * sy * cz | sx * cz - cx * sy * sz | cx * cy | 0
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
			Vector<T>::positiveX.blend<0, 0, 0, 1>(t.spreadX()),
			Vector<T>::positiveY.blend<0, 0, 0, 1>(t.spreadY()),
			Vector<T>::positiveZ.blend<0, 0, 0, 1>(t.spreadZ())
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
    inline Matrix<T> orthoMatrix(T left, T right, T bottom, T top, T near, T far)
	{
		static const Vector<T> q { 2,  2, -2, 0 };

		Vector<T> max { right, top,    far,   1 };
		Vector<T> min { left,  bottom, near, -1 };

		Vector<T> k = (min - max).inverse();
		Vector<T> t = (max + min) * k;
		Vector<T> s = q * k;

		return {
			s.shuffle<0, 3, 3, 0>(t),
			s.shuffle<3, 1, 3, 1>(t),
			t.shuffle<3, 3, 3, 2>().blend<0, 0, 1, 0>(s),
		};
	}

	template<class T>
	inline Matrix<T> perspectiveMatrix(T fov, T aspect, T near, T far)
	{
		const T f = 1 / std::tan(Math<T>::dtor(fov) / 2);
		const T delta = near - far;
		const Vector<T> v = {f / aspect, f, (far + near) / delta, (2 * far * near) / delta};

		return {
			v.maskX(),
			v.maskY(),
			v.mask<0, 0, 1, 1>(),
			Vector<T>::negativeZ
		};
	}

	template<class T>
	inline Matrix<T> frustumMatrix(T left, T right, T bottom, T top, T near, T far)
	{
		const T n = 2 * near;
		const Vector<T> v {1 / (right - left), 1 / (top - bottom), 1 / (near - far), 0};

		return {
			v.spreadX() * Vector<T>{ n, 0, right + left,   0       },
			v.spreadY() * Vector<T>{ 0, n, top   + bottom, 0       },
			v.spreadZ() * Vector<T>{ 0, 0, near  + far,    n * far },
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
			s.blend<0, 0, 0, 1>(-s.dot(position)),
			u.blend<0, 0, 0, 1>(-u.dot(position)),
			f.blend<0, 0, 0, 1>(-f.dot(position))
		};
	}
}

//---------------------------------------------------------------------------
#endif

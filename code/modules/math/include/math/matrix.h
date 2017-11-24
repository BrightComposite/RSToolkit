//---------------------------------------------------------------------------

#pragma once

#ifndef MATRIX_H
#define MATRIX_H

//---------------------------------------------------------------------------

#include "vector.h"

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<class T>
		struct alignas(sizeof(T) * 4) matrix : aligned_alloc
		{
			typedef intrin_data<T, 4> Data;
			typedef intrin_data<T, 2> Pair;
			typedef Intrinsic<T, 4> Intrin;
			typedef vector <T> Row;
			
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
				
				array<vector < T>, 4> v;
				array<Row, 4> rows;
				array<Data, 4> data;
				array<Pair, 8> pairs;
			};
			
			member_cast(m, array<T, 16>);
			member_cast(rows, array<Row, 4>);
			member_cast(data, array<Data, 4>);
			member_cast(a, array<array<T, 4>, 4>);
			
			matrix() : x(vector<T>::positiveX), y(vector<T>::positiveY), z(vector<T>::positiveZ), w(vector<T>::positiveW) {}
			
			matrix(const matrix & matrix) : x{matrix.x}, y{matrix.y}, z{matrix.z}, w{matrix.w} {}
			
			matrix(matrix && matrix) : m{move(matrix.m)} {}
			
			matrix(const Row (& v)[4]) : x{v[0]}, y{v[1]}, z{v[2]}, w{v[3]} {}
			
			matrix(const Row & x, const Row & y, const Row & z, const Row & w) : x{x}, y{y}, z{z}, w{w} {}
			
			matrix(const Row & x, const Row & y, const Row & z) : x{x}, y{y}, z{z}, w{vector<T>::positiveW} {}
			
			matrix(const T(& m)[16]) :
				x(m[0x0], m[0x1], m[0x2], m[0x3]),
				y(m[0x4], m[0x5], m[0x6], m[0x7]),
				z(m[0x8], m[0x9], m[0xA], m[0xB]),
				w(m[0xC], m[0xD], m[0xE], m[0xF]) {}
			
			matrix(const T(& rows)[4][4]) : x(rows[0]), y(rows[1]), z(rows[2]), w(rows[3]) {}
			
			matrix(
				T xx, T xy, T xz, T xw,
				T yx, T yy, T yz, T yw,
				T zx, T zy, T zz, T zw,
				T wx, T wy, T wz, T ww
			) :
				x(xx, xy, xz, xw),
				y(yx, yy, yz, yw),
				z(zx, zy, zz, zw),
				w(wx, wy, wz, ww) {}
			
			matrix & operator =(const matrix & matrix) {
				memory<T>::move(m, matrix.m);
				return *this;
			}
			
			matrix & operator =(matrix && matrix) {
				m.swap(matrix.m);
				return *this;
			}
			
			matrix & clear() {
				m.fill(0);
				return *this;
			}
			
			matrix & identity() {
				x = vector<T>::positiveX;
				y = vector<T>::positiveY;
				z = vector<T>::positiveZ;
				w = vector<T>::positiveW;
				
				return *this;
			}
			
			matrix & identity(T a) {
				vector<T> v(a);
				
				x = v.maskX();
				y = v.maskY();
				z = v.maskZ();
				w = v.maskW();
				
				return *this;
			}
			
			matrix & operator +=(const matrix & matrix) {
				rows[0] += matrix[0];
				rows[1] += matrix[1];
				rows[2] += matrix[2];
				rows[3] += matrix[3];
				
				return *this;
			}
			
			matrix & operator -=(const matrix & matrix) {
				rows[0] -= matrix[0];
				rows[1] -= matrix[1];
				rows[2] -= matrix[2];
				rows[3] -= matrix[3];
				
				return *this;
			}
			
			matrix & operator *=(const matrix & matrix) {
				return *this = operator *(*this, matrix);
			}
			
			matrix & operator *=(T value) {
				rows[0] *= value;
				rows[1] *= value;
				rows[2] *= value;
				rows[3] *= value;
				
				return *this;
			}
			
			matrix & operator /=(T value) {
				rows[0] /= value;
				rows[1] /= value;
				rows[2] /= value;
				rows[3] /= value;
				
				return *this;
			}
			
			Row & operator [](int row) & {
				return rows[row];
			}
			
			const Row & operator [](int row) const & {
				return rows[row];
			}
			
			Row && operator [](int row) && {
				return move(rows[row]);
			}
			
			T & operator ()(int i) {
				return m[i];
			}
			
			T operator ()(int i) const {
				return m[i];
			}
			
			T & operator ()(int row, int column) {
				return a[row][column];
			}
			
			T operator ()(int row, int column) const {
				return a[row][column];
			}
			
			inline T norm() const;
			inline vector <T> determinant() const;
			
			matrix & invert() & {
				getInverse(*this);
				return *this;
			}
			
			matrix && invert() && {
				getInverse(*this);
				return forward<matrix>(*this);
			}
			
			matrix & transpose() & {
				getTransposition(*this);
				return *this;
			}
			
			matrix && transpose() && {
				getTransposition(*this);
				return forward<matrix>(*this);
			}
			
			matrix inverse() const {
				matrix mat;
				getInverse(mat);
				return mat;
			}
			
			matrix transposition() const {
				matrix mat;
				getTransposition(mat);
				return mat;
			}
			
			inline void getInverse(matrix & mat) const;
			inline void getTransposition(matrix & mat) const;
			
			inline vector <T> transformPoint(const vector <T> & b) const;
			inline vector <T> transformDirection(const vector <T> & b) const;
			
			inline matrix & apply(const matrix<T> & m);
			
			inline matrix & scale(const vector <T> & s);
			inline matrix & scale(const T s);
			
			inline matrix & rotate_x(T);
			inline matrix & rotate_y(T);
			inline matrix & rotate_z(T);
			inline matrix & rotate(const vector <T> & axis, T angle);
			inline matrix & rotate(const vector <T> & rotation);
			
			inline matrix & mirrorX();
			inline matrix & mirrorY();
			inline matrix & mirrorZ();
			
			inline matrix & translate(const vector <T> & v);
			
			static inline matrix rotationX(T angle);
			static inline matrix rotationY(T angle);
			static inline matrix rotationZ(T angle);
			static inline matrix rotation(const vector <T> & v, T angle);
			static inline matrix rotation(const vector <T> & r);
			static inline matrix scaling(const vector <T> & s);
			static inline matrix translation(const vector <T> & t);
			
			static inline matrix ortho(T x0, T x1, T y0, T y1, T z0, T z1);
			static inline matrix orthot(T x0, T x1, T y0, T y1, T z0, T z1);
			static inline matrix perspective(T fov, T aspect, T z0, T z1);
			static inline matrix perspectivet(T fov, T aspect, T z0, T z1);
			static inline matrix frustum(T x0, T x1, T y0, T y1, T z0, T z1);
			static inline matrix lookAt(const vector <T> & position, const vector <T> & center, const vector <T> & up);
			static inline matrix lookTo(const vector <T> & position, const vector <T> & direction, const vector <T> & up);
		};
		
		using float_matrix = matrix<float>;
		using double_matrix = matrix<double>;
		using floatm = float_matrix;
		using doublem = double_matrix;
		
		template<class T>
		using aligned_matrix = aligned<matrix<T>>;
		
		using fmat = aligned_matrix<float>;
		using dmat = aligned_matrix<double>;
		
		template<class T>
		inline matrix<T> operator +(const matrix<T> & m1, const matrix<T> & m2) {
			return matrix<T>(m1) += m2;
		}
		
		template<class T>
		inline matrix<T> operator -(const matrix<T> & m1, const matrix<T> & m2) {
			return matrix<T>(m1) -= m2;
		}
		
		template<class T>
		inline matrix<T> operator *(const matrix<T> & a, const matrix<T> & b) {
			return {
				a[0] * b[0].spreadX() + a[1] * b[0].spreadY() + a[2] * b[0].spreadZ() + a[3] * b[0].spreadW(),
				a[0] * b[1].spreadX() + a[1] * b[1].spreadY() + a[2] * b[1].spreadZ() + a[3] * b[1].spreadW(),
				a[0] * b[2].spreadX() + a[1] * b[2].spreadY() + a[2] * b[2].spreadZ() + a[3] * b[2].spreadW(),
				a[0] * b[3].spreadX() + a[1] * b[3].spreadY() + a[2] * b[3].spreadZ() + a[3] * b[3].spreadW()
			};
		}
		
		template<class T>
		inline matrix<T> operator *(const matrix<T> & m, T value) {
			return matrix<T>(m) *= value;
		}
		
		template<class T>
		inline matrix<T> operator *(T value, const matrix<T> & m) {
			return matrix<T>(m) *= value;
		}
		
		template<class T>
		inline vector <T> operator *(const matrix<T> & m, const vector <T> & vec) {
			return m.transformPoint(vec);
		}
		
		template<class T>
		inline T matrix<T>::norm() const {
			return std::max({v[0].max(), v[1].max(), v[2].max(), v[3].max()});
		}
		
		template<class T>
		inline vector <T> matrix<T>::determinant() const {
			matrix tmp1, tmp2;
			
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
		inline void matrix<T>::getInverse(matrix<T> & m) const {
			vector<T> r[4], temp;
			
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
			m[0] *= det;
			m[1] *= det;
			m[2] *= det;
			m[3] *= det;
		}
		
		template<class T>
		inline void matrix<T>::getTransposition(matrix<T> & mat) const {
			auto v0 = rows[0].template shuffle<0, 1, 0, 1>(rows[1]);
			auto v1 = rows[0].template shuffle<2, 3, 2, 3>(rows[1]);
			auto v2 = rows[2].template shuffle<0, 1, 0, 1>(rows[3]);
			auto v3 = rows[2].template shuffle<2, 3, 2, 3>(rows[3]);
			
			matrix tmp{v0, v1, v2, v3};
			
			mat[0] = tmp[0].template shuffle<0, 2, 0, 2>(tmp[2]);
			mat[1] = tmp[0].template shuffle<1, 3, 1, 3>(tmp[2]);
			mat[2] = tmp[1].template shuffle<0, 2, 0, 2>(tmp[3]);
			mat[3] = tmp[1].template shuffle<1, 3, 1, 3>(tmp[3]);
		}
		
		template<class T>
		inline vector <T> matrix<T>::transformPoint(const vector <T> & v) const {
			return {
				sum(rows[0] * v) + m03,
				sum(rows[1] * v) + m13,
				sum(rows[2] * v) + m23
			};
		}
		
		template<class T>
		inline vector <T> matrix<T>::transformDirection(const vector <T> & v) const {
			return {
				sum(rows[0] * v),
				sum(rows[1] * v),
				sum(rows[2] * v)
			};
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::apply(const matrix<T> & m) {
			return *this *= m;
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::scale(const vector <T> & s) {
			return apply(scaling(s));
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::scale(const T s) {
			return apply(scaling({s}));
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::rotate_x(T angle) {
			return apply(rotationX(angle));
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::rotate_y(T angle) {
			return apply(rotationY(angle));
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::rotate_z(T angle) {
			return apply(rotationZ(angle));
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::rotate(const vector <T> & axis, T angle) {
			return apply(rotation(axis, angle));
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::rotate(const vector <T> & v) {
			return apply(rotation(v));
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::mirrorX() {
			return apply({
				vector<T>::negativeX,
				vector<T>::positiveY,
				vector<T>::positiveZ
			});
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::mirrorY() {
			return apply({
				vector<T>::positiveX,
				vector<T>::negativeY,
				vector<T>::positiveZ
			});
		}
		
		template<class T>
		inline matrix<T> & matrix<T>::mirrorZ() {
			return apply({
				vector<T>::positiveX,
				vector<T>::positiveY,
				vector<T>::negativeZ
			});
		}
		
		template<class T>
		matrix<T> & matrix<T>::translate(const vector <T> & v) {
			return apply(translation(v));
		}
		
		template<class T>
		inline matrix<T> matrix<T>::rotationX(T angle) {
			auto v = trigon(angle);
			
			return {
				vector<T>::positiveX,        // 1,  0,  0
				v.template shuffle<3, 1, 2, 3>(),    // 0,  c, -s
				v.template shuffle<3, 0, 1, 3>()        // 0,  s,  c
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::rotationY(T angle) {
			auto v = trigon(angle);
			
			return {
				v.template shuffle<1, 3, 0, 3>(),    // c,  0,  s
				vector<T>::positiveY,        // 0,  1,  0
				v.template shuffle<2, 3, 1, 3>()        //-s,  0,  c
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::rotationZ(T angle) {
			auto v = trigon(angle);
			
			return {
				v.template shuffle<1, 2, 3, 3>(),    // c, -s,  0
				v.template shuffle<0, 1, 3, 3>(),    // s,  c,  0
				vector<T>::positiveZ
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::rotation(const vector <T> & v, T angle) {
			auto sc = trigon(angle);
			auto cc = sc.template shuffle<1, 1, 1, 3>();
			auto nc = (vector<T>::oneXYZ - cc) * v;
			auto ss = sc.template shuffle<0, 0, 0, 3>() * v;
			
			sc = nc.template shuffle<1, 0, 0, 3>() * v.template shuffle<2, 2, 1, 3>();
			cc += v * nc;
			
			nc = sc + ss;
			sc -= ss;
			
			return {
				cc.template shuffle<0, 0, 2, 2>(nc).
					template shuffle<0, 2, 1, 3>(sc), // cc.x, nc.z, sc.y, 0,
				sc.template shuffle<2, 2, 1, 1>(cc).
					template shuffle<0, 2, 0, 3>(nc), // sc.z, cc.y, nc.x, 0,
				nc.template shuffle<1, 1, 0, 0>(sc).
					template shuffle<0, 2, 2, 3>(cc)  // nc.y, sc.x, cc.z, 0,
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::rotation(const vector <T> & euler) {
			vector<T> sine, cosine;
			sincos(euler, sine, cosine);
			
			vector<T> x = sine.template shuffle<0, 0, 0, 0>(cosine).
				template blend<0, 1, 1, 0>(vector<T>::positiveY);                                    // [ sx, 1, 0, cx ]
			vector<T> y = sine.template shuffle<1, 1, 1, 1>(cosine).
				template blend<0, 1, 1, 0>(vector<T>::positiveY);                                    // [ sy, 1, 0, cy ]
			vector<T> z = sine.template shuffle<2, 2, 2, 2>(cosine).
				template blend<0, 1, 1, 0>(vector<T>::positiveY).
				template shuffle<3, 0, 1, 2>();    // [ cz, sz, 1, 0 ]
			
			vector<T> yz = y.template shuffle<0, 0, 3, 2>().negateZ() * z;    // [ sy * cz, sy * sz, -cy, 0 ]
			vector<T> mz = z.template shuffle<1, 0, 2, 3>().negateX();        // [ -sz, cz, 1, 0 ]
			
			return {
				y.template shuffle<3, 3, 0, 0>() * z,                        //  cy * cz,			   | cy * sz,				| sy,	  | 0
				x.template shuffle<3, 3, 2, 2>() * mz + x.spreadX() * yz,    // -cx * sz + sx * sy * cz | cx * cz + sx * sy * sz |-sx * cy |	0
				x.template shuffle<0, 0, 2, 2>() * mz - x.spreadW() * yz    // -sx * sz - cx * sy * cz | sx * cz - cx * sy * sz | cx * cy | 0
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::scaling(const vector <T> & s) {
			return {
				s.maskX(), s.maskY(), s.maskZ()
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::translation(const vector <T> & t) {
			return {
				vector<T>::positiveX.template blend<0, 0, 0, 1>(t.spreadX()),
				vector<T>::positiveY.template blend<0, 0, 0, 1>(t.spreadY()),
				vector<T>::positiveZ.template blend<0, 0, 0, 1>(t.spreadZ())
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::ortho(T x0, T x1, T y0, T y1, T z0, T z1) {
			const vector<T> max{x1, y1, z1, 1};
			const vector<T> min{x0, y0, z0, 0};
			
			vector<T> d = (max - min).inverse();
			const vector<T> t = -(max + min) * d;
			d *= vector<T>::two;
			
			return {
				d.maskX().template blend<0, 0, 0, 1>(t.spreadX()),    // 2/w |  0  |  0  | -(x0+x1)/w
				d.maskY().template blend<0, 0, 0, 1>(t.spreadY()),    //  0  | 2/h |  0  | -(y0+y1)/h
				d.maskZ().template blend<0, 0, 0, 1>(t.spreadZ()),    //  0  |  0  | 2/l | -(z0+z1)/l
				vector<T>::positiveW                                //  0  |  0  |  0  |    1
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::orthot(T x0, T x1, T y0, T y1, T z0, T z1) {
			const vector<T> max{x1, y1, z1, 1};
			const vector<T> min{x0, y0, z0, 0};
			
			vector<T> d = (max - min).inverse();
			const vector<T> t = -(max + min) * d;
			d *= vector<T>::two;
			
			return {
				d.maskX(),                                            //    2/w    |     0     |     0    | 0
				d.maskY(),                                            //     0     |    2/h    |     0    | 0
				d.maskZ(),                                            //     0     |     0     |    2/l   | 0
				t.template blend<0, 0, 0, 1>(vector<T>::positiveW)    //-(x0+x1)/w |-(y0+y1)/h |-(z0+z1)/l| 1
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::perspective(T fov, T aspect, T z0, T z1) {
			const T f = 1 / std::tan(radians(fov) / 2);
			const T z = z1 / (z1 - z0);
			const vector<T> v = {f * aspect, f, z, -z0 * z};
			
			return {
				v.maskX(),                        // f/a | 0 | 0 |  0
				v.maskY(),                        //  0  | f | 0 |  0
				v.template mask<0, 0, 1, 1>(),    //  0  | 0 | z |-z0*z
				vector<T>::positiveZ            //  0  | 0 | 1 |  0
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::perspectivet(T fov, T aspect, T z0, T z1) {
			const T f = 1 / std::tan(radians(fov) / 2);
			const T z = z1 / (z1 - z0);
			const vector<T> v = {f * aspect, f, z, -z0 * z};
			
			return {
				v.maskX(),                                            // f*a | 0 |  0  | 0
				v.maskY(),                                            //  0  | f |  0  | 0
				v.template blend<1, 1, 0, 1>(vector<T>::positiveW),    //  0  | 0 |  z  | 1
				v.spreadW().maskZ(),                                //  0  | 0 |-z0*z| 0
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::frustum(T x0, T x1, T y0, T y1, T z0, T z1) {
			const T n = 2 * z1;
			const vector<T> v{1 / (x1 - x0), 1 / (y1 - y0), 1 / (z1 - z0), 0};
			
			return {
				v.spreadX() * vector<T>{n, 0, x1 + x0, 0},
				v.spreadY() * vector<T>{0, n, y1 + y0, 0},
				v.spreadZ() * vector<T>{0, 0, z1 + z0, n * z0},
				vector<T>::negativeZ
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::lookAt(const vector <T> & position, const vector <T> & center, const vector <T> & up) {
			const vector<T> f = (center - position).normalize();
			const vector<T> s = (up.cross(f)).normalize();
			const vector<T> u = (f.cross(s)).normalize();
			
			return {
				s.template blend<0, 0, 0, 1>(-s.dot(position)),
				u.template blend<0, 0, 0, 1>(-u.dot(position)),
				f.template blend<0, 0, 0, 1>(-f.dot(position))
			};
		}
		
		template<class T>
		inline matrix<T> matrix<T>::lookTo(const vector <T> & position, const vector <T> & direction, const vector <T> & up) {
			const vector<T> f = direction;
			const vector<T> s = (up.cross(f)).normalize();
			const vector<T> u = (f.cross(s)).normalize();
			
			return {
				s.template blend<0, 0, 0, 1>(-s.dot(position)),
				u.template blend<0, 0, 0, 1>(-u.dot(position)),
				f.template blend<0, 0, 0, 1>(-f.dot(position))
			};
		}
	}
	
	template<class T>
	inline void print(String & s, const math::matrix<T> & mat) {
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

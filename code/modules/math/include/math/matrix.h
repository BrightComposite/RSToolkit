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
		template <typename T, typename Implementation = Intrinsic<T, 4>, typename VectorStorage = DefaultVectorStorage <T, Implementation>>
		struct alignas(sizeof(T) * 4) matrix : aligned_alloc
		{
			typedef intrin_data<T, 4> Data;
			typedef intrin_data<T, 2> Pair;
			using vector_type = vector<T, Implementation, VectorStorage>;
			using row_type = vector_type;
			
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
					row_type x, y, z, w;
				};
				
				array<T, 16> m;
				array<array<T, 4>, 4> a;
				
				array<vector<T>, 4> v;
				array<row_type, 4> rows;
				array<Data, 4> data;
				array<Pair, 8> pairs;
			};
			
			member_cast(m, array<T, 16>);
			member_cast(rows, array<row_type, 4>);
			member_cast(data, array<Data, 4>);
			member_cast(a, array<array<T, 4>, 4>);
			
			matrix() : x(vector_constants<T>::positiveX), y(vector_constants<T>::positiveY), z(vector_constants<T>::positiveZ), w(vector_constants<T>::positiveW) {}
			
			matrix(const matrix & matrix) : x{matrix.x}, y{matrix.y}, z{matrix.z}, w{matrix.w} {}
			
			matrix(matrix && matrix) : m{move(matrix.m)} {}
			
			matrix(const row_type (& v)[4]) : x{v[0]}, y{v[1]}, z{v[2]}, w{v[3]} {}
			
			matrix(const row_type & x, const row_type & y, const row_type & z, const row_type & w) : x{x}, y{y}, z{z}, w{w} {}
			
			matrix(const row_type & x, const row_type & y, const row_type & z) : x{x}, y{y}, z{z}, w{vector_constants<T>::positiveW} {}
			
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
				x = vector_constants<T>::positiveX;
				y = vector_constants<T>::positiveY;
				z = vector_constants<T>::positiveZ;
				w = vector_constants<T>::positiveW;
				
				return *this;
			}
			
			matrix & identity(T a) {
				vector<T> v(a);
				
				x = v.mask_x();
				y = v.mask_y();
				z = v.mask_z();
				w = v.mask_w();
				
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
			
			row_type & operator [](int row) & {
				return rows[row];
			}
			
			const row_type & operator [](int row) const & {
				return rows[row];
			}
			
			row_type && operator [](int row) && {
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
			inline vector_type determinant() const;
			
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
			
			inline vector_type transformPoint(const vector_type & b) const;
			inline vector_type transformDirection(const vector_type & b) const;
			
			inline matrix & apply(const matrix & m);
			
			inline matrix & scale(const vector_type & s);
			inline matrix & scale(const T s);
			
			inline matrix & rotateX(T);
			inline matrix & rotateY(T);
			inline matrix & rotateZ(T);
			inline matrix & rotate(const vector_type & axis, T angle);
			inline matrix & rotate(const vector_type & rotation);
			
			inline matrix & mirrorX();
			inline matrix & mirrorY();
			inline matrix & mirrorZ();
			
			inline matrix & translate(const vector_type & v);
			
			static inline matrix rotationX(T angle);
			static inline matrix rotationY(T angle);
			static inline matrix rotationZ(T angle);
			static inline matrix rotation(const vector_type & v, T angle);
			static inline matrix rotation(const vector_type & r);
			static inline matrix scaling(const vector_type & s);
			static inline matrix translation(const vector_type & t);
			
			static inline matrix ortho(T x0, T x1, T y0, T y1, T z0, T z1);
			static inline matrix orthot(T x0, T x1, T y0, T y1, T z0, T z1);
			static inline matrix perspective(T fov, T aspect, T z0, T z1);
			static inline matrix perspectivet(T fov, T aspect, T z0, T z1);
			static inline matrix frustum(T x0, T x1, T y0, T y1, T z0, T z1);
			static inline matrix lookAt(const vector_type & position, const vector_type & center, const vector_type & up);
			static inline matrix lookTo(const vector_type & position, const vector_type & direction, const vector_type & up);
		};
		
		using float_matrix = matrix<float>;
		using double_matrix = matrix<double>;
		using floatm = float_matrix;
		using doublem = double_matrix;
		
		using fmat = matrix<float>;
		using dmat = matrix<double>;
		
		template<class T, typename Implementation = Intrinsic<T, 4>>
		using aligned_matrix = matrix<T, Implementation, AlignedVectorStorage<T, Implementation>>;
		
		template <class T, class I, class S>
		inline matrix<T, I, S> operator +(const matrix<T, I, S> & m1, const matrix<T, I, S> & m2) {
			return matrix<T, I, S>(m1) += m2;
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> operator -(const matrix<T, I, S> & m1, const matrix<T, I, S> & m2) {
			return matrix<T, I, S>(m1) -= m2;
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> operator *(const matrix<T, I, S> & a, const matrix<T, I, S> & b) {
			return {
				a[0] * b[0].spread_x() + a[1] * b[0].spread_y() + a[2] * b[0].spread_z() + a[3] * b[0].spread_w(),
				a[0] * b[1].spread_x() + a[1] * b[1].spread_y() + a[2] * b[1].spread_z() + a[3] * b[1].spread_w(),
				a[0] * b[2].spread_x() + a[1] * b[2].spread_y() + a[2] * b[2].spread_z() + a[3] * b[2].spread_w(),
				a[0] * b[3].spread_x() + a[1] * b[3].spread_y() + a[2] * b[3].spread_z() + a[3] * b[3].spread_w()
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> operator *(const matrix<T, I, S> & m, T value) {
			return matrix<T, I, S>(m) *= value;
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> operator *(T value, const matrix<T, I, S> & m) {
			return matrix<T, I, S>(m) *= value;
		}
		
		template <class T, class I, class S>
		inline vector<T, I, S> operator *(const matrix<T, I, S> & m, const vector<T, I, S> & vec) {
			return m.transformPoint(vec);
		}
		
		template <class T, class I, class S>
		inline T matrix<T, I, S>::norm() const {
			return std::max({v[0].max(), v[1].max(), v[2].max(), v[3].max()});
		}
		
		template <class T, class I, class S>
		inline vector<T, I, S> matrix<T, I, S>::determinant() const {
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
		
		template <class T, class I, class S>
		inline void matrix<T, I, S>::getInverse(matrix<T, I, S> & m) const {
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
		
		template <class T, class I, class S>
		inline void matrix<T, I, S>::getTransposition(matrix<T, I, S> & mat) const {
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
		
		template <class T, class I, class S>
		inline vector<T, I, S> matrix<T, I, S>::transformPoint(const vector<T, I, S> & v) const {
			return {
				sum(rows[0] * v) + m03,
				sum(rows[1] * v) + m13,
				sum(rows[2] * v) + m23
			};
		}
		
		template <class T, class I, class S>
		inline vector<T, I, S> matrix<T, I, S>::transformDirection(const vector<T, I, S> & v) const {
			return {
				sum(rows[0] * v),
				sum(rows[1] * v),
				sum(rows[2] * v)
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::apply(const matrix<T, I, S> & m) {
			return *this *= m;
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::scale(const vector<T, I, S> & s) {
			return apply(scaling(s));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::scale(const T s) {
			return apply(scaling({s}));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::rotateX(T angle) {
			return apply(rotationX(angle));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::rotateY(T angle) {
			return apply(rotationY(angle));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::rotateZ(T angle) {
			return apply(rotationZ(angle));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::rotate(const vector<T, I, S> & axis, T angle) {
			return apply(rotation(axis, angle));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::rotate(const vector<T, I, S> & v) {
			return apply(rotation(v));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::mirrorX() {
			return apply({
				vector_constants<T>::negativeX,
				vector_constants<T>::positiveY,
				vector_constants<T>::positiveZ
			});
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::mirrorY() {
			return apply({
				vector_constants<T>::positiveX,
				vector_constants<T>::negativeY,
				vector_constants<T>::positiveZ
			});
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> & matrix<T, I, S>::mirrorZ() {
			return apply({
				vector_constants<T>::positiveX,
				vector_constants<T>::positiveY,
				vector_constants<T>::negativeZ
			});
		}
		
		template <class T, class I, class S>
		matrix<T, I, S> & matrix<T, I, S>::translate(const vector<T, I, S> & v) {
			return apply(translation(v));
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::rotationX(T angle) {
			auto v = trigon(angle);
			
			return {
				vector_constants<T>::positiveX,        // 1,  0,  0
				v.template shuffle<3, 1, 2, 3>(),    // 0,  c, -s
				v.template shuffle<3, 0, 1, 3>()        // 0,  s,  c
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::rotationY(T angle) {
			auto v = trigon(angle);
			
			return {
				v.template shuffle<1, 3, 0, 3>(),    // c,  0,  s
				vector_constants<T>::positiveY,        // 0,  1,  0
				v.template shuffle<2, 3, 1, 3>()        //-s,  0,  c
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::rotationZ(T angle) {
			auto v = trigon(angle);
			
			return {
				v.template shuffle<1, 2, 3, 3>(),    // c, -s,  0
				v.template shuffle<0, 1, 3, 3>(),    // s,  c,  0
				vector_constants<T>::positiveZ
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::rotation(const vector<T, I, S> & v, T angle) {
			auto sc = trigon(angle);
			auto cc = sc.template shuffle<1, 1, 1, 3>();
			auto nc = (vector_constants<T>::oneXYZ - cc) * v;
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
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::rotation(const vector<T, I, S> & euler) {
			aligned_vec<T, I> sine, cosine;
			sincos<T, I, S>(euler, sine, cosine);
			
			auto x = sine.template shuffle<0, 0, 0, 0>(cosine).
				template blend<0, 1, 1, 0>(vector_constants<T>::positiveY);                                    // [ sx, 1, 0, cx ]
			auto y = sine.template shuffle<1, 1, 1, 1>(cosine).
				template blend<0, 1, 1, 0>(vector_constants<T>::positiveY);                                    // [ sy, 1, 0, cy ]
			auto z = sine.template shuffle<2, 2, 2, 2>(cosine).
				template blend<0, 1, 1, 0>(vector_constants<T>::positiveY).
				template shuffle<3, 0, 1, 2>();    // [ cz, sz, 1, 0 ]
			
			auto yz = y.template shuffle<0, 0, 3, 2>().negate_z() * z;    // [ sy * cz, sy * sz, -cy, 0 ]
			auto mz = z.template shuffle<1, 0, 2, 3>().negate_x();        // [ -sz, cz, 1, 0 ]
			
			return {
				y.template shuffle<3, 3, 0, 0>() * z,                        //  cy * cz,			   | cy * sz,				| sy,	  | 0
				x.template shuffle<3, 3, 2, 2>() * mz + x.spread_x() * yz,    // -cx * sz + sx * sy * cz | cx * cz + sx * sy * sz |-sx * cy |	0
				x.template shuffle<0, 0, 2, 2>() * mz - x.spread_w() * yz    // -sx * sz - cx * sy * cz | sx * cz - cx * sy * sz | cx * cy | 0
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::scaling(const vector<T, I, S> & s) {
			return {
				s.mask_x(), s.mask_y(), s.mask_z()
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::translation(const vector<T, I, S> & t) {
			return {
				vector_constants<T>::positiveX.template blend<0, 0, 0, 1>(t.spread_x()),
				vector_constants<T>::positiveY.template blend<0, 0, 0, 1>(t.spread_y()),
				vector_constants<T>::positiveZ.template blend<0, 0, 0, 1>(t.spread_z())
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::ortho(T x0, T x1, T y0, T y1, T z0, T z1) {
			const aligned_vec<T, I> max{x1, y1, z1, 1};
			const aligned_vec<T, I> min{x0, y0, z0, 0};
			
			auto d = (max - min).inverse();
			const auto t = -(max + min) * d;
			d *= vector_constants<T>::two;
			
			return {
				d.mask_x().template blend<0, 0, 0, 1>(t.spread_x()),    // 2/w |  0  |  0  | -(x0+x1)/w
				d.mask_y().template blend<0, 0, 0, 1>(t.spread_y()),    //  0  | 2/h |  0  | -(y0+y1)/h
				d.mask_z().template blend<0, 0, 0, 1>(t.spread_z()),    //  0  |  0  | 2/l | -(z0+z1)/l
				vector_constants<T>::positiveW                                //  0  |  0  |  0  |    1
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::orthot(T x0, T x1, T y0, T y1, T z0, T z1) {
			const aligned_vec<T, I> max{x1, y1, z1, 1};
			const aligned_vec<T, I> min{x0, y0, z0, 0};
			
			auto d = (max - min).inverse();
			const auto t = -(max + min) * d;
			d *= vector_constants<T>::two;
			
			return {
				d.mask_x(),                                            //    2/w    |     0     |     0    | 0
				d.mask_y(),                                            //     0     |    2/h    |     0    | 0
				d.mask_z(),                                            //     0     |     0     |    2/l   | 0
				t.template blend<0, 0, 0, 1>(vector_constants<T>::positiveW)    //-(x0+x1)/w |-(y0+y1)/h |-(z0+z1)/l| 1
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::perspective(T fov, T aspect, T z0, T z1) {
			const T f = 1 / std::tan(radians(fov) / 2);
			const T z = z1 / (z1 - z0);
			const vector<T, I, S> v = {f * aspect, f, z, -z0 * z};
			
			return {
				v.mask_x(),                        // f/a | 0 | 0 |  0
				v.mask_y(),                        //  0  | f | 0 |  0
				v.template mask<0, 0, 1, 1>(),    //  0  | 0 | z |-z0*z
				vector_constants<T>::positiveZ            //  0  | 0 | 1 |  0
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::perspectivet(T fov, T aspect, T z0, T z1) {
			const T f = 1 / std::tan(radians(fov) / 2);
			const T z = z1 / (z1 - z0);
			const vector<T, I, S> v = {f * aspect, f, z, -z0 * z};
			
			return {
				v.mask_x(),                                            // f*a | 0 |  0  | 0
				v.mask_y(),                                            //  0  | f |  0  | 0
				v.template blend<1, 1, 0, 1>(vector_constants<T>::positiveW),    //  0  | 0 |  z  | 1
				v.spread_w().mask_z(),                                //  0  | 0 |-z0*z| 0
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::frustum(T x0, T x1, T y0, T y1, T z0, T z1) {
			const T n = 2 * z1;
			const vector<T, I, S> v{1 / (x1 - x0), 1 / (y1 - y0), 1 / (z1 - z0), 0};
			
			return {
				v.spread_x() * vector<T>{n, 0, x1 + x0, 0},
				v.spread_y() * vector<T>{0, n, y1 + y0, 0},
				v.spread_z() * vector<T>{0, 0, z1 + z0, n * z0},
				vector_constants<T>::negativeZ
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::lookAt(const vector<T, I, S> & position, const vector<T, I, S> & center, const vector<T, I, S> & up) {
			auto f = (center - position).normalize();
			auto s = (up.cross(f)).normalize();
			auto u = (f.cross(s)).normalize();
			
			return {
				s.template blend<0, 0, 0, 1>(-s.dot(position)),
				u.template blend<0, 0, 0, 1>(-u.dot(position)),
				f.template blend<0, 0, 0, 1>(-f.dot(position))
			};
		}
		
		template <class T, class I, class S>
		inline matrix<T, I, S> matrix<T, I, S>::lookTo(const vector<T, I, S> & position, const vector<T, I, S> & direction, const vector<T, I, S> & up) {
			auto f = direction;
			auto s = (up.cross(f)).normalize();
			auto u = (f.cross(s)).normalize();
			
			return {
				s.template blend<0, 0, 0, 1>(-s.dot(position)),
				u.template blend<0, 0, 0, 1>(-u.dot(position)),
				f.template blend<0, 0, 0, 1>(-f.dot(position))
			};
		}
	}
	
	template <class Ch, class Traits, class T, class I, class S>
	inline std::basic_ostream<Ch, Traits> & operator << (std::basic_ostream<Ch, Traits> & out, const math::matrix<T, I, S> & mat) {
		return out << "(" <<
			mat(0, 0) << ", " << mat(0, 1) << ", " << mat(0, 2) << ", " << mat(0, 3) << " | " <<
			mat(1, 0) << ", " << mat(1, 1) << ", " << mat(1, 2) << ", " << mat(1, 3) << " | " <<
			mat(2, 0) << ", " << mat(2, 1) << ", " << mat(2, 2) << ", " << mat(2, 3) << " | " <<
			mat(3, 0) << ", " << mat(3, 1) << ", " << mat(3, 2) << ", " << mat(3, 3) <<
			")";
	}
}

//---------------------------------------------------------------------------
#endif

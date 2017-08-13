//---------------------------------------------------------------------------

#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

//---------------------------------------------------------------------------

#include <meta/Inheritance.h>

#include "vector.h"
#include "quaternion.h"
#include "rect.h"

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template<class T>
		struct alignas(sizeof(T) * 4) transform
		{
		public:
			transform() {}
			
			transform(const transform & t) : _matrix(t._matrix) {}
			
			transform(transform && t) : _matrix(move(t._matrix)) {}
			
			transform(const aligned_matrix<T> & m) : _matrix(m) {}
			
			transform(aligned_matrix<T> && m) : _matrix(forward<aligned_matrix<T>>(m)) {}
			
			transform(const vector<T> & p) : _matrix(matrix<T>::translation(p)) {}
			
			transform(const quaternion<T> & r) { r.toMatrix(_matrix); }
			
			transform(const vector<T> & p, const quaternion<T> & r) : transform(r) {
				(*_matrix)
					.translate(p);
			}
			
			transform(const vector<T> & p, const vector<T> & s) {
				(*_matrix)
					.scale(s)
					.translate(p);
			}
			
			transform(const vector<T> & p, const quaternion<T> & r, const vector<T> & s) {
				(*_matrix)
					.scale(s)
					.apply(r.toMatrix())
					.translate(p);
			}
			
			transform(const point<T> & p) {
				(*_matrix)(0, 3) = p.x;
				(*_matrix)(1, 3) = p.y;
			}
			
			transform(const size<T> & s) {
				(*_matrix).scale({s.x, s.y, 1.0f});
			}
			
			transform(const point<T> & p, const size<T> & s) {
				(*_matrix)
					.scale({s.x, s.y, 1.0f})
					.translate({p.x, p.y, 0.0f});
			}
			
			transform(const point<T> & p, const size<T> & s, T depth) {
				(*_matrix)
					.scale({s.x, s.y, 1.0f})
					.translate({p.x, p.y, depth});
			}
			
			transform & operator =(const transform & t) {
				_matrix = t._matrix;
				return *this;
			}
			
			transform & operator =(transform && t) {
				_matrix = move(t._matrix);
				return *this;
			}
			
			transform & operator =(const aligned_matrix<T> & m) {
				_matrix = m;
				return *this;
			}
			
			transform & operator =(aligned_matrix<T> && m) {
				_matrix = forward<aligned_matrix<T>>(m);
				return *this;
			}
			
			matrix<T> output() const {
				return _matrix->transposition();
			}
			
			void apply(const transform & t) {
				*_matrix *= t._matrix;
			}
			
			void applyTo(transform & t) const {
				*t._matrix *= _matrix;
			}
			
			transform & operator *=(const transform & t) & {
				*_matrix *= t._matrix;
				return *this;
			}
			
			transform && operator *=(const transform & t) && {
				*_matrix *= t._matrix;
				return move(*this);
			}
			
			transform & translate(const vector<T> & s) & {
				_matrix->translate(s);
				return *this;
			}
			
			transform && translate(const vector<T> & s) && {
				_matrix->translate(s);
				return move(*this);
			}
			
			transform & rotate(const quaternion<T> & q) & {
				*_matrix *= q.toMatrix();
				return *this;
			}
			
			transform && rotate(const quaternion<T> & q) && {
				*_matrix *= q.toMatrix();
				return move(*this);
			}
			
			transform & scale(const vector<T> & s) & {
				_matrix->scale(s);
				return *this;
			}
			
			transform && scale(const vector<T> & s) && {
				_matrix->scale(s);
				return move(*this);
			}
		
		protected:
			aligned_matrix<T> _matrix;
		};
		
		template<class T>
		transform<T> operator *(const transform<T> & t1, const transform<T> & t2) {
			return *t1._matrix * *t2._matrix;
		}
		
		using float_transform = transform<float>;
		using double_transform = transform<double>;
		
		using ftrans = float_transform;
		using dtrans = double_transform;
	}
}

//---------------------------------------------------------------------------
#endif

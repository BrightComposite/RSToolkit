//---------------------------------------------------------------------------

#pragma once

#ifndef PLANE_H
#define PLANE_H

//---------------------------------------------------------------------------

#include "vector.h"
#include "quaternion.h"

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		enum class plane_side
		{
			back = -1,
			both = 0,
			front = 1
		};
		
		template<class T>
		class plane
		{
		public:
			plane() : equation(vector<T>::positiveZ) {}
			
			plane(const plane & plane) : equation(plane.equation) {}
			
			plane(plane && plane) : equation(move(plane.equation)) {}
			
			plane(const vector<T> & equation) : equation(equation) {}
			
			plane(const vector<T> & normal, T distance) : equation(normal) {
				equation.w = distance;
			}
			
			plane(const vector<T> & a, const vector<T> & b, const vector<T> & c) : equation(cross(b - a, c - a)) {
				equation.normalize();
				equation.w = dot(a, equation);
			}
			
			plane & operator =(const plane & plane) {
				equation = plane.equation;
				return *this;
			}
			
			plane & operator =(plane && plane) {
				equation = move(plane.equation);
				return *this;
			}
			
			vector<T> normal() const {
				return equation.xyz;
			}
			
			T offset() const {
				return equation.w;
			}
			
			T advance(const vector<T> & v) const {
				return dot(v, equation.xyz) + equation.w;
			}
			
			T distance(const vector<T> & v) const {
				return abs(advance(v));
			}
			
			void normalize() {
				auto m = magnitude(equation.xyz);
				
				equation.xyz /= m;
				equation.w *= m;
			}
			
			vector<T> reflect(const vector<T> & position) const {
				auto n = normal();
				return position - vector<T>::two * (dot(position, n) + offset()) * n;
			}
			
			vector<T> mirror(const vector<T> & direction) const {
				auto n = normal();
				return direction - vector<T>::two * dot(direction, n) * n;
			}
			
			void rotate(const quaternion<T> & quat) const {
				equation.xyz = quat.apply_to(equation.xyz);
			}
			
			plane_side classify(const vector<T> & p) const {
				T v = advance(p);
				return (v > constants<T>::eps) ? plane_side::front : (v < -constants<T>::eps) ? plane_side::back : plane_side::both;
			}
			
			vector<T> equation;
		};
		
		using FloatPlane = plane<float>;
		using DoublePlane = plane<double>;
		
		template<class T>
		using aligned_plane = aligned<plane<T>>;
		
		using fplane = aligned_plane<float>;
		using dplane = aligned_plane<double>;
	}
}

//---------------------------------------------------------------------------
#endif

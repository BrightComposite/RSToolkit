//---------------------------------------------------------------------------

#pragma once

#ifndef PLANE_H
#define PLANE_H

//---------------------------------------------------------------------------

#include "Vector.h"
#include "Quaternion.h"

//---------------------------------------------------------------------------

namespace asd
{
	enum class PlaneSide
	{
		Back  = -1,
		Both  =  0,
		Front =  1
	};

	template<class T>
	class Plane
	{
	public:
		Plane() : equation(Vector<T>::positiveZ) {}
		Plane(const Plane & plane) : equation(plane.equation) {}
		Plane(Plane && plane) : equation(move(plane.equation)) {}

		Plane(const Vector<T> & equation) : equation(equation) {}

		Plane(const Vector<T> & normal, T distance) : equation(normal)
		{
			equation.w = distance;
		}

		Plane(const Vector<T> & a, const Vector<T> & b, const Vector<T> & c) : equation(cross(b - a, c - a))
		{
			equation.normalize();
			equation.w = dot(a, equation);
		}

		Plane & operator = (const Plane & plane)
		{
			equation = plane.equation;
		}

		Plane & operator = (Plane && plane)
		{
			equation = move(plane.equation);
		}

		Vector<T> normal() const
		{
			return equation.xyz;
		}

		T offset() const
		{
			return equation.w;
		}

		T advance(const Vector<T> & v) const
		{
			return dot(v, equation.xyz) + equation.w;
		}

		T distance(const Vector<T> & v) const
		{
			return Math<T>::abs(advance(v));
		}

		void normalize()
		{
			auto m = magnitude(equation.xyz);

			equation.xyz /= m;
			equation.w *= m;
		}

		Vector<T> reflect(const Vector<T> & position) const
		{
			auto n = normal();
			return position - Vector<T>::two * (dot(position, n) + offset()) * n;
		}

		Vector<T> mirror(const Vector<T> & direction) const
		{
			auto n = normal();
			return direction - Vector<T>::two * dot(direction, n) * n;
		}

		void rotate(const Quaternion<T> & quat) const
		{
			equation.xyz = quat.applyTo(equation.xyz);
		}

		PlaneSide classify(const Vector<T> & p) const
		{
			T v = advance(p);
			return (v > Math<T>::eps) ? PlaneSide::Front : (v < -Math<T>::eps) ? PlaneSide::Back : PlaneSide::Both;
		}

		Vector<T> equation;
	};

	using FloatPlane = Plane<float>;
	using DoublePlane = Plane<double>;

	template<class T>
	using AlignedPlane = Aligned<Plane<T>>;

	using fplane = AlignedPlane<float>;
	using dplane = AlignedPlane<double>;
}

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------

#pragma once

#ifndef SPATIAL_H
#define SPATIAL_H

//---------------------------------------------------------------------------

#include <math/Quaternion.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	// Here we make a big decision

#if !defined(VECTOR_DOUBLE_PRECISION) || !defined(DOUBLE_VECTOR_IMPLEMENTED)
	using scalar = float;
#else
	using scalar = double;
#endif

	using scalar2 = array<scalar, 2>;
	using scalar3 = array<scalar, 3>;
	using scalar4 = array<scalar, 4>;

	//---------------------------------------------------------------------------

	using math = Math<scalar>;
	using vector = Vector<scalar>;
	using quaternion = Quaternion<scalar>;
	using matrix = Matrix<scalar>;

	//---------------------------------------------------------------------------

	// Just remember, guys... Always store vectors (and another math stuff that use SIMD) ALIGNED!

	using Position = AlignedVector<scalar>;
	using Direction = AlignedVector<scalar>;
	using Rotation = AlignedQuaternion<scalar>;

	//---------------------------------------------------------------------------

	class Spatial
	{
		deny_copy(Spatial);

	public:
		Spatial() {}
		virtual ~Spatial() {}

		virtual vector position() const
		{
			return vector::identity;
		}

		virtual vector direction() const
		{
			return vector::forward;
		}

		virtual quaternion rotation() const
		{
			return quaternion::identity;
		}

		virtual void setPosition(const vector & pos) {}
		virtual void setDirection(const vector & rot) {}
		virtual void setRotation(const quaternion & rot) {}

		virtual void move(const vector & offset) {}
		virtual void rotate(const quaternion & rot) {}

		void move(float x, float y, float z)
		{
			move({x, y, z});
		}

		void moveX(float x)
		{
			move({x, 0, 0});
		}

		void moveY(float y)
		{
			move({0, y, 0});
		}

		void moveZ(float z)
		{
			move({0, 0, z});
		}

		void rotate(const vector & axis, float angle)
		{
			rotate({axis, angle});
		}

		void rotateX(float angle)
		{
			rotate(vector::positiveX, angle);
		}

		void rotateY(float angle)
		{
			rotate(vector::positiveY, angle);
		}

		void rotateZ(float angle)
		{
			rotate(vector::positiveZ, angle);
		}
	};

	class Positioned : public Spatial
	{
		deny_copy(Positioned);

	public:
		Positioned(const Position & pos = vector::identity) : _pos(pos) {}
		virtual ~Positioned() {}

		virtual vector position() const override
		{
			return _pos;
		}

		virtual void setPosition(const vector & pos) override
		{
			_pos = pos;
		}

		virtual void move(const vector & offset) override
		{
			*_pos += offset;
		}

	protected:
		Position _pos;
	};

	class Oriented : public Positioned
	{
		deny_copy(Oriented);

	public:
		Oriented(const Position & pos = vector::identity, const Rotation & rot = vector::identity) : Positioned(pos), _rot(rot) {}
		virtual ~Oriented() {}

		virtual vector direction() const override
		{
			return _rot->forward();
		}

		virtual quaternion rotation() const override
		{
			return _rot;
		}

		virtual void setDirection(const vector & dir) override
		{
			setRotation({vector::forward, dir});
		}

		virtual void setRotation(const quaternion & rot) override
		{
			_rot = rot;
		}

		virtual void move(const vector & offset) override
		{
			*_pos += _rot->applyTo(offset);
		}

		virtual void rotate(const quaternion & rot) override
		{
			_rot->rotateBy(rot);
		}

	protected:
		Rotation _rot {};
	};

#ifdef USE_STUPID_JOKES
	class ObjectOriented : public Oriented {};
#endif

	//---------------------------------------------------------------------------
	
#pragma warning(push)
#pragma warning(disable: 4455)

	inline constexpr scalar operator ""x (long double value)
	{
		return scalar(value);
	}

#pragma warning(push)
}

//---------------------------------------------------------------------------
#endif

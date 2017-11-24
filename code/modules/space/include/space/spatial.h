//---------------------------------------------------------------------------

#pragma once

#ifndef SPATIAL_H
#define SPATIAL_H

//---------------------------------------------------------------------------

#include <math/transform.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace space
	{
		/// Here we make the big decision

#if !defined(VECTOR_DOUBLE_PRECISION) || !defined(DOUBLE_VECTOR_IMPLEMENTED)
		using scalar = float;
#else
		using scalar = double;
#endif
		
		using scalar2 = array<scalar, 2>;
		using scalar3 = array<scalar, 3>;
		using scalar4 = array<scalar, 4>;
		
		//---------------------------------------------------------------------------
		
		using vector = math::vector<scalar>;
		using quaternion = math::quaternion<scalar>;
		using matrix = math::matrix<scalar>;
		using transform = math::transform<scalar>;
		
		//---------------------------------------------------------------------------
		
		using position = vector;
		using direction = vector;
		using rotation = quaternion;
		
		//---------------------------------------------------------------------------
		
		/// Just remember, guys... Always store vectors (and another math stuff that use SIMD) ALIGNED!
		
		class spatial : public aligned_alloc
		{
			deny_copy(spatial);
		
		public:
			spatial() {}
			
			virtual ~spatial() {}
			
			virtual vector position() const {
				return vector::identity;
			}
			
			virtual vector direction() const {
				return vector::forward;
			}
			
			virtual quaternion rotation() const {
				return quaternion::identity;
			}
			
			virtual void set_position(const vector & pos) {}
			
			virtual void set_direction(const vector & rot) {}
			
			virtual void set_rotation(const quaternion & rot) {}
			
			virtual void move(const vector & offset) {}
			
			virtual void rotate(const quaternion & rot) {}
			
			void move(float x, float y, float z) {
				move({x, y, z});
			}
			
			void move_x(float x) {
				move({x, 0, 0});
			}
			
			void move_y(float y) {
				move({0, y, 0});
			}
			
			void move_z(float z) {
				move({0, 0, z});
			}
			
			void rotate(const vector & axis, float angle) {
				rotate({axis, angle});
			}
			
			void rotate_x(float angle) {
				rotate(vector::positiveX, angle);
			}
			
			void rotate_y(float angle) {
				rotate(vector::positiveY, angle);
			}
			
			void rotate_z(float angle) {
				rotate(vector::positiveZ, angle);
			}
		};
		
		class positioned : public spatial
		{
			deny_copy(positioned);
		
		public:
			positioned(const space::position & pos = vector::identity) : _pos(pos) {}
			
			virtual ~positioned() {}
			
			forceinline virtual vector position() const override {
				return _pos;
			}
			
			forceinline virtual void set_position(const vector & pos) override {
				_pos = pos;
			}
			
			forceinline virtual void move(const vector & offset) override {
				_pos += offset;
			}
		
		protected:
			space::position _pos;
		};
		
		class oriented : public positioned
		{
			deny_copy(oriented);
		
		public:
			oriented(const space::position & pos = vector::identity, const space::rotation & rot = quaternion::identity) : positioned(pos), _rot(rot) {}
			
			virtual ~oriented() {}
			
			virtual vector direction() const override {
				return _rot.forward();
			}
			
			virtual quaternion rotation() const override {
				return _rot;
			}
			
			virtual void set_direction(const vector & dir) override {
				set_rotation({vector::forward, dir});
			}
			
			virtual void set_rotation(const quaternion & rot) override {
				_rot = rot;
			}
			
			virtual void move(const vector & offset) override {
				_pos += _rot.apply_to(offset);
			}
			
			virtual void rotate(const quaternion & rot) override {
				_rot.rotate_by(rot);
			}
		
		protected:
			space::rotation _rot;
		};

#ifdef USE_STUPID_JOKES
		class object_oriented : public oriented {};
#endif
		
		//---------------------------------------------------------------------------
		
		/// PLEASE PLEASE PLEASE

#ifdef MSC_VER
#pragma warning(push)
#pragma warning(disable: 4455)
#endif
		
		inline constexpr scalar operator ""x(long double value) {
			return scalar(value);
		}

#ifdef MSC_VER
#pragma warning(push)
#endif
	}
}

//---------------------------------------------------------------------------
#endif

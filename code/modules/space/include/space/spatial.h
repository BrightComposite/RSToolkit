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

        using vector_constants = math::vector_constants<scalar>;
        
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
                return vector_constants::identity;
            }
            
            virtual vector direction() const {
                return vector_constants::forward;
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
                rotate(vector_constants::positive_x, angle);
            }
            
            void rotate_y(float angle) {
                rotate(vector_constants::positive_y, angle);
            }
            
            void rotate_z(float angle) {
                rotate(vector_constants::positive_z, angle);
            }
        };
        
        class positioned : public spatial
        {
        public:
            positioned(const space::position & pos = vector_constants::identity) : _position(pos) {}
            
            positioned(positioned && p) noexcept : _position(std::move(p._position)) {}

            virtual ~positioned() {}

            positioned & operator = (positioned && p) noexcept {
                std::swap(_position, p._position);
                return *this;
            }
            
            virtual vector position() const override {
                return _position;
            }
            
            virtual void set_position(const vector & pos) override {
                _position = pos;
            }
            
            virtual void move(const vector & offset) override {
                _position += offset;
            }
        
        protected:
            space::position _position;
        };
        
        class oriented : public positioned
        {
        public:
            oriented(const space::position & pos = vector_constants::identity, const space::rotation & rot = quaternion::identity) : positioned(pos), _rotation(rot) {}
            
            oriented(oriented && o) noexcept : positioned(std::forward<oriented>(o)), _rotation(std::move(o._rotation)) {}

            virtual ~oriented() {}

            oriented & operator = (oriented && o) noexcept {
                positioned::operator = (std::forward<oriented>(o));
                std::swap(_rotation, o._rotation);
                return *this;
            }

            virtual vector direction() const override {
                return _rotation.forward();
            }
            
            virtual quaternion rotation() const override {
                return _rotation;
            }
            
            virtual void set_direction(const vector & dir) override {
                set_rotation({ vector_constants::forward, dir });
            }
            
            virtual void set_rotation(const quaternion & rot) override {
                _rotation = rot;
            }
            
            virtual void move(const vector & offset) override {
                _position += _rotation.apply_to(offset);
            }
            
            virtual void rotate(const quaternion & rot) override {
                _rotation.rotate_by(rot);
            }
        
        protected:
            space::rotation _rotation;
        };

#ifdef ENABLE_STUPID_JOKES
        class object_oriented : public oriented {};
#endif
        
        //---------------------------------------------------------------------------
        
        inline constexpr scalar operator ""_x(long double value) {
            return scalar(value);
        }
    }
}

//---------------------------------------------------------------------------
#endif

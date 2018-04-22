//---------------------------------------------------------------------------

#pragma once

#ifndef CAMERA_H
#define CAMERA_H

//---------------------------------------------------------------------------

#include "object.h"

//---------------------------------------------------------------------------

namespace asd
{
    namespace scene
    {
        class container;

        enum class projection
        {
            ortho,
            perspective
        };

        class camera : public space::oriented, public scene::object
        {
        public:
            api(scene)
            camera(scene::container & scene);

            api(scene)
            camera(scene::camera && camera) noexcept;

            api(scene)
            camera & operator = (camera &&) noexcept;

            api(scene)
            virtual void set_rotation(const quaternion & rot) override;

            api(scene)
            virtual void rotate(const quaternion & rot) override;

            api(scene)
            virtual void move(const vector & offset) override;

            scalar pitch() const {
                return _angles[0];
            }

            scalar yaw() const {
                return _angles[1];
            }

            scalar roll() const {
                return _angles[2];
            }

            api(scene)
            virtual void setPitch(scalar value);

            api(scene)
            virtual void setYaw(scalar value);

            api(scene)
            virtual void setRoll(scalar value);

            api(scene)
            virtual void setAngles(scalar pitch, scalar yaw, scalar roll);

            api(scene)
            virtual void addPitch(scalar value);

            api(scene)
            virtual void addYaw(scalar value);

            api(scene)
            virtual void addRoll(scalar value);

            api(scene)
            virtual void addAngles(scalar pitch, scalar yaw, scalar roll);

            scalar view_range() const {
                return _range;
            }

            scalar field_of_view() const {
                return _fov;
            }

            const matrix & projection_matrix() const {
                return _projection_matrix;
            }

            const matrix & view_matrix() const {
                return _view_matrix;
            }

            matrix normal_matrix(const matrix & model) const {
                return (_view_matrix * model).invert();
            }

            scene::projection projection() const {
                return _projection;
            }

            api(scene) void set_projection(scene::projection mode);

            api(scene) void set_view_range(scalar range);
            api(scene) void set_field_of_view(scalar fov);

            api(scene) void update_projection();
            api(scene) void update_view();

        protected:
            api(scene)
            camera(scene::container & scene, uniform::component & uniforms);

            api(scene) void update_normal_matrix();

            scalar _range = 0.01f;
            scalar _fov = 90.0f;

            space::rotation _pitch;
            space::rotation _yaw;
            space::rotation _roll;

            scalar3  _angles = { { 0.0_x, 0.0_x, 0.0_x } };
            scene::projection _projection = projection::ortho;
            space::matrix _projection_matrix;
            space::matrix _view_matrix;

            uniform::block _projection_data;
            uniform::block _view_data;
        };
    }
}

//---------------------------------------------------------------------------
#endif

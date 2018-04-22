//---------------------------------------------------------------------------

#include <scene/camera.h>
#include <scene/scene.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace scene
    {
        camera::camera(scene::container & scene) : camera(scene, get<uniform::component>(scene.graphics()))  {
            if (scene.camera() == boost::none) {
                scene.set_camera(*this);
            }

            update_projection();
        }
            
        camera::camera(scene::container & scene, uniform::component & uniforms) :
            scene::object(scene),
            _projection_data(uniforms.create_block("Projection")),
            _view_data(uniforms.create_block("View"))
        {}

        camera::camera(scene::camera && camera)noexcept :
            space::oriented(std::forward<scene::camera>(camera)),
            scene::object(std::forward<scene::camera>(camera)),

            _range(camera._range),
            _fov(camera._fov),

            _pitch(std::move(camera._pitch)),
            _yaw(std::move(camera._yaw)),
            _roll(std::move(camera._roll)),

            _angles(std::move(camera._angles)),
            _projection(camera._projection),
            _projection_matrix(std::move(camera._projection_matrix)),
            _view_matrix(std::move(camera._view_matrix)),

            _projection_data(std::move(camera._projection_data)),
            _view_data(std::move(camera._view_data))
        {}

        scene::camera & camera::operator = (scene::camera && camera) noexcept {
            space::oriented::operator = (std::forward<scene::camera>(camera)),
            scene::object::operator = (std::forward<scene::camera>(camera)),

            std::swap(_range, camera._range),
            std::swap(_fov, camera._fov);

            std::swap(_pitch, camera._pitch);
            std::swap(_yaw, camera._yaw);
            std::swap(_roll, camera._roll);

            std::swap(_angles, camera._angles);
            std::swap(_projection, camera._projection);
            std::swap(_projection_matrix, camera._projection_matrix);
            std::swap(_view_matrix, camera._view_matrix);

            std::swap(_projection_data, camera._projection_data);
            std::swap(_view_data, camera._view_data);

            return *this;
        }

        void camera::set_projection(scene::projection mode) {
            if (_projection == mode)
                return;

            _projection = mode;
            update_projection();
        }

        void camera::update_projection() {
            auto aspect = static_cast<scalar>(_scene.viewport().ratio());

            switch (_projection) {
            case projection::ortho:
            {
                _projection_matrix = matrix::ortho(-1.0_x / aspect, 1.0_x / aspect, -1.0_x, 1.0_x, -_range, _range);
                break;
            }

            case projection::perspective:
            {
                _projection_matrix = matrix::perspective(_fov, aspect, 0.01_x, 2 * _range);
                break;
            }
            }

            _projection_data.set<true>(0, _projection_matrix);
        }

        void camera::set_view_range(scalar range) {
            _range = range;
        }

        void camera::set_field_of_view(scalar fov) {
            _fov = fov;
        }

        void camera::set_rotation(const quaternion & rot) {
            _rotation = rot;
        }

        void camera::rotate(const quaternion & rot) {
            _rotation.rotate_by(rot);
        }

        void camera::move(const vector & offset) {
            _position += _yaw.apply_to(offset);
        }

        void camera::setPitch(scalar value) {
            _angles[0] = math::clamp(value, -math::constants<float>::half_pi, math::constants<float>::half_pi);
            _pitch = { vector_constants::right, _angles[0] };

            _rotation = _roll * _yaw * _pitch;
        }

        void camera::setYaw(scalar value) {
            _angles[1] = math::normalize_angle(value);
            _yaw = { vector_constants::up, _angles[1] };

            _rotation = _roll * _yaw * _pitch;
        }

        void camera::setRoll(scalar value) {
            _angles[2] = value;
            _roll = { vector_constants::forward, _angles[2] };

            _rotation = _roll * _yaw * _pitch;
        }

        void camera::setAngles(scalar pitch, scalar yaw, scalar roll) {
            _angles[0] = math::clamp(pitch, -math::constants<float>::pi * 0.49_x, math::constants<float>::pi * 0.49_x);
            _angles[1] = math::normalize_angle(yaw);
            _angles[2] = roll;

            _pitch = { vector_constants::right, _angles[0] };
            _yaw = { vector_constants::up, _angles[1] };
            _roll = { vector_constants::forward, _angles[2] };

            _rotation = _roll * _yaw * _pitch;
        }

        void camera::addPitch(scalar value) {
            setPitch(_angles[0] + value);
        }

        void camera::addYaw(scalar value) {
            setYaw(_angles[1] + value);
        }

        void camera::addRoll(scalar value) {
            setRoll(_angles[2] + value);
        }

        void camera::addAngles(scalar pitch, scalar yaw, scalar roll) {
            setAngles(_angles[0] + pitch, _angles[1] + yaw, _angles[2] + roll);
        }

        void camera::update_view() {
            _view_matrix = matrix::look_to(_position, _rotation.forward(), _rotation.up());
            _view_data.set<true>(0, _view_matrix);
        }
    }
}

//---------------------------------------------------------------------------

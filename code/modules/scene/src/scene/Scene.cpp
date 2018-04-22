//---------------------------------------------------------------------------

#include <scene/scene.h>
#include <scene/camera.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace scene
    {
        drawable::drawable(container & scene, bool transparent) : _transparent(transparent) {}

        container::container(gfx::context & gfx, flow::context & flow) : _gfx(gfx), _timer(flow, 100ms) {
            _timer.bind(make_method(this, update));
            _timer.start();

            auto & uniforms = *gfx.component<uniform::component>();
            
            _uniforms << uniforms.register_uniform("Color", { uniform::scheme::create<uniform::f32v4>("color") });
            _uniforms << uniforms.register_uniform("Model", { uniform::scheme::create<uniform::f32m4>("model") });
            _uniforms << uniforms.register_uniform("View", { uniform::scheme::create<uniform::f32m4>("view") });
            _uniforms << uniforms.register_uniform("Projection", { uniform::scheme::create<uniform::f32m4>("projection") });
        }

        container::container(container && scene) noexcept :
            _gfx(scene._gfx),
            _camera(std::move(scene._camera)),
            _objects(std::move(scene._objects)),
            _opaque(std::move(scene._opaque)),
            _transparent(std::move(scene._transparent)),
            _timer(std::move(scene._timer)),
            _uniforms(std::move(scene._uniforms))
        {}

        container & container::operator = (container && scene) noexcept {
            std::swap(_camera, scene._camera);
            std::swap(_objects, scene._objects);
            std::swap(_opaque, scene._opaque);
            std::swap(_transparent, scene._transparent);
            std::swap(_timer, scene._timer);
            std::swap(_uniforms, scene._uniforms);

            return *this;
        }

        container::~container() {}

        gfx::context & container::graphics() const {
            return _gfx;
        }

        const boost::optional<scene::camera &> & container::camera() const {
            return _camera;
        }

        const math::uint_size & container::viewport() const {
            return _viewport;
        }

        void container::set_camera(const boost::optional<scene::camera &> & camera) {
            _camera = camera;
        }

        void container::set_viewport(const math::uint_size & viewport) {
            _viewport = viewport;
        }

        void container::draw(const math::int_rect & viewport) const {
            for (auto & uniform : _uniforms) {
                uniform.get().update();
            }

            for (auto & drawable : _opaque) {
                drawable.get().draw(viewport, 1.0f);
            }

            for (auto & drawable : _transparent) {
                drawable.get().draw(viewport, 1.0f);
            }
        }

        matrix container::normal_matrix(const matrix & model) const {
            return /*_camera != nullptr ? _camera->normal_matrix(model) : */model.inverse();
        }

        void container::update(ticks_t ticks) {
            for (auto & obj : _objects) {
                obj->update(ticks);
            }
        }
    }
 }
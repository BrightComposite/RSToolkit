//---------------------------------------------------------------------------

#pragma once

#ifndef SCENE_H
#define SCENE_H

//---------------------------------------------------------------------------

#include <chrono>

#include "camera.h"

//---------------------------------------------------------------------------

namespace asd
{
    namespace scene
    {
        using namespace std::chrono_literals;

        class drawable;
        class container;

        class component
        {
            origin_class(component);

        public:
            component(scene::object * object) : _object(object) {}

            scene::object * object() {
                return _object;
            }

            const scene::object * object() const {
                return _object;
            }

        protected:
            scene::object * _object;
        };

        class drawable
        {
            deny_copy(drawable);

        public:
            api(scene)
            drawable(container & scene, bool transparent = false);
            virtual ~drawable() {}

            bool transparent() const {
                return _transparent;
            }

            virtual void draw(const math::int_rect & viewport, float zoom) const = 0;

        protected:
            bool _transparent = false;
        };

        class container : private boost::noncopyable
        {
        public:
            api(scene)
            container(gfx::context & gfx, flow::context & flow);

            api(scene)
            container(container &&) noexcept;

            api(scene)
            virtual ~container();

            container & operator = (container &&) noexcept;

            api(scene)
            gfx::context & graphics() const;

            api(scene)
            const boost::optional<scene::camera &> & camera() const;

            api(scene)
            const math::uint_size & viewport() const;

            api(scene)
            void set_camera(const boost::optional<scene::camera &> & camera);

            api(scene)
            void set_viewport(const math::uint_size & viewport);

            template<class Obj, class ... A, useif<based_on<Obj, object>::value, can_construct<Obj, container &, A...>::value>>
            Obj & append(A &&... args) {
                return static_cast<Obj &>(**_objects.emplace(_objects.end(), make::unique<Obj>(*this, forward<A>(args)...)));
            }

            api(scene)
            matrix normal_matrix(const matrix & model) const;

        protected:
            api(scene)
            virtual void draw(const math::int_rect & viewport) const;

            api(scene)
            virtual void update(ticks_t ticks);

            gfx::context & _gfx;
            boost::optional<scene::camera &> _camera = boost::none;
            math::uint_size _viewport;

            array_list<unique<object>> _objects;
            array_list<std::reference_wrapper<drawable>> _opaque;
            array_list<std::reference_wrapper<drawable>> _transparent;
            flow::tick_timer _timer;
            array_list<std::reference_wrapper<uniform::object>> _uniforms;
        };

        class provider
        {
            map<string, container> _scenes;
        };
    }
}

//---------------------------------------------------------------------------
#endif

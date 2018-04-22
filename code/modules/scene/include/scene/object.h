//---------------------------------------------------------------------------

#pragma once

#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

//---------------------------------------------------------------------------

#include <graphics/graphics.h>
#include <graphics3d/uniform.h>
#include <space/spatial.h>

#include <flow/timer.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace scene
    {
        class container;

        namespace uniform = gfx3d::uniform;

        using namespace space;

        using flow::ticks_t;

        class object : private boost::noncopyable
        {
        public:
            object(scene::container & scene) : _scene(scene) {}

            object(scene::object && object) noexcept : _scene(object._scene) {}

            virtual ~object() {}

            object & operator = (scene::object && object) noexcept {
                return *this;
            }

            container & scene() const {
                return _scene;
            }

            virtual void update(ticks_t ticks) {}

        protected:
            container & _scene;
        };
    }
}

//---------------------------------------------------------------------------
#endif

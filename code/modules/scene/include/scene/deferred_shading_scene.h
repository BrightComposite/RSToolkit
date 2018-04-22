//---------------------------------------------------------------------------

#pragma once

#ifndef DEFERRED_SHADING_SCENE_H
#define DEFERRED_SHADING_SCENE_H

//---------------------------------------------------------------------------

#include "scene.h"

//---------------------------------------------------------------------------

namespace asd
{
    namespace scene
    {
        struct Light
        {
            space::vector position = { 0.0f, 0.0f, 0.0f };
            gfx::colorf color = { 1.0f, 1.0f, 1.0f };

            struct Attenuation
            {
                float constant;
                float linear;
                float exponential;
            } attenuation = { 0.0f, 0.0f, 0.0f };

            float ambientFactor = { 1.0f };

            api(scene)
            void apply(gfx::context & graphics) const;
        };
/*
        class DeferredShadingScene : public container
        {
            friend class scene;

        public:
            virtual ~DeferredShadingScene() {}

            api(scene)
            Light & addLight();
            api(scene)
            void setGlobalLightColor(const gfx::colorf & color);

        protected:
            friend_owned_handle(DeferredShadingScene, DeferredShadingScene);

            virtual api(scene) void draw(Graphics3D & graphics, const IntRect & viewport) const override;
            virtual api(scene) void onWidgetResize(handle<WidgetResizeMessage> & msg, Widget & w) override;

            handle<TextureSurface> _surface = nullptr;
            handle<Texture> _positions = nullptr;
            handle<Texture> _normals = nullptr;
            handle<Texture> _colors = nullptr;

            ArrayList<Light> _lights;
            colorf _globalLightColor = { 0.0f, 0.0f, 0.0f };

            struct
            {
                handle<FxTechnique> global = nullptr;
                handle<FxTechnique> point = nullptr;
            } _lightTech;
        };*/
    }
}

//---------------------------------------------------------------------------
#endif

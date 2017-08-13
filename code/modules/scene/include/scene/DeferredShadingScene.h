//---------------------------------------------------------------------------

#pragma once

#ifndef DEFERRED_SHADING_SCENE_H
#define DEFERRED_SHADING_SCENE_H

//---------------------------------------------------------------------------

#include "Scene.h"

//---------------------------------------------------------------------------

namespace asd
{
	struct Light : Shared
	{
		fvec position = {0.0f, 0.0f, 0.0f};
		colorf color = {1.0f, 1.0f, 1.0f};

		struct Attenuation
		{
			float constant;
			float linear;
			float exponential;
		} attenuation = {0.0f, 0.0f, 0.0f};

		float ambientFactor = {1.0f};

		api(scene) void apply(Graphics3D & graphics) const;
	};

	class DeferredShadingScene : public Scene
	{
		friend class Scene;

		using OwnHandle = handle<DeferredShadingScene, DeferredShadingScene>;

	public:
		virtual ~DeferredShadingScene() {}

		virtual handle<Scene> clone(Widget * w) const override
		{
			return OwnHandle(w);
		}

		api(scene) handle<Light> addLight();
		api(scene) void setGlobalLightColor(const colorf & color);

		static handle<DeferredShadingScene> create(Widget * widget)
		{
			OwnHandle h(widget);
			Scene::construct(h);

			return h;
		}

	protected:
		friend_owned_handle(DeferredShadingScene, DeferredShadingScene);

		api(scene) DeferredShadingScene(Widget * widget);

		virtual api(scene) void draw(Graphics3D & graphics, const IntRect & viewport) const override;
		virtual api(scene) void onWidgetResize(handle<WidgetResizeMessage> & msg, Widget & w) override;

		handle<TextureSurface> _surface = nullptr;
		handle<Texture> _positions = nullptr;
		handle<Texture> _normals = nullptr;
		handle<Texture> _colors = nullptr;

		ArrayList<Light> _lights;
		colorf _globalLightColor = {0.0f, 0.0f, 0.0f};

		struct
		{
			handle<FxTechnique> global = nullptr;
			handle<FxTechnique> point = nullptr;
		} _lightTech;
	};
}

//---------------------------------------------------------------------------
#endif
